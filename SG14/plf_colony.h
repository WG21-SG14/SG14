// Copyright (c) 2015, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef plf_colony_H
#define plf_colony_H


#include <cstring>	// memset, memcpy
#include <cassert>	// assert
#include <memory>	// allocators
#include <climits>	// UINT_MAX
#include <iterator> // inheritance for iterator
#include <cstddef>	// ptrdiff_t for iterator typedef

#include "plf_stack.h" // Includes plf::stack and compiler-specific Macro defines 

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif




namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class colony
{
public:
	class iterator; // forward declaration for typedefs below

private:
	struct group; // forward declaration for typedefs below
	typedef typename element_allocator_type::template rebind<group>::other 		group_allocator_type;
	typedef typename element_allocator_type::template rebind<bool>::other 		bool_allocator_type;
	typedef typename element_allocator_type::template rebind<iterator>::other 	iterator_allocator_type;
	typedef typename plf::stack<iterator, iterator_allocator_type>				iterator_stack_type;
	typedef typename element_allocator_type::pointer 	element_pointer_type;
	typedef typename group_allocator_type::pointer 		group_pointer_type;
	typedef typename group_allocator_type::reference 	group_reference_type;
	typedef typename bool_allocator_type::pointer 		bool_pointer_type;
	typedef typename iterator_allocator_type::pointer 	iterator_pointer_type;

	struct group
	{
		element_allocator_type		allocator;
		bool_allocator_type			bool_allocator;
		element_pointer_type		last_endpoint; // the address that is one past the highest cell number that's been used so far in this group - does not change with erase command
		group_pointer_type			next_group;
		const element_pointer_type	elements;
		const bool_pointer_type		erased;
		const element_pointer_type	end; // the address that is one past the end of the elements array - used in - iterator operators
		const bool_pointer_type		end_erased; // the address that is one past the end of the erased array - used in - iterator operators
		group_pointer_type			previous_group;
		unsigned int				total_number_of_elements; // indicates total number of used cells - changes with erase command
		const unsigned int			group_number; // Used for comparison (> < >= <=) iterator operators and (in debug mode) checking order in multi-erase and multi-add commands
		const unsigned int			size; // The number of elements allocated for this particular group
		unsigned int				number_of_erased_elements; // Number of elements erased within this group - used for speeding up multi-increment/decrement iterator operations


		#ifdef PLF_VARIADICS_SUPPORT 
			group(const unsigned int elements_per_group): 
				last_endpoint(allocator.allocate(elements_per_group, 0)), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL), 
				elements(last_endpoint),
				erased(bool_allocator.allocate(elements_per_group, 0)),
				end(elements + elements_per_group),
				end_erased(erased + elements_per_group),
				previous_group(NULL), 
				total_number_of_elements(0), 
				group_number(0), 
				size(elements_per_group), 
				number_of_erased_elements(0)
			{
				std::memset(erased, false, sizeof(bool) * size);
			}


			group(const unsigned int elements_per_group, group * const previous): 
				last_endpoint(allocator.allocate(elements_per_group, previous->elements)), 
				next_group(NULL), 
				elements(last_endpoint),
				erased(bool_allocator.allocate(elements_per_group, previous->erased)),
				end(elements + elements_per_group),
				end_erased(erased + elements_per_group),
				previous_group(previous), 
				total_number_of_elements(0), 
				group_number(previous->group_number + 1), 
				size(elements_per_group), 
				number_of_erased_elements(0)
			{
				std::memset(erased, false, sizeof(bool) * size);
			}


			~group() PLF_NOEXCEPT
			{
				allocator.deallocate(elements, size);
				bool_allocator.deallocate(erased, size);
			}

		#else 
			// This is a hack around the fact that allocator.construct only supports copy construction in C++0x. And to avoid running out of memory + the performance overhead for allocating the same block twice, we're skipping the allocation for these pseudoconstructors - the allocation will happen in the copy constructor.
			group(const unsigned int elements_per_group) PLF_NOEXCEPT: 
				last_endpoint(NULL),
				next_group(NULL), 
				elements(NULL), 
				erased(NULL),
				end(NULL),
				end_erased(NULL),
				previous_group(0), 
				total_number_of_elements(0), 
				group_number(0), 
				size(elements_per_group), 
				number_of_erased_elements(0)
			{}


			group(const unsigned int elements_per_group, group * const previous) PLF_NOEXCEPT: 
				last_endpoint(NULL),
				next_group(NULL), 
				elements(NULL), 
				erased(NULL),
				end(NULL),
				end_erased(NULL),
				previous_group(previous), 
				total_number_of_elements(0), 
				group_number(previous_group->group_number + 1), 
				size(elements_per_group),
				number_of_erased_elements(0)
			{}


			// Not a 'real' copy constructor, only used for the manufacturing of groups using the copy constructor and allocator.construct. Does not copy content of source asides from size and previous group
			group(const group &source): 
				last_endpoint(allocator.allocate(source.size, (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL), 
				elements(last_endpoint),
				erased(bool_allocator.allocate(source.size, (source.previous_group == NULL) ? 0 : source.previous_group->erased)),
				end(elements + source.size),
				end_erased(erased + source.size),
				previous_group(source.previous_group), 
				total_number_of_elements(0), 
				group_number(source.group_number), 
				size(source.size), 
				number_of_erased_elements(0)
			{
				std::memset(erased, false, sizeof(bool) * size);
			}


			~group() PLF_NOEXCEPT
			{
				if (elements != NULL) // NULL check necessary for pseudoconstructors in above pre-c++11 block
				{
					allocator.deallocate(elements, size); 
					bool_allocator.deallocate(erased, size);
				}
			}
		#endif

	};
	

public:
	class iterator : public std::iterator<std::bidirectional_iterator_tag, element_type, std::ptrdiff_t, element_type *, element_type &>
	{
	private:
		group_pointer_type		group_pointer;
		element_pointer_type	element_pointer;
		bool_pointer_type		erasure_field;

	public:
		friend class colony;
		friend class reverse_iterator;


		inline iterator& operator = (const iterator &source) PLF_NOEXCEPT
		{
			assert (&source != this);

			// Will generally be vectorized by the compiler and will be faster than a call to memcpy:
			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			erasure_field = source.erasure_field;

			return *this;
		}	



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT 
			// Move assignment
			inline iterator & operator = (const iterator &&source) PLF_NOEXCEPT // Move is a copy in this scenario
			{
				assert (&source != this);

				group_pointer = source.group_pointer;
				element_pointer = source.element_pointer;
				erasure_field = source.erasure_field;
				
				// Not strictly necessary to NULL source - no destructor
				return *this;
			}
		#endif
		


		inline bool operator == (const iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}

		

		inline bool operator != (const iterator &rh) const PLF_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}

		

		inline element_type & operator * () const PLF_NOEXCEPT
		{
			return *element_pointer;
		}
		


		inline element_type * operator -> () const PLF_NOEXCEPT
		{
			return element_pointer;
		}



	#ifdef PLF_GCCX64
		iterator & operator ++ () // For some reason, this version compiles to faster code under gcc x64
		{
			assert(group_pointer != NULL); // covers uninitialised iterator 

			do
			{
				if (++element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
				{
					group_pointer = group_pointer->next_group;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased - 1;
				}
				else if (element_pointer == group_pointer->last_endpoint) // Yeah, I know. Go figure
				{
					++erasure_field;
					return *this;
				}
			} while (*++erasure_field);

			return *this;
		}
	#elif defined(PLF_GCCX86)
		iterator & operator ++ () // For some reason, this version compiles to faster code under gcc x86 - and nothing else
		{
			assert(group_pointer != NULL); // covers uninitialised iterator 

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

			return *this;
		}
	#else
		iterator & operator ++ () // Standard version - compiles better on most other compilers
		{
			assert(group_pointer != NULL); // covers uninitialised iterator 

			do
			{
				if (++element_pointer == group_pointer->last_endpoint) // ie. beyond end of available data
				{
					if (group_pointer->next_group != NULL)
					{
						group_pointer = group_pointer->next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased - 1;
					}
					else 
					{
						++erasure_field;
						return *this;
					}
				}
			} while (*++erasure_field);

			return *this;
		}
	#endif



		iterator & operator -- () 
		{
			assert(group_pointer != NULL);
			
			do
			{
				if (element_pointer != group_pointer->elements) // ie. beginning of available data (ie. [0]) not reached
				{
					--element_pointer; // Erasure field decrement covered by loop condition
				}
				else if (group_pointer->previous_group == NULL)
				{
					return *this;
				}
				else
				{
					group_pointer = group_pointer->previous_group;
					element_pointer = group_pointer->end - 1;
					erasure_field = group_pointer->end_erased; // -1 Covered by loop condition
				}
			} while (*--erasure_field);

			return *this;
		}
		


		iterator & operator += (unsigned int addition) 
		{
			assert(group_pointer != NULL); // covers uninitialised iterator && empty group
			
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->elements)
			{
				const group &current_group = *group_pointer;
				
				if (current_group.number_of_erased_elements == 0)
				{
					const unsigned int distance_from_end = (const unsigned int) (current_group.last_endpoint - element_pointer);
					
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

						if (!*(erasure_field) && --addition == 0)
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
							if (!*(++erasure_field) && --addition == 0)
							{
								return *this;
							}
							
							// Only actual loop point is here
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

							if (!*(erasure_field) && --addition == 0)
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
						const unsigned int difference = (const unsigned int) (current_group.last_endpoint - element_pointer);
						element_pointer = current_group.last_endpoint;
						erasure_field += difference;
						return *this;
					}

					addition -= current_group.total_number_of_elements - 1;
					group_pointer = current_group.next_group;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased;

					if (!*(erasure_field) && --addition == 0)
					{
						return *this;
					}

					// Here: The only actual loop point
				}
				else if (current_group.number_of_erased_elements == 0)
				{
					element_pointer += addition;
					erasure_field += addition;
					return *this;
				}
				else   // ie. total_number_of_elements > addition - safe to ignore endpoint check condition
				{
					while(true)
					{
						++element_pointer;

						if (!*(++erasure_field) && --addition == 0)
						{
							return *this;
						}
					}
				}
			}

			return *this;
		}

			

		iterator & operator -= (unsigned int subtraction) 
		{
			assert(group_pointer != NULL);
			
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;
				
				if (current_group.number_of_erased_elements == 0)
				{
					const unsigned int distance_from_beginning = (const unsigned int) (element_pointer - current_group.elements);
					
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
						element_pointer = group_pointer->end;
						erasure_field = group_pointer->end_erased;
						subtraction -= distance_from_beginning;
					}
				}
				else
				{
					while(true)
					{
						if (element_pointer != current_group.elements) // ie. not at beginning of group data
						{
							--element_pointer;
							
							if (!*(--erasure_field) && --subtraction == 0)
							{
								return *this;
							}
							
							// only actual loop point is *here*
						}
						else if (group_pointer->previous_group == NULL)
						{
							return *this;
						}
						else
						{
							group_pointer = group_pointer->previous_group;
							element_pointer = group_pointer->end;
							erasure_field = group_pointer->end_erased;
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
					element_pointer = group_pointer->end;
					erasure_field = group_pointer->end_erased;
					// The only loop point *here*
				}
				else if (current_group.number_of_erased_elements == 0)
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

						if (!*(--erasure_field) && --subtraction == 0)
						{
							return *this;
						}
					}
				}
			}

			return *this;
		}



		inline iterator operator + (const unsigned int increment) const
		{
			iterator return_iterator = *this;
			return return_iterator += increment;
		}



		inline iterator operator - (const unsigned int decrement) const
		{
			iterator return_iterator = *this;
			return return_iterator -= decrement;
		}
		

	private:

		const unsigned int get_current_index() const
		{
			group_pointer_type current_group = group_pointer;
			unsigned int distance_to_beginning = 0;
			
			if (element_pointer != current_group->last_endpoint) // iterator does not point to the index one past the last element in group
			{
				element_pointer_type current_element = element_pointer;
				bool_pointer_type current_erasure = erasure_field;

				// process current group:
				while (current_element != current_group->elements)
				{
					--current_element;
					
					if (!*(--current_erasure))
					{
						++distance_to_beginning;
					}
				}
			}
			else // Special case: iterator points to index one past the last element in the group:
			{
				distance_to_beginning = current_group->total_number_of_elements;
			}
			

			current_group = current_group->previous_group;
				
			// process previous groups:
			while (current_group != NULL)
			{
				distance_to_beginning += current_group->total_number_of_elements;
				current_group = current_group->previous_group;
			} 
			
			return distance_to_beginning;
		}

		

	public:	

		inline unsigned int operator + (const iterator &rh) const
		{
			return get_current_index() + rh.get_current_index();
		}



		inline int operator - (const iterator &rh) const
		{
			return (int)get_current_index() - (int)rh.get_current_index();
		}
		

		
		inline bool operator > (const iterator &rh) const PLF_NOEXCEPT
		{
			return (((group_pointer == rh.group_pointer) && (element_pointer > rh.element_pointer)) || (group_pointer->group_number > rh.group_pointer->group_number));
		}

		
		
		inline bool operator < (const iterator &rh) const PLF_NOEXCEPT
		{
			return rh > *this;
		}



		inline bool operator >= (const iterator &rh) const PLF_NOEXCEPT
		{
			return !(rh > *this);
		}
		
	

		inline bool operator <= (const iterator &rh) const PLF_NOEXCEPT
		{
			return !(*this > rh);
		}



		iterator() PLF_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), erasure_field(NULL) {}

		~iterator() PLF_NOEXCEPT {}
		

		inline iterator (const iterator &source) PLF_NOEXCEPT: 
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer),
			erasure_field(source.erasure_field)
		{}




		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move constructor
			inline iterator(const iterator &&source) PLF_NOEXCEPT: // exception guarantee required for some MS compilers
				group_pointer(source.group_pointer),
				element_pointer(source.element_pointer),
				erasure_field(source.erasure_field)
			{} // Nullifications of source not necessary as no destructor
		#endif

	};

	friend class iterator;



	class reverse_iterator // Not inheriting from std::reverse_iterator because visual studio ruins everything for everyone. Such a useless piece of crap.
	{
	private:
		iterator the_iterator;
		
	public:
		struct input_iterator_tag {};
		struct forward_iterator_tag : public input_iterator_tag {};
		struct bidirectional_iterator_tag : public forward_iterator_tag {};
		struct random_access_iterator_tag : public bidirectional_iterator_tag {};

		typedef random_access_iterator_tag	iterator_category;
		typedef element_type				value_type;
		typedef std::ptrdiff_t				difference_type;
		typedef element_type *				pointer;
		typedef element_type &				reference;

		friend class colony;


		inline reverse_iterator& operator = (const reverse_iterator &source) PLF_NOEXCEPT 
		{
			the_iterator = source.the_iterator;
			return *this;
		}
		


		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move assignment
			inline reverse_iterator& operator = (const reverse_iterator &&source) PLF_NOEXCEPT 
			{
				the_iterator = std::move(source.the_iterator);
				return *this;
			}
		#endif
		


		inline bool operator == (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator == rh.the_iterator);
		}
		
		

		inline bool operator != (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator != rh.the_iterator);
		}


		
		inline element_type & operator * () const PLF_NOEXCEPT
		{
			return *(the_iterator.element_pointer);
		}
		


		inline element_type * operator -> () const PLF_NOEXCEPT
		{
			return the_iterator.element_pointer;
		}
		


		inline reverse_iterator & operator ++ () 
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL);
			
			do
			{
				if (element_pointer > group_pointer->elements) // ie. beginning of available data (ie. [0]) not reached
				{
					--element_pointer; // --erasure_field handled by loop condition
				}
				else if (group_pointer->previous_group == NULL)
				{
					if (element_pointer == group_pointer->elements) // ie. not < group_pointer->elements ie. already at rend
					{
						--element_pointer;
						--erasure_field;
					}
					
					return *this;
				}
				else
				{
					group_pointer = group_pointer->previous_group;
					element_pointer = group_pointer->end - 1;
					erasure_field = group_pointer->end_erased; // -1 handled by loop condition
				}
			} while (*--erasure_field); 

			return *this;
		}
		


		inline reverse_iterator & operator -- () 
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL); // covers uninitialised iterator && empty group
			
			do 
			{
				if (group_pointer->next_group == NULL)
				{
					if (element_pointer != group_pointer->last_endpoint - 1) // ie. end of available data not reached
					{
						++element_pointer;
						++erasure_field;
					}
					else
					{
						return *this;
					}
				}
				else if (++element_pointer == group_pointer->last_endpoint) // ie. beyond end of available data 
				{
					group_pointer = group_pointer->next_group;
					element_pointer = group_pointer->elements;
					erasure_field = group_pointer->erased - 1;
				}
			} while (*++erasure_field);
			
			return *this;
		}
		


		inline reverse_iterator & operator += (unsigned int addition)
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL);
			
			while (addition != 0)
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
						element_pointer = group_pointer->end - 1;
						erasure_field = group_pointer->end_erased;
					}
				} while (*--erasure_field);
			
				--addition;
			}

			return *this;
		}
			


		inline reverse_iterator operator + (const unsigned int increment) 
		{
			reverse_iterator return_iterator = *this;
			return return_iterator += increment;
		}



		inline reverse_iterator & operator -= (unsigned int subtraction) 
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			bool_pointer_type &erasure_field = the_iterator.erasure_field;

			assert(group_pointer != NULL); // covers uninitialised iterator && empty group
			
			while (subtraction != 0)
			{
				do
				{
					if (group_pointer->next_group == NULL)
					{
						if (element_pointer != group_pointer->last_endpoint - 1) // ie. end of available data not reached
						{
							++element_pointer;
						}
						else
						{
							return *this;
						}
					}
					else if (++element_pointer == group_pointer->last_endpoint) // ie. not beyond end of available data
					{
						group_pointer = group_pointer->next_group;
						element_pointer = group_pointer->elements;
						erasure_field = group_pointer->erased - 1;
					}
				} while(*++erasure_field);
				
				--subtraction;
			}
			
			return *this;
		}



		inline reverse_iterator operator - (const unsigned int decrement) 
		{
			reverse_iterator return_iterator = *this;
			return return_iterator -= decrement;
		}
		


		inline unsigned int operator + (const iterator &rh) const
		{
			return the_iterator.get_current_index() + rh.the_iterator.get_current_index();
		}



		inline int operator - (const iterator &rh) const
		{
			return (int)the_iterator.get_current_index() - (int)rh.the_iterator.get_current_index();
		}
		

		
		inline iterator base() const
		{
			return the_iterator + 1;
		}
		
		

		inline bool operator > (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (rh.the_iterator > the_iterator);
		}

		
		
		inline bool operator < (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return (the_iterator > rh.the_iterator);
		}
		
		

		inline bool operator >= (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return !(the_iterator > rh.the_iterator);
		}
		
		

		inline bool operator <= (const reverse_iterator &rh) const PLF_NOEXCEPT
		{
			return !(rh.the_iterator > the_iterator);
		}
		
		


		reverse_iterator() PLF_NOEXCEPT 
		{}
		
		

		~reverse_iterator() PLF_NOEXCEPT {}



		reverse_iterator (const reverse_iterator &source) PLF_NOEXCEPT: the_iterator(source.the_iterator) 
		{}



		reverse_iterator (const iterator &source) PLF_NOEXCEPT: the_iterator(source)
		{}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move constructors
			reverse_iterator (const reverse_iterator &&source) PLF_NOEXCEPT: the_iterator(std::move(source.the_iterator)) 
			{}

			reverse_iterator (const iterator &&source) PLF_NOEXCEPT: the_iterator(std::move(source))
			{}
		#endif



	};


	friend class reverse_iterator;


