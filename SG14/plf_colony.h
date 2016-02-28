// Copyright (c) 2015, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef PLF_COLONY_H
#define PLF_COLONY_H


#include <cstring>	// memset, memcpy
#include <cassert>	// assert
#include <climits>	// USHRT_MAX - could differ between platforms, but at least 65535
#include <memory>	// std::uninitialized_copy, std::allocator
#include <iterator>	// typedef inheritance for iterators

#include "plf_stack.h" // Includes plf::stack and compiler-specific Macro defines


#ifdef PLF_TYPE_TRAITS_SUPPORT
    #include <type_traits> // std::is_trivially_destructible, etc
#endif

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif


namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class colony : private element_allocator_type  // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type										value_type;
	typedef element_allocator_type								allocator_type;
	typedef typename element_allocator_type::reference			reference;
	typedef typename element_allocator_type::const_reference	const_reference;
	typedef typename element_allocator_type::difference_type	difference_type;
	typedef typename element_allocator_type::size_type			size_type;
	typedef typename element_allocator_type::pointer			pointer;
	typedef typename element_allocator_type::const_pointer		const_pointer;

	// Iterator declarations:
	template <class colony_allocator_type, bool is_const> class colony_iterator;
	typedef colony_iterator<element_allocator_type, false> iterator;
	typedef colony_iterator<element_allocator_type, true> const_iterator;
	friend class colony_iterator<element_allocator_type, false>;
	friend class colony_iterator<element_allocator_type, true>;

	template <class r_colony_allocator_type, bool r_is_const> class colony_reverse_iterator;
	typedef colony_reverse_iterator<element_allocator_type, false> reverse_iterator;
	typedef colony_reverse_iterator<element_allocator_type, true> const_reverse_iterator;
	friend class colony_reverse_iterator<element_allocator_type, false>;
	friend class colony_reverse_iterator<element_allocator_type, true>;

