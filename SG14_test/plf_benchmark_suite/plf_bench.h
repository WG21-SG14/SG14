#pragma once

#ifndef PLF_BENCH_H
#define PLF_BENCH_H

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <stack>
#include <cstdio> // freopen, sprintf
#include <limits> // std::numeric_limits

#include "plf_colony.h"
#include "plf_stack.h"
#include "plf_nanotimer.h"
#include "plf_indexed_vector.h"
#include "plf_pointer_deque.h"
#include "plf_packed_deque.h"


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



	
struct large_struct_bool
{
	int numbers[100];
	char a_string[50];
	double unused_number;
	double number;
	double *empty_field_1;
	double *empty_field_2;
	unsigned int empty_field3;
	unsigned int empty_field4;
	bool erased;

	// This function is required for testing std::multiset:
	inline bool operator < (const large_struct_bool &rh) const
	{
		return rh.number < this->number;
	}
	
	large_struct_bool(const unsigned int num): number(num), erased(false) {};
};




// MATH FUNCTIONS:

// Fast xorshift+128 random number generator function (original: https://codingforspeed.com/using-faster-psudo-random-generator-xorshift/)
unsigned int xor_rand()
{
	static unsigned int x = 123456789;
	static unsigned int y = 362436069;
	static unsigned int z = 521288629;
	static unsigned int w = 88675123;
	
	const unsigned int t = x ^ (x << 11); 

	// Rotate the static values (w rotation in return statement):
	x = y;
	y = z;
	z = w;
   
	return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}


inline unsigned int fast_mod(const unsigned int input, const unsigned int ceiling) // courtesy of chandler carruth
{
    // apply the modulo operator only when needed
    // (i.e. when the input is greater than the ceiling)
    return (input >= ceiling) ? input % ceiling : input;
}


