// Copyright (c) 2016, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef PLF_STACK_H
#define PLF_STACK_H



// Compiler-specific defines used by stack:
// Note: these macros are replicated between stack and colony but given different names, to avoid compiler issues with multiple inclusions of colony and stack with a project, caused by the #undef'ing of these macros at the end of the headers:
#if defined(_MSC_VER)
	#define PLF_STACK_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_STACK_NOEXCEPT throw()
	#elif _MSC_VER == 1600
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
	#elif _MSC_VER == 1700
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
	#elif _MSC_VER == 1800
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
	#elif _MSC_VER >= 1900
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT noexcept
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
	#define PLF_STACK_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && !defined(__clang__) // If compiler is GCC/G++
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#endif
	#else // Assume type traits support for non-GCC compilers
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
	#endif

	#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
	#define PLF_STACK_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
	#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
	#define PLF_STACK_NOEXCEPT noexcept
#else
	#define PLF_STACK_FORCE_INLINE
	#define PLF_STACK_NOEXCEPT throw()
#endif


#ifdef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, ...) std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, data) std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_STACK_DESTROY(the_allocator, allocator_instance, location) 			std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
	#define PLF_STACK_ALLOCATE(the_allocator, allocator_instance, size, hint) 		std::allocator_traits<the_allocator>::allocate(allocator_instance, size, hint)
 	#define PLF_STACK_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 			std::allocator_traits<the_allocator>::allocate(*this, size, hint)
	#define PLF_STACK_DEALLOCATE(the_allocator, allocator_instance, location, size) std::allocator_traits<the_allocator>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, ...) 	allocator_instance.construct(location, __VA_ARGS__)
	#else
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, data) 	allocator_instance.construct(location, data)
	#endif

	#define PLF_STACK_DESTROY(the_allocator, allocator_instance, location) 				allocator_instance.destroy(location)
	#define PLF_STACK_ALLOCATE(the_allocator, allocator_instance, size, hint) 			allocator_instance.allocate(size, hint)
	#define PLF_STACK_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 				the_allocator::allocate(size, hint)
	#define PLF_STACK_DEALLOCATE(the_allocator, allocator_instance, location, size) 	allocator_instance.deallocate(location, size)
#endif





#include <cassert>	// assert
#include <limits>  // std::numeric_limits
#include <memory>	// std::uninitialized_copy, std::allocator

#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
	#include <type_traits> // std::is_trivially_destructible
#endif




namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class stack : private element_allocator_type  // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type																value_type;
	typedef element_allocator_type														allocator_type;

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::size_type		size_type;
		typedef element_type &															reference;
		typedef const element_type &													const_reference;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 		pointer;
		typedef typename std::allocator_traits<element_allocator_type>::const_pointer	const_pointer;
	#else
		typedef typename element_allocator_type::size_type			size_type;
		typedef typename element_allocator_type::reference			reference;
		typedef typename element_allocator_type::const_reference	const_reference;
		typedef typename element_allocator_type::pointer			pointer;
		typedef typename element_allocator_type::const_pointer		const_pointer;
	#endif

