#include <array>
#include <vector>

namespace sg14
{
	template<typename T, std::size_t capacity>
	class static_ring
	{
	public:
		typedef std::array<T, capacity> container_type;
		typedef typename container_type::value_type value_type;
		typedef typename container_type::size_type size_type;
		typedef typename container_type::reference reference;
		typedef typename container_type::const_reference const_reference;
		typedef typename container_type::iterator iterator;
		typedef typename container_type::const_iterator const_iterator;
		typedef typename container_type::reverse_iterator reverse_iterator;
		typedef typename container_type::const_reverse_iterator const_reverse_iterator;

		static_ring() noexcept(std::is_nothrow_default_constructible<T>::value)
			: c()
			, count(0)
			, next_element(std::begin(c))
			, last_element(next_element)
		{
		}

		static_ring(const static_ring& rhs) noexcept(std::is_nothrow_copy_constructible<T>::value)
			: c(rhs.c)
			, count(rhs.count)
			, next_element(std::begin(c) + (rhs.next_element - std::begin(rhs.c)))
			, last_element(std::begin(c) + (rhs.last_element - std::begin(rhs.c)))
		{
		}

		static_ring(static_ring&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
			: c(std::move(rhs.c))
			, count(std::move(rhs.count))
			, next_element(std::begin(c) + (rhs.next_element - std::begin(rhs.c)))
			, last_element(std::begin(c) + (rhs.last_element - std::begin(rhs.c)))
		{
		}

		static_ring& operator=(const static_ring& rhs) noexcept(std::is_nothrow_copy_assignable<T>::value)
		{
			c = rhs.c;
			return (*this);
		}

		static_ring& operator=(static_ring&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value)
		{
			c = std::move(rhs.c);
			return (*this);
		}

		bool push(const value_type& from_value) noexcept(std::is_nothrow_copy_assignable<T>::value)
		{
			if (count == capacity)
			{
				return false;
			}
			*next_element = from_value;
			increase_back();
			return true;
		}

		bool push(value_type&& from_value) noexcept(std::is_nothrow_move_assignable<T>::value)
		{
			if (count == capacity)
			{
				return false;
			}
			*next_element = std::move(from_value);
			increase_back();
			return true;
		}

		template<class... FromType>
		bool emplace(FromType&&... from_value) noexcept(std::is_nothrow_constructible<T, FromType...>::value
								&& std::is_nothrow_move_assignable<T>::value)
		{
			if (count == capacity)
			{
				return false;
			}
			*next_element = T(std::forward<FromType>(from_value)...);
			increase_back();
			return true;
		}

		void pop()
		{
			*last_element = T();
			increase_front();
		}

		bool empty() const noexcept
		{
			return (next_element == last_element);
		}

		size_type size() const noexcept
		{
			return count;
		}

		reference front() noexcept
		{
			return (*last_element);
		}

		const_reference front() const noexcept
		{
			return (*last_element);
		}

		reference back() noexcept
		{
			auto it = decrease(next_element);
			return (*it);
		}

		const_reference back() const noexcept
		{
			auto it = decrease(next_element);
			return (*it);
		}

		void swap(static_ring& rhs) noexcept
		{
			auto lhs_next = next_element - c.begin();
			auto lhs_last = last_element - c.begin();
			auto rhs_next = rhs.next_element - rhs.c.begin();
			auto rhs_last = rhs.last_element - rhs.c.begin();
			std::swap(c, rhs.c);
			std::swap(count, rhs.count);
			next_element = c.begin() + rhs_next;
			last_element = c.begin() + rhs_last;
			rhs.next_element = rhs.c.begin() + lhs_next;
			rhs.last_element = rhs.c.begin() + lhs_last;
		}

	private:
		container_type c;
		size_t count;
		iterator next_element;
		iterator last_element;

		void increase_back()
		{
			if (++next_element == c.end())
			{
				next_element = c.begin();
			}
			++count;
		}

		iterator decrease(iterator it)
		{
			if (it == c.begin())
			{
				it = c.end();
			}
			return --it;
		}

		void increase_front()
		{
			if (++last_element == c.end())
			{
				last_element = c.begin();
			}
			--count;
		}
	};

	template<typename T, class Container = std::vector<T, std::allocator<T>>>
	class dynamic_ring
	{
	public:
		typedef Container container_type;
		typedef typename container_type::value_type value_type;
		typedef typename container_type::size_type size_type;
		typedef typename container_type::reference reference;
		typedef typename container_type::const_reference const_reference;
		typedef typename container_type::iterator iterator;
		typedef typename container_type::const_iterator const_iterator;
		typedef typename container_type::reverse_iterator reverse_iterator;
		typedef typename container_type::const_reverse_iterator const_reverse_iterator;

