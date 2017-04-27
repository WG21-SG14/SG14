#pragma once

namespace stdext
{
	template<class ForwardIterator>
	void destruct(ForwardIterator begin, ForwardIterator end)
	{
		typedef typename std::iterator_traits<ForwardIterator>::value_type _T;
		while (begin != end)
		{
			begin->~_T();
			++begin;
		}
	}


	template<class SrcIt, class Sentinel, class FwdIt>
	FwdIt uninitialized_move(SrcIt SrcBegin, Sentinel SrcEnd, FwdIt Dst)
	{
		FwdIt current = Dst;
		try
		{
			while (SrcBegin != SrcEnd)
			{
				::new (static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<FwdIt>::value_type(std::move(*SrcBegin));
				++current;
				++SrcBegin;
			}
			return current;
		}
		catch (...)
		{
			destruct(Dst, current);
			throw;
		}

	}

	template<class FwdIt, class Sentinel>
	FwdIt uninitialized_value_construct(FwdIt first, Sentinel last)
	{
		FwdIt current = first;
		try
		{
			while (current != last)
			{
				::new (static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<FwdIt>::value_type();
				++current;
			}
			return current;
		}
		catch (...)
		{
			destruct(first, current);
			throw;
		}
	}

	template<class FwdIt, class Sentinel>
	FwdIt uninitialized_default_construct(FwdIt first, Sentinel last)
	{
		FwdIt current = first;
		try
		{
			while (current != last)
			{
				::new (static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<FwdIt>::value_type;
				++current;
			}
			return current;
		}
		catch (...)
		{
			destruct(first, current);
			throw;
		}
	}

	template<class BidirIt, class UnaryPredicate>
	BidirIt unstable_remove_if(BidirIt first, BidirIt last, UnaryPredicate p)
	{
		while (true) {
			while ((first != last) && p(*first)) {
				++first;
			}
			if (first == last) break;
			--last;
			while ((first != last) && !p(*last)) {
				--last;
			}
			if (first == last) break;
			*first = std::move(*last);
			++first;
		}
		return first;
	}

	template<class BidirIt, class Val>
	BidirIt unstable_remove(BidirIt first, BidirIt last, Val v)
	{
		while (true) {
			while ((first != last) && (*first == v)) {
				++first;
			}
			if (first == last) break;
			--last;
			while ((first != last) && !(*last == v)) {
				--last;
			}
			if (first == last) break;
			*first = std::move(*last);
			++first;
		}
		return first;
	}



	//this exists as a point of reference for providing a stable comparison vs unstable_remove_if
	template<class BidirIt, class UnaryPredicate>
	BidirIt partition(BidirIt first, BidirIt last, UnaryPredicate p)
	{
		while (true) {
			while ((first != last) && p(*first)) {
				++first;
			}
			if (first == last) break;
			--last;
			while ((first != last) && !p(*last)) {
				--last;
			}
			if (first == last) break;
			std::iter_swap(first, last);
			++first;
		}
		return first;
	}

	//this exists as a point of reference for providing a stable comparison vs unstable_remove_if
	template<class ForwardIt, class UnaryPredicate>
	ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p)
	{
		first = std::find_if(first, last, p);
		if (first != last) {
			for (ForwardIt i = first; ++i != last; ) {
				if (!p(*i)) {
					*first = std::move(*i);
					++first;
				}
			}
		}
		return first;
	}
}
