// Copyright (c) 2015, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef plf_stack_H
#define plf_stack_H


// Compiler-specific defines used by both stack and colony:
#ifdef __GNUC__
	#ifdef __i386__
		#define PLF_GCCX86
	#elif defined (__x86_64__)
		#define PLF_GCCX64
	#endif
#elif defined(_MSC_VER)
	#if (_MSC_VER < 1600)
		#define PLF_VS200X // No c++11 support
	#elif _MSC_VER == 1600
		#define PLF_VS2010 // move semantics supported
	#elif _MSC_VER == 1700
		#define PLF_VS2012 // traits also supported
	#elif (_MSC_VER == 1800) 
		#define PLF_VS2013 // variadic templates also supported
	#elif (_MSC_VER >= 1900) 
		#define PLF_VS201X // noexcept also supported for visual studio 2015 and above
	#endif
#endif


#if __cplusplus >= 201103L || defined PLF_VS201X 
	#define PLF_TRAITS_SUPPORT
	#define PLF_VARIADICS_SUPPORT
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT noexcept
#elif defined PLF_VS2013 
	#define PLF_TRAITS_SUPPORT
	#define PLF_VARIADICS_SUPPORT
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT throw()
#elif defined PLF_VS2012
	#define PLF_TRAITS_SUPPORT
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT throw()
#elif defined PLF_VS2010
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT throw()
#else
	#define PLF_NOEXCEPT throw()
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


