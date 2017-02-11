#ifndef PLF_INDEXED_VECTOR_H
#define PLF_INDEXED_VECTOR_H

#include <cstring> // for size_t
#include <vector>


#if (defined(_MSC_VER) && (_MSC_VER > 1600)) || (defined(__cplusplus) && __cplusplus >= 201103L)
	#define PLF_INDEXED_VECTOR_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif


namespace plf
{
	

template <class element_type, class allocator_type = std::allocator<element_type> >
class indexed_vector
{
private:	
	typedef std::vector<unsigned int, typename allocator_type::template rebind<unsigned int>::other> index_vector;

	std::vector<element_type, allocator_type> elements;
	index_vector element_indexes;
	unsigned int current_back_index;
	
public:
	typedef typename index_vector::iterator iterator;
	
	indexed_vector(): current_back_index(0) {};
	indexed_vector(const indexed_vector &source): elements(source.elements), element_indexes(source.element_indexes), current_back_index(source.current_back_index) {};
	~indexed_vector() {};

	
	inline const element_type & get(const iterator &location) const
	{
		return elements[*location];
	}

	
	inline element_type & set(const iterator &location)
	{
		return elements[*location];
	}

	
	inline void push_back(const element_type &element)
	{
		elements.push_back(element);
		element_indexes.push_back(current_back_index++);
	}

	
	inline iterator begin() { return element_indexes.begin(); };
	inline iterator end() { return element_indexes.end(); };
	inline const iterator & begin() const { return element_indexes.begin(); };
	inline const iterator & end() const { return element_indexes.end(); };


	inline iterator erase(iterator &location)
	{
		elements[*location].~element_type();
		return element_indexes.erase(location);
	}


	inline void reserve(unsigned int amount)
	{
		elements.reserve(amount);
		element_indexes.reserve(amount);
	}


	inline indexed_vector & operator = (const indexed_vector &source)
	{
		elements = source.elements;
		element_indexes = source.element_indexes;
		current_back_index = source.current_back_index;
		
		return *this;
	}


#ifdef PLF_INDEXED_VECTOR_MOVE_SEMANTICS_SUPPORT
	indexed_vector(indexed_vector &&source): elements(std::move(source.elements)), element_indexes(std::move(source.element_indexes)), current_back_index(source.current_back_index) {}


	inline indexed_vector & operator = (indexed_vector &&source)
	{
		elements = std::move(source.elements);
		element_indexes = std::move(source.element_indexes);
		current_back_index = source.current_back_index;
		
		return *this;
	}
#endif

	
	inline size_t size() const
	{
		return static_cast<size_t>(element_indexes.size());
	}

	
	inline size_t capacity() const
	{
		return static_cast<size_t>(elements.capacity());
	}
	

	inline size_t approximate_memory_use() const
	{
		return static_cast<size_t>(
			(elements.capacity() * sizeof(element_type)) + 
			(element_indexes.capacity() * sizeof(unsigned int)) + 
			sizeof(*this));
	}


	inline void clear()
	{
		elements.clear();
		element_indexes.clear();
		current_back_index = 0;
	}


	inline void remove_if()
	{
		iterator result = begin(), it = begin(), last = end();
		
		while (it != last)
		{
			if (!(set(it).erased))
			{
				*result = *it;
				++result;
			}

			++it;
		}
		
		for (it = result; it != last; ++it)
		{
			element_indexes.pop_back();
		}
	}
};


}

#ifdef PLF_INDEXED_VECTOR_MOVE_SEMANTICS_SUPPORT
#undef PLF_INDEXED_VECTOR_MOVE_SEMANTICS_SUPPORT
#endif

#endif // PLF_INDEXED_VECTOR_H
