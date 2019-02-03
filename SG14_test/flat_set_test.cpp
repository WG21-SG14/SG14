#include "SG14_test.h"
#include "flat_set.h"
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

static void ExtractDoesntSwapTest()
{
#if defined(__cpp_lib_memory_resource)
    // This test fails if extract() is implemented in terms of swap().
    {
        std::pmr::monotonic_buffer_resource mr;
        std::pmr::polymorphic_allocator<int> a(&mr);
        stdext::flat_set<int, std::less<>, std::pmr::vector<int>> fs({1, 2}, a);
        std::pmr::vector<int> v = std::move(fs).extract();
        assert(v.get_allocator() == a);
    }
#endif

    // Sanity-check with std::allocator, even though this can't fail.
    {
        std::allocator<int> a;
        stdext::flat_set<int, std::less<>, std::vector<int>> fs({1, 2}, a);
        std::vector<int> v = std::move(fs).extract();
        assert(v.get_allocator() == a);
    }
}

static void VectorBoolSanityTest()
{
    using FS = stdext::flat_set<bool>;
    FS fs;
    auto it_inserted = fs.emplace(true);
    assert(it_inserted.second);
    auto it = it_inserted.first;
    assert(it == fs.begin());
    assert(fs.size() == 1);
    it = fs.emplace_hint(it, false);
    assert(it == fs.begin());
    assert(fs.size() == 2);
    auto count = fs.erase(false);
    assert(count == 1);
    assert(fs.size() == 1);
    it = fs.erase(fs.begin());
    assert(fs.empty());
    assert(it == fs.begin());
    assert(it == fs.end());
}

static void MoveOperationsPilferOwnership()
{
    using FS = stdext::flat_set<InstrumentedWidget>;
    InstrumentedWidget::move_ctors = 0;
    InstrumentedWidget::copy_ctors = 0;
    FS fs;
    fs.insert(InstrumentedWidget("abc"));
    assert(InstrumentedWidget::move_ctors == 1);
    assert(InstrumentedWidget::copy_ctors == 0);

    fs.emplace(InstrumentedWidget("def")); fs.erase("def");  // poor man's reserve()
    InstrumentedWidget::copy_ctors = 0;
    InstrumentedWidget::move_ctors = 0;

    fs.emplace("def");  // is still not directly emplaced; a temporary is created to find()
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
    static_assert(std::is_same<int, typename FS::value_type>::value, "");
    using Compare = typename FS::key_compare;
    std::vector<int> vec = {1, 3, 5};
    if (true) {
        FS fs;  // default constructor
        fs = {1, 3, 5};  // assignment operator
        assert(std::is_sorted(fs.begin(), fs.end(), fs.key_comp()));
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

template<class FS>
static void SpecialMemberTest()
{
    static_assert(std::is_default_constructible<FS>::value, "");
    static_assert(std::is_nothrow_move_constructible<FS>::value == std::is_nothrow_move_constructible<typename FS::container_type>::value, "");
    static_assert(std::is_copy_constructible<FS>::value, "");
    static_assert(std::is_copy_assignable<FS>::value, "");
    static_assert(std::is_move_assignable<FS>::value, "");
    static_assert(std::is_nothrow_destructible<FS>::value, "");
}

void sg14_test::flat_set_test()
{
    AmbiguousEraseTest();
    ExtractDoesntSwapTest();
    VectorBoolSanityTest();
    MoveOperationsPilferOwnership();

    // Test the most basic flat_set.
    {
        using FS = stdext::flat_set<int>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a custom comparator.
    {
        using FS = stdext::flat_set<int, std::greater<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a transparent comparator.
    {
        using FS = stdext::flat_set<int, std::greater<>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

    // Test a custom container.
    {
        using FS = stdext::flat_set<int, std::less<int>, std::deque<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }

#if defined(__cpp_lib_memory_resource)
    // Test a pmr container.
    {
        using FS = stdext::flat_set<int, std::less<int>, std::pmr::vector<int>>;
        ConstructionTest<FS>();
        SpecialMemberTest<FS>();
    }
#endif
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::flat_set_test();
}
#endif
