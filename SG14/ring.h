#pragma once

#include <cstddef>
#include <type_traits>
#include <iterator>
#include <cassert>

namespace sg14
{
	template <typename T>
	struct null_popper
	{
		void operator()(T&);
	};

	template <typename T>
	struct default_popper
	{
		T operator()(T& t);
	};

	template <typename T>
	struct copy_popper
	{
		copy_popper(T&& t);
		T operator()(T& t);
		T copy;
	};

	template <typename, bool>
	class ring_iterator;

	template<typename T, class Popper = default_popper<T>>
	class ring_span
	{
	public:
		using type = ring_span<T, Popper>;
		using size_type = std::size_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using const_reference = const T&;
		using iterator = ring_iterator<type, false>;
		using const_iterator = ring_iterator<type, true>;

		friend class ring_iterator<type, false>;
		friend class ring_iterator<type, true>;

		template <class ContiguousIterator>
		ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p = Popper()) noexcept;

		template <class ContiguousIterator>
		ring_span(ContiguousIterator begin, ContiguousIterator end, ContiguousIterator first, size_type size, Popper p = Popper()) noexcept;

		ring_span(ring_span&&) = default;
		ring_span& operator=(ring_span&&) = default;

		bool empty() const noexcept;
		bool full() const noexcept;
		size_type size() const noexcept;
		size_type capacity() const noexcept;

		reference front() noexcept;
		const_reference front() const noexcept;
		reference back() noexcept;
		const_reference back() const noexcept;

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;

		template<bool b = true, typename = std::enable_if_t<b && std::is_copy_assignable<T>::value>>
		void push_back(const value_type& from_value) noexcept(std::is_nothrow_copy_assignable<T>::value);
		template<bool b = true, typename = std::enable_if_t<b && std::is_move_assignable<T>::value>>
		void push_back(value_type&& from_value) noexcept(std::is_nothrow_move_assignable<T>::value);
		template<class... FromType>
		void emplace_back(FromType&&... from_value) noexcept(std::is_nothrow_constructible<T, FromType...>::value && std::is_nothrow_move_assignable<T>::value);
		auto pop_front();

		void swap(type& rhs) noexcept;// (std::is_nothrow_swappable<Popper>::value);

		// Example implementation
	private:
		reference at(size_type idx) noexcept;
		const_reference at(size_type idx) const noexcept;
		size_type back_idx() const noexcept;
		void increase_size() noexcept;

		T* m_data;
		size_type m_size;
		size_type m_capacity;
		size_type m_front_idx;
		Popper m_popper;
	};

	template <typename Ring, bool is_const>
	class ring_iterator
	{
	public:
		using type = ring_iterator<Ring, is_const>;
		using value_type = typename Ring::value_type;
		using difference_type = std::ptrdiff_t;
		using pointer = typename std::conditional_t<is_const, const value_type, value_type>*;
		using reference = typename std::conditional_t<is_const, const value_type, value_type>&;
		using iterator_category = std::random_access_iterator_tag;

		template <bool C>
		bool operator==(const ring_iterator<Ring, C>& rhs) const noexcept;
		template <bool C>
		bool operator!=(const ring_iterator<Ring, C>& rhs) const noexcept;
		template <bool C>
		bool operator<(const ring_iterator<Ring, C>& rhs) const noexcept;
		template <bool C>
		bool operator<=(const ring_iterator<Ring, C>& rhs) const noexcept;
		template <bool C>
		bool operator>(const ring_iterator<Ring, C>& rhs) const noexcept;
		template <bool C>
		bool operator>=(const ring_iterator<Ring, C>& rhs) const noexcept;

		reference operator*() const noexcept;
		type& operator++() noexcept;
		type operator++(int) noexcept;
		type& operator--() noexcept;
		type operator--(int) noexcept;

		friend type& operator+=(type& it, int i) noexcept;
		friend type& operator-=(type& it, int i) noexcept;

		// Example implementation
	private:
		friend Ring;
		using size_type = typename Ring::size_type;
		ring_iterator(size_type idx, std::conditional_t<is_const, const Ring, Ring>* rv) noexcept;
		size_type m_idx;
		std::conditional_t<is_const, const Ring, Ring>* m_rv;
	};
}

// Sample implementation

template <typename T>
void sg14::null_popper<T>::operator()(T&)
{}

template <typename T>
T sg14::default_popper<T>::operator()(T& t)
{
	return std::move(t);
}

template <typename T>
sg14::copy_popper<T>::copy_popper(T&& t)
	: copy(std::move(t))
{}

template <typename T>
T sg14::copy_popper<T>::operator()(T& t)
{
	T old = t;
	t = copy;
	return t;
}

template<typename T, class Popper>
template<class ContiguousIterator>
sg14::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p) noexcept
	: m_data(&*begin)
	, m_size(0)
	, m_capacity(end - begin)
	, m_front_idx(0)
	, m_popper(std::move(p))
{}

template<typename T, class Popper>
template<class ContiguousIterator>
sg14::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end, ContiguousIterator first, size_type size, Popper p) noexcept
	: m_data(&*begin)
	, m_size(size)
	, m_capacity(end - begin)
	, m_front_idx(first - begin)
	, m_popper(std::move(p))
{}

template<typename T, class Popper>
bool sg14::ring_span<T, Popper>::empty() const noexcept
{
	return m_size == 0;
}

