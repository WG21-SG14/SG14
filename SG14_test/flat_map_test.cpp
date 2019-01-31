#include "SG14_test.h"
#include "flat_map.h"
#include <assert.h>
#include <deque>
#include <functional>
#if __has_include(<memory_resource>)
#include <memory_resource>
#endif
#include <string>
#include <vector>

namespace {

struct AmbiguousEraseWidget {
    explicit AmbiguousEraseWidget(const char *s) : s_(s) {}

    template<class T>
    AmbiguousEraseWidget(T) : s_("notfound") {}

    friend bool operator<(const AmbiguousEraseWidget& a, const AmbiguousEraseWidget& b) {
        return a.s_ < b.s_;
    }

private:
    std::string s_;
};

struct InstrumentedWidget {
    static int move_ctors, copy_ctors;
    InstrumentedWidget(const char *s) : s_(s) {}
    InstrumentedWidget(InstrumentedWidget&& o) : s_(std::move(o.s_)) { move_ctors += 1; }
    InstrumentedWidget(const InstrumentedWidget& o) : s_(o.s_) { copy_ctors += 1; }
    InstrumentedWidget& operator=(InstrumentedWidget&&) = default;
    InstrumentedWidget& operator=(const InstrumentedWidget&) = default;

    friend bool operator<(const InstrumentedWidget& a, const InstrumentedWidget& b) {
        return a.s_ < b.s_;
    }

private:
    std::string s_;
};
int InstrumentedWidget::move_ctors = 0;
int InstrumentedWidget::copy_ctors = 0;

} // anonymous namespace

static void AmbiguousEraseTest()
{
    stdext::flat_map<AmbiguousEraseWidget, int> fs;
    fs.emplace("a", 1);
    fs.emplace("b", 2);
    fs.emplace("c", 3);
    assert(fs.size() == 3);
    fs.erase(AmbiguousEraseWidget("a"));  // calls erase(const Key&)
    assert(fs.size() == 2);
    fs.erase(fs.begin());                 // calls erase(iterator)
    assert(fs.size() == 1);
    fs.erase(fs.cbegin());                // calls erase(const_iterator)
    assert(fs.size() == 0);
}

static void ExtractDoesntSwapTest()
{
#if defined(__cpp_lib_memory_resource)
    // This test fails if extract() is implemented in terms of swap().
    {
        std::pmr::monotonic_buffer_resource mr;
        std::pmr::polymorphic_allocator<int> a(&mr);
        stdext::flat_map<int, int, std::less<>, std::pmr::vector<int>, std::pmr::vector<int>> fs({{1, 10}, {2, 20}}, a);
        auto ctrs = std::move(fs).extract();
        assert(ctrs.keys.get_allocator() == a);
        assert(ctrs.values.get_allocator() == a);
    }
#endif

    // Sanity-check with std::allocator, even though this can't fail.
    {
        std::allocator<int> a;
        stdext::flat_map<int, int, std::less<>, std::vector<int>, std::vector<int>> fs({{1, 10}, {2, 20}}, a);
        auto ctrs = std::move(fs).extract();
        assert(ctrs.keys.get_allocator() == a);
        assert(ctrs.values.get_allocator() == a);
    }
}

static void VectorBoolSanityTest()
{
    using FM = stdext::flat_map<bool, bool>;
    FM fm;
    auto it_inserted = fm.emplace(true, false);
    assert(it_inserted.second);
    auto it = it_inserted.first;
    assert(it == fm.begin());
    assert(it->first == true); assert(it->first);
    assert(it->second == false); assert(!it->second);
    it->second = false;
    assert(fm.size() == 1);
    it = fm.emplace_hint(it, false, true);
    assert(it == fm.begin());
    assert(it->first == false); assert(!it->first);
    assert(it->second == true); assert(it->second);
    it->second = true;
    assert(fm.size() == 2);
    auto count = fm.erase(false);
    assert(count == 1);
    assert(fm.size() == 1);
    it = fm.erase(fm.begin());
    assert(fm.empty());
    assert(it == fm.begin());
    assert(it == fm.end());
}

static void MoveOperationsPilferOwnership()
{
    using FS = stdext::flat_map<InstrumentedWidget, int>;
    InstrumentedWidget::move_ctors = 0;
    InstrumentedWidget::copy_ctors = 0;
    FS fs;
    fs.insert(std::make_pair(InstrumentedWidget("abc"), 1));
    assert(InstrumentedWidget::move_ctors == 3);
    assert(InstrumentedWidget::copy_ctors == 0);

    fs.emplace(InstrumentedWidget("def"), 1); fs.erase("def");  // poor man's reserve()
    InstrumentedWidget::copy_ctors = 0;
    InstrumentedWidget::move_ctors = 0;

    fs.emplace("def", 1);  // is still not directly emplaced; a temporary is created to find()
    assert(InstrumentedWidget::move_ctors == 1);
    assert(InstrumentedWidget::copy_ctors == 0);
    InstrumentedWidget::move_ctors = 0;

    FS fs2 = std::move(fs);  // should just transfer buffer ownership
    assert(InstrumentedWidget::move_ctors == 0);
    assert(InstrumentedWidget::copy_ctors == 0);

    fs = std::move(fs2);  // should just transfer buffer ownership
    assert(InstrumentedWidget::move_ctors == 0);
    assert(InstrumentedWidget::copy_ctors == 0);

    FS fs3(fs, std::allocator<InstrumentedWidget>());
    assert(InstrumentedWidget::move_ctors == 0);
    assert(InstrumentedWidget::copy_ctors == 2);
    InstrumentedWidget::copy_ctors = 0;

    FS fs4(std::move(fs), std::allocator<InstrumentedWidget>());  // should just transfer buffer ownership
    assert(InstrumentedWidget::move_ctors == 0);
    assert(InstrumentedWidget::copy_ctors == 0);
}

