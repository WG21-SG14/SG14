#include "plf_bench.h"


int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	benchmark_range_general_use< plf::indexed_vector<small_struct> >(10, 100000, 1.1, 3600, .5, 122, 119.5, true);

	benchmark_range_general_use_remove_if< plf::indexed_vector<small_struct_bool> >(10, 100000, 1.1, 3600, .5, 122, 119.5, true);

	benchmark_range_general_use_percentage< plf::indexed_vector<small_struct> >(10, 100000, 1.1, 600, 10, 11, 5, true);

	benchmark_range_general_use_remove_if_percentage< plf::indexed_vector<small_struct_bool> >(10, 100000, 1.1, 600, 10, 11, 5, true);

	return 0;
}
