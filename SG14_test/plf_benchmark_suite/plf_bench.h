#ifndef PLF_BENCH_H
#define PLF_BENCH_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <stack>
#include <cstdlib> // srand
#include <cstdio> // freopen, sprintf
#include <ctime> // timer for srand

#include "plf_colony.h"
#include "plf_nanotimer.h"
#include "plf_indexed_vector.h"
#include "plf_pointer_deque.h"
#include "plf_booled_deque.h"


// Defines:

#if defined(_MSC_VER)
	#define PLF_FORCE_INLINE __forceinline
#elif defined(__GNUC__) && !defined(__clang__) // If compiler is GCC/G++
	#define PLF_FORCE_INLINE __attribute__((always_inline))
#else
	#define PLF_FORCE_INLINE
#endif



// Datatypes:

struct small_struct
{
	double *empty_field_1;
	double unused_number;
	unsigned int empty_field2;
	double *empty_field_3;
	double number;
	unsigned int empty_field4;
	
	// This function is required for testing std::multiset:
	inline bool operator < (const small_struct &rh) const
	{
		return rh.number < this->number;
	}
	
	small_struct(const unsigned int num): number(num) {};
};


struct small_struct_bool
{
	double *empty_field_1;
	double unused_number;
	unsigned int empty_field2;
	double *empty_field_3;
	double number;
	unsigned int empty_field4;
	bool erased;
	
	// This function is required for testing std::multiset:
	inline bool operator < (const small_struct &rh) const
	{
		return rh.number < this->number;
	}
	
	small_struct_bool(const unsigned int num): number(num), erased(false) {};
};


	
struct large_struct
{
	int numbers[100];
	char a_string[50];
	double unused_number;
	double number;
	double *empty_field_1;
	double *empty_field_2;
	unsigned int empty_field3;
	unsigned int empty_field4;

	// This function is required for testing std::multiset:
	inline bool operator < (const large_struct &rh) const
	{
		return rh.number < this->number;
	}
	
	large_struct(const unsigned int num): number(num) {};
};




// Reserve templates:

template<class container_type>
inline PLF_FORCE_INLINE void container_reserve(container_type &container, unsigned int amount)
{} // General case: does nothing


template<class container_contents>
inline PLF_FORCE_INLINE void container_reserve(std::vector<container_contents> &container, unsigned int amount)
{
	container.reserve(amount);
}


template<class container_contents>
inline PLF_FORCE_INLINE void container_reserve(plf::stack<container_contents> &container, unsigned int amount)
{
	container.reserve(amount);
}


template<class container_contents>
inline PLF_FORCE_INLINE void container_reserve(plf::colony<container_contents> &container, unsigned int amount)
{
	container.reserve(amount);
}




// INSERT FUNCTIONS:

