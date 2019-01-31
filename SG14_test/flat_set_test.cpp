#include "SG14_test.h"
#include "flat_set.h"
#include <assert.h>
#include <deque>
#include <functional>
#include <string>

namespace {

struct AmbiguousEraseWidget {
    using iterator = stdext::flat_set<AmbiguousEraseWidget>::iterator;
    using const_iterator = stdext::flat_set<AmbiguousEraseWidget>::const_iterator;

    explicit AmbiguousEraseWidget(const char *s) : s_(s) {}
    AmbiguousEraseWidget(iterator) : s_("notfound") {}
    AmbiguousEraseWidget(const_iterator) : s_("notfound") {}

    friend bool operator<(const AmbiguousEraseWidget& a, const AmbiguousEraseWidget& b) {
        return a.s_ < b.s_;
    }

private:
    std::string s_;
};

} // anonymous namespace

static void AmbiguousEraseTest()
{
    stdext::flat_set<AmbiguousEraseWidget> fs;
    fs.emplace("a");
    fs.emplace("b");
    fs.emplace("c");
    assert(fs.size() == 3);
    fs.erase(AmbiguousEraseWidget("a"));  // calls erase(const Key&)
    assert(fs.size() == 2);
    fs.erase(fs.begin());                 // calls erase(iterator)
    assert(fs.size() == 1);
    fs.erase(fs.cbegin());                // calls erase(const_iterator)
    assert(fs.size() == 0);
}

template<class FS>
static void ConstructionTest()
{
    static_assert(std::is_same<int, typename FS::key_type>::value, "");
    static_assert(std::is_same<int, typename FS::value_type>::value, "");
    using Compare = typename FS::key_compare;
    std::vector<int> vec = {1, 3, 5};
    if (true) {
        FS fs;  // default constructor
    }
    for (auto&& fs : {
        FS(vec),
        FS({1, 3, 5}),
        FS(vec.begin(), vec.end()),
        FS(vec.rbegin(), vec.rend()),
        FS(vec, Compare()),
        FS({1, 3, 5}, Compare()),
        FS(vec.begin(), vec.end(), Compare()),
        FS(vec.rbegin(), vec.rend(), Compare()),
    }) {
        auto cmp = fs.key_comp();
        assert(std::is_sorted(fs.begin(), fs.end(), cmp));
        assert(fs.find(0) == fs.end());
        assert(fs.find(1) != fs.end());
        assert(fs.find(2) == fs.end());
        assert(fs.find(3) != fs.end());
        assert(fs.find(4) == fs.end());
        assert(fs.find(5) != fs.end());
        assert(fs.find(6) == fs.end());
    }
    if (std::is_sorted(vec.begin(), vec.end(), Compare())) {
        for (auto&& fs : {
            FS(stdext::sorted_unique, vec),
            FS(stdext::sorted_unique, vec.begin(), vec.end()),
            FS(stdext::sorted_unique, {1, 3, 5}),
            FS(stdext::sorted_unique, vec, Compare()),
            FS(stdext::sorted_unique, vec.begin(), vec.end(), Compare()),
            FS(stdext::sorted_unique, {1, 3, 5}, Compare()),
        }) {
            auto cmp = fs.key_comp();
            assert(std::is_sorted(fs.begin(), fs.end(), cmp));
        }
    }
}

void sg14_test::flat_set_test()
{
    AmbiguousEraseTest();

    // Test the most basic flat_set.
    {
        using FS = stdext::flat_set<int>;
        ConstructionTest<FS>();
    }

    // Test a custom comparator.
    {
        using FS = stdext::flat_set<int, std::greater<int>>;
        ConstructionTest<FS>();
    }

    // Test a transparent comparator.
    {
        using FS = stdext::flat_set<int, std::greater<>>;
        ConstructionTest<FS>();
    }

    // Test a custom container.
    {
        using FS = stdext::flat_set<int, std::less<int>, std::deque<int>>;
        ConstructionTest<FS>();
    }

#if defined(__cpp_lib_memory_resource)
    // Test a pmr container.
    {
        using FS = stdext::flat_set<int, std::less<int>, std::pmr::vector<int>>;
        ConstructionTest<FS>();
    }
#endif
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::flat_set_test();
}
#endif
