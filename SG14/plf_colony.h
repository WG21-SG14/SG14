// Copyright (c) 2016, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef PLF_COLONY_H
#define PLF_COLONY_H



// Compiler-specific defines used by colony:

#if defined(_MSC_VER)
	#define PLF_COLONY_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_COLONY_NOEXCEPT throw()
	#elif _MSC_VER == 1600
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
	#elif _MSC_VER == 1700
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
	#elif _MSC_VER == 1800
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
	#define PLF_COLONY_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4 // 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#elif __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#endif
	#else // Assume type traits and initializer support for non-GCC compilers
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
	#endif

	#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
	#define PLF_COLONY_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
	#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
	#define PLF_COLONY_NOEXCEPT noexcept
#else
	#define PLF_COLONY_FORCE_INLINE
	#define PLF_COLONY_NOEXCEPT throw()
#endif



#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, ...)	std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, data) std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_COLONY_DESTROY(the_allocator, allocator_instance, location) 			std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
	#define PLF_COLONY_ALLOCATE(the_allocator, allocator_instance, size, hint) 			std::allocator_traits<the_allocator>::allocate(allocator_instance, size, hint)
 	#define PLF_COLONY_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 				std::allocator_traits<the_allocator>::allocate(*this, size, hint)
	#define PLF_COLONY_DEALLOCATE(the_allocator, allocator_instance, location, size) 	std::allocator_traits<the_allocator>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, ...)	allocator_instance.construct(location, __VA_ARGS__)
	#else
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, data) allocator_instance.construct(location, data)
	#endif

	#define PLF_COLONY_DESTROY(the_allocator, allocator_instance, location) 			allocator_instance.destroy(location)
	#define PLF_COLONY_ALLOCATE(the_allocator, allocator_instance, size, hint)	 		allocator_instance.allocate(size, hint)
	#define PLF_COLONY_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 				the_allocator::allocate(size, hint)
	#define PLF_COLONY_DEALLOCATE(the_allocator, allocator_instance, location, size) 	allocator_instance.deallocate(location, size)
#endif



#include <cstring>	// memset, memcpy
#include <cassert>	// assert
#include <limits>  // std::numeric_limits
#include <memory>	// std::uninitialized_copy, std::allocator
#include <iterator> // std::bidirectional_iterator_tag

#ifndef PLF_STACK_H
	#include "plf_stack.h" // For erased_location stack
#endif


#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
	#include <type_traits> // std::is_trivially_destructible, etc
#endif

#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#include <initializer_list>
#endif

namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type>, typename skipfield_type = unsigned short > class colony : private element_allocator_type  // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type															value_type;
	typedef element_allocator_type													allocator_type;

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT // C++11
		typedef typename std::allocator_traits<element_allocator_type>::size_type			size_type;
		typedef typename std::allocator_traits<element_allocator_type>::difference_type 	difference_type;
		typedef element_type &																reference;
		typedef const element_type &														const_reference;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 			pointer;
		typedef typename std::allocator_traits<element_allocator_type>::const_pointer		const_pointer;
	#else
		typedef typename element_allocator_type::size_type			size_type;
		typedef typename element_allocator_type::difference_type	difference_type;
		typedef typename element_allocator_type::reference			reference;
		typedef typename element_allocator_type::const_reference	const_reference;
		typedef typename element_allocator_type::pointer			pointer;
		typedef typename element_allocator_type::const_pointer		const_pointer;
	#endif

	// Iterator declarations:
	template <class colony_element_allocator_type, bool is_const> class colony_iterator;
	typedef colony_iterator<element_allocator_type, false>	iterator;
	typedef colony_iterator<element_allocator_type, true>	const_iterator;
	friend class colony_iterator<element_allocator_type, false>; // clang complains if typedefs used in friend declarations
	friend class colony_iterator<element_allocator_type, true>;

	template <class r_colony_element_allocator_type, bool r_is_const> class colony_reverse_iterator;
	typedef colony_reverse_iterator<element_allocator_type, false>	reverse_iterator;
	typedef colony_reverse_iterator<element_allocator_type, true>	const_reverse_iterator;
	friend class colony_reverse_iterator<element_allocator_type, false>;
	friend class colony_reverse_iterator<element_allocator_type, true>;