private:
	struct group; // forward declaration for typedefs below
	typedef typename element_allocator_type::template rebind<group>::other				group_allocator_type;
	typedef typename element_allocator_type::template rebind<unsigned short>::other		ushort_allocator_type; // Using 16-bit integer in best-case scenario, > 16-bit integer in case where platform doesn't support 16-bit types
	typedef typename element_allocator_type::template rebind<unsigned char>::other		uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas, for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.
	typedef typename element_allocator_type::template rebind<iterator>::other			iterator_allocator_type;
	typedef typename plf::stack<iterator, iterator_allocator_type> iterator_stack_type;
	typedef typename element_allocator_type::pointer 	element_pointer_type;
	typedef typename group_allocator_type::pointer 		group_pointer_type;
	typedef typename group_allocator_type::reference 	group_reference_type;
	typedef typename ushort_allocator_type::pointer 	ushort_pointer_type;
	typedef typename uchar_allocator_type::pointer		uchar_pointer_type;
	typedef typename iterator_allocator_type::pointer 	iterator_pointer_type;


	struct group : private uchar_allocator_type  // Empty base class optimisation - inheriting allocator functions
	{
		element_pointer_type		last_endpoint; // the address that is one past the highest cell number that's been used so far in this group - does not change with erase command
		group_pointer_type			next_group;
		const element_pointer_type	elements;
		const ushort_pointer_type	skipfield; // Now that both the elements and skipfield arrays are allocated contiguously, skipfield pointer also functions as a 'one-past-end' pointer for the elements array
		group_pointer_type			previous_group;
		size_type					group_number; // Used for comparison (> < >= <=) iterator operators
		unsigned short				number_of_elements; // indicates total number of used cells - changes with insert and erase commands - used to check for empty group in erase function, as indication to remove group
		const unsigned short		size; // The number of elements this particular group can house


		#ifdef PLF_VARIADICS_SUPPORT
			group(const unsigned short elements_per_group, group * const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(element_type))) + ((elements_per_group + 1) * sizeof(unsigned short))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL),
				elements(last_endpoint),
				skipfield(reinterpret_cast<ushort_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				number_of_elements(0),
				size(elements_per_group)
			{
				std::memset(&*skipfield, false, sizeof(unsigned short) * (static_cast<unsigned int>(size) + 1)); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
			}


			~group() PLF_NOEXCEPT
			{
				PLF_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), ((size * sizeof(element_type)) + ((static_cast<unsigned int>(size) + 1) * sizeof(unsigned short))));
			}

		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++0x and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor:
			group(const unsigned short elements_per_group, group * const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(element_type))) + ((elements_per_group + 1) * sizeof(unsigned short))), (previous == NULL) ? 0 : previous->elements))),
				elements(NULL), // unique destructor condition
				skipfield(reinterpret_cast<ushort_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				number_of_elements(0),
				size(elements_per_group)
			{
				std::memset(&*skipfield, false, sizeof(unsigned short) * (static_cast<unsigned int>(size) + 1));
			}
			

			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++0x for reasons stated above.
			group(const group &source) PLF_NOEXCEPT:
				last_endpoint(source.last_endpoint), 
				next_group(NULL),
				elements(source.last_endpoint),
				skipfield(source.skipfield),
				previous_group(source.previous_group),
				group_number(source.group_number),
				number_of_elements(0),
				size(source.size)
			{}


			~group() PLF_NOEXCEPT
			{
				if (elements != NULL) // NULL check necessary for correct destruction of pseudoconstructed groups as per above
				{
					PLF_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), ((static_cast<unsigned int>(size) * (sizeof(element_type))) + ((static_cast<unsigned int>(size) + 1) * sizeof(unsigned short))));
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


	// Iterators:
	template <class colony_allocator_type, bool is_const> class colony_iterator : public std::iterator<std::bidirectional_iterator_tag, typename colony::value_type>
	{
	private:
		group_pointer_type		group_pointer;
		element_pointer_type	element_pointer;
		ushort_pointer_type		skipfield_pointer;

	public:
		typedef typename choose<is_const, typename colony_allocator_type::const_reference, typename colony_allocator_type::reference>::type	reference;
		typedef typename choose<is_const, typename colony_allocator_type::const_pointer, typename colony_allocator_type::pointer>::type		pointer;
		
		friend class colony;
		template <class r_colony_allocator_type, bool r_is_const> friend class colony_reverse_iterator;

		template <class forward_iterator_type, class distance_type>
		friend void advance_implementation(forward_iterator_type &it, distance_type distance);

		template <class iterator_type>
		friend typename std::iterator_traits<iterator_type>::difference_type distance_implementation(const iterator_type &first, const iterator_type &last) PLF_NOEXCEPT;


		inline colony_iterator & operator = (const colony_iterator &source) PLF_NOEXCEPT
		{
			assert (&source != this);

			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			skipfield_pointer = source.skipfield_pointer;

			return *this;
		}



		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// Move assignment
			inline colony_iterator & operator = (const colony_iterator &&source) PLF_NOEXCEPT // Move is a copy in this scenario
			{
				assert (&source != this);

				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				skipfield_pointer = std::move(source.skipfield_pointer);

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

			#if defined (_MSC_VER) // this version faster in MSVC
				element_pointer += 1 + *(++skipfield_pointer);

				if (element_pointer != group_pointer->last_endpoint) // ie. beyond end of available data
				{
					skipfield_pointer += *skipfield_pointer;
				}
				else if (group_pointer->next_group != NULL)
				{
					group_pointer = group_pointer->next_group;
					skipfield_pointer = group_pointer->skipfield;
					element_pointer = group_pointer->elements + *skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;
				}
			#else
				element_pointer += 1 + *(++skipfield_pointer);
				skipfield_pointer += *skipfield_pointer;

				#if defined(__GNUC__) && defined(__x86_64__) // this version compiles to faster code under gcc x64
					if (group_pointer->next_group != NULL && element_pointer == group_pointer->last_endpoint)
				#else // standard version - performs better on most compilers
					if (element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
				#endif
				{
					group_pointer = group_pointer->next_group;
					skipfield_pointer = group_pointer->skipfield;
					element_pointer = group_pointer->elements + *skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;
				}
			#endif

			return *this;
		}



	private:
		inline colony_iterator & check_end_of_group_and_progress() // used by erase
		{
			#if defined(__GNUC__) && defined(__x86_64__) // For some reason, this version compiles to faster code under gcc x64
				if (group_pointer->next_group != NULL && element_pointer == group_pointer->last_endpoint)
			#else // Standard version - performs better on most compilers
				if (element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
			#endif
			{
				group_pointer = group_pointer->next_group;
				skipfield_pointer = group_pointer->skipfield;
				element_pointer = group_pointer->elements + *skipfield_pointer;
				skipfield_pointer += *skipfield_pointer;
			}
			
			return *this;
		}


	public:
		inline colony_iterator operator ++(int)
		{
			colony_iterator copy(*this);
			++*this;
			return copy;
		}



		colony_iterator & operator -- ()
		{
			assert(group_pointer != NULL);
			assert(!(element_pointer == group_pointer->elements && group_pointer->previous_group == NULL)); // Assert that we are not already at the beginning on the colony

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group 
			{
				element_pointer -= 1 + *(--skipfield_pointer);
				skipfield_pointer -= *skipfield_pointer;

				if (element_pointer != group_pointer->elements - 1) // ie. iterator was not already at beginning of colony (with some previous consecutive deleted elements), and skipfield does not takes us into the previous group)
				{
					return *this;
				}
			}

			group_pointer = group_pointer->previous_group;
			skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
			element_pointer = (reinterpret_cast<element_pointer_type>(group_pointer->skipfield) - 1) - *skipfield_pointer;
			skipfield_pointer -= *skipfield_pointer;

			return *this;
		}



		inline colony_iterator operator -- (int)
		{
			colony_iterator copy(*this);
			--*this;
			return copy;
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



		colony_iterator() PLF_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), skipfield_pointer(NULL) {}

		~colony_iterator() PLF_NOEXCEPT {}


		inline colony_iterator (const colony_iterator &source) PLF_NOEXCEPT:
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer),
			skipfield_pointer(source.skipfield_pointer)
		{}




		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			// move constructor
			inline colony_iterator(const colony_iterator &&source) PLF_NOEXCEPT:
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer)),
				skipfield_pointer(std::move(source.skipfield_pointer))
			{}
		#endif


	};




	
	// Reverse iterators:

	template <class r_colony_allocator_type, bool r_is_const> class colony_reverse_iterator : public std::iterator<std::bidirectional_iterator_tag, typename colony::value_type>
	{
	private:
		typename colony::iterator the_iterator;

	public:
		typedef typename choose<r_is_const, typename r_colony_allocator_type::const_reference, typename r_colony_allocator_type::reference>::type	reference;
		typedef typename choose<r_is_const, typename r_colony_allocator_type::const_pointer, typename r_colony_allocator_type::pointer>::type		pointer;

		friend class colony;

		template <class reverse_iterator_type>
		friend typename std::iterator_traits<reverse_iterator_type>::difference_type reverse_distance_implementation(const reverse_iterator_type &first, const reverse_iterator_type &last) PLF_NOEXCEPT;

		template <class reverse_iterator_type, class distance_type>
		friend void reverse_advance_implementation(reverse_iterator_type &it, distance_type distance);


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
		


		// Need to redefine rather than using internal iterator's -- operator, in order for ++ to be able to reach rend() ie. elements[-1]
		colony_reverse_iterator & operator ++ () 
		{
			group_pointer_type &group_pointer = the_iterator.group_pointer;
			element_pointer_type &element_pointer = the_iterator.element_pointer;
			ushort_pointer_type &skipfield_pointer = the_iterator.skipfield_pointer;

			assert(group_pointer != NULL);

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				element_pointer -= 1 + *(--skipfield_pointer);
				skipfield_pointer -= *skipfield_pointer;

				if (element_pointer != group_pointer->elements - 1) // ie. iterator was not already at beginning of colony, and skipfield does not takes us into the previous group)
				{
					return *this;
				}
			}
			
			if (group_pointer->previous_group != NULL)
			{
				group_pointer = group_pointer->previous_group;
				skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
				element_pointer = (reinterpret_cast<element_pointer_type>(group_pointer->skipfield) - 1) - *skipfield_pointer;
				skipfield_pointer -= *skipfield_pointer;
			}
			else // necessary so that reverse_iterator can end up == rend() ie. first_group->elements[-1]
			{
				--element_pointer;
				--skipfield_pointer;
			}

			return *this;
		}



		inline colony_reverse_iterator operator ++ (int)
		{
			colony_reverse_iterator copy(*this);
			++*this;
			return copy;
		}


		inline colony_reverse_iterator & operator -- ()
		{
			++the_iterator;
			return *this;
		}
		


		inline colony_reverse_iterator operator -- (int)
		{
			colony_reverse_iterator copy(*this);
			--*this;
			return copy;
		}



		inline typename colony::iterator base() const
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
	size_type				total_number_of_elements;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		unsigned short max_elements_per_group;
		ebco_pair(const unsigned short max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;
	iterator_stack_type		empty_indexes;


	void initialize(const unsigned short elements_per_group)
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
		begin_iterator.skipfield_pointer = first_group->skipfield;
		end_iterator = begin_iterator;
	}



