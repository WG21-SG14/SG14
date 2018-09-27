// Copyright (c) 2018, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

// zLib license (https://www.zlib.net/zlib_license.html):
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_COLONY_H
#define PLF_COLONY_H


// Compiler-specific defines used by colony:

#if defined(_MSC_VER)
	#define PLF_COLONY_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1600
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1700
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1800
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
		#define PLF_COLONY_ALIGNMENT_SUPPORT
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
	#define PLF_COLONY_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_COLONY_VARIADICS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || __GNUC__ < 4
			#define PLF_COLONY_NOEXCEPT throw()
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#elif __GNUC__ < 6
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
		#else // C++17 support
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
			#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ > 4
			#define PLF_COLONY_ALIGNMENT_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#endif

	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#if __GLIBCXX__ >= 20080606 	// libstdc++ 4.2 and below do not support variadic templates
			#define PLF_COLONY_VARIADICS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20090421 	// libstdc++ 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#elif __GLIBCXX__ >= 20120322
			#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
		#else
			#define PLF_COLONY_NOEXCEPT throw()
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#endif
		#if __GLIBCXX__ >= 20130322
			#define PLF_COLONY_ALIGNMENT_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#endif
	#elif defined(_LIBCPP_VERSION) // No type trait support in libc++ to date
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_ALIGNMENT_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
	#else // Assume type traits and initializer support for non-GCC compilers and standard libraries
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_ALIGNMENT_SUPPORT
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
	#endif

	#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
#else
	#define PLF_COLONY_FORCE_INLINE
	#define PLF_COLONY_NOEXCEPT throw()
	#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
	#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
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



#include <algorithm> // std::sort and std::fill_n

#include <cstring>	// memset, memmove
#include <cassert>	// assert
#include <limits>  // std::numeric_limits
#include <memory>	// std::allocator
#include <iterator> // std::bidirectional_iterator_tag
#include <cstddef>

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


template <class element_type, class element_allocator_type = std::allocator<element_type>, typename element_skipfield_type = unsigned short > class colony : private element_allocator_type  // Empty base class optimisation - inheriting allocator functions
// Note: unsigned short is equivalent to uint_least16_t ie. Using 16-bit unsigned integer in best-case scenario, greater-than-16-bit unsigned integer where platform doesn't support 16-bit types
{
public:
	// Standard container typedefs:
	typedef element_type																						value_type;
	typedef element_allocator_type																		allocator_type;
	typedef element_skipfield_type																		skipfield_type;

	#ifdef PLF_COLONY_ALIGNMENT_SUPPORT
		typedef typename std::aligned_storage<sizeof(element_type), (alignof(element_type) > sizeof(element_skipfield_type) ? alignof(element_type) : sizeof(element_skipfield_type))>::type	aligned_element_type;
	#else
		typedef element_type																				aligned_element_type;
	#endif

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::size_type							size_type;
		typedef typename std::allocator_traits<element_allocator_type>::difference_type 					difference_type;
		typedef element_type &																				reference;
		typedef const element_type &																		const_reference;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 							pointer;
		typedef typename std::allocator_traits<element_allocator_type>::const_pointer						const_pointer;
	#else
		typedef typename element_allocator_type::size_type			size_type;
		typedef typename element_allocator_type::difference_type	difference_type;
		typedef typename element_allocator_type::reference			reference;
		typedef typename element_allocator_type::const_reference	const_reference;
		typedef typename element_allocator_type::pointer			pointer;
		typedef typename element_allocator_type::const_pointer		const_pointer;
	#endif


	// Iterator declarations:
	template <bool is_const> class		colony_iterator;
	typedef colony_iterator<false>		iterator;
	typedef colony_iterator<true>		const_iterator;
	friend class colony_iterator<false>; // Using above typedef name here is illegal under C++03
	friend class colony_iterator<true>;

	template <bool r_is_const> class		colony_reverse_iterator;
	typedef colony_reverse_iterator<false>	reverse_iterator;
	typedef colony_reverse_iterator<true>	const_reverse_iterator;
	friend class colony_reverse_iterator<false>;
	friend class colony_reverse_iterator<true>;


private:


	struct group; // forward declaration for typedefs below

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<aligned_element_type>	aligned_element_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group>				group_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<skipfield_type>		skipfield_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<unsigned char>		uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.

		typedef typename std::allocator_traits<aligned_element_allocator_type>::pointer		aligned_pointer_type; // Different typedef to 'pointer' - this is a pointer to the overaligned element type, not the original element type
		typedef typename std::allocator_traits<group_allocator_type>::pointer 				group_pointer_type;
		typedef typename std::allocator_traits<skipfield_allocator_type>::pointer 			skipfield_pointer_type;
		typedef typename std::allocator_traits<uchar_allocator_type>::pointer				uchar_pointer_type;

		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<pointer>	pointer_allocator_type;
	#else
		typedef typename element_allocator_type::template rebind<aligned_element_type>::other	aligned_element_allocator_type;  // In case compiler supports alignment but not allocator_traits
		typedef typename element_allocator_type::template rebind<group>::other					group_allocator_type;
		typedef typename element_allocator_type::template rebind<skipfield_type>::other			skipfield_allocator_type;
		typedef typename element_allocator_type::template rebind<unsigned char>::other			uchar_allocator_type;

		typedef typename aligned_element_allocator_type::pointer	aligned_pointer_type;
		typedef typename group_allocator_type::pointer 				group_pointer_type;
		typedef typename skipfield_allocator_type::pointer 			skipfield_pointer_type;
		typedef typename uchar_allocator_type::pointer				uchar_pointer_type;

		typedef typename element_allocator_type::template rebind<pointer>::other pointer_allocator_type;
	#endif



	// Colony groups:
	struct group : private uchar_allocator_type	// Empty base class optimisation (EBCO) - inheriting allocator functions
	{
		aligned_pointer_type					last_endpoint; // The address that is one past the highest cell number that's been used so far in this group - does not change with erase command but may change with insert (if no previously-erased locations are available) - is necessary because an iterator cannot access the colony's end_iterator. Most-used variable in colony use (operator ++, --) so first in struct
		group_pointer_type					next_group; // Next group in the intrusive list of all groups. NULL if no next group
		const aligned_pointer_type			elements; // Element storage
		const skipfield_pointer_type		skipfield; // Skipfield storage. The element and skipfield arrays are allocated contiguously, hence the skipfield pointer also functions as a 'one-past-end' pointer for the elements array. There will always be one additional skipfield node allocated compared to the number of elements. This is to ensure a faster ++ iterator operation (fewer checks are required when this is present). The extra node is unused and always zero, but checked, and not having it will result in out-of-bounds memory errors.
		group_pointer_type					previous_group; // previous group in the intrusive list of all groups. NULL if no preceding group
		skipfield_type							free_list_head; // The index of the last erased element in the group. The last erased element will, in turn, contain the number of the index of the next erased element, and so on. If this is == maximum skipfield_type value then free_list is empty ie. no erasures have occurred in the group (or if they have, the erased locations have then been reused via insert()).
		const skipfield_type					size; // The element capacity of this particular group
		skipfield_type							number_of_elements; // indicates total number of active elements in group - changes with insert and erase commands - used to check for empty group in erase function, as an indication to remove the group
		group_pointer_type					erasures_list_next_group; // The next group in the intrusive singly-linked list of groups with erasures ie. with active erased-element free lists
		size_type								group_number; // Used for comparison (> < >= <=) iterator operators (used by distance function and user)


		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<aligned_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(aligned_element_type))) + ((elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is only initialised to element's base value initially, then incremented by one below */
				next_group(NULL),
				elements(last_endpoint++),
				skipfield(reinterpret_cast<skipfield_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				free_list_head(std::numeric_limits<skipfield_type>::max()),
				size(elements_per_group),
				number_of_elements(1),
				erasures_list_next_group(NULL),
				group_number((previous == NULL) ? 0 : previous->group_number + 1)
			{
				// Static casts to unsigned int from short not necessary as C++ automatically promotes lesser types for arithmetic purposes.
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (size + 1)); // &* to avoid problems with non-trivial pointers
			}

		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and losing performance) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<aligned_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(aligned_element_type))) + ((elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))),
				elements(NULL),
				skipfield(reinterpret_cast<skipfield_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				size(elements_per_group)
			{
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (size + 1));
			}



			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source) PLF_COLONY_NOEXCEPT:
				uchar_allocator_type(source),
				last_endpoint(source.last_endpoint + 1),
				next_group(NULL),
				elements(source.last_endpoint),
				skipfield(source.skipfield),
				previous_group(source.previous_group),
				free_list_head(std::numeric_limits<skipfield_type>::max()),
				size(source.size),
				number_of_elements(1),
				erasures_list_next_group(NULL),
				group_number((source.previous_group == NULL) ? 0 : source.previous_group->group_number + 1)
			{}
		#endif



		~group() PLF_COLONY_NOEXCEPT
		{
			// Null check not necessary (for copied group as above) as delete will ignore.
			PLF_COLONY_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), (size * sizeof(aligned_element_type)) + ((size + 1) * sizeof(skipfield_type)));
		}
	};




	// Implement const/non-const iterator switching pattern:
	template <bool flag, class is_true, class is_false> struct choose;

	template <class is_true, class is_false> struct choose<true, is_true, is_false>
	{
		typedef is_true type;
	};

	template <class is_true, class is_false> struct choose<false, is_true, is_false>
	{
		typedef is_false type;
	};


