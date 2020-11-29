// Copyright (c) 2020, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

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
// 	claim that you wrote the original software. If you use this software
// 	in a product, an acknowledgement in the product documentation would be
// 	appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// 	misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_COLONY_H
#define PLF_COLONY_H


// Compiler-specific defines used by colony:

#if defined(_MSC_VER)
	#define PLF_COLONY_FORCE_INLINE __forceinline

	#if _MSC_VER >= 1900
		#define PLF_COLONY_ALIGNMENT_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value || std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<the_allocator>::is_always_equal::value)
	#else
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator)
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#endif

	#if _MSC_VER >= 1600
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_STATIC_ASSERT(check, message) static_assert(check, message)
	#else
		#define PLF_COLONY_STATIC_ASSERT(check, message) assert(check)
	#endif

	#if _MSC_VER >= 1700
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
	#endif
	#if _MSC_VER >= 1800
		#define PLF_COLONY_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
		#define PLF_COLONY_CONSTEXPR constexpr
	#else
		#define PLF_COLONY_CONSTEXPR
	#endif
	#if defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)
		#define PLF_COLONY_CPP20_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#define PLF_COLONY_FORCE_INLINE // note: GCC creates faster code without forcing inline
	#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_COLONY_VARIADICS_SUPPORT
			#define PLF_COLONY_STATIC_ASSERT(check, message) static_assert(check, message)
		#else
			#define PLF_COLONY_STATIC_ASSERT(check, message) assert(check)
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || __GNUC__ < 4
			#define PLF_COLONY_NOEXCEPT throw()
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator)
		#elif __GNUC__ < 6
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept
		#else // C++17 support
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value || std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
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
			#define PLF_COLONY_STATIC_ASSERT(check, message) static_assert(check, message)
		#else
			#define PLF_COLONY_STATIC_ASSERT(check, message) assert(check)
		#endif
		#if __GLIBCXX__ >= 20090421 	// libstdc++ 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value || std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#elif __GLIBCXX__ >= 20120322
			#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT noexcept
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept
		#else
			#define PLF_COLONY_NOEXCEPT throw()
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
			#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator)
		#endif
		#if __GLIBCXX__ >= 20130322
			#define PLF_COLONY_ALIGNMENT_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#endif
	#elif (defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) || defined(_LIBCPP_HAS_NO_VARIADICS)) // Special case for checking C++11 support with libCPP
		#define PLF_COLONY_STATIC_ASSERT(check, message) assert(check)
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
		#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator)
	#else // Assume type traits and initializer support for other compilers and standard libraries
		#define PLF_COLONY_STATIC_ASSERT(check, message) static_assert(check, message)
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_COLONY_ALIGNMENT_SUPPORT
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#define PLF_COLONY_NOEXCEPT noexcept
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept
		#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator) noexcept
	#endif

	#if __cplusplus >= 201703L && ((defined(__clang__) && ((__clang_major__ == 3 && __clang_minor__ == 9) || __clang_major__ > 3))	 ||	(defined(__GNUC__) && __GNUC__ >= 7)	||   (!defined(__clang__) && !defined(__GNUC__))) // assume correct C++17 implementation for non-GNU/cland compilers
		#define PLF_COLONY_CONSTEXPR constexpr
	#else
		#define PLF_COLONY_CONSTEXPR
	#endif
	#if __cplusplus > 201703L && ((defined(__clang__) && (__clang_major__ >= 10)) || (defined(__GNUC__) && __GNUC__ >= 10) || (!defined(__clang__) && !defined(__GNUC__))) // assume correct C++20 implementation for other compilers
		#define PLF_COLONY_CPP20_SUPPORT
	#endif
#else
	#define PLF_COLONY_STATIC_ASSERT(check, message) assert(check)
	#define PLF_COLONY_FORCE_INLINE
	#define PLF_COLONY_NOEXCEPT throw()
	#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator)
	#define PLF_COLONY_NOEXCEPT_SPLICE(the_allocator)
	#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
	#define PLF_COLONY_CONSTEXPR
#endif





#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, ...)	std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_COLONY_CONSTRUCT(the_allocator, allocator_instance, location, data) std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_COLONY_DESTROY(the_allocator, allocator_instance, location) 				std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
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



#include <algorithm> // std::fill_n, std::sort
#include <cassert>	// assert
#include <cstring>	// memset, memcpy
#include <limits>  // std::numeric_limits
#include <memory>	// std::allocator
#include <iterator> // std::bidirectional_iterator_tag, iterator_traits, make_move_iterator
#include <stdexcept> // std::length_error


#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
	#include <cstddef> // offsetof, used in blank()
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


struct limits // for use in block_capacity setting/getting functions and constructors
{
	size_t min, max;
	limits(const size_t minimum, const size_t maximum) PLF_COLONY_NOEXCEPT : min(minimum), max(maximum) {}
};



