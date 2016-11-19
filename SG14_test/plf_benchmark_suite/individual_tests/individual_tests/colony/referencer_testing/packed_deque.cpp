#include "../../../plf_bench.h"



 struct component1
 {
 	int numbers[100];
 	char a_string[50];
 	double unused_number;
 	double number;
 	double *empty_field_1;
 	double *empty_field_2;
 	unsigned int empty_field_3;
 	unsigned int empty_field_4;
 
 	component1(): unused_number(0), number(0), empty_field_1(NULL), empty_field_2(NULL), empty_field_3(0), empty_field_4(0) {};
 };
 
 
 
 
 struct component2
 {
 	double unused_number;
 	double number;
 	unsigned int empty_field_3;
 	unsigned int empty_field_4;
 
 	component2(): unused_number(0), number(0), empty_field_3(0), empty_field_4(0)  {};
 };
 
 
 
 struct referencer; // forward declaration
 
 
 struct entity
 {
 	plf::packed_deque<component1>::handle comp1_handle;
 	plf::packed_deque<component2>::handle comp2_handle;
 	plf::packed_deque<referencer>::handle referencer_handle;
 
 	double *empty_field_1;
 	double unused_number;
 	unsigned int empty_field2;
 	double *empty_field_3;
 	double number;
 	unsigned int empty_field_4;
 
 	entity(): empty_field_1(NULL), unused_number(0), empty_field2(0), empty_field_3(NULL), number(0), empty_field_4(0) {};
 };
 
 
 
 
 struct referencer
 {
 	plf::packed_deque<entity>::handle entity_handle;
 	unsigned int x, y, w, h;
 
 	referencer(): x(0), y(0), w(0), h(0) {};
 };
 
 


  
 
 inline PLF_FORCE_INLINE void benchmark_referencer_percentage_packed_deque(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int erasure_percentage)
 {
 	assert (number_of_elements > 1);
 
 	const unsigned int total_number_of_insertions = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
 	const unsigned int erasure_percent_expanded = static_cast<unsigned int>((static_cast<double>(erasure_percentage) * 1.28) + 0.5);
 
 	double total = 0;
 	unsigned int end_approximate_memory_use = 0;
 	plf::nanotimer full_time;
 	full_time.start();
 
 	entity temp_entity;
 	component1 temp_comp1;
 	component2 temp_comp2;
 	referencer temp_referencer;
 
 
 	for (unsigned int run_number = 0; run_number != (number_of_runs / 10) + 1; ++run_number)
 	{
 		plf::packed_deque<entity> entities;
 		plf::packed_deque<component1> comp1s;
 		plf::packed_deque<component2> comp2s;
 		plf::packed_deque<referencer> collisions;
 
 		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
 		{
 			temp_entity.number = xor_rand() & 255;
 			temp_comp1.number = xor_rand() & 255;
 			temp_comp2.number = xor_rand() & 255;
 
 			temp_referencer.entity_handle = entities.insert(temp_entity);
 			temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 			temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 			temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 		}
 
 
 		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
 		{
 			for (plf::packed_deque<entity>::iterator current_entity_package = entities.begin(); current_entity_package != entities.end();)
 			{
 				if ((xor_rand() & 255) >= erasure_percent_expanded)
 				{
 					total += current_entity_package->element.number + current_entity_package->element.comp1_handle->number + current_entity_package->element.comp2_handle->number;
 					++current_entity_package;
 				}
 				else
 				{
 					comp1s.erase(current_entity_package->element.comp1_handle);
 					comp2s.erase(current_entity_package->element.comp2_handle);
 					collisions.erase(current_entity_package->element.referencer_handle);
 					current_entity_package = entities.erase(current_entity_package);
 				}
 			}
 
 			for (plf::packed_deque<referencer>::iterator current_referencer_package = collisions.begin(); current_referencer_package != collisions.end();)
 			{
 				if ((xor_rand() & 255) >= erasure_percent_expanded)
 				{
 					++current_referencer_package;
 				}
 				else
 				{
 					comp1s.erase(current_referencer_package->element.entity_handle->comp1_handle);
 					comp2s.erase(current_referencer_package->element.entity_handle->comp2_handle);
 					entities.erase(current_referencer_package->element.entity_handle);
 					current_referencer_package = collisions.erase(current_referencer_package);
 				}
 			}
 
 
 			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
 			{
 				temp_entity.number = xor_rand() & 255;
 				temp_comp1.number = xor_rand() & 255;
 				temp_comp2.number = xor_rand() & 255;
 
 				temp_referencer.entity_handle = entities.insert(temp_entity);
 				temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 				temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 				temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 			}
 		}
 
 		end_approximate_memory_use += static_cast<unsigned int>(entities.approximate_memory_use() + comp1s.approximate_memory_use() + comp2s.approximate_memory_use() + collisions.approximate_memory_use());
 	}
 
 	end_approximate_memory_use /= (number_of_runs / 10) + 1;
 
 	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
 
 
 	full_time.start();
 
 	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
 	{
 		plf::packed_deque<entity> entities;
 		plf::packed_deque<component1> comp1s;
 		plf::packed_deque<component2> comp2s;
 		plf::packed_deque<referencer> collisions;
 
 		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
 		{
 			temp_entity.number = xor_rand() & 255;
 			temp_comp1.number = xor_rand() & 255;
 			temp_comp2.number = xor_rand() & 255;
 
 			temp_referencer.entity_handle = entities.insert(temp_entity);
 			temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 			temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 			temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 		}
 
 
 		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
 		{
 			for (plf::packed_deque<entity>::iterator current_entity_package = entities.begin(); current_entity_package != entities.end();)
 			{
 				if ((xor_rand() & 255) >= erasure_percent_expanded)
 				{
 					total += current_entity_package->element.number + current_entity_package->element.comp1_handle->number + current_entity_package->element.comp2_handle->number;
 					++current_entity_package;
 				}
 				else
 				{
 					comp1s.erase(current_entity_package->element.comp1_handle);
 					comp2s.erase(current_entity_package->element.comp2_handle);
 					collisions.erase(current_entity_package->element.referencer_handle);
 					current_entity_package = entities.erase(current_entity_package);
 				}
 			}
 
 
 			for (plf::packed_deque<referencer>::iterator current_referencer_package = collisions.begin(); current_referencer_package != collisions.end();)
 			{
 				if ((xor_rand() & 255) >= erasure_percent_expanded)
 				{
 					++current_referencer_package;
 				}
 				else
 				{
 					comp1s.erase(current_referencer_package->element.entity_handle->comp1_handle);
 					comp2s.erase(current_referencer_package->element.entity_handle->comp2_handle);
 					entities.erase(current_referencer_package->element.entity_handle);
 					current_referencer_package = collisions.erase(current_referencer_package);
 				}
 			}
 
 
 			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
 			{
 				temp_entity.number = xor_rand() & 255;
 				temp_comp1.number = xor_rand() & 255;
 				temp_comp2.number = xor_rand() & 255;
 
 				temp_referencer.entity_handle = entities.insert(temp_entity);
 				temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 				temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 				temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 			}
 		}
 	}
 
 	const double total_time = full_time.get_elapsed_us();
 
 	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
 	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
 }
 
 
 
 
 void benchmark_range_referencer_percentage_packed_deque(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_erasure_percentage, const unsigned int max_erasure_percentage, const unsigned int erasure_addition)
 {
 	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
 	{
 		std::cout << "Erasure percentage: " << erasure_percentage << std::endl << std::endl;
 
 		std::cout << "Number of elements, Total time, Memory Usage" << std::endl;
 
 		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
 		{
 			std::cout << number_of_elements;
 			benchmark_referencer_percentage_packed_deque(number_of_elements, 100000 / number_of_elements, number_of_cycles, erasure_percentage);
 		}
 
 		std::cout << "\n,,,\n,,,\n";
 	}
 }
 
 
 
 
 
 
 inline PLF_FORCE_INLINE void benchmark_referencer_small_percentage_packed_deque(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const double erasure_percentage)
 {
 	assert (number_of_elements > 1);
 
 	const unsigned int comparison_percentage = static_cast<unsigned int>((erasure_percentage * 167772.16) + 0.5);
 
 	unsigned int num_erasures;
 	unsigned int end_approximate_memory_use = 0;
 	double total = 0;
 	plf::nanotimer full_time;
 	full_time.start();
 
 	entity temp_entity;
 	component1 temp_comp1;
 	component2 temp_comp2;
 	referencer temp_referencer;
 
 
 	for (unsigned int run_number = 0; run_number != (number_of_runs / 10) + 1; ++run_number)
 	{
 		plf::packed_deque<entity> entities;
 		plf::packed_deque<component1> comp1s;
 		plf::packed_deque<component2> comp2s;
 		plf::packed_deque<referencer> collisions;
 
 		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
 		{
 			temp_entity.number = xor_rand() & 255;
 			temp_comp1.number = xor_rand() & 255;
 			temp_comp2.number = xor_rand() & 255;
 
 			temp_referencer.entity_handle = entities.insert(temp_entity);
 			temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 			temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 			temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 		}
 
 
 		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
 		{
 			num_erasures = 0;
 
 			for (plf::packed_deque<entity>::iterator current_entity_package = entities.begin(); current_entity_package != entities.end();)
 			{
 				if ((xor_rand() & 16777215) >= comparison_percentage)
 				{
 					total += current_entity_package->element.number + current_entity_package->element.comp1_handle->number + current_entity_package->element.comp2_handle->number;
 					++current_entity_package;
 				}
 				else
 				{
 					comp1s.erase(current_entity_package->element.comp1_handle);
 					comp2s.erase(current_entity_package->element.comp2_handle);
 					collisions.erase(current_entity_package->element.referencer_handle);
 					current_entity_package = entities.erase(current_entity_package);
 					++num_erasures;
 				}
 			}
 
 			for (plf::packed_deque<referencer>::iterator current_referencer_package = collisions.begin(); current_referencer_package != collisions.end();)
 			{
 				if ((xor_rand() & 16777215) >= comparison_percentage)
 				{
 					++current_referencer_package;
 				}
 				else
 				{
 					comp1s.erase(current_referencer_package->element.entity_handle->comp1_handle);
 					comp2s.erase(current_referencer_package->element.entity_handle->comp2_handle);
 					entities.erase(current_referencer_package->element.entity_handle);
 					current_referencer_package = collisions.erase(current_referencer_package);
 					++num_erasures;
 				}
 			}
 
 
 			for (unsigned int number_of_insertions = 0; number_of_insertions != num_erasures; ++number_of_insertions)
 			{
 				temp_entity.number = xor_rand() & 255;
 				temp_comp1.number = xor_rand() & 255;
 				temp_comp2.number = xor_rand() & 255;
 
 				temp_referencer.entity_handle = entities.insert(temp_entity);
 				temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 				temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 				temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 			}
 		}
 
 		end_approximate_memory_use += static_cast<unsigned int>(entities.approximate_memory_use() + comp1s.approximate_memory_use() + comp2s.approximate_memory_use() + collisions.approximate_memory_use());
 	}
 
 	end_approximate_memory_use /= (number_of_runs / 10) + 1;
 
 	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
 
 
 	full_time.start();
 
 	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
 	{
 		plf::packed_deque<entity> entities;
 		plf::packed_deque<component1> comp1s;
 		plf::packed_deque<component2> comp2s;
 		plf::packed_deque<referencer> collisions;
 
 		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
 		{
 			temp_entity.number = xor_rand() & 255;
 			temp_comp1.number = xor_rand() & 255;
 			temp_comp2.number = xor_rand() & 255;
 
 			temp_referencer.entity_handle = entities.insert(temp_entity);
 			temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 			temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 			temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 		}
 
 
 		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
 		{
 			num_erasures = 0;
 
 			for (plf::packed_deque<entity>::iterator current_entity_package = entities.begin(); current_entity_package != entities.end();)
 			{
 				if ((xor_rand() & 16777215) >= comparison_percentage)
 				{
 					total += current_entity_package->element.number + current_entity_package->element.comp1_handle->number + current_entity_package->element.comp2_handle->number;
 					++current_entity_package;
 				}
 				else
 				{
 					comp1s.erase(current_entity_package->element.comp1_handle);
 					comp2s.erase(current_entity_package->element.comp2_handle);
 					collisions.erase(current_entity_package->element.referencer_handle);
 					current_entity_package = entities.erase(current_entity_package);
 					++num_erasures;
 				}
 			}
 
 			for (plf::packed_deque<referencer>::iterator current_referencer_package = collisions.begin(); current_referencer_package != collisions.end();)
 			{
 				if ((xor_rand() & 16777215) >= comparison_percentage)
 				{
 					++current_referencer_package;
 				}
 				else
 				{
 					comp1s.erase(current_referencer_package->element.entity_handle->comp1_handle);
 					comp2s.erase(current_referencer_package->element.entity_handle->comp2_handle);
 					entities.erase(current_referencer_package->element.entity_handle);
 					current_referencer_package = collisions.erase(current_referencer_package);
 					++num_erasures;
 				}
 			}
 
 
 			for (unsigned int number_of_insertions = 0; number_of_insertions != num_erasures; ++number_of_insertions)
 			{
 				temp_entity.number = xor_rand() & 255;
 				temp_comp1.number = xor_rand() & 255;
 				temp_comp2.number = xor_rand() & 255;
 
 				temp_referencer.entity_handle = entities.insert(temp_entity);
 				temp_referencer.entity_handle->comp1_handle = comp1s.insert(temp_comp1);
 				temp_referencer.entity_handle->comp2_handle = comp2s.insert(temp_comp2);
 				temp_referencer.entity_handle->referencer_handle = collisions.insert(temp_referencer);
 			}
 		}
 	}
 
 	const double total_time = full_time.get_elapsed_us();
 
 	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
 	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
 }
 
 
 
 
 void benchmark_range_referencer_small_percentage_packed_deque(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const double initial_erasure_percentage, const double max_erasure_percentage, const double erasure_addition)
 {
 	for (double erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
 	{
 		std::cout << "Erasure percentage: " << erasure_percentage << std::endl << std::endl;
 		std::cout << "Number of elements, Total time, Memory Usage" << std::endl;
 
 		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
 		{
 			std::cout << number_of_elements;
 
 			benchmark_referencer_small_percentage_packed_deque(number_of_elements, 100000 / number_of_elements, number_of_cycles, erasure_percentage);
 		}
 
 		std::cout << "\n,,,\n,,,\n";
 	}
 }
 
 


int main(int argc, char **argv)
{
	output_to_csv_file(argv[0]);

	// Test 1%, 5%, 10% erasure/insertion of container content per minute (assuming 60fps) for a minute
	benchmark_range_referencer_small_percentage_packed_deque(100, 100000, 1.5, 3600, .00028, 1, 1);
	benchmark_range_referencer_small_percentage_packed_deque(100, 100000, 1.5, 3600, .00139, 1, 1);
	benchmark_range_referencer_small_percentage_packed_deque(100, 100000, 1.5, 3600, .00278, 1, 1);

	// Test 1%, 5%, 10% erasure/insertion of container content per frame of gameplay (assuming 60fps) for a minute
	benchmark_range_referencer_percentage_packed_deque(100, 100000, 1.5, 3600, 1, 4, 4);
	benchmark_range_referencer_percentage_packed_deque(100, 100000, 1.5, 3600, 5, 7, 4);
	benchmark_range_referencer_percentage_packed_deque(100, 100000, 1.5, 3600, 10, 11, 2);

	return 0;
}