public:


	// Iterators:
	template <bool is_const> class colony_iterator
	{
	private:
		group_pointer_type		group_pointer;
		aligned_pointer_type	element_pointer;
		skipfield_pointer_type	skipfield_pointer;

	public:
		typedef std::bidirectional_iterator_tag 	iterator_category;
		typedef typename colony::value_type 		value_type;
		typedef typename colony::difference_type 	difference_type;
		typedef typename choose<is_const, typename colony::const_pointer, typename colony::pointer>::type		pointer;
		typedef typename choose<is_const, typename colony::const_reference, typename colony::reference>::type	reference;

		friend class colony;
		friend class colony_reverse_iterator<false>;
		friend class colony_reverse_iterator<true>;



		inline colony_iterator & operator = (const colony_iterator &source) PLF_COLONY_NOEXCEPT
		{
			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			skipfield_pointer = source.skipfield_pointer;
			return *this;
		}



		inline colony_iterator & operator = (const colony_iterator<!is_const> &source) PLF_COLONY_NOEXCEPT
		{
			group_pointer = source.group_pointer;
			element_pointer = source.element_pointer;
			skipfield_pointer = source.skipfield_pointer;
			return *this;
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// Move assignment - only really necessary if the allocator uses non-standard ie. smart pointers
			inline colony_iterator & operator = (colony_iterator &&source) PLF_COLONY_NOEXCEPT // Move is a copy in this scenario
			{
				assert (&source != this);
				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				skipfield_pointer = std::move(source.skipfield_pointer);
				return *this;
			}



			inline colony_iterator & operator = (colony_iterator<!is_const> &&source) PLF_COLONY_NOEXCEPT
			{
				assert (&source != this);
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



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
		}



		inline PLF_COLONY_FORCE_INLINE reference operator * () const // may cause exception with uninitialized iterator
		{
			return *(reinterpret_cast<pointer>(element_pointer));
		}



		inline PLF_COLONY_FORCE_INLINE pointer operator -> () const PLF_COLONY_NOEXCEPT
		{
			return reinterpret_cast<pointer>(element_pointer);
		}



#if defined(_MSC_VER) && _MSC_VER <= 1600 // MSVC 2010 needs a bit of a helping hand when it comes to optimizing
		inline PLF_COLONY_FORCE_INLINE colony_iterator & operator ++ ()
#else
		colony_iterator & operator ++ ()
#endif
		{
			assert(group_pointer != NULL); // covers uninitialised colony_iterator
			assert(!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL)); // Assert that iterator is not already at end()

			#if (defined(__GNUC__) && !defined(__clang__)) && (defined(__icelake_client__) || defined(__icelake_server__) || defined(__cannonlake__) || defined(__skylake_avx512__) || defined(__haswell__) || defined(__skylake__) || defined(__silvermont__) || defined(__sandybridge__) || defined(__ivybridge__) || defined(__broadwell__)) // faster under gcc on core i processors post-westmere
				skipfield_type skip = *(++skipfield_pointer);

				if ((element_pointer += skip + 1) == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
				{
					group_pointer = group_pointer->next_group;
					skip = *(group_pointer->skipfield);
					element_pointer = group_pointer->elements + skip;
					skipfield_pointer = group_pointer->skipfield;
				}

				skipfield_pointer += skip;
			#else
				const skipfield_type skip = *(++skipfield_pointer);
				skipfield_pointer += skip;

				if ((element_pointer += skip + 1) == group_pointer->last_endpoint && group_pointer->next_group != NULL)
				{
					group_pointer = group_pointer->next_group;
					const skipfield_type first_skip = *(group_pointer->skipfield);
					element_pointer = group_pointer->elements + first_skip;
					skipfield_pointer = group_pointer->skipfield + first_skip;
				}
			#endif

			return *this;
		}



		inline colony_iterator operator ++(int)
		{
			const colony_iterator copy(*this);
			++*this;
			return copy;
		}



	private:
		inline PLF_COLONY_FORCE_INLINE void check_for_end_of_group_and_progress() // used by erase
		{
			if (element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL)
			{
				group_pointer = group_pointer->next_group;
				const skipfield_type first_skip = *(group_pointer->skipfield);
				element_pointer = group_pointer->elements + first_skip;
				skipfield_pointer = group_pointer->skipfield + first_skip;
			}
		}



	public:

		colony_iterator & operator -- ()
		{
			assert(group_pointer != NULL);
			assert(!(element_pointer == group_pointer->elements && group_pointer->previous_group == NULL)); // Assert that we are not already at begin() - this is not required to be tested in the code below as we don't need a special condition to progress to begin(), like we do with end() in operator ++

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				const skipfield_type skip = *(--skipfield_pointer);
				skipfield_pointer -= skip;

				if ((element_pointer -= skip + 1) != group_pointer->elements - 1) // ie. iterator was not already at beginning of colony (with some previous consecutive deleted elements), and skipfield does not takes us into the previous group)
				{
					return *this;
				}
			}

			group_pointer = group_pointer->previous_group;
			skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
			const skipfield_type skip = *skipfield_pointer;
			element_pointer = (reinterpret_cast<colony::aligned_pointer_type>(group_pointer->skipfield) - 1) - skip;
			skipfield_pointer -= skip;

			return *this;
		}



		inline colony_iterator operator -- (int)
		{
			const colony_iterator copy(*this);
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



		inline bool operator > (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (((group_pointer == rh.group_pointer) && (element_pointer > rh.element_pointer)) || (group_pointer->group_number > rh.group_pointer->group_number));
		}



		inline bool operator < (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return rh > *this;
		}



		inline bool operator >= (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh > *this);
		}



		inline bool operator <= (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(*this > rh);
		}


		colony_iterator() PLF_COLONY_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), skipfield_pointer(NULL)  {}



	private:
		// Used by cend(), erase() etc:
		colony_iterator(const group_pointer_type group_p, const aligned_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: group_pointer(group_p), element_pointer(element_p), skipfield_pointer(skipfield_p) {}



	public:

		inline colony_iterator (const colony_iterator &source) PLF_COLONY_NOEXCEPT:
			group_pointer(source.group_pointer),
			element_pointer(source.element_pointer),
			skipfield_pointer(source.skipfield_pointer)
		{}


		inline colony_iterator(const colony_iterator<!is_const> &source) PLF_COLONY_NOEXCEPT:
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


			inline colony_iterator(colony_iterator<!is_const> &&source) PLF_COLONY_NOEXCEPT:
				group_pointer(std::move(source.group_pointer)),
				element_pointer(std::move(source.element_pointer)),
				skipfield_pointer(std::move(source.skipfield_pointer))
			{}
		#endif


	}; // colony_iterator





	// Reverse iterators:

	template <bool r_is_const> class colony_reverse_iterator
	{
	private:
		iterator it;

	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef typename colony::value_type 		value_type;
		typedef typename colony::difference_type 	difference_type;
		typedef typename choose<r_is_const, typename colony::const_pointer, typename colony::pointer>::type		pointer;
		typedef typename choose<r_is_const, typename colony::const_reference, typename colony::reference>::type	reference;

		friend class colony;


		inline colony_reverse_iterator& operator = (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT
		{
			it = source.it;
			return *this;
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move assignment
			inline colony_reverse_iterator& operator = (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT
			{
				it = std::move(source.it);
				return *this;
			}
		#endif



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it == rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it != rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE reference operator * () const PLF_COLONY_NOEXCEPT
		{
			return *(reinterpret_cast<pointer>(it.element_pointer));
		}



		inline PLF_COLONY_FORCE_INLINE pointer * operator -> () const PLF_COLONY_NOEXCEPT
		{
			return reinterpret_cast<pointer>(it.element_pointer);
		}



		// In this case we have to redefine the algorithm, rather than using the internal iterator's -- operator, in order for the reverse_iterator to be allowed to reach rend() ie. begin_iterator - 1
		colony_reverse_iterator & operator ++ ()
		{
			colony::group_pointer_type &group_pointer = it.group_pointer;
			colony::aligned_pointer_type &element_pointer = it.element_pointer;
			colony::skipfield_pointer_type &skipfield_pointer = it.skipfield_pointer;

			assert(group_pointer != NULL);
			assert(!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Assert that we are not already at rend()

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				const skipfield_type skip = *(--skipfield_pointer);
				skipfield_pointer -= skip;
				element_pointer -= skip + 1;

				if (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)) // ie. iterator is not == rend()
				{
					return *this;
				}
			}

			if (group_pointer->previous_group != NULL) // ie. not first group in colony
			{
				group_pointer = group_pointer->previous_group;
				skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
				const skipfield_type skip = *skipfield_pointer;
				element_pointer = (reinterpret_cast<colony::aligned_pointer_type>(group_pointer->skipfield) - 1) - skip;
				skipfield_pointer -= skip;
			}
			else // necessary so that reverse_iterator can end up == rend(), if we were already at first element in colony
			{
				--element_pointer;
				--skipfield_pointer;
			}

			return *this;
		}



		inline colony_reverse_iterator operator ++ (int)
		{
			const colony_reverse_iterator copy(*this);
			++*this;
			return copy;
		}



		inline PLF_COLONY_FORCE_INLINE colony_reverse_iterator & operator -- ()
		{
			assert(!(it.element_pointer == it.group_pointer->last_endpoint - 1 && it.group_pointer->next_group == NULL)); // ie. Check that we are not already at rbegin()
			++it;
			return *this;
		}



		inline colony_reverse_iterator operator -- (int)
		{
			const colony_reverse_iterator copy(*this);
			--*this;
			return copy;
		}



		inline typename colony::iterator base() const
		{
			return ++(typename colony::iterator(it));
		}



		inline bool operator > (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (rh.it > it);
		}



		inline bool operator < (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it > rh.it);
		}



		inline bool operator >= (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(it > rh.it);
		}



		inline bool operator <= (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh.it > it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it == rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it != rh.it);
		}



		inline bool operator > (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (rh.it > it);
		}



		inline bool operator < (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it > rh.it);
		}



		inline bool operator >= (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(it > rh.it);
		}



		inline bool operator <= (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh.it > it);
		}



		colony_reverse_iterator () PLF_COLONY_NOEXCEPT
		{}



		colony_reverse_iterator (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT:
			it(source.it)
		{}



		colony_reverse_iterator (const typename colony::iterator &source) PLF_COLONY_NOEXCEPT:
			it(source)
		{}



	private:
		// Used by rend(), etc:
		colony_reverse_iterator(const group_pointer_type group_p, const aligned_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: it(group_p, element_p, skipfield_p) {}



	public:

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move constructors
			colony_reverse_iterator (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT:
				it(std::move(source.it))
			{}

			colony_reverse_iterator (typename colony::iterator &&source) PLF_COLONY_NOEXCEPT:
				it(std::move(source))
			{}
		#endif

	}; // colony_reverse_iterator




private:

	// Used to prevent fill-insert/constructor calls being mistakenly resolved to range-insert/constructor calls
	template <bool condition, class T = void>
	struct plf_enable_if_c
	{
		typedef T type;
	};

	template <class T>
	struct plf_enable_if_c<false, T>
	{};


	iterator				end_iterator, begin_iterator;
	group_pointer_type		first_group, groups_with_erasures_list_head; // Head of a singly-linked intrusive list of groups which have erased-element memory locations available for reuse
	size_type				total_number_of_elements, total_capacity;

	struct ebco_pair2 : pointer_allocator_type // Packaging the element pointer allocator with a lesser-used member variable, for empty-base-class optimisation
	{
		skipfield_type min_elements_per_group;
		explicit ebco_pair2(const skipfield_type min_elements) PLF_COLONY_NOEXCEPT: min_elements_per_group(min_elements) {}
	}						pointer_allocator_pair;

	struct ebco_pair : group_allocator_type
	{
		skipfield_type max_elements_per_group;
		explicit ebco_pair(const skipfield_type max_elements) PLF_COLONY_NOEXCEPT: max_elements_per_group(max_elements) {}
	}						group_allocator_pair;


public:

	// Default constuctor:

	colony() PLF_COLONY_NOEXCEPT:
		element_allocator_type(element_allocator_type()),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair((sizeof(aligned_element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(aligned_element_type))),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max())
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed); // skipfield type must be of unsigned integer type (uchar, ushort, uint etc)

		#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
			assert(sizeof(element_type) >= sizeof(skipfield_type)); // eg. under C++03, aligned_storage is not available, so skipfield type must be larger or equal to element type size, otherwise element free lists will not work correctly. So if you're storing chars, for example, and using the default skipfield type (unsigned short), the compiler will flag you with this assert. Change your skipfield type to be unsigned char, or change your storage type to unsigned short or larger, or use C++11 and above and you'll be fine.
		#endif
	}



	// Default constuctor (allocator-extended):

	explicit colony(const element_allocator_type &alloc):
		element_allocator_type(alloc),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair((sizeof(aligned_element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(aligned_element_type))),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max())
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);

		#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
			assert(sizeof(element_type) >= sizeof(skipfield_type));
		#endif
	}



	// Copy constructor:

	colony(const colony &source):
		element_allocator_type(source),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>((source.pointer_allocator_pair.min_elements_per_group > source.total_number_of_elements) ? source.pointer_allocator_pair.min_elements_per_group : ((source.total_number_of_elements > source.group_allocator_pair.max_elements_per_group) ? source.group_allocator_pair.max_elements_per_group : source.total_number_of_elements))), // Make the first colony group size the greater of min_elements_per_group or total_number_of_elements, so long as total_number_of_elements isn't larger than max_elements_per_group
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		insert(source.begin_iterator, source.end_iterator);
		pointer_allocator_pair.min_elements_per_group = source.pointer_allocator_pair.min_elements_per_group; // reset to correct value for future clear() or erasures
	}



	// Copy constructor (allocator-extended):

	colony(const colony &source, const allocator_type &alloc):
		element_allocator_type(alloc),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>((source.pointer_allocator_pair.min_elements_per_group > source.total_number_of_elements) ? source.pointer_allocator_pair.min_elements_per_group : ((source.total_number_of_elements > source.group_allocator_pair.max_elements_per_group) ? source.group_allocator_pair.max_elements_per_group : source.total_number_of_elements))),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		insert(source.begin_iterator, source.end_iterator);
		pointer_allocator_pair.min_elements_per_group = source.pointer_allocator_pair.min_elements_per_group;
	}




private:

	inline void blank() PLF_COLONY_NOEXCEPT
	{
		#if defined(PLF_COLONY_TYPE_TRAITS_SUPPORT) && !(defined(__GNUC__) && (defined(__icelake_client__) || defined(__icelake_server__) || defined(__cannonlake__) || defined(__skylake_avx512__) || defined(__haswell__) || defined(__skylake__) || defined(__silvermont__) || defined(__sandybridge__) || defined(__ivybridge__) || defined(__broadwell__)))
			// this is faster under gcc if CPU is core2 and below, faster on MSVC/clang in-general:
			if (std::is_trivial<group_pointer_type>::value && std::is_trivial<aligned_pointer_type>::value && std::is_trivial<skipfield_pointer_type>::value && NULL == reinterpret_cast<void *>(0)) // if all pointer types are trivial, and NULL is zero, we can just nuke it from orbit with memset:
			{
				std::memset(reinterpret_cast<void *>(this), 0, offsetof(colony, pointer_allocator_pair));
			}
			else
		#endif
		{
			end_iterator.group_pointer = NULL;
			end_iterator.element_pointer = NULL;
			end_iterator.skipfield_pointer = NULL;
			begin_iterator.group_pointer = NULL;
			begin_iterator.element_pointer = NULL;
			begin_iterator.skipfield_pointer = NULL;
			first_group = NULL;
			groups_with_erasures_list_head = NULL;
			total_number_of_elements = 0;
			total_capacity = 0;
		}
	}



