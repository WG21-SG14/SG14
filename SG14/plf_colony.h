// Copyright (c) 2015, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef PLF_COLONY_H
#define PLF_COLONY_H


#include <cstring>	// memset, memcpy
#include <cassert>	// assert
#include <memory>	// std::uninitialized_copy, std::allocator
#include <climits>	// UINT_MAX
#include <iterator> // typedef inheritance for iterators

#include "plf_stack.h" // Includes plf::stack and compiler-specific Macro defines


#ifdef PLF_TYPE_TRAITS_SUPPORT
    #include <type_traits> // std::is_trivially_destructible
#endif

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif


namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class colony : private element_allocator_type
{
public:
	// Standard container typedefs:
	typedef element_type										value_type;
	typedef element_allocator_type								allocator_type;
	typedef typename element_allocator_type::reference			reference;
	typedef typename element_allocator_type::const_reference	const_reference;
	typedef unsigned int										difference_type;
	typedef unsigned int										size_type;
	typedef typename element_allocator_type::pointer			pointer;
	typedef typename element_allocator_type::const_pointer		const_pointer;

	template <class colony_element_type, bool is_const> class colony_iterator;
	typedef colony_iterator<element_allocator_type, false> iterator; 
	typedef colony_iterator<element_allocator_type, true> const_iterator;
	friend iterator;
	friend const_iterator;

	template <class colony_element_type, bool is_const> class colony_reverse_iterator;
	typedef colony_reverse_iterator<element_allocator_type, false> reverse_iterator; 
	typedef colony_reverse_iterator<element_allocator_type, true> const_reverse_iterator;
	friend reverse_iterator;
	friend const_reverse_iterator;

private:
	struct group; // forward declaration for typedefs below
	typedef typename element_allocator_type::template rebind<group>::other				group_allocator_type;
	typedef typename element_allocator_type::template rebind<bool>::other				bool_allocator_type;
	typedef typename element_allocator_type::template rebind<unsigned char>::other		uchar_allocator_type;
	typedef typename element_allocator_type::template rebind<iterator>::other			iterator_allocator_type;
	typedef typename plf::stack<iterator, iterator_allocator_type> iterator_stack_type;
	typedef typename element_allocator_type::pointer 	element_pointer_type;
	typedef typename group_allocator_type::pointer 		group_pointer_type;
	typedef typename group_allocator_type::reference 	group_reference_type;
	typedef typename bool_allocator_type::pointer 		bool_pointer_type;
	typedef typename uchar_allocator_type::pointer		uchar_pointer_type;
	typedef typename iterator_allocator_type::pointer 	iterator_pointer_type;


	struct group : private uchar_allocator_type
	{
		element_pointer_type		last_endpoint; // the address that is one past the highest cell number that's been used so far in this group - does not change with erase command
		group_pointer_type			next_group;
		const element_pointer_type	elements;
		const bool_pointer_type		erased; // Now that both the elements and erased arrays are allocated contiguously, erased pointer also functions as a 'one-past-end' pointer for the elements array
		group_pointer_type			previous_group;
		unsigned int				total_number_of_elements; // indicates total number of used cells - changes with insert and erase commands - used in multi-increment/decrement functions
		const unsigned int			group_number; // Used for comparison (> < >= <=) iterator operators and (in debug mode) checking order in multi-erase and multi-insert commands
		const unsigned int			size; // The number of elements allocated for this particular group


		#ifdef PLF_VARIADICS_SUPPORT
			group(const unsigned int elements_per_group, group * const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_ALLOCATE_INITIALIZATION(uchar_allocator_type, elements_per_group * (sizeof(element_type) + sizeof(bool)), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL),
				elements(last_endpoint),
				erased(reinterpret_cast<bool_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				total_number_of_elements(0),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				size(elements_per_group)
			{
				std::memset(&*erased, false, sizeof(bool) * size); // &* to avoid problems with non-trivial pointers
			}


			~group() PLF_NOEXCEPT
			{
				PLF_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), size * (sizeof(element_type) + sizeof(bool)));
			}

		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++0x and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor:
			group(const unsigned int elements_per_group, group * const previous = NULL) PLF_NOEXCEPT:
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_ALLOCATE_INITIALIZATION(uchar_allocator_type, elements_per_group * (sizeof(element_type) + sizeof(bool)), (previous == NULL) ? 0 : previous->elements))),
				elements(NULL), // unique destructor condition
				erased(reinterpret_cast<bool_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				size(elements_per_group)
			{
				std::memset(&*erased, false, sizeof(bool) * size);
			}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++0x for reasons stated above.
			group(const group &source):
				last_endpoint(source.last_endpoint), 
				next_group(NULL),
				elements(source.last_endpoint),
				erased(source.erased),
				previous_group(source.previous_group),
				total_number_of_elements(0),
				group_number(source.group_number),
				size(source.size)
			{}


			~group() PLF_NOEXCEPT
			{
				if (elements != NULL) // NULL check necessary for correct destruction of pseudoconstructed groups as per above
				{
					PLF_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), size * (sizeof(element_type) + sizeof(bool)));
				}
			}
		#endif

	};


	// Implement const/non-const iterator switching pattern:
	template <bool flag, class IsTrue, class IsFalse> struct choose;

	template <class IsTrue, class IsFalse> struct choose<true, IsTrue, IsFalse>
	{
	   typedef IsTrue type;
	};

	template <class IsTrue, class IsFalse> struct choose<false, IsTrue, IsFalse>
	{
	   typedef IsFalse type;
	};


