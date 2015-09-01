 #pragma once
#include <utility>
#include <memory>
//hot stands for hash, open-addressing w/ tombstone (for invalid value)
//note: this class is untested and incomplete

struct default_open_addressing_load_algorithm
{
	size_t occupancy(size_t allocated)
	{
		auto half = allocated >> 1;
		return half + (half >> 1);
	}
	size_t allocated(size_t occupied)
	{
		return occupied + (occupied << 1);
	}
};

namespace probe
{
	enum class result_type
	{
		empty,
		sought,
		unsought
	};

	
	struct forward
	{
		template<class U, class T, class Operation>
		void operator()(T* begin, T* start, T* end, Operation op)
		{
			auto It = start;
			for (unsigned pass = 0; pass < 2; ++pass)
			{
				do
				{
					if (op(It))
					{
						return;
					}
					++It;
				} while (It != end);

				end = start;
				It = begin;
			}
		}
	};

	struct perfect
	{
		template<class U, class T, class Operation>
		void operator()(T* begin, T* start, T* end, Operation op)
		{
			op(start);
		}
	};

	//alternating divergent series
	struct nearest
	{
		template<class U, class T, class Operation>
		void operator()(T* begin, T* start, T* end, Operation op)
		{
			auto It = start;
			ptrdiff_t offset = 1;
			while (It != end && It != begin - 1)
			{
				if (op(It))
				{
					return;
				}
				It = start + offset;
				offset *= -1;
				offset += ptrdiff_t(offset > 0);
			};

			auto inc = It == end ? -1 : -1;
			It = start + offset;

			if (It == end)
			{
				end = begin - 1;
			}
			else
			{
				assert(It == begin);
			}

			while (It != end)
			{
				if (op(It))
				{
					return;
				}
				It += inc;
			}
			
		}
	}

}

template<class T, T key>
struct static_alloc_adapter : public Alloc
{
	template<class T>
	void construct(T* p)
	{
		return new(p) T(key);
	}
};

template<class K, class Alloc>
struct dynamic_alloc_adapter : public Alloc
{
	K key;
	template<class T>
	void construct(T* p)
	{
		return new(p) T(key);
	}
};

template<
	class T,
	class Alloc = static_alloc_adapter<T, std::allocator<T>>,
	class Hash = std::hash<T>,
	class Probe = probe::forward,
	class Cap = default_open_addressing_load_algorithm,
	class Eq = std::equal_to<T>
>
class hotset : public Mgr, public Probe, public Hash, public Cap, public Alloc, public Eq
{
	using inner_iterator = std::begin(inner);
	using const_inner_iterator = std::begin((const Container&)inner);

	T* begin;
	T* end;
	size_t mcapacity;
	size_t moccupied;

	T* hash_at(const T& Item)
	{
		std::hash<T> h;
		return mbegin + (Hash::operator()(Item) & (moccupied - 1));
	}
	void init(size_t size)
	{
		if (size > 0)
		{
			//assert(powerOfTwo(size));
			inner.reserve(size);
			Mgr::initialize(mbegin, mend);
			moccupied = 0;
			mcapacity = Cap::upper_bound<T>(mend - mbegin);
		}
	}

	template<class U>
	std::pair<inner_iterator, bool> insert_internal(U&& item)
	{
		auto Found = find(item);
		*Found.first = std::forward<U>(item);
		return res;
	}

	template<class U>
	std::pair<inner_iterator, bool> find_internal(const U& in)
	{
		auto result = std::make_pair(std::end(inner), false);
		auto Invalid = Mgr::get();
		auto op = [&result, &in, &Invalid](T* at)
		{
			if (Eq::operator()(Invalid, *at))
			{
				result = std::make_pair(at, false);
			}
			else if (Eq::operator()(in, *at))
			{
				result = std::make_pair(at, true);
			}
			return cls != hashsearchclass::unsought;
		};

		Probe::operator()(begin(inner), hash_at(item), std::end(inner), op);
		return result;
	}
	void fillempty()
	{
		auto here = mbegin;
		while (here != mend)
		{
			Alloc::construct(here);
			++here;
		}
	}