template <class element_type, class allocator_type = std::allocator<element_type>, typename skipfield_type = unsigned short > class colony : private allocator_type	// Empty base class optimisation (EBCO) - inheriting allocator functions
// Note: unsigned short is equivalent to uint_least16_t ie. Using 16-bit unsigned integer in best-case scenario, greater-than-16-bit unsigned integer where platform doesn't support 16-bit types
{
public:
	// Standard container typedefs:
	typedef element_type																							value_type;

	#ifdef PLF_COLONY_ALIGNMENT_SUPPORT
		typedef typename std::aligned_storage<sizeof(element_type), (alignof(element_type) > (sizeof(skipfield_type) * 2)) ? alignof(element_type) : (sizeof(skipfield_type) * 2)>::type	aligned_element_type;
	#else
		typedef element_type																						aligned_element_type;
	#endif

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<allocator_type>::size_type							size_type;
		typedef typename std::allocator_traits<allocator_type>::difference_type 				difference_type;
		typedef element_type &																					reference;
		typedef const element_type &																			const_reference;
		typedef typename std::allocator_traits<allocator_type>::pointer 							pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer					const_pointer;
	#else
		typedef typename allocator_type::size_type			size_type;
		typedef typename allocator_type::difference_type	difference_type;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
	#endif


	// Iterator declarations:
	template <bool is_const> class			colony_iterator;
	typedef colony_iterator<false>			iterator;
	typedef colony_iterator<true>				const_iterator;
	friend class colony_iterator<false>; // Using above typedef name here is illegal under C++03
	friend class colony_iterator<true>;

	template <bool r_is_const> class			colony_reverse_iterator;
	typedef colony_reverse_iterator<false>	reverse_iterator;
	typedef colony_reverse_iterator<true>	const_reverse_iterator;
	friend class colony_reverse_iterator<false>;
	friend class colony_reverse_iterator<true>;



private:


	struct group; // forward declaration for typedefs below

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<aligned_element_type>	aligned_allocator_type;
		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<group>						group_allocator_type;
		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<skipfield_type>			skipfield_allocator_type;
		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<unsigned char>				uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.

		typedef typename std::allocator_traits<aligned_allocator_type>::pointer				aligned_pointer_type; // pointer to the overaligned element type, not the original element type
		typedef typename std::allocator_traits<group_allocator_type>::pointer 				group_pointer_type;
		typedef typename std::allocator_traits<skipfield_allocator_type>::pointer 			skipfield_pointer_type;
		typedef typename std::allocator_traits<uchar_allocator_type>::pointer				uchar_pointer_type;

		typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<pointer>	pointer_allocator_type;
	#else
		typedef typename allocator_type::template rebind<aligned_element_type>::other	aligned_allocator_type;	// In case compiler supports alignment but not allocator_traits
		typedef typename allocator_type::template rebind<group>::other						group_allocator_type;
		typedef typename allocator_type::template rebind<skipfield_type>::other			skipfield_allocator_type;
		typedef typename allocator_type::template rebind<unsigned char>::other			uchar_allocator_type;

		typedef typename aligned_allocator_type::pointer			aligned_pointer_type;
		typedef typename group_allocator_type::pointer 				group_pointer_type;
		typedef typename skipfield_allocator_type::pointer 		skipfield_pointer_type;
		typedef typename uchar_allocator_type::pointer				uchar_pointer_type;

		typedef typename allocator_type::template rebind<pointer>::other pointer_allocator_type;
	#endif



	// Colony groups:
	struct group : private uchar_allocator_type	// ebco - inherit allocator functions
	{
		aligned_pointer_type					last_endpoint; 			// The address that is one past the highest cell number that's been used so far in this group - does not change with erase command but may change with insert (if no previously-erased locations are available) - is necessary because an iterator cannot access the colony's end_iterator. Most-used variable in colony use (operator ++, --) so first in struct. If the colony has been completely filled at some point, it will be == reinterpret_cast<aligned_pointer_type>(skipfield)
		group_pointer_type					next_group; 				// Next group in the intrusive list of all groups. NULL if no next group
		const aligned_pointer_type			elements; 					// Element storage
		const skipfield_pointer_type		skipfield; 					// Skipfield storage. The element and skipfield arrays are allocated contiguously in this implementation, hence the skipfield pointer also functions as a 'one-past-end' pointer for the elements array. There will always be one additional skipfield node allocated compared to the number of elements. This is to ensure a faster ++ iterator operation (fewer checks are required when this is present). The extra node is unused and always zero, but checked, and not having it will result in out-of-bounds memory errors.
		group_pointer_type					previous_group; 			// previous group in the linked list of all groups. NULL if no preceding group
		skipfield_type							free_list_head; 			// The index of the last erased element in the group. The last erased element will, in turn, contain the number of the index of the next erased element, and so on. If this is == maximum skipfield_type value then free_list is empty ie. no erasures have occurred in the group (or if they have, the erased locations have subsequently been reused via insert()).
		const skipfield_type					capacity; 					// The element capacity of this particular group - can also be calculated from reinterpret_cast<aligned_pointer_type>(group->skipfield) - group->elements, however this space is effectively free due to struct padding and the default sizeof skipfield_type, and calculating it once is cheaper
		skipfield_type							size; 						// indicates total number of active elements in group - changes with insert and erase commands - used to check for empty group in erase function, as an indication to remove the group
		group_pointer_type					erasures_list_next_group; // The next group in the singly-linked list of groups with erasures ie. with active erased-element free lists
		size_type								group_number; 				// Used for comparison (> < >= <= <=>) iterator operators (used by distance function and user) and memory() function


		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<aligned_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(aligned_element_type))) + ((static_cast<size_type>(elements_per_group) + 1u) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is only initialised to element's base value initially, then incremented by one below */
				next_group(NULL),
				elements(last_endpoint++),
				skipfield(reinterpret_cast<skipfield_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				free_list_head(std::numeric_limits<skipfield_type>::max()),
				capacity(elements_per_group),
				size(1),
				erasures_list_next_group(NULL),
				group_number((previous == NULL) ? 0 : previous->group_number + 1u)
			{
				// Static casts to unsigned int from short not necessary as C++ automatically promotes lesser types for arithmetic purposes.
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (static_cast<size_type>(elements_per_group) + 1u)); // &* to avoid problems with non-trivial pointers
			}

		#else
			// This is a hack around the fact that allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and losing performance) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<aligned_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(aligned_element_type))) + (static_cast<size_type>(elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))),
				elements(NULL),
				skipfield(reinterpret_cast<skipfield_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				capacity(elements_per_group)
			{
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (static_cast<size_type>(elements_per_group) + 1u));
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
				capacity(source.capacity),
				size(1),
				erasures_list_next_group(NULL),
				group_number((source.previous_group == NULL) ? 0 : source.previous_group->group_number + 1u)
			{}
		#endif



		void reset(const skipfield_type increment, const group_pointer_type next, const group_pointer_type previous, const size_type group_num) PLF_COLONY_NOEXCEPT
		{
			last_endpoint = elements + increment;
			next_group = next;
			free_list_head = std::numeric_limits<skipfield_type>::max();
			previous_group = previous;
			size = increment;
			erasures_list_next_group = NULL;
			group_number = group_num;

			std::memset(&*skipfield, 0, sizeof(skipfield_type) * static_cast<size_type>(capacity)); // capacity + 1 is not necessary here as the end skipfield is never written to after initialization
		}



		~group() PLF_COLONY_NOEXCEPT
		{
			// Null check not necessary (for copied group as above) as delete will also perform a null check.
			PLF_COLONY_DEALLOCATE(uchar_allocator_type, (*this), reinterpret_cast<uchar_pointer_type>(elements), (capacity * sizeof(aligned_element_type)) + ((static_cast<size_type>(capacity) + 1u) * sizeof(skipfield_type)));
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
		aligned_pointer_type		element_pointer;
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
			// Move assignment - only really necessary if the allocator uses non-standard ie. "smart" pointers
			inline colony_iterator & operator = (colony_iterator &&source) PLF_COLONY_NOEXCEPT // Move is a copy in this scenario
			{
				assert(&source != this);
				group_pointer = std::move(source.group_pointer);
				element_pointer = std::move(source.element_pointer);
				skipfield_pointer = std::move(source.skipfield_pointer);
				return *this;
			}



			inline colony_iterator & operator = (colony_iterator<!is_const> &&source) PLF_COLONY_NOEXCEPT
			{
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
			skipfield_type skip = *(++skipfield_pointer);

			if ((element_pointer += static_cast<size_type>(skip) + 1u) == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of current memory block. Second condition allows iterator to reach end(), which may be 1 past end of block, if block has been fully used and another block is not allocated
			{
				group_pointer = group_pointer->next_group;
				const aligned_pointer_type elements = group_pointer->elements;
				const skipfield_pointer_type skipfield = group_pointer->skipfield;
				skip = *skipfield;
				element_pointer = elements + skip;
				skipfield_pointer = skipfield;
			}

			skipfield_pointer += skip;
			return *this;
		}



		inline colony_iterator operator ++(int)
		{
			const colony_iterator copy(*this);
			++*this;
			return copy;
		}



	public:

		colony_iterator & operator -- ()
		{
			assert(group_pointer != NULL);

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				const skipfield_type skip = *(--skipfield_pointer);
				skipfield_pointer -= skip;

				if ((element_pointer -= static_cast<size_type>(skip) + 1u) != group_pointer->elements - 1) // ie. iterator was not already at beginning of colony (with some previous consecutive deleted elements), and skipfield does not takes us into the previous group)
				{
					return *this;
				}
			}

			group_pointer = group_pointer->previous_group;
			const skipfield_pointer_type skipfield = group_pointer->skipfield + group_pointer->capacity - 1;
			const skipfield_type skip = *skipfield;
			element_pointer = (reinterpret_cast<colony::aligned_pointer_type>(group_pointer->skipfield) - 1) - skip;
			skipfield_pointer = skipfield - skip;

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
			return ((group_pointer == rh.group_pointer) & (element_pointer > rh.element_pointer)) || (group_pointer != rh.group_pointer && group_pointer->group_number > rh.group_pointer->group_number);
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
			return ((group_pointer == rh.group_pointer) & (element_pointer > rh.element_pointer)) || (group_pointer != rh.group_pointer && group_pointer->group_number > rh.group_pointer->group_number);
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



		#ifdef PLF_COLONY_CPP20_SUPPORT
	 		inline int operator <=> (const colony_iterator &rh) const PLF_COLONY_NOEXCEPT
	 		{
	 			return (element_pointer == rh.element_pointer) ? 0 : ((*this > rh) ? 1 : -1);
	 		}


	 		inline int operator <=> (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
	 		{
	 			return (element_pointer == rh.element_pointer) ? 0 : ((*this > rh) ? 1 : -1);
	 		}
	 	#endif



		colony_iterator() PLF_COLONY_NOEXCEPT: group_pointer(NULL), element_pointer(NULL), skipfield_pointer(NULL)	{}



	private:
		// Used by cend(), erase() etc:
		colony_iterator(const group_pointer_type group_p, const aligned_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: group_pointer(group_p), element_pointer(element_p), skipfield_pointer(skipfield_p) {}


	public:

		// Friend functions:

		template <class distance_type>
		friend inline void advance(colony_iterator &it, distance_type distance)
		{
			it.advance(static_cast<difference_type>(distance));
		}



		friend inline colony_iterator next(const colony_iterator &it, const difference_type distance)
		{
			colony_iterator return_iterator(it);
			return_iterator.advance(static_cast<difference_type>(distance));
			return return_iterator;
		}



		friend inline colony_iterator prev(const colony_iterator &it, const difference_type distance)
		{
			colony_iterator return_iterator(it);
			return_iterator.advance(-(static_cast<difference_type>(distance)));
			return return_iterator;
		}



		friend inline typename colony_iterator::difference_type distance(const colony_iterator &first, const colony_iterator &last)
		{
			return first.distance(last);
		}



	private:

		// Advance implementation:

		void advance(difference_type distance) // Cannot be noexcept due to the possibility of an uninitialized iterator
		{
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

				assert(!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group == NULL)); // Check that we're not already at end()

				// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
				if (element_pointer != group_pointer->elements + *(group_pointer->skipfield)) // ie. != first non-erased element in group
				{
					const difference_type distance_from_end = static_cast<difference_type>(group_pointer->last_endpoint - element_pointer);

					if (group_pointer->size == static_cast<skipfield_type>(distance_from_end)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->capacity)
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
				while (static_cast<difference_type>(group_pointer->size) <= distance)
				{
					if (group_pointer->next_group == NULL) // either we've reached end() or gone beyond it, so bound to end()
					{
						element_pointer = group_pointer->last_endpoint;
						skipfield_pointer = group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements);
						return;
					}
					else if ((distance -= group_pointer->size) == 0)
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
				else	 // ie. size > distance - safe to ignore endpoint check condition while incrementing:
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
						const difference_type distance_from_beginning = static_cast<difference_type>(element_pointer - group_pointer->elements);

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
				while(static_cast<difference_type>(group_pointer->size) < distance)
				{
					if (group_pointer->previous_group == NULL) // we've gone beyond begin(), so bound to it
					{
						element_pointer = group_pointer->elements + *(group_pointer->skipfield);
						skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
						return;
					}

					distance -= group_pointer->size;
					group_pointer = group_pointer->previous_group;
				}


				// Final group (if not already reached):
				if (static_cast<difference_type>(group_pointer->size) == distance)
				{
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
				else if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. no erased elements in this group
				{
					element_pointer = reinterpret_cast<aligned_pointer_type>(group_pointer->skipfield) - distance;
					skipfield_pointer = (group_pointer->skipfield + group_pointer->capacity) - distance;
					return;
				}
				else // ie. no more groups to traverse but there are erased elements in this group
				{
					skipfield_pointer = group_pointer->skipfield + group_pointer->capacity;

					do
					{
						--skipfield_pointer;
						skipfield_pointer -= *skipfield_pointer;
					} while(--distance != 0);

					element_pointer = group_pointer->elements + (skipfield_pointer - group_pointer->skipfield);
					return;
				}
			}

			// Only distance == 0 reaches here
		}



		// distance implementation:

		difference_type distance(const colony_iterator &last) const
		{
			// Code logic:
			// If iterators are the same, return 0
			// Otherwise, find which iterator is later in colony, copy that to iterator2. Copy the lower to iterator1.
			// If they are not pointing to elements in the same group, process the intermediate groups and add distances,
			// skipping manual incrementation in all but the initial and final groups.
			// In the initial and final groups, manual incrementation must be used to calculate distance, if there have been no prior erasures in those groups.
			// If there are no prior erasures in either of those groups, we can use pointer arithmetic to calculate the distances for those groups.

			assert(!(group_pointer == NULL) && !(last.group_pointer == NULL));  // Check that they are initialized

			if (last.element_pointer == element_pointer)
			{
				return 0;
			}

			difference_type distance = 0;
			colony_iterator iterator1 = *this, iterator2 = last;
			const bool swap = iterator1 > iterator2;

			if (swap) // Less common case
			{
				iterator1 = last;
				iterator2 = *this;
			}

			if (iterator1.group_pointer != iterator2.group_pointer) // if not in same group, process intermediate groups
			{
				// Process initial group:
				if (iterator1.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // If no prior erasures have occured in this group we can do simple addition
				{
					distance += static_cast<difference_type>(iterator1.group_pointer->last_endpoint - iterator1.element_pointer);
				}
				else if (iterator1.element_pointer == iterator1.group_pointer->elements + *(iterator1.group_pointer->skipfield)) // ie. element is at start of group - rare case
				{
					distance += static_cast<difference_type>(iterator1.group_pointer->size);
				}
				else // Manually iterate to find distance to end of group:
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
					distance += static_cast<difference_type>(iterator1.group_pointer->size);
					iterator1.group_pointer = iterator1.group_pointer->next_group;
				}

				iterator1.skipfield_pointer = iterator1.group_pointer->skipfield;
			}


			if (iterator2.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. no erasures in this group, direct subtraction is possible
			{
				distance += static_cast<difference_type>(iterator2.skipfield_pointer - iterator1.skipfield_pointer);
			}
			else if (iterator2.group_pointer->last_endpoint - 1 >= iterator2.element_pointer || iterator2.element_pointer + *(iterator2.skipfield_pointer + 1) == iterator2.group_pointer->last_endpoint) // ie. if iterator2 is .end() or the last element in the block
			{
				distance += static_cast<difference_type>(iterator2.group_pointer->size - (iterator2.group_pointer->last_endpoint - iterator2.element_pointer));
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
			{
				assert(&source != this);
			}


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
		typedef std::bidirectional_iterator_tag 	iterator_category;
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



		inline colony_reverse_iterator& operator = (const colony_reverse_iterator<!r_is_const> &source) PLF_COLONY_NOEXCEPT
		{
			it = source.it;
			return *this;
		}



		template<bool is_const>
		inline colony_reverse_iterator& operator = (const colony_iterator<is_const> &source) PLF_COLONY_NOEXCEPT
		{
			it = source;
			return *this;
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move assignment
			inline colony_reverse_iterator& operator = (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT
			{
				assert(&source != this);
				it = std::move(source.it);
				return *this;
			}


			inline colony_reverse_iterator& operator = (colony_reverse_iterator<!r_is_const> &&source) PLF_COLONY_NOEXCEPT
			{
				it = std::move(source.it);
				return *this;
			}
		#endif



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it == rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it == rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
		{
			return (it != rh.it);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
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

			if (element_pointer != group_pointer->elements) // ie. not already at beginning of group
			{
				element_pointer -= static_cast<size_type>(*(--skipfield_pointer)) + 1u;
				skipfield_pointer -= *skipfield_pointer;

				if (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)) // ie. iterator is not == rend()
				{
					return *this;
				}
			}

			if (group_pointer->previous_group != NULL) // ie. not first group in colony
			{
				group_pointer = group_pointer->previous_group;
				skipfield_pointer = group_pointer->skipfield + group_pointer->capacity - 1;
				element_pointer = (reinterpret_cast<colony::aligned_pointer_type>(group_pointer->skipfield) - 1) - *skipfield_pointer;
				skipfield_pointer -= *skipfield_pointer;
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



		// C++20:
		#ifdef PLF_COLONY_CPP20_SUPPORT
	 		inline int operator <=> (const colony_reverse_iterator &rh) const PLF_COLONY_NOEXCEPT
	 		{
	 			return (rh.it <=> it);
	 		}


	 		inline int operator <=> (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
	 		{
	 			return (rh.it <=> it);
	 		}
	 	#endif



		colony_reverse_iterator () PLF_COLONY_NOEXCEPT
		{}



		colony_reverse_iterator (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT:
			it(source.it)
		{}



		colony_reverse_iterator (const colony_reverse_iterator<!r_is_const> &source) PLF_COLONY_NOEXCEPT:
			it(source.it)
		{}



		template<bool is_const>
		colony_reverse_iterator (const colony_iterator<is_const> &source) PLF_COLONY_NOEXCEPT:
			it(source)
		{}




	private:
		// Used by rend(), etc:
		colony_reverse_iterator(const group_pointer_type group_p, const aligned_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: it(group_p, element_p, skipfield_p) {}



	public:
		// Friend functions:

		template <class distance_type>
		friend inline void advance(colony_reverse_iterator &it, distance_type distance)
		{
			it.advance(static_cast<difference_type>(distance));
		}



		friend inline colony_reverse_iterator next(const colony_reverse_iterator &it, const difference_type distance)
		{
			colony_reverse_iterator return_iterator(it);
			return_iterator.advance(static_cast<difference_type>(distance));
			return return_iterator;
		}



		template <class distance_type>
		friend inline colony_reverse_iterator prev(const colony_reverse_iterator &it, const difference_type distance)
		{
			colony_reverse_iterator return_iterator(it);
			return_iterator.advance(-(static_cast<difference_type>(distance)));
			return return_iterator;
		}



		friend inline typename colony_reverse_iterator::difference_type distance(const colony_reverse_iterator &first, const colony_reverse_iterator &last)
		{
			return first.distance(last);
		}



		// distance implementation:

		inline difference_type distance(const colony_reverse_iterator &last) const
		{
			return last.it.distance(it);
		}



	private:

		// Advance for reverse_iterator and const_reverse_iterator - this needs to be implemented slightly differently to forward-iterator's advance, as it needs to be able to reach rend() (ie. begin() - 1) and to be bounded by rbegin():
		void advance(difference_type distance) // could cause exception if iterator is uninitialized
		{
			group_pointer_type &group_pointer = it.group_pointer;
			aligned_pointer_type &element_pointer = it.element_pointer;
			skipfield_pointer_type &skipfield_pointer = it.skipfield_pointer;

			assert(element_pointer != NULL);

			if (distance > 0)
			{
				assert(!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Check that we're not already at rend()
				// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
				// Since a reverse_iterator cannot == last_endpoint (ie. before rbegin()) we don't need to check for that like with iterator
				if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
				{
					const difference_type distance_from_beginning = static_cast<difference_type>(element_pointer - group_pointer->elements);

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
				while(static_cast<difference_type>(group_pointer->size) < distance)
				{
					if (group_pointer->previous_group == NULL) // bound to rend()
					{
						element_pointer = group_pointer->elements - 1;
						skipfield_pointer = group_pointer->skipfield - 1;
						return;
					}

					distance -= static_cast<difference_type>(group_pointer->size);
					group_pointer = group_pointer->previous_group;
				}


				// Final group (if not already reached)
				if (static_cast<difference_type>(group_pointer->size) == distance)
				{
					element_pointer = group_pointer->elements + *(group_pointer->skipfield);
					skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);
					return;
				}
				else if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
				{
					element_pointer = reinterpret_cast<aligned_pointer_type>(group_pointer->skipfield) - distance;
					skipfield_pointer = (group_pointer->skipfield + group_pointer->capacity) - distance;
					return;
				}
				else
				{
					skipfield_pointer = group_pointer->skipfield + group_pointer->capacity;

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
				assert(!((element_pointer == (group_pointer->last_endpoint - 1) - *(group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements) - 1)) && group_pointer->next_group == NULL)); // Check that we're not already at rbegin()

				if (element_pointer != group_pointer->elements + *(group_pointer->skipfield)) // ie. != first non-erased element in group
				{
					if (group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max()) // ie. if there are no erasures in the group
					{
						const difference_type distance_from_end = static_cast<difference_type>(group_pointer->last_endpoint - element_pointer);

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
							--skipfield_pointer;
							element_pointer = (group_pointer->last_endpoint - 1) - *skipfield_pointer;
							skipfield_pointer -= *skipfield_pointer;
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
				while(static_cast<difference_type>(group_pointer->size) <= distance)
				{
					if (group_pointer->next_group == NULL) // bound to rbegin()
					{
						skipfield_pointer = group_pointer->skipfield + (group_pointer->last_endpoint - group_pointer->elements) - 1;
						element_pointer = (group_pointer->last_endpoint - 1) - *skipfield_pointer;
						skipfield_pointer -= *skipfield_pointer;
						return;
					}
					else if ((distance -= group_pointer->size) == 0)
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
				else // ie. size > distance - safe to ignore endpoint check condition while incrementing:
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



	public:

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move constructors
			colony_reverse_iterator (colony_reverse_iterator &&source) PLF_COLONY_NOEXCEPT:
				it(std::move(source.it))
			{
				assert(&source != this);
			}

			colony_reverse_iterator (colony_reverse_iterator<!r_is_const> &&source) PLF_COLONY_NOEXCEPT:
				it(std::move(source.it))
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



	// Colony Member variables:

	iterator					end_iterator, begin_iterator;
	group_pointer_type	groups_with_erasures_list_head,	// Head of the singly-linked list of groups which have erased-element memory locations available for re-use
								unused_groups;							// Groups retained by erase() or created by reserve()
	size_type				total_size, total_capacity;

	struct ebco_pair2 : pointer_allocator_type // Packaging the element pointer allocator with a lesser-used member variable, for empty-base-class optimisation
	{
		skipfield_type min_group_capacity;
		explicit ebco_pair2(const skipfield_type min_elements) PLF_COLONY_NOEXCEPT: min_group_capacity(min_elements) {}
	}							pointer_allocator_pair;

	struct ebco_pair : group_allocator_type
	{
		skipfield_type max_group_capacity;
		explicit ebco_pair(const skipfield_type max_elements) PLF_COLONY_NOEXCEPT: max_group_capacity(max_elements) {}
	}							group_allocator_pair;



	// An adaptive minimum based around sizeof(element_type) and sizeof the group metadata:
	#define PLF_COLONY_MIN_BLOCK_CAPACITY (sizeof(aligned_element_type) * 8 > (sizeof(plf::colony<element_type>) + sizeof(group)) * 2) ? 8 : (((sizeof(plf::colony<element_type>) + sizeof(group)) * 2) / sizeof(aligned_element_type))



	inline void check_capacities_conformance(plf::limits capacities) const
	{
  		if (capacities.min < 2 || capacities.min > capacities.max || capacities.max > std::numeric_limits<skipfield_type>::max())
		{
			throw std::length_error("Supplied memory block capacities outside of allowable ranges");
		}
	}



	inline void check_skipfield_conformance() const PLF_COLONY_NOEXCEPT
	{
	 	PLF_COLONY_STATIC_ASSERT(std::numeric_limits<skipfield_type>::is_integer && !std::numeric_limits<skipfield_type>::is_signed, "skipfield type must be of unsigned integer type (uchar, ushort, uint etc)");

		#ifndef PLF_COLONY_ALIGNMENT_SUPPORT
			PLF_COLONY_STATIC_ASSERT(sizeof(element_type) >= sizeof(skipfield_type) * 2, "Element type is not large enough to accomodate colony requirements under C++98/03. Change to C++11 or above, or use a larger type."); // eg. under C++98/03, aligned_storage is not available, so sizeof(skipfield type) * 2 must be larger or equal to sizeof(element_type), otherwise the doubly-linked free lists of erased element indexes will not work correctly. So if you're storing chars, for example, and using the default skipfield type (unsigned short), the compiler will flag you with this assert. You cannot store char or unsigned char in colony under C++03, and if storing short or unsigned short you must change your skipfield type to unsigned char. Or just use C++11 and above.
		#endif
	}



public:

	// Default constuctor:

	colony() PLF_COLONY_NOEXCEPT:
		allocator_type(allocator_type()),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(PLF_COLONY_MIN_BLOCK_CAPACITY),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max())
	{
		check_skipfield_conformance();
	}



	explicit colony(const plf::limits capacities) PLF_COLONY_NOEXCEPT:
		allocator_type(allocator_type()),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
		group_allocator_pair(static_cast<skipfield_type>(capacities.max))
	{
		check_skipfield_conformance();
		check_capacities_conformance(capacities);
	}



	// Default constuctor (allocator-extended):

	explicit colony(const allocator_type &alloc):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(PLF_COLONY_MIN_BLOCK_CAPACITY),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max())
	{
		check_skipfield_conformance();
	}



	explicit colony(const plf::limits capacities, const allocator_type &alloc):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
		group_allocator_pair(static_cast<skipfield_type>(capacities.max))
	{
		check_skipfield_conformance();
		check_capacities_conformance(capacities);
	}



	// Copy constructor:

	colony(const colony &source):
		allocator_type(source),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>((source.pointer_allocator_pair.min_group_capacity > source.total_size) ? source.pointer_allocator_pair.min_group_capacity : ((source.total_size > source.group_allocator_pair.max_group_capacity) ? source.group_allocator_pair.max_group_capacity : source.total_size))), // min group size is set to value closest to total number of elements in source colony in order to not create unnecessary small groups in the range-insert below, then reverts to the original min group size afterwards. This effectively saves a call to reserve.
		group_allocator_pair(source.group_allocator_pair.max_group_capacity)
	{
		check_skipfield_conformance();
		range_assign(source.begin_iterator, source.total_size);
		pointer_allocator_pair.min_group_capacity = source.pointer_allocator_pair.min_group_capacity; // reset to correct value for future clear() or erasures
	}



	// Copy constructor (allocator-extended):

	colony(const colony &source, const allocator_type &alloc):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>((source.pointer_allocator_pair.min_group_capacity > source.total_size) ? source.pointer_allocator_pair.min_group_capacity : ((source.total_size > source.group_allocator_pair.max_group_capacity) ? source.group_allocator_pair.max_group_capacity : source.total_size))),
		group_allocator_pair(source.group_allocator_pair.max_group_capacity)
	{
		check_skipfield_conformance();
		range_assign(source.begin_iterator, source.total_size);
		pointer_allocator_pair.min_group_capacity = source.pointer_allocator_pair.min_group_capacity;
	}



private:

	inline void blank() PLF_COLONY_NOEXCEPT
	{
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<aligned_pointer_type>::value && std::is_trivial<skipfield_pointer_type>::value)	// if all pointer types are trivial, we can just nuke it from orbit with memset (NULL is always 0 in C++):
			{
				std::memset(static_cast<void *>(this), 0, offsetof(colony, pointer_allocator_pair));
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
			groups_with_erasures_list_head = NULL;
			unused_groups = NULL;
			total_size = 0;
			total_capacity = 0;
		}
	}

public:



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move constructor:

		colony(colony &&source) PLF_COLONY_NOEXCEPT:
			allocator_type(source),
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			groups_with_erasures_list_head(std::move(source.groups_with_erasures_list_head)),
			unused_groups(std::move(source.unused_groups)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			pointer_allocator_pair(source.pointer_allocator_pair.min_group_capacity),
			group_allocator_pair(source.group_allocator_pair.max_group_capacity)
		{
			check_skipfield_conformance();
			assert(&source != this);
			source.blank();
		}



		// Move constructor (allocator-extended):

		colony(colony &&source, const allocator_type &alloc):
			allocator_type(alloc),
			end_iterator(std::move(source.end_iterator)),
			begin_iterator(std::move(source.begin_iterator)),
			groups_with_erasures_list_head(std::move(source.groups_with_erasures_list_head)),
			unused_groups(std::move(source.unused_groups)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			pointer_allocator_pair(source.pointer_allocator_pair.min_group_capacity),
			group_allocator_pair(source.group_allocator_pair.max_group_capacity)
		{
			check_skipfield_conformance();
			assert(&source != this);
			source.blank();
		}
	#endif



	// Fill constructor:

	colony(const size_type fill_number, const element_type &element, const plf::limits capacities = plf::limits(PLF_COLONY_MIN_BLOCK_CAPACITY, std::numeric_limits<skipfield_type>::max()), const allocator_type &alloc = allocator_type()):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
		group_allocator_pair(static_cast<skipfield_type>(capacities.max))
	{
		check_skipfield_conformance();
		check_capacities_conformance(capacities);
		assign(fill_number, element);
	}



	// Default-value fill constructor:

	explicit colony(const size_type fill_number, const plf::limits capacities = plf::limits(PLF_COLONY_MIN_BLOCK_CAPACITY, std::numeric_limits<skipfield_type>::max()), const allocator_type &alloc = allocator_type()):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
		group_allocator_pair(static_cast<skipfield_type>(capacities.max))
	{
		check_skipfield_conformance();
		check_capacities_conformance(capacities);
		assign(fill_number, element_type());
	}



	// Range constructor:

	template<typename iterator_type>
	colony(const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type &first, const iterator_type &last, const plf::limits capacities = plf::limits(PLF_COLONY_MIN_BLOCK_CAPACITY, std::numeric_limits<skipfield_type>::max()), const allocator_type &alloc = allocator_type()):
		allocator_type(alloc),
		groups_with_erasures_list_head(NULL),
		unused_groups(NULL),
		total_size(0),
		total_capacity(0),
		pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
		group_allocator_pair(static_cast<skipfield_type>(capacities.max))
	{
		check_skipfield_conformance();
		check_capacities_conformance(capacities);
		assign<iterator_type>(first, last);
	}



	// Initializer-list constructor:

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		colony(const std::initializer_list<element_type> &element_list, const plf::limits capacities = plf::limits(PLF_COLONY_MIN_BLOCK_CAPACITY, std::numeric_limits<skipfield_type>::max()), const allocator_type &alloc = allocator_type()):
			allocator_type(alloc),
			groups_with_erasures_list_head(NULL),
			unused_groups(NULL),
			total_size(0),
			total_capacity(0),
			pointer_allocator_pair(static_cast<skipfield_type>(capacities.min)),
			group_allocator_pair(static_cast<skipfield_type>(capacities.max))
		{
			check_skipfield_conformance();
			check_capacities_conformance(capacities);
			range_assign(element_list.begin(), static_cast<size_type>(element_list.size()));
		}

	#endif



	inline PLF_COLONY_FORCE_INLINE iterator begin() PLF_COLONY_NOEXCEPT
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const_iterator begin() const PLF_COLONY_NOEXCEPT // To allow for functions which only take const colony & as a source eg. copy constructor
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE iterator end() PLF_COLONY_NOEXCEPT
	{
		return end_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const_iterator end() const PLF_COLONY_NOEXCEPT
	{
		return end_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const_iterator cbegin() const PLF_COLONY_NOEXCEPT
	{
		return begin_iterator;
	}



	inline PLF_COLONY_FORCE_INLINE const_iterator cend() const PLF_COLONY_NOEXCEPT
	{
		return end_iterator;
	}



	inline reverse_iterator rbegin() const PLF_COLONY_NOEXCEPT
	{
		return (end_iterator.group_pointer != NULL) ? ++reverse_iterator(end_iterator) : reverse_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer - 1, begin_iterator.skipfield_pointer - 1);
	}



	inline reverse_iterator rend() const PLF_COLONY_NOEXCEPT
	{
		return reverse_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer - 1, begin_iterator.skipfield_pointer - 1);
	}



	inline const_reverse_iterator crbegin() const PLF_COLONY_NOEXCEPT
	{
		return (end_iterator.group_pointer != NULL) ? ++const_reverse_iterator(end_iterator) : const_reverse_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer - 1, begin_iterator.skipfield_pointer - 1);
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
		if (begin_iterator.group_pointer != NULL)
		{
			end_iterator.group_pointer->next_group = unused_groups;

			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
				{
			#endif // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise the 'destruct' loop out for POD types
			if (total_size != 0)
			{
				while (true) // Erase elements without bothering to update skipfield - much faster:
				{
					const aligned_pointer_type end_pointer = begin_iterator.group_pointer->last_endpoint;

					do
					{
						PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(begin_iterator.element_pointer));
						begin_iterator.element_pointer += static_cast<size_type>(*++begin_iterator.skipfield_pointer) + 1u;
						begin_iterator.skipfield_pointer += static_cast<size_type>(*begin_iterator.skipfield_pointer);
					} while(begin_iterator.element_pointer != end_pointer); // ie. beyond end of available data

					const group_pointer_type next_group = begin_iterator.group_pointer->next_group;
					PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer);
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer, 1);
					begin_iterator.group_pointer = next_group;

					if (next_group == unused_groups)
					{
						break;
					}

					begin_iterator.element_pointer = next_group->elements + *(next_group->skipfield);
					begin_iterator.skipfield_pointer = next_group->skipfield + *(next_group->skipfield);
				}
			}
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			}
			#endif

			while (begin_iterator.group_pointer != NULL)
			{
				const group_pointer_type next_group = begin_iterator.group_pointer->next_group;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer, 1);
				begin_iterator.group_pointer = next_group;
			}
		}
	}



	void initialize(const skipfield_type first_group_size)
	{
		begin_iterator.group_pointer = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer, first_group_size);
			#else
				PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer, group(first_group_size));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, begin_iterator.group_pointer, 1);
			begin_iterator.group_pointer = NULL;
			throw;
		}

		end_iterator.group_pointer = begin_iterator.group_pointer;
		end_iterator.element_pointer = begin_iterator.element_pointer = begin_iterator.group_pointer->elements;
		end_iterator.skipfield_pointer = begin_iterator.skipfield_pointer = begin_iterator.group_pointer->skipfield;
		total_capacity = first_group_size;
	}



	void update_skipblock(const iterator &new_location, const skipfield_type prev_free_list_index) PLF_COLONY_NOEXCEPT
	{
		const skipfield_type new_value = static_cast<skipfield_type>(*(new_location.skipfield_pointer) - 1);

		if (new_value != 0) // ie. skipfield was not 1, ie. a single-node skipblock, with no additional nodes to update
		{
			// set (new) start and (original) end of skipblock to new value:
			*(new_location.skipfield_pointer + new_value) = *(new_location.skipfield_pointer + 1) = new_value;

			// transfer free list node to new start node:
			++(groups_with_erasures_list_head->free_list_head);

			if (prev_free_list_index != std::numeric_limits<skipfield_type>::max()) // ie. not the tail free list node
			{
				*(reinterpret_cast<skipfield_pointer_type>(new_location.group_pointer->elements + prev_free_list_index) + 1) = groups_with_erasures_list_head->free_list_head;
			}

			*(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer + 1)) = prev_free_list_index;
			*(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer + 1) + 1) = std::numeric_limits<skipfield_type>::max();
		}
		else
		{
			groups_with_erasures_list_head->free_list_head = prev_free_list_index;

			if (prev_free_list_index != std::numeric_limits<skipfield_type>::max()) // ie. not the last free list node
			{
				*(reinterpret_cast<skipfield_pointer_type>(new_location.group_pointer->elements + prev_free_list_index) + 1) = std::numeric_limits<skipfield_type>::max();
			}
			else
			{
				groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group;
			}
		}

		*(new_location.skipfield_pointer) = 0;
		++(new_location.group_pointer->size);

		if (new_location.group_pointer == begin_iterator.group_pointer && new_location.element_pointer < begin_iterator.element_pointer)
		{ /* ie. begin_iterator was moved forwards as the result of an erasure at some point, this erased element is before the current begin, hence, set current begin iterator to this element */
			begin_iterator = new_location;
		}

		++total_size;
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
						if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
						{ // For no good reason this compiles to ridiculously faster code under GCC 5-9 in raw small struct tests with large N:
							PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
							end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
						}
						else
					#endif
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), element);
						end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer; // Shift the addition to the second operation, avoiding a try-catch block if an exception is thrown during construction
					}

					++(end_iterator.group_pointer->size);
					++end_iterator.skipfield_pointer;
					++total_size;

					return return_iterator; // return value before incrementation
				}
				case 1: // ie. there are no erased elements and end_iterator is at end of current final group - ie. colony is full - create new group
				{
					group_pointer_type next_group;

					if (unused_groups == NULL)
					{
						next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
						const skipfield_type new_group_size = (total_size < static_cast<size_type>(group_allocator_pair.max_group_capacity)) ? static_cast<skipfield_type>(total_size) : group_allocator_pair.max_group_capacity;

						try
						{
							#ifdef PLF_COLONY_VARIADICS_SUPPORT
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, new_group_size, end_iterator.group_pointer);
							#else
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group(new_group_size, end_iterator.group_pointer));
							#endif
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
							throw;
						}

						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
							{
								PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
							}
							else
						#endif
						{
							try
							{
								PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
							}
							catch (...)
							{
								PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, next_group);
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
								throw;
							}
						}

						total_capacity += new_group_size;
					}
					else
					{
						next_group = unused_groups;
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
						unused_groups = next_group->next_group;
						next_group->reset(1, NULL, end_iterator.group_pointer, end_iterator.group_pointer->group_number + 1u);
					}

					end_iterator.group_pointer->next_group = next_group;
					end_iterator.group_pointer = next_group;
					end_iterator.element_pointer = next_group->last_endpoint;
					end_iterator.skipfield_pointer = next_group->skipfield + 1;
					++total_size;

					return iterator(next_group, next_group->elements, next_group->skipfield); /* returns value before incrementation */
				}
				default: // ie. there are erased elements, reuse previous-erased element locations
				{
					iterator new_location(groups_with_erasures_list_head, groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head, groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head);

					// We always reuse the element at the start of the skipblock, this is also where the free-list information for that skipblock is stored. Get the previous free-list node's index from this memory space, before we write to our element to it. 'Next' index is always the free_list_head (as represented by the maximum value of the skipfield type) here so we don't need to get it:
					const skipfield_type prev_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), element);
					update_skipblock(new_location, prev_free_list_index);

					return new_location;
				}
			}
		}
		else // ie. newly-constructed colony, no insertions yet and no groups
		{
			initialize(pointer_allocator_pair.min_group_capacity);

			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
				}
				else
			#endif
			{
				try
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), element);
				}
				catch (...)
				{
					clear();
					throw;
				}
			}

			++end_iterator.skipfield_pointer;
			total_size = 1;
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
							if PLF_COLONY_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
							{
								PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
								end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
							}
							else
						#endif
						{
							PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), std::move(element));
							end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer;
						}

						++(end_iterator.group_pointer->size);
						++end_iterator.skipfield_pointer;
						++total_size;

						return return_iterator;
					}
					case 1:
					{
						group_pointer_type next_group;

						if (unused_groups == NULL)
						{
							next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
							const skipfield_type new_group_size = (total_size < static_cast<size_type>(group_allocator_pair.max_group_capacity)) ? static_cast<skipfield_type>(total_size) : group_allocator_pair.max_group_capacity;

							try
							{
								#ifdef PLF_COLONY_VARIADICS_SUPPORT
									PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, new_group_size, end_iterator.group_pointer);
								#else
									PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group(new_group_size, end_iterator.group_pointer));
								#endif
							}
							catch (...)
							{
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
								throw;
							}

							#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
								if PLF_COLONY_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
								{
									PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
								}
								else
							#endif
							{
								try
								{
									PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
								}
								catch (...)
								{
									PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, next_group);
									PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
									throw;
								}
							}

							total_capacity += new_group_size;
						}
						else
						{
							next_group = unused_groups;
							PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), element);
							unused_groups = next_group->next_group;
							next_group->reset(1, NULL, end_iterator.group_pointer, end_iterator.group_pointer->group_number + 1u);
						}

						end_iterator.group_pointer->next_group = next_group;
						end_iterator.group_pointer = next_group;
						end_iterator.element_pointer = next_group->last_endpoint;
						end_iterator.skipfield_pointer = next_group->skipfield + 1;
						++total_size;

						return iterator(next_group, next_group->elements, next_group->skipfield); /* returns value before incrementation */
					}
					default:
					{
						iterator new_location(groups_with_erasures_list_head, groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head, groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head);

						const skipfield_type prev_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), std::move(element));
						update_skipblock(new_location, prev_free_list_index);

						return new_location;
					}
				}
			}
			else
			{
				initialize(pointer_allocator_pair.min_group_capacity);

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if PLF_COLONY_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
					}
					else
				#endif
				{
					try
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::move(element));
					}
					catch (...)
					{
						clear();
						throw;
					}
				}

				++end_iterator.skipfield_pointer;
				total_size = 1;
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
							if PLF_COLONY_CONSTEXPR (std::is_nothrow_constructible<element_type, arguments ...>::value)
							{
								PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters)...);
								end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
							}
							else
						#endif
						{
							PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), std::forward<arguments>(parameters)...);
							end_iterator.group_pointer->last_endpoint = ++end_iterator.element_pointer;
						}

						++(end_iterator.group_pointer->size);
						++end_iterator.skipfield_pointer;
						++total_size;

						return return_iterator;
					}
					case 1:
					{
						group_pointer_type next_group;

						if (unused_groups == NULL)
						{
							next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
							const skipfield_type new_group_size = (total_size < static_cast<size_type>(group_allocator_pair.max_group_capacity)) ? static_cast<skipfield_type>(total_size) : group_allocator_pair.max_group_capacity;

							try
							{
								#ifdef PLF_COLONY_VARIADICS_SUPPORT
									PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, new_group_size, end_iterator.group_pointer);
								#else
									PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, next_group, group(new_group_size, end_iterator.group_pointer));
								#endif
							}
							catch (...)
							{
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
								throw;
							}

							#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
								if PLF_COLONY_CONSTEXPR (std::is_nothrow_constructible<element_type>::value)
								{
									PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), std::forward<arguments>(parameters) ...);
								}
								else
							#endif
							{
								try
								{
									PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), std::forward<arguments>(parameters) ...);
								}
								catch (...)
								{
									PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, next_group);
									PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, next_group, 1);
									throw;
								}
							}

							total_capacity += new_group_size;
						}
						else
						{
							next_group = unused_groups;
							PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(next_group->elements), std::forward<arguments>(parameters) ...);
							unused_groups = next_group->next_group;
							next_group->reset(1, NULL, end_iterator.group_pointer, end_iterator.group_pointer->group_number + 1u);
						}

						end_iterator.group_pointer->next_group = next_group;
						end_iterator.group_pointer = next_group;
						end_iterator.element_pointer = next_group->last_endpoint;
						end_iterator.skipfield_pointer = next_group->skipfield + 1;
						++total_size;

						return iterator(next_group, next_group->elements, next_group->skipfield); /* returns value before incrementation */
					}
					default:
					{
						iterator new_location(groups_with_erasures_list_head, groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head, groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head);

						const skipfield_type prev_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(new_location.element_pointer));
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(new_location.element_pointer), std::forward<arguments>(parameters) ...);
						update_skipblock(new_location, prev_free_list_index);

						return new_location;
					}
				}
			}
			else
			{
				initialize(pointer_allocator_pair.min_group_capacity);

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if PLF_COLONY_CONSTEXPR (std::is_nothrow_constructible<element_type, arguments ...>::value)
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters) ...);
					}
					else
				#endif
				{
					try
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer++), std::forward<arguments>(parameters) ...);
					}
					catch (...)
					{
						clear();
						throw;
					}
				}

				++end_iterator.skipfield_pointer;
				total_size = 1;
				return begin_iterator;
			}
		}
	#endif