public:

	colony():
		total_number_of_elements(0),
		group_allocator_pair(USHRT_MAX)
	{
		initialize(8);
	}




	colony(const unsigned short initial_allocation_amount, const unsigned short max_allocation_amount = USHRT_MAX):
		total_number_of_elements(0),
		group_allocator_pair(max_allocation_amount),
		empty_indexes((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount <= group_allocator_pair.max_elements_per_group);
		assert(max_allocation_amount <= element_allocator_type::max_size() / 2); // Only important if for some reason max_size / 2 < 65535. /2 because otherwise an allocation could overflow

		initialize(initial_allocation_amount);
	}



	colony (const colony &source):
		total_number_of_elements(0),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
		empty_indexes(static_cast<const size_type>((source.empty_indexes.first_group->end + 1) - source.empty_indexes.first_group->elements))
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
			total_number_of_elements(source.total_number_of_elements),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
			empty_indexes(std::move(source.empty_indexes))
		{
			source.first_group = NULL;
			source.total_number_of_elements = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_number_of_elements 0, to allow for clean destructor usage
		}
	#endif



	inline iterator & begin() PLF_NOEXCEPT
	{
		return begin_iterator;
	}


	inline const iterator & begin() const PLF_NOEXCEPT // To allow for functions which only take const colony & as a source ie. copy constructor
	{
		return begin_iterator;
	}



	inline iterator & end() PLF_NOEXCEPT
	{
		return end_iterator;
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
		return_iterator.skipfield_pointer = begin_iterator.skipfield_pointer;

		return return_iterator;
	}



	inline const_iterator cend() const PLF_NOEXCEPT
	{
		const_iterator return_iterator;

		return_iterator.group_pointer = end_iterator.group_pointer;
		return_iterator.element_pointer = end_iterator.element_pointer;
		return_iterator.skipfield_pointer = end_iterator.skipfield_pointer;

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
		--(beforebegin.the_iterator.skipfield_pointer); // ie. point to memory space before first erasure field
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
		--(beforebegin.the_iterator.skipfield_pointer); // ie. point to memory space before first erasure field
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
		if (total_number_of_elements != 0	&& !(std::is_trivially_destructible<element_type>::value)) 
	#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise the 'destruct' loop out for POD types
		if (total_number_of_elements != 0)
	#endif
		{
			total_number_of_elements = 0;
			first_group = NULL;

			group_pointer_type previous_group, group_pointer = begin_iterator.group_pointer;
			element_pointer_type element_pointer = begin_iterator.element_pointer, end_pointer = group_pointer->last_endpoint;
			ushort_pointer_type skipfield_pointer = begin_iterator.skipfield_pointer;

			while (true)
			{
				PLF_DESTROY(element_allocator_type, (*this), element_pointer);

				element_pointer += 1 + *(++skipfield_pointer);
				skipfield_pointer += *skipfield_pointer;

				if (element_pointer == end_pointer) // ie. beyond end of available data
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
					skipfield_pointer = group_pointer->skipfield;
					element_pointer = group_pointer->elements + *skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;
				}
			}
		}
		else // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructors
		{
			// Although technically under a type-traits-supporting compiler total_number_of_elements could be non-zero at this point, since first_group would already be NULL in the case of double-destruction, it's unnecessary to zero total_number_of_elements, and for some reason doing so creates a performance regression under gcc x64 (5.1 and below)
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
			if (end_iterator.element_pointer != reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)) /* ie. not past end of group */
			{
				const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
				PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, element);

				++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
				++end_iterator.skipfield_pointer;
				++(end_iterator.group_pointer->last_endpoint);
				++(end_iterator.group_pointer->number_of_elements);
				++total_number_of_elements;
				return return_iterator; /* returns value before incrementation */
			}
			else
			{
				end_iterator.group_pointer->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
				const group_reference_type next_group = *(end_iterator.group_pointer->next_group);

				try
				{
					#ifdef PLF_VARIADICS_SUPPORT
						PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const unsigned short>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
					#else // c++0x only supports copy construction
						PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const unsigned short>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
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
				end_iterator.skipfield_pointer = next_group.skipfield;
				const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

				try
				{
					PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
				}
				catch (...)
				{
					end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
					end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
					end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
					PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
					end_iterator.group_pointer->next_group = NULL;
					throw;
				}

				++end_iterator.skipfield_pointer;
				++(next_group.last_endpoint);
				++(next_group.number_of_elements);
				++total_number_of_elements;
				return return_iterator; /* returns value before incrementation */
			}
		}
		else
		{
			const iterator & new_index = empty_indexes.top();
			PLF_CONSTRUCT(element_allocator_type, (*this), new_index.element_pointer, element);

			++(new_index.group_pointer->number_of_elements);

			if (new_index.group_pointer == first_group && new_index.element_pointer < begin_iterator.element_pointer)
			{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
				begin_iterator = new_index;
			}

			empty_indexes.pop(); /* Doesn't affect new_index as memory is not deallocated by pop nor is there a destructor for iterator */
			++total_number_of_elements;

			// Pseudocode:
			// if !empty_indexes.empty(), check whether location we are restoring to has a slot before or after which is erased
			// if it has only a slot before which is erased (ie. at end of erasure block), update the prime erasure point
			// if it only has a slot after it which is erased, (ie. this is the prime erasure point), change next slot to prime erasure point and update all subsequent erasure points (ie. decrement by 1)
			// if it has both a slot before and after which is erased (ie. is in middle of erasure block), do both of the above
			const bool prev_skipfield = (new_index.skipfield_pointer != new_index.group_pointer->skipfield && *(new_index.skipfield_pointer - 1) != 0);
			const bool after_skipfield = (*(new_index.skipfield_pointer + 1) != 0); // NOTE: first test removed due to extra slot in skipfield (compared to element field)

			if (prev_skipfield && !after_skipfield) // previous erased consecutive elements, none following
			{
				if (*(new_index.skipfield_pointer) == 2) // ie. previous erased point was a duo, now will be solo
				{
					*(new_index.skipfield_pointer - 1) = 1;
				}
				else // Multiple prior erased points
				{
					*(new_index.skipfield_pointer - (*(new_index.skipfield_pointer) - 1)) = *(new_index.skipfield_pointer) - 1;
				}
			}
			else if (!prev_skipfield && after_skipfield) // No previous consecutive erased points, at least one following ie. this was the prime erasure point
			{
				*(new_index.skipfield_pointer + 1) = *new_index.skipfield_pointer - 1;
				ushort_pointer_type following = new_index.skipfield_pointer + 2;
				
				while (*following != 0) // note: end of skipfield covered by extra skipfield field (always 0)
				{
					--(*(following++)); // Decrement the value of *following, then increment the value of following.
				}
			}
			else if (prev_skipfield && after_skipfield) // ie. (prev_skipfield && after_skipfield)
			{
				// Change skipfield_pointers before:
				unsigned short prime_erasure_point_number;

				if (*(new_index.skipfield_pointer) == 2) // ie. only one previous element
				{
					prime_erasure_point_number = *(new_index.skipfield_pointer - 1); // Save this for use in changing skipfield_pointers after
					*(new_index.skipfield_pointer - 1) = 1; // is now solo
				}
				else // Multiple prior erased points
				{
					const unsigned short distance = *(new_index.skipfield_pointer) - 1; // distance to prime erasure point
					prime_erasure_point_number = *(new_index.skipfield_pointer - distance);
					*(new_index.skipfield_pointer - distance) = distance; // Split the erasure series
				}


				// Change skipfield_pointers after:
				*(new_index.skipfield_pointer + 1) = prime_erasure_point_number - *new_index.skipfield_pointer;
				ushort_pointer_type following = new_index.skipfield_pointer + 2;
				unsigned short count = 2;

				while (*following != 0) // note: end of skipfield covered by extra skipfield field (always 0)
				{
					*(following++) = count++; // Create new series
				}
			}

			*new_index.skipfield_pointer = 0;


			return new_index;
		}
	}
	


	// Group insert:
	inline void insert(const iterator &iterator1, const iterator &iterator2)
	{
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
				if (end_iterator.element_pointer != reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)) /* ie. not past end of group */
				{
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
					PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, std::move(element));

					++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
					++end_iterator.skipfield_pointer;
					++(end_iterator.group_pointer->last_endpoint);
					++(end_iterator.group_pointer->number_of_elements);
					++total_number_of_elements;
					return return_iterator; /* returns value before incrementation */
				}
				else
				{
					end_iterator.group_pointer->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
					const group_reference_type next_group = *(end_iterator.group_pointer->next_group);

					try
					{
						#ifdef PLF_VARIADICS_SUPPORT
							PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const unsigned short>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
						#else // c++0x only supports copy construction
							PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const unsigned short>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
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
					end_iterator.skipfield_pointer = next_group.skipfield;
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

					try
					{
						PLF_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, std::move(element));
					}
					catch (...)
					{
						end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
						end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
						end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
						PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					++end_iterator.skipfield_pointer;
					++(next_group.last_endpoint);
					++(next_group.number_of_elements);
					++total_number_of_elements;
					return return_iterator; /* returns value before incrementation */
				}
			}
			else
			{
				const iterator & new_index = empty_indexes.top();
				PLF_CONSTRUCT(element_allocator_type, (*this), new_index.element_pointer, std::move(element));

				++(new_index.group_pointer->number_of_elements);

				if (new_index.group_pointer == first_group && new_index.element_pointer < begin_iterator.element_pointer)
				{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
					begin_iterator = new_index;
				}

				empty_indexes.pop(); /* Doesn't affect new_index as memory is not deallocated by pop nor is there a destructor for iterator */
				++total_number_of_elements;

				// Pseudocode:
				// if !empty_indexes.empty(), check whether location we are restoring to has a slot before or after which is erased
				// if it has only a slot before which is erased (ie. at end of erasure block), update the prime erasure point
				// if it only has a slot after it which is erased, (ie. this is the prime erasure point), change next slot to prime erasure point and update all subsequent erasure points (ie. decrement by 1)
				// if it has both a slot before and after which is erased (ie. is in middle of erasure block), do both of the above
				const bool prev_skipfield = (new_index.skipfield_pointer != new_index.group_pointer->skipfield && *(new_index.skipfield_pointer - 1) != 0);
				const bool after_skipfield = (*(new_index.skipfield_pointer + 1) != 0);


				if (prev_skipfield && !after_skipfield) // previous erased consecutive elements, none following
				{
					if (*(new_index.skipfield_pointer) == 2) // ie. previous erased point was a duo, now will be solo
					{
						*(new_index.skipfield_pointer - 1) = 1;
					}
					else // Multiple prior erased points
					{
						*(new_index.skipfield_pointer - (*(new_index.skipfield_pointer) - 1)) = *(new_index.skipfield_pointer) - 1;
					}
				}
				else if (!prev_skipfield && after_skipfield) // No previous consecutive erased points, at least one following ie. this was the prime erasure point
				{
					*(new_index.skipfield_pointer + 1) = *new_index.skipfield_pointer - 1;
					ushort_pointer_type following = new_index.skipfield_pointer + 2;

					while (*following != 0) // end of skipfield covered by extra skipfield field (always 0)
					{
						--(*(following++)); // Decrement the value of *following, then increment the value of following.
					}
				}
				else if (prev_skipfield && after_skipfield) // ie. (prev_skipfield && after_skipfield)
				{
					// Change skipfield_pointers before:
					unsigned short prime_erasure_point_number;
					
					if (*(new_index.skipfield_pointer) == 2) // ie. only one previous element
					{
						prime_erasure_point_number = *(new_index.skipfield_pointer - 1); // Save this for use in changing skipfield_pointers after 
						*(new_index.skipfield_pointer - 1) = 1; // is now solo
					}
					else // Multiple prior skipfield points
					{
						const unsigned short distance = *(new_index.skipfield_pointer) - 1; // distance to prime erasure point
						prime_erasure_point_number = *(new_index.skipfield_pointer - distance);
						*(new_index.skipfield_pointer - distance) = distance; // Split the erasure series
					}


					// Change skipfield_pointers after:
					*(new_index.skipfield_pointer + 1) = prime_erasure_point_number - *new_index.skipfield_pointer;
					ushort_pointer_type following = new_index.skipfield_pointer + 2;
					unsigned short count = 2;
					
					while (*following != 0) // end of skipfield covered by extra skipfield field (always 0)
					{
						*(following++) = count++; // Create new series
					}
				}

				*new_index.skipfield_pointer = 0;

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




private:

	void consolidate_empty_indexes(const group_pointer_type the_group_pointer)
	{ 	
		// Remove all entries containing the provided colony group, from the empty_indexes stack, also consolidate the stack removing unused trailing groups in the process. 
		
		// Code explanation:
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

		const size_type initial_size = static_cast<const size_type>((empty_indexes.first_group->end + 1) - empty_indexes.first_group->elements);

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
		const size_type new_group_size = (empty_indexes.total_number_of_elements < 3) ? 3 : empty_indexes.total_number_of_elements;

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
		size_type total_number_of_copies = 0, number_to_be_copied = 0;

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
			const size_type number_of_group_elements = static_cast<const size_type>(source_end - iterator_pointer);

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
							typedef typename iterator_stack_type::allocator_type stack_allocator_type;
						#endif

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_DESTROY(stack_allocator_type, empty_indexes, element_pointer);
						}
					}

					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}
			}

			const bool test = (number_to_be_copied == static_cast<const size_type>((current_old_group->end + 1) - current_old_group->elements));

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
							typedef typename iterator_stack_type::allocator_type stack_allocator_type;
						#endif

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_DESTROY(stack_allocator_type, empty_indexes, element_pointer);
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
			empty_indexes.total_number_of_elements = total_number_of_copies;
		}
		else if (destination_begin != destination_start) // Some deletions occured ie. some elements copied, some old groups remaining - current_new_chain != NULL implied by previous if
		{
			new_group->previous_group = current_new_chain;
			empty_indexes.current_group = current_new_chain->next_group = new_group;
			empty_indexes.first_group = first_new_chain;
			empty_indexes.current_element = destination_begin - 1;
			empty_indexes.start_element = new_group->elements;
			empty_indexes.total_number_of_elements = total_number_of_copies;
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
			empty_indexes.total_number_of_elements = total_number_of_copies;
		}
		else // No elements remaining - least likely
		{
			empty_indexes.total_number_of_elements = 0;
			PLF_DESTROY(stack_group_allocator_type, stack_group_allocator, new_group); // New construction will occur within initialise function below
			empty_indexes.first_group = empty_indexes.current_group = new_group;
			empty_indexes.initialize(initial_size);
		}
	}



	inline void update_subsequent_group_numbers(group_pointer_type temp_group)
	{
		while (temp_group != NULL)
		{
			--(temp_group->group_number);
			temp_group = temp_group->next_group;
		}
	}