template <class element_type, class element_allocator_type = std::allocator<element_type> > class stack
{
private:
	struct group; // Forward declaration for typedefs below
	typedef typename element_allocator_type::template rebind<group>::other group_allocator_type;
	typedef typename element_allocator_type::pointer	element_pointer_type;
	typedef typename group_allocator_type::pointer		group_pointer_type;

	struct group
	{
		element_allocator_type		allocator;
		const element_pointer_type	elements;
		group_pointer_type			next_group, previous_group;
		const element_pointer_type	end; // End is the actual end element of the group, not one-past the end element as it is in colony
		const unsigned int			size;


		#ifdef PLF_VARIADICS_SUPPORT 
			group(const unsigned int elements_per_group): 
				elements(allocator.allocate(elements_per_group, 0)),
				next_group(NULL), 
				previous_group(NULL), 
				end(elements + elements_per_group - 1),
				size(elements_per_group)
			{}


			group(const unsigned int elements_per_group, group_pointer_type const previous):
				elements(allocator.allocate(elements_per_group, previous->elements)),
				next_group(NULL), 
				previous_group(previous), 
				end(elements + elements_per_group - 1),
				size(elements_per_group)
			{}


			~group() PLF_NOEXCEPT
			{
				allocator.deallocate(elements, size);
			}


		#else 
			// This is a hack around the fact that allocator.construct only supports copy construction in C++0x and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory due to allocating the same block twice, we're skipping the allocation for these pseudoconstructors.
			group(const unsigned int elements_per_group) PLF_NOEXCEPT:
				elements(NULL),
				previous_group(NULL), 
				end(NULL),
				size(elements_per_group) 
			{}


			group(const unsigned int elements_per_group, group_pointer_type const previous) PLF_NOEXCEPT: 
				elements(NULL),
				previous_group(previous), 
				end(NULL),
				size(elements_per_group)
			{}


			// Not a "real" copy constructor ie. does not copy content of source asides from previous_group and size. Only used for allocator.construct in C++0x.
			group(const group &source): 
				elements(allocator.allocate(source.size, (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL), 
				previous_group(source.previous_group), 
				end(elements + source.size - 1),
				size(source.size)
			{}


			~group() PLF_NOEXCEPT
			{
				if (elements != NULL)
				{
					allocator.deallocate(elements, size);
				}
			}
		#endif

	};


	element_allocator_type	allocator;
	group_allocator_type	group_allocator;
	group_pointer_type		current_group, first_group;
	element_pointer_type	current_element, end_element, start_element;
	unsigned int			total_size, max_elements_per_group;


	template <class colony_type, class colony_allocator> friend class colony;

	inline void initialize(const unsigned int elements_per_group)
	{
		assert(elements_per_group > 2);
		assert(max_elements_per_group <= UINT_MAX / 2);

		#ifdef PLF_VARIADICS_SUPPORT
			group_allocator.construct(first_group, elements_per_group);
		#else
			group_allocator.construct(first_group, group(elements_per_group));
		#endif

		start_element = first_group->elements;
		current_element = start_element - 1;
		end_element = first_group->end;
	}

public:

	stack(const unsigned int initial_allocation_amount = 8, const unsigned int max_allocation_amount = UINT_MAX /2):
		current_group(group_allocator.allocate(1, 0)),
		first_group(current_group),
		total_size(0),
		max_elements_per_group(max_allocation_amount)
	{
		initialize(initial_allocation_amount);
	}


	void reinitialize(const unsigned int initial_allocation_amount)
	{
		destroy_all_data();
		first_group = current_group = group_allocator.allocate(1, 0);
		initialize(initial_allocation_amount);
	}


	inline void reinitialize(const unsigned int initial_allocation_amount, const unsigned int max_allocation_amount)
	{
		max_elements_per_group = max_allocation_amount;
		reinitialize(initial_allocation_amount);
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// move constructor
		stack(stack &&source) PLF_NOEXCEPT: // nothrow required for some MS compilers
			current_group(source.current_group),
			first_group(source.first_group),
			current_element(source.current_element),
			end_element(source.end_element),
			start_element(source.start_element),
			total_size(source.total_size),
			max_elements_per_group(source.max_elements_per_group)
		{
			// Nullify source object's contents - only first_group required for destructor:
			source.first_group = NULL;
			source.total_size = 0;
		}
	#endif
	

	
	// Copy constructor
	stack(const stack &source): 
		current_group(group_allocator.allocate(1, 0)),
		first_group(current_group),
		total_size(source.total_size), 
		max_elements_per_group(source.max_elements_per_group)
	{
		assert(&source != this);

		// Initialize:
		#ifdef PLF_VARIADICS_SUPPORT
			group_allocator.construct(first_group, (total_size < 3) ? 3 : total_size);
		#else
			group_allocator.construct(first_group, group((total_size < 3) ? 3 : total_size));
		#endif

		current_element = start_element = first_group->elements;
		end_element = first_group->end;

		// Copy groups to this stack:
		group_pointer_type current_copy_group = source.first_group, *end_copy_group = source.current_group;

		while (current_copy_group != end_copy_group)
		{
			std::uninitialized_copy(current_copy_group->elements, current_copy_group->end + 1, current_element);
			current_element += current_copy_group->size;
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
	#else // If compiler doesn't support c++11, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
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
					allocator.destroy(element_pointer);
				}

				previous_group = first_group;
				first_group = first_group->next_group;
				group_allocator.destroy(previous_group);
				group_allocator.deallocate(previous_group, 1);
			} 
			
			// Special case for current group:
			past_end = current_element + 1;
			
			for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
			{
				allocator.destroy(element_pointer);
			}

			group_allocator.destroy(first_group); // ie. current_group
			group_allocator.deallocate(first_group, 1);
			first_group = NULL;
		}
		else
		{
			total_size = 0;

			while (first_group != NULL)
			{
				current_group = first_group;
				first_group = first_group->next_group;
				group_allocator.destroy(current_group);
				group_allocator.deallocate(current_group, 1);
			}
		}
	}




	// Begin add macro
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_STACK_PUSH_MACRO_GROUP_ADD (total_size < max_elements_per_group) ? total_size : max_elements_per_group, current_group 
	#else
		#define PLF_STACK_PUSH_MACRO_GROUP_ADD group((total_size < max_elements_per_group) ? total_size : max_elements_per_group, current_group)
	#endif


	#define PLF_STACK_PUSH_MACRO(ASSIGNMENT_OBJECT) \
		if(current_element != end_element) \
		{ \
			allocator.construct(++current_element, ASSIGNMENT_OBJECT); \
			++total_size; \
		} \
		else \
		{ \
			if (current_group->next_group == NULL) \
			{ \
				current_group->next_group = group_allocator.allocate(1, current_group); \
				group_allocator.construct(current_group->next_group, PLF_STACK_PUSH_MACRO_GROUP_ADD); \
			} \
			\
			++total_size; \
			current_group = current_group->next_group; \
			start_element = current_element = current_group->elements; \
			allocator.construct(current_element, ASSIGNMENT_OBJECT); \
			end_element = current_group->end; \
		} \
		\
	// end macro



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

		--total_size;
		
		if (current_element != start_element || total_size == 0) // If size is now 0, move current_element back to initial position (start_element - 1). Otherwise, this is a regular pop
		{
			allocator.destroy(current_element--);
		}
		else // ie. is start element, but not first group in stack (or totalsize would be 0)
		{
			allocator.destroy(current_element);
			current_group = current_group->previous_group;
			start_element = current_group->elements;
			end_element = current_element = current_group->end;
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
	
	

	void push() 
	{
		PLF_STACK_PUSH_MACRO(element_type()) // Use object's default constructor. Retrieve result via back()
	}

	
	
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

			// Nullify source object's contents - only first_group required to be altered for destructor to work on it:
			source.first_group = NULL;
			
			return *this;
		}
	#endif



	inline const bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}


	
	inline const unsigned int size() const PLF_NOEXCEPT
	{
		return total_size;
	}


	
	inline void clear()
	{
		reinitialize(first_group->size); // ie. Reverts to original size
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
		
		while (this_pointer != current_element)
		{
			if (*this_pointer != *rh_pointer)
			{
				return false;
			}
			else if (this_pointer != this_group->end)
			{
				++this_pointer;
			}
			else 
			{
				this_group = this_group->next_group;
				this_pointer = this_group->elements;
			}

			if (rh_pointer != rh_group->end)
			{
				++rh_pointer;
			}
			else
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