private:

	void fill(const element_type &element, const skipfield_type size)
	{
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
			{
				if PLF_COLONY_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_copy_constructible<element_type>::value) // ie. we can get away with using the cheaper fill_n here if there is no chance of an exception being thrown:
				{
					#ifdef PLF_COLONY_ALIGNMENT_SUPPORT
						if PLF_COLONY_CONSTEXPR (sizeof(aligned_element_type) != sizeof(element_type))
						{
							alignas (alignof(aligned_element_type)) element_type aligned_copy = element; // to avoid potentially violating memory boundaries in line below, create an initial copy object of same (but aligned) type
							std::fill_n(end_iterator.element_pointer, size, *(reinterpret_cast<aligned_pointer_type>(&aligned_copy)));
						}
						else
					#endif
					{ // This also covers C++03, where the type cannot be aligned to anything so it's safe to use fill_n:
						std::fill_n(reinterpret_cast<pointer>(end_iterator.element_pointer), size, element);
					}

					end_iterator.element_pointer += size;
				}
				else // If nothrow_constructible, can remove the large block of 'catch' code below
				{
					const aligned_pointer_type fill_end = end_iterator.element_pointer + size;

					do
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), element);
					} while (++end_iterator.element_pointer != fill_end);
				}
			}
			else
		#endif
		{
			const aligned_pointer_type fill_end = end_iterator.element_pointer + size;

			do
			{
				try
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), element);
				}
				catch (...)
				{
					end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
					const skipfield_type elements_constructed_before_exception = static_cast<skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);
					end_iterator.group_pointer->size = elements_constructed_before_exception;
					end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + elements_constructed_before_exception;
					total_size += elements_constructed_before_exception;
					unused_groups = end_iterator.group_pointer->next_group;
					end_iterator.group_pointer->next_group = NULL;
					throw;
				}
			} while (++end_iterator.element_pointer != fill_end);
		}

		total_size += size;
	}




	void fill_skipblock(const element_type &element, aligned_pointer_type const location, skipfield_pointer_type const skipfield_pointer, const skipfield_type size)
	{
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
			{
				if PLF_COLONY_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_copy_constructible<element_type>::value)
				{
					#ifdef PLF_COLONY_ALIGNMENT_SUPPORT
						if PLF_COLONY_CONSTEXPR (sizeof(aligned_element_type) != sizeof(element_type))
						{
							alignas (alignof(aligned_element_type)) element_type aligned_copy = element;
							std::fill_n(location, size, *(reinterpret_cast<aligned_pointer_type>(&aligned_copy)));
						}
						else
					#endif
					{
						std::fill_n(reinterpret_cast<pointer>(location), size, element);
					}
				}
				else
				{
					const aligned_pointer_type fill_end = location + size;

					for (aligned_pointer_type current_location = location; current_location != fill_end; ++current_location)
					{
						PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(current_location), element);
					}
				}
			}
			else
		#endif
		{
			const aligned_pointer_type fill_end = location + size;
			const skipfield_type prev_free_list_node = *(reinterpret_cast<skipfield_pointer_type>(location)); // in case of exception, grabbing indexes before free_list node is reused

			for (aligned_pointer_type current_location = location; current_location != fill_end; ++current_location)
			{
				try
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(current_location), element);
				}
				catch (...)
				{
					// Reconstruct existing skipblock and free-list indexes to reflect partially-reused skipblock:
					const skipfield_type elements_constructed_before_exception = static_cast<skipfield_type>((current_location - 1) - location);
					groups_with_erasures_list_head->size = static_cast<skipfield_type>(groups_with_erasures_list_head->size + elements_constructed_before_exception);
					total_size += elements_constructed_before_exception;

					std::memset(skipfield_pointer, 0, elements_constructed_before_exception * sizeof(skipfield_type));

					*(reinterpret_cast<skipfield_pointer_type>(location + elements_constructed_before_exception)) = prev_free_list_node;
					*(reinterpret_cast<skipfield_pointer_type>(location + elements_constructed_before_exception) + 1) = std::numeric_limits<skipfield_type>::max();

					const skipfield_type new_skipblock_head_index = static_cast<skipfield_type>(static_cast<skipfield_type>(location - groups_with_erasures_list_head->elements) + elements_constructed_before_exception);
					groups_with_erasures_list_head->free_list_head = new_skipblock_head_index;

					if (prev_free_list_node != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + prev_free_list_node) + 1) = new_skipblock_head_index;
					}

					throw;
				}
			}
		}

		std::memset(skipfield_pointer, 0, size * sizeof(skipfield_type)); // reset skipfield nodes within skipblock to 0
		groups_with_erasures_list_head->size = static_cast<skipfield_type>(groups_with_erasures_list_head->size + size);
		total_size += size;
	}



	void fill_unused_groups(size_type size, const element_type &element, size_type group_number, group_pointer_type previous_group, const group_pointer_type current_group)
	{
		end_iterator.group_pointer = current_group;

		for (; end_iterator.group_pointer->capacity < size; end_iterator.group_pointer = end_iterator.group_pointer->next_group)
		{
			const skipfield_type capacity = end_iterator.group_pointer->capacity;
			end_iterator.group_pointer->reset(capacity, end_iterator.group_pointer->next_group, previous_group, group_number++);
			previous_group = end_iterator.group_pointer;
			size -= static_cast<size_type>(capacity);
			end_iterator.element_pointer = end_iterator.group_pointer->elements;
			fill(element, capacity);
		}

		// Deal with final group (partial fill)
		unused_groups = end_iterator.group_pointer->next_group;
		end_iterator.group_pointer->reset(static_cast<skipfield_type>(size), NULL, previous_group, group_number);
		end_iterator.element_pointer = end_iterator.group_pointer->elements;
		end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + size;
		fill(element, static_cast<skipfield_type>(size));
	}



