#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "SG14_test.h"

int _tmain(int argc, _TCHAR* argv[])
{
	sg14_test::rolling_queue_test();
	sg14_test::unstable_remove_test();
	return 0;
}