public:



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move constructor:

		colony(colony &&source) PLF_COLONY_NOEXCEPT:
			element_allocator_type(source),
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			first_group(std::move(source.first_group)),
			groups_with_erasures_list_head(std::move(source.groups_with_erasures_list_head)),
			total_number_of_elements(source.total_number_of_elements),
			total_capacity(source.total_capacity),
			pointer_allocator_pair(source.pointer_allocator_pair.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			source.blank();
		}


		// Move constructor (allocator-extended):

		colony(colony &&source, const allocator_type &alloc):
			element_allocator_type(alloc),
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			first_group(std::move(source.first_group)),
			groups_with_erasures_list_head(std::move(source.groups_with_erasures_list_head)),
			total_number_of_elements(source.total_number_of_elements),
			total_capacity(source.total_capacity),
			pointer_allocator_pair(source.pointer_allocator_pair.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			source.blank();
		}
	#endif



	// Fill constructor:

	colony(const size_type fill_number, const element_type &element, const skipfield_type min_allocation_amount = 0, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair((min_allocation_amount != 0) ? min_allocation_amount :
			(fill_number > max_allocation_amount) ? max_allocation_amount :
			(fill_number > 8) ? static_cast<skipfield_type>(fill_number) : 8),
		group_allocator_pair(max_allocation_amount)
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
		assert((pointer_allocator_pair.min_elements_per_group > 2) & (pointer_allocator_pair.min_elements_per_group <= group_allocator_pair.max_elements_per_group));

		#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
			assert(sizeof(element_type) >= sizeof(skipfield_type)); // see default constructor explanation
		#endif

		insert(fill_number, element);
	}



	// Range constructor:

	template<typename iterator_type>
	colony(const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type &first, const iterator_type &last, const skipfield_type min_allocation_amount = 8, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		groups_with_erasures_list_head(NULL),
		total_number_of_elements(0),
		total_capacity(0),
		pointer_allocator_pair(min_allocation_amount),
		group_allocator_pair(max_allocation_amount)
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
		assert((pointer_allocator_pair.min_elements_per_group > 2) & (pointer_allocator_pair.min_elements_per_group <= group_allocator_pair.max_elements_per_group));

		#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
			assert(sizeof(element_type) >= sizeof(skipfield_type)); // see default constructor explanation
		#endif

		insert<iterator_type>(first, last);
	}



	// Initializer-list constructor:

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		colony(const std::initializer_list<element_type> &element_list, const skipfield_type min_allocation_amount = 0, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
			element_allocator_type(alloc),
			first_group(NULL),
			groups_with_erasures_list_head(NULL),
			total_number_of_elements(0),
			total_capacity(0),
			pointer_allocator_pair((min_allocation_amount != 0) ? min_allocation_amount :
			(element_list.size() > max_allocation_amount) ? max_allocation_amount :
			(element_list.size() > 8) ? static_cast<skipfield_type>(element_list.size()) : 8),
			group_allocator_pair(max_allocation_amount)
		{
		 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
			assert((pointer_allocator_pair.min_elements_per_group > 2) & (pointer_allocator_pair.min_elements_per_group <= group_allocator_pair.max_elements_per_group));

			#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
				assert(sizeof(element_type) >= sizeof(skipfield_type));
			#endif

			insert(element_list);
		}

	#endif



	inline PLF_COLONY_FORCE_INLINE iterator begin() PLF_COLONY_NOEXCEPT
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const iterator & begin() const PLF_COLONY_NOEXCEPT // To allow for functions which only take const colony & as a source eg. copy constructor
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



	inline const_iterator cbegin() const PLF_COLONY_NOEXCEPT
	{
		return const_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer, begin_iterator.skipfield_pointer);
	}



	inline const_iterator cend() const PLF_COLONY_NOEXCEPT
	{
		return const_iterator(end_iterator.group_pointer, end_iterator.element_pointer, end_iterator.skipfield_pointer);
	}



	inline reverse_iterator rbegin() const // May throw exception if colony is empty so end_iterator is uninitialized
	{
		return ++reverse_iterator(end_iterator);
	}



	inline reverse_iterator rend() const PLF_COLONY_NOEXCEPT
	{
		return reverse_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer - 1, begin_iterator.skipfield_pointer - 1);
	}



	inline const_reverse_iterator crbegin() const
	{
		return ++const_reverse_iterator(end_iterator);
	}



	inline const_reverse_iterator crend() const PLF_COLONY_NOEXCEPT
	{
		return const_reverse_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer - 1, begin_iterator.skipfield_pointer - 1);
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
			total_number_of_elements = 0; // to avoid double-destruction
			aligned_pointer_type element_pointer = begin_iterator.element_pointer;
			skipfield_pointer_type skipfield_pointer = begin_iterator.skipfield_pointer;

			while (true)
			{
				const aligned_pointer_type end_pointer = first_group->last_endpoint;

				do
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(element_pointer));
					const skipfield_type skip = *(++skipfield_pointer);
					skipfield_pointer += skip;
					element_pointer += skip + 1;
				} while(element_pointer != end_pointer); // ie. beyond end of available data

				const group_pointer_type next_group = first_group->next_group;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
     			first_group = next_group; // required to be before if statement in order for first_group to be NULL and avoid potential double-destruction in future

				if (next_group == NULL)
				{
					return;
				}

				element_pointer = next_group->elements + *(next_group->skipfield);
				skipfield_pointer = next_group->skipfield + *(next_group->skipfield);
			}
		}
		else // Avoid iteration for both empty groups and trivially-destructible types eg. POD, structs, classes with empty destructors
		{
			// Technically under a type-traits-supporting compiler total_number_of_elements could be non-zero at this point, but since first_group would already be NULL in the case of double-destruction, it's unnecessary to zero total_number_of_elements

			while (first_group != NULL)
			{
				const group_pointer_type next_group = first_group->next_group;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
				first_group = next_group;
			}
		}
	}



	void initialize(const skipfield_type first_group_size)
	{
		first_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, first_group_size);
			#else
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(first_group_size));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = NULL;
			throw;
		}

		end_iterator.group_pointer = begin_iterator.group_pointer = first_group;
		end_iterator.element_pointer = begin_iterator.element_pointer = first_group->elements;
		end_iterator.skipfield_pointer = begin_iterator.skipfield_pointer = first_group->skipfield;
		total_capacity = first_group_size;
	}



public:


	iterator insert(const element_type &element)
	{
		if (end_iterator.element_pointer != NULL)
		{
			switch(((groups_with_erasures_list_head != NULL) << 1) | (end_iterator.element_pointer == reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield)))
			{
				case 0: // ie. there are no erased elements and end_iterator is not at end of current final group
				{
					const iterator return_iterator = end_iterator; /* Make copy for return before modifying end_iterator */

					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (std::is_nothrow_copy_constructible<element_type>::value) // For no good reason this compiles to faster code under GCC 7.3
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
							end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
						}
						else
					#endif
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), element);
						end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer; // Shift the addition to the second operation, avoiding problems if an exception is thrown during construction
					}

					++(end_iterator.group_pointer->number_of_elements);
					++end_iterator.skipfield_pointer;
					++total_number_of_elements;

					return return_iterator; // return value before incrementation
				}
				case 1: // ie. there are no erased elements and end_iterator is at end of current final group - ie. colony is full - create new group
				{
					end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
					group &next_group = *(end_iterator.group_pointer->next_group);
					const skipfield_type new_group_size = (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group;

					try
					{
						#ifdef PLF_COLONY_VARIADICS_SUPPORT
							PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, new_group_size, end_iterator.group_pointer);
						#else
							PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group(new_group_size, end_iterator.group_pointer));
						#endif
					}
					catch (...)
					{
						PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (std::is_nothrow_copy_constructible<element_type>::value)
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), element);
						}
						else
					#endif
					{
						try
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), element);
						}
						catch (...)
						{
							PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}
					}

					end_iterator.group_pointer = &next_group;
					end_iterator.element_pointer = next_group.last_endpoint;
					end_iterator.skipfield_pointer = next_group.skipfield + 1;
					++total_number_of_elements;
					total_capacity += new_group_size;

					return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield); /* returns value before incrementation */
				}
				default: // ie. there are erased elements, reuse previous-erased element locations
				{
					iterator new_location;
					new_location.group_pointer = groups_with_erasures_list_head;
					new_location.element_pointer = groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head;
					new_location.skipfield_pointer = groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head;
					groups_with_erasures_list_head->free_list_head = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));

					if (groups_with_erasures_list_head->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if this group does not currently have any erased elements and so is not part of the list of groups with erased elements
					{
						groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group;
					}

					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), element);

					++(new_location.group_pointer->number_of_elements);

					if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
					{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
						begin_iterator = new_location;
					}

					++total_number_of_elements;

					// Code logic for next section:
					// ============================
					// Check whether element location we are reusing has a skipfield node before or after which is skipped (erased)
					// if only the node to the left is skipped (ie. current node is at end of skipblock), update the start node of that skipblock
					// if only the node to the right is skipped (ie. current node is the start node of a skipblock to the right), change next node to be the start node of the skipblock and update all subsequent nodes (decrement each by 1)
					// if both left and right nodes are skipped (ie. current node is in middle of skipblock), do both of the above

					// Explanation of the following optimization: we must avoid testing the left-hand skipfield node if we are already at the beginning of the skipfield, otherwise we create an out-of-bounds memory access.
					// To avoid this would Normally require a branching test ie. !is_at_start && left-hand-node != 0 (&& and || operations are conditional executation of the right-hand instruction, which causes branching). But instead we subtract 'test' (which is 0 if the skipfield node is at start of skipfield, 1 if not) from the skipfield node.
					// If not start of skipfield, this means we check to see if left-hand node is == 0 (value * 0).
					// If at start of skipfield, we perform an unnecessary test to see if the current skipfield node's value (*(skipfield - 0))
					// is == it's own value (value * 1). This may seem ridiculous, but it's less costly than branching - and since
					// node == skipfield_start is almost always going to be false, this needless check is only occasional.

					const skipfield_type value = *(new_location.skipfield_pointer);
					const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
					const char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
					const char after_skipfield = *(new_location.skipfield_pointer + 1) != 0; // NOTE: test for whether we're at end of skipfield removed  - unnecessary due to extra unused node in skipfield (which is currently required by operator ++)

					// Now we create a switch for the four different possible states (there are no additional instructions for state 0) depending on
					// whether the left and right-hand skipfield nodes are non-zero (or out-of-bounds), by bit-shifting the right-hand test and
					// logical-OR'ing it with the left-hand test:
					switch (prev_skipfield | (after_skipfield << 1))
					{
						case 1: // previous erased consecutive elements, none following
						{
							*(new_location.skipfield_pointer - (value - 1)) = value - 1;
							break;
						}
						case 2: // No previous consecutive erased points, at least one following ie. this was the prime erasure point
						{
							std::memmove(&*(new_location.skipfield_pointer + 2), &*(new_location.skipfield_pointer + 1), sizeof(skipfield_type) * (value - 2));
							*(new_location.skipfield_pointer + 1) = value - 1;
							break;
						}
						case 3: // both preceding and following consecutive erased elements
						{
							const skipfield_pointer_type start_node = new_location.skipfield_pointer - (value - 1);
							const skipfield_type update_count = *start_node - value;
							*start_node = value - 1;

							std::memmove(&*(new_location.skipfield_pointer + 2), &*(start_node + 1), sizeof(skipfield_type) * (update_count - 1));
							*(new_location.skipfield_pointer + 1) = update_count;
						}
					}

					*new_location.skipfield_pointer = 0;
					return new_location;
				}
			}
		}
		else // ie. newly-constructed colony, no insertions yet and no groups
		{
			initialize(pointer_allocator_pair.min_elements_per_group);

			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (std::is_nothrow_copy_constructible<element_type>::value)
				{
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
				}
				else
			#endif
			{
				try
				{
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
				}
				catch (...)
				{
					clear();
					throw;
				}
			}

			++end_iterator.skipfield_pointer;
			total_number_of_elements = 1;
			return begin_iterator;
		}
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		iterator insert(element_type &&element) // The move-insert function is near-identical to the regular insert function, with the exception of the element construction method and is_nothrow tests.
		{
			if (end_iterator.element_pointer != NULL)
			{
				switch(((groups_with_erasures_list_head != NULL) << 1) | (end_iterator.element_pointer == reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield)))
				{
					case 0:
					{
						const iterator return_iterator = end_iterator;

						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if (std::is_nothrow_move_constructible<element_type>::value)
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
								end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
							}
							else
						#endif
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), std::move(element));
							end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer;
						}

						++(end_iterator.group_pointer->number_of_elements);
						++end_iterator.skipfield_pointer;
						++total_number_of_elements;

						return return_iterator;
					}
					case 1:
					{
						end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
						group &next_group = *(end_iterator.group_pointer->next_group);
						const skipfield_type new_group_size = (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group;

						try
						{
							#ifdef PLF_COLONY_VARIADICS_SUPPORT
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, new_group_size, end_iterator.group_pointer);
							#else
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group(new_group_size, end_iterator.group_pointer));
							#endif
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if (std::is_nothrow_move_constructible<element_type>::value)
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), std::move(element));
							}
							else
						#endif
						{
							try
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), std::move(element));
							}
							catch (...)
							{
								PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
								end_iterator.group_pointer->next_group = NULL;
								throw;
							}
						}

						end_iterator.group_pointer = &next_group;
						end_iterator.element_pointer = next_group.last_endpoint;
						end_iterator.skipfield_pointer = next_group.skipfield + 1;
						++total_number_of_elements;
						total_capacity += new_group_size;

						return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield);
					}
					default:
					{
						iterator new_location;
						new_location.group_pointer = groups_with_erasures_list_head;
						new_location.element_pointer = groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head;
						new_location.skipfield_pointer = groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head;
						groups_with_erasures_list_head->free_list_head = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));

						if (groups_with_erasures_list_head->free_list_head == std::numeric_limits<skipfield_type>::max())
						{
							groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group;
						}

						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), std::move(element));

						++(new_location.group_pointer->number_of_elements);

						if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
						{
							begin_iterator = new_location;
						}

						++total_number_of_elements;

						const skipfield_type value = *(new_location.skipfield_pointer);
						const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
						const char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
						const char after_skipfield = *(new_location.skipfield_pointer + 1) != 0;

						switch (prev_skipfield | (after_skipfield << 1))
						{
							case 1:
							{
								*(new_location.skipfield_pointer - (value - 1)) = value - 1;
								break;
							}
							case 2:
							{
								std::memmove(&*(new_location.skipfield_pointer + 2), &*(new_location.skipfield_pointer + 1), sizeof(skipfield_type) * (value - 2));
								*(new_location.skipfield_pointer + 1) = value - 1;
								break;
							}
							case 3:
							{
								const skipfield_pointer_type start_node = new_location.skipfield_pointer - (value - 1);
								const skipfield_type update_count = *start_node - value;
								*start_node = value - 1;

								std::memmove(&*(new_location.skipfield_pointer + 2), &*(start_node + 1), sizeof(skipfield_type) * (update_count - 1));
								*(new_location.skipfield_pointer + 1) = update_count;
							}
						}

						*new_location.skipfield_pointer = 0;
						return new_location;
					}
				}
			}
			else
			{
				initialize(pointer_allocator_pair.min_elements_per_group);

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (std::is_nothrow_move_constructible<element_type>::value)
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
					}
					else
				#endif
				{
					try
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
					}
					catch (...)
					{
						clear();
						throw;
					}
				}

				++end_iterator.skipfield_pointer;
				total_number_of_elements = 1;
				return begin_iterator;
			}
		}
	#endif




	#ifdef PLF_COLONY_VARIADICS_SUPPORT
		template<typename... arguments>
		iterator emplace(arguments &&... parameters) // The emplace function is near-identical to the regular insert function, with the exception of the element construction method, removal of internal VARIADICS support checks, and change to is_nothrow tests.
		{
			if (end_iterator.element_pointer != NULL)
			{
				switch(((groups_with_erasures_list_head != NULL) << 1) | (end_iterator.element_pointer == reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield)))
				{
					case 0:
					{
						const iterator return_iterator = end_iterator;

						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if (std::is_nothrow_constructible<element_type, arguments ...>::value)
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters)...);
								end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
							}
							else
						#endif
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), std::forward<arguments>(parameters)...);
							end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer;
						}

						++(end_iterator.group_pointer->number_of_elements);
						++end_iterator.skipfield_pointer;
						++total_number_of_elements;

						return return_iterator;
					}
					case 1:
					{
						end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
						group &next_group = *(end_iterator.group_pointer->next_group);
						const skipfield_type new_group_size = (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group;

						try
						{
							PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, new_group_size, end_iterator.group_pointer);
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if (std::is_nothrow_constructible<element_type, arguments ...>::value)
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), std::forward<arguments>(parameters)...);
							}
							else
						#endif
						{
							try
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(next_group.elements), std::forward<arguments>(parameters)...);
							}
							catch (...)
							{
								PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
								end_iterator.group_pointer->next_group = NULL;
								throw;
							}
						}

						end_iterator.group_pointer = &next_group;
						end_iterator.element_pointer = next_group.last_endpoint;
						end_iterator.skipfield_pointer = next_group.skipfield + 1;
						total_capacity += new_group_size;
						++total_number_of_elements;

						return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield);
					}
					default:
					{
						iterator new_location;
						new_location.group_pointer = groups_with_erasures_list_head;
						new_location.element_pointer = groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head;
						new_location.skipfield_pointer = groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head;
						groups_with_erasures_list_head->free_list_head = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));

						if (groups_with_erasures_list_head->free_list_head == std::numeric_limits<skipfield_type>::max())
						{
							groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group;
						}

						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), std::forward<arguments>(parameters) ...);

						++(new_location.group_pointer->number_of_elements);

						if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
						{
							begin_iterator = new_location;
						}

						++total_number_of_elements;

						const skipfield_type value = *(new_location.skipfield_pointer);
						const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
						const char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
						const char after_skipfield = *(new_location.skipfield_pointer + 1) != 0;

						switch (prev_skipfield | (after_skipfield << 1))
						{
							case 1:
							{
								*(new_location.skipfield_pointer - (value - 1)) = value - 1;
								break;
							}
							case 2:
							{
								std::memmove(&*(new_location.skipfield_pointer + 2), &*(new_location.skipfield_pointer + 1), sizeof(skipfield_type) * (value - 2));
								*(new_location.skipfield_pointer + 1) = value - 1;
								break;
							}
							case 3:
							{
								const skipfield_pointer_type start_node = new_location.skipfield_pointer - (value - 1);
								const skipfield_type update_count = *start_node - value;
								*start_node = value - 1;

								std::memmove(&*(new_location.skipfield_pointer + 2), &*(start_node + 1), sizeof(skipfield_type) * (update_count - 1));
								*(new_location.skipfield_pointer + 1) = update_count;
							}
						}

						*new_location.skipfield_pointer = 0;
						return new_location;
					}
				}
			}
			else
			{
				initialize(pointer_allocator_pair.min_elements_per_group);

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (std::is_nothrow_constructible<element_type, arguments ...>::value)
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters) ...);
					}
					else
				#endif
				{
					try
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters) ...);
					}
					catch (...)
					{
						clear();
						throw;
					}
				}

				++end_iterator.skipfield_pointer;
				total_number_of_elements = 1;
				return begin_iterator;
			}
		}
	#endif