// default:
template <template <typename,typename> class container_type, typename container_contents, typename allocator_type>
inline PLF_FORCE_INLINE void container_insert(container_type<container_contents, allocator_type> &container)
{
	container.push_back(container_contents(rand() % 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::list<container_contents> &container)
{
	container.push_front(container_contents(rand() % 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::map<unsigned int, container_contents> &container)
{
	container.insert(std::make_pair(static_cast<unsigned int>(container.size()), container_contents(rand() % 255)));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::multiset<container_contents> &container)
{
	container.insert(container_contents(rand() % 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::stack<container_contents> &container)
{
	container.push(container_contents(rand() % 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::stack<container_contents> &container)
{
	container.push(container_contents(rand() % 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::colony<container_contents> &container)
{
	container.insert(container_contents(rand() % 255));
}



// Actual tests:




// ERASURE FUNCTIONS:

template <class container_type>
inline PLF_FORCE_INLINE void container_erase(container_type &container, typename container_type::iterator &the_iterator)
{
	the_iterator = container.erase(the_iterator);
}



template <class container_contents>
inline PLF_FORCE_INLINE void container_erase(std::map<unsigned int, container_contents> &container, typename std::map<unsigned int, container_contents>::iterator &the_iterator)
{
	// This procedure necessary in pre-C++11:
	typename std::map<unsigned int, container_contents>::iterator temp_iterator = the_iterator;
	++the_iterator;
	container.erase(temp_iterator);
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_erase(std::multiset<container_contents> &container, typename std::multiset<container_contents>::iterator &the_iterator)
{
	// This procedure necessary in pre-C++11:
	typename std::multiset<container_contents>::iterator temp_iterator = the_iterator;
	++the_iterator;
	container.erase(temp_iterator);
}



inline PLF_FORCE_INLINE void container_erase(std::vector<small_struct_bool> &container, std::vector<small_struct_bool>::iterator &the_iterator)
{
	the_iterator->erased = true;
	++the_iterator;
}


inline PLF_FORCE_INLINE void container_erase(std::deque<small_struct_bool> &container, std::deque<small_struct_bool>::iterator &the_iterator)
{
	the_iterator->erased = true;
	++the_iterator;
}


inline PLF_FORCE_INLINE void container_erase(plf::pointer_deque<small_struct_bool> &container, plf::pointer_deque<small_struct_bool>::iterator &the_iterator)
{
	(*the_iterator)->erased = true;
	++the_iterator;
}


inline PLF_FORCE_INLINE void container_erase(plf::indexed_vector<small_struct_bool> &container, plf::indexed_vector<small_struct_bool>::iterator &the_iterator)
{
	(container.set(the_iterator)).erased = true;
	++the_iterator;
}





// ITERATION FUNCTIONS:

// helper functions:

template <template <typename,typename> class container_type, typename container_contents, typename allocator_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<container_contents, allocator_type> &the_container, const typename container_type<container_contents, allocator_type>::iterator &the_iterator)
{
	return static_cast<unsigned int>(*the_iterator);
}


template <template <typename,typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<small_struct, std::allocator<small_struct>> &the_container, const typename container_type<small_struct, std::allocator<small_struct>>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<large_struct, std::allocator<large_struct>> &the_container, const typename container_type<large_struct, std::allocator<large_struct>>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <class container_contents>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::multiset<container_contents> &the_container, const typename std::multiset<container_contents>::iterator &the_iterator)
{
	return static_cast<unsigned int>(*the_iterator);
}


template <>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::multiset<small_struct> &the_container, const typename std::multiset<small_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::multiset<large_struct> &the_container, const typename std::multiset<large_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <class container_contents>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::map<unsigned int, container_contents> &the_container, const typename std::map<unsigned int, container_contents>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->second);
}


template <>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::map<unsigned int, small_struct> &the_container, const std::map<unsigned int, small_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->second.number);
}


template <>
inline PLF_FORCE_INLINE unsigned int container_iterate(const std::map<unsigned int, large_struct> &the_container, const std::map<unsigned int, large_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->second.number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const std::vector<small_struct_bool> &the_container, const std::vector<small_struct_bool>::iterator &the_iterator)
{
	return (the_iterator->erased) ? 0 : static_cast<unsigned int>(the_iterator->number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const std::deque<small_struct_bool> &the_container, const std::deque<small_struct_bool>::iterator &the_iterator)
{
	return (the_iterator->erased) ? 0 : static_cast<unsigned int>(the_iterator->number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::pointer_deque<small_struct> &the_container, const plf::pointer_deque<small_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>((*the_iterator)->number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::pointer_deque<small_struct_bool> &the_container, const plf::pointer_deque<small_struct_bool>::iterator &the_iterator)
{
	return static_cast<unsigned int>((*the_iterator)->number);
}



inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::indexed_vector<small_struct> &the_container, const plf::indexed_vector<small_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>((the_container.get(the_iterator)).number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::indexed_vector<small_struct_bool> &the_container, const plf::indexed_vector<small_struct_bool>::iterator &the_iterator)
{
	return static_cast<unsigned int>((the_container.get(the_iterator)).number);
}






// Actual tests:

template<class container_type>
inline PLF_FORCE_INLINE void iteration_test(container_type &container, const unsigned int number_of_runs, const bool output_csv = false)
{
	double total_time = 0, total = 0;
	plf::nanotimer timer;
	
	timer.start();
	
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end(); ++current_element)
		{
			total += container_iterate(container, current_element);
		}
	}

	total_time = timer.get_elapsed_us();

	std::cerr << "Dump time and total: " << total_time << ", " << total << std::endl;
	total_time = 0;

	timer.start();
	
	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end(); ++current_element)
		{
			total += container_iterate(container, current_element);
		}
	}

	total_time = timer.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / number_of_runs);
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / number_of_runs) << "us" << std::endl;
	}
	
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or loops will be removed) - no kidding, gcc will actually do this with std::vector.
}