public:

	// Fill insert

	void insert(size_type size, const element_type &element)
	{
		if (size == 0)
		{
			return;
		}
		else if (size == 1)
		{
			insert(element);
			return;
		}

		if (total_size == 0)
		{
			assign(size, element);
			return;
		}

		reserve(total_size + size);

		// Use up erased locations if available:
		while(groups_with_erasures_list_head != NULL) // skipblock loop: breaks when colony is exhausted of reusable skipblocks, or returns if size == 0
		{
			aligned_pointer_type const element_pointer = groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head;
			skipfield_pointer_type const skipfield_pointer = groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head;
			const skipfield_type skipblock_size = *skipfield_pointer;

			if (groups_with_erasures_list_head == begin_iterator.group_pointer && element_pointer < begin_iterator.element_pointer)
			{
				begin_iterator.element_pointer = element_pointer;
				begin_iterator.skipfield_pointer = skipfield_pointer;
			}

			if (skipblock_size <= size)
			{
				groups_with_erasures_list_head->free_list_head = *(reinterpret_cast<skipfield_pointer_type>(element_pointer)); // set free list head to previous free list node
				fill_skipblock(element, element_pointer, skipfield_pointer, skipblock_size);
				size -= skipblock_size;

				if (groups_with_erasures_list_head->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. there are more skipblocks to be filled in this group
				{
					*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head) + 1) = std::numeric_limits<skipfield_type>::max(); // set 'next' index of new free list head to 'end' (numeric max)
				}
				else
				{
					groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group; // change groups
				}

				if (size == 0)
				{
					return;
				}
			}
			else // skipblock is larger than remaining number of elements
			{
				const skipfield_type prev_index = *(reinterpret_cast<skipfield_pointer_type>(element_pointer)); // save before element location is overwritten
				fill_skipblock(element, element_pointer, skipfield_pointer, static_cast<skipfield_type>(size));
				const skipfield_type new_skipblock_size = static_cast<skipfield_type>(skipblock_size - size);

				// Update skipfield (earlier nodes already memset'd in fill_skipblock function):
				*(skipfield_pointer + size) = new_skipblock_size;
				*(skipfield_pointer + skipblock_size - 1) = new_skipblock_size;
				groups_with_erasures_list_head->free_list_head = static_cast<skipfield_type>(groups_with_erasures_list_head->free_list_head + size); // set free list head to new start node

				// Update free list with new head:
				*(reinterpret_cast<skipfield_pointer_type>(element_pointer + size)) = prev_index;
				*(reinterpret_cast<skipfield_pointer_type>(element_pointer + size) + 1) = std::numeric_limits<skipfield_type>::max();

				if (prev_index != std::numeric_limits<skipfield_type>::max())
				{
					*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + prev_index) + 1) = groups_with_erasures_list_head->free_list_head; // set 'next' index of previous skipblock to new start of skipblock
				}

				return;
			}
		}


		// Use up remaining available element locations in end group:
		// This variable is either the remaining capacity of the group or the number of elements yet to be filled, whichever is smaller:
		const skipfield_type group_remainder = (static_cast<skipfield_type>(
			reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) >= size) ?
			static_cast<skipfield_type>(size) :
			static_cast<skipfield_type>(reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer);

		if (group_remainder != 0)
		{
			fill(element, group_remainder);
			end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
			end_iterator.group_pointer->size = static_cast<skipfield_type>(end_iterator.group_pointer->size + group_remainder);

			if (size == group_remainder) // Ie. remaining capacity was >= remaining elements to be filled
			{
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->size;
				return;
			}

			size -= group_remainder;
		}


		// Use unused groups:
		end_iterator.group_pointer->next_group = unused_groups;
		fill_unused_groups(size, element, end_iterator.group_pointer->group_number + 1, end_iterator.group_pointer, unused_groups);
	}