inline unsigned int rand_within(const unsigned int range)
{
	return fast_mod(xor_rand(), range);
}




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
inline PLF_FORCE_INLINE void container_reserve(plf::indexed_vector<container_contents> &container, unsigned int amount)
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
	container.push_back(container_contents(xor_rand() & 255)); // using bitwise AND as a faster replacement for modulo, as this is a power of 2 number - 1
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::list<container_contents> &container)
{
	container.push_front(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::map<unsigned int, container_contents> &container)
{
	container.insert(std::make_pair(static_cast<unsigned int>(container.size()), container_contents(xor_rand() & 255)));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::packed_deque<container_contents> &container)
{
	container.insert(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::multiset<container_contents> &container)
{
	container.insert(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::stack<container_contents> &container)
{
	container.push(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(std::stack<container_contents> &container)
{
	container.push(container_contents(xor_rand() & 255));
}




template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::colony<container_contents, std::allocator<container_contents>, unsigned char> &container)
{
	container.insert(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::colony<container_contents, std::allocator<container_contents>, unsigned short> &container)
{
	container.insert(container_contents(xor_rand() & 255));
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_insert(plf::colony<container_contents, std::allocator<container_contents>, unsigned int> &container)
{
	container.insert(container_contents(xor_rand() & 255));
}


// The following is only present to cater for older versions of colony for comparison testing
//template <class container_contents>
//inline PLF_FORCE_INLINE void container_insert(plf::colony<container_contents> &container)
//{
//	container.insert(container_contents(xor_rand() & 255));
//}



// Actual tests:




// ERASURE FUNCTIONS:

template <class container_type>
inline PLF_FORCE_INLINE void container_erase(container_type &container, typename container_type::iterator &the_iterator)
{
	the_iterator = container.erase(the_iterator);
}


// This was used to check the pointer-erase colony function performance - not used normally:

//template <class container_contents>
//inline PLF_FORCE_INLINE void container_erase(plf::colony<container_contents> &container, typename plf::colony<container_contents>::iterator &the_iterator)
//{
//	the_iterator = container.erase(&(*the_iterator));
//}
//
//



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


inline PLF_FORCE_INLINE void container_erase(std::vector<large_struct_bool> &container, std::vector<large_struct_bool>::iterator &the_iterator)
{
	the_iterator->erased = true;
	++the_iterator;
}


inline PLF_FORCE_INLINE void container_erase(std::deque<large_struct_bool> &container, std::deque<large_struct_bool>::iterator &the_iterator)
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
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<small_struct, std::allocator<small_struct> > &the_container, const typename container_type<small_struct, std::allocator<small_struct> >::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<large_struct, std::allocator<large_struct> > &the_container, const typename container_type<large_struct, std::allocator<large_struct> >::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type, typename container_contents, typename allocator_type, typename skipfield_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<container_contents, allocator_type, skipfield_type> &the_container, const typename container_type<container_contents, allocator_type, skipfield_type>::iterator &the_iterator)
{
	return static_cast<unsigned int>(*the_iterator);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<small_struct, std::allocator<small_struct>, unsigned short> &the_container, const typename container_type<small_struct, std::allocator<small_struct>, unsigned short>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<large_struct, std::allocator<large_struct>, unsigned short> &the_container, const typename container_type<large_struct, std::allocator<large_struct>, unsigned short>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<small_struct, std::allocator<small_struct>, unsigned char> &the_container, const typename container_type<small_struct, std::allocator<small_struct>, unsigned char>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<large_struct, std::allocator<large_struct>, unsigned char> &the_container, const typename container_type<large_struct, std::allocator<large_struct>, unsigned char>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<small_struct, std::allocator<small_struct>, unsigned int> &the_container, const typename container_type<small_struct, std::allocator<small_struct>, unsigned int>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->number);
}


template <template <typename,typename, typename> class container_type>
inline PLF_FORCE_INLINE unsigned int container_iterate(const container_type<large_struct, std::allocator<large_struct>, unsigned int> &the_container, const typename container_type<large_struct, std::allocator<large_struct>, unsigned int>::iterator &the_iterator)
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


inline PLF_FORCE_INLINE unsigned int container_iterate(const std::vector<large_struct_bool> &the_container, const std::vector<large_struct_bool>::iterator &the_iterator)
{
	return (the_iterator->erased) ? 0 : static_cast<unsigned int>(the_iterator->number);
}


inline PLF_FORCE_INLINE unsigned int container_iterate(const std::deque<large_struct_bool> &the_container, const std::deque<large_struct_bool>::iterator &the_iterator)
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





inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::packed_deque<small_struct> &the_container, const plf::packed_deque<small_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->element.number);
}



inline PLF_FORCE_INLINE unsigned int container_iterate(const plf::packed_deque<large_struct> &the_container, const plf::packed_deque<large_struct>::iterator &the_iterator)
{
	return static_cast<unsigned int>(the_iterator->element.number);
}





// Actual tests:

template<class container_type>
inline PLF_FORCE_INLINE void iteration_test(container_type &container, const unsigned int number_of_runs, const bool output_csv = false)
{
	double total_time = 0, total = 0;
	plf::nanotimer timer;
	
	timer.start();
	
	const unsigned int end = (number_of_runs / 10) + 1;
	const typename container_type::iterator end_element = container.end(); 
	
	for (unsigned int run_number = 0; run_number != end; ++run_number)
	{	
		for (typename container_type::iterator current_element = container.begin(); current_element != end_element; ++current_element)
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
		for (typename container_type::iterator current_element = container.begin(); current_element != end_element; ++current_element)
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
	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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

	const unsigned int erasure_limit = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	unsigned int number_of_erasures;
	const unsigned int erasure_percent_expanded = static_cast<unsigned int>((static_cast<double>(erasure_percentage) * 1.28) + 0.5);
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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
			if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
			if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
					if ((xor_rand() & 127) < erasure_percent_expanded)
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



template <class container_class>
inline PLF_FORCE_INLINE void container_remove_if(container_class &container)
{
	typename container_class::iterator result = container.begin(), it = container.begin(), last = container.end();

	while (it != last)
	{
		if (!(it->erased))
		{
			*result = *it;
			++result;
		}

		++it;
	}
	
	for (it = result; it != last; ++it)
	{
		container.pop_back();
	}
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_remove_if(plf::pointer_deque<container_contents> &container)
{
	container.remove_if();
}


template <class container_contents>
inline PLF_FORCE_INLINE void container_remove_if(plf::indexed_vector<container_contents> &container)
{
	container.remove_if();
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_remove_if(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	const unsigned int erasure_percent_expanded = static_cast<unsigned int>((static_cast<double>(erasure_percentage) * 1.28) + 0.5);
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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
			if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) < erasure_percent_expanded)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		container_remove_if(container);

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
			if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) < erasure_percent_expanded)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		container_remove_if(container);

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
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
					if ((xor_rand() & 127) < erasure_percent_expanded)
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

		container_remove_if(container);
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
inline PLF_FORCE_INLINE unsigned int approximate_memory_use(container_type &container)
{
	return static_cast<unsigned int>(container.approximate_memory_use());
}



template<class container_contents>
inline PLF_FORCE_INLINE unsigned int approximate_memory_use(std::list<container_contents> &list)
{
	return static_cast<unsigned int>(sizeof(list) + (list.size() * (sizeof(container_contents) + sizeof(container_contents *) + (sizeof(void *) * 2))));
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int number_of_modifications)
{
	assert (number_of_elements > 1);

	double total = 0;
	unsigned int end_approximate_memory_use = 0;
	plf::nanotimer full_time;
	full_time.start();

	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
	{
		container_type container;
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(number_of_elements) >= number_of_modifications)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != number_of_modifications; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
		
		end_approximate_memory_use += approximate_memory_use(container);
	}
	
	end_approximate_memory_use /= end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(number_of_elements) >= number_of_modifications)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != number_of_modifications; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int erasure_percentage)
{
	assert (number_of_elements > 1);

	const unsigned int total_number_of_insertions = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	double total = 0;
	unsigned int end_approximate_memory_use = 0;
	plf::nanotimer full_time;
	full_time.start();

	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
	{
		container_type container;
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(100) >= erasure_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}

		end_approximate_memory_use += approximate_memory_use(container);
	}
	
	end_approximate_memory_use /= end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(100) >= erasure_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
}