template<class FS>
static void ConstructionTest()
{
    static_assert(std::is_same<int, typename FS::key_type>::value, "");
    static_assert(std::is_convertible<const char*, typename FS::mapped_type>::value, "");
    using Compare = typename FS::key_compare;
    std::vector<int> keys = {1, 3, 5};
    std::vector<const char*> values = {"a", "b", "c"};
    std::vector<std::pair<int, const char*>> pairs = {
        {1, "a"},
        {3, "b"},
        {5, "c"},
    };
    if (true) {
        FS fs;  // default constructor
    }
    for (auto&& fs : {
        FS({{1, "a"}, {3, "b"}, {5, "c"}}),
        FS(pairs.begin(), pairs.end()),
        FS(pairs.rbegin(), pairs.rend()),
        FS(pairs, Compare()),
        FS({{1, "a"}, {3, "b"}, {5, "c"}}, Compare()),
        FS(pairs.begin(), pairs.end(), Compare()),
        FS(pairs.rbegin(), pairs.rend(), Compare()),
    }) {
        assert(std::is_sorted(fs.keys().begin(), fs.keys().end(), fs.key_comp()));
        assert(std::is_sorted(fs.begin(), fs.end(), fs.value_comp()));
        assert(fs.find(0) == fs.end());
        assert(fs.find(1) != fs.end());
        assert(fs.find(2) == fs.end());
        assert(fs.find(3) != fs.end());
        assert(fs.find(4) == fs.end());
        assert(fs.find(5) != fs.end());
        assert(fs.find(6) == fs.end());
    }
    if (std::is_sorted(keys.begin(), keys.end(), Compare())) {
        for (auto&& fs : {
            FS(stdext::sorted_unique, pairs),
            FS(stdext::sorted_unique, pairs.begin(), pairs.end()),
            FS(stdext::sorted_unique, {{1, "a"}, {3, "b"}, {5, "c"}}),
            FS(stdext::sorted_unique, pairs, Compare()),
            FS(stdext::sorted_unique, pairs.begin(), pairs.end(), Compare()),
            FS(stdext::sorted_unique, {{1, "a"}, {3, "b"}, {5, "c"}}, Compare()),
        }) {
            assert(std::is_sorted(fs.keys().begin(), fs.keys().end(), fs.key_comp()));
            assert(std::is_sorted(fs.begin(), fs.end(), fs.value_comp()));
        }
    }
}

template<class FS>
static void SpecialMemberTest()
{
    static_assert(std::is_default_constructible<FS>::value, "");
    static_assert(std::is_nothrow_move_constructible<FS>::value == std::is_nothrow_move_constructible<typename FS::key_container_type>::value && std::is_nothrow_move_constructible<typename FS::mapped_container_type>::value, "");
    static_assert(std::is_copy_constructible<FS>::value, "");
    static_assert(std::is_copy_assignable<FS>::value, "");
    static_assert(std::is_move_assignable<FS>::value, "");
    static_assert(std::is_nothrow_destructible<FS>::value, "");

    static_assert(std::is_default_constructible<typename FS::containers>::value, "");
    static_assert(std::is_nothrow_move_constructible<typename FS::containers>::value == std::is_nothrow_move_constructible<typename FS::key_container_type>::value && std::is_nothrow_move_constructible<typename FS::mapped_container_type>::value, "");
    static_assert(std::is_copy_constructible<typename FS::containers>::value, "");
    static_assert(std::is_copy_assignable<typename FS::containers>::value, "");
    static_assert(std::is_move_assignable<typename FS::containers>::value, "");
    static_assert(std::is_nothrow_destructible<typename FS::containers>::value, "");
}

void sg14_test::flat_map_test()
{
    AmbiguousEraseTest();
    ExtractDoesntSwapTest();
    VectorBoolSanityTest();
    MoveOperationsPilferOwnership();

    // Test the most basic flat_set.
    {
        using FS = stdext::flat_map<int, const char*>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a custom comparator.
    {
        using FS = stdext::flat_map<int, const char*, std::greater<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a transparent comparator.
    {
        using FS = stdext::flat_map<int, const char*, std::greater<>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a custom container.
    {
        using FS = stdext::flat_map<int, const char*, std::less<int>, std::deque<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

#if defined(__cpp_lib_memory_resource)
    // Test a pmr container.
    {
        using FS = stdext::flat_map<int, const char*, std::less<int>, std::pmr::vector<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a pmr container with uses-allocator construction!
    {
        using FS = stdext::flat_map<int, std::pmr::string, std::less<int>, std::pmr::vector<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }
#endif
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::flat_map_test();
}
#endif