public:

	template <class colony_allocator_type, bool is_const> class colony_iterator : public std::iterator<std::random_access_iterator_tag, element_type>
	{
	private:
		group_pointer_type		group_pointer;
		element_pointer_type	element_pointer;
		bool_pointer_type		erasure_field;

	public:
		typedef typename choose<is_const, typename colony_allocator_type::const_reference, typename colony_allocator_type::reference>::type	reference;
		typedef typename choose<is_const, typename colony_allocator_type::const_pointer, typename colony_allocator_type::pointer>::type		pointer;
		
		friend class colony;
		friend reverse_iterator;


		inline colony_iterator & operator = (const colony_iterator &source) PLF_NOEXCEPT
		{
			assert (&source != this);

			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			erasure_field = source.erasure_field;

			return *this;
		}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// Move assignment
			inline colony_iterator & operator = (const colony_iterator &&source) PLF_NOEXCEPT // Move is a copy in this scenario
			{
				assert (&source != this);

				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				erasure_field = std::move(source.erasure_field);

				return *this;
			}
		#endif



		inline bool operator == (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}



		inline bool operator != (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}



		inline reference operator * () const PLF_NOEXCEPT
		{
			return *element_pointer;
		}



		inline pointer operator -> () const PLF_NOEXCEPT
		{
			return element_pointer;
		}



		colony_iterator & operator ++ ()
		{
			assert(group_pointer != NULL); // covers uninitialised colony_iterator

			#if defined(__GNUC__) && defined(__x86_64__) // For some reason, this version compiles to faster code under gcc x64
				do
				{
					if (group_pointer->next_group == NULL)
					{
						if (++element_pointer == group_pointer->last_endpoint) // ie. beyond end of available data
						{
							return *this;
						}
					}
					else if (++element_pointer == group_pointer->last_endpoint)
					{
						group_pointer = group_pointer->next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased - 1;
					}
				} while (*++erasure_field);
			#else // Standard version - performs better on most compilers
				do
				{
					++erasure_field;

					if (++element_pointer == group_pointer->last_endpoint) // ie. beyond end of available data
					{
						if (group_pointer->next_group == NULL)
						{
							return *this;
						}

						group_pointer = group_pointer->next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased;
					}
				} while (*erasure_field);
			#endif
			return *this;
		}



		inline colony_iterator operator ++(int)
		{
			colony_iterator copy(*this);
			++*this;
			return copy;
		}



		colony_iterator & operator -- ()
		{
			assert(group_pointer != NULL);

			do
			{
				if (element_pointer != group_pointer->elements) // ie. beginning of available data (ie. [0]) not reached
				{
					--element_pointer; // Erasure field decrement covered by loop condition
				}
				else if (group_pointer->previous_group != NULL)
				{
					group_pointer = group_pointer->previous_group;
					element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased) - 1; // Erased array is now allocated directly after the elements array, making it the "end" pointer for group_pointer->elements
					erasure_field = group_pointer->erased + group_pointer->size; // -1 Covered by loop condition
				}
				else
				{
					return *this;
				}
			} while (*--erasure_field);

			return *this;
		}



		inline colony_iterator operator -- (int)
		{
			colony_iterator copy(*this);
			--*this;
			return copy;
		}
		


		colony_iterator & operator += (unsigned int addition)
		{
			assert(group_pointer != NULL); // covers uninitialised colony_iterator && empty group

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->elements)
			{
				const group &current_group = *group_pointer;

				if (current_group.total_number_of_elements == static_cast<const unsigned int>(current_group.last_endpoint - current_group.elements))
				{
					const unsigned int distance_from_end = static_cast<const unsigned int>(current_group.last_endpoint - element_pointer);

					if(addition < distance_from_end)
					{
						element_pointer += addition;
						erasure_field += addition;
						return *this;
					}
					else if (current_group.next_group == NULL)
					{
						element_pointer = current_group.last_endpoint;
						erasure_field += distance_from_end;
						return *this;
					}
					else
					{
						group_pointer = current_group.next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased;
						addition -= distance_from_end - 1;

						if (!*(erasure_field) && addition-- == 1)
						{
							return *this;
						}
					}
				}
				else
				{
					while(true)
					{
						if (++element_pointer != current_group.last_endpoint) // ie. not beyond end of available data - note: using && short-circuits additional increments/decrements unless previous condition passes - intentional
						{
							if (!*(++erasure_field) && addition-- == 1)
							{
								return *this;
							}

							continue; // Only actual loop point is here
						}
						else if (current_group.next_group == NULL) // ie. beyond end of available data
						{
							++erasure_field;
							return *this;
						}
						else // ie. beyond end of available data
						{
							group_pointer = current_group.next_group;
							element_pointer = group_pointer->elements;
							erasure_field = group_pointer->erased;

							if (!*(erasure_field) && addition-- == 1) 
							{
								return *this;
							}

							break;
						}
					}
				}
			}


			// All other groups:
			while(true)
			{
				const group &current_group = *group_pointer;

				if (current_group.total_number_of_elements <= addition)
				{
					if (current_group.next_group == NULL)
					{
						const unsigned int difference = static_cast<const unsigned int>(current_group.last_endpoint - element_pointer);
						element_pointer = current_group.last_endpoint;
						erasure_field += difference;
						return *this;
					}

					addition -= current_group.total_number_of_elements - 1;
					group_pointer = current_group.next_group;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased;

					if (!*(erasure_field) && addition-- == 1)
					{
						return *this;
					}

					continue; // The only actual loop point
				}
				else if (current_group.total_number_of_elements == static_cast<const unsigned int>(current_group.last_endpoint - current_group.elements))
				{
					element_pointer += addition;
					erasure_field += addition;
					return *this;
				}
				else	 // ie. total_number_of_elements > addition - safe to ignore endpoint check condition
				{
					while(true)
					{
						++element_pointer;

						if (!*(++erasure_field) && addition-- == 1)
						{
							return *this;
						}
					}
				}
			}

			return *this;
		}

			

		colony_iterator & operator -= (unsigned int subtraction) 
		{
			assert(group_pointer != NULL);
			
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;
				
				if (current_group.total_number_of_elements == static_cast<const unsigned int>(current_group.last_endpoint - current_group.elements))
				{
					const unsigned int distance_from_beginning = static_cast<const unsigned int>(element_pointer - current_group.elements);
					
					if(subtraction <= distance_from_beginning)
					{
						element_pointer -= subtraction;
						erasure_field -= subtraction;
						return *this;
					}
					else if (current_group.previous_group == NULL) 
					{
						element_pointer = current_group.elements;
						erasure_field = current_group.erased;
						return *this;
					}
					else
					{
						group_pointer = current_group.previous_group;
						element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased);
						erasure_field = reinterpret_cast<bool_pointer_type>(element_pointer) + group_pointer->size;
						subtraction -= distance_from_beginning;
					}
				}
				else
				{
					while(true)
					{
						if (element_pointer-- != current_group.elements) // ie. not at beginning of group data
						{
							if (!*(--erasure_field) && subtraction-- == 1)
							{
								return *this;
							}
							
							continue; // only actual loop point 
						}
						else if (group_pointer->previous_group == NULL)
						{
							++element_pointer;
							return *this;
						}
						else
						{
							group_pointer = group_pointer->previous_group;
							element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased);
							erasure_field = reinterpret_cast<bool_pointer_type>(element_pointer) + group_pointer->size;
							break; // Process through other groups
						}
					}
				}
			}
			

			// All other groups:
			while(true) 
			{
				const group &current_group = *group_pointer;
				
				if (current_group.total_number_of_elements < subtraction)
				{
					if (current_group.previous_group == NULL) 
					{
						element_pointer = current_group.elements;
						erasure_field = current_group.erased;
						return *this;
					}
					
					subtraction -= current_group.total_number_of_elements;
					group_pointer = current_group.previous_group;
					element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased);
					erasure_field = reinterpret_cast<bool_pointer_type>(element_pointer) + group_pointer->size;
					continue;
				}
				else if (current_group.total_number_of_elements == static_cast<const unsigned int>(current_group.last_endpoint - current_group.elements))
				{
					element_pointer -= subtraction;
					erasure_field -= subtraction;
					return *this;
				}
				else
				{
					while(true)
					{
						--element_pointer;

						if (!*(--erasure_field) && subtraction-- == 1)
						{
							return *this;
						}
					}
				}
			}

			return *this;
		}



		inline colony_iterator operator + (const unsigned int increment) const
		{
			return colony_iterator(*this) += increment;
		}



		inline colony_iterator operator - (const unsigned int decrement) const
		{
			return colony_iterator(*this) -= decrement;
		}
		

	private:

		unsigned int get_current_index() const PLF_NOEXCEPT
		{
			unsigned int distance_to_beginning = 0;

			if (element_pointer != group_pointer->last_endpoint) // colony_iterator does not point to the index one past the last element in group
			{
				element_pointer_type current_element = group_pointer->elements;
				bool_pointer_type current_erasure = group_pointer->erased;

				if (element_pointer < group_pointer->elements) // ie. reverse_iterator rend()
				{
					return element_pointer - group_pointer->elements;
				}
				
				// process current group:
				while (current_element++ != element_pointer)
				{
					if (!*(current_erasure++))
					{
						++distance_to_beginning;
					}
				}
			}
			else // Special case: colony_iterator points to index one past the last element in the group:
			{
				distance_to_beginning = group_pointer->total_number_of_elements;
			}


			group_pointer_type current_group = group_pointer->previous_group;

			// process previous groups:
			while (current_group != NULL)
			{
				distance_to_beginning += current_group->total_number_of_elements;
				current_group = current_group->previous_group;
			} 
			
			return distance_to_beginning;
		}



	public:	

		inline unsigned int operator + (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return get_current_index() + rh.get_current_index();
		}



		inline int operator - (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return get_current_index() - rh.get_current_index();
		}
		


		inline bool operator > (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return (((group_pointer == rh.group_pointer) && (element_pointer > rh.element_pointer)) || (group_pointer->group_number > rh.group_pointer->group_number));
		}

		
		
		inline bool operator < (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return rh > *this;
		}



		inline bool operator >= (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return !(rh > *this);
		}
		
	

		inline bool operator <= (const colony_iterator &rh) const PLF_NOEXCEPT
		{
			return !(*this > rh);
		}



		colony_iterator() PLF_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), erasure_field(NULL) {}

		~colony_iterator() PLF_NOEXCEPT {}
		

		inline colony_iterator (const colony_iterator &source) PLF_NOEXCEPT: 
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer),
			erasure_field(source.erasure_field)
		{}




		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move constructor
			inline colony_iterator(const colony_iterator &&source) PLF_NOEXCEPT:
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer)),
				erasure_field(std::move(source.erasure_field))
			{}
		#endif

	};



	template <class colony_allocator_type, bool is_const> class colony_reverse_iterator : public std::iterator<std::random_access_iterator_tag, element_type>
	{
	private:
		typename colony::iterator the_iterator;

	public:
		typedef typename choose<is_const, typename colony_allocator_type::const_reference, typename colony_allocator_type::reference>::type	reference;
		typedef typename choose<is_const, typename colony_allocator_type::const_pointer, typename colony_allocator_type::pointer>::type		pointer;

		friend class colony;


		inline colony_reverse_iterator& operator = (const colony_reverse_iterator &source) PLF_NOEXCEPT 
		{
			the_iterator = source.the_iterator;
			return *this;
		}
		


		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move assignment
			inline colony_reverse_iterator& operator = (const colony_reverse_iterator &&source) PLF_NOEXCEPT 
			{
				the_iterator = std::move(source.the_iterator);
				return *this;
			}
		#endif
		


		inline bool operator == (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator == rh.the_iterator);
		}
		
		

		inline bool operator != (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator != rh.the_iterator);
		}



		inline reference operator * () const PLF_NOEXCEPT
		{
			return *(the_iterator.element_pointer);
		}
		


		inline pointer * operator -> () const PLF_NOEXCEPT
		{
			return the_iterator.element_pointer;
		}
		


		colony_reverse_iterator & operator ++ () 
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL);

			do
			{
				if (element_pointer <= group_pointer->elements) // ie. beginning of available data (ie. [0] or before) reached
				{
					if (group_pointer->previous_group != NULL)
					{
						group_pointer = group_pointer->previous_group;
						element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased); // -1 handled by loop end
						erasure_field = reinterpret_cast<bool_pointer_type>(element_pointer) + group_pointer->size; // -1 Covered by loop condition
					}
					else
					{
						--element_pointer;
						--erasure_field;
						return *this;
					}
				}
				
				--element_pointer;
			} while (*--erasure_field);

			return *this;
		}



		inline colony_reverse_iterator operator ++ (int)
		{
			colony_reverse_iterator copy(*this);
			++*this;
			return copy;
		}


		colony_reverse_iterator & operator -- ()
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL); // covers uninitialised iterator && empty group

			do
			{
				++erasure_field;

				if (group_pointer->next_group == NULL)
				{
					if (++element_pointer >= group_pointer->last_endpoint) // ie. end of available data not reached
					{
						return *this;
					}
				}
				else if (++element_pointer == group_pointer->last_endpoint) // ie. beyond end of available data
				{
					group_pointer = group_pointer->next_group;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased;
				}
			} while (*erasure_field);

			return *this;
		}
		


		inline colony_reverse_iterator operator -- (int)
		{
			colony_reverse_iterator copy(*this);
			--*this;
			return copy;
		}



		colony_reverse_iterator & operator += (unsigned int addition)
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL);
			
			while (addition-- != 0)
			{
				do
				{
					if (element_pointer > group_pointer->elements) // ie. beginning of available data (ie. [0]) not reached
					{
						--element_pointer;
					}
					else if (group_pointer->previous_group == NULL)
					{
						if (element_pointer == group_pointer->elements)
						{
							--element_pointer;
							--erasure_field;
						}
						
						return *this;
					}
					else
					{
						group_pointer = group_pointer->previous_group;
						element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->erased) - 1;
						erasure_field = group_pointer->erased + group_pointer->size; // -1 Covered by loop condition
					}
				} while (*--erasure_field);
			}

			return *this;
		}



		colony_reverse_iterator & operator -= (unsigned int subtraction)
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL); // covers uninitialised iterator && empty group

			while (subtraction-- != 0)
			{
				do
				{
					++erasure_field;
					
					if (group_pointer->next_group == NULL)
					{
						if (++element_pointer >= group_pointer->last_endpoint) // ie. end of available data not reached
						{
							return *this;
						}
					}
					else if (++element_pointer == group_pointer->last_endpoint) // ie. not beyond end of available data
					{
						group_pointer = group_pointer->next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased;
					}
				} while(*erasure_field);
			}

			return *this;
		}



		inline colony_reverse_iterator operator + (const unsigned int increment)
		{
			return colony_reverse_iterator(*this) += increment;
		}



		inline colony_reverse_iterator operator - (const unsigned int decrement)
		{
			return colony_reverse_iterator(*this) -= decrement;
		}



		inline unsigned int operator + (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return the_iterator.get_current_index() + rh.the_iterator.get_current_index();
		}



		inline int operator - (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return  rh.the_iterator.get_current_index() - the_iterator.get_current_index();
		}



		typename colony::iterator base() const
		{
			return ++(typename colony::iterator(the_iterator));
		}



		inline bool operator > (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (rh.the_iterator > the_iterator);
		}



		inline bool operator < (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator > rh.the_iterator);
		}



		inline bool operator >= (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return !(the_iterator > rh.the_iterator);
		}



		inline bool operator <= (const colony_reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return !(rh.the_iterator > the_iterator);
		}




		colony_reverse_iterator() PLF_NOEXCEPT
		{}



		~colony_reverse_iterator() PLF_NOEXCEPT {}



		colony_reverse_iterator (const colony_reverse_iterator &source) PLF_NOEXCEPT: the_iterator(source.the_iterator)
		{}



		colony_reverse_iterator (const typename colony::iterator &source) PLF_NOEXCEPT: the_iterator(source)
		{}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move constructors
			colony_reverse_iterator (const colony_reverse_iterator &&source) PLF_NOEXCEPT: the_iterator(std::move(source.the_iterator))
			{}

			colony_reverse_iterator (const typename colony::iterator &&source) PLF_NOEXCEPT: the_iterator(std::move(source))
			{}
		#endif

	};