public:

	// Still must retain return iterator in case of full group erasure:
	iterator erase(const iterator &the_iterator)
	{
		assert(total_number_of_elements != 0);
		const group_pointer_type the_group_pointer = the_iterator.group_pointer;
		assert(the_group_pointer != NULL);
		assert(the_iterator.element_pointer != the_group_pointer->last_endpoint);
		group &the_group = *the_group_pointer;

		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if (!(std::is_trivially_destructible<element_type>::value)) // This if-statement should be removed by the compiler on resolution of element_type. For some optimizing compilers this step won't be necessary (for MSVC 2013 it makes a difference)
		#endif
		{
			PLF_DESTROY(element_allocator_type, (*this), the_iterator.element_pointer); // Destruct element
		}

		--total_number_of_elements;

		if (the_group.number_of_elements-- != 1) // ie. non-empty group at this point in time, don't consolidate
		{
			empty_indexes.push(the_iterator);

			// Pseudocode:
			// If slot has no erased slots on either side, continue as usual
			// If slot has erased slot before it, add 1 to prev slot no. and use as skipfield_pointer. Update prime erasure point
			// If slot has erased slot after it but none before it, make this slot prime erasure slot and update subsequent slots
			// If slot has erasure slot before it and after it, remove secondary prime erasure slot and update all slots after this point

			const bool prev_skipfield = (the_iterator.skipfield_pointer != the_iterator.group_pointer->skipfield && *(the_iterator.skipfield_pointer - 1) != 0);
			const bool after_skipfield = (the_iterator.element_pointer + 1 != the_iterator.group_pointer->last_endpoint && *(the_iterator.skipfield_pointer + 1) != 0);
			
			iterator return_iterator;

			if (!prev_skipfield && !after_skipfield) // no consecutive erased elements
			{
				*the_iterator.skipfield_pointer = 1; // solo erase point

				return_iterator.group_pointer = the_iterator.group_pointer;
				return_iterator.element_pointer = the_iterator.element_pointer + 1;
				return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
				return_iterator.check_end_of_group_and_progress();
			}
			else if (prev_skipfield && !after_skipfield) // previous erased consecutive elements, none following
			{
				if (*(the_iterator.skipfield_pointer - 1) == 1) // ie. previous erased point is solo
				{
					*(the_iterator.skipfield_pointer - 1) = 2;
					*the_iterator.skipfield_pointer = 2;
				}
				else // Multiple prior erased points
				{
					++(*(the_iterator.skipfield_pointer - *(the_iterator.skipfield_pointer - 1)));
					*the_iterator.skipfield_pointer = *(the_iterator.skipfield_pointer - 1) + 1;
				}

				return_iterator.group_pointer = the_iterator.group_pointer;
				return_iterator.element_pointer = the_iterator.element_pointer + 1;
				return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
				return_iterator.check_end_of_group_and_progress();
			}
			else if (!prev_skipfield && after_skipfield) // No previous consecutive erased points, at least one following
			{
				*the_iterator.skipfield_pointer = *(the_iterator.skipfield_pointer + 1) + 1;
				*(the_iterator.skipfield_pointer + 1) = 2;
				ushort_pointer_type following = the_iterator.skipfield_pointer + 2;

				while (*following != 0) // end of skipfield covered by extra skipfield field (always 0)
				{
					++(*(following++)); // Increment the value of *following, then increment the value of following.
				}

				return_iterator.group_pointer = the_iterator.group_pointer;
				return_iterator.element_pointer = the_iterator.element_pointer + (following - the_iterator.skipfield_pointer);
				return_iterator.skipfield_pointer = following;
				return_iterator.check_end_of_group_and_progress();
			}
			else // ie. (prev_skipfield && after_skipfield)
			{
				if (*(the_iterator.skipfield_pointer - 1) == 1) // ie. previous erased point is solo
				{
					*(the_iterator.skipfield_pointer - 1) = *(the_iterator.skipfield_pointer + 1) + 2;
					*the_iterator.skipfield_pointer = 2;
				}
				else // Multiple prior erased points
				{
					*(the_iterator.skipfield_pointer - *(the_iterator.skipfield_pointer - 1)) += 1 + *(the_iterator.skipfield_pointer + 1); // Add 1 + the value of the next prime erasure point to the first prime erasure point
					*the_iterator.skipfield_pointer = *(the_iterator.skipfield_pointer - 1) + 1;
				}

				unsigned short count = *the_iterator.skipfield_pointer;
				ushort_pointer_type following = the_iterator.skipfield_pointer + 1;

				while (*following != 0) // end of skipfield covered by extra skipfield field (always 0)
				{
					*(following++) = ++count; // Make *following = ++count, then increment the value of following.
				}

				return_iterator.group_pointer = the_iterator.group_pointer;
				return_iterator.element_pointer = the_iterator.element_pointer + (following - the_iterator.skipfield_pointer);
				return_iterator.skipfield_pointer = following;
				return_iterator.check_end_of_group_and_progress();
			}


			if (the_iterator == begin_iterator) // If original iterator was first element in colony, update it's value with the next non-skipfield element:
			{
				begin_iterator = return_iterator;
			}

			return return_iterator;
		}

		// else: consolidation of empty groups
		if (the_group_pointer == first_group)
		{
			if (the_group.next_group == NULL) // ie. only group in colony
			{
				const unsigned short first_size = first_group->size;
				PLF_DESTROY(group_allocator_type, group_allocator_pair, first_group);

				#ifdef PLF_VARIADICS_SUPPORT
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, first_size);
				#else
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(first_size));
				#endif

				empty_indexes.clear();
				// Reset begin_iterator:
				begin_iterator.element_pointer = first_group->elements;
				begin_iterator.skipfield_pointer = first_group->skipfield;
				end_iterator = begin_iterator;

				return end_iterator;
			}
			else // Remove first group, change first group to next group
			{
				the_group.next_group->previous_group = NULL; // Cut off this group from the chain
				first_group = the_group.next_group; // Make the next group the first group

				// Update group numbers:
				first_group->group_number = 0;
				update_subsequent_group_numbers(first_group->next_group);

				if (!empty_indexes.empty())
				{
					consolidate_empty_indexes(the_group_pointer);
				}

				PLF_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				begin_iterator.group_pointer = first_group; // note: end iterator only needs to be changed if the deleted group was the final group in the chain
				begin_iterator.element_pointer = first_group->elements;
				begin_iterator.skipfield_pointer = first_group->skipfield;

				// If the beginning index has been erased (ie. != 0), skip to next non-erased element:
				begin_iterator.element_pointer += *(begin_iterator.skipfield_pointer);
				begin_iterator.skipfield_pointer += *(begin_iterator.skipfield_pointer);

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
				return_iterator.skipfield_pointer = return_iterator.group_pointer->skipfield;

				// If first element of next group is erased (ie. skipfield != 0), skip to the next non-erased element:
				return_iterator.element_pointer += *(return_iterator.skipfield_pointer);
				return_iterator.skipfield_pointer += *(return_iterator.skipfield_pointer);

				// Update group numbers:
				update_subsequent_group_numbers(the_group.next_group);
			}
			else // This is the final group in the chain
			{
				the_group.previous_group->next_group = NULL;
				end_iterator.group_pointer = the_group.previous_group; // end iterator only needs to be changed if this is the final group in the chain
				end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
			 	end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
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



	inline bool empty() const PLF_NOEXCEPT
	{
		return total_number_of_elements == 0;
	}



	inline size_type size() const PLF_NOEXCEPT
	{
		return total_number_of_elements;
	}



	void reinitialize(const unsigned short initial_allocation_amount)
	{
		assert(initial_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(initial_allocation_amount <= group_allocator_pair.max_elements_per_group);

		destroy_all_data();
		total_number_of_elements = 0;

		empty_indexes.reinitialize((initial_allocation_amount < 8) ? initial_allocation_amount : (initial_allocation_amount >> 7) + 8);
		initialize(initial_allocation_amount);
	}



	inline void reinitialize(const unsigned short initial_allocation_amount, const unsigned short max_allocation_amount)
	{
		assert(max_allocation_amount <= UINT_MAX / 2); // Only important if for some reason max_size / 2 < 65535

		group_allocator_pair.max_elements_per_group = max_allocation_amount;
		reinitialize(initial_allocation_amount);
	}



	inline void clear()
	{
		reinitialize(first_group->size); // Note: May not be original size of colony- if all original first group elements were erased the group will have been removed and the next group becomes the 'first_group'
	}



	colony & operator = (const colony &source)
	{
		assert (&source != this);
		assert (source.first_group != NULL); // ie. data has been destroyed on this element, possibly result of std::move

		destroy_all_data();

		total_number_of_elements = source.total_number_of_elements;
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
			total_number_of_elements = source.total_number_of_elements;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
			empty_indexes = std::move(source.empty_indexes);

			source.first_group = NULL; 
			source.total_number_of_elements = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_number_of_elements 0, to allow for clean destructor usage
			return *this;
		}
	#endif



	bool operator == (const colony &rh) const PLF_NOEXCEPT
	{
		assert (this != &rh);

		if (total_number_of_elements != rh.total_number_of_elements)
		{
			return false;
		}
		else if (total_number_of_elements == 0)
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




private:

	// Advance implementation for iterators and const_iterators:
	template <class forward_iterator_type, class distance_type>
	void advance_implementation(forward_iterator_type &it, distance_type distance)
	{
		// For simplicity - should hopefully be optimized out by compiler:
		group_pointer_type &group_pointer = it.group_pointer;
		element_pointer_type &element_pointer = it.element_pointer;
		ushort_pointer_type &skipfield_pointer = it.skipfield_pointer;

		assert(group_pointer != NULL); // covers uninitialised colony_iterator && empty group

		// Now, run code based on the nature of the distance type - negative, positive or zero:
		if (distance > 0) // ie. +=
		{
			// Code explanation:
			// For the initial state of the iterator, we don't know how what's been erased before it in the group.
			// So for the first group, we follow the following logic:
			// 1. If no elements have been erased in the group, we do simple addition to progress either to the end of the group, or if there is another group after it and the addition amount specified is larger than the distance between the iterator and the end of the group, we progress onto the next group.
			// 2. If any of the first group elements have been erased, we manually iterate, as we don't know whether the skipfield element slots are before or after the initial iterator state. Iteration continues until we reach either reduce the addition amount to zero, or reach the end of the group.

			// For all subsequent groups, we follow the logic below:
			// 1. If the addition amount is larger than the total number of non-skipfield elements in the group, we skip the group and reduce the addition amount by the number of non-skipfield elements
			// 2. If the addition amount is smaller than the total number of non-skipfield elements in the group, then:
			//    a. if there're no erased elements in the group we do simple addition to find the new location for the iterator
			//    b. if there are erased elements in the group, manually iterate until the new iterator location is found ie. addition amount is reduced to zero

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->elements)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<unsigned short>(current_group.last_endpoint - current_group.elements)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->size)
				{
					const distance_type distance_from_end = static_cast<const distance_type>(current_group.last_endpoint - element_pointer);

					if(distance < distance_from_end)
					{
						element_pointer += distance;
						skipfield_pointer += distance;
						return;
					}
					else if (current_group.next_group == NULL)
					{
						element_pointer = current_group.last_endpoint;
						skipfield_pointer += distance_from_end;
						return;
					}
					else
					{
						group_pointer = group_pointer->next_group;
						skipfield_pointer = group_pointer->skipfield;
						element_pointer = group_pointer->elements + *skipfield_pointer;
						skipfield_pointer += *skipfield_pointer;

						distance -= distance_from_end;

						if (distance == 0)
						{
							return;
						}
					}
				}
				else
				{
					while(true)
					{
						element_pointer += 1 + *(++skipfield_pointer);
						skipfield_pointer += *skipfield_pointer;

						if (element_pointer != group_pointer->last_endpoint)
						{
							if (distance-- == 1)
							{
								return;
							}

							continue; // only loop point
						}
						else if(group_pointer->next_group != NULL) // ie. beyond end of available data
						{
							group_pointer = group_pointer->next_group;
							skipfield_pointer = group_pointer->skipfield;
							element_pointer = group_pointer->elements + *skipfield_pointer;
							skipfield_pointer += *skipfield_pointer;
						}

						if (distance-- == 1)
						{
							return;
						}

						break;
					}
				}
			}


			// All other groups:
			while(true)
			{
				const group &current_group = *group_pointer;

				if (static_cast<distance_type>(current_group.number_of_elements) <= distance)
				{
					if (current_group.next_group == NULL)
					{
						const unsigned short difference = static_cast<const unsigned short>(current_group.last_endpoint - element_pointer);
						element_pointer = current_group.last_endpoint;
						skipfield_pointer += difference;
						return;
					}

					distance -= current_group.number_of_elements;

					group_pointer = group_pointer->next_group;
					skipfield_pointer = group_pointer->skipfield;
					element_pointer = group_pointer->elements + *skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;

					if (distance == 0)
					{
						return;
					}

					continue; // The only actual loop point
				}
				else if (current_group.number_of_elements == static_cast<const unsigned short>(current_group.last_endpoint - current_group.elements))
				{
					element_pointer += distance;
					skipfield_pointer += distance;
					return;
				}
				else	 // ie. number_of_elements > distance - safe to ignore endpoint check condition
				{
					while(distance-- != 1)
					{
						element_pointer += 1 + *(++skipfield_pointer);
						skipfield_pointer += *skipfield_pointer;
					}

					return;
				}
			}
			
			return;
		}
		else if (distance < 0) // for negative change
		{
			// Code logic is very similar to += above
			assert(!(element_pointer == group_pointer->elements && group_pointer->previous_group == NULL)); // Assert that we are not already at the beginning on the colony
			distance = -distance;

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<const unsigned short>(current_group.last_endpoint - current_group.elements))
				{
					const distance_type distance_from_beginning = static_cast<const distance_type>(element_pointer - current_group.elements);
					
					if(distance <= distance_from_beginning) // can ignore skipfield field as no elements have been erased in this group
					{
						element_pointer -= distance;
						skipfield_pointer -= distance;
						return;
					}
					else if (current_group.previous_group == NULL) 
					{
						element_pointer = current_group.elements;
						skipfield_pointer = current_group.skipfield;
						return;
					}
					else 
					{ // No need to process erasure field in prev group either - this will be handled by the next section of code, if necessary
						group_pointer = current_group.previous_group;
						element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
						skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
						distance -= distance_from_beginning;
					}
				}
				else
				{
					while(distance-- != 1)
					{
						if (element_pointer != current_group.elements) // ie. not already at beginning of group 
						{
							element_pointer -= 1 + *(--skipfield_pointer);
							skipfield_pointer -= *skipfield_pointer;

							if (element_pointer != current_group.elements - 1) // ie. iterator was not already at beginning of colony, and skipfield does not takes us into the previous group)
							{
								continue;
							}
						}
						
						if (current_group.previous_group != NULL)
						{
							group_pointer = current_group.previous_group;
							element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
							skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
							break; // Process through other groups
						}
						else
						{
							return;
						}
					}
				}
			}
			

			// All other groups:
			while(true)
			{
				const group &current_group = *group_pointer;

				if (static_cast<distance_type>(current_group.number_of_elements) < distance)
				{
					if (current_group.previous_group == NULL)
					{
						element_pointer = current_group.elements;
						skipfield_pointer = current_group.skipfield;
						return;
					}

					distance -= current_group.number_of_elements;
					group_pointer = current_group.previous_group;
					element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
					skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
					continue;
				}
				// number_of_elements is > distance, so no need to worry about traversing more groups
				else if (current_group.number_of_elements == static_cast<const unsigned short>(current_group.last_endpoint - current_group.elements)) // ie. no erased elements in this group
				{
					element_pointer -= distance;
					skipfield_pointer -= distance;
					return;
				}
				else // ie. no more groups to traverse but there are erased elements in this group
				{
					while(distance-- != 1)
					{
						element_pointer -= 1 + *(--skipfield_pointer);
						skipfield_pointer -= *skipfield_pointer;
					}

					return;
				}
			}

			return;
		}

		return;
	}