private:

	// Internal functions for fill insert:

	void group_create(const skipfield_type number_of_elements)
	{
		const group_pointer_type next_group = end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, number_of_elements, end_iterator.group_pointer);
			#else
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group(number_of_elements, end_iterator.group_pointer));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, next_group);
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
			end_iterator.group_pointer->next_group = NULL;
			throw;
		}

		end_iterator.group_pointer = next_group;
		end_iterator.element_pointer = next_group->elements;
		total_capacity += number_of_elements;
	}



	void group_fill(const element_type &element, const skipfield_type number_of_elements)
	{
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (std::is_trivially_copyable<element_type>::value && std::is_trivially_copy_constructible<element_type>::value && std::is_nothrow_copy_constructible<element_type>::value) // ie. we can get away with using the cheaper fill_n here if there is no chance of an exception being thrown:
			{
				#ifdef PLF_COLONY_ALIGNMENT_SUPPORT
					if (sizeof(aligned_element_type) == sizeof(element_type)) // This statement should be resolved at compile-time
					{
						std::fill_n(reinterpret_cast<pointer>(end_iterator.element_pointer), number_of_elements, element);
					}
					else
					{
						alignas (sizeof(aligned_element_type)) element_type aligned_copy = element; // to avoid potentially violating memory boundaries in line below, create an initial copy object of same (but aligned) type
						std::fill_n(end_iterator.element_pointer, number_of_elements, *(reinterpret_cast<aligned_pointer_type>(&aligned_copy)));
					}
				#else // type is not aligned to anything so is safe to use fill_n anyway:
					std::fill_n(reinterpret_cast<pointer>(end_iterator.element_pointer), number_of_elements, element);
				#endif

				end_iterator.element_pointer += number_of_elements;
			}
			else
		#endif
		{
			const aligned_pointer_type fill_end = end_iterator.element_pointer + number_of_elements;

			do
			{
				try
				{
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
				}
				catch (...)
				{
					end_iterator.group_pointer->last_endpoint = --end_iterator.element_pointer;
					const skipfield_type elements_constructed_before_exception = static_cast<const skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);
					end_iterator.group_pointer->number_of_elements = elements_constructed_before_exception;
					end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + elements_constructed_before_exception;
					throw;
				}
			} while (end_iterator.element_pointer != fill_end);
		}

		end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
		end_iterator.group_pointer->number_of_elements = number_of_elements;
	}



public:

	// Fill insert

	void insert(size_type number_of_elements, const element_type &element)
	{
		if (number_of_elements == 0)
		{
			return;
		}
		else if (number_of_elements == 1)
		{
			insert(element);
			return;
		}

		if (first_group == NULL) // Empty colony, no groups created yet
		{
			initialize((number_of_elements > group_allocator_pair.max_elements_per_group) ? group_allocator_pair.max_elements_per_group : (number_of_elements < pointer_allocator_pair.min_elements_per_group) ? pointer_allocator_pair.min_elements_per_group : static_cast<skipfield_type>(number_of_elements)); // Construct first group
		}

		if (total_number_of_elements != 0) // ie. not an uninitialized colony or a situation where reserve has been called
		{
			// Use up erased locations:
			while (groups_with_erasures_list_head != NULL)
			{
				insert(element);

				if (--number_of_elements == 0)
				{
					return;
				}
			}

			const skipfield_type group_remainder = static_cast<skipfield_type>((static_cast<skipfield_type>(reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) > number_of_elements) ? number_of_elements : reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer);

			if (group_remainder != 0)
			{
				group_fill(element, group_remainder);
				end_iterator.group_pointer->number_of_elements = end_iterator.group_pointer->size;
				total_number_of_elements += group_remainder;
				number_of_elements -= group_remainder;
			}
		}
		else if (end_iterator.group_pointer->size >= number_of_elements)
		{
			group_fill(element, static_cast<skipfield_type>(number_of_elements));
			end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + number_of_elements;
			total_number_of_elements = number_of_elements;
			return;
		}
		else
		{
			group_fill(element, end_iterator.group_pointer->size);
			total_number_of_elements += end_iterator.group_pointer->size;
			number_of_elements -= end_iterator.group_pointer->size;
		}


		// If there's some elements left that need to be created, create new groups and fill:
		if (number_of_elements > group_allocator_pair.max_elements_per_group)
		{
			size_type multiples = (number_of_elements / static_cast<size_type>(group_allocator_pair.max_elements_per_group));
			const skipfield_type element_remainder = static_cast<const skipfield_type>(number_of_elements - (multiples * static_cast<size_type>(group_allocator_pair.max_elements_per_group)));

			while (multiples-- != 0)
			{
				group_create(group_allocator_pair.max_elements_per_group);
				group_fill(element, group_allocator_pair.max_elements_per_group);
			}

			if (element_remainder != 0)
			{
				group_create(group_allocator_pair.max_elements_per_group);
				group_fill(element, element_remainder);
			}
		}
		else if (number_of_elements != 0)
		{
			group_create(static_cast<skipfield_type>((number_of_elements > total_number_of_elements) ? number_of_elements : total_number_of_elements));
			group_fill(element, static_cast<skipfield_type>(number_of_elements));
		}

		total_number_of_elements += number_of_elements; // Adds the remainder from the last if-block - the insert functions in the first if/else block will already have incremented total_number_of_elements
		end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->number_of_elements;
	}



	// Range insert

	template <class iterator_type>
	inline void insert (typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type first, const iterator_type last)
	{
		while (first != last)
		{
			insert(*first++);
		}
	}



	// Initializer-list insert

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		inline void insert (const std::initializer_list<element_type> &element_list)
		{ // use range insert:
			insert(element_list.begin(), element_list.end());
		}
	#endif



