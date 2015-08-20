#if defined(_MSC_VER)
#include <SDKDDKVer.h>
#endif

#include <stdio.h>

#include "SG14_test.h"

int main(int, char *[])
{
	sg14_test::rolling_queue_test();
	sg14_test::unstable_remove_test();
	sg14_test::fixed_point_test();
    
    puts("tests completed");
	return 0;
}