private:

	iterator				end_iterator, begin_iterator;
	group_pointer_type		first_group;
	size_type				total_size;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		unsigned int max_elements_per_group;
		ebco_pair(const unsigned int max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;
	iterator_stack_type		empty_indexes;


	inline void initialize(const unsigned int elements_per_group)
	{
		first_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		try
		{
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, elements_per_group);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(elements_per_group));
			#endif
		}
		catch (...)
		{
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			throw;
		}
		
		begin_iterator.group_pointer = first_group;
		begin_iterator.element_pointer = first_group->elements;
		begin_iterator.erasure_field = first_group->erased;
		end_iterator = begin_iterator;
	}



public:

	colony():
		total_size(0),
		group_allocator_pair(UINT_MAX / 2)
	{
		initialize(8);
	}




	colony(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount = UINT_MAX / 2):
		total_size(0),
		group_allocator_pair(max_allocation_amount),
		empty_indexes((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount < group_allocator_pair.max_elements_per_group);
		assert(group_allocator_pair.max_elements_per_group <= UINT_MAX / 2);

		initialize(initial_allocation_amount);
	}



	colony (const colony &source): 
		total_size(0), 
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
		empty_indexes(static_cast<const unsigned int>((source.empty_indexes.first_group->end + 1) - source.empty_indexes.first_group->elements))
	{
		assert (source.first_group != NULL); // Uninitialised/destructed source colony

		initialize(source.first_group->size);
		
		// Copy data from source:
		if (!source.empty())
		{
			insert(source.begin(), source.end());
		}
	}
	


	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		colony(colony &&source) PLF_NOEXCEPT: 
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			first_group(std::move(source.first_group)),
			total_size(source.total_size),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
			empty_indexes(std::move(source.empty_indexes))
		{
			source.first_group = NULL; 
			source.total_size = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_size 0, to allow for clean destructor usage
		}
	#endif
	
	

	inline const iterator & begin() const PLF_NOEXCEPT // const return necessary for visual c++
	{
		return begin_iterator;
	}



	inline const iterator & end() const PLF_NOEXCEPT
	{
		return end_iterator;
	}
	


	inline const_iterator cbegin() const PLF_NOEXCEPT
	{
		const_iterator return_iterator;

		return_iterator.group_pointer = begin_iterator.group_pointer;
		return_iterator.element_pointer = begin_iterator.element_pointer;
		return_iterator.erasure_field = begin_iterator.erasure_field;
		
		return return_iterator;
	}



	inline const_iterator cend() const PLF_NOEXCEPT
	{
		const_iterator return_iterator;

		return_iterator.group_pointer = end_iterator.group_pointer;
		return_iterator.element_pointer = end_iterator.element_pointer;
		return_iterator.erasure_field = end_iterator.erasure_field;
		
		return return_iterator;
	}
	


	inline reverse_iterator rbegin() const
	{
		return ++reverse_iterator(end_iterator);
	}



	inline reverse_iterator rend() const PLF_NOEXCEPT
	{
		reverse_iterator beforebegin(begin_iterator);
		--(beforebegin.the_iterator.element_pointer); // ie. point to memory space before first element
		--(beforebegin.the_iterator.erasure_field); // ie. point to memory space before first erasure field
		return beforebegin;
	}



	inline const_reverse_iterator crbegin() const
	{
		return ++const_reverse_iterator(end_iterator);
	}



	inline const_reverse_iterator crend() const PLF_NOEXCEPT
	{
		const_reverse_iterator beforebegin(begin_iterator);
		--(beforebegin.the_iterator.element_pointer); // ie. point to memory space before first element
		--(beforebegin.the_iterator.erasure_field); // ie. point to memory space before first erasure field
		return beforebegin;
	}



	~colony() PLF_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_NOEXCEPT
	{
	#ifdef PLF_TYPE_TRAITS_SUPPORT
		if (total_size != 0	&& !(std::is_trivially_destructible<element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructor
	#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise the 'destruct' loop out for POD types
		if (total_size != 0)
	#endif
		{
			total_size = 0;
			first_group = NULL;

			group_pointer_type previous_group, group_pointer = begin_iterator.group_pointer;
			element_pointer_type element_pointer = begin_iterator.element_pointer, end_pointer = group_pointer->last_endpoint;
			bool_pointer_type erasure_field = begin_iterator.erasure_field;

			while (true)
			{
				if (!(*erasure_field++))
				{
					PLF_DESTROY(element_allocator_type, (*this), element_pointer);
				}

				if (++element_pointer == end_pointer) // ie. not beyond end of available data
				{
					previous_group = group_pointer;
					group_pointer = group_pointer->next_group;
					PLF_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
					PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);
					
					if (group_pointer == NULL)
					{
						return;
					}

					end_pointer = group_pointer->last_endpoint;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased;
				}
			}
		}
		else
		{
			// Although technically under a type-traits-supporting compiler total_size could be non-zero at this point, since first_group would already be NULL in the case of double-destruction, it's unnecessary to zero total_size, and for some reason doing so creates a performance regression under gcc x64 (5.1 and below)
			group_pointer_type previous_group;

			while (first_group != NULL)
			{
				previous_group = first_group;
				first_group = first_group->next_group;
				PLF_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);
			}
		}
	}