private:
	struct group; // Forward declaration for typedefs below

	#ifdef PLF_COLONY_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group> group_allocator_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer		group_pointer_type;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 	element_pointer_type;
	#else
		typedef typename element_allocator_type::template rebind<group>::other	group_allocator_type;
		typedef typename group_allocator_type::pointer							group_pointer_type;
		typedef typename element_allocator_type::pointer						element_pointer_type;
	#endif

	struct group : private element_allocator_type // Empty base class optimisation - inheriting allocator functions
	{
		const element_pointer_type		elements;
		group_pointer_type				next_group, previous_group;
		const element_pointer_type		end; // End is the actual end element of the group, not one-past the end element as it is in colony


		#ifdef PLF_STACK_VARIADICS_SUPPORT
			group(const size_type elements_per_group, group_pointer_type const previous = NULL):
				elements(PLF_STACK_ALLOCATE_INITIALIZATION(element_allocator_type, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
				next_group(NULL),
				previous_group(previous),
				end(elements + elements_per_group - 1)
			{}

		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const size_type elements_per_group, group_pointer_type const previous = NULL):
				elements(NULL),
				next_group(reinterpret_cast<group_pointer_type>(elements_per_group)),
				previous_group(previous),
				end(NULL)
			{}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source) PLF_STACK_NOEXCEPT:
				element_allocator_type(source),
				elements(PLF_STACK_ALLOCATE_INITIALIZATION(element_allocator_type, reinterpret_cast<size_type>(source.next_group), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL),
				previous_group(source.previous_group),
				end(elements + reinterpret_cast<size_type>(source.next_group) - 1)
			{}
		#endif


		~group() PLF_STACK_NOEXCEPT
		{
			// Null check not necessary (for empty group and copied group as above) as delete will ignore.
			PLF_STACK_DEALLOCATE(element_allocator_type, (*this), elements, (end - elements) + 1); // Size is calculated from end and elements
		}
	};


	group_pointer_type		current_group, first_group;
	element_pointer_type	current_element, start_element, end_element;
	size_type				total_number_of_elements, min_elements_per_group;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		size_type max_elements_per_group;
		ebco_pair(const size_type max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;


	template <class colony_type, class colony_allocator, typename colony_skipfield_type> friend class colony;



public:

	explicit stack(const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		current_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group(8),
		group_allocator_pair(std::numeric_limits<size_type>::max() / 2)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
	}



	explicit stack(const size_type min_allocation_amount, const size_type max_allocation_amount = (std::numeric_limits<size_type>::max() / 2), const element_allocator_type &alloc = element_allocator_type()):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		current_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group(min_allocation_amount),
		group_allocator_pair(max_allocation_amount)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
		assert(group_allocator_pair.max_elements_per_group <= std::numeric_limits<size_type>::max() / 2); // Must be half of what the allocator can allocate, otherwise could result in overflow, because at the point where we might allocate a max group of that size, the previous groups may have a a total size equal to it
	}



	stack(const stack &source):
		element_allocator_type(source),
		current_group(NULL),
		first_group(NULL),
		current_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(source.total_number_of_elements),
		min_elements_per_group(source.min_elements_per_group),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		assert(&source != this);

		if (total_number_of_elements == 0)
		{
			return;
		}

		group_pointer_type current_copy_group = source.first_group;
		const group_pointer_type end_copy_group = source.current_group;

		if (total_number_of_elements <= group_allocator_pair.max_elements_per_group) // most common case
		{
			min_elements_per_group = total_number_of_elements;
			initialize();
			min_elements_per_group = source.min_elements_per_group;

			// Copy groups to this stack:
			while (current_copy_group != end_copy_group)
			{
				std::uninitialized_copy(current_copy_group->elements, current_copy_group->end + 1, current_element);
				current_element += (current_copy_group->end + 1) - current_copy_group->elements;
				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			std::uninitialized_copy(source.start_element, source.current_element + 1, current_element);
			end_element = current_element += source.current_element - source.start_element; // This should make current_element == the last "pushed" element, rather than the one past it
		}
		else // uncommon edge case, so not optimising:
		{
			min_elements_per_group = group_allocator_pair.max_elements_per_group;
			total_number_of_elements = 0;
			initialize();

			while (current_copy_group != end_copy_group)
			{
				for (element_pointer_type element_to_copy = current_copy_group->elements; element_to_copy != current_copy_group->end + 1; ++element_to_copy)
				{
					push(*element_to_copy);
				}

				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			for (element_pointer_type element_to_copy = source.start_element; element_to_copy != source.current_element + 1; ++element_to_copy)
			{
				push(*element_to_copy);
			}

			min_elements_per_group = source.min_elements_per_group;
		}
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// move constructor
		stack(stack &&source) PLF_STACK_NOEXCEPT:
			element_allocator_type(source),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			current_element(std::move(source.current_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_number_of_elements(source.total_number_of_elements),
			min_elements_per_group(source.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			// Nullify source object's contents - only first_group and total_number_of_elements required for destructor:
			source.first_group = NULL;
			source.total_number_of_elements = 0;
		}
	#endif
	


	~stack() PLF_STACK_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_STACK_NOEXCEPT
	{
		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if (total_number_of_elements != 0 && !(std::is_trivially_destructible<element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with ermpty destructor
		#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
			if (total_number_of_elements != 0)
		#endif
		{
			total_number_of_elements = 0;
			group_pointer_type previous_group;
			element_pointer_type past_end;

			while (first_group != current_group)
			{
				past_end = first_group->end + 1;

				for (element_pointer_type element_pointer = first_group->elements; element_pointer != past_end; ++element_pointer)
				{
					PLF_STACK_DESTROY(element_allocator_type, (*this), element_pointer);
				}

				previous_group = first_group;
				first_group = first_group->next_group;

				PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
				PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);
			}

			// Special case for current group:
			past_end = current_element + 1;

			for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
			{
				PLF_STACK_DESTROY(element_allocator_type, (*this), element_pointer);
			}

			PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, first_group);
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = NULL;
		}
		else
		{
			// Although technically under a type-traits-supporting compiler total_number_of_elements could be non-zero at this point, since first_group would already be NULL in the case of double-destruction, it's unnecessary to zero total_number_of_elements here
			while (first_group != NULL)
			{
				current_group = first_group;
				first_group = first_group->next_group;
				PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, current_group);
				PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
			}
		}
	}


	void initialize()
	{
		first_group = current_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		// Initialize:
		try
		{
			#ifdef PLF_STACK_VARIADICS_SUPPORT
				PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_elements_per_group);
			#else
				PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_elements_per_group));
			#endif
		}
		catch (...)
		{
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = current_group = NULL;
			throw;
		}

		current_element = start_element = first_group->elements;
		end_element = current_group->end;
	}


public:

	void push(const element_type &the_element)
	{
		switch ((current_element == NULL) + (current_element == end_element))
		{
			case 0:
			{
				try
				{
					PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++current_element, the_element);
				}
				catch (...)
				{
					--current_element;
					throw;
				}

				++total_number_of_elements;
				return;
			}
			case 1:
			{
				if (current_group->next_group == NULL)
				{
					current_group->next_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group);

					try
					{
						#ifdef PLF_STACK_VARIADICS_SUPPORT
							PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group);
						#else
							PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group((total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group));
						#endif
					}
					catch (...)
					{
						PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
						current_group->next_group = NULL;
						throw;
					}
				}

				current_group = current_group->next_group;
				start_element = current_element = current_group->elements;

				try
				{
					PLF_STACK_CONSTRUCT(element_allocator_type, (*this), current_element, the_element);
				}
				catch (...)
				{
					current_group = current_group->previous_group;
					start_element = current_group->elements;
					current_element = current_group->end;
					throw;
				}

				end_element = current_group->end;
				++total_number_of_elements;
				return;
			}
			case 2:
			{
				initialize();
				
				try
				{
					PLF_STACK_CONSTRUCT(element_allocator_type, (*this), current_element, the_element);
				}
				catch (...)
				{
					clear();
					throw;
				}

				++total_number_of_elements; 
				return;
			}
		}
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// Note: the reason for code duplication from non-move push, as opposed to using std::forward for both, was because most compilers didn't actually create as-optimal code in that strategy. Also more C++03 compatible
		void push(element_type &&the_element)
		{
			switch ((current_element == NULL) + (current_element == end_element))
			{
				case 0:
				{
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++current_element, std::move(the_element));
					}
					catch (...)
					{
						--current_element;
						throw;
					}
		
					++total_number_of_elements;
					return;
				}
				case 1:
				{
					if (current_group->next_group == NULL)
					{
						current_group->next_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group); 
						
						try
						{ 
							#ifdef PLF_STACK_VARIADICS_SUPPORT
								PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group);
							#else
								PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group((total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group));
							#endif
						} 
						catch (...) 
						{ 
							PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
							current_group->next_group = NULL; 
							throw;
						}
					}
					
					current_group = current_group->next_group; 
					start_element = current_element = current_group->elements;
					
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), current_element, std::move(the_element));
					}
					catch (...)
					{
						current_group = current_group->previous_group;
						start_element = current_group->elements;
						current_element = current_group->end;
						throw;
					}
		
					end_element = current_group->end;
					++total_number_of_elements; 
					return;
				}	
				case 2:
				{
					initialize();
					
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), current_element, std::move(the_element));
					}
					catch (...)
					{
						clear();
						throw;
					}
		
					++total_number_of_elements; 
					return;
				}
			}
		}


		#ifdef PLF_STACK_VARIADICS_SUPPORT
			template<typename... Arguments> inline void emplace(Arguments... parameters)
			{
				push(element_type(std::forward<Arguments>(parameters)...)); // Use object's parameter'd constructor
			}
		#endif
	#endif



	inline PLF_STACK_FORCE_INLINE reference top() const PLF_STACK_NOEXCEPT
	{
		assert(!empty());
		return *current_element;
	}



	void pop() PLF_STACK_NOEXCEPT
	{
		assert(!empty());

		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if (!(std::is_trivially_destructible<element_type>::value)) // This if-statement should be removed by the compiler on resolution of element_type
		#endif
		{
			PLF_STACK_DESTROY(element_allocator_type, (*this), current_element);
		}
		
		// ie. if total_number_of_elements != 0 after decrement, or we were not already at the start of a non-first group
		if (total_number_of_elements-- == 1 || current_element != start_element) // If total_number_of_elements is now 0 after decrement, this essentially moves current_element back to it's initial position (start_element - 1). But otherwise, this is just a regular pop
		{
			--current_element;
		}
		else
		{ // ie. is start element, but not first group in stack (if it were, totalsize would be 0 after decrement)
			current_group = current_group->previous_group;
			start_element = current_group->elements;
			end_element = current_element = current_group->end;
		}
	}



	inline stack & operator = (const stack &source)
	{
		assert(&source != this);

		destroy_all_data();
		stack temp(source);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			this->swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		stack & operator = (stack &&source) PLF_STACK_NOEXCEPT
		{
			assert (&source != this);

			destroy_all_data();

			current_group = std::move(source.current_group);
			first_group = std::move(source.first_group);
			current_element = std::move(source.current_element);
			start_element = std::move(source.start_element);
			end_element = std::move(source.end_element);
			total_number_of_elements = source.total_number_of_elements;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;

			// Nullify source object's contents - only first_group and total_number_of_elements required to be altered for destructor to work on it:
			source.first_group = NULL;
			source.total_number_of_elements = 0;

			return *this;
		}
	#endif



	inline PLF_STACK_FORCE_INLINE bool empty() const PLF_STACK_NOEXCEPT
	{
		return total_number_of_elements == 0;
	}


	
	inline size_type size() const PLF_STACK_NOEXCEPT
	{
		return total_number_of_elements;
	}



	inline size_type max_size() const PLF_STACK_NOEXCEPT
	{
		return element_allocator_type::max_size();
	}



	size_type capacity() const PLF_STACK_NOEXCEPT
	{
		size_type total_size = 0;
		group_pointer_type temp_group = first_group;

		while (temp_group != NULL)
		{
			total_size += static_cast<size_type>((temp_group->end + 1) - temp_group->elements);
			temp_group = temp_group->next_group;
		}

		return total_size;
	}



	size_type approximate_memory_use() const PLF_STACK_NOEXCEPT
	{
		size_type memory_use = sizeof(*this);
		group_pointer_type temp_group = first_group;

		while (temp_group != NULL)
		{
			memory_use += static_cast<size_type>((((temp_group->end + 1) - temp_group->elements) * sizeof(value_type)) + sizeof(group));
			temp_group = temp_group->next_group;
		}

		return memory_use;
	}



	inline void change_minimum_group_size(const size_type min_allocation_amount)
	{
		change_group_sizes(min_allocation_amount, group_allocator_pair.max_elements_per_group);
	}
	
	

	inline void change_maximum_group_size(const size_type max_allocation_amount)
	{
		change_group_sizes(min_elements_per_group, max_allocation_amount);
	}
	
	

	void change_group_sizes(const size_type min_allocation_amount, const size_type max_allocation_amount)
	{
		assert(min_allocation_amount > 2);
		assert(min_allocation_amount <= max_allocation_amount);
		assert(max_allocation_amount <= std::numeric_limits<size_type>::max() / 2);

		min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;

		if (first_group != NULL)
		{
			stack temp(*this);

			#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
				*this = std::move(temp); // Avoid generating 2nd temporary
			#else
				this->swap(temp);
			#endif
		}
	}



	void clear() PLF_STACK_NOEXCEPT
	{
		destroy_all_data();
		total_number_of_elements = 0;
		current_element = NULL;
		start_element = NULL;
		end_element = NULL;
	}



	bool operator == (const stack &rh) const PLF_STACK_NOEXCEPT
	{
		assert (this != &rh);

		if (total_number_of_elements != rh.total_number_of_elements)
		{
			return false;
		}
		else if (total_number_of_elements == 0 && rh.total_number_of_elements == 0)
		{
			return true;
		}

		group_pointer_type this_group = first_group, rh_group = rh.first_group;
		element_pointer_type this_pointer = first_group->elements, rh_pointer = rh.first_group->elements;

		while(true)
		{
			if (*this_pointer != *rh_pointer)
			{
				return false;
			}
			else if (this_pointer == current_element) // end condition
			{
				break;
			}
			else if (this_pointer++ == this_group->end) // incrementing in the more common case where this is not true - combining the equality test and increment usually compiles into single instruction as opposed to two instructions for the usual way of doing it
			{
				this_group = this_group->next_group;
				this_pointer = this_group->elements;
			}

			if (rh_pointer++ == rh_group->end)
			{
				rh_group = rh_group->next_group;
				rh_pointer = rh_group->elements;
			}
		}
		
		return true;
	}



	inline bool operator != (const stack &rh) const PLF_STACK_NOEXCEPT
	{
		return !(*this == rh);
	}



	// Remove trailing stack groups (not removed in general 'pop' usage for performance reasons)
	void trim_trailing_groups() PLF_STACK_NOEXCEPT
	{
		group_pointer_type temp_group = current_group->next_group, temp_group2;
		current_group->next_group = NULL; // Set to NULL regardless of whether it is already NULL (avoids branching). Cuts off rest of groups from this group.

		while (temp_group != NULL)
		{
			temp_group2 = temp_group;
			temp_group = temp_group->next_group;
			PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, temp_group2);
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, temp_group2, 1);
		}
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

		const size_type original_min_elements = min_elements_per_group;
		min_elements_per_group = (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? static_cast<unsigned short>(total_number_of_elements) : group_allocator_pair.max_elements_per_group;
		min_elements_per_group = (min_elements_per_group < 3) ? 3 : min_elements_per_group;

		stack temp(*this);

		#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			this->swap(temp);
		#endif

		min_elements_per_group = original_min_elements;
	}



	void reserve(size_type reserve_amount)
	{
		assert(reserve_amount > 2);

		if (reserve_amount > group_allocator_pair.max_elements_per_group)
		{
			reserve_amount = group_allocator_pair.max_elements_per_group;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = max_size();
		}

		if (first_group == NULL) // If this is a newly-created stack, no pushes yet
		{
			const size_type original_min_elements = min_elements_per_group;
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
			// Reallocate all data:
			const size_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;

			stack temp(*this);

			#ifdef PLF_COLONY_MOVE_SEMANTICS_SUPPORT
				*this = std::move(temp); // Avoid generating 2nd temporary
			#else
				this->swap(temp);
			#endif

			min_elements_per_group = original_min_elements;
		}
	}



	void swap(stack &source) PLF_STACK_NOEXCEPT
	{
		#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
			stack temp(std::move(source));
			source = std::move(*this);
			*this = std::move(temp);
		#else
			group_pointer_type		swap_current_group = current_group, swap_first_group = first_group;
			element_pointer_type	swap_current_element = current_element, swap_start_element = start_element, swap_end_element = end_element;
			size_type				swap_total_number_of_elements = total_number_of_elements, swap_min_elements_per_group = min_elements_per_group, swap_max_elements_per_group = group_allocator_pair.max_elements_per_group;

			current_group = source.current_group;
			first_group = source.first_group;
			current_element = source.current_element;
			start_element = source.start_element;
			end_element = source.end_element;
			total_number_of_elements = source.total_number_of_elements;
			min_elements_per_group = source.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
			
			source.current_group = swap_current_group;
			source.first_group = swap_first_group;
			source.current_element = swap_current_element;
			source.start_element = swap_start_element;
			source.end_element = swap_end_element;
			source.total_number_of_elements = swap_total_number_of_elements;
			source.min_elements_per_group = swap_min_elements_per_group;
			source.group_allocator_pair.max_elements_per_group = swap_max_elements_per_group;
		#endif
	}	


	friend inline void swap (stack &a, stack &b) PLF_STACK_NOEXCEPT
	{
		a.swap(b);
	}

}; // stack


} // plf namespace


#undef PLF_STACK_TYPE_TRAITS_SUPPORT
#undef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_STACK_VARIADICS_SUPPORT
#undef PLF_STACK_MOVE_SEMANTICS_SUPPORT
#undef PLF_STACK_NOEXCEPT

#undef PLF_STACK_CONSTRUCT
#undef PLF_STACK_DESTROY
#undef PLF_STACK_ALLOCATE
#undef PLF_STACK_ALLOCATE_INITIALIZATION
#undef PLF_STACK_DEALLOCATE


#endif // PLF_STACK_H
