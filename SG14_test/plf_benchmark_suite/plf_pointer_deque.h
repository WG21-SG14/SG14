#pragma once

#ifndef PLF_POINTER_DEQUE_H
#define PLF_POINTER_DEQUE_H

#include <cstring> // for size_t

#if (defined(_MSC_VER) && (_MSC_VER > 1600)) || (defined(__cplusplus) && __cplusplus >= 201103L)
	#define PLF_POINTER_DEQUE_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif


namespace plf
{
	

template <class element_type, class allocator_type = std::allocator<element_type> >
class pointer_deque
{
private:	
	typedef std::deque<element_type *, typename allocator_type::template rebind<element_type *>::other> element_pointer_deque;

	std::deque<element_type, allocator_type> elements;
	element_pointer_deque element_pointers;
	
public:
	typedef typename element_pointer_deque::iterator iterator;
	
	pointer_deque() {};
	pointer_deque(const pointer_deque &source): elements(source.elements) { copy_fix(source); };
	pointer_deque(unsigned int amount): elements(amount), element_pointers(amount) { };
	~pointer_deque() {};
	
	
	inline void push_back(const element_type &element)
	{
		elements.push_back(element);
		element_pointers.push_back(&(elements.back()));
	}
	
	
	inline void copy_fix(const pointer_deque &source)
	{
		element_pointers.clear();
		unsigned int index = 0;
		typename std::deque<element_type, allocator_type>::const_iterator last_iterator = source.elements.cbegin();
		
		// Deque elements are non-contiguous in memory so we need to iterate over the source elements to find the index, rather than doing a simple memory -= operation like we would with a vector:
		for (typename std::deque<element_type *, allocator_type>::const_iterator location = source.element_pointers.cbegin(); location != source.element_pointers.cend(); ++location)
		{
			for (typename std::deque<element_type, allocator_type>::const_iterator element_location = last_iterator; element_location != source.elements.cend(); ++element_location)
			{
				if (&(*(*location)) == &(*element_location))
				{
					last_iterator = element_location;
					break;
				}
				
				++index;
			}
			
			element_pointers.push_back(&(elements[index]));
		}
	}
	
	
	inline iterator begin() { return element_pointers.begin(); };
 	inline iterator end() { return element_pointers.end(); };
	inline const iterator & begin() const { return element_pointers.begin(); };
	inline const iterator & end() const { return element_pointers.end(); };

	
	inline iterator erase(iterator &location)
	{
		(*location)->~element_type();
		return element_pointers.erase(location);
	}

	
	inline pointer_deque & operator = (const pointer_deque &source)
	{
		elements = source.elements;
		copy_fix(source);
		
		return *this;
	}

	

#ifdef PLF_POINTER_DEQUE_MOVE_SEMANTICS_SUPPORT
	pointer_deque(pointer_deque &&source): elements(std::move(source.elements)), element_pointers(std::move(source.element_pointers)) { }


	inline pointer_deque & operator = (pointer_deque &&source)
	{
		elements = std::move(source.elements);
		element_pointers = std::move(source.element_pointers);
		
		return *this;
	}
#endif	


	inline size_t size() const
	{
		return static_cast<size_t>(element_pointers.size());
	}


	inline size_t capacity() const
	{
		return static_cast<size_t>(
		((((elements.size() * sizeof(element_type)) / 512) + 1) * 512) / sizeof(element_type)
		); // this approximation based on GCC (libstdc++) deque implementation only
	}
	
	
	inline size_t approximate_memory_use() const
	{
		return static_cast<size_t>(
			64 + // node map
			((((elements.size() * sizeof(element_type)) / 512) + 1) * 512) + 
			((((element_pointers.size() * sizeof(element_type *)) / 512) + 1) * 512) + 
			sizeof(*this)); // this approximation based on GCC (libstdc++) deque implementation only
	}
	
	
	inline void clear()
	{
		elements.clear();
		element_pointers.clear();
	}
	
	
	inline void remove_if()
	{
		iterator result = begin(), it = begin(), last = end();
		
		while (it != last)
		{
			if (!((*it)->erased))
			{
				*result = *it;
				++result;
			}

			++it;
		}
		
		for (it = result; it != last; ++it)
		{
			element_pointers.pop_back();
		}
	}

};

}


#ifdef PLF_POINTER_DEQUE_MOVE_SEMANTICS_SUPPORT
#undef PLF_POINTER_DEQUE_MOVE_SEMANTICS_SUPPORT
#endif

#endif // PLF_POINTER_DEQUE_H