private:
	struct group; // forward declaration for typedefs below

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT // C++11
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group>				group_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<skipfield_type>		ushort_allocator_type; // Equivalent to uint_least16_t ie. Using 16-bit integer in best-case scenario, > 16-bit integer in case where platform doesn't support 16-bit types
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<unsigned char>		uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas, for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.

		typedef typename std::allocator_traits<element_allocator_type>::pointer				element_pointer_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer 				group_pointer_type;
		typedef group &												 						group_reference_type;
		typedef typename std::allocator_traits<ushort_allocator_type>::pointer 				ushort_pointer_type;
		typedef typename std::allocator_traits<uchar_allocator_type>::pointer				uchar_pointer_type;

		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<element_pointer_type>	element_pointer_allocator_type;
	#else
		typedef typename element_allocator_type::template rebind<group>::other				group_allocator_type;
		typedef typename element_allocator_type::template rebind<skipfield_type>::other		ushort_allocator_type; // Equivalent to uint_least16_t ie. Using 16-bit integer in best-case scenario, > 16-bit integer in case where platform doesn't support 16-bit types
		typedef typename element_allocator_type::template rebind<unsigned char>::other		uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas, for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.

		typedef typename element_allocator_type::pointer			element_pointer_type; // Identical typedef to 'pointer', for clarity in code (between group_pointers etc)
		typedef typename group_allocator_type::pointer 				group_pointer_type;
		typedef typename group_allocator_type::reference 			group_reference_type;
		typedef typename ushort_allocator_type::pointer 			ushort_pointer_type;
		typedef typename uchar_allocator_type::pointer				uchar_pointer_type;

		typedef typename element_allocator_type::template rebind<element_pointer_type>::other element_pointer_allocator_type;
	#endif

	
	typedef typename plf::stack<element_pointer_type, element_pointer_allocator_type> element_pointer_stack_type;



	struct group : private uchar_allocator_type  // Empty base class optimisation - inheriting allocator functions
	{
		element_pointer_type		last_endpoint; // the address that is one past the highest cell number that's been used so far in this group - does not change with erase command - is necessary because an iterator cannot access the colony's end_iterator
		group_pointer_type			next_group;
		const element_pointer_type	elements;
		const ushort_pointer_type	skipfield; // Now that both the elements and skipfield arrays are allocated contiguously, skipfield pointer also functions as a 'one-past-end' pointer for the elements array
		group_pointer_type			previous_group;
		size_type					group_number; // Used for comparison (> < >= <=) iterator operators
		skipfield_type				number_of_elements; // indicates total number of used cells - changes with insert and erase commands - used to check for empty group in erase function, as indication to remove group
		const skipfield_type		size; // The number of elements this particular group can house


		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(element_type))) + ((elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL),
				elements(last_endpoint),
				skipfield(reinterpret_cast<ushort_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				number_of_elements(0),
				size(elements_per_group)
			{
				// Static casts to unsigned int from short not necessary as C++ automatically promotes lesser types for arithmetic purposes.
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (size + 1)); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
			}

		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(element_type))) + ((elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL),
				elements(NULL),
				skipfield(reinterpret_cast<ushort_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				number_of_elements(0),
				size(elements_per_group)
			{
				// Static casts to unsigned int from short not necessary as C++ automatically promotes lesser types for arithmetic purposes.
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (size + 1)); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
			}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source) PLF_COLONY_NOEXCEPT:
				uchar_allocator_type(source),
				last_endpoint(source.last_endpoint),
				next_group(NULL),
				elements(source.last_endpoint),
				skipfield(source.skipfield),
				previous_group(source.previous_group),
				group_number(source.group_number),
				number_of_elements(0),
				size(source.size)
			{}
		#endif


		~group() PLF_COLONY_NOEXCEPT
		{
			// Null check not necessary (for copied group as above) as delete will ignore.
			PLF_COLONY_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), (size * sizeof(element_type)) + ((size + 1) * sizeof(skipfield_type)));
		}
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
	template <class colony_allocator_type, bool is_const> class colony_iterator 
	{
	private:
		group_pointer_type		group_pointer;
		element_pointer_type	element_pointer;
		ushort_pointer_type		skipfield_pointer;

	public:
		typedef std::bidirectional_iterator_tag 	iterator_category;
		typedef typename colony::value_type 		value_type;
		typedef typename colony::difference_type 	difference_type;
		typedef typename choose<is_const, typename colony::const_pointer, typename colony::pointer>::type		pointer;
		typedef typename choose<is_const, typename colony::const_reference, typename colony::reference>::type	reference;

		friend class colony;
		friend class colony_reverse_iterator<typename colony::allocator_type, false>;
		friend class colony_reverse_iterator<typename colony::allocator_type, true>;



		inline colony_iterator & operator = (const colony_iterator &source) PLF_COLONY_NOEXCEPT
		{
			assert (&source != this);

			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			skipfield_pointer = source.skipfield_pointer;

			return *this;
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// Move assignment
			inline colony_iterator & operator = (colony_iterator &&source) PLF_COLONY_NOEXCEPT // Move is a copy in this scenario
			{
				assert (&source != this);

				// Only really useful if the allocator uses non-standard ie. smart pointers
				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				skipfield_pointer = std::move(source.skipfield_pointer);

				return *this;
			}
		#endif



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}


		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}


		inline PLF_COLONY_FORCE_INLINE reference operator * () const PLF_COLONY_NOEXCEPT
		{
			return *element_pointer;
		}



		inline PLF_COLONY_FORCE_INLINE pointer operator -> () const PLF_COLONY_NOEXCEPT
		{
			return element_pointer;
		}



		inline PLF_COLONY_FORCE_INLINE colony_iterator & operator ++ ()
		{
			assert(group_pointer != NULL); // covers uninitialised colony_iterator
			assert(!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL)); // Assert that iterator is not already at end()

			element_pointer += 1 + *(++skipfield_pointer);
			skipfield_pointer += *skipfield_pointer;

			if (element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
			{
				group_pointer = group_pointer->next_group;
				skipfield_pointer = group_pointer->skipfield;
				element_pointer = group_pointer->elements + *skipfield_pointer;
				skipfield_pointer += *skipfield_pointer;
			}

			return *this;
		}



	private:
		inline PLF_COLONY_FORCE_INLINE void check_end_of_group_and_progress() // used by erase
		{
			if (element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
			{
				group_pointer = group_pointer->next_group;
				skipfield_pointer = group_pointer->skipfield;
				element_pointer = group_pointer->elements + *skipfield_pointer;
				skipfield_pointer += *skipfield_pointer;
			}
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
			assert(!(element_pointer == group_pointer->elements && group_pointer->previous_group == NULL)); // Assert that we are not already at begin() - this is not required to be tested in the code below as we don't need a special condition to progress to begin(), like we do with end() in operator ++

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
			element_pointer = (reinterpret_cast<colony::element_pointer_type>(group_pointer->skipfield) - 1) - *skipfield_pointer;
			skipfield_pointer -= *skipfield_pointer;

			return *this;
		}



		inline colony_iterator operator -- (int)
		{
			colony_iterator copy(*this);
			--*this;
			return copy;
		}


		inline bool operator > (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (((group_pointer == rh.group_pointer) && (element_pointer > rh.element_pointer)) || (group_pointer->group_number > rh.group_pointer->group_number));
		}


		inline bool operator < (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return rh > *this;
		}


		inline bool operator >= (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh > *this);
		}


		inline bool operator <= (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(*this > rh);
		}



		// C++14-specific functionality:
		#if defined(__cplusplus) && __cplusplus >= 201402L
			inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (element_pointer == rh.element_pointer);
			}


			inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (element_pointer != rh.element_pointer);
			}


			inline bool operator > (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (((group_pointer == rh.group_pointer) && (element_pointer > rh.element_pointer)) || (group_pointer->group_number > rh.group_pointer->group_number));
			}


			inline bool operator < (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return rh > *this;
			}


			inline bool operator >= (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return !(rh > *this);
			}


			inline bool operator <= (const colony_iterator<colony_allocator_type, !is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return !(*this > rh);
			}
		#endif


		colony_iterator() PLF_COLONY_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), skipfield_pointer(NULL) {}



		inline colony_iterator (const colony_iterator &source) PLF_COLONY_NOEXCEPT:
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer),
			skipfield_pointer(source.skipfield_pointer)
		{}




		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move constructor
			inline colony_iterator(colony_iterator &&source) PLF_COLONY_NOEXCEPT:
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer)),
				skipfield_pointer(std::move(source.skipfield_pointer))
			{}
		#endif


	}; // colony_iterator





	// Reverse iterators:

	template <class r_colony_allocator_type, bool r_is_const> class colony_reverse_iterator
	{
	private:
		iterator the_iterator;

	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef typename colony::value_type 		value_type;
		typedef typename colony::difference_type 	difference_type;
		typedef typename choose<r_is_const, typename colony::const_pointer, typename colony::pointer>::type		pointer;
		typedef typename choose<r_is_const, typename colony::const_reference, typename colony::reference>::type	reference;

		friend class colony;
		


		inline colony_reverse_iterator& operator = (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT
		{
			the_iterator = source.the_iterator;
			return *this;
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move assignment
			inline colony_reverse_iterator& operator = (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT 
			{
				the_iterator = std::move(source.the_iterator);
				return *this;
			}
		#endif



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator == rh.the_iterator);
		}
		

		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator != rh.the_iterator);
		}


		inline PLF_COLONY_FORCE_INLINE reference operator * () const PLF_COLONY_NOEXCEPT
		{
			return *(the_iterator.element_pointer);
		}



		inline PLF_COLONY_FORCE_INLINE pointer * operator -> () const PLF_COLONY_NOEXCEPT
		{
			return the_iterator.element_pointer;
		}
		


		// Need to redefine rather than using internal iterator's -- operator, in order for ++ to be able to reach rend() ie. elements[-1]
		colony_reverse_iterator & operator ++ ()
		{
			colony::group_pointer_type &group_pointer = the_iterator.group_pointer;
			colony::element_pointer_type &element_pointer = the_iterator.element_pointer;
			colony::ushort_pointer_type &skipfield_pointer = the_iterator.skipfield_pointer;

			assert(group_pointer != NULL);
			assert(!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Assert that we are not already at rend()

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				element_pointer -= 1 + *(--skipfield_pointer);
				skipfield_pointer -= *skipfield_pointer;

				if (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)) // ie. iterator is not now at rend()
				{
					return *this;
				}
			}

			if (group_pointer->previous_group != NULL)
			{
				group_pointer = group_pointer->previous_group;
				skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
				element_pointer = (reinterpret_cast<colony::element_pointer_type>(group_pointer->skipfield) - 1) - *skipfield_pointer;
				skipfield_pointer -= *skipfield_pointer;
			}
			else // necessary so that reverse_iterator can end up == rend() ie. first_group->elements[-1], if we were already at first element in colony
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


		inline PLF_COLONY_FORCE_INLINE colony_reverse_iterator & operator -- ()
		{
			assert(!(the_iterator.element_pointer == the_iterator.group_pointer->last_endpoint - 1 && the_iterator.group_pointer->next_group == NULL)); // Assert that we are not already at rbegin()
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


		inline bool operator > (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (rh.the_iterator > the_iterator);
		}


		inline bool operator < (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator > rh.the_iterator);
		}


		inline bool operator >= (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(the_iterator > rh.the_iterator);
		}


		inline bool operator <= (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh.the_iterator > the_iterator);
		}



		// C++14-specific functionality:
		#if defined(__cplusplus) && __cplusplus >= 201402L
			inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (the_iterator == rh.the_iterator);
			}
			

			inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (the_iterator != rh.the_iterator);
			}
	
	
			inline bool operator > (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (rh.the_iterator > the_iterator);
			}
	
	
			inline bool operator < (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return (the_iterator > rh.the_iterator);
			}

	
			inline bool operator >= (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return !(the_iterator > rh.the_iterator);
			}
	

			inline bool operator <= (const colony_reverse_iterator<r_colony_allocator_type, !r_is_const> &rh) const PLF_COLONY_NOEXCEPT
			{
				return !(rh.the_iterator > the_iterator);
			}
		#endif






		colony_reverse_iterator (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT: 
			the_iterator(source.the_iterator)
		{}



		colony_reverse_iterator (const typename colony::iterator &source) PLF_COLONY_NOEXCEPT:
			the_iterator(source)
		{}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move constructors
			colony_reverse_iterator (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT:
				the_iterator(std::move(source.the_iterator))
			{}

			colony_reverse_iterator (typename colony::iterator &&source) PLF_COLONY_NOEXCEPT:
				the_iterator(std::move(source))
			{}
		#endif

	}; // colony_reverse_iterator




private:

	iterator				end_iterator, begin_iterator;
	group_pointer_type		first_group;
	size_type				total_number_of_elements;
	skipfield_type 			min_elements_per_group;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		skipfield_type max_elements_per_group;
		ebco_pair(const skipfield_type max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;
	
	element_pointer_stack_type erased_locations;


	// Used by range-insert and range-constructor to prevent fill-insert and fill-constructor function calls mistakenly resolving to the range insert/constructor
	template <bool condition, class T = void> 
	struct plf_enable_if_c
	{
		typedef T type;
	};

	template <class T>
	struct plf_enable_if_c<false, T>
	{};
	

public:

	// Default constuctor:

	explicit colony(const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(8),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max())
	{
    	assert(std::numeric_limits<skipfield_type>::is_integer && !std::numeric_limits<skipfield_type>::is_signed); // skipfield type must be of unsigned integer type (uchar, ushort, uint etc)
	}
	


	// Copy constructor:
	
	colony (const colony &source):
		element_allocator_type(source),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(source.min_elements_per_group),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
		erased_locations(source.erased_locations.min_elements_per_group)
	{
		// Copy data from source:
		insert(source.begin(), source.end());
	}


	
	// Move constructor:

	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		colony(colony &&source) PLF_COLONY_NOEXCEPT:
			element_allocator_type(source),
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			first_group(std::move(source.first_group)),
			total_number_of_elements(source.total_number_of_elements),
			min_elements_per_group(source.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
			erased_locations(std::move(source.erased_locations))
		{
			source.first_group = NULL;
			source.total_number_of_elements = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_number_of_elements 0, to allow for clean destructor usage
		}
	#endif



	// Fill constructor:

	colony(const size_type fill_number, const element_type &element, const skipfield_type min_allocation_amount = 8, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(min_allocation_amount),
		group_allocator_pair(max_allocation_amount),
		erased_locations((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8)
	{
    	assert(std::numeric_limits<skipfield_type>::is_integer && !std::numeric_limits<skipfield_type>::is_signed);
		assert(min_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(min_allocation_amount <= group_allocator_pair.max_elements_per_group);

		if (fill_number != 0)
		{
			insert(fill_number, element);
		}
	}



	// Range constructor:

	template<typename iterator_type>
	colony(const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type &first, const iterator_type &last, const skipfield_type min_allocation_amount = 8, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(min_allocation_amount),
		group_allocator_pair(max_allocation_amount),
		erased_locations((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8)
	{
    	assert(std::numeric_limits<skipfield_type>::is_integer && !std::numeric_limits<skipfield_type>::is_signed);
		assert(min_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
		assert(min_allocation_amount <= group_allocator_pair.max_elements_per_group);

		insert<iterator_type>(first, last);
	}



	// Initializer-list constructor:

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		colony(const std::initializer_list<element_type> &element_list, const skipfield_type min_allocation_amount = 8, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
			element_allocator_type(alloc),
			first_group(NULL),
			total_number_of_elements(0),
			min_elements_per_group(min_allocation_amount),
			group_allocator_pair(max_allocation_amount),
			erased_locations((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8)
		{
	    	assert(std::numeric_limits<skipfield_type>::is_integer && !std::numeric_limits<skipfield_type>::is_signed);
			assert(min_allocation_amount > 2); 	// Otherwise, too much overhead for too small a group
			assert(min_allocation_amount <= group_allocator_pair.max_elements_per_group);

			insert(element_list);
		}
		
	#endif



	inline PLF_COLONY_FORCE_INLINE iterator begin() PLF_COLONY_NOEXCEPT
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const iterator & begin() const PLF_COLONY_NOEXCEPT // To allow for functions which only take const colony & as a source ie. copy constructor
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE iterator end() PLF_COLONY_NOEXCEPT
	{
		return end_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const iterator & end() const PLF_COLONY_NOEXCEPT
	{
		return end_iterator;
	}



	inline const const_iterator cbegin() const PLF_COLONY_NOEXCEPT
	{
		const_iterator return_iterator;

		return_iterator.group_pointer = begin_iterator.group_pointer;
		return_iterator.element_pointer = begin_iterator.element_pointer;
		return_iterator.skipfield_pointer = begin_iterator.skipfield_pointer;

		return return_iterator;
	}



	inline const_iterator cend() const PLF_COLONY_NOEXCEPT
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



	inline reverse_iterator rend() const PLF_COLONY_NOEXCEPT
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



	inline const_reverse_iterator crend() const PLF_COLONY_NOEXCEPT
	{
		const_reverse_iterator beforebegin(begin_iterator);
		--(beforebegin.the_iterator.element_pointer); // ie. point to memory space before first element
		--(beforebegin.the_iterator.skipfield_pointer); // ie. point to memory space before first erasure field
		return beforebegin;
	}



	~colony() PLF_COLONY_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_COLONY_NOEXCEPT
	{
	#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
		if (total_number_of_elements != 0 && !(std::is_trivially_destructible<element_type>::value))
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
				PLF_COLONY_DESTROY(element_allocator_type, (*this), element_pointer);

				element_pointer += 1 + *(++skipfield_pointer);
				skipfield_pointer += *skipfield_pointer;

				if (element_pointer == end_pointer) // ie. beyond end of available data
				{
					previous_group = group_pointer;
					group_pointer = group_pointer->next_group;

					PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);

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
		else // Avoid iteration for both empty groups and trivially-destructible types eg. POD, structs, classes with empty destructors
		{
			// Technically under a type-traits-supporting compiler total_number_of_elements could be non-zero at this point, but since first_group would already be NULL in the case of double-destruction, it's unnecessary to zero total_number_of_elements
			group_pointer_type previous_group;

			while (first_group != NULL)
			{
				previous_group = first_group;
				first_group = first_group->next_group;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);
			}
		}
	}



	void initialize()
	{
		first_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_elements_per_group);
			#else
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_elements_per_group));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = NULL;
			throw;
		}

		begin_iterator.group_pointer = first_group;
		begin_iterator.element_pointer = first_group->elements;
		begin_iterator.skipfield_pointer = first_group->skipfield;
		end_iterator = begin_iterator;
	}

public:


	iterator insert(const element_type &element)
	{
		if (end_iterator.element_pointer != NULL)
		{
			switch(((!erased_locations.empty()) << 1) | (end_iterator.element_pointer == reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)))
			{
				case 0: // ie. erased_locations is empty and end_iterator is not at end of current final group
				{
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, element);
	
					++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
					++end_iterator.skipfield_pointer;
					++(end_iterator.group_pointer->last_endpoint);
					++(end_iterator.group_pointer->number_of_elements);
					++total_number_of_elements;
					return return_iterator; /* returns value before incrementation */
				}
				case 1:  // ie. erased_locations is empty and end_iterator is at end of current final group - ie. colony is full - create new group
				{
					end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
					group_reference_type next_group = *(end_iterator.group_pointer->next_group);

					try
					{
						#ifdef PLF_COLONY_VARIADICS_SUPPORT
							PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
						#else // C++03 only supports copy construction
							PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
						#endif
					}
					catch (...)
					{
						PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					end_iterator.group_pointer = &next_group;
					end_iterator.element_pointer = next_group.elements;
					end_iterator.skipfield_pointer = next_group.skipfield;
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

					try
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
					}
					catch (...)
					{
						end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
						end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
						end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
						PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					++end_iterator.skipfield_pointer;
					++(next_group.last_endpoint);
					++(next_group.number_of_elements);
					++total_number_of_elements;
					return return_iterator; /* returns value before incrementation */
				}
				default: // ie. erased_locations is not empty, reuse previous-erased element locations
				{
					iterator new_location;
					new_location.element_pointer = erased_locations.top();
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), new_location.element_pointer, element);
					erased_locations.pop();

					new_location.group_pointer = end_iterator.group_pointer; // Start with last group first, as will be the largest group
			
					while (new_location.element_pointer < new_location.group_pointer->elements || new_location.element_pointer >= reinterpret_cast<element_pointer_type>(new_location.group_pointer->skipfield))
					{
						new_location.group_pointer = new_location.group_pointer->previous_group;
					}

					new_location.skipfield_pointer = new_location.group_pointer->skipfield + (new_location.element_pointer - new_location.group_pointer->elements);
					
					++(new_location.group_pointer->number_of_elements);
	
					if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
					{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
						begin_iterator = new_location;
					}
	
					++total_number_of_elements;
	
					// Pseudocode:
					// if !erased_locations.empty(), check whether location we are restoring to has a slot before or after which is erased
					// if it has only a slot before which is erased (ie. at end of erasure block), update the prime erasure point
					// if it only has a slot after it which is erased, (ie. this is the prime erasure point), change next slot to prime erasure point and update all subsequent erasure points (ie. decrement by 1)
					// if it has both a slot before and after which is erased (ie. is in middle of erasure block), do both of the above
					const unsigned char prev_skipfield = (new_location.skipfield_pointer != new_location.group_pointer->skipfield && *(new_location.skipfield_pointer - 1) != 0);
					const unsigned char after_skipfield = (*(new_location.skipfield_pointer + 1) != 0); // NOTE: first test removed due to extra slot in skipfield (compared to element field)
	
					switch (prev_skipfield | (after_skipfield << 1))
					{
						case 1: // previous erased consecutive elements, none following
						{
							*(new_location.skipfield_pointer - (*(new_location.skipfield_pointer) - 1)) = *(new_location.skipfield_pointer) - 1;
							break;
						}
						case 2: // No previous consecutive erased points, at least one following ie. this was the prime erasure point
						{
							skipfield_type update_count = *(new_location.skipfield_pointer) - 1;
							ushort_pointer_type following = new_location.skipfield_pointer + 1;
							*following = update_count;
							skipfield_type update_value = 2;
	
							skipfield_type vectorize = --update_count / 4;
							update_count -= vectorize * 4;
	
							while (vectorize != 0)
							{
								--vectorize;
	
								*(following + 1) = update_value;
								*(following + 2) = update_value + 1;
								*(following + 3) = update_value + 2;
								*(following + 4) = update_value + 3;
	
								following += 4;
								update_value += 4;
							}
	
							while (update_count != 0)
							{
								--update_count;
								*(++following) = update_value++;
							}
	
							break;
						}
						case 3: // both preceding and following consecutive erased elements
						{
							const skipfield_type value = *(new_location.skipfield_pointer);
							ushort_pointer_type start_node = new_location.skipfield_pointer - (value - 1);
							skipfield_type update_count = *start_node - value;
							*start_node = value - 1;
	
							ushort_pointer_type following = new_location.skipfield_pointer + 1;
							*following = update_count;
	
							skipfield_type update_value = 2;
	
							skipfield_type vectorize = --update_count / 4;
							update_count -= vectorize * 4;
	
							while (vectorize != 0)
							{
								--vectorize;
	
								*(following + 1) = update_value;
								*(following + 2) = update_value + 1;
								*(following + 3) = update_value + 2;
								*(following + 4) = update_value + 3;
	
								following += 4;
								update_value += 4;
							}
	
	
							while (update_count != 0)
							{
								--update_count;
								*(++following) = update_value++; // Create new series
							}
						}
					}
	
					*new_location.skipfield_pointer = 0;
					return new_location;
				}
			}	
		}
		else // ie. newly-constructed colony, no insertions yet and no groups
		{
			initialize();
			
			try
			{
				PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
			}
			catch (...)
			{
				clear();
				throw;
			}

			++end_iterator.skipfield_pointer;
			++first_group->last_endpoint;
			++first_group->number_of_elements;
			++total_number_of_elements;

			return begin_iterator; /* returns value before incrementation */
		}
	}


	
	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Note: the reason for code duplication from non-move insert, as opposed to using std::forward for both, was because most compilers didn't actually create as-optimal code in that strategy. Also, C++03 compliance...

		iterator insert(element_type &&element)
		{
			if (end_iterator.element_pointer != NULL)
			{
				switch(((!erased_locations.empty()) << 1) | (end_iterator.element_pointer == reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)))
				{
					case 0: // ie. erased_locations is empty and end_iterator is not at end of current final group
					{
						const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, std::move(element));
	
						++end_iterator.element_pointer; /* not postfix incrementing prev statement as necessitates a slower try-catch block to reverse increment if necessary */
						++end_iterator.skipfield_pointer;
						++(end_iterator.group_pointer->last_endpoint);
						++(end_iterator.group_pointer->number_of_elements);
						++total_number_of_elements;
						return return_iterator; /* returns value before incrementation */
					}
					case 1:  // ie. erased_locations is empty and end_iterator is at end of current final group - ie. colony is full - create new group
					{
						end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
						group_reference_type next_group = *(end_iterator.group_pointer->next_group);

						try
						{
							#ifdef PLF_COLONY_VARIADICS_SUPPORT
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
							#else // C++03 only supports copy construction
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
							#endif
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						end_iterator.group_pointer = &next_group;
						end_iterator.element_pointer = next_group.elements;
						end_iterator.skipfield_pointer = next_group.skipfield;
						const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */

						try
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, std::move(element));
						}
						catch (...)
						{
							end_iterator.group_pointer = end_iterator.group_pointer->previous_group;
							end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
							end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, end_iterator.group_pointer->next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						++end_iterator.skipfield_pointer;
						++(next_group.last_endpoint);
						++(next_group.number_of_elements);
						++total_number_of_elements;
						return return_iterator; /* returns value before incrementation */
					}
					default: // ie. erased_locations is not empty, reuse previous-erased element locations
					{
						iterator new_location;
						new_location.element_pointer = erased_locations.top();
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), new_location.element_pointer, std::move(element));
						erased_locations.pop();
	
						new_location.group_pointer = end_iterator.group_pointer; // Start with last group first, as will be the largest group
				
						while (new_location.element_pointer < new_location.group_pointer->elements || new_location.element_pointer >= reinterpret_cast<element_pointer_type>(new_location.group_pointer->skipfield))
						{
							new_location.group_pointer = new_location.group_pointer->previous_group;
						}
	
						new_location.skipfield_pointer = new_location.group_pointer->skipfield + (new_location.element_pointer - new_location.group_pointer->elements);
						
						++(new_location.group_pointer->number_of_elements);
		
						if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
						{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
							begin_iterator = new_location;
						}
		
						++total_number_of_elements;
	
						// Pseudocode:
						// if !erased_locations.empty(), check whether location we are restoring to has a slot before or after which is erased
						// if it has only a slot before which is erased (ie. at end of erasure block), update the prime erasure point
						// if it only has a slot after it which is erased, (ie. this is the prime erasure point), change next slot to prime erasure point and update all subsequent erasure points (ie. decrement by 1)
						// if it has both a slot before and after which is erased (ie. is in middle of erasure block), do both of the above
						const unsigned char prev_skipfield = (new_location.skipfield_pointer != new_location.group_pointer->skipfield && *(new_location.skipfield_pointer - 1) != 0);
						const unsigned char after_skipfield = (*(new_location.skipfield_pointer + 1) != 0); // NOTE: first test removed due to extra slot in skipfield (compared to element field)
	
						switch (prev_skipfield | (after_skipfield << 1))
						{
							case 1: // previous erased consecutive elements, none following
							{
								*(new_location.skipfield_pointer - (*(new_location.skipfield_pointer) - 1)) = *(new_location.skipfield_pointer) - 1;
								break;
							}
							case 2: // No previous consecutive erased points, at least one following ie. this was the prime erasure point
							{
								skipfield_type update_count = *(new_location.skipfield_pointer) - 1;
								ushort_pointer_type following = new_location.skipfield_pointer + 1;
								*following = update_count;
								skipfield_type update_value = 2;
	
								skipfield_type vectorize = --update_count / 4;
								update_count -= vectorize * 4;
	
								while (vectorize != 0)
								{
									--vectorize;
	
									*(following + 1) = update_value;
									*(following + 2) = update_value + 1;
									*(following + 3) = update_value + 2;
									*(following + 4) = update_value + 3;
	
									following += 4;
									update_value += 4;
								}
	
								while (update_count != 0)
								{
									--update_count;
									*(++following) = update_value++;
								}
	
								break;
							}
							case 3: // both preceding and following consecutive erased elements
							{
								const skipfield_type value = *(new_location.skipfield_pointer);
								ushort_pointer_type start_node = new_location.skipfield_pointer - (value - 1);
								skipfield_type update_count = *start_node - value;
								*start_node = value - 1;
	
								ushort_pointer_type following = new_location.skipfield_pointer + 1;
								*following = update_count;
	
								skipfield_type update_value = 2;
	
								skipfield_type vectorize = --update_count / 4;
								update_count -= vectorize * 4;
								
								while (vectorize != 0)
								{
									--vectorize;
									
									*(following + 1) = update_value;
									*(following + 2) = update_value + 1;
									*(following + 3) = update_value + 2;
									*(following + 4) = update_value + 3;
	
									following += 4;
									update_value += 4;
								}
	
	
								while (update_count != 0)
								{
									--update_count;
									*(++following) = update_value++; // Create new series
								}
							}
						}
	
						*new_location.skipfield_pointer = 0;
						return new_location;
					}
				}
			}	
			else // ie. newly-constructed colony, no insertions yet and no groups
			{
				initialize();

				try
				{
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, std::move(element));
				}
				catch (...)
				{
					clear();
					throw;
				}

				++end_iterator.skipfield_pointer;
				++first_group->last_endpoint;
				++first_group->number_of_elements;
				++total_number_of_elements;

				return begin_iterator; /* returns value before incrementation */
			}
		}


		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			template<typename... Arguments> inline iterator emplace(Arguments... parameters)
			{
				return insert(std::move(element_type(std::forward<Arguments>(parameters)...))); // Use object's parameter'd constructor
			}
		#endif
	#endif



