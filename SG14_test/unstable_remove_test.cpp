#include "SG14_test.h"
#include <vector>
#include <array>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "algorithm_ext.h"
#include <cassert>

struct foo
{
	std::array<int,15> info;
	static foo make()
	{
		foo result;
		std::fill(result.info.begin(), result.info.end(), ::rand());
		return result;
	}
};
template<class T, class U>
void* operator new(size_t s, std::raw_storage_iterator<T, U> it)
{
	return ::operator new(s, it.base());
}

void sg14_test::unstable_remove_test()
{
#if 0
	std::vector<foo> list1;
	std::generate_n(std::back_inserter(list1), 3000000, foo::make);
	auto list2 = list1;

	auto cmp = [](foo& f) {return f.info[0] & 1; };
	auto t0 = clock();
	stdext::partition(list2.begin(), list2.end(), cmp);
	auto t1 = clock();
	stdext::unstable_remove_if(list1.begin(), list1.end(), cmp);

	auto t2 = clock();

	std::cout << "partition: " << t1 - t0 << "\n";
	std::cout << "unstable: " << t2 - t1 << "\n";
	std::cin.get();
#endif
}