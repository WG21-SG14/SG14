// Copyright (c) 2017, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

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
		#define PLF_COLONY_VARIADICS_SUPPORT
		#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		#define PLF_COLONY_NOEXCEPT throw()
		#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) 
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
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
		#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4 // 4.3 and below do not support initializer lists
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
		#elif __GNUC__ == 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
		#elif __GNUC__ >= 6 // C++17 support
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#endif
	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#if __GLIBCXX__ >= 20160111
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#elif __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
			#define PLF_COLONY_TYPE_TRAITS_SUPPORT
		#elif __GLIBCXX__ >= 20090421 	// libstdc++ 4.3 and below do not support initializer lists
			#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#endif
	#elif defined(_LIBCPP_VERSION) // No type trait support in libc++ to date
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
	#else // Assume type traits and initializer support for non-GCC compilers and standard libraries
		#define PLF_COLONY_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
		#define PLF_COLONY_INITIALIZER_LIST_SUPPORT
		#define PLF_COLONY_TYPE_TRAITS_SUPPORT
	#endif	

	#define PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
	#define PLF_COLONY_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
	#define PLF_COLONY_MOVE_SEMANTICS_SUPPORT
	#define PLF_COLONY_NOEXCEPT noexcept
	#define PLF_COLONY_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
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


#ifndef PLF_TIMSORT_AVAILABLE
	#include <algorithm> // std::sort instead
#endif

