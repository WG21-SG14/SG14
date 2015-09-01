#include "SG14_test.h"
#include <cassert>
#include "hotset.h"

void hotsettest()
{

	hotset<int> foo(-1);
	assert(!foo.insert(0));
	assert(!foo.insert(1));
	assert(!foo.insert(234));

	assert(foo[0]);
	assert(foo[1]);
	assert(foo[234]);
	assert(!foo[282]);
	assert(!foo[-55]);
}