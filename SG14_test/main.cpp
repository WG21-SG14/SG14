#if defined(_MSC_VER)
#include <SDKDDKVer.h>
#include <tchar.h>
#endif

#include <stdio.h>

#include "SG14_test.h"

#if defined(_MSC_VER)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int, char *[])
#endif
{
	sg14_test::rolling_queue_test();
	sg14_test::unstable_remove_test();
	return 0;
}