private:
	
	// Internal functions for insert-fill:
	void group_fill(const element_type &element, const skipfield_type number_of_elements)
	{
		const element_pointer_type fill_end = end_iterator.element_pointer + number_of_elements;
		
		while (end_iterator.element_pointer != fill_end)
		{
			try
			{
				PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
			}
			catch (...)
			{
				clear();
				throw;
			}
		}
		
		end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
		end_iterator.group_pointer->number_of_elements = number_of_elements;
	}
	


	void group_create()
	{
		const group_pointer_type next_group = end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
			#else 
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group(group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
			#endif
		}
		catch (...)
		{
			clear();
			throw;
		}

		end_iterator.group_pointer = next_group;
		end_iterator.element_pointer = next_group->elements;
	}
	
	
public:


	// Fill-insert
	
	iterator insert(const size_type number_of_elements, const element_type &element)
	{
		assert(number_of_elements != 0);
		
		if (first_group == NULL) // Empty colony, no groups created yet
		{
			if (number_of_elements > group_allocator_pair.max_elements_per_group)
			{
				// Create and fill first group:
				const skipfield_type temp = min_elements_per_group;
				min_elements_per_group = group_allocator_pair.max_elements_per_group;
				initialize(); // Construct first group
				min_elements_per_group = temp;
				group_fill(element, group_allocator_pair.max_elements_per_group);
				
				// Create and fill all remaining groups:
				size_type multiples = (number_of_elements / static_cast<size_type>(group_allocator_pair.max_elements_per_group));
				const unsigned short element_remainder = static_cast<skipfield_type>(number_of_elements - (multiples * static_cast<size_type>(group_allocator_pair.max_elements_per_group)));
				
				while (--multiples != 0)
				{
					group_create();
					group_fill(element, group_allocator_pair.max_elements_per_group);
				}
				
				if (element_remainder != 0)
				{
					group_create();
					group_fill(element, element_remainder);
				}

				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + element_remainder;
			}
			else
			{
				const skipfield_type temp = min_elements_per_group;
				min_elements_per_group = (number_of_elements < min_elements_per_group) ? min_elements_per_group : static_cast<skipfield_type>(number_of_elements);
				initialize(); // Construct first group
				min_elements_per_group = temp;
				
				group_fill(element, static_cast<skipfield_type>(number_of_elements));
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + number_of_elements;
			}

			total_number_of_elements += number_of_elements;
			return begin_iterator;
		}
		else
		{
			const iterator return_iterator = insert(element);
			size_type num_elements = number_of_elements - 1;
			size_type capacity_available = (reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) + erased_locations.size();
			
			// Use up erased locations and remainder of current group first:
			while (capacity_available-- != 0 && num_elements-- != 0)
			{
				insert(element);
			}

			// If still some left over, create new groups and fill:
			size_type multiples = (num_elements / static_cast<size_type>(group_allocator_pair.max_elements_per_group));
			const skipfield_type element_remainder = static_cast<skipfield_type>(num_elements - (multiples * static_cast<size_type>(group_allocator_pair.max_elements_per_group)));
			
			while (multiples-- != 0)
			{
				group_create();
				group_fill(element, group_allocator_pair.max_elements_per_group);
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + group_allocator_pair.max_elements_per_group;
			}
			
			if (element_remainder != 0)
			{
				group_create();
				group_fill(element, element_remainder);
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + element_remainder;
			}
			
			total_number_of_elements += num_elements;
			
			return return_iterator;
		}
	}
	


	// Range insert


	template <class iterator_type>
	iterator insert (const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type &first, const iterator_type &last)
	{
		if (first == last)
		{
			return iterator();
		}
		
		const iterator return_iterator = insert(*first);
		iterator_type current_element = first;
		
		while (++current_element != last)
		{
			insert(*current_element);
		}

		return return_iterator;
	}
	

	
	// Initializer-list insert

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		iterator insert (const std::initializer_list<element_type> &element_list)
		{
			if (element_list.size() == 0)
			{
				return iterator();
			}
			
			if (first_group == NULL)
			{
				initialize();
			}
			
			typename std::initializer_list<element_type>::iterator current_element = element_list.begin(), last = element_list.end();
				
			const iterator return_iterator = insert(*current_element);
			
			while(++current_element != last)
			{
				insert(*current_element);
			}
			
			return return_iterator;
		}
	#endif
	