// For stack testing:
template <class container_contents>
inline PLF_FORCE_INLINE void container_back_pop(plf::stack<container_contents> &container, double &total)
{
	total += container.top();
	container.pop();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(plf::stack<small_struct> &container, double &total)
{
	total += container.top().number;
	container.pop();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(plf::stack<large_struct> &container, double &total)
{
	total += container.top().number;
	container.pop();
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_back_pop(std::stack<container_contents> &container, double &total)
{
	total += container.top();
	container.pop();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(std::stack<small_struct> &container, double &total)
{
	total += container.top().number;
	container.pop();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(std::stack<large_struct> &container, double &total)
{
	total += container.top().number;
	container.pop();
}



template <class container_contents>
inline PLF_FORCE_INLINE void container_back_pop(std::vector<container_contents> &container, double &total)
{
	total += container.back();
	container.pop_back();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(std::vector<small_struct> &container, double &total)
{
	total += container.back().number;
	container.pop_back();
}


template <>
inline PLF_FORCE_INLINE void container_back_pop(std::vector<large_struct> &container, double &total)
{
	total += container.back().number;
	container.pop_back();
}




template<class container_type>
inline PLF_FORCE_INLINE void benchmark_stack(const unsigned int number_of_elements, const unsigned int number_of_runs, const bool output_csv = false, const bool reserve = false)
{
	double push_time = 0, pop_back_time = 0, total = 0;
	plf::nanotimer timer, timer2;
	
	timer2.start();
	
	// Warm up cache:
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		timer.start();
	
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		push_time += timer.get_elapsed_us();

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_back_pop(container, total);
		}
	}

	// Doing this because pop times are generally too short and lead to chaotic timing results
	// remainder of total time for loop once push and container creation are accounted for must be pop/back time:
	pop_back_time = timer2.get_elapsed_us() - push_time; 

	std::cerr << "Dump total and time: " << total << ", " << push_time << ", " << pop_back_time << std::endl;
	push_time = 0;
	pop_back_time = 0;
	total = 0;
	
	
	timer2.start();
	
	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		timer.start();
	
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		push_time += timer.get_elapsed_us();

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_back_pop(container, total);
		}
	}

	// Doing this because pop times are generally too short and lead to chaotic timing results
	// remainder of total time for loop once push and container creation are accounted for must be pop/back time:
	pop_back_time = timer2.get_elapsed_us() - push_time; 

	
	if (output_csv)
	{
		std::cout << ", " << (push_time / number_of_runs) << ", " << (pop_back_time / number_of_runs) << ", " << ((pop_back_time + push_time) / number_of_runs) << std::endl;
	}
	else
	{
		std::cout << "Push " << number_of_elements << " elements: " << (push_time / number_of_runs) << "us" << std::endl;
		std::cout << "Pop and sum: " << (pop_back_time / number_of_runs) << "us" << std::endl;
		std::cout << "Total time: " << ((pop_back_time + push_time) / number_of_runs) << "us" << "\n\n\n";
	}

	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
}





// Combinations:

template <class container_type>
inline PLF_FORCE_INLINE void benchmark(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 100.0f));
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}
		
		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		erase_time += erase_timer.get_elapsed_us();
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;
	insert_time = 0; erase_time = 0; total_size = 0;



	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;


	if (output_csv)
	{
		std::cout << ", " << (insert_time / number_of_runs) << ", ";

		if (erasure_percentage != 0)
		{
			std::cout << (erase_time / number_of_runs);
		}
	}
	else
	{
		std::cout << "Insert " << number_of_elements << " elements: " << (insert_time / number_of_runs) << "us" << std::endl;
		
		if (erasure_percentage != 0)
		{
			std::cout << "Randomly erase " << erasure_percentage << "% of elements: " << (erase_time / number_of_runs) << "us" << std::endl;
		}
	}


	// Do whole process one more time so we have something for iteration test, without copying/moving containers:
	container_type container;

	{
		// Insert test
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		// Erase test
		if (erasure_percentage != 0)
		{
			number_of_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
				else
				{
					++current_element;
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
					{
						container_erase(container, current_element);

						if (++number_of_erasures == erasure_limit)
						{
							break;
						}
					}
				}
			}
		}
	}


	iteration_test(container, number_of_runs * 1000, output_csv);

	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_remove_if(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 100.0f));
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}
		
		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}
		
		container.remove_if();

		erase_time += erase_timer.get_elapsed_us();
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;
	insert_time = 0; erase_time = 0; total_size = 0;



	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		container.remove_if();

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;


	if (output_csv)
	{
		std::cout << ", " << (insert_time / number_of_runs) << ", ";

		if (erasure_percentage != 0)
		{
			std::cout << (erase_time / number_of_runs);
		}
	}
	else
	{
		std::cout << "Insert " << number_of_elements << " elements: " << (insert_time / number_of_runs) << "us" << std::endl;
		
		if (erasure_percentage != 0)
		{
			std::cout << "Randomly erase " << erasure_percentage << "% of elements: " << (erase_time / number_of_runs) << "us" << std::endl;
		}
	}


	// Do whole process one more time so we have something for iteration test, without copying/moving containers:
	container_type container;

	{
		// Insert test
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		// Erase test
		if (erasure_percentage != 0)
		{
			number_of_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
				else
				{
					++current_element;
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
					{
						container_erase(container, current_element);

						if (++number_of_erasures == erasure_limit)
						{
							break;
						}
					}
				}
			}
		}

		container.remove_if();
	}


	iteration_test(container, number_of_runs * 1000, output_csv);

	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const double chance_of_change, const bool output_csv = false, const bool reserve = false)
{
	assert (number_of_elements > 1);

	double total = 0;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if ((rand() % static_cast<unsigned int>(chance_of_change * static_cast<double>(number_of_elements))) == 0)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			if (chance_of_change < 1)
			{
				for (unsigned int number_of_insertions = 0; number_of_insertions != static_cast<unsigned int>(1.0 / chance_of_change); ++number_of_insertions)
				{
					container_insert(container);
				}
			}
			else
			{
				if (rand() % static_cast<unsigned int>(chance_of_change) == 0 || container.size() == 0)
				{
					container_insert(container);
				}
			}
		}
	}

	std::cerr << "Dump total and time: " << total << full_time.get_elapsed_us() << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if ((rand() % static_cast<unsigned int>(chance_of_change * static_cast<double>(number_of_elements))) == 0)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			if (chance_of_change < 1)
			{
				for (unsigned int number_of_insertions = 0; number_of_insertions != static_cast<unsigned int>(1.0 / chance_of_change); ++number_of_insertions)
				{
					container_insert(container);
				}
			}
			else
			{
				if (rand() % static_cast<unsigned int>(chance_of_change) == 0 || container.size() == 0)
				{
					container_insert(container);
				}
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / static_cast<double>(number_of_runs));
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / static_cast<double>(number_of_runs)) << "us" << std::endl;
	}
	
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}

}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (number_of_elements > 1);

	const unsigned int total_number_of_insertions = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100));

	double total = 0;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	std::cerr << "Dump total and time: " << total << full_time.get_elapsed_us() << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / static_cast<double>(number_of_runs));
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / static_cast<double>(number_of_runs)) << "us" << std::endl;
	}
	
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}

}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_remove_if(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const double chance_of_change, const bool output_csv = false, const bool reserve = false)
{
	assert (number_of_elements > 1);

	double total = 0;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if ((rand() % static_cast<unsigned int>(chance_of_change * static_cast<double>(number_of_elements))) == 0)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			container.remove_if();

			if (chance_of_change < 1)
			{
				for (unsigned int number_of_insertions = 0; number_of_insertions != static_cast<unsigned int>(1.0 / chance_of_change); ++number_of_insertions)
				{
					container_insert(container);
				}
			}
			else
			{
				if (rand() % static_cast<unsigned int>(chance_of_change) == 0 || container.size() == 0)
				{
					container_insert(container);
				}
			}
		}
	}

	std::cerr << "Dump total and time: " << total << full_time.get_elapsed_us() << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if ((rand() % static_cast<unsigned int>(chance_of_change * static_cast<double>(number_of_elements))) == 0)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			container.remove_if();

			if (chance_of_change < 1)
			{
				for (unsigned int number_of_insertions = 0; number_of_insertions != static_cast<unsigned int>(1.0 / chance_of_change); ++number_of_insertions)
				{
					container_insert(container);
				}
			}
			else
			{
				if (rand() % static_cast<unsigned int>(chance_of_change) == 0 || container.size() == 0)
				{
					container_insert(container);
				}
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / static_cast<double>(number_of_runs));
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / static_cast<double>(number_of_runs)) << "us" << std::endl;
	}
	
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}

}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_remove_if_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (number_of_elements > 1);

	const unsigned int total_number_of_insertions = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100));

	double total = 0;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			container.remove_if();

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	std::cerr << "Dump total and time: " << total << full_time.get_elapsed_us() << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		if (reserve)
		{
			container_reserve(container, number_of_elements);
		}
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);
				}
				else
				{
					total += container_iterate(container, current_element);
					++current_element;
				}
			}

			container.remove_if();

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / static_cast<double>(number_of_runs));
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / static_cast<double>(number_of_runs)) << "us" << std::endl;
	}
	
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}

}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_reinsertion(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage > 0 && erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 100.0f));
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;
	insert_time = 0; erase_time = 0; total_size = 0;



	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;


	if (output_csv)
	{
		std::cout << ", " << (insert_time / number_of_runs) << ", ";

		if (erasure_percentage != 0)
		{
			std::cout << (erase_time / number_of_runs);
		}
	}
	else
	{
		std::cout << "Insert " << number_of_elements << " elements: " << (insert_time / number_of_runs) << "us" << std::endl;
		std::cout << "Randomly erase " << erasure_percentage << "% of elements: " << (erase_time / number_of_runs) << "us" << std::endl;
	}


	// Do whole process one more time so we have something for iteration test, without copying/moving containers:
	container_type container;

	{
		// Insert test
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		// Erase test
		if (erasure_percentage != 0)
		{
			number_of_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
				else
				{
					++current_element;
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
					{
						container_erase(container, current_element);

						if (++number_of_erasures == erasure_limit)
						{
							break;
						}
					}
				}
			}
		}
	}


	iteration_test(container, number_of_runs * 1000, output_csv);

	const unsigned int number_of_inserts = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 300));
	
	for (unsigned int element_number = 0; element_number != number_of_inserts; ++element_number)
	{
		container_insert(container);
	}
	
	iteration_test(container, number_of_runs * 1000, output_csv);


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_remove_if_reinsertion(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage > 0 && erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 100.0f));
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	for (unsigned int run_number = 0; run_number != number_of_runs / 10; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		container.remove_if();

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;
	insert_time = 0; erase_time = 0; total_size = 0;



	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
	
		// Insert test
		insert_timer.start();
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		insert_time += insert_timer.get_elapsed_us();
		

		// Erase test
		if (erasure_percentage == 0)
		{
			continue;
		}

		number_of_erasures = 0;
		erase_timer.start();
	
		for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
		{
			if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
			else
			{
				++current_element;
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}


		container.remove_if();

		erase_time += erase_timer.get_elapsed_us();
		
		total_size += container.size();
	}


	// To stop compiler optimising out the above loop
	std::cerr << "Dump times and size: " << insert_time << ", " << erase_time << ", " << total_size << std::endl;


	if (output_csv)
	{
		std::cout << ", " << (insert_time / number_of_runs) << ", ";

		if (erasure_percentage != 0)
		{
			std::cout << (erase_time / number_of_runs);
		}
	}
	else
	{
		std::cout << "Insert " << number_of_elements << " elements: " << (insert_time / number_of_runs) << "us" << std::endl;
		std::cout << "Randomly erase " << erasure_percentage << "% of elements: " << (erase_time / number_of_runs) << "us" << std::endl;
	}


	// Do whole process one more time so we have something for iteration test, without copying/moving containers:
	container_type container;

	{
		// Insert test
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}
		
		// Erase test
		if (erasure_percentage != 0)
		{
			number_of_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
				else
				{
					++current_element;
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if (static_cast<unsigned int>(rand() % 100) < erasure_percentage)
					{
						container_erase(container, current_element);

						if (++number_of_erasures == erasure_limit)
						{
							break;
						}
					}
				}
			}

			container.remove_if();
		}
	}


	iteration_test(container, number_of_runs * 1000, output_csv);

	const unsigned int number_of_inserts = static_cast<unsigned int>(static_cast<float>(number_of_elements) * (static_cast<float>(erasure_percentage) / 300));
	
	for (unsigned int element_number = 0; element_number != number_of_inserts; ++element_number)
	{
		container_insert(container);
	}
	
	iteration_test(container, number_of_runs * 1000, output_csv);


	if (output_csv)
	{
		std::cout << "\n";
	}
	else
	{
		std::cout << "\n\n";
	}
}





