#include "SG14_test.h"
#include <vector>
#include <array>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "algorithm_ext.h"
#include <cassert>
#include <memory>

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


void sg14_test::unstable_remove_test()
{
#if 0
	auto makelist = []
	{
		std::vector<foo> list;
		std::generate_n(std::back_inserter(list), 3000000, foo::make);
		return list;
	};

	auto cmp = [](foo& f) {return f.info[0] & 1; };

	auto partitionfn = [&](std::vector<foo>& f)
	{
		stdext::partition(f.begin(), f.end(), cmp);
	};
	auto unstablefn = [&](std::vector<foo>& f)
	{
		stdext::unstable_remove_if(f.begin(), f.end(), cmp);
	};
	auto removefn = [&](std::vector<foo>& f)
	{
		stdext::remove_if(f.begin(), f.end(), cmp);
	};
	auto time = [&](auto&& f)
	{
		auto list = makelist();
		auto t0 = clock();
		f(list);
		auto t1 = clock();
		return t1 - t0;
	};

	auto partition = time(partitionfn);
	auto unstable_remove_if = time(unstablefn);
	auto remove_if = time(removefn);

	std::cout << "partition: " << partition << "\n";
	std::cout << "unstable: " << unstable_remove_if << "\n";
	std::cout << "remove_if: " << remove_if << "\n";
	std::cin.get();
#endif
}