// new type for more realistic test:
template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_small_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const double erasure_percentage)
{
	assert (number_of_elements > 1);

	double total = 0;
	unsigned int end_approximate_memory_use = 0, num_erasures;
	const unsigned int comparison_percentage = static_cast<unsigned int>((erasure_percentage * 167772.16) + 0.5), dump_run_end = (number_of_runs / 10) + 1;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != dump_run_end; ++run_number)
	{
		container_type container;

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;
			
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{ // substituting bitwise-and for modulo for speed:
				if ((xor_rand() & 16777215) >= comparison_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			for (unsigned int counter = 0; counter != num_erasures; ++counter)
			{
				container_insert(container);
			}
		}

		end_approximate_memory_use += approximate_memory_use(container);
	}

	end_approximate_memory_use /= dump_run_end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed)

	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;
			
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{ // substituting bitwise-and for modulo for speed:
				if ((xor_rand() & 16777215) >= comparison_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			for (unsigned int counter = 0; counter != num_erasures; ++counter)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.

}




template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_remove_if_small_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const double erasure_percentage)
{
	assert (number_of_elements > 1);

	double total = 0;
	unsigned int end_approximate_memory_use = 0, num_erasures;
	const unsigned int comparison_percentage = static_cast<unsigned int>(erasure_percentage * 167772.16), dump_run_end = (number_of_runs / 10) + 1;
	plf::nanotimer full_time;
	full_time.start();

	for (unsigned int run_number = 0; run_number != dump_run_end; ++run_number)
	{
		container_type container;

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;
			
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{ // substituting bitwise-and for modulo for speed:
				if ((xor_rand() & 16777215) >= comparison_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			if (num_erasures != 0)
			{
				container_remove_if(container);
			}

			for (unsigned int counter = 0; counter != num_erasures; ++counter)
			{
				container_insert(container);
			}
		}

		end_approximate_memory_use += approximate_memory_use(container);
	}

	end_approximate_memory_use /= dump_run_end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed)

	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;

		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;
			
			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{ // substituting bitwise-and for modulo for speed:
				if ((xor_rand() & 16777215) >= comparison_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			if (num_erasures != 0)
			{
				container_remove_if(container);
			}

			for (unsigned int counter = 0; counter != num_erasures; ++counter)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_general_use_remove_if(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int number_of_modifications, const bool output_csv = false, const bool reserve = false)
{
	assert (number_of_elements > 1);

	double total = 0;
	unsigned int end_approximate_memory_use = 0;
	plf::nanotimer full_time;
	full_time.start();

	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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
				if (rand_within(number_of_elements) >= number_of_modifications)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			container_remove_if(container);

			for (unsigned int number_of_insertions = 0; number_of_insertions != number_of_modifications; ++number_of_insertions)
			{
				container_insert(container);
			}
		}

		end_approximate_memory_use += approximate_memory_use(container);
	}
	
	end_approximate_memory_use /= end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
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
				if (rand_within(number_of_elements) >= number_of_modifications)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
				}
			}

			container_remove_if(container);

			for (unsigned int number_of_insertions = 0; number_of_insertions != number_of_modifications; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	if (output_csv)
	{
		std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use;
	}
	else
	{
		std::cout << "Iterate and sum: " << (total_time / static_cast<double>(number_of_runs)) << "us, size = " << end_approximate_memory_use << std::endl;
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
inline PLF_FORCE_INLINE void benchmark_general_use_remove_if_percentage(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int number_of_cycles, const unsigned int erasure_percentage)
{
	assert (number_of_elements > 1);

	const unsigned int total_number_of_insertions = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	
	double total = 0;
	unsigned int end_approximate_memory_use = 0, num_erasures;
	plf::nanotimer full_time;
	full_time.start();

	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
	{
		container_type container;
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(100) >= erasure_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			if (num_erasures != 0)
			{
				container_remove_if(container);
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
		
		end_approximate_memory_use += approximate_memory_use(container);
	}

	end_approximate_memory_use /= end;

	std::cerr << "Dump total and time and approximate_memory_use: " << total << full_time.get_elapsed_us() << end_approximate_memory_use << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
	 
	full_time.start();

	for (unsigned int run_number = 0; run_number != number_of_runs; ++run_number)
	{
		container_type container;
		
		for (unsigned int element_number = 0; element_number != number_of_elements; ++element_number)
		{
			container_insert(container);
		}

		for (unsigned int cycle = 0; cycle != number_of_cycles; ++cycle)
		{
			num_erasures = 0;

			for (typename container_type::iterator current_element = container.begin(); current_element != container.end();)
			{
				if (rand_within(100) >= erasure_percentage)
				{
					total += container_iterate(container, current_element++);
				}
				else
				{
					container_erase(container, current_element);
					++num_erasures;
				}
			}

			if (num_erasures != 0)
			{
				container_remove_if(container);
			}

			for (unsigned int number_of_insertions = 0; number_of_insertions != total_number_of_insertions; ++number_of_insertions)
			{
				container_insert(container);
			}
		}
	}

	const double total_time = full_time.get_elapsed_us();

	std::cout << ", " << (total_time / static_cast<double>(number_of_runs)) << ", " << end_approximate_memory_use << "\n";
	std::cerr << "Dump total: " << total << std::endl; // To prevent compiler from optimizing out both inner loops (ie. total must have a side effect or it'll be removed) - no kidding, gcc will actually do this with std::vector.
}



template <class container_type>
inline PLF_FORCE_INLINE void benchmark_reinsertion(const unsigned int number_of_elements, const unsigned int number_of_runs, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
{
	assert (erasure_percentage > 0 && erasure_percentage < 100); // Ie. lower than 100%
	assert (number_of_elements > 1);


	const unsigned int erasure_limit = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	const unsigned int erasure_percent_expanded = static_cast<unsigned int>((static_cast<double>(erasure_percentage) * 1.28) + 0.5);
	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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
			if ((xor_rand() & 127) >= erasure_percent_expanded)
			{
				++current_element;
			}
			else
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
			if ((xor_rand() & 127) >= erasure_percent_expanded)
			{
				++current_element;
			}
			else
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if ((xor_rand() & 127) < erasure_percent_expanded)
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
				if ((xor_rand() & 127) >= erasure_percent_expanded)
				{
					++current_element;
				}
				else
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if ((xor_rand() & 127) < erasure_percent_expanded)
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


	const unsigned int erasure_limit = static_cast<unsigned int>((static_cast<double>(number_of_elements) * (static_cast<double>(erasure_percentage) / 100.0)) + 0.5);
	const unsigned int erasure_percent_expanded = static_cast<unsigned int>((static_cast<double>(erasure_percentage) * 1.28) + 0.5);

	unsigned int number_of_erasures;
	double insert_time = 0, erase_time = 0, total_size = 0;
	plf::nanotimer insert_timer, erase_timer;


	// Dump-runs to get the cache 'warmed up':
	const unsigned int end = (number_of_runs / 10) + 1;
	for (unsigned int run_number = 0; run_number != end; ++run_number)
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
			if ((xor_rand() & 127) >= erasure_percent_expanded)
			{
				++current_element;
			}
			else
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if ((xor_rand() & 127) < erasure_percent_expanded)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}

		container_remove_if(container);

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
			if ((xor_rand() & 127) >= erasure_percent_expanded)
			{
				++current_element;
			}
			else
			{
				container_erase(container, current_element);

				if (++number_of_erasures == erasure_limit)
				{
					break;
				}
			}
		}
		
		if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
		{
			for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
			{
				if ((xor_rand() & 127) < erasure_percent_expanded)
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
		}


		container_remove_if(container);

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
				if ((xor_rand() & 127) >= erasure_percent_expanded)
				{
					++current_element;
				}
				else
				{
					container_erase(container, current_element);

					if (++number_of_erasures == erasure_limit)
					{
						break;
					}
				}
			}
			
			if (number_of_erasures != erasure_limit) // If not enough erasures have occured, reverse_iterate until they have - this prevents differences in container size during iteration
			{
				for (typename container_type::iterator current_element = --(container.end()); current_element != container.begin(); --current_element)
				{
					if ((xor_rand() & 127) < erasure_percent_expanded)
					{
						container_erase(container, current_element);

						if (++number_of_erasures == erasure_limit)
						{
							break;
						}
					}
				}
			}

			container_remove_if(container);
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
void benchmark_range_remove_if(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
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
void benchmark_range_remove_if_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
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
void benchmark_range_reinsertion(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int erasure_percentage, const bool output_csv = false, const bool reserve = false)
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
void benchmark_range_stack(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const bool output_csv = false, const bool reserve = false)
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
void benchmark_range_general_use(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_number_of_modifications, const unsigned int max_number_of_modifications, const unsigned int number_of_modification_addition_amount)
{
	for (double number_of_modifications = initial_number_of_modifications; number_of_modifications <= max_number_of_modifications; number_of_modifications += number_of_modification_addition_amount)
	{
		std::cout << "Number of modifications during 1 frame: " << number_of_modifications << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;
			
			benchmark_general_use<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_modifications);
		}

		std::cout << "\n,,,\n,,,\n";
	}
}



template <class container_type>
void benchmark_range_general_use_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_erasure_percentage, const unsigned int max_erasure_percentage, const unsigned int erasure_addition)
{
	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage: " << erasure_percentage << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;

			benchmark_general_use_percentage<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_cycles, erasure_percentage);
		}
		
		std::cout << "\n,,,\n,,,\n";
	}
}