private:

	void consolidate_erased_locations(const group_pointer_type the_group_pointer)
	{ 	
		// Remove all entries containing the provided colony group, from the erased_locations stack, also consolidate the stack removing unused trailing groups in the process.
		
		// Code explanation:
		// First, remove any trailing unused groups from the stack. These may be present if a stack has pushed then popped a lot, as plf::stack never deallocates.
		// If there're no entries from the supplied colony group in particular groups in the stack, preserve those groups (no alteration) in the new chain.
		// If there are entries from the supplied colony group in any group in the stack, copy these to the new stack group and remove the old group.
		// If the group is at the end of the old stack but is partial, and there have been some copies made from old groups, copy this group and don't preserve it (otherwise end up with a new group following a partially-full old group). If there're no copies (just old groups) preserve it.
		// At the end, if the new group has copies in it, add it to the chain. Or if the stack is now empty, reinitialize it. Or if there're no old groups remaining make the new group the first group. Or if there are only old groups link put the new chain into the stack to replace the old one.
		// Complicated but faster than any other alternative.

		typedef typename element_pointer_stack_type::group_pointer_type stack_group_pointer;
		typedef typename element_pointer_stack_type::pointer stack_element_pointer;

		#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			typedef typename element_pointer_stack_type::group_allocator_type stack_group_allocator_type; // Not used by PLF_COLONY_DESTROY etc when std::allocator_traits not supported
		#endif

		typename element_pointer_stack_type::ebco_pair &stack_group_allocator = erased_locations.group_allocator_pair;

		// Remove trailing stack groups (not removed in general 'pop' usage in plf::stack for performance reasons)
		erased_locations.trim_trailing_groups();

		// Determine what the size of erased_locations should be, based on the size of the first colony group:
		const size_type temp_size = (min_elements_per_group < 8) ? min_elements_per_group : (min_elements_per_group >> 7) + 8;

		// Note: All groups from here onwards refer to erased_location's stack groups, not colony groups, unless stated otherwise
		
		// Use either the size determined above or the current total number of elements in stack as an estimate of how large the first group in it should be:
		// The current total number of elements in the stack is a good estimate for how large it might become in future, but if it's smaller than the first_group determination, we should probably use the first_group determination.
		const size_type new_group_size = (erased_locations.total_number_of_elements < temp_size) ? temp_size : erased_locations.total_number_of_elements;

		stack_group_pointer current_old_group = erased_locations.first_group, 
							new_group = PLF_COLONY_ALLOCATE(stack_group_allocator_type, stack_group_allocator, 1, erased_locations.current_group), 
							first_new_chain = NULL,
							current_new_chain = NULL;

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(stack_group_allocator_type, stack_group_allocator, new_group, new_group_size, erased_locations.current_group);
			#else
				PLF_COLONY_CONSTRUCT(stack_group_allocator_type, stack_group_allocator, new_group, typename element_pointer_stack_type::group(new_group_size, erased_locations.current_group));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, new_group, 1);
			throw;
		}


		stack_element_pointer iterator_pointer, source_end, destination_begin = new_group->elements, the_end = NULL;
		const stack_element_pointer destination_start = destination_begin;
		const element_pointer_type group_elements_start = the_group_pointer->elements, group_elements_end = reinterpret_cast<element_pointer_type>(the_group_pointer->skipfield);
		size_type total_number_of_copies = 0, number_to_be_copied = 0;

		do // per old group of stack
		{
			if (current_old_group->next_group != NULL)
			{
				source_end = current_old_group->end + 1;
			}
			else
			{
				source_end = erased_locations.current_element + 1;
			}

			iterator_pointer = current_old_group->elements;
			const size_type number_of_group_elements = static_cast<const size_type>(source_end - iterator_pointer);

			for (; iterator_pointer != source_end; ++iterator_pointer) // per old group element
			{
				if (*iterator_pointer < group_elements_start || *iterator_pointer >= group_elements_end) // ie. this stack element doesn't match the skipfield)) // ie. this stack element doesn't match the group pointer, doesn't need to be removed
				{
					++number_to_be_copied;
				}
				else if (number_to_be_copied != 0) // Indicates a break in a run of elements matching the group_pointer, so copy time:
				{
					// Use the fastest method for moving iterators, while perserving values if allocator provides non-trivial pointers - unused if/else branches will be optimised out by any decent compiler:
					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (std::is_trivially_copyable<element_pointer_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
						{
							std::memcpy(&*destination_begin, &*(iterator_pointer - number_to_be_copied), number_to_be_copied * sizeof(element_pointer_type)); // &* to avoid problems with non-trivial pointers that are trivially-copyable
						}
						#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
							else if (std::is_move_constructible<element_pointer_type>::value)
							{
								std::uninitialized_copy(std::make_move_iterator(iterator_pointer - number_to_be_copied), std::make_move_iterator(iterator_pointer), destination_begin);
							}
						#endif
						else
					#endif
					{
						std::uninitialized_copy(iterator_pointer - number_to_be_copied, iterator_pointer, destination_begin);

						#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT // Typedef will not be used by macro if allocator_traits not supported
							typedef typename element_pointer_stack_type::allocator_type stack_allocator_type;
						#endif

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_COLONY_DESTROY(stack_allocator_type, erased_locations, element_pointer);
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
					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (std::is_trivially_copyable<element_pointer_type>::value)
						{ // Dereferencing here in order to deal with smart pointer situations ie. obtaining the raw pointer from the smart pointer
							std::memcpy(&*destination_begin, &*(iterator_pointer - number_to_be_copied), number_to_be_copied * sizeof(element_pointer_type));
						}
						#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
							else if (std::is_move_constructible<element_pointer_type>::value)
							{
								std::uninitialized_copy(std::make_move_iterator(iterator_pointer - number_to_be_copied), std::make_move_iterator(iterator_pointer), destination_begin);
							}
						#endif
						else
					#endif
					{
						std::uninitialized_copy(iterator_pointer - number_to_be_copied, iterator_pointer, destination_begin);

						#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
							typedef typename element_pointer_stack_type::allocator_type stack_allocator_type;
						#endif

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_COLONY_DESTROY(stack_allocator_type, erased_locations, element_pointer);
						}
					}

					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}

				// Remove old group:
				const stack_group_pointer prev_group = current_old_group;
				current_old_group = current_old_group->next_group;
				PLF_COLONY_DESTROY(stack_group_allocator_type, stack_group_allocator, prev_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, prev_group, 1);
			}
		} while (current_old_group != NULL);


		switch((current_new_chain != NULL) | ((destination_begin == destination_start) << 1))
		{
			case 0: // if (current_new_chain == NULL && destination_begin != destination_start) - No old groups remaining, some copies
			{
				new_group->previous_group = NULL;
				erased_locations.first_group = erased_locations.current_group = new_group;
				erased_locations.current_element = destination_begin - 1;
				erased_locations.start_element = new_group->elements;
				erased_locations.end_element = new_group->end;
				erased_locations.total_number_of_elements = total_number_of_copies;
				break;
			}
			case 1: // else if (current_new_chain != NULL && destination_begin != destination_start) - Some deletions occured ie. some elements copied, some old groups remaining - current_new_chain != NULL implied by previous if
			{
				new_group->previous_group = current_new_chain;
				erased_locations.current_group = current_new_chain->next_group = new_group;
				erased_locations.first_group = first_new_chain;
				erased_locations.current_element = destination_begin - 1;
				erased_locations.start_element = new_group->elements;
				erased_locations.end_element = new_group->end;
				erased_locations.total_number_of_elements = total_number_of_copies;
				break;
			}
			case 3: // else if (current_new_chain != NULL && destination_begin == destination_start) - No copies, some old groups - destination_begin == destination_start implied by previous if's
			{
				PLF_COLONY_DESTROY(stack_group_allocator_type, stack_group_allocator, new_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, stack_group_allocator, new_group, 1);

				current_new_chain->next_group = NULL;
				erased_locations.current_group = current_new_chain;
				erased_locations.first_group = first_new_chain;
				erased_locations.current_element = the_end;
				erased_locations.start_element = current_new_chain->elements;
				erased_locations.end_element = current_new_chain->end;
				erased_locations.total_number_of_elements = total_number_of_copies;
				break;
			}
			default: // else (current_new_chain == NULL && destination_begin == destination_start) - No elements remaining - least likely
			{
				// Instead of removing the newly-constructed group and returning to zero,
				// we bank on the idea that if an element has been removed, elements are likely to be removed later, and so keep the group rather than deleting it.

				erased_locations.current_group = new_group;
				erased_locations.first_group = new_group;
				erased_locations.current_element = new_group->elements - 1;
				erased_locations.start_element = new_group->elements;
				erased_locations.end_element = new_group->end;
				erased_locations.total_number_of_elements = 0;
				break;
			}
		}
	}



	inline void update_subsequent_group_numbers(group_pointer_type the_group) PLF_COLONY_NOEXCEPT
	{
		while (the_group != NULL)
		{
			--(the_group->group_number);
			the_group = the_group->next_group;
		}
	}