public:

	iterator insert(const element_type &element)
	{
		if (empty_indexes.empty())
		{
			if (end_iterator.element_pointer != reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->erased)) /* ie. not past end of group */
			{
				const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
				PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, element);

				++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
				++end_iterator.erasure_field;
				++(end_iterator.group_pointer->last_endpoint);
				++(end_iterator.group_pointer->total_number_of_elements);
				++total_size;
				return return_iterator; /* returns value before incrementation */
			}
			else
			{
				end_iterator.group_pointer->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
				const group_reference_type next_group = *(end_iterator.group_pointer->next_group);

				try
				{
					#ifdef PLF_VARIADICS_SUPPORT
						PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
					#else // c++0x only supports copy construction
						PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
					#endif
				}
				catch (...)
				{
					PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
					end_iterator.group_pointer->next_group = NULL;
					throw;
				}

				end_iterator.group_pointer = &next_group;
				end_iterator.element_pointer = next_group.elements;
				end_iterator.erasure_field = next_group.erased;
				const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

				try
				{
					PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
				}
				catch (...)
				{
					end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
					end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->erased);
					end_iterator.erasure_field = reinterpret_cast<bool_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
					PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
					end_iterator.group_pointer->next_group = NULL;
					throw;
				}

				++end_iterator.erasure_field;
				++(next_group.last_endpoint);
				++(next_group.total_number_of_elements);
				++total_size;
				return return_iterator; /* returns value before incrementation */
			}
		}
		else
		{
			const iterator & new_index = empty_indexes.top();
			PLF_CONSTRUCT(element_allocator_type, (*this), new_index.element_pointer, element);

			*new_index.erasure_field = false;
			++(new_index.group_pointer->total_number_of_elements);

			if (new_index.group_pointer == first_group && new_index.element_pointer < begin_iterator.element_pointer)
			{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
				begin_iterator = new_index;
			}

			empty_indexes.pop(); /* Doesn't affect new_index as memory is not deallocated by pop nor is there a destructor for iterator */
			++total_size;
			return new_index;
		}
	}


	// Group insert:
	inline void insert(const iterator &iterator1, const iterator &iterator2)
	{
		assert(iterator1 <= iterator2);

		for (iterator the_iterator = iterator1; the_iterator != iterator2; ++the_iterator)
		{
			insert(*the_iterator);
		}
	}


	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		iterator insert(const element_type &&element)
		{
			if (empty_indexes.empty())
			{
				if (end_iterator.element_pointer != reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->erased)) /* ie. not past end of group */
				{
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
					PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, std::move(element));
					++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
					++end_iterator.erasure_field;
					++(end_iterator.group_pointer->last_endpoint);
					++(end_iterator.group_pointer->total_number_of_elements);
					++total_size;
					return return_iterator; /* returns value before incrementation */
				}
				else
				{
					end_iterator.group_pointer->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
					const group_reference_type next_group = *(end_iterator.group_pointer->next_group);

					try
					{
						#ifdef PLF_VARIADICS_SUPPORT
							PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
						#else // c++0x only supports copy construction
							PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
						#endif
					}
					catch (...)
					{
						PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					end_iterator.group_pointer = &next_group;
					end_iterator.element_pointer = next_group.elements;
					end_iterator.erasure_field = next_group.erased;
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

					try
					{
						PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, std::move(element));
					}
					catch (...)
					{
						end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
						end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->erased);
						end_iterator.erasure_field = reinterpret_cast<bool_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
						PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					++end_iterator.erasure_field;
					++(next_group.last_endpoint);
					++(next_group.total_number_of_elements);
					++total_size;
					return return_iterator; /* returns value before incrementation */
				}
			}
			else
			{
				const iterator &new_index = empty_indexes.top();
				PLF_CONSTRUCT(element_allocator_type, (*this), new_index.element_pointer, std::move(element));

				*new_index.erasure_field = false;
				++(new_index.group_pointer->total_number_of_elements); 
				 
				if (new_index.group_pointer == first_group && new_index.element_pointer < begin_iterator.element_pointer)  
				{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
					begin_iterator = new_index; 
				} 
				 
				empty_indexes.pop(); /* Doesn't affect new_index as memory is not deallocated by pop nor is there a destructor for iterator */
				++total_size; 
				return new_index;
			}
		}


		// Group move-insert:
		inline void insert(iterator &&iterator1, iterator &&iterator2)
		{
			assert(iterator1 <= iterator2);

			for (iterator the_iterator = iterator1; the_iterator != iterator2; ++the_iterator)
			{
				insert(std::move(*the_iterator));
			}
		}


		#ifdef PLF_VARIADICS_SUPPORT
			template<typename... Arguments> inline iterator emplace(Arguments... parameters)
			{
				return insert(std::move(element_type(std::forward<Arguments>(parameters)...))); // Use object's parameter'd constructor
			}
		#endif
	#endif





	// Still must retain return iterator in case of full group erasure:
	iterator erase(const iterator &the_iterator)
	{
		assert(total_size != 0);
		const group_pointer_type the_group_pointer = the_iterator.group_pointer;
		assert(the_group_pointer != NULL);
		group &the_group = *the_group_pointer;

		*the_iterator.erasure_field = true;
		PLF_DESTROY(element_allocator_type, (*this), the_iterator.element_pointer); // Destruct element

		--total_size;

		if (the_group.total_number_of_elements-- != 1) // ie. non-empty group at this point in time, don't consolidate
		{
			empty_indexes.push(the_iterator);

			if (the_iterator != begin_iterator)
			{
				return ++iterator(the_iterator);
			}
			else
			{
				return ++begin_iterator; // Increment the beginning iterator to a non-erased position
			}
		}

		// else: consolidation of empty groups
		if (the_group_pointer == first_group)
		{
			if (the_group.next_group == NULL) // ie. only group in colony
			{
				const unsigned int first_size = first_group->size;
				PLF_DESTROY(group_allocator_type, group_allocator_pair, first_group);

				#ifdef PLF_VARIADICS_SUPPORT
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, first_size);
				#else
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(first_size));
				#endif

				empty_indexes.clear();
				// Reset begin_iterator:
				begin_iterator.element_pointer = first_group->elements;
				begin_iterator.erasure_field = first_group->erased;
				end_iterator = begin_iterator;

				return end_iterator;
			}
			else // Remove first group, change first group to next group
			{
				the_group.next_group->previous_group = NULL; // Cut off this group from the chain
				first_group = the_group.next_group; // Make the next group the first group

				begin_iterator.group_pointer = first_group; // note: end iterator only needs to be changed if the deleted group was the final group in the chain
				begin_iterator.element_pointer = first_group->elements;
				begin_iterator.erasure_field = first_group->erased;

				if (!empty_indexes.empty())
				{
					consolidate_empty_indexes(the_group_pointer); 
				}

				PLF_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);
				
				// If the beginning index has been erased, find the next non-erased element:
				if (*begin_iterator.erasure_field)
				{
					++begin_iterator;
				}
				
				return begin_iterator;
			}
		}
		else // this is a non-first group: the group is completely empty of elements, so delete the group, then link previous group's next-group field to the next non-empty group in the series, removing this link in the chain:
		{
			iterator return_iterator;
			
			if (the_group.next_group != NULL) // Not the final group in chain
			{
				the_group.next_group->previous_group = the_group.previous_group;
				return_iterator.group_pointer = the_group.previous_group->next_group = the_group.next_group; // close the chain, removing this group from it
				return_iterator.element_pointer = return_iterator.group_pointer->elements;
				return_iterator.erasure_field = return_iterator.group_pointer->erased;

				if (*return_iterator.erasure_field) // If first element of next group is erased, increment until we find a non-erased element:
				{
					++return_iterator;
				}
			}
			else // This is the final group in the chain
			{
				the_group.previous_group->next_group = NULL;
				end_iterator.group_pointer = the_group.previous_group; // end iterator only needs to be changed if this is the final group in the chain
				end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->erased);
			 	end_iterator.erasure_field = reinterpret_cast<bool_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
				return_iterator = end_iterator;
			}

			if (!empty_indexes.empty())
			{
				consolidate_empty_indexes(the_group_pointer);
			}

			PLF_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

			return return_iterator;
		}
	}



	inline void erase(const iterator &iterator1, const iterator &iterator2)
	{
		assert(iterator1 != iterator2);
		assert(iterator1 < iterator2);

		for (iterator the_iterator = iterator1; the_iterator != iterator2; ++the_iterator)
		{
			erase(*the_iterator);
		}
	}



