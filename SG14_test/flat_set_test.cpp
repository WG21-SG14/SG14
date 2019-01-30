#include "SG14_test.h"
#include "flat_set.h"
#include <assert.h>
#include <deque>
#include <functional>

void sg14_test::flat_set_test()
{
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