template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);
	
	if (output_csv)
	{
		if (erasure_percentage == 0)
		{
			std::cout << "Number of elements, Insertion,, Iteration," << std::endl;
		}
		else
		{
			std::cout << "Number of elements, Insertion, Erasure, Iteration" << std::endl;
		}
	}


	for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
	{
		if (output_csv)
		{
			std::cout << number_of_elements;
		}
		
		benchmark<container_type>(number_of_elements, 1000000 / number_of_elements, erasure_percentage, output_csv, reserve);
	}
	
	if (output_csv)
	{
		std::cout << "\n,,,\n,,,\n";
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_remove_if(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);
	
	if (output_csv)
	{
		if (erasure_percentage == 0)
		{
			std::cout << "Number of elements, Insertion,, Iteration," << std::endl;
		}
		else
		{
			std::cout << "Number of elements, Insertion, Erasure, Iteration" << std::endl;
		}
	}


	for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
	{
		if (output_csv)
		{
			std::cout << number_of_elements;
		}
		
		benchmark_remove_if<container_type>(number_of_elements, 1000000 / number_of_elements, erasure_percentage, output_csv, reserve);
	}
	
	if (output_csv)
	{
		std::cout << "\n,,,\n,,,\n";
	}
}


template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_remove_if_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);
	
	if (output_csv)
	{
		if (erasure_percentage == 0)
		{
			std::cout << "Number of elements, Insertion,, Iteration," << std::endl;
		}
		else
		{
			std::cout << "Number of elements, Insertion, Erasure, Iteration" << std::endl;
		}
	}


	for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
	{
		if (output_csv)
		{
			std::cout << number_of_elements;
		}
		
		benchmark_remove_if_reinsertion<container_type>(number_of_elements, 1000000 / number_of_elements, erasure_percentage, output_csv, reserve);
	}
	
	if (output_csv)
	{
		std::cout << "\n,,,\n,,,\n";
	}
}