#include <cstring>	// memset, memcpy, memmove
#include <cassert>	// assert
#include <limits>  // std::numeric_limits
#include <memory>	// std::uninitialized_copy, std::allocator
#include <iterator> // std::bidirectional_iterator_tag


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
// Note: unsigned short is equivalent to uint_least16_t ie. Using 16-bit integer in best-case scenario, > or < 16-bit integer in case where platform doesn't support 16-bit types
{
public:
	// Standard container typedefs:
	typedef element_type															value_type;
	typedef element_allocator_type													allocator_type;
	typedef element_skipfield_type													skipfield_type;

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
	template <bool is_const> class colony_iterator;
	typedef colony_iterator<false>		iterator;
	typedef colony_iterator<true>		const_iterator;
	friend class colony_iterator<false>; // Using typedef name here would be illegal under C++03 (according to clang)
	friend class colony_iterator<true>;

	template <bool r_is_const> class colony_reverse_iterator;
	typedef colony_reverse_iterator<false>	reverse_iterator;
	typedef colony_reverse_iterator<true>	const_reverse_iterator;
	friend class colony_reverse_iterator<false>;
	friend class colony_reverse_iterator<true>;


private:


	struct group; // forward declaration for typedefs below

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT // C++11
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group>				group_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<skipfield_type>		skipfield_allocator_type;
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<unsigned char>		uchar_allocator_type; // Using uchar as the generic allocator type, as sizeof is always guaranteed to be 1 byte regardless of the number of bits in a byte on given computer, whereas, for example, uint8_t would fail on machines where there are more than 8 bits in a byte eg. Texas Instruments C54x DSPs.

		typedef typename std::allocator_traits<element_allocator_type>::pointer				element_pointer_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer 				group_pointer_type;
		typedef typename std::allocator_traits<skipfield_allocator_type>::pointer 			skipfield_pointer_type;
		typedef typename std::allocator_traits<uchar_allocator_type>::pointer				uchar_pointer_type;

		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<element_pointer_type>	element_pointer_allocator_type;
	#else
		typedef typename element_allocator_type::template rebind<group>::other				group_allocator_type;
		typedef typename element_allocator_type::template rebind<skipfield_type>::other		skipfield_allocator_type;
		typedef typename element_allocator_type::template rebind<unsigned char>::other		uchar_allocator_type;

		typedef typename element_allocator_type::pointer			element_pointer_type; // Identical typedef to 'pointer', for clarity in code (to differentiate element pointers from group_pointers, etc)
		typedef typename group_allocator_type::pointer 				group_pointer_type;
		typedef typename skipfield_allocator_type::pointer 			skipfield_pointer_type;
		typedef typename uchar_allocator_type::pointer				uchar_pointer_type;

		typedef typename element_allocator_type::template rebind<element_pointer_type>::other element_pointer_allocator_type;
	#endif



	// Reduced stack (for erased-locations)
	class reduced_stack : private element_pointer_allocator_type  // Empty base class optimisation - inheriting allocator functions
	{
	private:
		typedef typename colony::element_pointer_type stack_element_type;
		struct group; // Forward declaration for typedefs below

		#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			typedef typename std::allocator_traits<element_pointer_allocator_type>::template rebind_alloc<group> stack_group_allocator_type;
			typedef typename std::allocator_traits<stack_group_allocator_type>::pointer		stack_group_pointer_type;
			typedef typename std::allocator_traits<element_pointer_allocator_type>::pointer stack_element_pointer_type;
		#else
			typedef typename element_pointer_allocator_type::template rebind<group>::other	stack_group_allocator_type;
			typedef typename stack_group_allocator_type::pointer							stack_group_pointer_type;
			typedef typename element_pointer_allocator_type::pointer						stack_element_pointer_type;
		#endif

		struct group : private element_pointer_allocator_type // Empty base class optimisation - inheriting allocator functions
		{
			const stack_element_pointer_type		elements;
			stack_group_pointer_type				next_group, previous_group;
			const stack_element_pointer_type		end; // End is the actual back element of the memory block, not one-past the back element as it is in colony


			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				group(const size_type elements_per_group, stack_group_pointer_type const previous = NULL):
					elements(PLF_COLONY_ALLOCATE_INITIALIZATION(element_pointer_allocator_type, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
					next_group(NULL),
					previous_group(previous),
					end(elements + elements_per_group - 1)
				{}

			#else
				// This is a hack around the fact that allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
				group(const size_type elements_per_group, stack_group_pointer_type const previous = NULL) PLF_COLONY_NOEXCEPT:
					elements(NULL),
					next_group(reinterpret_cast<stack_group_pointer_type>(elements_per_group)), // Guaranteed by the standard to be safe on any platform where size_type bitdepth == pointer bitdepth (ie. all known platforms)
					previous_group(previous),
					end(NULL)
				{}


				// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
				group(const group &source):
					element_pointer_allocator_type(source),
					elements(PLF_COLONY_ALLOCATE_INITIALIZATION(element_pointer_allocator_type, reinterpret_cast<size_type>(source.next_group), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
					next_group(NULL),
					previous_group(source.previous_group),
					end(elements + reinterpret_cast<size_type>(source.next_group) - 1)
				{}
			#endif


			~group() PLF_COLONY_NOEXCEPT
			{
				// Null check not necessary (for empty group and copied group as above) as delete will ignore NULL pointer.
				PLF_COLONY_DEALLOCATE(element_pointer_allocator_type, (*this), elements, (end + 1) - elements); // Size is calculated from end and elements
			}
		};


		stack_group_pointer_type		current_group, first_group;
		stack_element_pointer_type		top_element, start_element, end_element;
		size_type						total_number_of_elements;
		struct ebco_pair : stack_group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
		{
			size_type min_elements_per_group;
			explicit ebco_pair(const size_type min_elements) : min_elements_per_group(min_elements) {};
		}						group_allocator_pair;

		friend class colony;


	public:

		explicit reduced_stack(const size_type min_allocation_amount = 8, const element_pointer_allocator_type &alloc = element_pointer_allocator_type()):
			element_pointer_allocator_type(alloc),
			current_group(NULL),
			first_group(NULL),
			top_element(NULL),
			start_element(NULL),
			end_element(NULL),
			total_number_of_elements(0),
			group_allocator_pair(min_allocation_amount)
		{}


		reduced_stack(const reduced_stack &source):
			element_pointer_allocator_type(source),
			current_group(NULL),
			first_group(NULL),
			top_element(NULL),
			start_element(NULL),
			end_element(NULL),
			total_number_of_elements(source.total_number_of_elements),
			group_allocator_pair(source.group_allocator_pair.min_elements_per_group)
		{
			if (total_number_of_elements == 0)
			{
				return;
			}

			stack_group_pointer_type current_copy_group = source.first_group;
			const stack_group_pointer_type end_copy_group = source.current_group;

			initialize((total_number_of_elements > group_allocator_pair.min_elements_per_group) ? total_number_of_elements : group_allocator_pair.min_elements_per_group);

			// Copy elements to this stack:
			while (current_copy_group != end_copy_group)
			{
				std::uninitialized_copy(current_copy_group->elements, current_copy_group->end + 1, top_element);
				top_element += (current_copy_group->end + 1) - current_copy_group->elements;
				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);
			top_element += source.top_element - source.start_element; // This should make top_element == the last "pushed" element, rather than the one past it
		}



		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			// move constructor
			reduced_stack(reduced_stack &&source) PLF_COLONY_NOEXCEPT:
				element_pointer_allocator_type(source),
				current_group(std::move(source.current_group)),
				first_group(std::move(source.first_group)),
				top_element(std::move(source.top_element)),
				start_element(std::move(source.start_element)),
				end_element(std::move(source.end_element)),
				total_number_of_elements(source.total_number_of_elements),
				group_allocator_pair(source.group_allocator_pair.min_elements_per_group)
			{
				// Nullify source object's contents - only first_group and total_number_of_elements required for destructor:
				source.first_group = NULL;
				source.total_number_of_elements = 0;
			}



			// Move assignment
			reduced_stack & operator = (reduced_stack &&source) PLF_COLONY_NOEXCEPT
			{
				destroy_all_data();

				current_group = std::move(source.current_group);
				first_group = std::move(source.first_group);
				top_element = std::move(source.top_element);
				start_element = std::move(source.start_element);
				end_element = std::move(source.end_element);
				total_number_of_elements = source.total_number_of_elements;
				group_allocator_pair.min_elements_per_group = source.group_allocator_pair.min_elements_per_group;

				// Nullify source object's contents - only first_group and total_number_of_elements required to be altered for destructor to work on it:
				source.first_group = NULL;
				source.total_number_of_elements = 0;

				return *this;
			}
		#endif                  



		~reduced_stack() 
		{
			destroy_all_data();
		}



	private:

		void destroy_all_data() PLF_COLONY_NOEXCEPT
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (total_number_of_elements != 0 && !(std::is_trivially_destructible<stack_element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with ermpty destructor
			#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
				if (total_number_of_elements != 0)
			#endif
			{
				while (first_group != current_group)
				{
					const stack_element_pointer_type past_end = first_group->end + 1;

					for (stack_element_pointer_type element_pointer = first_group->elements; element_pointer != past_end; ++element_pointer)
					{
						PLF_COLONY_DESTROY(element_pointer_allocator_type, (*this), element_pointer);
					}

					const stack_group_pointer_type next_group = first_group->next_group;
					PLF_COLONY_DESTROY(stack_group_allocator_type, group_allocator_pair, first_group);
					PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, first_group, 1);
					first_group = next_group;
				}

				// Special case for current group:
				const stack_element_pointer_type past_end = top_element + 1;

				for (stack_element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
				{
					PLF_COLONY_DESTROY(element_pointer_allocator_type, (*this), element_pointer);
				}

				first_group = current_group->next_group;
				PLF_COLONY_DESTROY(stack_group_allocator_type, group_allocator_pair, current_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, current_group, 1);
			}

			total_number_of_elements = 0;

			while (first_group != NULL) // trim trailing groups:
			{
				current_group = first_group->next_group;
				PLF_COLONY_DESTROY(stack_group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, first_group, 1);
				first_group = current_group;
			}
		}



		void initialize(const size_type first_group_size)
		{
			first_group = current_group = PLF_COLONY_ALLOCATE(stack_group_allocator_type, group_allocator_pair, 1, 0);

			try
			{
				#ifdef PLF_COLONY_VARIADICS_SUPPORT
					PLF_COLONY_CONSTRUCT(stack_group_allocator_type, group_allocator_pair, first_group, first_group_size);
				#else
					PLF_COLONY_CONSTRUCT(stack_group_allocator_type, group_allocator_pair, first_group, group(first_group_size));
				#endif
			}
			catch (...)
			{
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, first_group, 1);
				first_group = current_group = NULL;
				throw;
			}

			top_element = start_element = first_group->elements;
			end_element = first_group->end;
		}



	public:


		void push(const stack_element_type the_element)
		{
			switch ((top_element == NULL) + (top_element == end_element))
			{
				case 0:
				{
					PLF_COLONY_CONSTRUCT(element_pointer_allocator_type, (*this), ++top_element, the_element); // According to the C++ standard, construction of a pointer (of any type) may not trigger an exception (17.6.3.3) - hence, no try-catch blocks are necessary for storing the erased location pointers.
					++total_number_of_elements;
					return;
				}
				case 1:
				{
					if (current_group->next_group == NULL)
					{
						current_group->next_group = PLF_COLONY_ALLOCATE(stack_group_allocator_type, group_allocator_pair, 1, current_group);

						try
						{
							#ifdef PLF_COLONY_VARIADICS_SUPPORT
								PLF_COLONY_CONSTRUCT(stack_group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < std::numeric_limits<size_type>::max() / 2) ? total_number_of_elements : std::numeric_limits<size_type>::max() / 2, current_group);
							#else
								PLF_COLONY_CONSTRUCT(stack_group_allocator_type, group_allocator_pair, current_group->next_group, group((total_number_of_elements < std::numeric_limits<size_type>::max() / 2) ? total_number_of_elements : std::numeric_limits<size_type>::max() / 2, current_group));
							#endif
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, current_group->next_group, 1);
							current_group->next_group = NULL;
							throw;
						}
					}

					current_group = current_group->next_group;
					start_element = top_element = current_group->elements;
					end_element = current_group->end;

					PLF_COLONY_CONSTRUCT(element_pointer_allocator_type, (*this), top_element, the_element);
					++total_number_of_elements;
					return;
				}
				case 2: // ie. empty stack, must initialize
				{
					initialize(group_allocator_pair.min_elements_per_group);
					PLF_COLONY_CONSTRUCT(element_pointer_allocator_type, (*this), top_element, the_element);
					total_number_of_elements = 1;
					return;
				}
			}
		}



		void pop() PLF_COLONY_NOEXCEPT
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (!(std::is_trivially_destructible<stack_element_type>::value)) // This if-statement should be removed by the compiler on resolution of stack_element_type
			#endif
			{
				PLF_COLONY_DESTROY(element_pointer_allocator_type, (*this), top_element);
			}

			// ie. if total_number_of_elements != 0 after decrement, or we were not already at the start of a non-first group
			if (total_number_of_elements-- == 1 || top_element != start_element) // If total_number_of_elements is now 0 after decrement, this essentially moves top_element back to it's initial position (start_element - 1). But otherwise, this is just a regular pop
			{
				--top_element;
			}
			else
			{ // ie. is start element, but not first group in stack (if it were, totalsize would be 0 after decrement)
				current_group = current_group->previous_group;
				start_element = current_group->elements;
				end_element = top_element = current_group->end;
			}
		}



		size_type approximate_memory_use() const PLF_COLONY_NOEXCEPT
		{
			size_type memory_use = 0; // Ignore self-size, only return size of allocated objects
			stack_group_pointer_type temp_group = first_group;

			while (temp_group != NULL)
			{
				memory_use += static_cast<size_type>((((temp_group->end + 1) - temp_group->elements) * sizeof(stack_element_type)) + sizeof(group));
				temp_group = temp_group->next_group;
			}

			return memory_use;
		}



		void clear() PLF_COLONY_NOEXCEPT
		{
			destroy_all_data(); 
			current_group = NULL;
			top_element = NULL;
			start_element = NULL;
			end_element = NULL;
		}



		void swap(reduced_stack &source) PLF_COLONY_NOEXCEPT_SWAP(element_pointer_allocator_type)
		{
			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				reduced_stack temp(std::move(source));
				source = std::move(*this);
				*this = std::move(temp);
			#else
				const stack_group_pointer_type		swap_current_group = current_group, swap_first_group = first_group;
				const stack_element_pointer_type	swap_top_element = top_element, swap_start_element = start_element, swap_end_element = end_element;
				const size_type				swap_total_number_of_elements = total_number_of_elements, swap_min_elements_per_group = group_allocator_pair.min_elements_per_group;

				current_group = source.current_group;
				first_group = source.first_group;
				top_element = source.top_element;
				start_element = source.start_element;
				end_element = source.end_element;
				total_number_of_elements = source.total_number_of_elements;
				group_allocator_pair.min_elements_per_group = source.group_allocator_pair.min_elements_per_group;

				source.current_group = swap_current_group;
				source.first_group = swap_first_group;
				source.top_element = swap_top_element;
				source.start_element = swap_start_element;
				source.end_element = swap_end_element;
				source.total_number_of_elements = swap_total_number_of_elements;
				source.group_allocator_pair.min_elements_per_group = swap_min_elements_per_group;
			#endif
		}



		void trim_trailing_groups() PLF_COLONY_NOEXCEPT	// Remove trailing stack groups (not removed in general 'pop' usage in reduced_stack for performance reasons). Used by splice and consolidate_erased_locations
		{
			stack_group_pointer_type temp_group = current_group->next_group, temp_group2;
			current_group->next_group = NULL; // Set to NULL regardless of whether it is already NULL (avoids branching). Cuts off rest of groups from this group.

			while (temp_group != NULL)
			{
				temp_group2 = temp_group;
				temp_group = temp_group->next_group;
				PLF_COLONY_DESTROY(stack_group_allocator_type, group_allocator_pair, temp_group2);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, group_allocator_pair, temp_group2, 1);
			}
		}
		
		

		void splice(reduced_stack &source) PLF_COLONY_NOEXCEPT_SWAP(element_pointer_allocator_type)
		{
			// Process: if there are unused memory spaces at the end of the last current back group of the chain, fill those up
			// with elements from the source back group. Then link the destination stack's groups to the source stack's groups.
			// If the source has more unused memory spaces in the back group than the destination, swap them before processing.
		
			if (source.total_number_of_elements == 0)
			{
				return;
			}
			else if (total_number_of_elements == 0)
			{
				#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
					*this = std::move(source);
				#else
					destroy_all_data();
					swap(source);
				#endif
	
				source.current_group = NULL;
				source.top_element = NULL;
				source.start_element = NULL;
				source.end_element = NULL;
				return;
			}


			// If there's more unused memory at the end of the last group in the destination than the source, swap:
			if ((end_element - top_element) > (source.end_element - source.top_element))
			{
				swap(source);
			}

			total_number_of_elements += source.total_number_of_elements;

			
			// Fill up the last group in the destination with pointers from the source:
			size_type elements_to_be_transferred = static_cast<size_type>(end_element - top_element++);
			size_type available_to_be_transferred = static_cast<size_type>((source.top_element - source.start_element) + 1); 
			
			while (elements_to_be_transferred >= available_to_be_transferred)
			{
				// Use the fastest method for moving iterators, while perserving values if allocator provides non-trivial pointers - unused if/else branches will be optimised out by any decent compiler:
				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (std::is_trivially_copyable<stack_element_type>::value && std::is_trivially_destructible<stack_element_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
					{
						std::memcpy(&*top_element, &*source.start_element, available_to_be_transferred * sizeof(stack_element_type));
					}
					#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
						else if (std::is_move_constructible<stack_element_type>::value)
						{
							std::uninitialized_copy(std::make_move_iterator(source.start_element), std::make_move_iterator(source.top_element + 1), top_element);
						}
					#endif
					else
				#endif
				{
					std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);

					// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
					for (stack_element_pointer_type element_pointer = source.start_element; element_pointer != source.top_element + 1; ++element_pointer)
					{
						PLF_COLONY_DESTROY(element_pointer_allocator_type, source, element_pointer);
					}
				}

				top_element += available_to_be_transferred;
				
				if (source.current_group == source.first_group)
				{
					--top_element;
					source.clear();
					return;
				}

				elements_to_be_transferred -= available_to_be_transferred;
				source.current_group = source.current_group->previous_group;
				source.start_element = source.current_group->elements;
				source.end_element = source.top_element = source.current_group->end;
				available_to_be_transferred = static_cast<size_type>((source.top_element - source.start_element) + 1);
			}

			
			if (elements_to_be_transferred != 0)
			{
				const stack_element_pointer_type start = source.top_element - (elements_to_be_transferred - 1);

				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (std::is_trivially_copyable<stack_element_type>::value && std::is_trivially_destructible<stack_element_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
					{
						std::memcpy(&*top_element, &*start, elements_to_be_transferred * sizeof(stack_element_type));
					}
					#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
						else if (std::is_move_constructible<stack_element_type>::value)
						{
							std::uninitialized_copy(std::make_move_iterator(start), std::make_move_iterator(source.top_element + 1), top_element);
						}
					#endif
					else
				#endif
				{
					std::uninitialized_copy(start, source.top_element + 1, top_element);

					// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
					for (stack_element_pointer_type element_pointer = start; element_pointer != source.top_element + 1; ++element_pointer)
					{
						PLF_COLONY_DESTROY(element_pointer_allocator_type, source, element_pointer);
					}
				}

				source.top_element = start - 1;
			}

			// Trim trailing groups on both, link source and destinations groups and remove references to source groups from source:
			source.trim_trailing_groups();
	        trim_trailing_groups();
	        

			current_group->next_group = source.first_group;
			source.first_group->previous_group = current_group;
			
			current_group = source.current_group;
			top_element = source.top_element;
			start_element = source.start_element;
			end_element = source.end_element;
			
			// Correct group sizes if necessary:
			if (source.group_allocator_pair.min_elements_per_group < group_allocator_pair.min_elements_per_group)
			{
				group_allocator_pair.min_elements_per_group = source.group_allocator_pair.min_elements_per_group;
			}	
			
			source.current_group = NULL;
			source.first_group = NULL;
			source.top_element = NULL;
			source.start_element = NULL;
			source.end_element = NULL;
			source.total_number_of_elements = 0;
		}	
	}; // end reduced_stack



	// Colony groups:
	struct group : private uchar_allocator_type	// Empty base class optimisation - inheriting allocator functions
	{
		element_pointer_type				last_endpoint; // the address that is one past the highest cell number that's been used so far in this group - does not change with erase command - is necessary because an iterator cannot access the colony's end_iterator - also used to determine whether erasures have occurred in the group by subtracting 'elements' and comparing with 'number_of_elements' - useful for some functions. Most-used variable in colony use (operator ++, --) so first in struct
		group_pointer_type					next_group;
		const element_pointer_type			elements;
		const skipfield_pointer_type		skipfield; // Now that both the elements and skipfield arrays are allocated contiguously, skipfield pointer also functions as a 'one-past-end' pointer for the elements array
		group_pointer_type					previous_group;
		size_type							group_number; // Used for comparison (> < >= <=) iterator operators (used by distance function and user)
		skipfield_type						number_of_elements; // indicates total number of used cells - changes with insert and erase commands - used to check for empty group in erase function, as indication to remove group
		const skipfield_type				size; // The number of elements this particular group can house



		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			group(const skipfield_type elements_per_group, group_pointer_type const previous = NULL):
				last_endpoint(reinterpret_cast<element_pointer_type>(PLF_COLONY_ALLOCATE_INITIALIZATION(uchar_allocator_type, ((elements_per_group * (sizeof(element_type))) + ((elements_per_group + 1) * sizeof(skipfield_type))), (previous == NULL) ? 0 : previous->elements))), /* allocating to here purely because it is first in the struct sequence - actual pointer is elements, last_endpoint is simply initialised to element's base value initially */
				next_group(NULL),
				elements(last_endpoint++),
				skipfield(reinterpret_cast<skipfield_pointer_type>(elements + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				number_of_elements(1),
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
				skipfield(reinterpret_cast<skipfield_pointer_type>(last_endpoint + elements_per_group)),
				previous_group(previous),
				group_number((previous == NULL) ? 0 : previous->group_number + 1),
				size(elements_per_group)
			{
				// Static casts to unsigned int from short not necessary as C++ automatically promotes lesser types for arithmetic purposes.
				std::memset(&*skipfield, 0, sizeof(skipfield_type) * (size + 1)); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
			}



			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source) PLF_COLONY_NOEXCEPT:
				uchar_allocator_type(source),
				last_endpoint(source.last_endpoint + 1),
				next_group(NULL),
				elements(source.last_endpoint),
				skipfield(source.skipfield),
				previous_group(source.previous_group),
				group_number(source.group_number),
				number_of_elements(1),
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
		element_pointer_type	element_pointer;
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
			
			

			inline colony_iterator & operator = (colony_iterator<!is_const> &&source) PLF_COLONY_NOEXCEPT // Move is a copy in this scenario
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



		inline PLF_COLONY_FORCE_INLINE reference operator * () const // may cause exception with uninitialized iterator
		{
			return *element_pointer;
		}



		inline PLF_COLONY_FORCE_INLINE pointer operator -> () const PLF_COLONY_NOEXCEPT
		{
			return element_pointer;
		}



#if defined(_MSC_VER) && _MSC_VER <= 1600 // MSVC 2010 needs a bit of a helping hand when it comes to optimizing
		inline PLF_COLONY_FORCE_INLINE colony_iterator & operator ++ ()
#else
		inline colony_iterator & operator ++ ()
#endif
		{
			assert(group_pointer != NULL); // covers uninitialised colony_iterator
			assert(!(element_pointer == group_pointer->last_endpoint && group_pointer->next_group != NULL)); // Assert that iterator is not already at end()

			#if (defined(__GNUC__) && !defined(__clang__)) && (defined(__haswell__) || defined(__skylake__) || defined(__silvermont__) || defined(__sandybridge__) || defined(__broadwell__)) // faster under gcc on core i processors post-westmere
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
	
				if ((element_pointer += skip + 1) == group_pointer->last_endpoint && group_pointer->next_group != NULL) // ie. beyond end of available data
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
				element_pointer -= skip + 1;

				if (element_pointer != group_pointer->elements - 1) // ie. iterator was not already at beginning of colony (with some previous consecutive deleted elements), and skipfield does not takes us into the previous group)
				{
					return *this;
				}
			}

			group_pointer = group_pointer->previous_group;
			skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
			const skipfield_type skip = *skipfield_pointer;
			element_pointer = (reinterpret_cast<colony::element_pointer_type>(group_pointer->skipfield) - 1) - skip;
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



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer == rh.element_pointer);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_iterator<!is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (element_pointer != rh.element_pointer);
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
		colony_iterator(const group_pointer_type group_p, const element_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: group_pointer(group_p), element_pointer(element_p), skipfield_pointer(skipfield_p) {}



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



		// In this case we have to redefine the algorithm, rather than using the internal iterator's -- operator, in order for the reverse_iterator to be able to reach rend() ie. elements[-1]
		colony_reverse_iterator & operator ++ ()
		{
			colony::group_pointer_type &group_pointer = the_iterator.group_pointer;
			colony::element_pointer_type &element_pointer = the_iterator.element_pointer;
			colony::skipfield_pointer_type &skipfield_pointer = the_iterator.skipfield_pointer;

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

			if (group_pointer->previous_group != NULL)
			{
				group_pointer = group_pointer->previous_group;
				skipfield_pointer = group_pointer->skipfield + group_pointer->size - 1;
				const skipfield_type skip = *skipfield_pointer;
				element_pointer = (reinterpret_cast<colony::element_pointer_type>(group_pointer->skipfield) - 1) - skip;
				skipfield_pointer -= skip;
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
			const colony_reverse_iterator copy(*this);
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
			const colony_reverse_iterator copy(*this);
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



		inline PLF_COLONY_FORCE_INLINE bool operator == (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator == rh.the_iterator);
		}



		inline PLF_COLONY_FORCE_INLINE bool operator != (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator != rh.the_iterator);
		}



		inline bool operator > (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (rh.the_iterator > the_iterator);
		}



		inline bool operator < (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return (the_iterator > rh.the_iterator);
		}



		inline bool operator >= (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(the_iterator > rh.the_iterator);
		}



		inline bool operator <= (const colony_reverse_iterator<!r_is_const> &rh) const PLF_COLONY_NOEXCEPT
		{
			return !(rh.the_iterator > the_iterator);
		}



		colony_reverse_iterator () PLF_COLONY_NOEXCEPT
		{}



		colony_reverse_iterator (const colony_reverse_iterator &source) PLF_COLONY_NOEXCEPT:
			the_iterator(source.the_iterator)
		{}



		colony_reverse_iterator (const typename colony::iterator &source) PLF_COLONY_NOEXCEPT:
			the_iterator(source)
		{}



	private:
		// Used by rend(), etc:
		colony_reverse_iterator(const group_pointer_type group_p, const element_pointer_type element_p, const skipfield_pointer_type skipfield_p) PLF_COLONY_NOEXCEPT: the_iterator(group_p, element_p, skipfield_p) {}



	public:

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

	// Used by range-insert and range-constructor to prevent fill-insert and fill-constructor function calls mistakenly resolving to the range insert/constructor
	template <bool condition, class T = void>
	struct plf_enable_if_c
	{
		typedef T type;
	};

	template <class T>
	struct plf_enable_if_c<false, T>
	{};


	iterator				end_iterator, begin_iterator;
	group_pointer_type		first_group;
	size_type				total_number_of_elements;
	skipfield_type 			min_elements_per_group;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		skipfield_type max_elements_per_group;
		explicit ebco_pair(const skipfield_type max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;

	reduced_stack erased_locations;


public:

	// Default constuctor:

	colony():
		element_allocator_type(element_allocator_type()),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group((sizeof(element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(element_type)) + 1),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max()),
		erased_locations((min_elements_per_group >> 7) + 8)
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed); // skipfield type must be of unsigned integer type (uchar, ushort, uint etc)
	}



	// Default constuctor (allocator-extended):

	explicit colony(const element_allocator_type &alloc):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group((sizeof(element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(element_type)) + 1),
		group_allocator_pair(std::numeric_limits<skipfield_type>::max()),
		erased_locations((min_elements_per_group >> 7) + 8)
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed); // skipfield type must be of unsigned integer type (uchar, ushort, uint etc)
	}



	// Copy constructor:

	colony(const colony &source):
		element_allocator_type(source),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(static_cast<skipfield_type>((source.min_elements_per_group > source.total_number_of_elements) ? source.min_elements_per_group : ((source.total_number_of_elements > source.group_allocator_pair.max_elements_per_group) ? source.group_allocator_pair.max_elements_per_group : source.total_number_of_elements))), // Make the first colony group size the greater of min_elements_per_group or total_number_of_elements, so long as total_number_of_elements isn't larger than max_elements_per_group
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
		erased_locations(static_cast<size_type>(source.erased_locations.group_allocator_pair.min_elements_per_group))
	{
		insert(source.begin_iterator, source.end_iterator);
		min_elements_per_group = source.min_elements_per_group; // reset to correct value for future clear() or erasures
	}



   	// Copy constructor (allocator-extended):

	colony(const colony &source, const allocator_type &alloc):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group(static_cast<skipfield_type>((source.min_elements_per_group > source.total_number_of_elements) ? source.min_elements_per_group : ((source.total_number_of_elements > source.group_allocator_pair.max_elements_per_group) ? source.group_allocator_pair.max_elements_per_group : source.total_number_of_elements))),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group),
		erased_locations(static_cast<size_type>(source.erased_locations.group_allocator_pair.min_elements_per_group))
	{
		insert(source.begin_iterator, source.end_iterator);
		min_elements_per_group = source.min_elements_per_group;
	}




	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move constructor:

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
		
		
		// Move constructor (allocator-extended):

		colony(colony &&source, const allocator_type &alloc):
			element_allocator_type(alloc),
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

	colony(const size_type fill_number, const element_type &element, const skipfield_type min_allocation_amount = 0, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		first_group(NULL),
		total_number_of_elements(0),
		min_elements_per_group((min_allocation_amount != 0) ? min_allocation_amount : 
			(fill_number > max_allocation_amount) ? max_allocation_amount : 
			(fill_number > 8) ? static_cast<skipfield_type>(fill_number) : 8),
		group_allocator_pair(max_allocation_amount),
		erased_locations((min_elements_per_group < 8) ? min_elements_per_group : (min_elements_per_group >> 7) + 8)
	{
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
		assert((min_elements_per_group > 2) & (min_elements_per_group <= group_allocator_pair.max_elements_per_group));

		insert(fill_number, element);
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
	 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
		assert((min_elements_per_group > 2) & (min_elements_per_group <= group_allocator_pair.max_elements_per_group));

		insert<iterator_type>(first, last);
	}



	// Initializer-list constructor:

	#ifdef PLF_COLONY_INITIALIZER_LIST_SUPPORT
		colony(const std::initializer_list<element_type> &element_list, const skipfield_type min_allocation_amount = 0, const skipfield_type max_allocation_amount = std::numeric_limits<skipfield_type>::max(), const element_allocator_type &alloc = element_allocator_type()):
			element_allocator_type(alloc),
			first_group(NULL),
			total_number_of_elements(0),
			min_elements_per_group((min_allocation_amount != 0) ? min_allocation_amount : 
			(element_list.size() > max_allocation_amount) ? max_allocation_amount : 
			(element_list.size() > 8) ? static_cast<skipfield_type>(element_list.size()) : 8),
			group_allocator_pair(max_allocation_amount),
			erased_locations((min_elements_per_group < 8) ? min_elements_per_group : (min_elements_per_group >> 7) + 8)
		{
		 	assert(std::numeric_limits<skipfield_type>::is_integer & !std::numeric_limits<skipfield_type>::is_signed);
			assert((min_elements_per_group > 2) & (min_elements_per_group <= group_allocator_pair.max_elements_per_group));

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



	inline const_iterator cbegin() const PLF_COLONY_NOEXCEPT
	{
		return const_iterator(begin_iterator.group_pointer, begin_iterator.element_pointer, begin_iterator.skipfield_pointer);
	}



	inline const_iterator cend() const PLF_COLONY_NOEXCEPT
	{
		return const_iterator(end_iterator.group_pointer, end_iterator.element_pointer, end_iterator.skipfield_pointer);
	}



	inline reverse_iterator rbegin() const // May throw if end_iterator is uninitialized ie. no elements inserted into colony yet
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



	~colony()
	{
		destroy_all_data();
	}



private:

	void destroy_all_data()
	{
	#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
		if (total_number_of_elements != 0 && !(std::is_trivially_destructible<element_type>::value))
	#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise the 'destruct' loop out for POD types
		if (total_number_of_elements != 0)
	#endif
		{
			total_number_of_elements = 0;
			element_pointer_type element_pointer = begin_iterator.element_pointer;
			skipfield_pointer_type skipfield_pointer = begin_iterator.skipfield_pointer;

			while (true)
			{
				const element_pointer_type end_pointer = first_group->last_endpoint;

				do
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), element_pointer);
					const skipfield_type skip = *(++skipfield_pointer);
					skipfield_pointer += skip;
					element_pointer += skip + 1;
				} while(element_pointer != end_pointer); // ie. beyond end of available data

				const group_pointer_type next_group = first_group->next_group;
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
     			first_group = next_group;

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

		begin_iterator.group_pointer = first_group;
		begin_iterator.element_pointer = first_group->elements;
		begin_iterator.skipfield_pointer = first_group->skipfield;
		end_iterator.group_pointer = first_group;
		end_iterator.element_pointer = first_group->elements;
		end_iterator.skipfield_pointer = first_group->skipfield;
	}



public:

	iterator insert(const element_type &element)
	{
		if (end_iterator.element_pointer != NULL)
		{
			switch(((erased_locations.total_number_of_elements != 0) << 1) | (end_iterator.element_pointer == reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)))
			{
				case 0: // ie. erased_locations is empty and end_iterator is not at end of current final group
				{
					const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
					PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, element);

					++end_iterator.element_pointer; // not postfix incrementing prev statement as it would necessitate a try-catch block to reverse increment if necessary (which would decrease speed by increasing code size)
					++end_iterator.skipfield_pointer;
					++(end_iterator.group_pointer->last_endpoint);
					++(end_iterator.group_pointer->number_of_elements);
					++total_number_of_elements;

					return return_iterator; // return value before incrementing
				}
				case 1:	// ie. erased_locations is empty and end_iterator is at end of current final group - ie. colony is full - create new group
				{
					end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
					group &next_group = *(end_iterator.group_pointer->next_group);

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
						PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					try
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), next_group.elements, element);
					}
					catch (...)
					{
						PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
						PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
						end_iterator.group_pointer->next_group = NULL;
						throw;
					}

					end_iterator.group_pointer = &next_group;
					end_iterator.element_pointer = next_group.last_endpoint;
					end_iterator.skipfield_pointer = next_group.skipfield + 1;
					++total_number_of_elements;

					return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield); /* returns value before incrementation */
				}
				default: // ie. erased_locations is not empty, reuse previous-erased element locations
				{
					iterator new_location;
					new_location.element_pointer = *erased_locations.top_element;
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

					// Code logic for next section:
					// ============================
					// check whether location we are restoring to has a skipfield node before or after which is erased
					// if it has only a node before which is erased (ie. at end of erasure block), update the prime erasure point
					// if it only has a node after it which is erased, (ie. this is the prime erasure point), change next node to prime erasure point and update all subsequent erasure points (ie. decrement by 1)
					// if it has both a node before and after which is erased (ie. is in middle of erasure block), do both of the above

					// Explanation of the following optimization: we must avoid testing the left-hand skipfield node if we are already at the beginning of the skipfield, otherwise we create an out-of-bounds memory access.
					// To avoid this would Normally require a branching test ie. !is_at_start && left-hand-node != 0 (&& and || operations are conditional executation of the right-hand instruction, which causes branching). But instead we subtract 'test' (which is 0 if the skipfield node is at start of skipfield, 1 if not) from the skipfield node.
					// If not start of skipfield, this means we check to see if left-hand node is == 0 (value * 0).
					// If at start of skipfield, we perform an unnecessary test to see if the current skipfield node's value (*(skipfield - 0))
					// is == it's own value (value * 1). This may seem ridiculous, but it's less costly than branching - and since
					// node == skipfield_start is almost always going to be false, this needless check is only occasional.
					const skipfield_type value = *(new_location.skipfield_pointer);
					const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
					const unsigned char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
					const unsigned char after_skipfield = *(new_location.skipfield_pointer + 1) != 0; // NOTE: first test removed due to extra unused node in skipfield (required by operator ++)

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
			initialize(min_elements_per_group);

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
			total_number_of_elements = 1;

			return begin_iterator; // returns value before incrementation
		}
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Note: the reason for code duplication from non-move insert, as opposed to using std::forward for both, was because most compilers didn't actually create as-optimal code in that strategy. Also, C++03 compliance...

		iterator insert(element_type &&element)
		{
			if (end_iterator.element_pointer != NULL)
			{
				switch(((erased_locations.total_number_of_elements != 0) << 1) | (end_iterator.element_pointer == reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)))
				{
					case 0:
					{
						const iterator return_iterator = end_iterator; /* Make copy for return before adjusting components */
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, std::move(element));

						++end_iterator.element_pointer;
						++end_iterator.skipfield_pointer;
						++end_iterator.group_pointer->last_endpoint;
						++end_iterator.group_pointer->number_of_elements;
						++total_number_of_elements;
						return return_iterator;
					}
					case 1:
					{
						end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
						group &next_group = *(end_iterator.group_pointer->next_group);

						try
						{
							#ifdef PLF_COLONY_VARIADICS_SUPPORT
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
							#else
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, group((total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer));
							#endif
						}
						catch (...)
						{
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						try
						{
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), next_group.elements, std::move(element));
						}
						catch (...)
						{
							PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
							PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
							end_iterator.group_pointer->next_group = NULL;
							throw;
						}

						end_iterator.group_pointer = &next_group;
						end_iterator.element_pointer = next_group.last_endpoint;
						end_iterator.skipfield_pointer = next_group.skipfield + 1;
						++total_number_of_elements;
	
						return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield); /* returns value before incrementation */
					}
					default:
					{
						iterator new_location;
						new_location.element_pointer = *erased_locations.top_element;
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
						{
							begin_iterator = new_location;
						}

						++total_number_of_elements;

						const skipfield_type value = *(new_location.skipfield_pointer);
						const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
						const unsigned char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
						const unsigned char after_skipfield = *(new_location.skipfield_pointer + 1) != 0;
	
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
				initialize(min_elements_per_group);

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
				total_number_of_elements = 1;

				return begin_iterator;
			}
		}



		#ifdef PLF_COLONY_VARIADICS_SUPPORT
			template<typename... Arguments>
			iterator emplace(Arguments&&... parameters)
			{
				if (end_iterator.element_pointer != NULL)
				{
					switch(((erased_locations.total_number_of_elements != 0) << 1) | (end_iterator.element_pointer == reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield)))
					{
						case 0:
						{
							const iterator return_iterator = end_iterator;
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer, std::forward<Arguments>(parameters)...);
	
							++end_iterator.element_pointer;
							++end_iterator.skipfield_pointer;
							++end_iterator.group_pointer->last_endpoint;
							++end_iterator.group_pointer->number_of_elements;
							++total_number_of_elements;
							return return_iterator;
						}
						case 1:
						{
							end_iterator.group_pointer->next_group = PLF_COLONY_ALLOCATE(group_allocator_type, group_allocator_pair, 1, end_iterator.group_pointer);
							group &next_group = *(end_iterator.group_pointer->next_group);
	
							try
							{
								PLF_COLONY_CONSTRUCT(group_allocator_type, group_allocator_pair, &next_group, (total_number_of_elements < static_cast<size_type>(group_allocator_pair.max_elements_per_group)) ? static_cast<const skipfield_type>(total_number_of_elements) : group_allocator_pair.max_elements_per_group, end_iterator.group_pointer);
							}
							catch (...)
							{
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
								end_iterator.group_pointer->next_group = NULL;
								throw;
							}
	
							try
							{
								PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), next_group.elements, std::forward<Arguments>(parameters)...);
							}
							catch (...)
							{
								PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, &next_group);
								PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, &next_group, 1);
								end_iterator.group_pointer->next_group = NULL;
								throw;
							}
		
							end_iterator.group_pointer = &next_group;
							end_iterator.element_pointer = next_group.last_endpoint;
							end_iterator.skipfield_pointer = next_group.skipfield + 1;
							++total_number_of_elements;
		
							return iterator(end_iterator.group_pointer, next_group.elements, next_group.skipfield);
						}
						default:
						{
							iterator new_location;
							new_location.element_pointer = *erased_locations.top_element;
							PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), new_location.element_pointer, std::forward<Arguments>(parameters)...);
							erased_locations.pop();
	
							new_location.group_pointer = end_iterator.group_pointer;
	
							while (new_location.element_pointer < new_location.group_pointer->elements || new_location.element_pointer >= reinterpret_cast<element_pointer_type>(new_location.group_pointer->skipfield))
							{
								new_location.group_pointer = new_location.group_pointer->previous_group;
							}
	
							new_location.skipfield_pointer = new_location.group_pointer->skipfield + (new_location.element_pointer - new_location.group_pointer->elements);
	
							++(new_location.group_pointer->number_of_elements);
	
							if (new_location.group_pointer == first_group && new_location.element_pointer < begin_iterator.element_pointer)
							{
								begin_iterator = new_location;
							}
	
							++total_number_of_elements;
	
							const skipfield_type value = *(new_location.skipfield_pointer);
							const bool test = (new_location.skipfield_pointer == new_location.group_pointer->skipfield);
							const unsigned char prev_skipfield = *(new_location.skipfield_pointer - !test) != value * test;
							const unsigned char after_skipfield = *(new_location.skipfield_pointer + 1) != 0;
	
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
					initialize(min_elements_per_group);
	
					try
					{
						PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, std::forward<Arguments>(parameters)...);
					}
					catch (...)
					{
						clear();
						throw;
					}
	
					++end_iterator.skipfield_pointer;
					total_number_of_elements = 1;
	
					return begin_iterator;
				}
			}
		#endif
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
	}



	void group_fill(const element_type &element, const skipfield_type number_of_elements)
	{
		const element_pointer_type fill_end = end_iterator.element_pointer + number_of_elements;

		do
		{
			try
			{
				PLF_COLONY_CONSTRUCT(element_allocator_type, (*this), end_iterator.element_pointer++, element);
			}
			catch (...)
			{
				end_iterator.group_pointer->last_endpoint = --end_iterator.element_pointer;
				const skipfield_type temp = static_cast<const skipfield_type>(end_iterator.element_pointer - end_iterator.group_pointer->elements);
				end_iterator.group_pointer->number_of_elements = temp;
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + temp;
				throw;
			}
		} while (end_iterator.element_pointer != fill_end);

		end_iterator.group_pointer->last_endpoint = end_iterator.element_pointer;
		end_iterator.group_pointer->number_of_elements = number_of_elements;
	}