private:

	template <class iterator_type>
	iterator_type range_fill(iterator_type it, const skipfield_type size)
	{
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
			{
				const aligned_pointer_type fill_end = end_iterator.element_pointer + size;

				do
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), *it++);
				} while (++end_iterator.element_pointer != fill_end);
			}
			else
		#endif
		{
			const aligned_pointer_type fill_end = end_iterator.element_pointer + size;

			do
			{
				try
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(end_iterator.element_pointer), *it++);
				}
				catch (...)
				{
					end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
					const skipfield_type elements_constructed_before_exception = static_cast<skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);
					end_iterator.group_pointer->size = elements_constructed_before_exception;
					end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + elements_constructed_before_exception;
					total_size += elements_constructed_before_exception;
					unused_groups = end_iterator.group_pointer->next_group;
					end_iterator.group_pointer->next_group = NULL;
					throw;
				}
			} while (++end_iterator.element_pointer != fill_end);
		}

		total_size += size;
		return it;
	}



	template <class iterator_type>
	iterator_type range_fill_skipblock(iterator_type it, aligned_pointer_type const location, skipfield_pointer_type const skipfield_pointer, const skipfield_type size)
	{
		const aligned_pointer_type fill_end = location + size;

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
			{
				for (aligned_pointer_type current_location = location; current_location != fill_end; ++current_location)
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(current_location), *it++);
				}
			}
			else
		#endif
		{
			const skipfield_type prev_free_list_node = *(reinterpret_cast<skipfield_pointer_type>(location)); // in case of exception, grabbing indexes before free_list node is reused

			for (aligned_pointer_type current_location = location; current_location != fill_end; ++current_location)
			{
				try
				{
					PLF_COLONY_CONSTRUCT(allocator_type, (*this), reinterpret_cast<pointer>(current_location), *it++);
				}
				catch (...)
				{
					// Reconstruct existing skipblock and free-list indexes to reflect partially-reused skipblock:
					const skipfield_type elements_constructed_before_exception = static_cast<skipfield_type>((current_location - 1) - location);
					groups_with_erasures_list_head->size = static_cast<skipfield_type>(groups_with_erasures_list_head->size + elements_constructed_before_exception);
					total_size += elements_constructed_before_exception;

					std::memset(skipfield_pointer, 0, elements_constructed_before_exception * sizeof(skipfield_type));

					*(reinterpret_cast<skipfield_pointer_type>(location + elements_constructed_before_exception)) = prev_free_list_node;
					*(reinterpret_cast<skipfield_pointer_type>(location + elements_constructed_before_exception) + 1) = std::numeric_limits<skipfield_type>::max();

					const skipfield_type new_skipblock_head_index = static_cast<skipfield_type>(static_cast<skipfield_type>(location - groups_with_erasures_list_head->elements) + elements_constructed_before_exception);
					groups_with_erasures_list_head->free_list_head = new_skipblock_head_index;

					if (prev_free_list_node != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + prev_free_list_node) + 1) = new_skipblock_head_index;
					}

					throw;
				}
			}
		}

		std::memset(skipfield_pointer, 0, size * sizeof(skipfield_type)); // reset skipfield nodes within skipblock to 0
		groups_with_erasures_list_head->size = static_cast<skipfield_type>(groups_with_erasures_list_head->size + size);
		total_size += size;

		return it;
	}



	template <class iterator_type>
	void range_fill_unused_groups(size_type size, iterator_type it, size_type group_number, group_pointer_type previous_group, const group_pointer_type current_group)
	{
		end_iterator.group_pointer = current_group;

		for (; end_iterator.group_pointer->capacity < size; end_iterator.group_pointer = end_iterator.group_pointer->next_group)
		{
			const skipfield_type capacity = end_iterator.group_pointer->capacity;
			end_iterator.group_pointer->reset(capacity, end_iterator.group_pointer->next_group, previous_group, group_number++);
			previous_group = end_iterator.group_pointer;
			size -= static_cast<size_type>(capacity);
			end_iterator.element_pointer = end_iterator.group_pointer->elements;
			it = range_fill(it, capacity);
		}

		// Deal with final group (partial fill)
		unused_groups = end_iterator.group_pointer->next_group;
		end_iterator.group_pointer->reset(static_cast<skipfield_type>(size), NULL, previous_group, group_number);
		end_iterator.element_pointer = end_iterator.group_pointer->elements;
		end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + size;
		range_fill(it, static_cast<skipfield_type>(size));
	}



	template <class iterator_type>
	void range_insert (iterator_type it, size_type size)
	{
		if (size == 0)
		{
			return;
		}
		else if (size == 1)
		{
			insert(*it);
			return;
		}

		if (total_size == 0)
		{
			range_assign(it, size);
			return;
		}


		reserve(total_size + size);

		// Use up erased locations if available:
		while(groups_with_erasures_list_head != NULL) // skipblock loop: breaks when colony is exhausted of reusable skipblocks, or returns if size == 0
		{
			aligned_pointer_type const element_pointer = groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head;
			skipfield_pointer_type const skipfield_pointer = groups_with_erasures_list_head->skipfield + groups_with_erasures_list_head->free_list_head;
			const skipfield_type skipblock_size = *skipfield_pointer;

			if (groups_with_erasures_list_head == begin_iterator.group_pointer && element_pointer < begin_iterator.element_pointer)
			{
				begin_iterator.element_pointer = element_pointer;
				begin_iterator.skipfield_pointer = skipfield_pointer;
			}

			if (skipblock_size <= size)
			{
				groups_with_erasures_list_head->free_list_head = *(reinterpret_cast<skipfield_pointer_type>(element_pointer)); // set free list head to previous free list node
				it = range_fill_skipblock(it, element_pointer, skipfield_pointer, skipblock_size);
				size -= skipblock_size;

				if (groups_with_erasures_list_head->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. there are more skipblocks to be filled in this group
				{
					*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + groups_with_erasures_list_head->free_list_head) + 1) = std::numeric_limits<skipfield_type>::max(); // set 'next' index of new free list head to 'end' (numeric max)
				}
				else
				{
					groups_with_erasures_list_head = groups_with_erasures_list_head->erasures_list_next_group; // change groups
				}

				if (size == 0)
				{
					return;
				}
			}
			else // skipblock is larger than remaining number of elements
			{
				const skipfield_type prev_index = *(reinterpret_cast<skipfield_pointer_type>(element_pointer)); // save before element location is overwritten
				it = range_fill_skipblock(it, element_pointer, skipfield_pointer, static_cast<skipfield_type>(size));
				const skipfield_type new_skipblock_size = static_cast<skipfield_type>(skipblock_size - size);

				// Update skipfield (earlier nodes already memset'd in fill_skipblock function):
				*(skipfield_pointer + size) = new_skipblock_size;
				*(skipfield_pointer + skipblock_size - 1) = new_skipblock_size;
				groups_with_erasures_list_head->free_list_head = static_cast<skipfield_type>(groups_with_erasures_list_head->free_list_head + size); // set free list head to new start node

				// Update free list with new head:
				*(reinterpret_cast<skipfield_pointer_type>(element_pointer + size)) = prev_index;
				*(reinterpret_cast<skipfield_pointer_type>(element_pointer + size) + 1) = std::numeric_limits<skipfield_type>::max();

				if (prev_index != std::numeric_limits<skipfield_type>::max())
				{
					*(reinterpret_cast<skipfield_pointer_type>(groups_with_erasures_list_head->elements + prev_index) + 1) = groups_with_erasures_list_head->free_list_head; // set 'next' index of previous skipblock to new start of skipblock
				}

				return;
			}
		}


		// Use up remaining available element locations in end group:
		// This variable is either the remaining capacity of the group or the number of elements yet to be filled, whichever is smaller:
		const skipfield_type group_remainder = (static_cast<skipfield_type>(
			reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) >= size) ?
			static_cast<skipfield_type>(size) :
			static_cast<skipfield_type>(reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer);

		if (group_remainder != 0)
		{
			it = range_fill(it, group_remainder);
			end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
			end_iterator.group_pointer->size = static_cast<skipfield_type>(end_iterator.group_pointer->size + group_remainder);

			if (size == group_remainder) // Ie. remaining capacity was >= remaining elements to be filled
			{
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->size;
				return;
			}

			size -= group_remainder;
		}


		// Use unused groups:
		end_iterator.group_pointer->next_group = unused_groups;
		range_fill_unused_groups(size, it, end_iterator.group_pointer->group_number + 1, end_iterator.group_pointer, unused_groups);
	}