private:

	void consolidate_empty_indexes(const group_pointer_type the_group_pointer)
	{ 	
		// Remove all entries containing the provided colony group, from the empty_indexes stack, also consolidate the stack removing unused trailing groups in the process. 
		
		// Pseudocode:
		// First, remove any trailing unused groups from the stack. These may be present if a stack has pushed then popped a lot, as plf::stack never deallocates.
		// If there're no entries from the supplied colony group in particular groups in the stack, preserve those groups (no alteration) in the new chain.
		// If there are entries from the supplied colony group in any group in the stack, copy these to the new stack group and remove the old group.
		// If the group is at the end of the old stack but is partial, and there have been some copies made from old groups, copy this group and don't preserve it (otherwise end up with a new group following a partially-full old group). If there're no copies (just old groups) preserve it.
		// At the end, if the new group has copies in it, add it to the chain. Or if the stack is now empty, reinitialize it. Or if there're no old groups remaining make the new group the first group. Or if there are only old groups link put the new chain into the stack to replace the old one.
		// Complicated but faster than any other alternative.

		typedef typename iterator_stack_type::group_pointer_type stack_group_pointer;
		typedef typename iterator_stack_type::element_pointer_type stack_element_pointer;

		#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
			typedef typename iterator_stack_type::group_allocator_type stack_group_allocator_type; // Not used by PLF_DESTROY etc when std::allocator_traits not supported
		#endif

		typename iterator_stack_type::ebco_pair &stack_group_allocator = empty_indexes.group_allocator_pair;

		const unsigned int initial_size = static_cast<const unsigned int>((empty_indexes.first_group->end + 1) - empty_indexes.first_group->elements);

		// Remove trailing stack groups (not removed in general 'pop' usage in plf::stack for performance reasons)
		if (empty_indexes.current_group->next_group != NULL)
		{
			stack_group_pointer temp_group = empty_indexes.current_group->next_group;
			empty_indexes.current_group->next_group = NULL; // Close off chain from trailing groups

			do
			{
				const stack_group_pointer previous_group = temp_group;
				temp_group = temp_group->next_group;
				PLF_DESTROY(stack_group_allocator_type, stack_group_allocator, previous_group);
				PLF_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, previous_group, 1);
			} while (temp_group != NULL);
		}

		// All groups from here onwards refer to empty_index's stack groups, not colony groups, unless stated otherwise
		const unsigned int new_group_size = (empty_indexes.total_size < 3) ? 3 : empty_indexes.total_size;

		stack_group_pointer current_old_group = empty_indexes.first_group, new_group = PLF_ALLOCATE(stack_group_allocator_type, stack_group_allocator, 1, empty_indexes.current_group), first_new_chain = NULL, current_new_chain = NULL;

		try
		{
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(stack_group_allocator_type, stack_group_allocator, new_group, new_group_size, empty_indexes.current_group);
			#else
				PLF_CONSTRUCT(stack_group_allocator_type, stack_group_allocator, new_group, typename iterator_stack_type::group(new_group_size, empty_indexes.current_group));
			#endif
		}
		catch (...)
		{
			PLF_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, new_group, 1);
			throw;
		}


		stack_element_pointer iterator_pointer, source_end, destination_begin = new_group->elements, the_end = NULL;
		const stack_element_pointer destination_start = destination_begin;
		unsigned int total_number_of_copies = 0, number_to_be_copied = 0;

		do // per old group of stack
		{
			if (current_old_group->next_group != NULL)
			{
				source_end = current_old_group->end + 1;
			}
			else
			{
				source_end = empty_indexes.current_element + 1;
			}

			iterator_pointer = current_old_group->elements;
			const unsigned int number_of_group_elements = static_cast<const unsigned int>(source_end - iterator_pointer);

			for (; iterator_pointer != source_end; ++iterator_pointer) // per old group element
			{
				if (iterator_pointer->group_pointer != the_group_pointer) // ie. this stack element doesn't match the group pointer, doesn't need to be removed
				{
					++number_to_be_copied;
				}
				else if (number_to_be_copied != 0)
				{
					// Use the fastest method for moving iterators, while perserving values if allocator provides non-trivial pointers - unused if/else branches will be optimised out by any decent compiler:
					#ifdef PLF_TYPE_TRAITS_SUPPORT
						if (std::is_trivially_copyable<element_pointer_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
						{
							std::memcpy(&*destination_begin, &*(iterator_pointer - number_to_be_copied), number_to_be_copied * sizeof(iterator)); // &* to avoid problems with non-trivial pointers that are trivially-copyable
						}
						#ifdef PLF_MOVE_SEMANTICS_SUPPORT
							else if (std::is_move_constructible<element_pointer_type>::value)
							{
								std::uninitialized_copy(std::make_move_iterator(iterator_pointer - number_to_be_copied), std::make_move_iterator(iterator_pointer), destination_begin);
							}
						#endif
						else
					#endif
					{
						std::uninitialized_copy(iterator_pointer - number_to_be_copied, iterator_pointer, destination_begin);
						
						#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT // Typedef will not be used by macro if allocator_traits not supported
							typedef typename iterator_stack_type::allocator_type stack_element_allocator_type;
						#endif

						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_DESTROY(stack_element_allocator_type, empty_indexes, element_pointer);
						}
					}

					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}
			}

			const bool test = number_to_be_copied == static_cast<const unsigned int>((current_old_group->end + 1) - current_old_group->elements);

			if (test || (number_to_be_copied == number_of_group_elements && destination_begin == destination_start))
			{
				if (test)
				{
					the_end = current_old_group->end;
				}
				else
				{
					the_end = source_end - 1;
				}

				// No deletions, preserve group
				if (current_new_chain == NULL)
				{
					// First in chain
					current_new_chain = first_new_chain = current_old_group;
					current_old_group->previous_group = NULL;
				}
				else
				{
					// Add to chain
					current_new_chain->next_group = current_old_group;
					current_old_group->previous_group = current_new_chain;
					current_new_chain = current_old_group;
				}

				total_number_of_copies += number_to_be_copied;
				number_to_be_copied = 0;
				current_old_group = current_old_group->next_group;
			}
			else
			{
				if (number_to_be_copied != 0)
				{
					// move remainder:
					#ifdef PLF_TYPE_TRAITS_SUPPORT
						if (std::is_trivially_copyable<element_pointer_type>::value)
						{
							std::memcpy(&*destination_begin, &*(iterator_pointer - number_to_be_copied), number_to_be_copied * sizeof(iterator));
						}
						#ifdef PLF_MOVE_SEMANTICS_SUPPORT
							else if (std::is_move_constructible<element_pointer_type>::value)
							{
								std::uninitialized_copy(std::make_move_iterator(iterator_pointer - number_to_be_copied), std::make_move_iterator(iterator_pointer), destination_begin);
							}
						#endif
						else
					#endif
					{
						std::uninitialized_copy(iterator_pointer - number_to_be_copied, iterator_pointer, destination_begin);

						#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT 
							typedef typename iterator_stack_type::allocator_type stack_element_allocator_type;
						#endif

						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_DESTROY(stack_element_allocator_type, empty_indexes, element_pointer);
						}
					}

					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}

				// Remove old group:
				const stack_group_pointer prev_group = current_old_group;
				current_old_group = current_old_group->next_group;
				PLF_DESTROY(stack_group_allocator_type, stack_group_allocator, prev_group);
				PLF_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, prev_group, 1);
			}
		} while (current_old_group != NULL);


		if (current_new_chain == NULL && destination_begin != destination_start) // No old groups remaining, some copies
		{
			new_group->previous_group = NULL;
			empty_indexes.first_group = empty_indexes.current_group = new_group;
			empty_indexes.current_element = destination_begin - 1;
			empty_indexes.start_element = new_group->elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else if (destination_begin != destination_start) // Some deletions occured ie. some elements copied, some old groups remaining - current_new_chain != NULL implied by previous if
		{
			new_group->previous_group = current_new_chain;
			empty_indexes.current_group = current_new_chain->next_group = new_group;
			empty_indexes.first_group = first_new_chain;
			empty_indexes.current_element = destination_begin - 1;
			empty_indexes.start_element = new_group->elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else if (current_new_chain != NULL) // No copies, some old groups - destination_begin == destination_start implied by previous if's
		{
			PLF_DESTROY(stack_group_allocator_type, stack_group_allocator, new_group);
			PLF_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, new_group, 1);

			current_new_chain->next_group = NULL;
			empty_indexes.current_group = current_new_chain;
			empty_indexes.first_group = first_new_chain;
			empty_indexes.current_element = the_end;
			empty_indexes.start_element = current_new_chain->elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else // No elements remaining - least likely
		{
			empty_indexes.total_size = 0;
			PLF_DESTROY(stack_group_allocator_type, stack_group_allocator, new_group); // New construction will occur within initialise function below
			empty_indexes.first_group = empty_indexes.current_group = new_group;
			empty_indexes.initialize(initial_size);
		}
	}



public:

	inline bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}



	inline size_type size() const PLF_NOEXCEPT
	{
		return total_size;
	}



	void reinitialize(const unsigned int initial_allocation_amount)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount < group_allocator_pair.max_elements_per_group);

		destroy_all_data();

		empty_indexes.reinitialize((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8);
		initialize(initial_allocation_amount);
	}



	inline void reinitialize(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount)
	{
		assert(group_allocator_pair.max_elements_per_group <= UINT_MAX / 2);

		group_allocator_pair.max_elements_per_group = max_allocation_amount;
		reinitialize(initial_allocation_amount);
	}



	inline void clear()
	{
		reinitialize(first_group->size); // Note: May not be original size of colony- if all first group elements were erased by colony the group will have be removed and the next group becomes the 'first_group'
	}



	colony & operator = (const colony &source)
	{
		assert (&source != this);
		assert (source.first_group != NULL); // ie. data has been destroyed on this element, possibly result of std::move

		destroy_all_data();

		total_size = source.total_size;
		group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
		initialize(source.first_group->size);
		empty_indexes.reinitialize((source.empty_indexes.first_group->end + 1) - source.empty_indexes.first_group->elements);

		// Copy data from source:
		if (!source.empty())
		{
			insert(source.begin(), source.end());
		}

		return *this;
	}




	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		colony & operator = (colony &&source) PLF_NOEXCEPT
		{
			assert (&source != this);
			assert (source.first_group != NULL);

			destroy_all_data();
			empty_indexes.destroy_all_data();

			// Move source values across:
			end_iterator = std::move(source.end_iterator);
			begin_iterator = std::move(source.begin_iterator);
			first_group = std::move(source.first_group);
			total_size = source.total_size;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
			empty_indexes = std::move(source.empty_indexes);

			source.first_group = NULL; 
			source.total_size = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_size 0, to allow for clean destructor usage
			return *this;
		}
	#endif



	bool operator == (const colony &rh) const PLF_NOEXCEPT
	{
		assert (this != &rh);

		if (total_size != rh.total_size)
		{
			return false;
		}
		else if (total_size == 0)
		{
			return true;
		}

		iterator rh_iterator = rh.begin_iterator;

		for (iterator the_iterator = begin_iterator; the_iterator != end_iterator; ++the_iterator)
		{
			if (*rh_iterator++ != *the_iterator)
			{
				return false;
			}
		}

		return true;
	}




	inline bool operator != (const colony &rh) const PLF_NOEXCEPT
	{
		return !(*this == rh);
	}


	inline iterator operator [] (const unsigned int index) const PLF_NOEXCEPT
	{
		return begin_iterator + index;
	}
	
};

}

#undef PLF_TYPE_TRAITS_SUPPORT
#undef PLF_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_VARIADICS_SUPPORT
#undef PLF_MOVE_SEMANTICS_SUPPORT
#undef PLF_NOEXCEPT

#undef PLF_CONSTRUCT
#undef PLF_DESTROY
#undef PLF_ALLOCATE
#undef PLF_ALLOCATE_INITIALIZATION
#undef PLF_DEALLOCATE


#endif // PLF_COLONY_H