public:

	// Still must retain return iterator in case of full group erasure:
	iterator erase(const iterator &the_iterator)
	{
		assert(!empty());
		const group_pointer_type the_group_pointer = the_iterator.group_pointer;
		assert(the_group_pointer != NULL); // not uninitialized iterator
		group &the_group = *the_group_pointer;
		assert(the_iterator.element_pointer != the_group.last_endpoint); // not end()
		assert(*(the_iterator.skipfield_pointer) == 0); // element has not been previously erased

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (!(std::is_trivially_destructible<element_type>::value)) // This if-statement should be removed by the compiler on resolution of element_type. For some optimizing compilers this step won't be necessary (for MSVC 2013 it makes a difference)
		#endif
		{
			PLF_COLONY_DESTROY(element_allocator_type, (*this), the_iterator.element_pointer); // Destruct element
		}

		--total_number_of_elements;

		if (the_group.number_of_elements-- != 1) // ie. non-empty group at this point in time, don't consolidate
		{
			erased_locations.push(the_iterator.element_pointer);

			// Pseudocode:
			// If slot has no erased slots on either side, continue as usual
			// If slot has erased slot before it, add 1 to prev slot no. and use as skipfield_pointer. Update prime erasure point
			// If slot has erased slot after it but none before it, make this slot prime erasure slot and update subsequent slots
			// If slot has erasure slot before it and after it, remove secondary prime erasure slot and update all slots after this point

			iterator return_iterator;

			const unsigned char prev_skipfield = (the_iterator.skipfield_pointer != the_group.skipfield && *(the_iterator.skipfield_pointer - 1) != 0);
			const unsigned char after_skipfield = (the_iterator.element_pointer + 1 != the_group.last_endpoint && *(the_iterator.skipfield_pointer + 1) != 0);

			switch (prev_skipfield | (after_skipfield << 1))
			{
				case 0: // no consecutive erased elements
				{
					*the_iterator.skipfield_pointer = 1; // solo erase point

					return_iterator.group_pointer = the_iterator.group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + 1;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
					return_iterator.check_end_of_group_and_progress();
					break;
				}
				case 1: // previous erased consecutive elements, none following
				{
					*the_iterator.skipfield_pointer = *(the_iterator.skipfield_pointer - 1) + 1;
					++(*(the_iterator.skipfield_pointer - *(the_iterator.skipfield_pointer - 1)));

					return_iterator.group_pointer = the_iterator.group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + 1;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
					return_iterator.check_end_of_group_and_progress();
					break;
				}
				case 2: // following erased consecutive elements, none preceding
				{
					ushort_pointer_type following = the_iterator.skipfield_pointer + 1;
					skipfield_type update_count = *following;
					*the_iterator.skipfield_pointer = update_count + 1;
					skipfield_type update_value = 1;

					skipfield_type vectorize = update_count / 4;
					update_count -= vectorize * 4;

					while (vectorize-- != 0)
					{
						*(following) = update_value + 1;
						*(following + 1) = update_value + 2;
						*(following + 2) = update_value + 3;
						*(following + 3) = update_value + 4;

						following += 4;
						update_value += 4;
					}

					while (update_count-- != 0)
					{
						*(following++) = ++update_value;
					}

					return_iterator.group_pointer = the_iterator.group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + (following - the_iterator.skipfield_pointer);
					return_iterator.skipfield_pointer = following;
					return_iterator.check_end_of_group_and_progress();
					break;
				}
				case 3: // both preceding and following consecutive erased elements
				{
					ushort_pointer_type following = the_iterator.skipfield_pointer - 1;
					skipfield_type update_value = *following;
					skipfield_type update_count = *(following + 2) + 1;
					*(the_iterator.skipfield_pointer - update_value) += update_count;

					return_iterator.group_pointer = the_iterator.group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + update_count;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + update_count;
					return_iterator.check_end_of_group_and_progress();

					skipfield_type vectorize = update_count / 4;
					update_count -= vectorize * 4;

					while (vectorize-- != 0)
					{
						*(following + 1) = update_value + 1;
						*(following + 2) = update_value + 2;
						*(following + 3) = update_value + 3;
						*(following + 4) = update_value + 4;

						following += 4;
						update_value += 4;
					}

					while (update_count-- != 0)
					{
						*(++following) = ++update_value;
					}

					break;
				}
			}


			if (the_iterator == begin_iterator) // If original iterator was first element in colony, update it's value with the next non-skipfield element:
			{
				begin_iterator = return_iterator;
			}

			return return_iterator;
		}

		// else: consolidation of empty groups
		switch((the_group.next_group != NULL) | (the_group_pointer != first_group) << 1)
		{
			case 0: // ie. the_group_pointer == first_group && the_group.next_group == NULL; only group in colony
			{
				// Reset skipfield and erased_locations:
				std::memset(&*(first_group->skipfield), 0, sizeof(skipfield_type) * (first_group->size + 1)); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
				erased_locations.clear();

				// Reset begin_iterator:
				first_group->number_of_elements = 0;
				begin_iterator.element_pointer = first_group->last_endpoint = first_group->elements;
				begin_iterator.skipfield_pointer = first_group->skipfield;
				end_iterator = begin_iterator;

				return end_iterator;
			}
			case 1: // ie. the_group_pointer == first_group && the_group.next_group != NULL. Remove first group, change first group to next group
			{
				the_group.next_group->previous_group = NULL; // Cut off this group from the chain
				first_group = the_group.next_group; // Make the next group the first group

				// Update group numbers:
				first_group->group_number = 0;
				update_subsequent_group_numbers(first_group->next_group);

				if (!erased_locations.empty())
				{
					consolidate_erased_locations(the_group_pointer);
				}

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				begin_iterator.group_pointer = first_group; // note: end iterator only needs to be changed if the deleted group was the final group in the chain
				begin_iterator.skipfield_pointer = first_group->skipfield;
				// If the beginning index has been erased (ie. != 0), skip to next non-erased element:
				begin_iterator.element_pointer = first_group->elements + *(begin_iterator.skipfield_pointer);
				begin_iterator.skipfield_pointer += *(begin_iterator.skipfield_pointer);

				return begin_iterator;
			}
			case 3: // this is a non-first group but not final group in chain: the group is completely empty of elements, so delete the group, then link previous group's next-group field to the next non-empty group in the series, removing this link in the chain:
			{
				iterator return_iterator;

				the_group.next_group->previous_group = the_group.previous_group;
				return_iterator.group_pointer = the_group.previous_group->next_group = the_group.next_group; // close the chain, removing this group from it
				return_iterator.skipfield_pointer = return_iterator.group_pointer->skipfield;

				// If first element of next group is erased (ie. skipfield != 0), skip to the next non-erased element:
				return_iterator.element_pointer = return_iterator.group_pointer->elements + *(return_iterator.skipfield_pointer);
				return_iterator.skipfield_pointer += *(return_iterator.skipfield_pointer);


				// Update group numbers:
				update_subsequent_group_numbers(the_group.next_group);

				if (!erased_locations.empty())
				{
					consolidate_erased_locations(the_group_pointer);
				}

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				return return_iterator;
			}
			default: // this is a non-first group and the final group in the chain: the group is completely empty of elements, so delete the group, then set previous group's next-group to NULL:
			{
				the_group.previous_group->next_group = NULL;
				end_iterator.group_pointer = the_group.previous_group; // end iterator only needs to be changed if this is the final group in the chain
				end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
				end_iterator.skipfield_pointer = reinterpret_cast<ushort_pointer_type>(end_iterator.element_pointer) + end_iterator.group_pointer->size;

				if (!erased_locations.empty())
				{
					consolidate_erased_locations(the_group_pointer);
				}

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				return end_iterator;
			}
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



	int erase(const element_pointer_type the_pointer)
	{
		assert(!empty());

		group_pointer_type the_group = end_iterator.group_pointer; // Start with last group first, as will be the largest group

		while (the_group != NULL)
		{
			if (the_pointer >= the_group->elements && the_pointer < reinterpret_cast<element_pointer_type>(the_group->skipfield))
			{
				iterator the_iterator;
				the_iterator.group_pointer = the_group;
				the_iterator.element_pointer = the_pointer;
				the_iterator.skipfield_pointer = the_group->skipfield + (the_pointer - the_group->elements);
				erase(the_iterator);
				return 0;
			}

			the_group = the_group->previous_group;
		}

		return -1;
	}



	inline PLF_COLONY_FORCE_INLINE bool empty() const PLF_COLONY_NOEXCEPT
	{
		return total_number_of_elements == 0;
	}



	inline size_type size() const PLF_COLONY_NOEXCEPT
	{
		return total_number_of_elements;
	}



	inline size_type max_size() const PLF_COLONY_NOEXCEPT
	{
		return element_allocator_type::max_size();
	}



	inline size_type capacity() const PLF_COLONY_NOEXCEPT
	{
		return (first_group == NULL) ? 0 : (total_number_of_elements + static_cast<size_type>(erased_locations.size()) + 
			static_cast<size_type>(reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer));
	}



	size_type approximate_memory_use() const
	{
		return static_cast<size_type>(
			sizeof(*this) + // sizeof colony basic structure
			(erased_locations.approximate_memory_use() - sizeof(erased_locations)) +  // sizeof erased_locations stack (stack structure sizeof included in colony basic structure sizeof so negated from result)
			(capacity() * (sizeof(value_type) + sizeof(skipfield_type))) + // sizeof current colony data capacity + skipfields
			((end_iterator.group_pointer == NULL) ? 0 : ((end_iterator.group_pointer->group_number + 1) * (sizeof(group) + sizeof(skipfield_type))))); // if colony not empty, add the memory usage of the group structures themselves, adding the extra skipfield entry
	}


	inline void change_minimum_group_size(const skipfield_type min_allocation_amount)
	{
		change_group_sizes(min_allocation_amount, group_allocator_pair.max_elements_per_group);
	}
	
	

	inline void change_maximum_group_size(const skipfield_type max_allocation_amount)
	{
		change_group_sizes(min_elements_per_group, max_allocation_amount);
	}
	
	

	void change_group_sizes(const skipfield_type min_allocation_amount, const skipfield_type max_allocation_amount)
	{
		assert(min_allocation_amount > 2); // Otherwise, too much overhead for too small a group
		assert(min_allocation_amount <= max_allocation_amount);

		min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;

		if (first_group != NULL)
		{
			colony temp(*this);

			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(temp); // Avoid generating 2nd temporary
			#else
				this->swap(temp);
			#endif
		}

		erased_locations.clear();
		erased_locations.change_minimum_group_size((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8);
	}
	
		

	void clear() PLF_COLONY_NOEXCEPT
	{
		destroy_all_data();
		erased_locations.clear();
		total_number_of_elements = 0;
		begin_iterator.group_pointer = NULL;
		begin_iterator.element_pointer = NULL;
		begin_iterator.skipfield_pointer = NULL;
		end_iterator = begin_iterator;
	}



	inline colony & operator = (const colony &source)
	{
		assert (&source != this);

		destroy_all_data();
		colony temp(source);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			this->swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		colony & operator = (colony &&source) PLF_COLONY_NOEXCEPT
		{
			assert (&source != this);

			destroy_all_data();

			// Move source values across:
			end_iterator = std::move(source.end_iterator);
			begin_iterator = std::move(source.begin_iterator);
			first_group = std::move(source.first_group);
			total_number_of_elements = source.total_number_of_elements;
			min_elements_per_group = source.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
			erased_locations = std::move(source.erased_locations);

			source.first_group = NULL;
			source.total_number_of_elements = 0; // Nullifying the other data members is unnecessary - technically all can be removed except first_group NULL and total_number_of_elements 0, to allow for clean destructor usage
			return *this;
		}
	#endif



	bool operator == (const colony &rh) const PLF_COLONY_NOEXCEPT
	{
		assert (this != &rh);

		if (total_number_of_elements != rh.total_number_of_elements)
		{
			return false;
		}

		iterator rh_iterator = rh.begin_iterator;

		for (iterator the_iterator = begin_iterator; the_iterator != end_iterator;)
		{
			if (*rh_iterator++ != *the_iterator++)
			{
				return false;
			}
		}

		return true;
	}



	inline bool operator != (const colony &rh) const PLF_COLONY_NOEXCEPT
	{
		return !(*this == rh);
	}



	void shrink_to_fit()
	{
		if (first_group == NULL || total_number_of_elements == capacity())
		{
			return;
		}
		else if (total_number_of_elements == 0) // Edge case
		{
			clear();
			return;
		}

		const skipfield_type original_min_elements = min_elements_per_group;
		min_elements_per_group = (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? static_cast<skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group;
		min_elements_per_group = (min_elements_per_group < 3) ? 3 : min_elements_per_group;

		colony temp(*this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			this->swap(temp);
		#endif

		min_elements_per_group = original_min_elements;
	}



	void reserve(skipfield_type reserve_amount)
	{
		assert(reserve_amount > 2);

		if (reserve_amount > group_allocator_pair.max_elements_per_group)
		{
			reserve_amount = group_allocator_pair.max_elements_per_group;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = static_cast<skipfield_type>(max_size());
		}

		if (total_number_of_elements == 0) // Most common scenario
		{
			if (first_group != NULL) // Edge case - empty colony but first group is initialized
			{
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			} // else: Empty colony, no inserts as yet, time to allocate

			const skipfield_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;
			initialize();
			min_elements_per_group = original_min_elements;
		}
		else if (reserve_amount <= capacity())
		{
			return;
		}
		else
		{
			const skipfield_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;

			colony temp(*this);

			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(temp); // Avoid generating 2nd temporary
			#else
				this->swap(temp);
			#endif

			min_elements_per_group = original_min_elements;
		}
	}



	void swap(colony &source) PLF_COLONY_NOEXCEPT
	{
		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			colony temp(std::move(source));
			source = std::move(*this);
			*this = std::move(temp);
		#else
			iterator				swap_end_iterator = end_iterator, swap_begin_iterator = begin_iterator;
			group_pointer_type		swap_first_group = first_group;
			size_type				swap_total_number_of_elements = total_number_of_elements;
			skipfield_type 			swap_min_elements_per_group = min_elements_per_group, swap_max_elements_per_group = group_allocator_pair.max_elements_per_group;
			
			end_iterator = source.end_iterator;
			begin_iterator = source.begin_iterator;
			first_group = source.first_group;
			total_number_of_elements = source.total_number_of_elements;
			min_elements_per_group = source.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;

			source.end_iterator = swap_end_iterator;
			source.begin_iterator = swap_begin_iterator;
			source.first_group = swap_first_group;
			source.total_number_of_elements = swap_total_number_of_elements;
			source.min_elements_per_group = swap_min_elements_per_group;
			source.group_allocator_pair.max_elements_per_group = swap_max_elements_per_group;
			
			erased_locations.swap(source.erased_locations);
		#endif
	}	



	friend inline void swap (colony &a, colony &b) PLF_COLONY_NOEXCEPT
	{
		a.swap(b);
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

		assert(group_pointer != NULL); // covers uninitialized colony_iterator && empty group

		// Now, run code based on the nature of the distance type - negative, positive or zero:
		if (distance > 0) // ie. +=
		{
			// Code explanation:
			// For the initial state of the iterator, we don't know how what's been erased before it in the group.
			// So for the first group, we follow the following logic:
			// 1. If no elements have been erased in the group, we do simple addition to progress either to the end of the group, or if there is another group after it and the addition amount specified is larger than the distance between the iterator and the end of the group, we progress onto the next group.
			// 2. If any of the first group elements have been erased, we manually iterate, as we don't know whether the erased elements occur before or after the initial iterator position, subtracting 1 from the addition amount each time. Iteration continues until either the addition amount becomes zero, or we reach the end of the group.

			// For all subsequent groups, we follow the logic below:
			// 1. If the addition amount is larger than the total number of non-erased elements in the group, we skip the group and reduce the addition amount by the number of non-skipfield elements
			// 2. If the addition amount is smaller than the total number of non-erased elements in the group, then:
			//	  a. if there're no erased elements in the group we do simple addition to find the new location for the iterator
			//	  b. if there are erased elements in the group, manually iterate until the new iterator location is found ie. addition amount is reduced to zero


			assert (!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group == NULL)); // Check that we're not already at end()
			
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->elements)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<skipfield_type>(current_group.last_endpoint - current_group.elements)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->size)
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

						if ((distance -= distance_from_end) == 0)
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
						const skipfield_type difference = static_cast<const skipfield_type>(current_group.last_endpoint - element_pointer);
						element_pointer = current_group.last_endpoint;
						skipfield_pointer += difference;
						return;
					}

					group_pointer = group_pointer->next_group;
					skipfield_pointer = group_pointer->skipfield;
					element_pointer = group_pointer->elements + *skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;

					if ((distance -= current_group.number_of_elements) == 0)
					{
						return;
					}

					continue; // The only actual loop point
				}
				else if (current_group.number_of_elements == static_cast<const skipfield_type>(current_group.last_endpoint - current_group.elements))
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
			assert(!(element_pointer == group_pointer->elements && group_pointer->previous_group == NULL)); // check that we're not already at begin()
			distance = -distance;

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<const skipfield_type>(current_group.last_endpoint - current_group.elements)) // ie. full group
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
				else if (current_group.number_of_elements == static_cast<const skipfield_type>(current_group.last_endpoint - current_group.elements)) // ie. no erased elements in this group
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
		advance_implementation(it, distance);
	}


	template <class distance_type>
	void advance(const_iterator &it, distance_type distance)
	{
		advance_implementation(it, distance);
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
			assert (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Check that we're not already at rend()
			
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint)
			{
				const group &current_group = *group_pointer;

				if (current_group.number_of_elements == static_cast<const skipfield_type>(current_group.last_endpoint - current_group.elements))
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
				else if (current_group.number_of_elements == static_cast<const skipfield_type>(current_group.last_endpoint - current_group.elements))
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
			assert (!(element_pointer == group_pointer->last_endpoint - 1 && group_pointer->next_group == NULL)); // Check that we're not already at rbegin()				
			advance_implementation(it.the_iterator, -distance);
		}

		return;
	}


public:

	template <class distance_type>
	void advance(reverse_iterator &it, distance_type distance)
	{
		reverse_advance_implementation(it, distance);
	}


	template <class distance_type>
	void advance(const_reverse_iterator &it, distance_type distance)
	{
		reverse_advance_implementation(it, distance);
	}
	
	

	// Next implementations:
	inline iterator next(const iterator &it, typename iterator::difference_type distance = 1)
	{
		iterator return_iterator(it);
		advance_implementation(return_iterator, distance);
		return return_iterator;
	}
	

	inline const_iterator next(const const_iterator &it, typename const_iterator::difference_type distance = 1)
	{
		const_iterator return_iterator(it);
		advance_implementation(return_iterator, distance);
		return return_iterator;
	}


	inline reverse_iterator next(const reverse_iterator &it, typename reverse_iterator::difference_type distance = 1)
	{
		reverse_iterator return_iterator(it);
		reverse_advance_implementation(return_iterator, distance);
		return return_iterator;
	}
	

	inline const_reverse_iterator next(const const_reverse_iterator &it, typename const_reverse_iterator::difference_type distance = 1)
	{
		const_reverse_iterator return_iterator(it);
		reverse_advance_implementation(return_iterator, distance);
		return return_iterator;
	}


	

	// Prev implementations:
	inline iterator prev(const iterator &it, typename iterator::difference_type distance = 1)
	{
		iterator return_iterator(it);
		advance_implementation(return_iterator, -distance);
		return return_iterator;
	}
	

	inline const_iterator prev(const const_iterator &it, typename const_iterator::difference_type distance = 1)
	{
		const_iterator return_iterator(it);
		advance_implementation(return_iterator, -distance);
		return return_iterator;
	}


	inline reverse_iterator prev(const reverse_iterator &it, typename reverse_iterator::difference_type distance = 1)
	{
		reverse_iterator return_iterator(it);
		reverse_advance_implementation(return_iterator, -distance);
		return return_iterator;
	}
	

	inline const_reverse_iterator prev(const const_reverse_iterator &it, typename const_reverse_iterator::difference_type distance = 1)
	{
		const_reverse_iterator return_iterator(it);
		reverse_advance_implementation(return_iterator, -distance);
		return return_iterator;
	}
	


private:

	// distance implementation:
	template <class iterator_type>
	typename iterator_type::difference_type distance_implementation(const iterator_type &first, const iterator_type &last) PLF_COLONY_NOEXCEPT
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


		typedef typename iterator_type::difference_type diff_type;
		diff_type distance = 0;
		bool swap = false;

		iterator_type iterator1 = first, iterator2 = last;

		if (iterator1 > iterator2) // Less common case
		{
			iterator1 = last;
			iterator2 = first;
			swap = true;
		}


		if (iterator1.group_pointer != iterator2.group_pointer) // if not in same group, process intermediate groups
		{
			if (iterator1.group_pointer->number_of_elements == static_cast<skipfield_type>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // If no deletions in this group
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

		
		if (iterator1.group_pointer->number_of_elements == static_cast<skipfield_type>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // ie. no deletions in this group, direct subtraction is possible
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

	inline typename iterator::difference_type distance(const iterator &first, const iterator &last) PLF_COLONY_NOEXCEPT
	{
		return distance_implementation(first, last);
	}


	inline typename const_iterator::difference_type distance(const const_iterator &first, const const_iterator &last) PLF_COLONY_NOEXCEPT
	{
		return distance_implementation(first, last);
	}


	template <class reverse_iterator_type>
	inline typename reverse_iterator_type::difference_type reverse_distance_implementation(const reverse_iterator_type &first, const reverse_iterator_type &last) PLF_COLONY_NOEXCEPT
	{
		return distance_implementation(first.the_iterator, last.the_iterator);
	}


	inline typename reverse_iterator::difference_type distance(const reverse_iterator &first, const reverse_iterator &last) PLF_COLONY_NOEXCEPT
	{
		return reverse_distance_implementation(first, last);
	}


	inline typename const_reverse_iterator::difference_type distance(const const_reverse_iterator &first, const const_reverse_iterator &last) PLF_COLONY_NOEXCEPT
	{
		return reverse_distance_implementation(first, last);
	}

};	// colony


} // plf namespace




#undef PLF_COLONY_FORCE_INLINE

#undef PLF_COLONY_INITIALIZER_LIST_SUPPORT
#undef PLF_COLONY_TYPE_TRAITS_SUPPORT
#undef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_COLONY_VARIADICS_SUPPORT
#undef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
#undef PLF_COLONY_NOEXCEPT

#undef PLF_COLONY_CONSTRUCT
#undef PLF_COLONY_DESTROY
#undef PLF_COLONY_ALLOCATE
#undef PLF_COLONY_ALLOCATE_INITIALIZATION
#undef PLF_COLONY_DEALLOCATE


#endif // PLF_COLONY_H
