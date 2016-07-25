#include "../../../plf_bench.h"


int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	benchmark_range_general_use_percentage< plf::pointer_deque<small_struct> >(100, 100000, 1.1, 600, 1, 12, 5, true);

	benchmark_range_general_use_remove_if_percentage< plf::pointer_deque<small_struct_bool> >(100, 100000, 1.1, 600, 1, 12, 5, true);

	return 0;
}
