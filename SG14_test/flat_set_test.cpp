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

void sg14_test::flat_set_test()
{
    AmbiguousEraseTest();

    // Test the most basic flat_set.
    {
        using FS = stdext::flat_set<int>;
        FS fs;
    }

    // Test a custom comparator.
    {
        using FS = stdext::flat_set<int, std::greater<int>>;
        FS fs;
    }

    // Test a transparent comparator.
    {
        using FS = stdext::flat_set<int, std::greater<>>;
        FS fs;
    }

    // Test a custom container.
    {
        using FS = stdext::flat_set<int, std::less<int>, std::deque<int>>;
        FS fs;
    }
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::flat_set_test();
}
#endif
