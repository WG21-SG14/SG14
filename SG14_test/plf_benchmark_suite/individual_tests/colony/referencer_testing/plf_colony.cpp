#include "../../../plf_bench.h"



int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	// Test 1%, 5%, 10% erasure/insertion of container content per minute (assuming 60fps) for a minute
	benchmark_range_referencer_small_percentage_colony(100, 100000, 1.5, 3600, .00028, 1, 1);
	benchmark_range_referencer_small_percentage_colony(100, 100000, 1.5, 3600, .00139, 1, 1);
	benchmark_range_referencer_small_percentage_colony(100, 100000, 1.5, 3600, .00278, 1, 1);

	// Test 1%, 5%, 10% erasure/insertion of container content per frame of gameplay (assuming 60fps) for a minute
	benchmark_range_referencer_percentage_colony(100, 100000, 1.5, 3600, 1, 4, 4);
	benchmark_range_referencer_percentage_colony(100, 100000, 1.5, 3600, 5, 7, 4);
	benchmark_range_referencer_percentage_colony(100, 100000, 1.5, 3600, 10, 11, 2);

	return 0;
}