public:
	template <class distance_type>
	void advance(iterator &it, distance_type distance)
	{
		advance_implementation<iterator, distance_type>(it, distance);
	}
	
	
	template <class distance_type>
	void advance(const_iterator &it, distance_type distance)
	{
		advance_implementation<const_iterator, distance_type>(it, distance);
	}
	
	
private:
	
	// Advance for reverse_iterator and const_reverse_iterator:
	template <class reverse_iterator_type, class distance_type>
	void reverse_advance_implementation(reverse_iterator_type &it, distance_type distance)
	{
		group_pointer_type &group_pointer = it.the_iterator.group_pointer;
		element_pointer_type &element_pointer = it.the_iterator.element_pointer;
		ushort_pointer_type &skipfield_pointer = it.the_iterator.skipfield_pointer;

		assert(group_pointer != NULL);
		
		if (distance > 0) // ie. += - this needs to be implemented differently to iterator's advance, as it needs to be able to reach rend() ie. beginning - 1
		{
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<const unsigned short>(current_group.last_endpoint - current_group.elements))
				{
					distance_type distance_from_beginning = static_cast<distance_type>(element_pointer - current_group.elements);
					
					if(distance <= distance_from_beginning || current_group.previous_group == NULL) // can ignore skipfield field as no elements have been erased in this group
					{
						element_pointer -= distance;
						skipfield_pointer -= distance;
						return;
					}
					else 
					{ // No need to process erasure field in prev group either - this will be handled by the next section of code, if necessary
						group_pointer = current_group.previous_group;
						element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
						skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
						distance -= distance_from_beginning;
					}
				}
				else
				{
					while(distance-- != 1)
					{
						if (element_pointer != current_group.elements) // ie. not already at beginning of group
						{
							element_pointer -= 1 + *(--skipfield_pointer);
							skipfield_pointer -= *skipfield_pointer;

							if (element_pointer != current_group.elements - 1) // ie. iterator was not already at beginning of colony, and skipfield does not takes us into the previous group)
							{
								continue;
							}
						}

						if (current_group.previous_group != NULL)
						{
							group_pointer = current_group.previous_group;
							element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
							skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
							break; // Process through other groups
						}
						else
						{
							element_pointer = current_group.elements - 1;
							skipfield_pointer = current_group.skipfield - 1;
							return;
						}
					}
				}
			}


			// All other groups:
			while(true) 
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements < distance)
				{
					if (current_group.previous_group == NULL) 
					{
						element_pointer = current_group.elements - 1;
						skipfield_pointer = current_group.skipfield - 1;
						return;
					}
					
					distance -= static_cast<distance_type>(current_group.number_of_elements);
					group_pointer = current_group.previous_group;
					element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield);
					skipfield_pointer = reinterpret_cast<ushort_pointer_type>(element_pointer) + group_pointer->size;
					continue;
				}
				else if (current_group.number_of_elements == static_cast<const unsigned short>(current_group.last_endpoint - current_group.elements))
				{
					element_pointer -= distance;
					skipfield_pointer -= distance;
					return;
				}
				else
				{
					while(distance-- != 1)
					{
						element_pointer -= 1 + *(--skipfield_pointer);
						skipfield_pointer -= *skipfield_pointer;
					}
					
					return;
				}
			}
		}
		else if (distance < 0) // ie. -=
		{
			advance_implementation(it.the_iterator, -distance);
		}

		return;
	}

	