private:
	element_allocator_type	allocator;
	group_allocator_type	group_allocator;
	iterator_stack_type		empty_indexes;
	iterator				end_iterator, begin_iterator;
	element_pointer_type	current_end;
	group_pointer_type		first_group;
	unsigned int			total_size, max_elements_per_group;


	inline void initialize(const unsigned int elements_per_group)
	{
		first_group = group_allocator.allocate(1, 0);

		#ifdef PLF_VARIADICS_SUPPORT
			group_allocator.construct(first_group, elements_per_group); 
		#else
			group_allocator.construct(first_group, group(elements_per_group));
		#endif

		begin_iterator.group_pointer = first_group;
		begin_iterator.element_pointer = first_group->elements;
		begin_iterator.erasure_field = first_group->erased;
		end_iterator = begin_iterator;
		current_end = first_group->end;
	}



public:

	colony(): 
		total_size(0), 
		max_elements_per_group(UINT_MAX / 2)
	{
		initialize(8);
	}
	
	


	colony(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount = UINT_MAX / 2):
		empty_indexes((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8),
		total_size(0), 
		max_elements_per_group(max_allocation_amount)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount < max_elements_per_group);
		assert(max_elements_per_group <= UINT_MAX / 2);

		initialize(initial_allocation_amount);
	}



	colony (const colony &source): 
		empty_indexes(source.empty_indexes.first_group->size),
		total_size(0), 
		max_elements_per_group(source.max_elements_per_group)
	{
		assert (source.first_group != NULL); // Uninitialised/destructed source colony

		initialize(source.first_group->size);
		
		// Copy data from source:
		if (!source.empty())
		{
			add(source.begin(), source.end());
		}
	}
	


	#ifdef PLF_MOVE_SEMANTICS_SUPPORT 
		colony(colony &&source) PLF_NOEXCEPT: 
			empty_indexes(std::move(source.empty_indexes)),
			end_iterator(source.end_iterator),
			begin_iterator(source.begin_iterator),
			current_end(source.current_end), 
			first_group(source.first_group), 
			total_size(source.total_size), 
			max_elements_per_group(source.max_elements_per_group)
		{
			source.first_group = NULL; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL, to allow for clean destructor usage
			source.empty_indexes.first_group = NULL; // Ensures the destruction of the source stack doesn't remove this colony's stack groups as well
		}
	#endif
	
	

	inline const iterator & begin() const PLF_NOEXCEPT
	{
		return begin_iterator;
	}



	inline const iterator & end() const PLF_NOEXCEPT
	{
		return end_iterator;
	}
	


	inline const reverse_iterator rbegin() const
	{
		return reverse_iterator(end_iterator - 1);
	}



	inline const reverse_iterator rend() const
	{
		reverse_iterator beforebegin(begin_iterator);
		--(beforebegin.the_iterator.element_pointer); // ie. point to memory space before first element
		return beforebegin;
	}


	~colony() PLF_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_NOEXCEPT
	{
	#ifdef PLF_TRAITS_SUPPORT 
		if (total_size != 0  && !(std::is_trivially_destructible<element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructor
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
					allocator.destroy(element_pointer);
				}

				if (++element_pointer == end_pointer) // ie. not beyond end of available data
				{
					{
						previous_group = group_pointer;
						group_pointer = group_pointer->next_group;
						group_allocator.destroy(previous_group);
						group_allocator.deallocate(previous_group, 1);
					}
					
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
			total_size = 0;
			group_pointer_type previous_group;

			while (first_group != NULL)
			{
				previous_group = first_group;
				first_group = first_group->next_group;
				group_allocator.destroy(previous_group);
				group_allocator.deallocate(previous_group, 1);
			}
		}
	}



#ifdef PLF_VARIADICS_SUPPORT
	#define PLF_COLONY_ADD_MACRO_GROUP_ADD (total_size < max_elements_per_group) ? total_size : max_elements_per_group, end_iterator.group_pointer
#else
	#define PLF_COLONY_ADD_MACRO_GROUP_ADD group((total_size < max_elements_per_group) ? total_size : max_elements_per_group, end_iterator.group_pointer) // c++0x only supports copy construction
#endif


#define PLF_COLONY_ADD_MACRO(ASSIGNMENT_OBJECT) \
	if (empty_indexes.empty()) \
	{ \
		if (end_iterator.element_pointer != current_end) /* ie. not past end of group */\
		{ \
			++total_size; \
			++(end_iterator.group_pointer->last_endpoint); \
			++(end_iterator.group_pointer->total_number_of_elements); \
			const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */\
			allocator.construct(end_iterator.element_pointer++, ASSIGNMENT_OBJECT); \
			++end_iterator.erasure_field; \
			\
			return return_iterator; /* returns value before incrementation */\
		} \
		else \
		{ \
			const group_reference_type next_group = *(end_iterator.group_pointer->next_group = group_allocator.allocate(1, end_iterator.group_pointer)); \
			group_allocator.construct(&next_group, PLF_COLONY_ADD_MACRO_GROUP_ADD); \
			++total_size; \
			end_iterator.group_pointer = &next_group; \
			end_iterator.element_pointer = next_group.elements; \
			end_iterator.erasure_field = next_group.erased; \
			const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */ \
			allocator.construct(end_iterator.element_pointer++, ASSIGNMENT_OBJECT); \
			++end_iterator.erasure_field; \
			++(next_group.last_endpoint); \
			current_end = next_group.end; \
			++(next_group.total_number_of_elements); \
			return return_iterator; /* returns value before incrementation */ \
		} \
	} \
	else \
	{ \
		++total_size; \
		 \
		const iterator &new_index = empty_indexes.back(); \
		 \
		allocator.construct(new_index.element_pointer, ASSIGNMENT_OBJECT); \
		*new_index.erasure_field = false; \
		++(new_index.group_pointer->total_number_of_elements); \
		--(new_index.group_pointer->number_of_erased_elements); \
		 \
		if (new_index.group_pointer == first_group && new_index.element_pointer < begin_iterator.element_pointer)  \
		{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */\
			begin_iterator = new_index; \
		} \
		 \
		empty_indexes.pop(); /* Doesn't affect new_index as memory is not deallocated by pop nor is there a destructor for iterator */\
		 \
		return new_index; \
	}




public:


	iterator add(const element_type &element) 
	{
		PLF_COLONY_ADD_MACRO(element) // Use copy constructor
	}
	


	inline void add(const iterator &iterator1, const iterator &iterator2)
	{
		assert(iterator1 <= iterator2);

		for (iterator the_iterator = iterator1; the_iterator != iterator2; ++the_iterator)
		{
			add(*the_iterator);
		}
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Move-add
		iterator add(element_type &&element) 
		{
			PLF_COLONY_ADD_MACRO(std::move(element)) // Use move constructor
		}



		inline void add(iterator &&iterator1, iterator &&iterator2)
		{
			assert(iterator1 != iterator2);
			assert(iterator1 < iterator2);

			for (iterator the_iterator = iterator1; the_iterator != iterator2; ++the_iterator)
			{
				add(std::move(*the_iterator));
			}
		}
	#endif


	#ifdef PLF_VARIADICS_SUPPORT 
		template<typename... Arguments>
		iterator emplace(Arguments... parameters)
		{
			PLF_COLONY_ADD_MACRO(parameters...) // Use object's parameter'd constructor
		}

	#endif
	


	// add(): Semantics are the same as 'add' above but without writing any data to the element ie. for non-POD types it uses the default parameterless constructor. 
	// It is also useful for pre-c++11 syntax where variadic templates (and hence, emplace) is not available, and where an initialisation function is used separately to the constructor.

	iterator add() 
	{
		PLF_COLONY_ADD_MACRO(element_type()) // Use object's default constructor
	}
	



	// Still must retain return iterator in case of full group erasure:
	iterator erase(const iterator &the_iterator)
	{
		assert(total_size != 0);
		const group_pointer_type the_group_pointer = the_iterator.group_pointer;
		assert(the_group_pointer != NULL);
		group &the_group = *the_group_pointer;

		*the_iterator.erasure_field = true;
		allocator.destroy(the_iterator.element_pointer); // Destruct element

		--total_size;
		
		if (--(the_group.total_number_of_elements) != 0) // ie. non-empty group at this point in time, don't consolidate
		{
			++(the_group.number_of_erased_elements);
			empty_indexes.push(the_iterator);

			if (the_iterator != begin_iterator)
			{
				iterator return_iterator = the_iterator;
				return ++return_iterator;
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
				group_allocator.destroy(first_group);

				#ifdef PLF_VARIADICS_SUPPORT
					group_allocator.construct(first_group, first_size); 
				#else
					group_allocator.construct(first_group, group(first_size));
				#endif

				empty_indexes.clear();
				// Reset begin_iterator:
				begin_iterator.element_pointer = first_group->elements;
				begin_iterator.erasure_field = first_group->erased;
				current_end = first_group->end;
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

				group_allocator.destroy(the_group_pointer);
				group_allocator.deallocate(the_group_pointer, 1);
				
				// If the beginning index has been erased, find the next non-erased element:
				return *(begin_iterator.erasure_field) ? ++begin_iterator : begin_iterator;
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
				end_iterator.element_pointer = current_end = end_iterator.group_pointer->end;
				end_iterator.erasure_field = end_iterator.group_pointer->end_erased;
				return_iterator = end_iterator;
			}
			
			if (!empty_indexes.empty())
			{
				consolidate_empty_indexes(the_group_pointer);
			}

			group_allocator.destroy(the_group_pointer);
			group_allocator.deallocate(the_group_pointer, 1);

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
		typename iterator_stack_type::group_allocator_type &stack_group_allocator = empty_indexes.group_allocator;
		
		const unsigned int initial_size = empty_indexes.first_group->size;
		
		// Remove trailing stack groups (not removed in general 'pop' usage in plf::stack for performance reasons)
		if (empty_indexes.current_group->next_group != NULL) 
		{
			stack_group_pointer temp_group = empty_indexes.current_group->next_group;
			empty_indexes.current_group->next_group = NULL; // Close off chain from trailing groups

			do
			{
				const stack_group_pointer previous_group = temp_group;
				temp_group = temp_group->next_group;
				stack_group_allocator.destroy(previous_group);
				stack_group_allocator.deallocate(previous_group, 1);
			} while (temp_group != NULL);
		}

		// All groups from here onwards refer to empty_index's stack groups, not colony groups, unless stated otherwise
		const unsigned int new_group_size = (empty_indexes.total_size < 3) ? 3 : empty_indexes.total_size;
		
		stack_group_pointer current_old_group = empty_indexes.first_group, new_group = stack_group_allocator.allocate(1, empty_indexes.current_group), first_new_chain = NULL, current_new_chain = NULL;

		#ifdef PLF_VARIADICS_SUPPORT
			stack_group_allocator.construct(new_group, new_group_size, empty_indexes.current_group);
		#else
			typedef typename iterator_stack_type::group stack_group_type;
			stack_group_allocator.construct(new_group, stack_group_type(new_group_size, empty_indexes.current_group));
		#endif
		

		stack_element_pointer iterator_pointer, source_end, destination_begin = new_group->elements, the_end = NULL;
		const stack_element_pointer destination_start = destination_begin;
		unsigned int total_number_of_copies = 0, number_to_be_copied = 0;

		do // per old group of stack
		{
			source_end = (current_old_group->next_group != NULL) ? current_old_group->end + 1 : empty_indexes.current_element + 1;

			iterator_pointer = current_old_group->elements; 
			const unsigned int number_of_group_elements = (const unsigned int) (source_end - iterator_pointer);

			for (; iterator_pointer != source_end; ++iterator_pointer) // per old group element
			{
				if (iterator_pointer->group_pointer != the_group_pointer) // ie. this stack element doesn't match the group pointer, doesn't need to be removed
				{
					++number_to_be_copied;
				}
				else if (number_to_be_copied != 0)
				{
					// Memcpy fine for this even in uninitialised memory space, as iterators are just POD structs
					std::memcpy(destination_begin, iterator_pointer - number_to_be_copied, number_to_be_copied * sizeof(iterator));
					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}
			}

			if (number_to_be_copied == current_old_group->size || (number_to_be_copied == number_of_group_elements && destination_begin == destination_start))
			{
				the_end = (number_to_be_copied == current_old_group->size) ? current_old_group->end : source_end - 1;

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
					// copy remainder:
					std::memcpy(destination_begin, iterator_pointer - number_to_be_copied, number_to_be_copied * sizeof(iterator));
					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}

				// Remove old group:
				const stack_group_pointer prev_group = current_old_group;
				current_old_group = current_old_group->next_group;
				stack_group_allocator.destroy(prev_group);
				stack_group_allocator.deallocate(prev_group, 1);
			}

		} while (current_old_group != NULL);


		if (current_new_chain == NULL && destination_begin != destination_start) // No old groups remaining, some copies
		{
			new_group->previous_group = NULL;
			empty_indexes.first_group = empty_indexes.current_group = new_group;
			empty_indexes.current_element = destination_begin - 1;
			empty_indexes.end_element = new_group->end;
			empty_indexes.start_element = new_group->elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else if (destination_begin != destination_start) // Some deletions occured ie. some elements copied, some old groups remaining - current_new_chain != NULL implied by previous if
		{
			new_group->previous_group = current_new_chain;
			empty_indexes.current_group = current_new_chain->next_group = new_group;
			empty_indexes.first_group = first_new_chain;
			empty_indexes.current_element = destination_begin - 1;
			empty_indexes.end_element = new_group->end;
			empty_indexes.start_element = new_group->elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else if (current_new_chain != NULL) // No copies, some old groups - destination_begin == destination_start implied by previous if's
		{
			stack_group_allocator.destroy(new_group);
			stack_group_allocator.deallocate(new_group, 1); // Remove new group

			current_new_chain->next_group = NULL;
			empty_indexes.current_group = current_new_chain;
			empty_indexes.first_group = first_new_chain;
			empty_indexes.current_element = the_end;
			const typename iterator_stack_type::group &current_group = *current_new_chain;
			empty_indexes.end_element = current_group.end;
			empty_indexes.start_element = current_group.elements;
			empty_indexes.total_size = total_number_of_copies;
		}
		else // No elements remaining - least likely
		{
			empty_indexes.total_size = 0;
			stack_group_allocator.destroy(new_group);
			empty_indexes.first_group = empty_indexes.current_group = new_group;
			empty_indexes.initialize(initial_size);
		}
	}



public:

	const inline bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}


	
	inline unsigned int size() const PLF_NOEXCEPT
	{
		return total_size;
	}



	void reinitialize(const unsigned int initial_allocation_amount)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount < max_elements_per_group);

		destroy_all_data();

		empty_indexes.reinitialize((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8);
		initialize(initial_allocation_amount);
	}



	inline void reinitialize(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount)
	{
		assert(max_elements_per_group <= UINT_MAX / 2);

		max_elements_per_group = max_allocation_amount;
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
		max_elements_per_group = source.max_elements_per_group;
		initialize(source.first_group->size);
		empty_indexes.reinitialize(source.empty_indexes.first_group->size);
		
		// Copy data from source:
		if (!source.empty())
		{
			add(source.begin(), source.end());
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
			std::memcpy(this, &source, sizeof(colony<element_type, element_allocator_type>));

			source.first_group = NULL; // Nullifying the other data members is unnecessary - first_group NULL required for clean destructor usage
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
			if (*rh_iterator != *the_iterator)
			{
				return false;
			}
			
			++rh_iterator;
		}
		
		return true;
	}




	inline bool operator != (const colony &rh) const PLF_NOEXCEPT
	{
		return !(*this == rh);
	}


};

}


#endif // plf_colony_H