template <class container_type>
void benchmark_range_general_use_small_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const double initial_erasure_percentage, const double max_erasure_percentage, const double erasure_addition)
{
	for (double erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage: " << erasure_percentage << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;

			benchmark_general_use_small_percentage<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_cycles, erasure_percentage);
		}
		
		std::cout << "\n,,,\n,,,\n";
	}
}




template <class container_type>
void benchmark_range_general_use_remove_if_small_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const double initial_erasure_percentage, const double max_erasure_percentage, const double erasure_addition)
{
	for (double erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage (remove_if): " << erasure_percentage << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;
			
			benchmark_general_use_remove_if_small_percentage<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_cycles, erasure_percentage);
		}
		
		std::cout << "\n,,,\n,,,\n";
	}
}



template <class container_type>
void benchmark_range_general_use_remove_if(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_number_of_modifications, const unsigned int max_number_of_modifications, const unsigned int number_of_modification_addition_amount)
{
	for (double number_of_modifications = initial_number_of_modifications; number_of_modifications <= max_number_of_modifications; number_of_modifications += number_of_modification_addition_amount)
	{
		std::cout << "Number of modifications during 1 frame: " << number_of_modifications << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;

			benchmark_general_use_remove_if<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_cycles, number_of_modifications);
		}

		std::cout << "\n,,,\n,,,\n";
	}
}




