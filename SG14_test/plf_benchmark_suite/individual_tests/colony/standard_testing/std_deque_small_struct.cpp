#include "../../../plf_bench.h"


int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	benchmark_range< std::deque<small_struct> >(10, 100000, 1.1, 25, true);

	benchmark_erasure_if_range< std::deque<small_struct_bool> >(10, 100000, 1.1, 25, 25, 30, true);

	return 0;
}