private:

	inline PLF_COLONY_FORCE_INLINE void update_subsequent_group_numbers(group_pointer_type current_group) PLF_COLONY_NOEXCEPT
	{
		do
		{
			--(current_group->group_number);
			current_group = current_group->next_group;
		} while (current_group != NULL);
	}



	inline PLF_COLONY_FORCE_INLINE void consolidate() // get all elements contiguous in memory and shrink to fit, remove erasures and erasure free lists
	{
		colony temp(*this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif
	}



	void remove_from_groups_with_erasures_list(const group_pointer_type group_to_remove) PLF_COLONY_NOEXCEPT
	{
		if (group_to_remove == groups_with_erasures_list_head)
		{
			groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group;
			return;
		}

		group_pointer_type previous_group = groups_with_erasures_list_head, current_group = groups_with_erasures_list_head->erasures_list_next_group;

		while (group_to_remove != current_group)
		{
			previous_group = current_group;
			current_group = current_group->erasures_list_next_group;
		}

		previous_group->erasures_list_next_group = current_group->erasures_list_next_group;
	}



public:

	// must return iterator to subsequent non-erased element (or end()), in case the group containing the element which the iterator points to becomes empty after the erasure, and is thereafter removed from the colony chain, making the current iterator invalid and unusable in a ++ operation:
	iterator erase(const const_iterator &it) // if uninitialized/invalid iterator supplied, function could generate an exception
	{
		assert(!empty());
		const group_pointer_type group_pointer = it.group_pointer;
		assert(group_pointer != NULL); // ie. not uninitialized iterator
		assert(it.element_pointer != group_pointer->last_endpoint); // ie. != end()
		assert(*(it.skipfield_pointer) == 0); // ie. element pointed to by iterator has not been erased previously

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (!(std::is_trivially_destructible<element_type>::value)) // This if-statement should be removed by the compiler on resolution of element_type. For some optimizing compilers this step won't be necessary (for MSVC 2013 it makes a difference)
		#endif
		{
			PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(it.element_pointer)); // Destruct element
		}

		--total_number_of_elements;

		if (group_pointer->number_of_elements-- != 1) // ie. non-empty group at this point in time, don't consolidate - optimization note: GCC optimizes postfix + 1 comparison better than prefix + 1 comparison in many cases.
		{
			if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if this group does not currently have any erased elements
			{
				group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
				groups_with_erasures_list_head = group_pointer;
			}

			*(reinterpret_cast<skipfield_pointer_type>(it.element_pointer)) = group_pointer->free_list_head;
			group_pointer->free_list_head = static_cast<skipfield_type>(it.element_pointer - group_pointer->elements);

			// Code logic for following section:
			// ---------------------------------
			// If current skipfield node has no skipped node on either side, continue as usual
			// If node only has skipped node on left, set current node and start node of the skipblock to left node value + 1.
			// If node only has skipped node on right, make this node the start node of the skipblock and update subsequent nodes.
			// If node has skipped nodes on left and right, set current node to left node + 1, then update all nodes after the current node (add 1 per node from the current node's value)

			iterator return_iterator;

			// Optimization explanation:
			// The contextual logic below is the same as that in the insert() functions but in this case the value of the current skipfield node will always be
			// zero (since it is not yet erased), meaning no additional manipulations are necessary for the previous skipfield node comparison - we only have to check against zero
			const char prev_skipfield = *(it.skipfield_pointer - (it.skipfield_pointer != group_pointer->skipfield)) != 0;
			const char after_skipfield = *(it.skipfield_pointer + 1) != 0;  // NOTE: boundary test (checking against end-of-elements) is able to be skipped due to the extra skipfield node (compared to element field) - which is present to enable faster iterator operator ++ operations


			switch (prev_skipfield | (after_skipfield << 1))
			{
				case 0: // no consecutive erased elements
				{
					*it.skipfield_pointer = 1; // solo skipped node

					return_iterator.group_pointer = group_pointer;
					return_iterator.element_pointer = it.element_pointer + 1;
					return_iterator.skipfield_pointer = it.skipfield_pointer + 1;
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 1: // previous erased consecutive elements, none following
				{
					*it.skipfield_pointer = *(it.skipfield_pointer - 1) + 1;
					++(*(it.skipfield_pointer - *(it.skipfield_pointer - 1)));

					return_iterator.group_pointer = group_pointer;
					return_iterator.element_pointer = it.element_pointer + 1;
					return_iterator.skipfield_pointer = it.skipfield_pointer + 1;
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 2: // following erased consecutive elements, none preceding
				{
					const skipfield_type update_count = *(it.skipfield_pointer + 1);
					std::memmove(&*(it.skipfield_pointer + 1), &*(it.skipfield_pointer + 2), sizeof(skipfield_type) * (update_count - 1));
					*(it.skipfield_pointer + update_count) = *(it.skipfield_pointer) = update_count + 1;

					return_iterator.group_pointer = group_pointer;
					return_iterator.element_pointer = it.element_pointer + *(it.skipfield_pointer);
					return_iterator.skipfield_pointer = it.skipfield_pointer + *(it.skipfield_pointer);
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 3: // both preceding and following consecutive erased elements
				{
					skipfield_pointer_type following = it.skipfield_pointer - 1;
					skipfield_type update_value = *following;
					skipfield_type update_count = *(following + 2) + 1;
					*(it.skipfield_pointer - update_value) += update_count;

					return_iterator.group_pointer = group_pointer;
					return_iterator.element_pointer = it.element_pointer + update_count;
					return_iterator.skipfield_pointer = it.skipfield_pointer + update_count;
					return_iterator.check_for_end_of_group_and_progress();

					skipfield_type vectorize = update_count >> 2;
					update_count &= 3; // ie. % 4

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


			if (it == begin_iterator) // If original iterator was first element in colony, update it's value with the next non-erased element:
			{
				begin_iterator = return_iterator;
			}

			return return_iterator;
		}

		// else: group is empty, consolidate groups
		switch((group_pointer->next_group != NULL) | ((group_pointer != first_group) << 1))
		{
			case 0: // ie. group_pointer == first_group && group_pointer->next_group == NULL; only group in colony
			{
				// Reset skipfield and free list rather than clearing - leads to fewer allocations/deallocations:
				std::memset(&*(group_pointer->skipfield), 0, sizeof(skipfield_type) * group_pointer->size); // &* to avoid problems with non-trivial pointers. Although there is one more skipfield than group_pointer->size, size + 1 is not necessary here as the end skipfield is never written to after initialization
				group_pointer->free_list_head = std::numeric_limits<skipfield_type>::max();
				groups_with_erasures_list_head = NULL;

				// Reset begin and end iterators:
				end_iterator.element_pointer = begin_iterator.element_pointer = group_pointer->last_endpoint = group_pointer->elements;
				end_iterator.skipfield_pointer = begin_iterator.skipfield_pointer = group_pointer->skipfield;

				return end_iterator;
			}
			case 1: // ie. group_pointer == first_group && group_pointer->next_group != NULL. Remove first group, change first group to next group
			{
				group_pointer->next_group->previous_group = NULL; // Cut off this group from the chain
				first_group = group_pointer->next_group; // Make the next group the first group

				update_subsequent_group_numbers(first_group);

				if (group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // Erasures present within the group, ie. was part of the intrusive list of groups with erasures.
				{
					remove_from_groups_with_erasures_list(group_pointer);
				}

				total_capacity -= group_pointer->size;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, group_pointer, 1);

				begin_iterator.group_pointer = first_group; // note: end iterator only needs to be changed if the deleted group was the final group in the chain ie. not in this case
				begin_iterator.element_pointer = first_group->elements + *(first_group->skipfield); // If the beginning index has been erased (ie. skipfield != 0), skip to next non-erased element
				begin_iterator.skipfield_pointer = first_group->skipfield + *(first_group->skipfield);

				return begin_iterator;
			}
			case 3: // this is a non-first group but not final group in chain: delete the group, then link previous group to the next group in the chain:
			{
				group_pointer->next_group->previous_group = group_pointer->previous_group;
				const group_pointer_type return_group = group_pointer->previous_group->next_group = group_pointer->next_group; // close the chain, removing this group from it

				update_subsequent_group_numbers(return_group);

				if (group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(group_pointer);
				}

				total_capacity -= group_pointer->size;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, group_pointer, 1);

				// Return next group's first non-erased element:
				return iterator(return_group, return_group->elements + *(return_group->skipfield), return_group->skipfield + *(return_group->skipfield));
			}
			default: // this is a non-first group and the final group in the chain
			{
				if (group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(group_pointer);
				}

				group_pointer->previous_group->next_group = NULL;
				end_iterator.group_pointer = group_pointer->previous_group; // end iterator needs to be changed as element supplied was the back element of the colony
				end_iterator.element_pointer = reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield);
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->size;

				total_capacity -= group_pointer->size;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, group_pointer, 1);

				return end_iterator;
			}
		}
	}



	// Range-erase:

	void erase(const const_iterator &iterator1, const const_iterator &iterator2)  // if uninitialized/invalid iterators supplied, function could generate an exception. If iterator1 > iterator2, behaviour is undefined.
	{
		assert(iterator1 <= iterator2);

		iterator current = iterator1;

		if (current.group_pointer != iterator2.group_pointer)
		{
			if (current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // if iterator1 is not the first non-erased element in it's group - most common case
			{
				const aligned_pointer_type end = reinterpret_cast<aligned_pointer_type>(iterator1.group_pointer->last_endpoint);
				skipfield_type number_of_group_erasures = 0;

				// Now update skipfield:
				skipfield_type node_value = *(current.skipfield_pointer - 1); // Find value of left-hand node
				skipfield_type update_count = static_cast<skipfield_type>(end - current.element_pointer);

				*(current.skipfield_pointer - node_value) = node_value + update_count; // Either set current node as the start node, or if previous node is part of a skipblock, update that skipblock's start node and join to that skipblock

				if (node_value == 0)
				{
					node_value = 1;
					number_of_group_erasures = 1;

					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (!(std::is_trivially_destructible<element_type>::value))
					#endif
					{
						PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
					}

					if (current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if this group does not currently have any erased elements
					{
						current.group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
						groups_with_erasures_list_head = current.group_pointer;
					}

					*(reinterpret_cast<skipfield_pointer_type>(current.element_pointer)) = current.group_pointer->free_list_head;
					current.group_pointer->free_list_head = static_cast<skipfield_type>(current.element_pointer++ - current.group_pointer->elements);

					++current.skipfield_pointer;
					--update_count;
				}

				while (current.element_pointer != end)
				{
					if (*current.skipfield_pointer == 0)
					{
						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if (!(std::is_trivially_destructible<element_type>::value))
						#endif
						{
							PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
						}

						if (current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // this group is not currently part of the group free list
						{
							current.group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
							groups_with_erasures_list_head = current.group_pointer;
						}

						*(reinterpret_cast<skipfield_pointer_type>(current.element_pointer)) = current.group_pointer->free_list_head;
						current.group_pointer->free_list_head = static_cast<skipfield_type>(current.element_pointer - current.group_pointer->elements);

						++number_of_group_erasures;
					}

					*(current.skipfield_pointer++) = ++node_value;
					++current.element_pointer;
				}

				current.group_pointer->number_of_elements -= number_of_group_erasures;
				total_number_of_elements -= number_of_group_erasures;

				current.group_pointer = current.group_pointer->next_group;
			}


			// Intermediate groups:
			group_pointer_type current_group, previous_group = current.group_pointer->previous_group;

			while (current.group_pointer != iterator2.group_pointer)
			{
				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (!(std::is_trivially_destructible<element_type>::value)) // This should be removed by the compiler
				#endif
				{
					current.element_pointer = current.group_pointer->elements + *(current.group_pointer->skipfield);
					current.skipfield_pointer = current.group_pointer->skipfield + *(current.group_pointer->skipfield);
					const aligned_pointer_type end = current.group_pointer->last_endpoint;

					do
					{
						PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
						const skipfield_type skip = *(++current.skipfield_pointer);
						current.skipfield_pointer += skip;
						current.element_pointer += skip + 1;
					} while (current.element_pointer != end);
				}

				if (current.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(current.group_pointer);
				}

				total_number_of_elements -= current.group_pointer->number_of_elements;
				current_group = current.group_pointer;
				current.group_pointer = current.group_pointer->next_group;

				total_capacity -= current_group->size;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, current_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
			}

			current.element_pointer = current.group_pointer->elements + *(current.group_pointer->skipfield);
			current.skipfield_pointer = current.group_pointer->skipfield + *(current.group_pointer->skipfield);
			current.group_pointer->previous_group = previous_group;

			if (previous_group != NULL)
			{
				previous_group->next_group = current.group_pointer;
			}
			else
			{
				first_group = current.group_pointer;
				begin_iterator = iterator2; // This line is included here primarily to avoid a secondary if statement within the if block below - it will not be needed in any other situation
			}
		}

		if (current.element_pointer == iterator2.element_pointer) // also covers empty range case (first == last)
		{
			return;
		}

		// Final group:
		// Code explanation:
		// If not erasing entire final group, 1. Destruct elements (if non-trivial destructor) and add locations to group free list. 2. process skipfield.
		// If erasing entire group, 1. Destruct elements (if non-trivial destructor), 2. if no elements left in colony, clear() 3. otherwise reset end_iterator and remove group from groups-with-erasures list (if free list of erasures present)

		if (iterator2.element_pointer != end_iterator.element_pointer || current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // ie. not erasing entire group
		{
			skipfield_type number_of_group_erasures = 0;

			// Update skipfield:
			skipfield_type node_value = *(current.skipfield_pointer - (current.group_pointer->skipfield != current.skipfield_pointer)); // Find value of left-hand node - if current node is at start of skipfield, we check the current node instead, which will always be zero.
			skipfield_type update_count = static_cast<skipfield_type>(iterator2.element_pointer - current.element_pointer);

			*(current.skipfield_pointer - node_value) = node_value + update_count; // Either set current node as the start node, or if previous node is part of a skipblock, update that skipblock's start node

			if (node_value == 0)
			{
				node_value = 1;
				number_of_group_erasures = 1;

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (!(std::is_trivially_destructible<element_type>::value)) // This should be removed by the compiler
				#endif
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer));
				}

				if (current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if this group does not currently have any erased elements
				{
					current.group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
					groups_with_erasures_list_head = current.group_pointer;
				}

				*(reinterpret_cast<skipfield_pointer_type>(current.element_pointer)) = current.group_pointer->free_list_head;
				current.group_pointer->free_list_head = static_cast<skipfield_type>(current.element_pointer++ - current.group_pointer->elements);
				++current.skipfield_pointer;
				--update_count;
			}

			while (current.skipfield_pointer != iterator2.skipfield_pointer)
			{
				if (*(current.skipfield_pointer) == 0)
				{
					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (!(std::is_trivially_destructible<element_type>::value)) // This should be removed by the compiler
					#endif
					{
						PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer));
					}

					if (current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. this group is not currently part of the groups-with-erasures free list, add it to the list:
					{
						current.group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
						groups_with_erasures_list_head = current.group_pointer;
					}

					*(reinterpret_cast<skipfield_pointer_type>(current.element_pointer)) = current.group_pointer->free_list_head;
					current.group_pointer->free_list_head = static_cast<skipfield_type>(current.element_pointer - current.group_pointer->elements);
					++number_of_group_erasures;
				}

				++current.element_pointer;
				*(current.skipfield_pointer++) = ++node_value;
			}

			total_number_of_elements -= number_of_group_erasures;
			current.group_pointer->number_of_elements -= number_of_group_erasures;


			if (iterator1.element_pointer == begin_iterator.element_pointer) // Retaining this check for the case when iterator1 and iterator2 have the same group
			{
				begin_iterator = iterator2;
			}
		}
		else // ie. full group erasure
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (!(std::is_trivially_destructible<element_type>::value)) // This decision should be resolved at compile-time
			#endif
			{
				while(current.element_pointer != iterator2.element_pointer)
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer));
					const skipfield_type skip = *(++current.skipfield_pointer);
					current.skipfield_pointer += skip;
					current.element_pointer += skip + 1;
				}
			}


			if ((total_number_of_elements -= current.group_pointer->number_of_elements) != 0) // ie. previous_group != NULL
			{
				// Note: it is not possible that next_group != NULL at this point (if it were, iterator2 could not be == end_iterator in above if/else)
				current.group_pointer->previous_group->next_group = NULL;
				end_iterator.group_pointer = current.group_pointer->previous_group;
				end_iterator.element_pointer = current.group_pointer->previous_group->last_endpoint;
				end_iterator.skipfield_pointer = current.group_pointer->previous_group->skipfield + current.group_pointer->previous_group->size;
				total_capacity -= current.group_pointer->size;

				if (current.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(current.group_pointer);
				}
			}
			else // ie. colony is now empty
			{
				blank();
			}

			PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, current.group_pointer);
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current.group_pointer, 1);
		}
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
		return total_capacity;
	}



	inline size_type approximate_memory_use() const PLF_COLONY_NOEXCEPT
	{
		return
			sizeof(*this) + // sizeof colony basic structure
			(total_capacity * (sizeof(aligned_element_type) + sizeof(skipfield_type))) + // sizeof current colony data capacity + skipfields
			((end_iterator.group_pointer == NULL) ? 0 : ((end_iterator.group_pointer->group_number + 1) * (sizeof(group) + sizeof(skipfield_type)))); // if colony not empty, add the memory usage of the group structures themselves, adding the extra skipfield node
	}



	void change_group_sizes(const skipfield_type min_allocation_amount, const skipfield_type max_allocation_amount)
	{
		assert((min_allocation_amount > 2) & (min_allocation_amount <= max_allocation_amount));

		pointer_allocator_pair.min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;

		if (first_group != NULL && ((first_group->size < min_allocation_amount) || (end_iterator.group_pointer->size > max_allocation_amount)))
		{
			consolidate();
		}
	}



	inline void change_minimum_group_size(const skipfield_type min_allocation_amount)
	{
		change_group_sizes(min_allocation_amount, group_allocator_pair.max_elements_per_group);
	}



	inline void change_maximum_group_size(const skipfield_type max_allocation_amount)
	{
		change_group_sizes(pointer_allocator_pair.min_elements_per_group, max_allocation_amount);
	}



	inline void get_group_sizes(skipfield_type &minimum_group_size, skipfield_type &maximum_group_size) const PLF_COLONY_NOEXCEPT
	{
		minimum_group_size = pointer_allocator_pair.min_elements_per_group;
		maximum_group_size = group_allocator_pair.max_elements_per_group;
	}



	inline void reinitialize(const skipfield_type min_allocation_amount, const skipfield_type max_allocation_amount) PLF_COLONY_NOEXCEPT
	{
		assert((min_allocation_amount > 2) & (min_allocation_amount <= max_allocation_amount));
		pointer_allocator_pair.min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;
		clear();
	}



	inline PLF_COLONY_FORCE_INLINE void clear() PLF_COLONY_NOEXCEPT
	{
		destroy_all_data();
		blank();
	}



	inline colony & operator = (const colony &source)
	{
		assert (&source != this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			destroy_all_data();
			colony temp(source);
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			clear();
			colony temp(source);
			swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		colony & operator = (colony &&source) PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(allocator_type)
		{
			assert (&source != this);
			destroy_all_data();

			end_iterator = std::move(source.end_iterator);
			begin_iterator = std::move(source.begin_iterator);
			first_group = std::move(source.first_group);
			groups_with_erasures_list_head = source.groups_with_erasures_list_head;
			total_number_of_elements = source.total_number_of_elements;
			total_capacity = source.total_capacity;
			pointer_allocator_pair.min_elements_per_group = source.pointer_allocator_pair.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;

			source.blank();
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

		for (iterator lh_iterator = begin_iterator, rh_iterator = rh.begin_iterator; lh_iterator != end_iterator;)
		{
			if (*rh_iterator++ != *lh_iterator++)
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
		if (total_number_of_elements == total_capacity)
		{
			return;
		}
		else if (total_number_of_elements == 0) // Edge case
		{
			clear();
			return;
		}

		consolidate();
	}



	void reserve(const size_type original_reserve_amount)
	{
		assert(original_reserve_amount > 2);

		skipfield_type reserve_amount = static_cast<skipfield_type>(original_reserve_amount);

		if (original_reserve_amount > static_cast<size_type>(group_allocator_pair.max_elements_per_group))
		{
			reserve_amount = group_allocator_pair.max_elements_per_group;
		}
		else if (original_reserve_amount < static_cast<size_type>(pointer_allocator_pair.min_elements_per_group))
		{
			reserve_amount = pointer_allocator_pair.min_elements_per_group;
		}
		else if (original_reserve_amount > max_size())
		{
			reserve_amount = static_cast<skipfield_type>(max_size());
		}

		if (total_number_of_elements == 0) // Most common scenario
		{
			if (first_group != NULL) // Edge case - empty colony but first group is initialized ie. had some insertions but all elements got subsequently erased
			{
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			} // else: Empty colony, no insertions yet, time to allocate

			initialize(reserve_amount);
			first_group->last_endpoint = first_group->elements; // elements + 1 by default
			first_group->number_of_elements = 0; // 1 by default
		}
		else if (reserve_amount <= total_capacity)
		{
			return;
		}
		else
		{
			const skipfield_type original_min_elements = pointer_allocator_pair.min_elements_per_group;
			pointer_allocator_pair.min_elements_per_group = reserve_amount; // Make sure all groups are at maximum appropriate size
			consolidate();
			pointer_allocator_pair.min_elements_per_group = original_min_elements;
		}
	}



	// Advance implementation for iterator and const_iterator:
	template <bool is_const>
	void advance(colony_iterator<is_const> &it, difference_type distance) const // Cannot be noexcept due to the possibility of an uninitialized iterator
	{
		// For code simplicity - should hopefully be optimized out by compiler:
		group_pointer_type &group_pointer = it.group_pointer;
		aligned_pointer_type &element_pointer = it.element_pointer;
		skipfield_pointer_type &skipfield_pointer = it.skipfield_pointer;

		assert(group_pointer != NULL); // covers uninitialized colony_iterator && empty group

		// Now, run code based on the nature of the distance type - negative, positive or zero:
		if (distance > 0) // ie. +=
		{
			// Code explanation:
			// For the initial state of the iterator, we don't know how what elements have been erased before that element in that group.
			// So for the first group, we follow the following logic:
			// 1. If no elements have been erased in the group, we do simple addition to progress either to within the group (if the distance is small enough) or the end of the group and subtract from distance accordingly.
			// 2. If any of the first group elements have been erased, we manually iterate, as we don't know whether the erased elements occur before or after the initial iterator position, and we subtract 1 from the distance amount each time. Iteration continues until either distance becomes zero, or we reach the end of the group.

			// For all subsequent groups, we follow this logic:
			// 1. If distance is larger than the total number of non-erased elements in a group, we skip that group and subtract the number of elements in that group from distance
			// 2. If distance is smaller than the total number of non-erased elements in a group, then:
			//	  a. if there're no erased elements in the group we simply add distance to group->elements to find the new location for the iterator
			//	  b. if there are erased elements in the group, we manually iterate and subtract 1 from distance on each iteration, until the new iterator location is found ie. distance = 0

			// Note: incrementing element_pointer is avoided until necessary to avoid needless calculations

			assert (!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group == NULL)); // Check that we're not already at end()

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->elements + *(group_pointer->skipfield)) // ie. != first non-erased element in group
			{
				const difference_type distance_from_end = static_cast<const difference_type>(group_pointer->last_endpoint - element_pointer);

				if (group_pointer->number_of_elements == static_cast<skipfield_type>(distance_from_end)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->size)
				{
					if (distance < distance_from_end)
					{
						element_pointer += distance;
						skipfield_pointer += distance;
						return;
					}
					else if (group_pointer->next_group == NULL) // either we've reached end() or gone beyond it, so bound to end()
					{
						element_pointer = group_pointer->last_endpoint;
						skipfield_pointer += distance_from_end;
						return;
					}
					else
					{
						distance -= distance_from_end;
					}
				}
				else
				{
					const skipfield_pointer_type endpoint = skipfield_pointer + distance_from_end;

					while(true)
					{
						++skipfield_pointer;
						skipfield_pointer += *skipfield_pointer;
						--distance;

						if (skipfield_pointer == endpoint)
						{
							break;
						}
						else if (distance == 0)
						{
							element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
							return;
						}
					}

					if (group_pointer->next_group == NULL) // either we've reached end() or gone beyond it, so bound to end()
					{
						element_pointer = group_pointer->last_endpoint;
						return;
					}
				}

				group_pointer = group_pointer->next_group;

				if (distance == 0)
				{
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
			}


			// Intermediary groups - at the start of this code block and the subsequent block, the position of the iterator is assumed to be the first non-erased element in the current group:
			while (static_cast<difference_type>(group_pointer->number_of_elements) <= distance)
			{
				if (group_pointer->next_group == NULL) // either we've reached end() or gone beyond it, so bound to end()
				{
					element_pointer = group_pointer->last_endpoint;
					skipfield_pointer = group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements);
					return;
				}
				else if ((distance -= group_pointer->number_of_elements) == 0)
				{
					group_pointer = group_pointer->next_group;
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
				else
				{
					group_pointer = group_pointer->next_group;
				}
			}


			// Final group (if not already reached):
			if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // No erasures in this group, use straight pointer addition
			{
				element_pointer = group_pointer->elements + distance;
				skipfield_pointer = group_pointer->skipfield + distance;
				return;
			}
			else	 // ie. number_of_elements > distance - safe to ignore endpoint check condition while incrementing:
			{
				skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);

				do
				{
					++skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;
				} while(--distance != 0);

				element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
				return;
			}

			return;
		}
		else if (distance < 0) // for negative change
		{
			// Code logic is very similar to += above
			assert(!((element_pointer == group_pointer->elements + *(group_pointer->skipfield)) && group_pointer->previous_group == NULL)); // check that we're not already at begin()
			distance = -distance;

			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			if (element_pointer != group_pointer->last_endpoint) // ie. != end()
			{
				if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. no prior erasures have occurred in this group
				{
					const difference_type distance_from_beginning = static_cast<const difference_type>(element_pointer - group_pointer->elements);

					if (distance <= distance_from_beginning)
					{
						element_pointer -= distance;
						skipfield_pointer -= distance;
						return;
					}
					else if (group_pointer->previous_group == NULL) // ie. we've gone before begin(), so bound to begin()
					{
						element_pointer = group_pointer->elements;
						skipfield_pointer = group_pointer->skipfield;
						return;
					}
					else
					{
						distance -= distance_from_beginning;
					}
				}
				else
				{
					const skipfield_pointer_type beginning_point = group_pointer->skipfield + *(group_pointer->skipfield);

					while(skipfield_pointer != beginning_point)
					{
						--skipfield_pointer;
						skipfield_pointer -= *skipfield_pointer;

						if (--distance == 0)
						{
							element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
							return;
						}
					}

					if (group_pointer->previous_group == NULL)
					{
						element_pointer = group_pointer->elements + *(group_pointer->skipfield); // This is first group, so bound to begin() (just in case final decrement took us before begin())
						skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
						return;
					}
				}

				group_pointer = group_pointer->previous_group;
			}


			// Intermediary groups - at the start of this code block and the subsequent block, the position of the iterator is assumed to be either the first non-erased element in the next group over, or end():
			while(static_cast<difference_type>(group_pointer->number_of_elements) < distance)
			{
				if (group_pointer->previous_group == NULL) // we've gone beyond begin(), so bound to it
				{
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}

				distance -= group_pointer->number_of_elements;
				group_pointer = group_pointer->previous_group;
			}


			// Final group (if not already reached):
			if (static_cast<difference_type>(group_pointer->number_of_elements) == distance)
			{
				element_pointer = group_pointer->elements + *(group_pointer->skipfield);
				skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
				return;
			}
			else if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. no erased elements in this group
			{
				element_pointer = reinterpret_cast<aligned_pointer_type>(group_pointer->skipfield) - distance;
				skipfield_pointer = (group_pointer->skipfield + group_pointer->size) - distance;
				return;
			}
			else // ie. no more groups to traverse but there are erased elements in this group
			{
				skipfield_pointer = group_pointer->skipfield + group_pointer->size;

				do
				{
					--skipfield_pointer;
					skipfield_pointer -= *skipfield_pointer;
				} while(--distance != 0);

				element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
				return;
			}
		}

		return; // Only distance == 0 reaches here
	}




	// Advance for reverse_iterator and const_reverse_iterator - this needs to be implemented slightly differently to forward-iterator's advance, as it needs to be able to reach rend() (ie. begin() - 1) and to be bounded by rbegin():
	template <bool is_const>
	void advance(colony_reverse_iterator<is_const> &reverse_it, difference_type distance) const // could cause exception if iterator is uninitialized
	{
		group_pointer_type &group_pointer = reverse_it.it.group_pointer;
		aligned_pointer_type &element_pointer = reverse_it.it.element_pointer;
		skipfield_pointer_type &skipfield_pointer = reverse_it.it.skipfield_pointer;

		assert(element_pointer != NULL);

		if (distance > 0)
		{
			assert (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Check that we're not already at rend()
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			// Since a reverse_iterator cannot == last_endpoint (ie. before rbegin()) we don't need to check for that like with iterator
			if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
			{
				difference_type distance_from_beginning = static_cast<difference_type>(element_pointer - group_pointer->elements);

				if (distance <= distance_from_beginning)
				{
					element_pointer -= distance;
					skipfield_pointer -= distance;
					return;
				}
				else if (group_pointer->previous_group == NULL) // Either we've reached rend() or gone beyond it, so bound to rend()
				{
					element_pointer = group_pointer->elements - 1;
					skipfield_pointer = group_pointer->skipfield - 1;
					return;
				}
				else
				{
					distance -= distance_from_beginning;
				}
			}
			else
			{
				const skipfield_pointer_type beginning_point = group_pointer->skipfield + *(group_pointer->skipfield);

				while(skipfield_pointer != beginning_point)
				{
					--skipfield_pointer;
					skipfield_pointer -= *skipfield_pointer;

					if (--distance == 0)
					{
						element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
						return;
					}
				}

				if (group_pointer->previous_group == NULL)
				{
					element_pointer = group_pointer->elements - 1; // If we've reached rend(), bound to that
					skipfield_pointer = group_pointer->skipfield - 1;
					return;
				}
			}

			group_pointer = group_pointer->previous_group;


			// Intermediary groups - at the start of this code block and the subsequent block, the position of the iterator is assumed to be the first non-erased element in the next group:
			while(static_cast<difference_type>(group_pointer->number_of_elements) < distance)
			{
				if (group_pointer->previous_group == NULL) // bound to rend()
				{
					element_pointer = group_pointer->elements - 1;
					skipfield_pointer = group_pointer->skipfield - 1;
					return;
				}

				distance -= static_cast<difference_type>(group_pointer->number_of_elements);
				group_pointer = group_pointer->previous_group;
			}


			// Final group (if not already reached)
			if (static_cast<difference_type>(group_pointer->number_of_elements) == distance)
			{
				element_pointer = group_pointer->elements + *(group_pointer->skipfield);
				skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
				return;
			}
			else if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
			{
				element_pointer = reinterpret_cast<aligned_pointer_type>(group_pointer->skipfield) - distance;
				skipfield_pointer = (group_pointer->skipfield + group_pointer->size) - distance;
				return;
			}
			else
			{
				skipfield_pointer = group_pointer->skipfield + group_pointer->size;

				do
				{
					--skipfield_pointer;
					skipfield_pointer -= *skipfield_pointer;
				} while(--distance != 0);

				element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
				return;
			}
		}
		else if (distance < 0)
		{
			assert (!((element_pointer == (group_pointer->last_endpoint - 1) - *(group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements) - 1)) && group_pointer->next_group == NULL)); // Check that we're not already at rbegin()

			if (element_pointer != group_pointer->elements + *(group_pointer->skipfield)) // ie. != first non-erased element in group
			{
				if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if there are no erasures in the group
				{
					const difference_type distance_from_end = static_cast<const difference_type>(group_pointer->last_endpoint - element_pointer);

					if (distance < distance_from_end)
					{
						element_pointer += distance;
						skipfield_pointer += distance;
						return;
					}
					else if (group_pointer->next_group == NULL) // bound to rbegin()
					{
						element_pointer = group_pointer->last_endpoint - 1; // no erasures so we don't have to subtract skipfield value as we do below
						skipfield_pointer += distance_from_end - 1;
						return;
					}
					else
					{
						distance -= distance_from_end;
					}
				}
				else
				{
					const skipfield_pointer_type endpoint = skipfield_pointer + (group_pointer->last_endpoint - element_pointer);

					while(true)
					{
						++skipfield_pointer;
						skipfield_pointer += *skipfield_pointer;
						--distance;

						if (skipfield_pointer == endpoint)
						{
							break;
						}
						else if (distance == 0)
						{
							element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
							return;
						}
					}

					if (group_pointer->next_group == NULL) // bound to rbegin()
					{
						const skipfield_type skip = *(--skipfield_pointer);
						skipfield_pointer -= skip;
						element_pointer = (group_pointer->last_endpoint - 1) - skip;
						return;
					}
				}

				group_pointer = group_pointer->next_group;

				if (distance == 0)
				{
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
			}


			// Intermediary groups - at the start of this code block and the subsequent block, the position of the iterator is assumed to be the first non-erased element in the current group, as a result of the previous code blocks:
			while(static_cast<difference_type>(group_pointer->number_of_elements) <= distance)
			{
				if (group_pointer->next_group == NULL) // bound to rbegin()
				{
					skipfield_pointer = group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements) - 1;
					const skipfield_type skip = *(--skipfield_pointer);
					skipfield_pointer -= skip;
					element_pointer = (group_pointer->last_endpoint - 1) - skip;
					return;
				}
				else if ((distance -= group_pointer->number_of_elements) == 0)
				{
					group_pointer = group_pointer->next_group;
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
				else
				{
					group_pointer = group_pointer->next_group;
				}
			}


			// Final group (if not already reached):
			if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // No erasures in this group, use straight pointer addition
			{
				element_pointer = group_pointer->elements + distance;
				skipfield_pointer = group_pointer->skipfield + distance;
				return;
			}
			else // ie. number_of_elements > distance - safe to ignore endpoint check condition while incrementing:
			{
				skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);

				do
				{
					++skipfield_pointer;
					skipfield_pointer += *skipfield_pointer;
				} while(--distance != 0);

				element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
				return;
			}

			return;
		}
	}




	// Next implementations:
	template <bool is_const>
	inline colony_iterator<is_const> next(const colony_iterator<is_const> &it, const typename colony_iterator<is_const>::difference_type distance = 1) const
	{
		colony_iterator<is_const> return_iterator(it);
		advance(return_iterator, distance);
		return return_iterator;
	}



	template <bool is_const>
	inline colony_reverse_iterator<is_const> next(const colony_reverse_iterator<is_const> &it, const typename colony_reverse_iterator<is_const>::difference_type distance = 1) const
	{
		colony_reverse_iterator<is_const> return_iterator(it);
		advance(return_iterator, distance);
		return return_iterator;
	}



	// Prev implementations:
	template <bool is_const>
	inline colony_iterator<is_const> prev(const colony_iterator<is_const> &it, const typename colony_iterator<is_const>::difference_type distance = 1) const
	{
		colony_iterator<is_const> return_iterator(it);
		advance(return_iterator, -distance);
		return return_iterator;
	}



	template <bool is_const>
	inline colony_reverse_iterator<is_const> prev(const colony_reverse_iterator<is_const> &it, const typename colony_reverse_iterator<is_const>::difference_type distance = 1) const
	{
		colony_reverse_iterator<is_const> return_iterator(it);
		advance(return_iterator, -distance);
		return return_iterator;
	}



	// distance implementation:

	template <bool is_const>
	typename colony_iterator<is_const>::difference_type distance(const colony_iterator<is_const> &first, const colony_iterator<is_const> &last) const
	{
		// Code logic:
		// If iterators are the same, return 0
		// Otherwise, find which iterator is later in colony, copy that to iterator2. Copy the lower to iterator1.
		// If they are not pointing to elements in the same group, process the intermediate groups and add distances,
		// skipping manual incrementation in all but the initial and final groups.
		// In the initial and final groups, manual incrementation must be used to calculate distance, if there have been no prior erasures in those groups.
		// If there are no prior erasures in either of those groups, we can use pointer arithmetic to calculate the distances for those groups.

		assert(!(first.group_pointer == NULL) && !(last.group_pointer == NULL));  // Check that they are initialized

		if (last.element_pointer == first.element_pointer)
		{
			return 0;
		}

		typedef colony_iterator<is_const> iterator_type;
		typedef typename iterator_type::difference_type diff_type;
		diff_type distance = 0;

		iterator_type iterator1 = first, iterator2 = last;
		const bool swap = first > last;

		if (swap) // Less common case
		{
			iterator1 = last;
			iterator2 = first;
		}

		if (iterator1.group_pointer != iterator2.group_pointer) // if not in same group, process intermediate groups
		{
			// Process initial group:
			if (iterator1.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // If no prior erasures have occured in this group we can do simple addition
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->last_endpoint - iterator1.element_pointer);
			}
			else if (iterator1.element_pointer == iterator1.group_pointer->elements) // ie. element is at start of group - rare case
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements);
			}
			else
			{
				const skipfield_pointer_type endpoint = iterator1.skipfield_pointer + (iterator1.group_pointer->last_endpoint - iterator1.element_pointer);

				while (iterator1.skipfield_pointer != endpoint)
				{
					iterator1.skipfield_pointer += *(++iterator1.skipfield_pointer);
					++distance;
				}
			}

			// Process all other intermediate groups:
			iterator1.group_pointer = iterator1.group_pointer->next_group;

			while (iterator1.group_pointer != iterator2.group_pointer)
			{
				distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements);
				iterator1.group_pointer = iterator1.group_pointer->next_group;
			}

			iterator1.skipfield_pointer = iterator1.group_pointer->skipfield;
		}


		if (iterator1.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. no erasures in this group, direct subtraction is possible
		{
			distance += static_cast<diff_type>(iterator2.skipfield_pointer - iterator1.skipfield_pointer);
		}
		else if (iterator1.group_pointer->last_endpoint - 1 >= iterator2.element_pointer) // ie. if iterator2 is .end() or 1 before
		{
			distance += static_cast<diff_type>(iterator1.group_pointer->number_of_elements - (iterator1.group_pointer->last_endpoint - iterator2.element_pointer));
		}
		else
		{
			while (iterator1.skipfield_pointer != iterator2.skipfield_pointer)
			{
				iterator1.skipfield_pointer += *(++iterator1.skipfield_pointer);
				++distance;
			}
		}


		if (swap)
		{
			distance = -distance;
		}

		return distance;
	}



	template <bool is_const>
	inline typename colony_reverse_iterator<is_const>::difference_type distance(const colony_reverse_iterator<is_const> &first, const colony_reverse_iterator<is_const> &last) const
	{
		return distance(last.it, first.it);
	}




	// Type-changing functions:

	iterator get_iterator_from_pointer(const pointer element_pointer) const // Cannot be noexcept as colony could be empty or pointer invalid
	{
		assert(!empty());
		assert(element_pointer != NULL);

		group_pointer_type current_group = end_iterator.group_pointer; // Start with last group first, as will be the largest group

		while (current_group != NULL)
		{
			if (reinterpret_cast<aligned_pointer_type>(element_pointer) >= current_group->elements && reinterpret_cast<aligned_pointer_type>(element_pointer) < reinterpret_cast<aligned_pointer_type>(current_group->skipfield))
			{
				const skipfield_pointer_type skipfield_pointer = current_group->skipfield + (reinterpret_cast<aligned_pointer_type>(element_pointer) - current_group->elements);
				return (*skipfield_pointer == 0) ? iterator(current_group, reinterpret_cast<aligned_pointer_type>(element_pointer), skipfield_pointer) : end_iterator; // If element has been erased, return end()
			}

			current_group = current_group->previous_group;
		}

		return end_iterator;
	}



	template <bool is_const>
	size_type get_index_from_iterator(const colony_iterator<is_const> &it) const // may throw exception if iterator is invalid/uninitialized
	{
		assert(!empty());
		assert(it.group_pointer != NULL);

		// This is essentially a simplified version of distance() optimized for counting from begin()
		size_type index = 0;
		group_pointer_type group_pointer = first_group;

		// For all prior groups, add group sizes
		while (group_pointer != it.group_pointer)
		{
			index += static_cast<size_type>(group_pointer->number_of_elements);
			group_pointer = group_pointer->next_group;
		}

		if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
		{
			index += static_cast<size_type>(it.element_pointer - group_pointer->elements); // If no erased elements in group exist, do straight pointer arithmetic to get distance to start for first element
		}
		else // Otherwise do manual ++ loop - count from beginning of group until location is reached
		{
			skipfield_pointer_type skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);

			while(skipfield_pointer != it.skipfield_pointer)
			{
				++skipfield_pointer;
				skipfield_pointer += *skipfield_pointer;
				++index;
			}
		}

		return index;
	}



	template <bool is_const>
	inline size_type get_index_from_reverse_iterator(const colony_reverse_iterator<is_const> &rev_iterator) const
	{
		return get_index_from_iterator(rev_iterator.it);
	}



	template <typename index_type>
	iterator get_iterator_from_index(const index_type index) const // Cannot be noexcept as colony could be empty
	{
		assert(!empty());
		assert(std::numeric_limits<index_type>::is_integer);

		iterator it(begin_iterator);
		advance(it, static_cast<difference_type>(index));
		return it;
	}



    inline allocator_type get_allocator() const PLF_COLONY_NOEXCEPT
    {
		return element_allocator_type();
	}