		explicit dynamic_ring(size_type initial_capacity = 8)
			: c(initial_capacity)
			, count(0)
			, next_element(std::begin(c))
			, last_element(next_element)
		{
		}

/*		dynamic_ring()
			: dynamic_ring(Allocator())
		{
		}

		explicit dynamic_ring(const Allocator& alloc);
		explicit dynamic_ring(size_type count);
		explicit dynamic_ring(size_type count, const Allocator& alloc = Allocator());*/

		dynamic_ring(const dynamic_ring& rhs)
			: c(rhs.c)
			, count(rhs.count)
			, next_element(std::begin(c) + (rhs.next_element - std::begin(rhs.c)))
			, last_element(std::begin(c) + (rhs.last_element - std::begin(rhs.c)))
		{
		}

/*		dynamic_ring(dynamic_ring&& rhs)
			: c(std::swap(rhs.c, c))
			, count(std::swap(rhs.count, count))
			, next_element(std::begin(c) + (rhs.next_element - std::begin(rhs.c)))
			, last_element(std::begin(c) + (rhs.last_element - std::begin(rhs.c)))
		{
		}*/

		dynamic_ring& operator=(const dynamic_ring& rhs)
		{
			c = rhs.c;
			return (*this);
		}

		dynamic_ring& operator=(dynamic_ring&& rhs)
		{
			c = std::move(rhs.c);
			return (*this);
		}

		void push(const value_type& from_value)
		{
			if (count == c.capacity())
			{
				resize();
			}
			*next_element = from_value;
			increase_back();
		}

		void push(value_type&& from_value)
		{
			if (count == c.capacity())
			{
				resize();
			}
			*next_element = std::move(from_value);
			increase_back();
		}

		template<class... FromType>
		void emplace(FromType&&... from_value)
		{
			if (count == c.capacity())
			{
				resize();
			}
			*next_element = T(std::forward<FromType>(from_value)...);
			increase_back();
		}

		void pop()
		{
			*last_element = T();
			increase_front();
		}

		bool empty() const noexcept
		{
			return (next_element == last_element);
		}

		size_type size() const noexcept
		{
			return count;
		}

		reference front() noexcept
		{
			return (*last_element);
		}

		const_reference front() const noexcept
		{
			return (*last_element);
		}

		reference back() noexcept
		{
			auto it = decrease(next_element);
			return (*it);
		}

		const_reference back() const noexcept
		{
			auto it = decrease(next_element);
			return (*it);
		}

		void swap(dynamic_ring& rhs) noexcept
		{
			auto lhs_next = next_element - c.begin();
			auto lhs_last = last_element - c.begin();
			auto rhs_next = rhs.next_element - rhs.c.begin();
			auto rhs_last = rhs.last_element - rhs.c.begin();
			std::swap(c, rhs.c);
			std::swap(count, rhs.count);
			next_element = c.begin() + rhs_next;
			last_element = c.begin() + rhs_last;
			rhs.next_element = rhs.c.begin() + lhs_next;
			rhs.last_element = rhs.c.begin() + lhs_last;
		}

	private:
		container_type c;
		size_t count;
		iterator next_element;
		iterator last_element;

		void increase_back()
		{
			if (++next_element == c.end())
			{
				next_element = c.begin();
			}
			++count;
		}

		iterator decrease(iterator it)
		{
			if (it == c.begin())
			{
				it = c.end();
			}
			return --it;
		}

		void increase_front()
		{
			if (++last_element == c.end())
			{
				last_element = c.begin();
			}
			--count;
		}

		void resize()
		{
			// Save the index of next_element
			// Double the size of the vector
			// Move everything, from next_element to back, to the end of the vector
			// Update last_element and next_element
			auto next_index = std::distance(c.begin(), next_element);
			auto tail_length = std::distance(c.end(), next_element);
			c.resize(count * 2);
			last_element = c.begin();
			std::advance(last_element, next_index - 1);
			next_element = c.begin();
			std::advance(next_element, next_index + count);
			auto begin_tail = last_element;
			++begin_tail;
			auto end_tail = begin_tail;
			std::advance(end_tail, tail_length);
			std::copy(begin_tail, end_tail, next_element);
		}
	}; 
}