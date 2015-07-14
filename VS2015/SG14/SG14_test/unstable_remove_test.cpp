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
	std::array<uint32_t, 32> info;

	static foo make()
	{
		foo result;
		std::fill(result.info.begin(), result.info.end(), ::rand());
		return result;
	}
};

void sg14_test::unstable_remove_test()
{
	std::vector<foo> list1;
	std::generate_n(std::back_inserter(list1), 1600000, foo::make);
	auto list2 = list1;

	auto cmp = [](foo& f) {return f.info[0] & 1; };
	auto t0 = clock();
	stdext::unstable_remove_if(list1.begin(), list1.end(), cmp);
	auto t1 = clock();
	std::partition(list2.begin(), list2.end(), cmp);
	auto t2 = clock();

	std::cout << "partition: " << t2 - t1 << "\n";
	std::cout << "unstable: " << t1 - t0 << "\n";
}