template <class container_type>
void benchmark_range_general_use_remove_if_percentage(const unsigned int min_number_of_elements, const unsigned int max_number_of_elements, const double multiply_factor, const unsigned int number_of_cycles, const unsigned int initial_erasure_percentage, const unsigned int max_erasure_percentage, const unsigned int erasure_addition)
{
	for (unsigned int erasure_percentage = initial_erasure_percentage; erasure_percentage < max_erasure_percentage; erasure_percentage += erasure_addition)
	{
		std::cout << "Erasure percentage: " << erasure_percentage << "\n\nNumber of elements, Total time, Memory Usage\n";

		for (unsigned int number_of_elements = min_number_of_elements; number_of_elements <= max_number_of_elements; number_of_elements = static_cast<unsigned int>(static_cast<double>(number_of_elements) * multiply_factor))
		{
			std::cout << number_of_elements;

			benchmark_general_use_remove_if_percentage<container_type>(number_of_elements, (1000 / number_of_elements) + 1, number_of_cycles, erasure_percentage);
		}

		std::cout << "\n,,,\n,,,\n";
	}
}



 
// Utility functions:

inline PLF_FORCE_INLINE void output_to_csv_file(char *filename)
{
	freopen("errors.log","w", stderr);
	char logfile[512];
	sprintf(logfile, "%s.csv", filename);
	std::cout << "Outputting results to logfile " << logfile << "." << std::endl << "Please wait while program completes. This may take a while. Program will close once complete." << std::endl;
	freopen(logfile,"w", stdout);
}


#endif // PLF_BENCH_H