public:

	template <class distance_type>
	void advance(reverse_iterator &it, distance_type distance)
	{
		reverse_advance_implementation<reverse_iterator, distance_type>(it, distance);
	}
	
	
	template <class distance_type>
	void advance(const_reverse_iterator &it, distance_type distance)
	{
		reverse_advance_implementation<const_reverse_iterator, distance_type>(it, distance);
	}
	
	

	// Next implementations:
	inline iterator next(iterator &it, typename std::iterator_traits<iterator>::difference_type distance)
	{
		iterator return_iterator(it);
		advance_implementation<iterator>(return_iterator, distance);
		return return_iterator;
	}

	inline const_iterator next(const_iterator &it, typename std::iterator_traits<const_iterator>::difference_type distance)
	{
		const_iterator return_iterator(it);
		advance_implementation<const_iterator>(return_iterator, distance);
		return return_iterator;
	}


	inline reverse_iterator next(reverse_iterator &it, typename std::iterator_traits<reverse_iterator>::difference_type distance)
	{
		reverse_iterator return_iterator(it);
		reverse_advance_implementation<reverse_iterator>(return_iterator, distance);
		return return_iterator;
	}

	inline const_reverse_iterator next(const_reverse_iterator &it, typename std::iterator_traits<const_reverse_iterator>::difference_type distance)
	{
		const_reverse_iterator return_iterator(it);
		reverse_advance_implementation<const_reverse_iterator>(return_iterator, distance);
		return return_iterator;
	}


	

	// Prev implementations:
	inline iterator prev(const iterator &it, typename std::iterator_traits<iterator>::difference_type distance)
	{
		iterator return_iterator(it);
		advance_implementation<iterator>(return_iterator, -distance);
		return return_iterator;
	}

	inline const_iterator prev(const const_iterator &it, typename std::iterator_traits<const_iterator>::difference_type distance)
	{
		const_iterator return_iterator(it);
		advance_implementation<const_iterator>(return_iterator, -distance);
		return return_iterator;
	}


	inline reverse_iterator prev(const reverse_iterator &it, typename std::iterator_traits<reverse_iterator>::difference_type distance)
	{
		reverse_iterator return_iterator(it);
		reverse_advance_implementation<reverse_iterator>(return_iterator, -distance);
		return return_iterator;
	}

	inline const_reverse_iterator prev(const const_reverse_iterator &it, typename std::iterator_traits<const_reverse_iterator>::difference_type distance)
	{
		const_reverse_iterator return_iterator(it);
		reverse_advance_implementation<const_reverse_iterator>(return_iterator, -distance);
		return return_iterator;
	}
	


