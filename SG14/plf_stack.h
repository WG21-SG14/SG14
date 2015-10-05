// Copyright (c) 2015, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef plf_stack_H
#define plf_stack_H


// Compiler-specific defines used by both stack and colony:
#if __cplusplus >= 201103L
	#define PLF_TRAITS_SUPPORT
	#define PLF_ALLOCATOR_TRAITS_SUPPORT
	#define PLF_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros together
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT noexcept
#elif defined(_MSC_VER)
	#if _MSC_VER < 1600
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1600
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1700
		#define PLF_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1800
		#define PLF_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER >= 1900
		#define PLF_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT noexcept
	#endif
#else
	#define PLF_NOEXCEPT throw()
#endif




#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(allocator_type, allocator_instance, location, ...) std::allocator_traits<allocator_type>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(allocator_type, allocator_instance, location, data) std::allocator_traits<allocator_type>::construct(allocator_instance, location, data)
	#endif

	#define PLF_DESTROY(allocator_type, allocator_instance, location) std::allocator_traits<allocator_type>::destroy(allocator_instance, location)
	#define PLF_ALLOCATE(allocator_type, allocator_instance, size, hint) std::allocator_traits<allocator_type>::allocate(allocator_instance, size, hint)
	#define PLF_ALLOCATE_INITIALIZATION(allocator_type, size, hint) std::allocator_traits<allocator_type>::allocate(*this, size, hint)
	#define PLF_DEALLOCATE(allocator_type, allocator_instance, location, size) std::allocator_traits<allocator_type>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(allocator_type, allocator_instance, location, ...) allocator_instance.construct(location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(allocator_type, allocator_instance, location, data) allocator_instance.construct(location, data)
	#endif

	#define PLF_DESTROY(allocator_type, allocator_instance, location) allocator_instance.destroy(location)
	#define PLF_ALLOCATE(allocator_type, allocator_instance, size, hint) allocator_instance.allocate(size, hint)
	#define PLF_ALLOCATE_INITIALIZATION(allocator_type, size, hint) allocator_type::allocate(size, hint)
	#define PLF_DEALLOCATE(allocator_type, allocator_instance, location, size) allocator_instance.deallocate(location, size)
#endif




#include <cstring>	// memcpy
#include <cassert>	// assert
#include <climits>	// UINT_MAX
#include <memory>	// std::uninitialized_copy, allocators

#ifdef PLF_MOVE_SEMANTICS_SUPPORT 
	#include <utility> // std::move
#endif



namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class stack : private element_allocator_type
{
private:
	struct group; // Forward declaration for typedefs below
	typedef typename element_allocator_type::template rebind<group>::other group_allocator_type;
	typedef typename element_allocator_type::pointer	element_pointer_type;
	typedef typename group_allocator_type::pointer		group_pointer_type;

	struct group : private element_allocator_type
	{
		const element_pointer_type	elements;
		group_pointer_type			next_group, previous_group;
		const element_pointer_type	end; // End is the actual end element of the group, not one-past the end element as it is in colony


		#ifdef PLF_VARIADICS_SUPPORT
			group(const unsigned int elements_per_group, group_pointer_type const previous = NULL):
				elements(PLF_ALLOCATE_INITIALIZATION(element_allocator_type, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
				next_group(NULL),
				previous_group(previous),
				end(elements + elements_per_group - 1)
			{}


			~group() PLF_NOEXCEPT
			{
				PLF_DEALLOCATE(element_allocator_type, (*this), elements, (end - elements) + 1); // Size is calculated from end and elements pointers now
			}


		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++0x and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory due to allocating the same block twice, we're skipping the allocation for this pseudoconstructor:
			group(const unsigned int elements_per_group, group_pointer_type const previous = NULL) PLF_NOEXCEPT:
				elements(NULL),
				previous_group(previous),
				end(reinterpret_cast<element_pointer_type>(elements_per_group)) // Using end as a temporary to store elements_per_group till copy constructor
			{}


			// Not a "real" copy constructor ie. does not copy content of source asides from previous_group and size. Only used for allocator.construct in C++0x.
			group(const group &source):
				elements(PLF_ALLOCATE_INITIALIZATION(element_allocator_type, reinterpret_cast<unsigned int>(source.end), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL),
				previous_group(source.previous_group),
				end(elements + reinterpret_cast<unsigned int>(source.end) - 1)
			{}


			~group() PLF_NOEXCEPT
			{
				if (elements != NULL) // Necessary to avoid deallocation for pseudoconstructor above
				{
					PLF_DEALLOCATE(element_allocator_type, (*this), elements, (end - elements) + 1);
				}
			}
		#endif

	};


	group_pointer_type		current_group, first_group;
	element_pointer_type	current_element, start_element;
	unsigned int			total_size;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with least-used member variable, for empty-base-class optimisation
	{
		unsigned int max_elements_per_group;
		ebco_pair(const unsigned int max_elements) : max_elements_per_group(max_elements) {};
	}						group_allocator_pair;


	template <class colony_type, class colony_allocator> friend class colony;

	inline void initialize(const unsigned int elements_per_group)
	{
		assert(elements_per_group > 2);
		assert(group_allocator_pair.max_elements_per_group <= UINT_MAX / 2);

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
		
		start_element = first_group->elements;
		current_element = start_element - 1;
	}

public:

	stack(const unsigned int initial_allocation_amount = 8, const unsigned int max_allocation_amount = UINT_MAX /2):
		current_group(PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0)),
		first_group(current_group),
		total_size(0),
		group_allocator_pair(max_allocation_amount)
	{
		initialize(initial_allocation_amount);
	}


	void reinitialize(const unsigned int initial_allocation_amount)
	{
		destroy_all_data();
		first_group = current_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);
		initialize(initial_allocation_amount);
	}


	inline void reinitialize(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount)
	{
		group_allocator_pair.max_elements_per_group = max_allocation_amount;
		reinitialize(initial_allocation_amount);
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// move constructor
		stack(stack &&source) PLF_NOEXCEPT:
			current_group(source.current_group),
			first_group(source.first_group),
			current_element(source.current_element),
			start_element(source.start_element),
			total_size(source.total_size),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			// Nullify source object's contents - only first_group required for destructor:
			source.first_group = NULL;
			source.total_size = 0;
		}
	#endif
	

	
	// Copy constructor
	stack(const stack &source): 
		current_group(PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0)),
		first_group(current_group),
		total_size(source.total_size), 
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		assert(&source != this);

		// Initialize:
		try
		{
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, (total_size < 3) ? 3 : total_size);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group((total_size < 3) ? 3 : total_size));
			#endif
		}
		catch (...)
		{
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			throw;
		}

		current_element = start_element = first_group->elements;

		// Copy groups to this stack:
		group_pointer_type current_copy_group = source.first_group, end_copy_group = source.current_group;

		while (current_copy_group != end_copy_group)
		{
			std::uninitialized_copy(current_copy_group->elements, current_copy_group->end + 1, current_element);
			current_element += (current_copy_group->end + 1) - current_copy_group->elements;
			current_copy_group = current_copy_group->next_group;
		}

		// Handle special case of last group:
		std::uninitialized_copy(source.start_element, source.current_element + 1, current_element);
		current_element += source.current_element - source.start_element; // This should make current_element = the last "pushed" element, rather than the one past it
	}



	~stack() PLF_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_NOEXCEPT
	{
	#ifdef PLF_TRAITS_SUPPORT 
		if (total_size != 0 && !(std::is_trivially_destructible<element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with ermpty destructor
	#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
		if (total_size != 0)
	#endif				
		{
			total_size = 0;

			group_pointer_type previous_group;
			element_pointer_type past_end;
			
			while (first_group != current_group)
			{
				past_end = first_group->end + 1;
				
				for (element_pointer_type element_pointer = first_group->elements; element_pointer != past_end; ++element_pointer)
				{
					PLF_DESTROY(element_allocator_type, (*this), element_pointer);
				}

				previous_group = first_group;
				first_group = first_group->next_group;

				PLF_DESTROY(group_allocator_type, group_allocator_pair, previous_group);
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group, 1);
			} 
			
			// Special case for current group:
			past_end = current_element + 1;
			
			for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
			{
				PLF_DESTROY(element_allocator_type, (*this), element_pointer);
			}

			PLF_DESTROY(group_allocator_type, group_allocator_pair, first_group);
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = NULL;
		}
		else
		{
			while (first_group != NULL)
			{
				current_group = first_group;
				first_group = first_group->next_group;
				PLF_DESTROY(group_allocator_type, group_allocator_pair, current_group);
				PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
			}
		}
	}




	// Begin push macro
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_STACK_PUSH_MACRO_GROUP_ADD (total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, current_group 
	#else
		#define PLF_STACK_PUSH_MACRO_GROUP_ADD group((total_size < group_allocator_pair.max_elements_per_group) ? total_size : group_allocator_pair.max_elements_per_group, current_group)
	#endif


	#define PLF_STACK_PUSH_MACRO(ASSIGNMENT_OBJECT) \
		if(current_element != current_group->end) \
		{ \
			try \
			{ \
				PLF_CONSTRUCT(element_allocator_type, (*this), ++current_element, ASSIGNMENT_OBJECT); \
			} \
			catch (...) \
			{ \
				--current_element; \
				throw; \
			} \
			\
			++total_size; \
		} \
		else \
		{ \
			if (current_group->next_group == NULL) \
			{ \
				current_group->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group); \
				\
				try \
				{ \
					PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, PLF_STACK_PUSH_MACRO_GROUP_ADD); \
				} \
				catch (...) \
				{ \
					PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1); \
					current_group->next_group = NULL; \
					throw; \
				} \
			} \
			\
			current_group = current_group->next_group; \
			start_element = current_element = current_group->elements; \
			\
			try \
			{ \
				PLF_CONSTRUCT(element_allocator_type, (*this), current_element, ASSIGNMENT_OBJECT); \
			} \
			catch (...) \
			{ \
				current_group = current_group->previous_group; \
				start_element = current_group->elements; \
				current_element = current_group->end; \
				throw; \
			} \
			\
			++total_size; \
		} \
	// end push macro



