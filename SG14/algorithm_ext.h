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

	template<class T, class _Pr>
	inline T unstable_remove_if(T first, T last, _Pr p)
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
}