private:

	// distance implementation:
	template <class iterator_type>
	typename std::iterator_traits<iterator_type>::difference_type distance_implementation(const iterator_type &first, const iterator_type &last) PLF_NOEXCEPT
	{
		// Code logic:
		// If iterators are the same, return 0
		// Otherwise, find which iterator is later in colony, copy that to iterator2. Copy the lower to iterator1.
		// If not in the same group, process the intermediate groups and add distances,
		// skipping manual incrementation in all but the initial and final groups.
		// In the initial and final groups, manual incrementation may be skipped if there are no deletions in those groups.
		// In that case, simple subtraction of pointer values is possible to determine the distances between current location
		// and end.
		
		if (last.element_pointer == first.element_pointer)
		{
			return 0;
		}


		iterator_type iterator1 = first, iterator2 = last;
		typedef typename std::iterator_traits<iterator_type>::difference_type diff_type;
		diff_type distance = 0;
		bool swap = false;
		
		if (first > last) // Less common case
		{
			iterator1 = last;
			iterator2 = first;
			swap = true;
		}

		
		if (iterator1.group_pointer != iterator2.group_pointer) // if not in same group, process intermediate groups
		{
			if (iterator1.group_pointer->number_of_elements == static_cast<unsigned short>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // If no deletions in this group
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->last_endpoint - iterator1.element_pointer);
			}
			else if (iterator1.element_pointer == iterator1.group_pointer->elements) // ie. element is at start of group - rare case
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements);
			}
			else
			{
				element_pointer_type endpoint = iterator1.group_pointer->last_endpoint;
				
				while (iterator1.element_pointer != endpoint)
				{
					iterator1.element_pointer += 1 + *(++iterator1.skipfield_pointer);
					iterator1.skipfield_pointer += *iterator1.skipfield_pointer;

					++distance;
				}
			}
			
			
			iterator1.group_pointer = iterator1.group_pointer->next_group;
			
			while (iterator1.group_pointer != iterator2.group_pointer)
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements);
				iterator1.group_pointer = iterator1.group_pointer->next_group;
			}
			
			iterator1.element_pointer = iterator1.group_pointer->elements;
			iterator1.skipfield_pointer = iterator1.group_pointer->skipfield;
		}

		
		if (iterator1.group_pointer->number_of_elements == static_cast<unsigned short>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // ie. no deletions in this group, direct subtraction is possible
		{
			distance += static_cast<diff_type>(iterator2.element_pointer - iterator1.element_pointer);
		}
		else if (iterator1.group_pointer->last_endpoint - 1 >= iterator2.element_pointer) // ie. if iterator2 is .end() or 1 before (impossible to have any deletions between .end() and this)
		{
			distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements - (iterator1.group_pointer->last_endpoint - iterator2.element_pointer));
		}
		else
		{
			while (iterator1.element_pointer != iterator2.element_pointer)
			{
				iterator1.element_pointer += 1 + *(++iterator1.skipfield_pointer);
				iterator1.skipfield_pointer += *iterator1.skipfield_pointer;

				++distance;
			}
		}

		
		if (swap)
		{
			distance = -distance;
		}

		return distance;
	}
	

