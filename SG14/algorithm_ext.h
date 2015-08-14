#pragma once

namespace stdext
{
	template<class BEGIN, class SENTINEL>
	void destruct(BEGIN begin, SENTINEL end)
	{
		typedef decltype(*begin) T;
		while (begin != end)
		{
			begin->~T();
			++begin;
		}
	}

	template<class BidirIt, class UnaryPredicate>
	BidirIt unstable_remove_if(BidirIt first, BidirIt last, UnaryPredicate p)
	{
		while (1) {
			while ((first != last) && p(*first)) {
				++first;
			}
			if (first == last--) break;
			while ((first != last) && !p(*last)) {
				--last;
			}
			if (first == last) break;
			*first++ = std::move(*last);
		}
		return first;
	}
	template<class BidirIt, class Val>
	BidirIt unstable_remove(BidirIt first, BidirIt last, Val v)
	{
		while (1) {
			while ((first != last) && (*first == v)) {
				++first;
			}
			if (first == last--) break;
			while ((first != last) && !(*last == v)) {
				--last;
			}
			if (first == last) break;
			*first++ = std::move(*last);
		}
		return first;
	}
	//this exists as a point of reference for providing a stable comparison vs unstable_remove_if
	template<class BidirIt, class UnaryPredicate>
	BidirIt partition(BidirIt first, BidirIt last, UnaryPredicate p)
	{
		while (1) {
			while ((first != last) && p(*first)) {
				++first;
			}
			if (first == last--) break;
			while ((first != last) && !p(*last)) {
				--last;
			}
			if (first == last) break;
			std::iter_swap(first++, last);
		}
		return first;
	}
}