private:

	struct less
	{
		bool operator() (const element_type &a, const element_type &b) const PLF_COLONY_NOEXCEPT
		{
			return a < b;
		}
	};


	// Function-object, used to redirect the sort function to compare element pointers by the elements they point to, and sort the element pointers instead of the elements:
	template <class comparison_function>
	struct sort_dereferencer
	{
		comparison_function stored_instance;

		explicit sort_dereferencer(const comparison_function &function_instance):
			stored_instance(function_instance)
		{}

		sort_dereferencer() PLF_COLONY_NOEXCEPT
		{}

		bool operator() (const pointer first, const pointer second)
		{
			return stored_instance(*first, *second);
		}
	};


public:


	template <class comparison_function>
	void sort(comparison_function compare)
	{
		if (total_number_of_elements < 2)
		{
			return;
		}

		pointer * const element_pointers = PLF_COLONY_ALLOCATE(pointer_allocator_type, pointer_allocator_pair, total_number_of_elements, NULL);
		pointer *element_pointer = element_pointers;

		// Construct pointers to all elements in the colony in sequence:
		for (iterator current_element = begin_iterator; current_element != end_iterator; ++current_element)
		{
			PLF_COLONY_CONSTRUCT(pointer_allocator_type, pointer_allocator_pair, element_pointer++, &*current_element);
		}

		// Now, sort the pointers by the values they point to:
		#ifdef PLF_TIMSORT_AVAILABLE
			plf::timsort(element_pointers, element_pointers + total_number_of_elements, sort_dereferencer<comparison_function>(compare));
		#else
			std::sort(element_pointers, element_pointers + total_number_of_elements, sort_dereferencer<comparison_function>(compare));
		#endif


		// Create a new colony and copy the elements from the old one to the new one in the order of the sorted pointers:
		colony new_location;
		new_location.change_group_sizes(pointer_allocator_pair.min_elements_per_group, group_allocator_pair.max_elements_per_group);


		try
		{
			new_location.reserve(static_cast<skipfield_type>((total_number_of_elements > std::numeric_limits<skipfield_type>::max()) ? std::numeric_limits<skipfield_type>::max() : total_number_of_elements));

			#if defined(PLF_COLONY_TYPE_TRAITS_SUPPORT) && defined(PLF_COLONY_MOVE_SEMANTICS_SUPPORT)
				if (std::is_move_constructible<element_type>::value)
				{
					for (pointer *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
					{
						new_location.insert(std::move(*reinterpret_cast<pointer>(*current_element_pointer)));
					}
				}
				else
			#endif
			{
				for (pointer *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
				{
					new_location.insert(*reinterpret_cast<pointer>(*current_element_pointer));
				}
			}
		}
		catch (...)
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (!std::is_trivially_destructible<pointer>::value)
			#endif
			{
				for (pointer *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
				{
					PLF_COLONY_DESTROY(pointer_allocator_type, pointer_allocator_pair, current_element_pointer);
				}
			}

			PLF_COLONY_DEALLOCATE(pointer_allocator_type, pointer_allocator_pair, element_pointers, total_number_of_elements);
			throw;
		}


		// Make the old colony the new one, destroy the old one's data/sequence:
		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(new_location); // avoid generating temporary
		#else
			swap(new_location);
		#endif


		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (!std::is_trivially_destructible<pointer>::value)
		#endif
		{
			for (pointer *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
			{
				PLF_COLONY_DESTROY(pointer_allocator_type, pointer_allocator_pair, current_element_pointer);
			}
		}

		PLF_COLONY_DEALLOCATE(pointer_allocator_type, pointer_allocator_pair, element_pointers, total_number_of_elements);
	}




	inline void sort()
	{
		sort(less());
	}



	void splice(colony &source) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		// Process: if there are unused memory spaces at the end of the current back group of the chain, convert them
		// to skipped elements and add the locations to the group's free list.
		// Then link the destination's groups to the source's groups and nullify the source.
		// If the source has more unused memory spaces in the back group than the destination, swap them before processing to reduce the number of locations added to a free list and also subsequent jumps during iteration.

		assert(&source != this);

		if (source.total_number_of_elements == 0)
		{
			return;
		}
		else if (total_number_of_elements == 0)
		{
			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(source);
			#else
				clear();
				swap(source);
			#endif

			return;
		}

		// If there's more unused element locations at end of destination than source, swap with source to reduce number of skipped elements and size of free-list:
		if ((reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) > (reinterpret_cast<aligned_pointer_type>(source.end_iterator.group_pointer->skipfield) - source.end_iterator.element_pointer))
		{
			swap(source);
		}


		// Correct group sizes if necessary:
		if (source.pointer_allocator_pair.min_elements_per_group < pointer_allocator_pair.min_elements_per_group)
		{
			pointer_allocator_pair.min_elements_per_group = source.pointer_allocator_pair.min_elements_per_group;
		}

		if (source.group_allocator_pair.max_elements_per_group > group_allocator_pair.max_elements_per_group)
		{
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
		}

		// Add source list of groups-with-erasures to destination list of groups-with-erasures:
		if (source.groups_with_erasures_list_head != NULL)
		{
			if (groups_with_erasures_list_head != NULL)
			{
				group_pointer_type tail_group = groups_with_erasures_list_head;

				while (tail_group->erasures_list_next_group != NULL)
				{
					tail_group = tail_group->erasures_list_next_group;
				}

				tail_group->erasures_list_next_group = source.groups_with_erasures_list_head;
			}
			else
			{
				groups_with_erasures_list_head = source.groups_with_erasures_list_head;
			}
		}


		const skipfield_type distance_to_end = static_cast<skipfield_type>(reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer);

		if (distance_to_end != 0) // 0 == edge case
		{   // Mark unused element memory locations from back group as skipped/erased:

			// Skipfield:
			skipfield_pointer_type current_skipfield_node = end_iterator.skipfield_pointer;
			const skipfield_type previous_node_value = *(current_skipfield_node - 1);

			if (previous_node_value == 0)
			{
				*current_skipfield_node = distance_to_end;
			}
			else
			{
				*current_skipfield_node = previous_node_value + 1;
				*(current_skipfield_node - previous_node_value) += distance_to_end;
			}

			skipfield_type current_skipfield_value = previous_node_value + 1;
			const skipfield_pointer_type end_node = current_skipfield_node + distance_to_end;
			while(++current_skipfield_node != end_node)
			{
				*current_skipfield_node = ++current_skipfield_value;
			}


			// Erased-element free-list:

			aligned_pointer_type current_element = end_iterator.element_pointer;
			const aligned_pointer_type last_element = end_iterator.element_pointer + distance_to_end - 1;
			skipfield_type next_free_list_index = static_cast<skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);
			const skipfield_type previous_free_list_head = end_iterator.group_pointer->free_list_head;

			if (end_iterator.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if this group does not currently have any erased elements, add it to the global groups-with-erasures list
			{
				end_iterator.group_pointer->erasures_list_next_group = groups_with_erasures_list_head;
				groups_with_erasures_list_head = end_iterator.group_pointer;
			}

			end_iterator.group_pointer->free_list_head = next_free_list_index;

			do
			{
				*(reinterpret_cast<skipfield_pointer_type>(current_element)) = ++next_free_list_index;
			} while(++current_element != last_element);

			*(reinterpret_cast<skipfield_pointer_type>(current_element)) = previous_free_list_head;
			end_iterator.group_pointer->last_endpoint = current_element + 1;
		}


		// Update subsequent group numbers:
		group_pointer_type current_group = source.first_group;
		size_type current_group_number = end_iterator.group_pointer->group_number;

		do
		{
			current_group->group_number = ++current_group_number;
			current_group = current_group->next_group;
		} while (current_group != NULL);


		// Join the destination and source group chains:
		end_iterator.group_pointer->next_group = source.first_group;
		source.first_group->previous_group = end_iterator.group_pointer;
		end_iterator = source.end_iterator;
		total_number_of_elements += source.total_number_of_elements;
		source.blank();
	}



	void swap(colony &source) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		assert(&source != this);

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (std::is_trivial<group_pointer_type>::value && std::is_trivial<aligned_pointer_type>::value && std::is_trivial<skipfield_pointer_type>::value) // if all pointer types are trivial we can just copy using memcpy - much faster
			{
				char temp[sizeof(colony)];
				std::memcpy(reinterpret_cast<void *>(&temp), reinterpret_cast<void *>(this), sizeof(colony));
				std::memcpy(reinterpret_cast<void *>(this), reinterpret_cast<void *>(&source), sizeof(colony));
				std::memcpy(reinterpret_cast<void *>(&source), reinterpret_cast<void *>(&temp), sizeof(colony));
			}
			else
		#endif
		{
			// The below is faster than doing a move-swap, by a significant margin:
			const iterator						swap_end_iterator = end_iterator, swap_begin_iterator = begin_iterator;
			const group_pointer_type		swap_first_group = first_group, swap_groups_with_erasures_list_head = groups_with_erasures_list_head;
			const size_type					swap_total_number_of_elements = total_number_of_elements, swap_total_capacity = total_capacity;
			const skipfield_type 			swap_min_elements_per_group = pointer_allocator_pair.min_elements_per_group, swap_max_elements_per_group = group_allocator_pair.max_elements_per_group;

			end_iterator = source.end_iterator;
			begin_iterator = source.begin_iterator;
			first_group = source.first_group;
			groups_with_erasures_list_head = source.groups_with_erasures_list_head;
			total_number_of_elements = source.total_number_of_elements;
			total_capacity = source.total_capacity;
			pointer_allocator_pair.min_elements_per_group = source.pointer_allocator_pair.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;

			source.end_iterator = swap_end_iterator;
			source.begin_iterator = swap_begin_iterator;
			source.first_group = swap_first_group;
			source.groups_with_erasures_list_head = swap_groups_with_erasures_list_head;
			source.total_number_of_elements = swap_total_number_of_elements;
			source.total_capacity = swap_total_capacity;
			source.pointer_allocator_pair.min_elements_per_group = swap_min_elements_per_group;
			source.group_allocator_pair.max_elements_per_group = swap_max_elements_per_group;
		}
	}

};	// colony




template <class element_type, class element_allocator_type, typename element_skipfield_type>
inline void swap (colony<element_type, element_allocator_type, element_skipfield_type> &a, colony<element_type, element_allocator_type, element_skipfield_type> &b) PLF_COLONY_NOEXCEPT_SWAP(element_allocator_type)
{
	a.swap(b);
}



} // plf namespace




#undef PLF_COLONY_FORCE_INLINE

#undef PLF_COLONY_ALIGNMENT_SUPPORT
#undef PLF_COLONY_INITIALIZER_LIST_SUPPORT
#undef PLF_COLONY_TYPE_TRAITS_SUPPORT
#undef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_COLONY_VARIADICS_SUPPORT
#undef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
#undef PLF_COLONY_NOEXCEPT
#undef PLF_COLONY_NOEXCEPT_SWAP
#undef PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT

#undef PLF_COLONY_CONSTRUCT
#undef PLF_COLONY_DESTROY
#undef PLF_COLONY_ALLOCATE
#undef PLF_COLONY_ALLOCATE_INITIALIZATION
#undef PLF_COLONY_DEALLOCATE


#endif // PLF_COLONY_H