template<typename T, class Popper>
bool sg14::ring_span<T, Popper>::full() const noexcept
{
	return m_size == m_capacity;
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::size_type sg14::ring_span<T, Popper>::size() const noexcept
{
	return m_size;
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::size_type sg14::ring_span<T, Popper>::capacity() const noexcept
{
	return m_capacity;
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::reference sg14::ring_span<T, Popper>::front() noexcept
{
	return *begin();
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::reference sg14::ring_span<T, Popper>::back() noexcept
{
	return *(--end());
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_reference sg14::ring_span<T, Popper>::front() const noexcept
{
	return *begin();
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_reference sg14::ring_span<T, Popper>::back() const noexcept
{
	return *(--end());
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::iterator sg14::ring_span<T, Popper>::begin() noexcept
{
	return iterator(m_front_idx, this);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::iterator sg14::ring_span<T, Popper>::end() noexcept
{
	return iterator(size() + m_front_idx, this);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_iterator sg14::ring_span<T, Popper>::begin() const noexcept
{
	return const_iterator(m_front_idx, this);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_iterator sg14::ring_span<T, Popper>::cbegin() const noexcept
{
	return const_iterator(m_front_idx, this);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_iterator sg14::ring_span<T, Popper>::end() const noexcept
{
	return const_iterator(size() + m_front_idx, this);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_iterator sg14::ring_span<T, Popper>::cend() const noexcept
{
	return const_iterator(size() + m_front_idx, this);
}

template<typename T, class Popper>
template<bool b, typename>
void sg14::ring_span<T, Popper>::push_back(const T& value) noexcept(std::is_nothrow_copy_assignable<T>::value)
{
	m_data[back_idx()] = value;
	increase_size();
}

template<typename T, class Popper>
template<bool b, typename>
void sg14::ring_span<T, Popper>::push_back(T&& value) noexcept(std::is_nothrow_move_assignable<T>::value)
{
	m_data[back_idx()] = std::move(value);
	increase_size();
}

template<typename T, class Popper>
template<class... FromType>
void sg14::ring_span<T, Popper>::emplace_back(FromType&&... from_value) noexcept(std::is_nothrow_constructible<T, FromType...>::value && std::is_nothrow_move_assignable<T>::value)
{
	m_data[back_idx()] = T(std::forward<FromType>(from_value)...);
	increase_size();
}

template<typename T, class Popper>
auto sg14::ring_span<T, Popper>::pop_front()
{
	assert(m_size != 0);
	auto old_front_idx = m_front_idx;
	m_front_idx = (m_front_idx + 1) % m_capacity;
	--m_size;
	return m_popper(m_data[old_front_idx]);
}

template<typename T, class Popper>
void sg14::ring_span<T, Popper>::swap(sg14::ring_span<T, Popper>& rhs) noexcept//(std::is_nothrow_swappable<Popper>::value)
{
	using std::swap;
	swap(m_data, rhs.m_data);
	swap(m_size, rhs.m_size);
	swap(m_capacity, rhs.m_capacity);
	swap(m_front_idx, rhs.m_front_idx);
	swap(m_popper, rhs.m_popper);
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::reference sg14::ring_span<T, Popper>::at(size_type i) noexcept
{
	return m_data[i % m_capacity];
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::const_reference sg14::ring_span<T, Popper>::at(size_type i) const noexcept
{
	return m_data[i % m_capacity];
}

template<typename T, class Popper>
typename sg14::ring_span<T, Popper>::size_type sg14::ring_span<T, Popper>::back_idx() const noexcept
{
	return (m_front_idx + m_size) % m_capacity;
}

template<typename T, class Popper>
void sg14::ring_span<T, Popper>::increase_size() noexcept
{
	if (++m_size > m_capacity)
	{
		m_size = m_capacity;
		m_front_idx = (m_front_idx + 1) % m_capacity;
	}
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator==(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return (m_idx == rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator!=(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return !(*this == rhs);
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator<(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return (m_idx < rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator<=(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return (m_idx <= rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator>(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return (m_idx > rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template<bool C>
bool sg14::ring_iterator<Ring, is_const>::operator>=(const sg14::ring_iterator<Ring, C>& rhs) const noexcept
{
	return (m_idx > rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
typename sg14::ring_iterator<Ring, is_const>::reference sg14::ring_iterator<Ring, is_const>::operator*() const noexcept
{
	return m_rv->at(m_idx);
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const>& sg14::ring_iterator<Ring, is_const>::operator++() noexcept
{
	++m_idx;
	return *this;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const> sg14::ring_iterator<Ring, is_const>::operator++(int) noexcept
{
	auto r(*this);
	++*this;
	return r;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const>& sg14::ring_iterator<Ring, is_const>::operator--() noexcept
{
	--m_idx;
	return *this;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const> sg14::ring_iterator<Ring, is_const>::operator--(int) noexcept
{
	auto r(*this);
	--*this;
	return r;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const>& operator+=(sg14::ring_iterator<Ring, is_const>& it, int i) noexcept
{
	it.m_idx += i;
	return it;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const>& operator-=(sg14::ring_iterator<Ring, is_const>& it, int i) noexcept
{
	it.m_idx -= i;
	return it;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const>::ring_iterator(typename sg14::ring_iterator<Ring, is_const>::size_type idx, std::conditional_t<is_const, const Ring, Ring>* rv) noexcept
	: m_idx(idx)
	, m_rv(rv)
{}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const> operator+(sg14::ring_iterator<Ring, is_const> it, int i) noexcept
{
	it += i;
	return it;
}

template <typename Ring, bool is_const>
sg14::ring_iterator<Ring, is_const> operator-(sg14::ring_iterator<Ring, is_const> it, int i) noexcept
{
	it -= i;
	return it;
}