public:

	// Range insert:

	template <class iterator_type>
	inline void insert (const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type first, const iterator_type last)
	{
		range_insert(first, static_cast<size_type>(distance(first,last)));
	}



	// Range insert, move_iterator overload:

	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		template <class iterator_type>
		inline void insert (const std::move_iterator<iterator_type> first, const std::move_iterator<iterator_type> last)
		{
			range_insert(first, static_cast<size_type>(distance(first.base(),last.base())));
		}
	#endif



	// Initializer-list insert

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		inline void insert (const std::initializer_list<element_type> &element_list)
		{
			range_insert(element_list.begin(), static_cast<size_type>(element_list.size()));
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



	inline void reset_only_group_left(group_pointer_type const group_pointer) PLF_COLONY_NOEXCEPT
	{
		groups_with_erasures_list_head = NULL;
		group_pointer->reset(0, NULL, NULL, 0);

		// Reset begin and end iterators:
		end_iterator.element_pointer = begin_iterator.element_pointer = group_pointer->last_endpoint;
		end_iterator.skipfield_pointer = begin_iterator.skipfield_pointer = group_pointer->skipfield;
	}



	inline PLF_COLONY_FORCE_INLINE void add_group_to_unused_groups_list(group * const group_pointer) PLF_COLONY_NOEXCEPT
	{
		group_pointer->next_group = unused_groups;
		unused_groups = group_pointer;
	}



public:

	// must return iterator to subsequent non-erased element (or end()), in case the group containing the element which the iterator points to becomes empty after the erasure, and is thereafter removed from the colony chain, making the current iterator invalid and unusable in a ++ operation:
	iterator erase(const const_iterator it) // if uninitialized/invalid iterator supplied, function could generate an exception
	{
		assert(total_size != 0);
		assert(it.group_pointer != NULL); // ie. not uninitialized iterator
		assert(it.element_pointer != it.group_pointer->last_endpoint); // ie. != end()
		assert(*(it.skipfield_pointer) == 0); // ie. element pointed to by iterator has not been erased previously

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value) // This if-statement should be removed by the compiler on resolution of element_type. For some optimizing compilers this step won't be necessary (for MSVC 2013 it makes a difference)
		#endif
		{
			PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(it.element_pointer)); // Destruct element
		}

		--total_size;

		if (it.group_pointer->size-- != 1) // ie. non-empty group at this point in time, don't consolidate - optimization note: GCC optimizes postfix - 1 comparison better than prefix - 1 comparison in many cases.
		{
			// Code logic for following section:
			// ---------------------------------
			// If current skipfield node has no skipped node on either side, continue as usual
			// If node only has skipped node on left, set current node and start node of the skipblock to left node value + 1.
			// If node only has skipped node on right, make this node the start node of the skipblock and update end node
			// If node has skipped nodes on left and right, set start node of left skipblock and end node of right skipblock to the values of the left + right nodes + 1

			// Optimization explanation:
			// The contextual logic below is the same as that in the insert() functions but in this case the value of the current skipfield node will always be
			// zero (since it is not yet erased), meaning no additional manipulations are necessary for the previous skipfield node comparison - we only have to check against zero
			const char prev_skipfield = *(it.skipfield_pointer - (it.skipfield_pointer != it.group_pointer->skipfield)) != 0;
			const char after_skipfield = *(it.skipfield_pointer + 1) != 0;  // NOTE: boundary test (checking against end-of-elements) is able to be skipped due to the extra skipfield node (compared to element field) - which is present to enable faster iterator operator ++ operations
			skipfield_type update_value = 1;

			switch ((after_skipfield << 1) | prev_skipfield)
			{
				case 0: // no consecutive erased elements
				{
					*it.skipfield_pointer = 1; // solo skipped node
					const skipfield_type index = static_cast<skipfield_type>(it.element_pointer - it.group_pointer->elements);

					if (it.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. if this group already has some erased elements
					{
						*(reinterpret_cast<skipfield_pointer_type>(it.group_pointer->elements + it.group_pointer->free_list_head) + 1) = index; // set prev free list head's 'next index' number to the index of the current element
					}
					else
					{
						it.group_pointer->erasures_list_next_group = groups_with_erasures_list_head; // add it to the groups-with-erasures free list
						groups_with_erasures_list_head = it.group_pointer;
					}

					*(reinterpret_cast<skipfield_pointer_type>(it.element_pointer)) = it.group_pointer->free_list_head;
					*(reinterpret_cast<skipfield_pointer_type>(it.element_pointer) + 1) = std::numeric_limits<skipfield_type>::max();
					it.group_pointer->free_list_head = index;
					break;
				}
				case 1: // previous erased consecutive elements, none following
				{
					*(it.skipfield_pointer - *(it.skipfield_pointer - 1)) = *it.skipfield_pointer = static_cast<skipfield_type>(*(it.skipfield_pointer - 1) + 1);
					break;
				}
				case 2: // following erased consecutive elements, none preceding
				{
					const skipfield_type following_value = static_cast<skipfield_type>(*(it.skipfield_pointer + 1) + 1);
					*(it.skipfield_pointer + following_value - 1) = *(it.skipfield_pointer) = following_value;

					const skipfield_type following_previous = *(reinterpret_cast<skipfield_pointer_type>(it.element_pointer + 1));
					const skipfield_type following_next = *(reinterpret_cast<skipfield_pointer_type>(it.element_pointer + 1) + 1);
					*(reinterpret_cast<skipfield_pointer_type>(it.element_pointer)) = following_previous;
					*(reinterpret_cast<skipfield_pointer_type>(it.element_pointer) + 1) = following_next;

					const skipfield_type index = static_cast<skipfield_type>(it.element_pointer - it.group_pointer->elements);

					if (following_previous != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(it.group_pointer->elements + following_previous) + 1) = index; // Set next index of previous free list node to this node's 'next' index
					}

					if (following_next != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(it.group_pointer->elements + following_next)) = index;	// Set previous index of next free list node to this node's 'previous' index
					}
					else
					{
						it.group_pointer->free_list_head = index;
					}

					update_value = following_value;
					break;
				}
				case 3: // both preceding and following consecutive erased elements - erased element is between two skipblocks
				{
					const skipfield_type preceding_value = *(it.skipfield_pointer - 1);
					const skipfield_type following_value = static_cast<skipfield_type>(*(it.skipfield_pointer + 1) + 1);

					// Join the skipblocks
					*(it.skipfield_pointer - preceding_value) = *(it.skipfield_pointer + following_value - 1) = static_cast<skipfield_type>(preceding_value + following_value);

					// Remove the following skipblock's entry from the free list
					const skipfield_type following_previous = *(reinterpret_cast<skipfield_pointer_type>(it.element_pointer + 1));
					const skipfield_type following_next = *(reinterpret_cast<skipfield_pointer_type>(it.element_pointer + 1) + 1);

					if (following_previous != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(it.group_pointer->elements + following_previous) + 1) = following_next; // Set next index of previous free list node to this node's 'next' index
					}

					if (following_next != std::numeric_limits<skipfield_type>::max())
					{
						*(reinterpret_cast<skipfield_pointer_type>(it.group_pointer->elements + following_next)) = following_previous; // Set previous index of next free list node to this node's 'previous' index
					}
					else
					{
						it.group_pointer->free_list_head = following_previous;
					}

					update_value = following_value;
					break;
				}
			}

			iterator return_iterator(it.group_pointer, it.element_pointer + update_value, it.skipfield_pointer + update_value);

			if (return_iterator.element_pointer == it.group_pointer->last_endpoint && it.group_pointer->next_group != NULL)
			{
				return_iterator.group_pointer = it.group_pointer->next_group;
				const aligned_pointer_type elements = return_iterator.group_pointer->elements;
				const skipfield_pointer_type skipfield = return_iterator.group_pointer->skipfield;
				const skipfield_type skip = *skipfield;
				return_iterator.element_pointer = elements + skip;
				return_iterator.skipfield_pointer = skipfield + skip;
			}

			if (it.element_pointer == begin_iterator.element_pointer) // If original iterator was first element in colony, update it's value with the next non-erased element:
			{
				begin_iterator = return_iterator;
			}

			return return_iterator;
		}

		// else: group is empty, consolidate groups
		switch((it.group_pointer->next_group != NULL) | ((it.group_pointer != begin_iterator.group_pointer) << 1))
		{
			case 0: // ie. it.group_pointer == begin_iterator.group_pointer && it.group_pointer->next_group == NULL; only group in colony
			{
				// Reset skipfield and free list rather than clearing - leads to fewer allocations/deallocations:
				reset_only_group_left(it.group_pointer);
				return end_iterator;
			}
			case 1: // ie. group_pointer == begin_iterator.group_pointer && group_pointer->next_group != NULL. Remove first group, change first group to next group
			{
				it.group_pointer->next_group->previous_group = NULL; // Cut off this group from the chain
				begin_iterator.group_pointer = it.group_pointer->next_group; // Make the next group the first group

				update_subsequent_group_numbers(begin_iterator.group_pointer);

				if (it.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // Erasures present within the group, ie. was part of the linked list of groups with erasures.
				{
					remove_from_groups_with_erasures_list(it.group_pointer);
				}

				total_capacity -= it.group_pointer->capacity;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, it.group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, it.group_pointer, 1);

				// note: end iterator only needs to be changed if the deleted group was the final group in the chain ie. not in this case
				begin_iterator.element_pointer = begin_iterator.group_pointer->elements + *(begin_iterator.group_pointer->skipfield); // If the beginning index has been erased (ie. skipfield != 0), skip to next non-erased element
				begin_iterator.skipfield_pointer = begin_iterator.group_pointer->skipfield + *(begin_iterator.group_pointer->skipfield);

				return begin_iterator;
			}
			case 3: // this is a non-first group but not final group in chain: delete the group, then link previous group to the next group in the chain:
			{
				it.group_pointer->next_group->previous_group = it.group_pointer->previous_group;
				const group_pointer_type return_group = it.group_pointer->previous_group->next_group = it.group_pointer->next_group; // close the chain, removing this group from it

				update_subsequent_group_numbers(return_group);

				if (it.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(it.group_pointer);
				}

				if (it.group_pointer->next_group != end_iterator.group_pointer)
				{
					total_capacity -= it.group_pointer->capacity;
					PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, it.group_pointer);
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, it.group_pointer, 1);
				}
				else
				{
					add_group_to_unused_groups_list(it.group_pointer);
				}

				// Return next group's first non-erased element:
				return iterator(return_group, return_group->elements + *(return_group->skipfield), return_group->skipfield + *(return_group->skipfield));
			}
			default: // this is a non-first group and the final group in the chain
			{
				if (it.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(it.group_pointer);
				}

				it.group_pointer->previous_group->next_group = NULL;
				end_iterator.group_pointer = it.group_pointer->previous_group; // end iterator needs to be changed as element supplied was the back element of the colony
				end_iterator.element_pointer = reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield);
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->capacity;

				add_group_to_unused_groups_list(it.group_pointer);

				return end_iterator;
			}
		}
	}



	// Range erase:

	iterator erase(const const_iterator iterator1, const const_iterator iterator2)	// if uninitialized/invalid iterators supplied, function could generate an exception. If iterator1 > iterator2, behaviour is undefined.
	{
		assert(iterator1 <= iterator2);

		const_iterator current = iterator1;

		if (current.group_pointer != iterator2.group_pointer) // ie. if start and end iterators are in separate groups
		{
			if (current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // if iterator1 is not the first non-erased element in it's group - most common case
			{
				size_type number_of_group_erasures = 0;

				// Now update skipfield:
				const aligned_pointer_type end = iterator1.group_pointer->last_endpoint;

				// Schema: first erase all non-erased elements until end of group & remove all skipblocks post-iterator1 from the free_list. Then, either update preceding skipblock or create new one:

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT // if trivially-destructible, and C++11 or higher, and no erasures in group, skip while loop below and just jump straight to the location
					if (std::is_trivially_destructible<element_type>::value && current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
					{
						number_of_group_erasures += static_cast<size_type>(end - current.element_pointer);
					}
					else
				#endif
				{
					while (current.element_pointer != end)
					{
						if (*current.skipfield_pointer == 0)
						{
							#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
								if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
							#endif
							{
								PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
							}

							++number_of_group_erasures;
							++current.element_pointer;
							++current.skipfield_pointer;
						}
						else // remove skipblock from group:
						{
							const skipfield_type prev_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(current.element_pointer));
							const skipfield_type next_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(current.element_pointer) + 1);

							current.element_pointer += *(current.skipfield_pointer);
							current.skipfield_pointer += *(current.skipfield_pointer);

							if (next_free_list_index == std::numeric_limits<skipfield_type>::max() && prev_free_list_index == std::numeric_limits<skipfield_type>::max()) // if this is the last skipblock in the free list
							{
								remove_from_groups_with_erasures_list(iterator1.group_pointer); // remove group from list of free-list groups - will be added back in down below, but not worth optimizing for
								iterator1.group_pointer->free_list_head = std::numeric_limits<skipfield_type>::max();
								number_of_group_erasures += static_cast<size_type>(end - current.element_pointer);

								#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
									if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
								#endif
								{
									while (current.element_pointer != end) // miniloop - avoid checking skipfield for rest of elements in group, as there are no more skipped elements now
									{
										PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer++)); // Destruct element
									}
								}

								break; // end overall while loop
							}
							else if (next_free_list_index == std::numeric_limits<skipfield_type>::max()) // if this is the head of the free list
							{
								current.group_pointer->free_list_head = prev_free_list_index; // make free list head equal to next free list node
								*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + prev_free_list_index) + 1) = std::numeric_limits<skipfield_type>::max();
							}
							else // either a tail or middle free list node
							{
								*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + next_free_list_index)) = prev_free_list_index;

								if (prev_free_list_index != std::numeric_limits<skipfield_type>::max()) // ie. not the tail free list node
								{
									*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + prev_free_list_index) + 1) = next_free_list_index;
								}
							}
						}
					}
				}


				const skipfield_type previous_node_value = *(iterator1.skipfield_pointer - 1);
				const skipfield_type distance_to_end = static_cast<skipfield_type>(end - iterator1.element_pointer);


				if (previous_node_value == 0) // no previous skipblock
				{
					*iterator1.skipfield_pointer = distance_to_end; // set start node value
					*(iterator1.skipfield_pointer + distance_to_end - 1) = distance_to_end; // set end node value

					const skipfield_type index = static_cast<skipfield_type>(iterator1.element_pointer - iterator1.group_pointer->elements);

					if (iterator1.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. if this group already has some erased elements
					{
						*(reinterpret_cast<skipfield_pointer_type>(iterator1.group_pointer->elements + iterator1.group_pointer->free_list_head) + 1) = index; // set prev free list head's 'next index' number to the index of the iterator1 element
					}
					else
					{
						iterator1.group_pointer->erasures_list_next_group = groups_with_erasures_list_head; // add it to the groups-with-erasures free list
						groups_with_erasures_list_head = iterator1.group_pointer;
					}

					*(reinterpret_cast<skipfield_pointer_type>(iterator1.element_pointer)) = iterator1.group_pointer->free_list_head;
					*(reinterpret_cast<skipfield_pointer_type>(iterator1.element_pointer) + 1) = std::numeric_limits<skipfield_type>::max();
					iterator1.group_pointer->free_list_head = index;
				}
				else
				{ // update previous skipblock, no need to update free list:
					*(iterator1.skipfield_pointer - previous_node_value) = *(iterator1.skipfield_pointer + distance_to_end - 1) = static_cast<skipfield_type>(previous_node_value + distance_to_end);
				}

				iterator1.group_pointer->size = static_cast<skipfield_type>(iterator1.group_pointer->size - number_of_group_erasures);
				total_size -= number_of_group_erasures;

				current.group_pointer = current.group_pointer->next_group;
			}


			// Intermediate groups:
			const group_pointer_type previous_group = current.group_pointer->previous_group;

			while (current.group_pointer != iterator2.group_pointer)
			{
				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
				#endif
				{
					current.element_pointer = current.group_pointer->elements + *(current.group_pointer->skipfield);
					current.skipfield_pointer = current.group_pointer->skipfield + *(current.group_pointer->skipfield);
					const aligned_pointer_type end = current.group_pointer->last_endpoint;

					do
					{
						PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
						const skipfield_type skip = *(++current.skipfield_pointer);
						current.element_pointer += static_cast<size_type>(skip) + 1u;
						current.skipfield_pointer += skip;
					} while (current.element_pointer != end);
				}

				if (current.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(current.group_pointer);
				}

				total_size -= current.group_pointer->size;
				const group_pointer_type current_group = current.group_pointer;
				current.group_pointer = current.group_pointer->next_group;

				if (current_group != end_iterator.group_pointer && current_group->next_group != end_iterator.group_pointer)
				{
					total_capacity -= current_group->capacity;
					PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, current_group);
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
				}
				else
				{
					add_group_to_unused_groups_list(current_group);
				}
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
				begin_iterator = iterator2; // This line is included here primarily to avoid a secondary if statement within the if block below - it will not be needed in any other situation
			}
		}

		if (current.element_pointer == iterator2.element_pointer) // in case iterator2 was at beginning of it's group - also covers empty range case (first == last)
		{
			return iterator2;
		}

		// Final group:
		// Code explanation:
		// If not erasing entire final group, 1. Destruct elements (if non-trivial destructor) and add locations to group free list. 2. process skipfield.
		// If erasing entire group, 1. Destruct elements (if non-trivial destructor), 2. if no elements left in colony, clear() 3. otherwise reset end_iterator and remove group from groups-with-erasures list (if free list of erasures present)

		if (iterator2.element_pointer != end_iterator.element_pointer || current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // ie. not erasing entire group
		{
			size_type number_of_group_erasures = 0;
			// Schema: first erased all non-erased elements until end of group & remove all skipblocks post-iterator2 from the free_list. Then, either update preceding skipblock or create new one:

			const const_iterator current_saved = current;

			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT // if trivially-destructible, and C++11 or higher, and no erasures in group, skip while loop below and just jump straight to the location
				if (std::is_trivially_destructible<element_type>::value && current.group_pointer->free_list_head == std::numeric_limits<skipfield_type>::max())
				{
					number_of_group_erasures += static_cast<size_type>(iterator2.element_pointer - current.element_pointer);
				}
				else
			#endif
			{
				while (current.element_pointer != iterator2.element_pointer)
				{
					if (*current.skipfield_pointer == 0)
					{
						#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
							if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
						#endif
						{
							PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer)); // Destruct element
						}

						++number_of_group_erasures;
						++current.element_pointer;
						++current.skipfield_pointer;
					}
					else // remove skipblock from group:
					{
						const skipfield_type prev_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(current.element_pointer));
						const skipfield_type next_free_list_index = *(reinterpret_cast<skipfield_pointer_type>(current.element_pointer) + 1);

						current.element_pointer += *(current.skipfield_pointer);
						current.skipfield_pointer += *(current.skipfield_pointer);

						if (next_free_list_index == std::numeric_limits<skipfield_type>::max() && prev_free_list_index == std::numeric_limits<skipfield_type>::max()) // if this is the last skipblock in the free list
						{
							remove_from_groups_with_erasures_list(iterator2.group_pointer); // remove group from list of free-list groups - will be added back in down below, but not worth optimizing for
							iterator2.group_pointer->free_list_head = std::numeric_limits<skipfield_type>::max();
							number_of_group_erasures += static_cast<size_type>(iterator2.element_pointer - current.element_pointer);

							#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
								if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
							#endif
							{
								while (current.element_pointer != iterator2.element_pointer)
								{
									PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer++)); // Destruct element
								}
							}

							break; // end overall while loop
						}
						else if (next_free_list_index == std::numeric_limits<skipfield_type>::max()) // if this is the head of the free list
						{
							current.group_pointer->free_list_head = prev_free_list_index;
							*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + prev_free_list_index) + 1) = std::numeric_limits<skipfield_type>::max();
						}
						else
						{
							*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + next_free_list_index)) = prev_free_list_index;

							if (prev_free_list_index != std::numeric_limits<skipfield_type>::max()) // ie. not the tail free list node
							{
								*(reinterpret_cast<skipfield_pointer_type>(current.group_pointer->elements + prev_free_list_index) + 1) = next_free_list_index;
							}
						}
					}
				}
			}


			const skipfield_type distance_to_iterator2 = static_cast<skipfield_type>(iterator2.element_pointer - current_saved.element_pointer);
			const skipfield_type index = static_cast<skipfield_type>(current_saved.element_pointer - iterator2.group_pointer->elements);


			if (index == 0 || *(current_saved.skipfield_pointer - 1) == 0) // element is either at start of group or previous skipfield node is 0
			{
				*(current_saved.skipfield_pointer) = distance_to_iterator2;
				*(iterator2.skipfield_pointer - 1) = distance_to_iterator2;

				if (iterator2.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. if this group already has some erased elements
				{
					*(reinterpret_cast<skipfield_pointer_type>(iterator2.group_pointer->elements + iterator2.group_pointer->free_list_head) + 1) = index;
				}
				else
				{
					iterator2.group_pointer->erasures_list_next_group = groups_with_erasures_list_head; // add it to the groups-with-erasures free list
					groups_with_erasures_list_head = iterator2.group_pointer;
				}

				*(reinterpret_cast<skipfield_pointer_type>(current_saved.element_pointer)) = iterator2.group_pointer->free_list_head;
				*(reinterpret_cast<skipfield_pointer_type>(current_saved.element_pointer) + 1) = std::numeric_limits<skipfield_type>::max();
				iterator2.group_pointer->free_list_head = index;
			}
			else // If iterator 1 & 2 are in same group, but iterator 1 was not at start of group, and previous skipfield node is an end node in a skipblock:
			{
				// Just update existing skipblock, no need to create new free list node:
				const skipfield_type prev_node_value = *(current_saved.skipfield_pointer - 1);
				*(current_saved.skipfield_pointer - prev_node_value) = static_cast<skipfield_type>(prev_node_value + distance_to_iterator2);
				*(iterator2.skipfield_pointer - 1) = static_cast<skipfield_type>(prev_node_value + distance_to_iterator2);
			}


			if (iterator1.element_pointer == begin_iterator.element_pointer)
			{
				begin_iterator = iterator2;
			}

			iterator2.group_pointer->size = static_cast<skipfield_type>(iterator2.group_pointer->size - number_of_group_erasures);
			total_size -= number_of_group_erasures;
		}
		else // ie. full group erasure
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
			#endif
			{
				while(current.element_pointer != iterator2.element_pointer)
				{
					PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer));
					++current.skipfield_pointer;
					current.element_pointer += static_cast<size_type>(*current.skipfield_pointer) + 1u;
					current.skipfield_pointer += *current.skipfield_pointer;
				}
			}


			if ((total_size -= current.group_pointer->size) != 0) // ie. either previous_group != NULL or next_group != NULL
			{
				if (current.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max())
				{
					remove_from_groups_with_erasures_list(current.group_pointer);
				}

				current.group_pointer->previous_group->next_group = current.group_pointer->next_group;

				if (current.group_pointer == end_iterator.group_pointer)
				{
					end_iterator.group_pointer = current.group_pointer->previous_group;
					end_iterator.element_pointer = end_iterator.group_pointer->last_endpoint;
					end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->capacity;
					add_group_to_unused_groups_list(current.group_pointer);
					return end_iterator;
				}
				else if (current.group_pointer == begin_iterator.group_pointer)
				{
					begin_iterator.group_pointer = current.group_pointer->next_group;
					const skipfield_type skip = *(begin_iterator.group_pointer->skipfield);
					begin_iterator.element_pointer = begin_iterator.group_pointer->elements + skip;
					begin_iterator.skipfield_pointer = begin_iterator.group_pointer->skipfield + skip;
				}

				if (current.group_pointer->next_group != end_iterator.group_pointer)
				{
					total_capacity -= current.group_pointer->capacity;
				}
				else
				{
					add_group_to_unused_groups_list(current.group_pointer);
					return iterator2;
				}
			}
			else // ie. colony is now empty
			{
				// Reset skipfield and free list rather than clearing - leads to fewer allocations/deallocations:
				reset_only_group_left(current.group_pointer);
				return end_iterator;
			}

			PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, current.group_pointer);
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current.group_pointer, 1);
		}

		return iterator2;
	}