	template<class U>
	bool rehash(U&& value)
	{
		auto oldbegin = mbegin;
		auto oldend = mend;
		auto size = (mend-mbegin) << 1;
		mcapacity = Cap::operator()(size);

		mbegin = allocate(size, mbegin);
		mend = mbegin + size;
		fillempty();

		auto it = oldbegin;
		std::aligned_storage_t<sizeof(T), __alignof(T)> sInvalid;
		allocator::construct(&sInvalid);
		while (it != oldend)
		{
			if (Eq::operator()(Invalid, *at))
			{
				auto h = hash_at(*it);
				insert_internal(std::move(*it));
			}
		}
		deallocate(oldbegin, oldend-oldbegin);
		auto h = hash_at(value);
		++moccupied;
		return insert_internal(std::forward<U>(value)).second;
		
	}

	void partial_rehash(T* start)
	{
		auto op = [this](T* at)
		{
			auto cls = this->Mgr::classify(*at);
			if (cls == hashsearchclass::empty)
			{
				return true;
			}
			else if (cls == hashsearchclass::sought)
			{
				auto temp = std::move(*It);
				c.invalidate(It, It + 1);
				*find(begin, start, end, c).first = std::move(temp);
			}
			return cls != hashsearchclass::unsought;
		};

		return Probe::operator()(begin, start, end, op);
	}
	void remove_internal(T* found)
	{
		--moccupied;
		Mgr::remove(found, found+1);
		partial_rehash(found + 1);
	}

public:
	hotset() = default;
	hotset(const hotset& in) = default;
	hotset(hotset&& in) = default;

	hotset( Probe p = Probe(), Hash h = Hash(),Mgr m = Mgr(), Cap cap = Cap(), const Alloc& alloc = Alloc())
		: Mgr(m)
		, Probe(p)
		, Hash(h)
		, Cap(cap)
		, Alloc(alloc)
		, c(inc)
		, moccupied(0)
	{
		init(num);
	}

	size_t reserved() const
	{
		return mend - mbegin;
	}
	size_t capacity() const
	{
		return mcapacity;
	}
	size_t size() const
	{
		return moccupied;
	}

	template<class U>
	const_inner_iterator insert(U&& value)
	{
		if (mcapacity == moccupied)
		{
			return rehash(std::forward<U>(value));
		}
		auto result = insert_internal(std::forward<U>(value));
		moccupied += uint32_t(result.second == false);
		return result.second ? result.first : std::end(inner);
	}

	void erase(const T* value)
	{
		remove_internal(value);
	}
	bool erase(const T& value)
	{
		auto found = find_internal(item);
		if (found.second)
		{
			remove_internal(found.first);
			return true;
		}
		return false;
	}
	bool empty() const
	{
		return moccupied == 0;
	}
	bool clear()
	{
		
		moccupied = 0;
	}

	template<class U>
	const_inner_iterator operator[](const U& value) const
	{
		auto result = find_internal(value);
		return result.second ? result.first : std::end(inner);
	}

	void resize(size_t s)
	{
		auto reals = nextpoweroftwo(s);
		if (reals != reserved())
		{

		}
	}

	struct begin_iterator : std::iterator< std::bidirectional_iterator_tag, T>
	{
		T* pos;
		T* end;
		hotset& h;

		begin_iterator(hotset& h)
			:pos(at), invalid(std::move(inv)
		{
		}

		const T& operator*() const
		{
			return *pos;
		}
		iterator& operator++()
		{
			pos = std::find_if(pos + 1, end, [&](T& elem) { return !h.is_invalid(elem); });
		}

		const T* base() const
		{
			return pos;
		}
	};

	begin_iterator begin()
	{

	}

	T* end()
	{
		return mend;
	}
	~hotset() = default;


};

template<class T> using hodtset = hotset< T, dynamic_tombstone<T> >;
template<class T, T val> using hostset = hotset< T, static_tombstone<T> >;