template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);
	
	if (output_csv)
	{
		std::cout << "Number of elements, Insertion, Erase " << erasure_percentage << "%, Iteration, Iteration after reinsert " << static_cast<unsigned int>(static_cast<float>(erasure_percentage) / 3.0f) << "%\n";
	}

	for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
	{
		if (output_csv)
		{
			std::cout << number_of_elements;
		}
		
		benchmark_reinsertion<container_type>(number_of_elements, 1000000 / number_of_elements, erasure_percentage, output_csv, reserve);
	}
	
	if (output_csv)
	{
		std::cout << "\n,,,\n,,,\n";
	}
}






template <class container_type>
inline PLF_FORCE_INLINE void benchmark_erasure_range(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int initial_erasure_percentage, const unsigned int erasure_addition, const unsigned int max_erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (initial_erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);

	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		benchmark_range<container_type>(min_number_of_elements, max_number_of_elements, multiply_factor, erasure_percentage, output_csv, reserve);
		std::cout << "\n,,,\n,,,\n";
	}
}


template <class container_type>
inline PLF_FORCE_INLINE void benchmark_erasure_if_range(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int initial_erasure_percentage, const unsigned int erasure_addition, const unsigned int max_erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (initial_erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);

	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		benchmark_range_remove_if<container_type>(min_number_of_elements, max_number_of_elements, multiply_factor, erasure_percentage, output_csv, reserve);
		std::cout << "\n,,,\n,,,\n";
	}
}


