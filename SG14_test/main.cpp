#if defined(_MSC_VER)
#include <SDKDDKVer.h>
#endif

#include <stdio.h>

#include "SG14_test.h"

int main(int, char *[])
{
    sg14_test::flat_map_test();
    sg14_test::flat_set_test();
    sg14_test::inplace_function_test();
    sg14_test::plf_colony_test();
    sg14_test::ring_test();
    sg14_test::slot_map_test();
    sg14_test::uninitialized_test();
    sg14_test::unstable_remove_test();

    puts("tests completed");

    return 0;
}