public:


	void push(const element_type &the_element)
	{
		PLF_STACK_PUSH_MACRO(the_element) // Use copy constructor
	}
	


	inline element_type & back() const PLF_NOEXCEPT
	{
		assert(!empty());
		return *current_element;
	}



	void pop() PLF_NOEXCEPT
	{
		assert(!empty());

		PLF_DESTROY(element_allocator_type, (*this), current_element); // If total_size is now 0 after decrement (below), this essentially moves current_element back to initial position (start_element - 1). But otherwise, this is a regular pop

		if (total_size-- == 1 || current_element != start_element)
		{
			--current_element;
		}
		else
		{ // ie. is start element, but not first group in stack (or totalsize would be 0 after decrement)
			current_group = current_group->previous_group;
			start_element = current_group->elements;
			current_element = current_group->end;
		}
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Move-push
		void push(element_type &&the_element)
		{
			PLF_STACK_PUSH_MACRO(std::move(the_element)) // Use move constructor
		}
	#endif



	#ifdef PLF_VARIADICS_SUPPORT 
		template<typename... Arguments>
		void emplace(Arguments... parameters)
		{
			PLF_STACK_PUSH_MACRO(parameters...) // Use object's parameter'd constructor
		}
	#endif
	
	

	stack & operator = (const stack &source)
	{
		assert(&source != this);

		destroy_all_data(); // Deallocating before reallocation in copy constructor, to free up memory and avoid potential out-of-memory exceptions

		stack temp_stack(source); // Use copy constructor
		std::memcpy(this, &temp_stack, sizeof(stack<element_type, element_allocator_type>));
		temp_stack.first_group = NULL;
		
		return *this;
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT 
		// Move assignment
		stack & operator = (stack &&source) PLF_NOEXCEPT
		{
			assert (&source != this);

			destroy_all_data();
			std::memcpy(this, &source, sizeof(stack<element_type, element_allocator_type>));

			// Nullify source object's contents - only first_group and total_size required to be altered for destructor to work on it:
			source.first_group = NULL;
			source.total_size = 0;
			
			return *this;
		}
	#endif



	inline bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}


	
	inline unsigned int size() const PLF_NOEXCEPT
	{
		return total_size;
	}


	
	inline void clear()
	{
		reinitialize(static_cast<const unsigned int>((first_group->end + 1) - first_group->elements)); // ie. Reverts to original size
	}



	bool operator == (const stack &rh) const PLF_NOEXCEPT
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




	inline bool operator != (const stack &rh) const PLF_NOEXCEPT
	{
		return !(*this == rh);
	}


};

}

#endif // plf_stack_H