public:

	// Fill insert

	iterator insert(const size_type number_of_elements, const element_type &element)
	{
		if (number_of_elements == 0)
		{
			return end_iterator;
		}

		if (first_group == NULL) // Empty colony, no groups created yet
		{
			if (number_of_elements > group_allocator_pair.max_elements_per_group)
			{
				// Create and fill first group:
				initialize(group_allocator_pair.max_elements_per_group); // Construct first group
				group_fill(element, group_allocator_pair.max_elements_per_group);

				// Create and fill all remaining groups:
				size_type multiples = (number_of_elements / static_cast<size_type>(group_allocator_pair.max_elements_per_group));
				const skipfield_type element_remainder = static_cast<const skipfield_type>(number_of_elements - (multiples * static_cast<size_type>(group_allocator_pair.max_elements_per_group)));

				while (--multiples != 0)
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
			else
			{
				initialize((number_of_elements < min_elements_per_group) ? min_elements_per_group : static_cast<skipfield_type>(number_of_elements)); // Construct first group
				group_fill(element, static_cast<skipfield_type>(number_of_elements));
			}

			end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->number_of_elements;
			total_number_of_elements += number_of_elements;
			return begin_iterator;
		}
		else
		{
			const iterator return_iterator = insert(element);
			size_type num_elements = number_of_elements - 1;
			size_type capacity_available = (reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer) + erased_locations.total_number_of_elements;

			// Use up erased locations and remainder of current group first:
			while (capacity_available-- != 0 && num_elements-- != 0)
			{
				insert(element);
			}

			// If still some left over, create new groups and fill:
			if (num_elements > group_allocator_pair.max_elements_per_group)
			{
				size_type multiples = (num_elements / static_cast<size_type>(group_allocator_pair.max_elements_per_group));
				const skipfield_type element_remainder = static_cast<const skipfield_type>(num_elements - (multiples * static_cast<size_type>(group_allocator_pair.max_elements_per_group)));

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
			else if (num_elements > total_number_of_elements)
			{
				group_create(static_cast<skipfield_type>(num_elements));
				group_fill(element, static_cast<skipfield_type>(num_elements));
			}

			end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->number_of_elements;
			total_number_of_elements += num_elements; // Adds the remainder - the insert functions above will already have incremented total_number_of_elements

			return return_iterator;
		}
	}



	// Range insert

	template <class iterator_type>
	iterator insert (const typename plf_enable_if_c<!std::numeric_limits<iterator_type>::is_integer, iterator_type>::type first, const iterator_type last)
	{
		if (first == last)
		{
			return end_iterator;
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
		inline iterator insert (const std::initializer_list<element_type> &element_list)
		{ // use range insert:
			return insert(element_list.begin(), element_list.end());
		}
	#endif



private:

	void consolidate_erased_locations(const group_pointer_type the_group_pointer)
	{
		// Function: Remove all entries in the stack containing memory locations from the supplied colony group, and incidentally remove unused trailing groups from the stack.

		// Process explanation:
		// First, remove any trailing unused groups from the reduced_stack. These may be present if the stack has pushed then popped a lot, as it never deallocates during pop, for performance reasons.
		// If there're no pointers to memory locations from the supplied colony group within stack groups, preserve those particular stack groups with no alterations in the new chain.
		// If there are pointers to memory locations from the supplied colony group in any stack group, copy these locations to the new stack group and remove the old group.
		// If a stack group is at the end of the old stack but is partial, and there have been some copies of memory locations made from old groups (see sentence immediately above this one), copy the locations from this group to the new group and remove it (otherwise you would end up with two partially-full groups at the end of the new chain - a new partially-full group following a partially-full old group). If there're no copies of memory locations (just old groups), then preserve the old end group.
		// At the end, if the new group has copies in it, add it to the new chain. Alternatively if the stack is now empty, reinitialize it. Or if there're no old groups remaining make the new group the first group. Or if there are only old groups, link the new chain (of old groups) into the stack.
		// Complicated but faster than any other alternative.

		typedef typename reduced_stack::stack_group_pointer_type stack_group_pointer;
		typedef typename reduced_stack::stack_element_pointer_type stack_element_pointer;

		#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
			typedef typename reduced_stack::stack_group_allocator_type stack_group_allocator_type; // Not used by PLF_COLONY_DESTROY etc when std::allocator_traits not supported
		#endif

		erased_locations.trim_trailing_groups();
		
		// Determine what the size of the first new group in erased_locations should be, based on the size of the first colony group:
		const size_type temp_size = (min_elements_per_group < 8) ? min_elements_per_group : (min_elements_per_group >> 7) + 8;

		// Note: All groups from here onwards refer to erased_location's stack groups, not colony groups, unless stated otherwise

		// Use either the size determined above or the current total number of elements in stack as an estimate of how large the first group in it should be:
		// The current total number of elements in the stack is a good estimate for how large it might become in future, but if it's smaller than the first_group determination, we should probably use the first_group determination.
		const size_type new_group_size = (erased_locations.total_number_of_elements < temp_size) ? temp_size : erased_locations.total_number_of_elements;

		stack_group_pointer current_old_group = erased_locations.first_group,
							new_group = PLF_COLONY_ALLOCATE(stack_group_allocator_type, erased_locations.group_allocator_pair, 1, erased_locations.current_group),
							first_new_chain = NULL,
							current_new_chain = NULL;

		try
		{
			#ifdef PLF_COLONY_VARIADICS_SUPPORT
				PLF_COLONY_CONSTRUCT(stack_group_allocator_type, erased_locations.group_allocator_pair, new_group, new_group_size, erased_locations.current_group);
			#else
				PLF_COLONY_CONSTRUCT(stack_group_allocator_type, erased_locations.group_allocator_pair, new_group, typename reduced_stack::group(new_group_size, erased_locations.current_group));
			#endif
		}
		catch (...)
		{
			PLF_COLONY_DEALLOCATE(stack_group_allocator_type, erased_locations.group_allocator_pair, new_group, 1);
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
				source_end = erased_locations.top_element + 1;
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
						if (std::is_trivially_copyable<element_pointer_type>::value && std::is_trivially_destructible<element_pointer_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
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

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_COLONY_DESTROY(element_pointer_allocator_type, erased_locations, element_pointer);
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
						if (std::is_trivially_copyable<element_pointer_type>::value && std::is_trivially_destructible<element_pointer_type>::value)
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

						// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
						for (stack_element_pointer element_pointer = iterator_pointer - number_to_be_copied; element_pointer != iterator_pointer; ++element_pointer)
						{
							PLF_COLONY_DESTROY(element_pointer_allocator_type, erased_locations, element_pointer);
						}
					}

					destination_begin += number_to_be_copied;
					total_number_of_copies += number_to_be_copied;
					number_to_be_copied = 0;
				}

				// Remove old group:
				const stack_group_pointer prev_group = current_old_group;
				current_old_group = current_old_group->next_group;
				PLF_COLONY_DESTROY(stack_group_allocator_type, erased_locations.group_allocator_pair, prev_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, erased_locations.group_allocator_pair, prev_group, 1);
			}
		} while (current_old_group != NULL);


		switch((current_new_chain != NULL) | ((destination_begin == destination_start) << 1))
		{
			case 0: // if (current_new_chain == NULL && destination_begin != destination_start) - No old groups remaining, some copies
			{
				new_group->previous_group = NULL;
				erased_locations.first_group = erased_locations.current_group = new_group;
				erased_locations.top_element = destination_begin - 1;
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
				erased_locations.top_element = destination_begin - 1;
				erased_locations.start_element = new_group->elements;
				erased_locations.end_element = new_group->end;
				erased_locations.total_number_of_elements = total_number_of_copies;
				break;
			}
			case 3: // else if (current_new_chain != NULL && destination_begin == destination_start) - No copies, some old groups - destination_begin == destination_start implied by previous if's
			{
				PLF_COLONY_DESTROY(stack_group_allocator_type, erased_locations.group_allocator_pair, new_group);
				PLF_COLONY_DEALLOCATE(stack_group_allocator_type, erased_locations.group_allocator_pair, new_group, 1);

				current_new_chain->next_group = NULL;
				erased_locations.current_group = current_new_chain;
				erased_locations.first_group = first_new_chain;
				erased_locations.top_element = the_end;
				erased_locations.start_element = current_new_chain->elements;
				erased_locations.end_element = current_new_chain->end;
				erased_locations.total_number_of_elements = total_number_of_copies;
				break;
			}
			default: // else (current_new_chain == NULL && destination_begin == destination_start) - No elements remaining - least likely
			{
				// Instead of removing the newly-constructed group and returning to zero,
				// we bank on the idea that if an element has been removed, more elements are likely to be removed later, and so keep the group rather than deleting it.

				erased_locations.current_group = new_group;
				erased_locations.first_group = new_group;
				erased_locations.top_element = new_group->elements - 1;
				erased_locations.start_element = new_group->elements;
				erased_locations.end_element = new_group->end;
				erased_locations.total_number_of_elements = 0;
				break;
			}
		}
	}



	inline PLF_COLONY_FORCE_INLINE void update_subsequent_group_numbers(group_pointer_type the_group) PLF_COLONY_NOEXCEPT
	{
		do
		{
			--(the_group->group_number);
			the_group = the_group->next_group;
		} while (the_group != NULL);
	}



	inline PLF_COLONY_FORCE_INLINE void consolidate() // get all elements contigious in memory and shrink to fit, remove erased locations
	{
		colony temp(*this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif
	}


public:

	// must return iterator in case the group which the iterator is within becomes empty after the erasure and is thereby removed from the colony chain:
	iterator erase(const const_iterator &the_iterator)
	{
		assert(!empty());
		const group_pointer_type the_group_pointer = the_iterator.group_pointer;
		assert(the_group_pointer != NULL); // ie. not uninitialized iterator
		assert(the_iterator.element_pointer != the_group_pointer->last_endpoint); // ie. not == end()
		assert(*(the_iterator.skipfield_pointer) == 0); // ie. element pointed to by iterator has not been erased previously

		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (!(std::is_trivially_destructible<element_type>::value)) // This if-statement should be removed by the compiler on resolution of element_type. For some optimizing compilers this step won't be necessary (for MSVC 2013 it makes a difference)
		#endif
		{
			PLF_COLONY_DESTROY(element_allocator_type, (*this), the_iterator.element_pointer); // Destruct element
		}

		--total_number_of_elements;

		if (the_group_pointer->number_of_elements-- != 1) // ie. non-empty group at this point in time, don't consolidate - optimization note: GCC optimizes postfix + 1 comparison better than prefix + 1 comparison in many cases.
		{
			erased_locations.push(the_iterator.element_pointer);

			// Code logic for following section:
			// ---------------------------------
			// If current skipfield node has no erased node on either side, continue as usual
			// If node has erased node before it, add 1 to prev node value and set current node and start node of the skipblock to this.
			// If node has erased node after it but none before it, make this node the start node of the skipblock and update subsequent nodes
			// If node has erased nodes before and after it, set current node to left node + 1, then update all nodes after  the current node - effectively removing start node of the right-hand skipblock

			iterator return_iterator;

			// Optimization explanation:
			// The contextual logic below is the same as that in the insert() functions but in this case the value of the current skipfield node will always be
			// zero (since it is not yet erased), meaning no additional manipulations are necessary for the previous skipfield node comparison - we only have to check against zero
			const unsigned char prev_skipfield = *(the_iterator.skipfield_pointer - (the_iterator.skipfield_pointer != the_group_pointer->skipfield)) != 0;
			const unsigned char after_skipfield = *(the_iterator.skipfield_pointer + 1) != 0;  // NOTE: boundary test (checking against end-of-elements) is able to be skipped due to the extra skipfield node (compared to element field) - which is present to enable faster iterator operator ++ operations


			switch (prev_skipfield | (after_skipfield << 1))
			{
				case 0: // no consecutive erased elements
				{
					*the_iterator.skipfield_pointer = 1; // solo erase point

					return_iterator.group_pointer = the_group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + 1;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 1: // previous erased consecutive elements, none following
				{
					*the_iterator.skipfield_pointer = *(the_iterator.skipfield_pointer - 1) + 1;
					++(*(the_iterator.skipfield_pointer - *(the_iterator.skipfield_pointer - 1)));

					return_iterator.group_pointer = the_group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + 1;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + 1;
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 2: // following erased consecutive elements, none preceding
				{
					const skipfield_type update_count = *(the_iterator.skipfield_pointer + 1);
					std::memmove(&*(the_iterator.skipfield_pointer + 1), &*(the_iterator.skipfield_pointer + 2), sizeof(skipfield_type) * (update_count - 1));
					*(the_iterator.skipfield_pointer + update_count) = *(the_iterator.skipfield_pointer) = update_count + 1;

					return_iterator.group_pointer = the_group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + *(the_iterator.skipfield_pointer);
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + *(the_iterator.skipfield_pointer);
					return_iterator.check_for_end_of_group_and_progress();
					break;
				}
				case 3: // both preceding and following consecutive erased elements
				{
					skipfield_pointer_type following = the_iterator.skipfield_pointer - 1;
					skipfield_type update_value = *following;
					skipfield_type update_count = *(following + 2) + 1;
					*(the_iterator.skipfield_pointer - update_value) += update_count;

					return_iterator.group_pointer = the_group_pointer;
					return_iterator.element_pointer = the_iterator.element_pointer + update_count;
					return_iterator.skipfield_pointer = the_iterator.skipfield_pointer + update_count;
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


			if (the_iterator == begin_iterator) // If original iterator was first element in colony, update it's value with the next non-skipfield element:
			{
				begin_iterator = return_iterator;
			}

			return return_iterator;
		}

		// else: consolidation of empty groups
		switch((the_group_pointer->next_group != NULL) | ((the_group_pointer != first_group) << 1))
		{
			case 0: // ie. the_group_pointer == first_group && the_group_pointer->next_group == NULL; only group in colony
			{
				// Reset skipfield and erased_locations rather than clearing - leads to fewer allocations/deallocations:
				std::memset(&*(the_group_pointer->skipfield), 0, sizeof(skipfield_type) * the_group_pointer->size); // &* to avoid problems with non-trivial pointers - size + 1 to allow for computationally-faster operator ++ and other operations - extra field is unused but checked - not having it will result in out-of-bounds checks
				erased_locations.clear();

				// Reset begin_iterator:
				end_iterator.element_pointer = begin_iterator.element_pointer = the_group_pointer->last_endpoint = the_group_pointer->elements;
				end_iterator.skipfield_pointer = begin_iterator.skipfield_pointer = the_group_pointer->skipfield;

				return end_iterator;
			}
			case 1: // ie. the_group_pointer == first_group && the_group_pointer->next_group != NULL. Remove first group, change first group to next group
			{
				the_group_pointer->next_group->previous_group = NULL; // Cut off this group from the chain
				first_group = the_group_pointer->next_group; // Make the next group the first group

				// Update group numbers:
				update_subsequent_group_numbers(first_group);
				consolidate_erased_locations(the_group_pointer); // There must be erased_locations for an empty non-final group, no emptiness check is necessary

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				begin_iterator.group_pointer = first_group; // note: end iterator only needs to be changed if the deleted group was the final group in the chain ie. not in this case
				begin_iterator.element_pointer = first_group->elements + *(first_group->skipfield); // If the beginning index has been erased (ie. skipfield != 0), skip to next non-erased element
				begin_iterator.skipfield_pointer = first_group->skipfield + *(first_group->skipfield);

				return begin_iterator;
			}
			case 3: // this is a non-first group but not final group in chain: the group is completely empty of elements, so delete the group, then link previous group's next-group field to the next non-empty group in the series, removing this link in the chain:
			{
				the_group_pointer->next_group->previous_group = the_group_pointer->previous_group;
				const group_pointer_type return_group = the_group_pointer->previous_group->next_group = the_group_pointer->next_group; // close the chain, removing this group from it

				// Update group numbers:
				update_subsequent_group_numbers(return_group);

				consolidate_erased_locations(the_group_pointer); // There must be erased_locations for an empty non-final group, no emptiness check is necessary

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				// If first element of next group is erased (ie. skipfield != 0), skip to the next non-erased element:
				return iterator(return_group, return_group->elements + *(return_group->skipfield), return_group->skipfield + *(return_group->skipfield));
			}
			default: // this is a non-first group and the final group in the chain: the group is completely empty of elements
			{
				if (!((the_iterator.element_pointer == the_group_pointer->elements) & (the_iterator.element_pointer + 1 == end_iterator.element_pointer))) // If the element pointer is not right at the start of the group and the end_iterator right afterwards, that means there must be some erased element locations that've been stored. This check is necessary because the consolidate function doesn't check if erased_locations is empty
				{
					consolidate_erased_locations(the_group_pointer);
				}

				the_group_pointer->previous_group->next_group = NULL;
				end_iterator.group_pointer = the_group_pointer->previous_group; // end iterator only needs to be changed if this is the final group in the chain
				end_iterator.element_pointer = reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield);
				end_iterator.skipfield_pointer = end_iterator.group_pointer->skipfield + end_iterator.group_pointer->size;

				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, the_group_pointer);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group_pointer, 1);

				return end_iterator;
			}
		}
	}



	void erase(const const_iterator &iterator1, const const_iterator &iterator2)
	{
		assert(iterator1 != iterator2);
		assert(iterator1 < iterator2);

		iterator current = iterator1;

		if (current.group_pointer != iterator2.group_pointer)
		{
			if (current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // if iterator1 is not the first non-erased element in it's group - most common case
			{
				const skipfield_pointer_type end = iterator1.group_pointer->skipfield + iterator1.group_pointer->size;
				skipfield_type number_of_group_erasures = 0;

				// Destroy elements first:
				do
				{
					#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
						if (!(std::is_trivially_destructible<element_type>::value))
					#endif
					{
						PLF_COLONY_DESTROY(element_allocator_type, (*this), current.element_pointer); // Destruct element
					}

        			erased_locations.push(current.element_pointer);

					const skipfield_type skip = *(++current.skipfield_pointer);
					current.skipfield_pointer += skip;
					current.element_pointer += skip + 1;
					++number_of_group_erasures;
				} while (current.skipfield_pointer != end);

				current.group_pointer->number_of_elements -= number_of_group_erasures;
				total_number_of_elements -= number_of_group_erasures;

				// Now update skipfield:
				skipfield_pointer_type current_skipfield = iterator1.skipfield_pointer;
				skipfield_type node_value = *(current_skipfield - 1); // Find value of left-hand node
				skipfield_type update_count = static_cast<skipfield_type>(end - current_skipfield);
				
				*(current_skipfield - node_value) = node_value + update_count; // Either set current node as the start node, or if previous node is part of a skipblock, update that skipblock's start node and join to that skipblock

				if (node_value == 0)
				{
					node_value = 1;
					++current_skipfield;
					--update_count;
				}

				// Vectorize the updates:
				while (update_count >= 4)
				{
					*(current_skipfield) = node_value + 1;
					*(current_skipfield + 1) = node_value + 2;
					*(current_skipfield + 2) = node_value + 3;
					*(current_skipfield + 3) = node_value + 4;

					current_skipfield += 4;
					node_value += 4;
					update_count -= 4;
				}

				while (current_skipfield != end)
				{
					*(current_skipfield++) = ++node_value;
				}

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
					const element_pointer_type end = current.group_pointer->last_endpoint;

					do
					{
						PLF_COLONY_DESTROY(element_allocator_type, (*this), current.element_pointer); // Destruct element
						const skipfield_type skip = *(++current.skipfield_pointer);
						current.skipfield_pointer += skip;
						current.element_pointer += skip + 1;
					} while (current.element_pointer != end);
				}

				if (current.group_pointer->number_of_elements != static_cast<skipfield_type>(current.group_pointer->last_endpoint - current.group_pointer->elements)) // no erasures
				{
					consolidate_erased_locations(current.group_pointer);
				}

				total_number_of_elements -= current.group_pointer->number_of_elements;
				current_group = current.group_pointer;
				current.group_pointer = current.group_pointer->next_group;

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

			// If iterator2 is right at the start of the final group (if so, there is nothing more to be erased), return:
			if (iterator2.group_pointer->elements + *(iterator2.group_pointer->skipfield) == iterator2.element_pointer)
			{
				return;
			}
		}


		// Final group:
		// Code explanation:
		// If not erasing entire final group, 1. Destruct elements (if non-trivial destructor) and add locations to erased stack. 2. process skipfield.
		// If erasing entire group, 1. Destruct elements (if non-trivial destructor), 2. if no elements left in colony, clear() 3. otherwise reset end_iterator and consolidate_erased_locations if there were some prior erasures in group


		if (iterator2.element_pointer != current.group_pointer->last_endpoint || current.element_pointer != current.group_pointer->elements + *(current.group_pointer->skipfield)) // ie. not erasing entire group
		{
			skipfield_type number_of_group_erasures = 0;

			element_pointer_type current_element = current.element_pointer;
			skipfield_pointer_type current_skipfield = current.skipfield_pointer;

			do
			{
				#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
					if (!(std::is_trivially_destructible<element_type>::value)) // This should be removed by the compiler
				#endif
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), current_element);
				}

				erased_locations.push(current_element);

				++current_skipfield;
				current_element += 1 + *current_skipfield;
				current_skipfield += *current_skipfield;
				++number_of_group_erasures;
			} while(current_element != iterator2.element_pointer);

			total_number_of_elements -= number_of_group_erasures;
			current.group_pointer->number_of_elements -= number_of_group_erasures;

			// Update skipfield:
			skipfield_type node_value = *(current.skipfield_pointer - (current.group_pointer->skipfield != current.skipfield_pointer)); // Find value of left-hand node - if current node is at start of skipfield, we check the current node instead, which will always be zero.
			skipfield_type update_count = static_cast<skipfield_type>(iterator2.element_pointer - current.element_pointer);

			*(current.skipfield_pointer - node_value) = node_value + update_count; // Either set current node as the start node, or if previous node is part of a skipblock, update that skipblock's start node

			if (node_value == 0)
			{
				node_value = 1;
				++current.skipfield_pointer;
				--update_count;
			}

			// Vectorize the updates:
			while (update_count >= 4)
			{
				*(current.skipfield_pointer) = node_value + 1;
				*(current.skipfield_pointer + 1) = node_value + 2;
				*(current.skipfield_pointer + 2) = node_value + 3;
				*(current.skipfield_pointer + 3) = node_value + 4;

				current.skipfield_pointer += 4;
				node_value += 4;
				update_count -= 4;
			}

			while (current.skipfield_pointer != iterator2.skipfield_pointer)
			{
				*(current.skipfield_pointer++) = ++node_value;
			}

			if (iterator1.element_pointer == begin_iterator.element_pointer) // Retaining this check for the case when iterator1 and iterator2 have the same group
			{
				begin_iterator = iterator2;
			}
		}
		else // ie. full group erasure
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (!(std::is_trivially_destructible<element_type>::value)) // This should be removed by the compiler
			#endif
			{
				while(current.element_pointer != iterator2.element_pointer)
				{
					PLF_COLONY_DESTROY(element_allocator_type, (*this), current.element_pointer);
					const skipfield_type skip = *(++current.skipfield_pointer);
					current.skipfield_pointer += skip;
					current.element_pointer += skip + 1;
				}
			}

			// Note: it is not possible that next_group != NULL at this point (if it were, iterator2.element_pointer could not be == last_endpoint - which indicates that it is == end())

			if ((total_number_of_elements -= current.group_pointer->number_of_elements) != 0) // ie. previous_group != NULL
			{
				current.group_pointer->previous_group->next_group = NULL;
				end_iterator.group_pointer = current.group_pointer->previous_group;
				end_iterator.element_pointer = current.group_pointer->previous_group->last_endpoint;
				end_iterator.skipfield_pointer = current.group_pointer->previous_group->skipfield + current.group_pointer->previous_group->size;

				if (current.group_pointer->number_of_elements != static_cast<skipfield_type>(current.group_pointer->last_endpoint - current.group_pointer->elements)) // ie. some prior erasures exist in group
				{
					consolidate_erased_locations(current.group_pointer);
				}
			}
			else // ie. colony is now empty
			{
				first_group = NULL;
				clear();
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
		return (first_group == NULL) ? 0 : (total_number_of_elements + static_cast<size_type>(erased_locations.total_number_of_elements) +
			static_cast<size_type>(reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.element_pointer));
	}



	inline size_type approximate_memory_use() const
	{
		return static_cast<size_type>(
			sizeof(*this) + // sizeof colony basic structure
			(erased_locations.approximate_memory_use()) +  // sizeof erased_locations stack (stack structure sizeof included in colony basic structure sizeof so negated from result)
			(capacity() * (sizeof(value_type) + sizeof(skipfield_type))) + // sizeof current colony data capacity + skipfields
			((end_iterator.group_pointer == NULL) ? 0 : ((end_iterator.group_pointer->group_number + 1) * (sizeof(group) + sizeof(skipfield_type))))); // if colony not empty, add the memory usage of the group structures themselves, adding the extra skipfield entry
	}



	void change_group_sizes(const skipfield_type min_allocation_amount, const skipfield_type max_allocation_amount)
	{
		assert((min_allocation_amount > 2) & (min_allocation_amount <= max_allocation_amount));

		min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;

		// The following will only have an effect if the erased_locations becomes empty or is already empty
		erased_locations.group_allocator_pair.min_elements_per_group = ((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8);

		if (first_group != NULL && (first_group->size < min_allocation_amount || end_iterator.group_pointer->size > max_allocation_amount))
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
		change_group_sizes(min_elements_per_group, max_allocation_amount);
	}



	inline void get_group_sizes(skipfield_type &minimum_group_size, skipfield_type &maximum_group_size) const PLF_COLONY_NOEXCEPT
	{
		minimum_group_size = min_elements_per_group;
		maximum_group_size = group_allocator_pair.max_elements_per_group;		
	}	



	inline void reinitialize(const skipfield_type min_allocation_amount, const skipfield_type max_allocation_amount) PLF_COLONY_NOEXCEPT
	{
		assert((min_allocation_amount > 2) & (min_allocation_amount <= max_allocation_amount));

		min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;

		clear();
		erased_locations.group_allocator_pair.min_elements_per_group = ((min_allocation_amount < 8) ? min_allocation_amount : (min_allocation_amount >> 7) + 8);
	}



	void clear()
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

		clear();
		colony temp(source);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		colony & operator = (colony &&source)
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



	void shrink_to_fit() // uninitialized list or full
	{
		if ((first_group == NULL) | (total_number_of_elements == capacity()))
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



	void reserve(skipfield_type reserve_amount)
	{
		assert(reserve_amount > 2);

		if (reserve_amount > group_allocator_pair.max_elements_per_group)
		{
			reserve_amount = group_allocator_pair.max_elements_per_group;
		}
		else if (reserve_amount < min_elements_per_group)
		{
			reserve_amount = min_elements_per_group;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = static_cast<skipfield_type>(max_size());
		}

		if (total_number_of_elements == 0) // Most common scenario
		{
			if (first_group != NULL) // Edge case - empty colony but first group is initialized ie. did have some insertions but all got erased
			{
				PLF_COLONY_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_COLONY_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			} // else: Empty colony, no inserts as yet, time to allocate

			initialize(reserve_amount);
			first_group->last_endpoint = first_group->elements; // elements + 1 by default
			first_group->number_of_elements = 0; // 1 by default
		}
		else if (reserve_amount <= capacity())
		{
			return;
		}
		else
		{
			const skipfield_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;
			consolidate();
			min_elements_per_group = original_min_elements;
		}
	}



	void swap(colony &source) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		assert(&source != this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			colony temp(std::move(source));
			source = std::move(*this);
			*this = std::move(temp);
		#else
			const iterator					swap_end_iterator = end_iterator, swap_begin_iterator = begin_iterator;
			const group_pointer_type		swap_first_group = first_group;
			const size_type					swap_total_number_of_elements = total_number_of_elements;
			const skipfield_type 			swap_min_elements_per_group = min_elements_per_group, swap_max_elements_per_group = group_allocator_pair.max_elements_per_group;

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



	// Advance implementation for iterator and const_iterator:
	template <bool is_const>
	void advance(colony_iterator<is_const> &it, difference_type distance) const
	{
		// For code simplicity - should hopefully be optimized out by compiler:
		group_pointer_type &group_pointer = it.group_pointer;
		element_pointer_type &element_pointer = it.element_pointer;
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
				if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->size)
				{
					const difference_type distance_from_end = static_cast<const difference_type>(group_pointer->last_endpoint - element_pointer);

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
			if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // No erasures in this group, use straight pointer addition
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
				if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // ie. no prior erasures have occurred in this group
				{
					const difference_type distance_from_beginning = static_cast<const difference_type>(element_pointer - group_pointer->elements);

					if (distance <= distance_from_beginning)
					{
						element_pointer -= distance;
						skipfield_pointer -= distance;
						return;
					}
					else if (group_pointer->previous_group == NULL) // ie. we've gone beyond begin(), so bound to begin()
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
			else if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // ie. no erased elements in this group
			{
				element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield) - distance;
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




	// Advance for reverse_iterator and const_reverse_iterator:
	template <bool is_const>
	void advance(colony_reverse_iterator<is_const> &it, difference_type distance) const
	{
		group_pointer_type &group_pointer = it.the_iterator.group_pointer;
		element_pointer_type &element_pointer = it.the_iterator.element_pointer;
		skipfield_pointer_type &skipfield_pointer = it.the_iterator.skipfield_pointer;

		assert(element_pointer != NULL);

        // this needs to be implemented slightly differently to forward-iterator's advance, as it needs to be able to reach rend() ie. begin() - 1 and to be bound to rbegin()

		if (distance > 0) 
		{
			assert (!(element_pointer == group_pointer->elements - 1 && group_pointer->previous_group == NULL)); // Check that we're not already at rend()
			// Special case for initial element pointer and initial group (we don't know how far into the group the element pointer is)
			// Since a reverse_iterator cannot == last_endpoint (ie. before rbegin()) we don't need to check for that like with iterator
			if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements))
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
			else if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements))
			{
				element_pointer = reinterpret_cast<element_pointer_type>(group_pointer->skipfield) - distance;
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
				if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // ie. if there are no erasures in the group (using endpoint - elements_start to determine number of elements in group just in case this is the last group of the colony, in which case group->last_endpoint != group->elements + group->size)
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
						element_pointer = group_pointer->last_endpoint - 1; // no erasures so don't have to subtract skipfield value as we do below
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


			// Intermediary groups - at the start of this code block and the subsequent block, the position of the iterator is assumed to be the first non-erased element in the current group:
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
			if (group_pointer->number_of_elements == static_cast<skipfield_type>(group_pointer->last_endpoint - group_pointer->elements)) // No erasures in this group, use straight pointer addition
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
		// If not in the same group, process the intermediate groups and add distances,
		// skipping manual incrementation in all but the initial and final groups.
		// In the initial and final groups, manual incrementation may be skipped if there are no prior erasures in those groups.
		// In such a case, simple subtraction of pointer values is possible to calculate the distances between current location
		// and the end of the group's element memory block.

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
			if (iterator1.group_pointer->number_of_elements == static_cast<skipfield_type>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // If no prior erasures have occured in this group we can do simple addition
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


		if (iterator1.group_pointer->number_of_elements == static_cast<skipfield_type>(iterator1.group_pointer->last_endpoint - iterator1.group_pointer->elements)) // ie. no deletions in this group, direct subtraction is possible
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
		return distance(last.the_iterator, first.the_iterator);
	}




	// Type-changing functions:

	iterator get_iterator_from_pointer(const element_pointer_type the_pointer) const PLF_COLONY_NOEXCEPT
	{
		assert(!empty());
		
		group_pointer_type the_group = end_iterator.group_pointer; // Start with last group first, as will be the largest group

		while (the_group != NULL)
		{
			if (the_pointer >= the_group->elements && the_pointer < reinterpret_cast<element_pointer_type>(the_group->skipfield))
			{
				const skipfield_pointer_type the_skipfield = the_group->skipfield + (the_pointer - the_group->elements);
				return (*the_skipfield == 0) ? iterator(the_group, the_pointer, the_skipfield) : end_iterator; // If element has been erased, return end()
			}

			the_group = the_group->previous_group;
		}

		return end_iterator;
	}



	template <bool is_const>
	size_type get_index_from_iterator(const colony_iterator<is_const> &the_iterator) const
	{
		assert(!empty());
		
		// This is essentially, a simplified version of distance() optimized for counting from begin()
		size_type index = 0;
		group_pointer_type group_pointer = first_group;

		// For all prior groups, add group sizes
		while (group_pointer != the_iterator.group_pointer)
		{
			index += static_cast<size_type>(group_pointer->number_of_elements);
			group_pointer = group_pointer->next_group;
		}

		if (group_pointer->last_endpoint - group_pointer->elements == group_pointer->number_of_elements)
		{
			index += static_cast<size_type>(the_iterator.element_pointer - group_pointer->elements); // If no erased elements in group exist, do straight pointer arithmetic to get distance to start for first element
		}
		else // Otherwise do manual ++ loop - count from beginning of group until reach location
		{
			skipfield_pointer_type skipfield_pointer = group_pointer->skipfield + *(group_pointer->skipfield);

			while(skipfield_pointer != the_iterator.skipfield_pointer)
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
		assert(!empty());
		return get_index_from_iterator(rev_iterator.the_iterator);
	}



	inline iterator get_iterator_from_index(size_type index) const
	{
		assert(!empty());
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
		bool operator() (const element_type &a, const element_type &b) const 
		{
			return a < b;
		}
	};


	// To redirect the sort function to compare by ->element, but sort the node pointers instead of the elements
	template <class comparison_function>
	struct sort_dereferencer
	{
		comparison_function stored_instance;

		explicit sort_dereferencer(const comparison_function &function_instance):
			stored_instance(function_instance)
		{}
		
		sort_dereferencer()
		{}

		bool operator() (const element_pointer_type first, const element_pointer_type second) const
		{
			return stored_instance(*first, *second);
		}
	};


public:


	inline void sort()
	{
		sort(less());
	}



	template <class comparison_function>
	void sort(comparison_function compare)
	{
		if (total_number_of_elements < 2)
		{
			return;
		}
		
		element_pointer_type * const element_pointers = PLF_COLONY_ALLOCATE(element_pointer_allocator_type, erased_locations, total_number_of_elements, NULL);
		element_pointer_type *element_pointer = element_pointers;
		
		try
		{
			for (iterator current_element = begin_iterator; current_element != end_iterator; ++current_element)
			{
				PLF_COLONY_CONSTRUCT(element_pointer_allocator_type, erased_locations, element_pointer++, &*current_element);
			}	
		
			#ifdef PLF_TIMSORT_AVAILABLE
				plf::timsort(element_pointers, element_pointers + total_number_of_elements, sort_dereferencer<comparison_function>(compare));
			#else
				std::sort(element_pointers, element_pointers + total_number_of_elements, sort_dereferencer<comparison_function>(compare));
			#endif
	
			
			colony new_location;
			new_location.change_group_sizes(min_elements_per_group, group_allocator_pair.max_elements_per_group);
			new_location.reserve(static_cast<skipfield_type>((total_number_of_elements > std::numeric_limits<skipfield_type>::max()) ? std::numeric_limits<skipfield_type>::max() : total_number_of_elements));
			
			#if defined(PLF_COLONY_TYPE_TRAITS_SUPPORT) && defined(PLF_COLONY_MOVE_SEMANTICS_SUPPORT)
				if (std::is_move_assignable<element_type>::value)
				{
					for (element_pointer_type *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
					{
						new_location.insert(std::move(*(*current_element_pointer)));
					}
				}
				else
			#endif
			{
				for (element_pointer_type *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
				{
					new_location.insert(*(*current_element_pointer));
				}
			}

			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(new_location); // avoid generating temporary
			#else
				swap(new_location);
			#endif
		}
		catch (...)
		{
			#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
				if (!std::is_trivially_destructible<element_pointer_type>::value)
			#endif
			{
				for (element_pointer_type *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
				{
					PLF_COLONY_DESTROY(element_pointer_allocator_type, erased_locations, current_element_pointer);
				}
			}

			PLF_COLONY_DEALLOCATE(element_pointer_allocator_type, erased_locations, element_pointers, total_number_of_elements);
			throw;
		}


		#ifdef PLF_COLONY_TYPE_TRAITS_SUPPORT
			if (!std::is_trivially_destructible<element_pointer_type>::value)
		#endif
		{
			for (element_pointer_type *current_element_pointer = element_pointers; current_element_pointer != element_pointer; ++current_element_pointer)
			{
				PLF_COLONY_DESTROY(element_pointer_allocator_type, erased_locations, current_element_pointer);
			}
		}

		PLF_COLONY_DEALLOCATE(element_pointer_allocator_type, erased_locations, element_pointers, total_number_of_elements);
	}




	void splice(colony &source) PLF_COLONY_NOEXCEPT_SWAP(allocator_type)
	{
		// Process: if there are unused memory spaces at the end of the last current back group of the chain, convert them
		// to skipped elements and add the locations to the erased_locations stack. 
		// Then link the destination stack's groups to the source stack's groups and nullify the source.
		// If the source has more unused memory spaces in the back group than the destination, swap them before processing to reduce stack usage and iteration skipping.

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
				swap(source);
			#endif
			
			source.clear();

			return;
		}

		// If there's more unused element indexes at end of destination, swap with destination to reduce number of skipped elements and reduce content of erased_locations:
		if ((reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.group_pointer->last_endpoint) > (reinterpret_cast<element_pointer_type>(source.end_iterator.group_pointer->skipfield) - source.end_iterator.group_pointer->last_endpoint))
		{
			swap(source);
		}

		
		// Correct group sizes if necessary:
		if (source.min_elements_per_group < min_elements_per_group)
		{
			min_elements_per_group = source.min_elements_per_group;
		}	

		if (source.group_allocator_pair.max_elements_per_group > group_allocator_pair.max_elements_per_group)
		{
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
		}	

		// Add source erased_locations to destination:
		erased_locations.splice(source.erased_locations);
		
		const skipfield_type distance_to_end = static_cast<skipfield_type>(reinterpret_cast<element_pointer_type>(end_iterator.group_pointer->skipfield) - end_iterator.group_pointer->last_endpoint);
		
		if (distance_to_end != 0) // 0 == edge case
		{   // Mark unused element indexes from back group as skipped/erased:
			skipfield_pointer_type current_skipfield_node = end_iterator.group_pointer->skipfield + (end_iterator.group_pointer->last_endpoint - end_iterator.group_pointer->elements);
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
			
			erased_locations.push(end_iterator.group_pointer->last_endpoint++);
			skipfield_type current = previous_node_value + 1;
			const skipfield_pointer_type end_node = current_skipfield_node + distance_to_end - 1;
	
			while(current_skipfield_node != end_node)
			{
       			erased_locations.push(end_iterator.group_pointer->last_endpoint++);
				*(++current_skipfield_node) = ++current;
			}
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
		total_number_of_elements += source.total_number_of_elements;;
		
		
		source.end_iterator.group_pointer = NULL;
		source.end_iterator.element_pointer = NULL;
		source.end_iterator.skipfield_pointer = NULL;
		source.begin_iterator = source.end_iterator;
		source.first_group = NULL;
		source.total_number_of_elements = 0;
	}


};	// colony




template <class element_type, class element_allocator_type, typename element_skipfield_type>
inline void swap (colony<element_type, element_allocator_type, element_skipfield_type> &a, colony<element_type, element_allocator_type, element_skipfield_type> &b) PLF_COLONY_NOEXCEPT_SWAP(element_allocator_type)
{
	a.swap(b);
}




} // plf namespace




#undef PLF_COLONY_FORCE_INLINE

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