private:

	void prepare_groups_for_assign(const size_type size)
	{
		// Destroy all elements if non-trivial:
		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
		#endif
		{
			for (iterator current = begin_iterator; current != end_iterator; ++current)
			{
				PLF_COLONY_DESTROY(allocator_type, (*this), reinterpret_cast<pointer>(current.element_pointer));
			}
		}

		if (size < total_capacity && (total_capacity - size) >= pointer_allocator_pair.min_group_capacity)
		{
			size_type difference = total_capacity - size;
			end_iterator.group_pointer->next_group = unused_groups;

			// Remove surplus groups which're under the difference limit:
			group_pointer_type current_group = begin_iterator.group_pointer, previous_group = NULL;

			do
			{
				const group_pointer_type next_group = current_group->next_group;

				if (current_group->capacity <= difference)
				{ // Remove group:
					difference -= current_group->capacity;
					total_capacity -= current_group->capacity;

					PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, current_group);
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);

					if (current_group == begin_iterator.group_pointer)
					{
						begin_iterator.group_pointer = next_group;
					}
				}
				else
				{
					if (previous_group != NULL)
					{
						previous_group->next_group = current_group;
					}

					previous_group = current_group;
				}

				current_group = next_group;
			} while (current_group != NULL);

			previous_group->next_group = NULL;
		}
		else
		{
			if (size > total_capacity)
			{
				reserve(size);
			}

			// Join all unused_groups to main chain:
			end_iterator.group_pointer->next_group = unused_groups;
		}

		begin_iterator.element_pointer = begin_iterator.group_pointer->elements;
		begin_iterator.skipfield_pointer = begin_iterator.group_pointer->skipfield;
  		groups_with_erasures_list_head = NULL;
		total_size = 0;
	}


public:


	// Fill assign:

	inline void assign(size_type size, const element_type &element)
	{
		if (size == 0)
		{
			clear();
			return;
		}

		prepare_groups_for_assign(size);
		fill_unused_groups(size, element, 0, NULL, begin_iterator.group_pointer);
	}



private:

	// Range assign core:

	template <class iterator_type>
	inline void range_assign(iterator_type it, size_type size)
	{
		if (size == 0)
		{
			clear();
			return;
		}

		prepare_groups_for_assign(size);
		range_fill_unused_groups(size, it, 0, NULL, begin_iterator.group_pointer);
	}




public:

	// Range assign:

	template <class iterator_type>
	inline void assign(const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type first, const iterator_type last)
	{
		range_assign(first, static_cast<size_type>(distance(first,last)));
	}



	// Range assign, move_iterator overload:

	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		template <class iterator_type>
		inline void assign (const std::move_iterator<iterator_type> first, const std::move_iterator<iterator_type> last)
		{
			range_assign(first, static_cast<size_type>(distance(first.base(),last.base())));
		}
	#endif


	// Initializer-list assign:

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		inline void assign(const std::initializer_list<element_type> &element_list)
		{
			range_assign(element_list.begin(), static_cast<size_type>(element_list.size()));
		}
	#endif




	#ifdef PLF_COLONY_CPP20_SUPPORT
		[[nodiscard]]
	#endif
	inline PLF_COLONY_FORCE_INLINE bool empty() const PLF_COLONY_NOEXCEPT
	{
		return total_size == 0;
	}



	inline size_type size() const PLF_COLONY_NOEXCEPT
	{
		return total_size;
	}



	#ifdef PLF_COLONY_TEST_DEBUG // used for debugging during internal testing only:
		inline size_type group_size_sum() const PLF_COLONY_NOEXCEPT
		{
			size_type temp = 0;

			for (group_pointer_type current = begin_iterator.group_pointer; current != NULL; current = current->next_group)
			{
				temp += current->size;
			}

			return temp;
		}
	#endif



	inline size_type max_size() const PLF_COLONY_NOEXCEPT
	{
		#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			return std::allocator_traits<allocator_type>::max_size(*this);
		#else
			return allocator_type::max_size();
		#endif
	}



	inline size_type capacity() const PLF_COLONY_NOEXCEPT
	{
		return total_capacity;
	}



	inline size_type memory() const PLF_COLONY_NOEXCEPT
	{
		size_type mem = sizeof(*this) + // sizeof colony basic structure
			(total_capacity * (sizeof(aligned_element_type) + sizeof(skipfield_type))) + // sizeof current colony data capacity + skipfields
			((end_iterator.group_pointer == NULL) ? 0 : ((end_iterator.group_pointer->group_number + 1) * (sizeof(group) + sizeof(skipfield_type)))); // if colony not empty, add the memory use of the group structures themselves, adding the extra skipfield node per-group

		for(group_pointer_type current = unused_groups; current != NULL; current = current->next_group)
		{
			mem += (sizeof(group) + sizeof(skipfield_type));
		}

		return mem;
	}



private:

	// get all elements contiguous in memory and shrink to fit, remove erasures and erasure free lists. Invalidates all iterators and pointers to elements.
	void consolidate()
	{
		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			colony temp(plf::limits(pointer_allocator_pair.min_group_capacity, group_allocator_pair.max_group_capacity));
			temp.range_assign(std::make_move_iterator(begin_iterator), total_size);
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			colony temp(*this);
			swap(temp);
		#endif
	}