public:

	inline typename std::iterator_traits<iterator>::difference_type distance(const iterator &first, const iterator &last) PLF_NOEXCEPT
	{
		return distance_implementation<iterator>(first, last);
	}


	inline typename std::iterator_traits<const_iterator>::difference_type distance(const const_iterator &first, const const_iterator &last) PLF_NOEXCEPT
	{
		return distance_implementation<const_iterator>(first, last);
	}


	template <class reverse_iterator_type>
	inline typename std::iterator_traits<reverse_iterator_type>::difference_type reverse_distance_implementation(const reverse_iterator_type &first, const reverse_iterator_type &last) PLF_NOEXCEPT
	{
		return distance_implementation<iterator>(first.the_iterator, last.the_iterator);
	}


	inline typename std::iterator_traits<reverse_iterator>::difference_type distance(const reverse_iterator &first, const reverse_iterator &last) PLF_NOEXCEPT
	{
		return reverse_distance_implementation<reverse_iterator>(first, last);
	}


	inline typename std::iterator_traits<const_reverse_iterator>::difference_type distance(const const_reverse_iterator &first, const const_reverse_iterator &last) PLF_NOEXCEPT
	{
		return distance_implementation<const_reverse_iterator>(first, last);
	}



};  // colony


} // plf namespace


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
