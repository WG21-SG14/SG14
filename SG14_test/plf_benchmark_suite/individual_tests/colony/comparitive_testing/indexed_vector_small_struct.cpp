#include "plf_bench.h"


int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	benchmark_erasure_range< plf::indexed_vector<small_struct> >(10, 100000, 1.1, 0, 25, 70, true);

	// Check reinsertion for 75% erasure only:
	benchmark_erasure_range_reinsertion< plf::indexed_vector<small_struct> >(10, 100000, 1.1, 75, 25, 95, true);

	benchmark_erasure_if_range< plf::indexed_vector<small_struct_bool> >(10, 100000, 1.1, 0, 25, 70, true);
	benchmark_erasure_if_range_reinsertion< plf::indexed_vector<small_struct_bool> >(10, 100000, 1.1, 75, 25, 95, true);

	return 0;
}