public:


	void reshape(const plf::limits capacities)
	{
		check_capacities_conformance(capacities);
		pointer_allocator_pair.min_group_capacity = static_cast<skipfield_type>(capacities.min);
		group_allocator_pair.max_group_capacity = static_cast<skipfield_type>(capacities.max);

		// Need to check all group sizes here, because splice might append smaller blocks to the end of a larger block:
		for (group_pointer_type current = begin_iterator.group_pointer; current != end_iterator.group_pointer; current = current->next_group)
		{
			if (current->capacity < pointer_allocator_pair.min_group_capacity || current->capacity > group_allocator_pair.max_group_capacity)
			{
				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT // If type is non-copyable/movable, cannot be consolidated, throw exception:
					if PLF_COLONY_CONSTEXPR (!((std::is_copy_constructible<element_type>::value && std::is_copy_assignable<element_type>::value) || (std::is_move_constructible<element_type>::value && std::is_move_assignable<element_type>::value)))
					{
						throw;
					}
					else
				#endif
				{
					consolidate();
				}

				return;
			}
		}
	}



	inline plf::limits block_limits() const PLF_COLONY_NOEXCEPT
	{
		return plf::limits(static_cast<size_t>(pointer_allocator_pair.min_group_capacity), static_cast<size_t>(group_allocator_pair.max_group_capacity));
	}



	inline PLF_COLONY_FORCE_INLINE void clear() PLF_COLONY_NOEXCEPT
	{
		destroy_all_data();
		blank();
	}



	inline colony & operator = (const colony &source)
	{
		assert(&source != this);
		range_assign(source.begin_iterator, source.total_size);
		return *this;
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		colony & operator = (colony &&source) PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(allocator_type)
		{
			assert(&source != this);
			destroy_all_data();

			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if PLF_COLONY_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<aligned_pointer_type>::value && std::is_trivial<skipfield_pointer_type>::value)
				{
					std::memcpy(static_cast<void *>(this), &source, sizeof(colony));
				}
				else
			#endif
			{
				end_iterator = std::move(source.end_iterator);
				begin_iterator = std::move(source.begin_iterator);
				groups_with_erasures_list_head = std::move(source.groups_with_erasures_list_head);
				unused_groups =  std::move(source.unused_groups);
				total_size = source.total_size;
				total_capacity = source.total_capacity;
				pointer_allocator_pair.min_group_capacity = source.pointer_allocator_pair.min_group_capacity;
				group_allocator_pair.max_group_capacity = source.group_allocator_pair.max_group_capacity;
			}

			source.blank();
			return *this;
		}
	#endif



	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		inline colony & operator = (const std::initializer_list<element_type> &element_list)
		{
			range_assign(element_list.begin(), static_cast<size_type>(element_list.size()));
			return *this;
		}
	#endif



	friend bool operator == (const colony &lh, const colony &rh)
	{
		if (lh.total_size != rh.total_size)
		{
			return false;
		}

		for (const_iterator lh_iterator = lh.begin_iterator, rh_iterator = rh.begin_iterator; lh_iterator != lh.end_iterator; ++lh_iterator, ++rh_iterator)
		{
			if (*lh_iterator != *rh_iterator)
			{
				return false;
			}
		}

		return true;
	}



	friend bool operator != (const colony &lh, const colony &rh)
	{
		return !(lh == rh);
	}



	void shrink_to_fit()
	{
		if (total_size == total_capacity)
		{
			return;
		}
		else if (total_size == 0)
		{
			clear();
			return;
		}

		consolidate();
	}



	void trim() PLF_COLONY_NOEXCEPT
	{
		while(unused_groups != NULL)
		{
			total_capacity -= unused_groups->capacity;
			const group_pointer_type next_group = unused_groups->next_group;
			PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, unused_groups);
			PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, unused_groups, 1);
			unused_groups = next_group;
		}
	}



	void reserve(size_type new_capacity)
	{
		if (new_capacity == 0 || new_capacity <= total_capacity) // We already have enough space allocated
		{
			return;
		}

		if (new_capacity > max_size())
		{
			throw std::length_error("Capacity requested via reserve() greater than max_size()");
		}

		new_capacity -= total_capacity;

		size_type number_of_max_groups = new_capacity / group_allocator_pair.max_group_capacity;
		skipfield_type remainder = static_cast<skipfield_type>(new_capacity - (number_of_max_groups * group_allocator_pair.max_group_capacity));


		if (remainder == 0)
		{
			remainder = group_allocator_pair.max_group_capacity;
			--number_of_max_groups;
		}
		else if (remainder < pointer_allocator_pair.min_group_capacity)
		{
			remainder = pointer_allocator_pair.min_group_capacity;
		}


		group_pointer_type current_group, first_unused_group;

		if (begin_iterator.group_pointer == NULL) // Most common scenario - empty colony
		{
			initialize(remainder);
			begin_iterator.group_pointer->last_endpoint = begin_iterator.group_pointer->elements; // last_endpoint initially == elements + 1 via default constructor
			begin_iterator.group_pointer->size = 0; // 1 by default

			if (number_of_max_groups == 0)
			{
				return;
			}
			else
			{
				first_unused_group = current_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, begin_iterator.group_pointer);

				try
				{
					#ifdef PLF_COLONY_VARIADICS_SUPPORT
						PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group, group_allocator_pair.max_group_capacity);
					#else
						PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group, group(group_allocator_pair.max_group_capacity));
					#endif
				}
				catch (...)
				{
					current_group = NULL;
					PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
					throw;
				}

				total_capacity += group_allocator_pair.max_group_capacity;
				--number_of_max_groups;
			}
		}
		else // Non-empty colony, add first group:
		{
			first_unused_group = current_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);

			try
			{
				#ifdef PLF_COLONY_VARIADICS_SUPPORT
					PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group, remainder, end_iterator.group_pointer);
				#else
					PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group, group(remainder, end_iterator.group_pointer));
				#endif
			}
			catch (...)
			{
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
				current_group = NULL;
				throw;
			}

			total_capacity += remainder;
		}


		while (number_of_max_groups != 0)
		{
			try
			{
				current_group->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group);
			}
			catch (...)
			{
				current_group->next_group = unused_groups;
				unused_groups = first_unused_group;
				throw;
			}


			try
			{
				#ifdef PLF_COLONY_VARIADICS_SUPPORT
					PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group_allocator_pair.max_group_capacity, current_group);
				#else
					PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group(group_allocator_pair.max_group_capacity, current_group));
				#endif
			}
			catch (...)
			{
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
				current_group->next_group = unused_groups;
				unused_groups = first_unused_group;
				throw;
			}

			current_group = current_group->next_group;
			total_capacity += group_allocator_pair.max_group_capacity;
			--number_of_max_groups;
		}

		current_group->next_group = unused_groups;
		unused_groups = first_unused_group;
	}



	iterator get_iterator_from_pointer(const pointer element_pointer) const PLF_COLONY_NOEXCEPT
	{
		if (total_size != 0)
		{
			 // Start with last group first, as will be the largest group in most cases:
			for (group_pointer_type current_group = end_iterator.group_pointer; current_group != NULL; current_group = current_group->previous_group)
			{
				if (reinterpret_cast<aligned_pointer_type>(element_pointer) >= current_group->elements && reinterpret_cast<aligned_pointer_type>(element_pointer) < reinterpret_cast<aligned_pointer_type>(current_group->skipfield))
				{
					const skipfield_pointer_type skipfield_pointer = current_group->skipfield + (reinterpret_cast<aligned_pointer_type>(element_pointer) - current_group->elements);
					return (*skipfield_pointer == 0) ? iterator(current_group, reinterpret_cast<aligned_pointer_type>(element_pointer), skipfield_pointer) : end_iterator; // If element has been erased, return end()
				}
			}
		}

		return end_iterator;
	}



	inline allocator_type get_allocator() const PLF_COLONY_NOEXCEPT
	{
		return allocator_type();
	}



	void splice(colony &source) PLF_COLONY_NOEXCEPT_SPLICE(allocator_type)
	{
		// Process: if there are unused memory spaces at the end of the current back group of the chain, convert them
		// to skipped elements and add the locations to the group's free list.
		// Then link the destination's groups to the source's groups and nullify the source.
		// If the source has more unused memory spaces in the back group than the destination, swap them before processing to reduce the number of locations added to a free list and also subsequent jumps during iteration.

		assert(&source != this);

		if (source.total_size == 0)
		{
			return;
		}
		else if (total_size == 0)
		{
			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(source);
			#else
				clear();
				swap(source);
			#endif

			return;
		}

		// If there's more unused element locations in back memory block of destination than in back memory block of source, swap with source to reduce number of skipped elements during iteration, and reduce size of free-list:
		if ((reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) > (reinterpret_cast<aligned_pointer_type>(source.end_iterator.group_pointer->skipfield) - source.end_iterator.element_pointer))
		{
			swap(source);
		}


		// Correct group sizes if necessary:
		if (source.pointer_allocator_pair.min_group_capacity < pointer_allocator_pair.min_group_capacity)
		{
			pointer_allocator_pair.min_group_capacity = source.pointer_allocator_pair.min_group_capacity;
		}

		if (source.group_allocator_pair.max_group_capacity > group_allocator_pair.max_group_capacity)
		{
			group_allocator_pair.max_group_capacity = source.group_allocator_pair.max_group_capacity;
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
		{	 // Mark unused element memory locations from back group as skipped/erased:

			// Update skipfield:
			const skipfield_type previous_node_value = *(end_iterator.skipfield_pointer - 1);
			end_iterator.group_pointer->last_endpoint = reinterpret_cast<aligned_pointer_type>(end_iterator.group_pointer->skipfield);

			if (previous_node_value == 0) // no previous skipblock
			{
				*end_iterator.skipfield_pointer = distance_to_end;
				*(end_iterator.skipfield_pointer + distance_to_end - 1) = distance_to_end;

				const skipfield_type index = static_cast<skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);

				if (end_iterator.group_pointer->free_list_head != std::numeric_limits<skipfield_type>::max()) // ie. if this group already has some erased elements
				{
					*(reinterpret_cast<skipfield_pointer_type>(end_iterator.group_pointer->elements + end_iterator.group_pointer->free_list_head) + 1) = index; // set prev free list head's 'next index' number to the index of the current element
				}
				else
				{
					end_iterator.group_pointer->erasures_list_next_group = groups_with_erasures_list_head; // add it to the groups-with-erasures free list
					groups_with_erasures_list_head = end_iterator.group_pointer;
				}

				*(reinterpret_cast<skipfield_pointer_type>(end_iterator.element_pointer)) = end_iterator.group_pointer->free_list_head;
				*(reinterpret_cast<skipfield_pointer_type>(end_iterator.element_pointer) + 1) = std::numeric_limits<skipfield_type>::max();
				end_iterator.group_pointer->free_list_head = index;
			}
			else
			{ // update previous skipblock, no need to update free list:
				*(end_iterator.skipfield_pointer - previous_node_value) = *(end_iterator.skipfield_pointer + distance_to_end - 1) = static_cast<skipfield_type>(previous_node_value + distance_to_end);
			}
		}


		// Update subsequent group numbers:
		group_pointer_type current_group = source.begin_iterator.group_pointer;
		size_type current_group_number = end_iterator.group_pointer->group_number;

		do
		{
			current_group->group_number = ++current_group_number;
			current_group = current_group->next_group;
		} while (current_group != NULL);


		// Join the destination and source group chains:
		end_iterator.group_pointer->next_group = source.begin_iterator.group_pointer;
		source.begin_iterator.group_pointer->previous_group = end_iterator.group_pointer;
		end_iterator = source.end_iterator;
		total_size += source.total_size;
		total_capacity += source.total_capacity;

		// Remove source unused groups:
		source.trim();
		source.blank();
	}



private:

	struct less
	{
		bool operator() (const element_type &a, const element_type &b) const PLF_COLONY_NOEXCEPT
		{
			return a < b;
		}
	};



	struct item_index_tuple
	{
		pointer original_location;
		size_type original_index;

		item_index_tuple(const pointer _item, const size_type _index) PLF_COLONY_NOEXCEPT:
			original_location(_item),
			original_index(_index)
		{}
	};



	template <class comparison_function>
	struct sort_dereferencer
	{
		comparison_function stored_instance;

		explicit sort_dereferencer(const comparison_function &function_instance):
			stored_instance(function_instance)
		{}

		sort_dereferencer() PLF_COLONY_NOEXCEPT
		{}

		bool operator() (const item_index_tuple first, const item_index_tuple second)
		{
			return stored_instance(*(first.original_location), *(second.original_location));
		}
	};



public:


	template <class comparison_function>
	void sort(comparison_function compare)
	{
		if (total_size < 2)
		{
			return;
		}

		#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<item_index_tuple>	tuple_allocator_type;
		#else
			typedef typename allocator_type::template rebind<item_index_tuple>::other	tuple_allocator_type;
		#endif

		tuple_allocator_type tuple_allocator;

		item_index_tuple * const sort_array = PLF_COLONY_ALLOCATE(tuple_allocator_type, tuple_allocator, total_size, NULL);
		item_index_tuple *tuple_pointer = sort_array;

		// Construct pointers to all elements in the sequence:
		size_type index = 0;

		for (iterator current_element = begin_iterator; current_element != end_iterator; ++current_element, ++tuple_pointer, ++index)
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(tuple_allocator_type, tuple_allocator, tuple_pointer, &*current_element, index);
			#else
				PLF_COLONY_CONSTRUCT(tuple_allocator_type, tuple_allocator, tuple_pointer, item_index_tuple(&*current_element, index));
			#endif
		}

		// Now, sort the pointers by the values they point to (std::sort is default sort function if the macro below is not defined):
		#ifndef PLF_COLONY_SORT_FUNCTION
			std::sort(sort_array, sort_array + total_size, sort_dereferencer<comparison_function>(compare));
		#else
			PLF_COLONY_SORT_FUNCTION(sort_array, sort_array + total_size, sort_dereferencer<comparison_function>(compare));
		#endif

		// Sort the actual elements via the tuple array:
		index = 0;

		for (item_index_tuple *current_tuple = sort_array; current_tuple != tuple_pointer; ++current_tuple, ++index)
		{
			if (current_tuple->original_index != index)
			{
				#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
					element_type end_value = std::move(*(current_tuple->original_location));
				#else
					element_type end_value = *(current_tuple->original_location);
				#endif

				size_type destination_index = index;
				size_type source_index = current_tuple->original_index;

				do
				{
					#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
						*(sort_array[destination_index].original_location) = std::move(*(sort_array[source_index].original_location));
					#else
						*(sort_array[destination_index].original_location) = *(sort_array[source_index].original_location);
					#endif

					destination_index = source_index;
					source_index = sort_array[destination_index].original_index;
					sort_array[destination_index].original_index = destination_index;
				} while (source_index != index);

				#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
					*(sort_array[destination_index].original_location) = std::move(end_value);
				#else
					*(sort_array[destination_index].original_location) = end_value;
				#endif
			}
		}

		PLF_COLONY_DEALLOCATE(tuple_allocator_type, tuple_allocator, sort_array, total_size);
	}



	inline void sort()
	{
		sort(less());
	}



	void swap(colony &source) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		assert(&source != this);

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if PLF_COLONY_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<aligned_pointer_type>::value && std::is_trivial<skipfield_pointer_type>::value) // if all pointer types are trivial we can just copy using memcpy - avoids constructors/destructors etc and is faster
			{
				char temp[sizeof(colony)];
				std::memcpy(&temp, static_cast<void *>(this), sizeof(colony));
				std::memcpy(static_cast<void *>(this), static_cast<void *>(&source), sizeof(colony));
				std::memcpy(static_cast<void *>(&source), &temp, sizeof(colony));
			}
			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT // If pointer types are not trivial, moving them is probably going to be more efficient than copying them below
				else if PLF_COLONY_CONSTEXPR (std::is_move_assignable<group_pointer_type>::value && std::is_move_assignable<aligned_pointer_type>::value && std::is_move_assignable<skipfield_pointer_type>::value && std::is_move_constructible<group_pointer_type>::value && std::is_move_constructible<aligned_pointer_type>::value && std::is_move_constructible<skipfield_pointer_type>::value)
				{
					colony temp(std::move(source));
					source = std::move(*this);
					*this = std::move(temp);
				}
			#endif
			else
		#endif
		{
			const iterator						swap_end_iterator = end_iterator, swap_begin_iterator = begin_iterator;
			const group_pointer_type		swap_groups_with_erasures_list_head = groups_with_erasures_list_head, swap_unused_groups = unused_groups;
			const size_type					swap_total_size = total_size, swap_total_capacity = total_capacity;
			const skipfield_type 			swap_min_group_capacity = pointer_allocator_pair.min_group_capacity, swap_max_group_capacity = group_allocator_pair.max_group_capacity;

			end_iterator = source.end_iterator;
			begin_iterator = source.begin_iterator;
			groups_with_erasures_list_head = source.groups_with_erasures_list_head;
			unused_groups = source.unused_groups;
			total_size = source.total_size;
			total_capacity = source.total_capacity;
			pointer_allocator_pair.min_group_capacity = source.pointer_allocator_pair.min_group_capacity;
			group_allocator_pair.max_group_capacity = source.group_allocator_pair.max_group_capacity;

			source.end_iterator = swap_end_iterator;
			source.begin_iterator = swap_begin_iterator;
			source.groups_with_erasures_list_head = swap_groups_with_erasures_list_head;
			source.unused_groups = swap_unused_groups;
			source.total_size = swap_total_size;
			source.total_capacity = swap_total_capacity;
			source.pointer_allocator_pair.min_group_capacity = swap_min_group_capacity;
			source.group_allocator_pair.max_group_capacity = swap_max_group_capacity;
		}
	}

};	// colony


// Used by erase_if()
template<class element_type>
struct colony_eq_to
{
	const element_type value;

	explicit colony_eq_to(const element_type store_value):
		value(store_value)
	{}

	colony_eq_to() PLF_COLONY_NOEXCEPT
	{}

	inline bool operator() (const element_type compare_value) const PLF_COLONY_NOEXCEPT
	{
		return value == compare_value;
	}
};

} // plf namespace



namespace std
{

	template <class element_type, class allocator_type, typename skipfield_type>
	inline void swap (plf::colony<element_type, allocator_type, skipfield_type> &a, plf::colony<element_type, allocator_type, skipfield_type> &b) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		a.swap(b);
	}



	template <class element_type, class allocator_type, typename skipfield_type, class predicate_function>
	typename plf::colony<element_type, allocator_type, skipfield_type>::size_type erase_if(plf::colony<element_type, allocator_type, skipfield_type> &container, predicate_function predicate)
	{
		typedef typename plf::colony<element_type, allocator_type, skipfield_type> colony;
		typedef typename colony::const_iterator 	const_iterator;
      typedef typename colony::size_type 			size_type;
		size_type count = 0;

		for(const_iterator current = container.begin(), end = container.end(); current != end;)
		{
			if (predicate(*current))
			{
   			const size_type original_count = ++count;
				const_iterator last(++const_iterator(current));

				while(last != end && predicate(*last))
				{
					++last;
					++count;
				}

				if (count != original_count)
				{
					current = container.erase(current, last);
				}
				else
				{
					current = container.erase(current);
				}

				end = container.end();
			}
			else
			{
				++current;
			}
		}

		return count;
	}



	template <class element_type, class allocator_type, typename skipfield_type>
	inline typename plf::colony<element_type, allocator_type, skipfield_type>::size_type erase(plf::colony<element_type, allocator_type, skipfield_type> &container, const element_type &value)
	{
		return erase_if(container, plf::colony_eq_to<element_type>(value));
	}
}


#undef PLF_COLONY_MIN_BLOCK_CAPACITY

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
#undef PLF_COLONY_CONSTEXPR
#undef PLF_COLONY_CPP20_SUPPORT
#undef PLF_COLONY_STATIC_ASSERT

#undef PLF_COLONY_CONSTRUCT
#undef PLF_COLONY_DESTROY
#undef PLF_COLONY_ALLOCATE
#undef PLF_COLONY_ALLOCATE_INITIALIZATION
#undef PLF_COLONY_DEALLOCATE

#endif // PLF_COLONY_H
