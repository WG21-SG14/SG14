#ifndef PLF_BOOLED_DEQUE_H
#define PLF_BOOLED_DEQUE_H

#if (defined(_MSC_VER) && (_MSC_VER > 1600)) || (defined(__cplusplus) && __cplusplus >= 201103L)
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

namespace plf
{


template <class element_type, class allocator_type = std::allocator<element_type> >
class booled_deque
{
private:	
	typedef std::deque<bool, typename allocator_type::template rebind<bool>::other> skipfield_deque;

	std::deque<element_type, allocator_type> elements;
	skipfield_deque skipfield;
	
public:
	class iterator
	{
	private:
		typename std::deque<element_type, allocator_type>::iterator element_iterator;
		typename skipfield_deque::iterator skipfield_iterator;
		skipfield_deque *skipfield;
		
		friend class booled_deque;
	public:
		iterator() {};
		~iterator() {};
	
		iterator(const iterator &source):
			element_iterator(source.element_iterator),
			skipfield_iterator(source.skipfield_iterator),
			skipfield(source.skipfield)
		{};
		
		inline iterator & operator = (const iterator &source)
		{
			element_iterator = source.element_iterator;
			skipfield_iterator = source.skipfield_iterator;
			skipfield = source.skipfield;
			return *this;
		}
		
		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			iterator(const iterator &&source):
				element_iterator(source.element_iterator),
				skipfield_iterator(source.skipfield_iterator),
				skipfield(source.skipfield)
			{};

			inline iterator & operator = (const iterator &&source)
			{
				element_iterator = source.element_iterator;
				skipfield_iterator = source.skipfield_iterator;
				skipfield = source.skipfield;
				return *this;
			}
		#endif
				
		inline iterator & operator ++ ()
		{
			do
			{
				++element_iterator;
			} while (*(++skipfield_iterator) && skipfield_iterator != skipfield->end());
			
			return *this;
		}
		
		inline iterator & operator -- ()
		{
			do 
			{
				--element_iterator;
			} while (*(--skipfield_iterator) && skipfield_iterator != skipfield->begin());

			return *this;
		}
		
		
		inline element_type & operator * () const 
		{
			return *element_iterator;
		}



		inline element_type * operator -> () const 
		{
			return &(*element_iterator);
		}
		
	
		inline bool operator == (const iterator &rh) const 
		{
			return (element_iterator == rh.element_iterator);
		}



		inline bool operator != (const iterator &rh) const 
		{
			return (element_iterator != rh.element_iterator);
		}
	
	};

private:

	iterator begin_iterator, end_iterator;
	unsigned int total_size;

public:
	booled_deque(): total_size(0)
	{
		begin_iterator.element_iterator = elements.begin();
		begin_iterator.skipfield_iterator = skipfield.begin();
		begin_iterator.skipfield = &skipfield;
		end_iterator.element_iterator = elements.end();
		end_iterator.skipfield_iterator = skipfield.end();
		end_iterator.skipfield = &skipfield;
	};
	
	booled_deque(const booled_deque &source): total_size(0)
	{ 
		copy(source);
		begin_iterator.skipfield = &skipfield;
		end_iterator.skipfield = &skipfield;
	};
	
	~booled_deque() {};
	
	
	inline iterator begin()
	{
		return begin_iterator;
	}
	
	
	inline const iterator & begin() const
	{
		return begin_iterator;
	}
	
	
	inline iterator end()
	{
		return end_iterator;
	}
	
	
	inline const iterator & end() const
	{
		return end_iterator;
	}
	
	
	inline void push_back(const element_type &element)
	{
		elements.push_back(element);
		skipfield.push_back(false);
		begin_iterator.element_iterator = elements.begin();
		begin_iterator.skipfield_iterator = skipfield.begin();
		end_iterator.element_iterator = elements.end();
		end_iterator.skipfield_iterator = skipfield.end();
		++total_size;
	}
	
	
	inline void copy(const booled_deque &source)
	{
		for (iterator it = source.begin(); it != source.end(); ++it)
		{
			push_back(*it);
		}
	}
	
	
	inline iterator erase(const iterator &location)
	{
		location.element_iterator->~element_type();
		*(location.skipfield_iterator) = true;
		--total_size;
		return ++(iterator(location));
	}

	
	inline void reserve(unsigned int amount)
	{
		elements.reserve(amount);
		skipfield.reserve(amount);
	}

	
	inline booled_deque & operator = (const booled_deque &source)
	{
		elements.clear();
		skipfield.clear();
		total_size = 0;

		copy(source);
		
		return *this;
	}

	
#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	booled_deque(booled_deque &&source): elements(std::move(source.elements)), skipfield(std::move(source.skipfield)), total_size(source.total_size)
	{
		begin_iterator.element_iterator = elements.begin();
		begin_iterator.skipfield_iterator = skipfield.begin();
		begin_iterator.skipfield = &skipfield;
		end_iterator.element_iterator = elements.end();
		end_iterator.skipfield_iterator = skipfield.end();
		end_iterator.skipfield = &skipfield;
	}
	

	inline booled_deque & operator = (booled_deque &&source)
	{
		elements = std::move(source.elements);
		skipfield = std::move(source.skipfield);
		begin_iterator.element_iterator = elements.begin();
		begin_iterator.skipfield_iterator = skipfield.begin();
		begin_iterator.skipfield = &skipfield;
		end_iterator.element_iterator = elements.end();
		end_iterator.skipfield_iterator = skipfield.end();
		end_iterator.skipfield = &skipfield;
		total_size = source.total_size;
		
		return *this;
	}
#endif
	
	inline unsigned int size() const
	{
		return total_size;
	}
	
	inline unsigned int capacity() const
	{
		return static_cast<unsigned int>(
		((((elements.size() * sizeof(element_type)) / 512) + 1) * 512) / sizeof(element_type)
		); // this approximation based on GCC (libstdc++) deque implementation only
	}
	
	inline unsigned int approximate_memory_use() const
	{
		return static_cast<unsigned int>(
			((((elements.size() * sizeof(element_type)) / 512) + 1) * 512) + 
			((((skipfield.size() * sizeof(element_type *)) / 512) + 1) * 512) + 
			sizeof(*this)); // this approximation based on GCC (libstdc++) deque implementation only
	}

	inline void clear()
	{
		elements.clear();
		skipfield.clear();
		total_size = 0;
		begin_iterator.element_iterator = elements.begin();
		begin_iterator.skipfield_iterator = skipfield.begin();
		begin_iterator.skipfield = &skipfield;
		end_iterator.element_iterator = elements.end();
		end_iterator.skipfield_iterator = skipfield.end();
		end_iterator.skipfield = &skipfield;
	}
	
	
};

}

#endif // PLF_BOOLED_DEQUE_H
