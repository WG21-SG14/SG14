#include "SG14_test.h"
#include "algorithm_ext.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <vector>

struct foo
{
    std::array<int,16> info;
    static foo make()
    {
        foo result;
        std::fill(result.info.begin(), result.info.end(), ::rand());
        return result;
    }
};


void sg14_test::unstable_remove_test()
{
    size_t test_runs = 200;

    auto makelist = []
    {
        std::vector<foo> list;
        std::generate_n(std::back_inserter(list), 30000, foo::make);
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
        auto t0 = std::chrono::high_resolution_clock::now();
        f(list);
        auto t1 = std::chrono::high_resolution_clock::now();
        return (t1 - t0).count();
    };

    using time_lambda_t = decltype(time(removefn));

    auto median = [](std::vector<time_lambda_t>& v)
    {
        auto b = v.begin();
        auto e = v.end();
        return *(b + ((e - b) / 2));
    };

    std::vector<time_lambda_t> partition;
    std::vector<time_lambda_t> unstable_remove_if;
    std::vector<time_lambda_t> remove_if;

    partition.reserve(test_runs);
    unstable_remove_if.reserve(test_runs);
    remove_if.reserve(test_runs);

    for (decltype(test_runs) i = 0; i < test_runs; ++i)
    {
        remove_if.push_back(time(removefn));
        unstable_remove_if.push_back(time(unstablefn));
        partition.push_back(time(partitionfn));
    }
    std::sort(partition.begin(), partition.end());
    std::sort(unstable_remove_if.begin(), unstable_remove_if.end());
    std::sort(remove_if.begin(), remove_if.end());

    auto partition_med = median(partition);
    auto unstable_med = median(unstable_remove_if);
    auto remove_med = median(remove_if);


    std::cout << "partition: " << partition_med << "\n";
    std::cout << "unstable: " << unstable_med << "\n";
    std::cout << "remove_if: " << remove_med << "\n";
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::unstable_remove_test();
}
#endif