template <class container_type>
inline PLF_FORCE_INLINE void benchmark_erasure_if_range_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int initial_erasure_percentage, const unsigned int erasure_addition, const unsigned int max_erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (initial_erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);

	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		benchmark_range_remove_if_reinsertion<container_type>(min_number_of_elements, max_number_of_elements, multiply_factor, erasure_percentage, output_csv, reserve);
		std::cout << "\n,,,\n,,,\n";
	}
}




template <class container_type>
inline PLF_FORCE_INLINE void benchmark_erasure_range_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int initial_erasure_percentage, const unsigned int erasure_addition, const unsigned int max_erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (initial_erasure_percentage < 100); // Ie. lower than 100%
	assert (min_number_of_elements > 1);
	assert (min_number_of_elements < max_number_of_elements);
	
	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		benchmark_range_reinsertion<container_type>(min_number_of_elements, max_number_of_elements, multiply_factor, erasure_percentage, output_csv, reserve);
		std::cout << "\n,,,\n,,,\n";
	}
}




template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_stack(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const bool output_csv = false, const bool reserve = false)
{
	if (output_csv)
	{
		std::cout << "Number of elements, Push, Back/Pop, Total time" << std::endl;
	}

	for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
	{
		if (output_csv)
		{
			std::cout << number_of_elements;
		}
		
		benchmark_stack<container_type>(number_of_elements, 100000000 / number_of_elements, output_csv, reserve);
	}
	
	if (output_csv)
	{
		std::cout << "\n,,,\n,,,\n";
	}
}





