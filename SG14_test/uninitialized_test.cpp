#include "SG14_test.h"
#include <vector>
#include <array>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "algorithm_ext.h"
#include <cassert>
#include <memory>

namespace
{
	struct lifetest
	{
		static uint64_t construct;
		static uint64_t destruct;
		static uint64_t move;
		lifetest()
		{
			++construct;
		}
		lifetest(lifetest&& /*in*/) noexcept
		{
			++move;
		}
		~lifetest()
		{
			++destruct;
		}
		static void reset()
		{
			construct = 0;
			destruct = 0;
			move = 0;
		}

		// To avoid "unused argument" error/warning. 
		#ifdef NDEBUG
			static void test(uint64_t, uint64_t, uint64_t)
			{
				
			}
		#else
			static void test(uint64_t inconstruct, uint64_t indestruct, uint64_t inmove)
			{
				assert(construct == inconstruct);
				assert(destruct == indestruct);
				assert(move == inmove);
			}
		#endif
		
	};
	uint64_t lifetest::construct;
	uint64_t lifetest::destruct;
	uint64_t lifetest::move;

	void value()
	{
		for (auto n = 0; n < 256; ++n)
		{
			auto m = (lifetest*)malloc(sizeof(lifetest) * n);
			lifetest::test(0, 0, 0);
			stdext::uninitialized_value_construct(m, m + n);
			lifetest::test(n, 0, 0);
			stdext::destruct(m, m + n);
			lifetest::test(n, n, 0);
			free(m);
			lifetest::reset();
		}

		auto m = (int*)malloc(sizeof(int) * 5);
		stdext::uninitialized_value_construct(m, m + 5);
		assert(std::all_of(m, m + 5, [](int x) { return x == 0; }));
		free(m);
	};

	void def()
	{
		for (auto n = 0; n < 256; ++n)
		{
			auto mem1 = (lifetest*)malloc(sizeof(lifetest) * n);
			lifetest::test(0, 0, 0);
			stdext::uninitialized_default_construct(mem1, mem1 + n);
			lifetest::test(n, 0, 0);

			auto mem2 = (lifetest*)malloc(sizeof(lifetest) * n);
			stdext::uninitialized_move(mem1, mem1 + n, mem2);
			lifetest::test(n, 0, n);
			stdext::destruct(mem2, mem2 + n);
			lifetest::test(n, n, n);
			free(mem1);
			free(mem2);
			lifetest::reset();
		}
	}
}



void sg14_test::uninitialized_test()
{
	value();
	def();

}

#ifdef TEST_MAIN
int main()
{
    sg14_test::uninitialized_test();
}
#endif