template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_general_use(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const double initial_chance_of_change, const double max_chance_of_change, const double chance_of_change_addition_amount, const bool output_csv = false, const bool reserve = false)
{
	for (double chance_of_change = initial_chance_of_change; chance_of_change <= max_chance_of_change; chance_of_change += chance_of_change_addition_amount)
	{
		std::cout << "Chance of modification during 1 frame: 1 in " << chance_of_change << std::endl << std::endl;
		
		if (output_csv)
		{
			std::cout << "Number of elements, Total time" << std::endl;
		}

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			if (output_csv)
			{
				std::cout << number_of_elements;
			}
			
			benchmark_general_use<container_type>(number_of_elements, 100000 / number_of_elements, number_of_cycles, chance_of_change, output_csv, reserve);
		}
		
		if (output_csv)
		{
			std::cout << "\n,,,\n,,,\n";
		}
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_general_use_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_erasure_percentage, const unsigned int max_erasure_percentage, const unsigned int erasure_addition, const bool output_csv = false, const bool reserve = false)
{
	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage: " << erasure_percentage << std::endl << std::endl;
		
		if (output_csv)
		{
			std::cout << "Number of elements, Total time" << std::endl;
		}

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			if (output_csv)
			{
				std::cout << number_of_elements;
			}
			
			benchmark_general_use_percentage<container_type>(number_of_elements, 100000 / number_of_elements, number_of_cycles, erasure_percentage, output_csv, reserve);
		}
		
		if (output_csv)
		{
			std::cout << "\n,,,\n,,,\n";
		}
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_general_use_remove_if(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const double initial_chance_of_change, const double max_chance_of_change, const double chance_of_change_addition_amount, const bool output_csv = false, const bool reserve = false)
{
	for (double chance_of_change = initial_chance_of_change; chance_of_change <= max_chance_of_change; chance_of_change += chance_of_change_addition_amount)
	{
		std::cout << "Chance of modification during 1 frame: 1 in " << chance_of_change << std::endl << std::endl;
		
		if (output_csv)
		{
			std::cout << "Number of elements, Total time" << std::endl;
		}

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			if (output_csv)
			{
				std::cout << number_of_elements;
			}
			
			benchmark_general_use_remove_if<container_type>(number_of_elements, 100000 / number_of_elements, number_of_cycles, chance_of_change, output_csv, reserve);
		}
		
		if (output_csv)
		{
			std::cout << "\n,,,\n,,,\n";
		}
	}
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_range_general_use_remove_if_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_erasure_percentage, const unsigned int max_erasure_percentage, const unsigned int erasure_addition, const bool output_csv = false, const bool reserve = false)
{
	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage: " << erasure_percentage << std::endl << std::endl;
		
		if (output_csv)
		{
			std::cout << "Number of elements, Total time" << std::endl;
		}

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			if (output_csv)
			{
				std::cout << number_of_elements;
			}
			
			benchmark_general_use_remove_if_percentage<container_type>(number_of_elements, 100000 / number_of_elements, number_of_cycles, erasure_percentage, output_csv, reserve);
		}
		
		if (output_csv)
		{
			std::cout << "\n,,,\n,,,\n";
		}
	}
}




// Utility function:
inline PLF_FORCE_INLINE void output_to_csv_file(char *filename)
{
	// Seed randomiser with time:
	time_t timer;
	time(&timer);
	srand((unsigned int)timer); // Note: using random numbers to avoid CPU prediction

	freopen("errors.log","w", stderr); 
	char logfile[512];
	sprintf(logfile, "%s.csv", filename);
	std::cout << "Outputting results to logfile " << logfile << "." << std::endl << "Please wait while program completes. This may take a while. Program will close once complete." << std::endl;
	freopen(logfile,"w", stdout);
}


#endif // PLF_BENCH_H