// About: there are 8 tests here;
// one with a vector of dynamically-allocated objects vs colony, array with boolean field, list, map and deque(small structs)
// one with vector with boolean field, deque with boolean field and array with boolean field vs colony (small structs)
// three with vector vs colony vs array vs deque with boolean field (unsigned ints, small structs and large structs)
// three with vector vs plf::stack (unsigned ints, small structs and large structs)

// The array results simply show comparative performance using C-arrays - only appropriate for a situation where absolute size is known in advance and no objects are expected to be added to or genuinely erased from the container ('erasure' is simulated via a boolean flag, but no actual destructors are called or memory deallocated, as is the case with colonies).
// Comment/uncomment tests as appropriate. Timing may be fickle for small tests if using STL clock for timing instead of SDL, as below.


#include <vector>
#include <list>
#include <map>
#include <deque>
#include <iostream>
#include <cstdio> // log redirection
#include <cstdlib> // rand
#include <ctime> // timer
//#include "SDL_timer.h" // Install and use SDL2 (libsdl.org) instead of ctime for more accurate cross-platform timing - regular STL/C timing may only be accurate to around 15ms, regardless of function used (including high_resolution_clock), depending on the OS and compiler.

#include "plf_colony.h"


inline unsigned long get_time_in_ms()
{
	return (unsigned long)((double(clock()) / CLOCKS_PER_SEC) * 1000);
//	return (unsigned long) SDL_GetTicks(); // Use this for more accurate timing
}


void one_sec_delay()
{
	unsigned long end_time = get_time_in_ms() + 1000;

	while(get_time_in_ms() < end_time)
	{
	}
}



struct generic
{
	double *empty_field_1;
	double unused_number;
	unsigned int empty_field2;
	double *empty_field_3;
	double number;
	unsigned int empty_field4;
};



struct generic_for_array
{
	double *empty_field_1;
	double unused_number;
	unsigned int empty_field2;
	double *empty_field_3;
	double number;
	unsigned int empty_field4;
	bool erased;
};



struct large_generic
{
	double unused_number;
	double number;
	double *empty_field_1;
	double *empty_field_2;
	unsigned int empty_field3;
	unsigned int empty_field4;
	int numbers[5000];
	char a_string[500];
};



struct large_generic_for_array
{
	double unused_number;
	double number;
	double *empty_field_1;
	double *empty_field_2;
	unsigned int empty_field3;
	unsigned int empty_field4;
	int numbers[5000];
	char a_string[500];
	bool erased;
};





int main(int argc, char **argv)
{
	srand(clock()); // Note: using random numbers to avoid CPU prediction
//	srand(SDL_GetTicks());

	using namespace std;
	using namespace plf;



	cout << "OO TESTS:\n=========\n\n";


	// 1st real-world test - plf_colony speed against vector of pointers to objects, array with boolean field, std::map and std::list with a generic small struct:
	{
		vector<generic *> data_vector;
		data_vector.reserve(50000);
		deque<generic *> data_deque;
		list<generic> data_list;
		map<unsigned int, generic> data_map;
		colony<generic> data_colony(50000);

		generic_for_array *data_array = new generic_for_array[2000000];


		cout << "Real-world OO performance comparison between a std::vector of pointers to objects, plf::colony, an array with a boolean field, a list and a map using small structs." << endl;
		cout << "press enter to continue" << endl << endl;
		cin.get();

		cout << "small struct tests begin:" << endl << endl;
		cout << "milliseconds to add 2000000 generic structs:" << endl;
		one_sec_delay(); // to remove potential overhead from cout.

		unsigned long current_time, difference1, difference2, difference3, difference4, difference5, difference6;
		generic the_struct, *struct_pointer;
		generic_for_array the_array_struct;
		the_array_struct.erased = false;

		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 2000000; ++num)
		{
			the_struct.number = rand() % 1000000;
			struct_pointer = new generic(the_struct);
			data_vector.push_back(struct_pointer);
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 2000000; ++num)
		{
			the_struct.number = rand() % 1000000;
			data_colony.add(the_struct);
		}

		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 2000000; ++num)
		{
			
			the_array_struct.number = rand() % 1000000;
			memcpy(&(data_array[num]), &the_array_struct, sizeof(generic_for_array));
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 2000000; ++num)
		{
			the_struct.number = rand() % 1000000;
			data_list.push_front(the_struct);
		}

		difference4 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 2000000; ++num)
		{
			the_struct.number = rand() % 1000000;
			data_map.insert(std::make_pair(num, the_struct));
		}

		difference5 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 2000000; ++num)
		{
			the_struct.number = rand() % 1000000;
			struct_pointer = new generic(the_struct);
			data_deque.push_back(struct_pointer);
		}

		difference6 = get_time_in_ms() - current_time;

		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl  << "list: " << difference4 << endl  << "map: " << difference5 << endl << "deque: " << difference6 << endl << endl;

		cout << "milliseconds to randomly erase 1 in every 5000 entries:" << endl;
		one_sec_delay();


		current_time = get_time_in_ms();

		for (vector<generic *>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				delete *the_iterator;
				the_iterator = data_vector.erase(the_iterator);
			}
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_colony.erase(the_iterator);
			}
		}

		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 2000000; ++num)
		{
			if (rand() % 5000 == 0)
			{
				data_array[num].erased = true;
			}
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (list<generic>::iterator the_iterator = data_list.begin(); the_iterator != data_list.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_list.erase(the_iterator);
			}
		}

		difference4 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (map<unsigned int, generic>::iterator the_iterator = data_map.begin(); the_iterator != data_map.end();)
		{
			if (rand() % 5000 == 0)
			{
				map<unsigned int, generic>::iterator temp_iterator = the_iterator;
				++temp_iterator;
				data_map.erase(the_iterator);
				the_iterator = temp_iterator;
			}
			else
			{
				++the_iterator;
			}
		}

		difference5 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (deque<generic *>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				delete *the_iterator;
				the_iterator = data_deque.erase(the_iterator);
			}
		}

		difference6 = get_time_in_ms() - current_time;
		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl  << "list: " << difference4 << endl  << "map: " << difference5 << endl << "deque: " << difference6 << endl << endl;

		double total;
		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (vector<generic *>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
			{
				total += (*the_iterator)->number;
			}
		}

		difference1 = get_time_in_ms() - current_time;
		cout << "totaling vector after random erasures: " << total << endl;
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference2 = get_time_in_ms() - current_time;
		cout << "totaling colony after random erasures: " << total << endl;
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (unsigned int num = 0; num != 2000000; ++num)
			{
				if (!(data_array[num].erased))
				{
					total += data_array[num].number;
				}
			}
		}

		difference3 = get_time_in_ms() - current_time;
		cout <<  "totaling array after random erasures: " << total << endl;
		one_sec_delay();
		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (list<generic>::iterator the_iterator = data_list.begin(); the_iterator != data_list.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference4 = get_time_in_ms() - current_time;
		cout << "totaling list after random erasures: " << total << endl;
		one_sec_delay();
		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (map<unsigned int, generic>::iterator the_iterator = data_map.begin(); the_iterator != data_map.end(); ++the_iterator)
			{
				total += the_iterator->second.number;
			}
		}

		difference5 = get_time_in_ms() - current_time;
		cout << "totaling map after random erasures: " << total << endl;

		one_sec_delay();
		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (deque<generic *>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
			{
				total += (*the_iterator)->number;
			}
		}

		difference6 = get_time_in_ms() - current_time;
		cout << "totaling deque after random erasures: " << total << endl << endl;

		cout << "milliseconds to iterate over all entries 100 times and add stored random number to total:" << endl;
		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl  << "list: " << difference4 << endl  << "map: " << difference5 << endl << "deque: " << difference6 << endl << endl;
		cout << "milliseconds to clear containers and deallocate all objects:" << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();

		for (vector<generic *>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
		{
			delete *the_iterator;
		}

		data_vector.clear();
		difference1 = get_time_in_ms() - current_time;
		one_sec_delay();
		current_time = get_time_in_ms();
		data_colony.clear();
		difference2 = get_time_in_ms() - current_time;
		one_sec_delay();
		current_time = get_time_in_ms();
		delete [] data_array;
		difference3 = get_time_in_ms() - current_time;
		one_sec_delay();
		current_time = get_time_in_ms();
		data_list.clear();
		difference4 = get_time_in_ms() - current_time;
		one_sec_delay();
		current_time = get_time_in_ms();
		data_map.clear();
		difference5 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (deque<generic *>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
		{
			delete *the_iterator;
		}

		data_deque.clear();
		difference6 = get_time_in_ms() - current_time;

		cout << endl << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl  << "list: " << difference4 << endl  << "map: " << difference5 << endl << "deque: " << difference6 << endl << endl;
		
		cout << "press enter to exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;
	}



	// 2nd real-world test - plf_colony speed against vector, deque & array with boolean 'erased' fields, with a generic struct:
	{
		vector<generic_for_array> data_vector;
		deque<generic_for_array> data_deque;
		data_vector.reserve(500000);
		colony<generic> data_colony(500000);

		generic_for_array *data_array = new generic_for_array[5000000];


		cout << "Real-world oo performance comparison between a std::vector with a boolean field, plf::colony and an array with a boolean field, using small structs." << endl;
		cout << "press enter to continue" << endl << endl;
		cin.get();

		cout << "small struct tests begin:" << endl << endl;
		cout << "milliseconds to add 5000000 generic structs:" << endl;
		
		one_sec_delay(); // to remove potential overhead from cout.

		unsigned long current_time, difference1, difference2, difference3, difference4;
		generic the_struct;
		generic_for_array the_array_struct;
		the_array_struct.erased = false;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_array_struct.number = rand() % 5000000;
			data_vector.push_back(the_array_struct);
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_struct.number = rand() % 5000000;
			data_colony.add(the_struct);
		}

		difference2 = get_time_in_ms() - current_time;

		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			
			the_array_struct.number = rand() % 5000000;
			memcpy(&(data_array[num]), &the_array_struct, sizeof(generic_for_array));
		}

		difference3 = get_time_in_ms() - current_time;

		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_array_struct.number = rand() % 5000000;
			data_deque.push_back(the_array_struct);
		}

		difference4 = get_time_in_ms() - current_time;

		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl << "deque: " << difference4 << endl << endl;

		cout << "milliseconds to randomly erase 1 in every 5000 entries:" << endl;
		
		one_sec_delay();


		current_time = get_time_in_ms();

		for (vector<generic_for_array>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
		{
			if (rand() % 5000 == 0)
			{
				the_iterator->erased = true;
			}
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_colony.erase(the_iterator);
			}
		}

		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			if (rand() % 5000 == 0)
			{
				data_array[num].erased = true;
			}
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (deque<generic_for_array>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
		{
			if (rand() % 5000 == 0)
			{
				the_iterator->erased = true;
			}
		}

		difference4 = get_time_in_ms() - current_time;
		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl << "deque: " << difference4 << endl << endl;

		

		double total;
		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (vector<generic_for_array>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
			{
				if (!(the_iterator->erased))
				{
					total += the_iterator->number;
				}
			}
		}

		difference1 = get_time_in_ms() - current_time;
		cout << "totaling vector after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}
		
		difference2 = get_time_in_ms() - current_time;
		cout << "totaling colony after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (unsigned int num = 0; num != 5000000; ++num)
			{
				if (!(data_array[num].erased))
				{
					total += data_array[num].number;
				}
			}
		}

		difference3 = get_time_in_ms() - current_time;
		cout <<  "totaling array after random erasures: " << total << endl << endl;
		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (deque<generic_for_array>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
			{
				if (!(the_iterator->erased))
				{
					total += the_iterator->number;
				}
			}
		}

		difference4 = get_time_in_ms() - current_time;
		cout <<  "totaling deque after random erasures: " << total << endl << endl;

		cout << "milliseconds to iterate over all entries 100 times and add to total:" << endl;
		cout << "vector: " << difference1 << endl << "colony: " << difference2 << endl << "array: " << difference3 << endl << "deque: " << difference4 << endl << endl;
		cout << "press enter to exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;

		delete [] data_array;
	}




	cout << "RAW TESTS:\n=========\n\n";


	// 1st "raw" test - plf_colony speed against vector, array with boolean field, with unsigned ints:
	{
		vector<unsigned int> data_vector;
		deque<unsigned int> data_deque;
		data_vector.reserve(500000);
		colony<unsigned int> data_colony(500000);

        struct s_data 
        {
        	unsigned int number;
        	bool erased;
        };

		s_data *data_array = new s_data[5000000];

	
		cout << "Performance comparison between std::vector, plf::colony and an array, using unsigned ints." << endl;
		cout << "The array is included only to show raw speeds when there is no necessity for a container which can resize as needed to fit the data, but where objects can be 'erased' via a boolean flag." << endl << endl;
		cout << "Press Enter to continue" << endl << endl;
		cin.get();
	
		cout << "Unsigned int tests begin." << endl << endl;
		cout << "Milliseconds to add 50000000 unsigned ints:" << endl;
		
		one_sec_delay(); // To remove potential overhead from cout.
	
		unsigned long current_time, difference1, difference2, difference3, difference4;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			data_vector.push_back(rand() % 5000000);
		}
	
		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();
	
	
		for (unsigned int num = 0; num != 5000000; ++num)
		{
			data_colony.add(rand() % 5000000);
		}
	
		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();
	
		for (unsigned int num = 0; num != 5000000; ++num)
		{
			data_array[num].number = rand() % 5000000;
			data_array[num].erased = false;
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			data_deque.push_back(rand() % 5000000);
		}
	
		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;
		cout << "Milliseconds to randomly erase 1 in every 5000 entries:" << endl;
		 
		one_sec_delay();

	
		current_time = get_time_in_ms();
	
		for (vector<unsigned int>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_vector.erase(the_iterator);
			}
		}
	
		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();
	
		for (colony<unsigned int>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_colony.erase(the_iterator);
			}
		}
	
		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();
	
		for (unsigned int num = 0; num != 5000000; ++num)
		{
			if (rand() % 5000 == 0)
			{
				data_array[num].erased = true;
			}
		}
	
		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();
	
		for (deque<unsigned int>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_deque.erase(the_iterator);
			}
		}
	
		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;
		
		double total;
		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (vector<unsigned int>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
			{
				total += *the_iterator;
			}
		}
	
		difference1 = get_time_in_ms() - current_time;
		cout << "Totaling vector after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (colony<unsigned int>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end(); ++the_iterator)
			{
				total += *the_iterator;
			}
		}
	
		difference2 = get_time_in_ms() - current_time;
		cout << "Totaling colony after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (unsigned int num = 0; num != 5000000; ++num)
			{
				if (!(data_array[num].erased))
				{
					total += data_array[num].number;
				}
			}
		}

		difference3 = get_time_in_ms() - current_time;
		cout <<  "Totaling array after random erasures: " << total << endl << endl;

		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (deque<unsigned int>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
			{
				total += *the_iterator;
			}
		}
	
		difference4 = get_time_in_ms() - current_time;
		cout << "Totaling deque after random erasures: " << total << endl;
		

		cout << "Milliseconds to iterate over all entries 100 times and add to total:" << endl;
		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;
		cout << "Press Enter to Exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;

		delete [] data_array;
	}




	// 2nd "raw" test - plf_colony speed against vector, array with boolean field, with a generic small struct:
	{
		vector<generic> data_vector;
		deque<generic> data_deque;
		data_vector.reserve(500000);
		colony<generic> data_colony(500000);

		generic_for_array *data_array = new generic_for_array[5000000];


		cout << "Performance comparison between std::vector, plf::colony and an array using small structs." << endl;
		cout << "Press Enter to continue" << endl << endl;
		cin.get();

		cout << "Small struct tests begin:" << endl << endl;
		cout << "Milliseconds to add 5000000 generic structs:" << endl;
		
		one_sec_delay(); // To remove potential overhead from cout.

		unsigned long current_time, difference1, difference2, difference3, difference4;
		generic the_struct;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_struct.number = rand() % 5000000;
			data_vector.push_back(the_struct);
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_struct.number = rand() % 5000000;
			data_colony.add(the_struct);
		}

		difference2 = get_time_in_ms() - current_time;
		generic_for_array the_array_struct;
		the_array_struct.erased = false;

		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			
			the_array_struct.number = rand() % 5000000;
			memcpy(&(data_array[num]), &the_array_struct, sizeof(generic_for_array));
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			the_struct.number = rand() % 5000000;
			data_deque.push_back(the_struct);
		}

		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;

		cout << "Milliseconds to randomly erase 1 in every 5000 entries:" << endl;
		
		one_sec_delay();


		current_time = get_time_in_ms();

		for (vector<generic>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_vector.erase(the_iterator);
			}
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_colony.erase(the_iterator);
			}
		}

		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 5000000; ++num)
		{
			if (rand() % 5000 == 0)
			{
				data_array[num].erased = true;
			}
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (deque<generic>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end();)
		{
			if (rand() % 5000 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_deque.erase(the_iterator);
			}
		}

		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;

		

		double total;
		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (vector<generic>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference1 = get_time_in_ms() - current_time;
		cout << "Totaling vector after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (colony<generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}
		
		difference2 = get_time_in_ms() - current_time;
		cout << "Totaling colony after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (unsigned int num = 0; num != 5000000; ++num)
			{
				if (!(data_array[num].erased))
				{
					total += data_array[num].number;
				}
			}
		}

		difference3 = get_time_in_ms() - current_time;
		cout <<  "Totaling array after random erasures: " << total << endl << endl;
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 100; ++counter)
		{
			for (deque<generic>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference4 = get_time_in_ms() - current_time;
		cout << "Totaling deque after random erasures: " << total << endl;


		cout << "Milliseconds to iterate over all entries 100 times and add to total:" << endl;
		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;
		cout << "Press Enter to Exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;

		delete [] data_array;
	}



	// 3rd "raw" test: vector vs colony vs array with boolean field, for large generic struct:
	{
		vector<large_generic> data_vector;
		deque<large_generic> data_deque;
		data_vector.reserve(500);
		colony<large_generic> data_colony(500);

		large_generic_for_array *data_array = new large_generic_for_array[10000];


		cout << "Performance comparison between std::vector, std::deque, plf::colony and an array using a small number of large structs." << endl;
		cout << "Press Enter to continue" << endl << endl;
		cin.get();

		cout << "Large struct tests begin:" << endl << endl;
		cout << "Milliseconds to add 10000 large structs:" << endl;
  
		one_sec_delay(); // To remove potential overhead from cout.

		unsigned long current_time, difference1, difference2, difference3, difference4;
		large_generic the_struct;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 10000; ++num)
		{
			the_struct.number = rand() % 10000;
			data_vector.push_back(the_struct);
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 10000; ++num)
		{
			the_struct.number = rand() % 10000;
			data_colony.add(the_struct);
		}

		difference2 = get_time_in_ms() - current_time;
		large_generic_for_array the_array_struct;
		the_array_struct.erased = false;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 10000; ++num)
		{
			the_array_struct.number = rand() % 10000;
			memcpy(&data_array[num], &the_array_struct, sizeof(large_generic_for_array));
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 10000; ++num)
		{
			the_struct.number = rand() % 10000;
			data_deque.push_back(the_struct);
		}

		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;

		cout << "Milliseconds to randomly erase 1 in every 50 entries:" << endl;
		
		one_sec_delay();


		current_time = get_time_in_ms();

		for (vector<large_generic>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end();)
		{
			if (rand() % 50 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_vector.erase(the_iterator);
			}
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (colony<large_generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end();)
		{
			if (rand() % 50 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_colony.erase(the_iterator);
			}
		}

		difference2 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 10000; ++num)
		{
			if (rand() % 50 == 0)
			{
				data_array[num].erased = true;
			}
		}

		difference3 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();

		for (deque<large_generic>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end();)
		{
			if (rand() % 50 != 0)
			{
				++the_iterator;
			}
			else
			{
				the_iterator = data_deque.erase(the_iterator);
			}
		}

		difference4 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;

		

		double total;
		one_sec_delay();


		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 10000; ++counter)
		{
			for (vector<large_generic>::iterator the_iterator = data_vector.begin(); the_iterator != data_vector.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference1 = get_time_in_ms() - current_time;
		cout << "Totaling vector after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 10000; ++counter)
		{
			for (colony<large_generic>::iterator the_iterator = data_colony.begin(); the_iterator != data_colony.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}
		
		difference2 = get_time_in_ms() - current_time;
		cout << "Totaling colony after random erasures: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 10000; ++counter)
		{
			for (unsigned int num = 0; num != 10000; ++num)
			{
				if (!(data_array[num].erased))
				{
					total += data_array[num].number;
				}
			}
		}

		difference3 = get_time_in_ms() - current_time;
		cout <<  "Totaling array after random erasures: " << total << endl << endl;
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;

		for (unsigned int counter = 0; counter != 10000; ++counter)
		{
			for (deque<large_generic>::iterator the_iterator = data_deque.begin(); the_iterator != data_deque.end(); ++the_iterator)
			{
				total += the_iterator->number;
			}
		}

		difference4 = get_time_in_ms() - current_time;
		cout << "Totaling vector after random erasures: " << total << endl;
		
		cout << "Milliseconds to iterate over all entries 10000 times and add to total:" << endl;
		cout << "Vector: " << difference1 << endl << "Colony: " << difference2 << endl << "Array: " << difference3 << endl << "Deque: " << difference4 << endl << endl;
		cout << "Press Enter to Exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;
		
		delete [] data_array;
	}




	// 4th "raw" test - plf::stack vs vector, with unsigned ints:
	{
		vector<unsigned int> data_vector;
		data_vector.reserve(500000);
		stack<unsigned int> data_stack(500000);


		cout << "Performance comparison between std::vector, plf::stack using unsigned ints." << endl;
		cout << "Press Enter to continue" << endl << endl;
		cin.get();

		cout << "Unsigned int tests begin:" << endl << endl;
		cout << "Milliseconds to push 10000000 unsigned ints:" << endl;
		
		one_sec_delay(); // To remove potential overhead from cout.

		unsigned long current_time, difference1, difference2;
		current_time = get_time_in_ms();

		for (unsigned int num = 0; num != 10000000; ++num)
		{
			data_vector.push_back(rand() % 10000000);
		}

		difference1 = get_time_in_ms() - current_time;
		current_time = get_time_in_ms();


		for (unsigned int num = 0; num != 10000000; ++num)
		{
			data_stack.push(rand() % 10000000);
		}
	
		difference2 = get_time_in_ms() - current_time;

		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
		
		one_sec_delay();
		double total;

		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int num = 0; num != 10000000; ++num)
		{
			total += data_vector.back();
			data_vector.pop_back();
		}


		difference1 = get_time_in_ms() - current_time;
		cout << "Totaling vector: " << total << endl;
		
		one_sec_delay();

		current_time = get_time_in_ms();
		total = 0;
	
		for (unsigned int num = 0; num != 10000000; ++num)
		{
			total += data_stack.back();
			data_stack.pop();
		}


		difference2 = get_time_in_ms() - current_time;
 		cout << "Totaling stack: " << total << endl << endl;
 		cout << "Milliseconds to pop all elements and add to total:" << endl;
		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
		cout << "Press Enter to Exit";

		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;
	}




	// 5th "raw" test - plf::stack vs vector, with generic small structs:
	{
 		vector<generic> data_vector;
 		data_vector.reserve(500000);
 		stack<generic> data_stack(500000);
 
 
 		cout << "Performance comparison between std::vector, plf::stack using a generic struct." << endl;
 		cout << "Press Enter to continue" << endl << endl;
 		cin.get();
 
 		cout << "Small struct tests begin:" << endl << endl;
 		cout << "Milliseconds to push 5000000 generic structs:" << endl;
 		
 		one_sec_delay(); // To remove potential overhead from cout.
 
 		unsigned long current_time, difference1, difference2;
 		current_time = get_time_in_ms();
 		generic the_struct;
 
 		for (unsigned int num = 0; num != 5000000; ++num)
 		{
 			the_struct.number = rand() % 5000000;
 			data_vector.push_back(the_struct);
 		}
 	
 		difference1 = get_time_in_ms() - current_time;
 		current_time = get_time_in_ms();
 
 	
 		for (unsigned int num = 0; num != 5000000; ++num)
 		{
 			the_struct.number = rand() % 5000000;
 			data_stack.push(the_struct);
 		}
 	
 		difference2 = get_time_in_ms() - current_time;
 
 		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
 		
 		one_sec_delay();
 		double total;
 
 		current_time = get_time_in_ms();
 		total = 0;
 	
 		for (unsigned int num = 0; num != 5000000; ++num)
 		{
 			the_struct = data_vector.back();
 			total += the_struct.number;
 			data_vector.pop_back();
 		}
 
 
 		difference1 = get_time_in_ms() - current_time;
 		cout << "Totaling vector: " << total << endl;
 		
 		one_sec_delay();
 
 		current_time = get_time_in_ms();
 		total = 0;
 	
 		for (unsigned int num = 0; num != 5000000; ++num)
 		{
 			the_struct = data_stack.back();
 			total += the_struct.number;
 			data_stack.pop();
 		}
 
 	
 		difference2 = get_time_in_ms() - current_time;
 		cout << "Totaling stack: " << total << endl << endl;
 		cout << "Milliseconds to pop all elements and add to total:" << endl;
 		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
 		cout << "Press Enter to continue";
 	
 		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;
	}




	// 6th "raw" test - plf::stack vs vector, with a generic large structs:
	{
 		vector<large_generic> data_vector;
 		data_vector.reserve(500);
 		stack<large_generic> data_stack(500);
 
 
 		cout << "Performance comparison between std::vector, plf::stack using a large generic struct." << endl;
 		cout << "Press Enter to continue" << endl << endl;
 		cin.get();
 
 		cout << "Large struct tests begin:" << endl << endl;
 		cout << "Milliseconds to push 10000 large generic structs:" << endl;
 		
 		one_sec_delay(); // To remove potential overhead from cout.
 
 		unsigned long current_time, difference1, difference2;
 		current_time = get_time_in_ms();
 		large_generic the_struct;
 
 		for (unsigned int num = 0; num != 10000; ++num)
 		{
 			the_struct.number = rand() % 10000;
 			data_vector.push_back(the_struct);
 		}
 	
 		difference1 = get_time_in_ms() - current_time;
 		current_time = get_time_in_ms();
 
 	
 		for (unsigned int num = 0; num != 10000; ++num)
 		{
 			the_struct.number = rand() % 10000;
 			data_stack.push(the_struct);
 		}
 	
 		difference2 = get_time_in_ms() - current_time;
 
 		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
 		
 		one_sec_delay();
 		double total;

 		current_time = get_time_in_ms();
 		total = 0;
 	
 		for (unsigned int num = 0; num != 10000; ++num)
 		{
 			the_struct = data_vector.back();
 			total += the_struct.number;
 			data_vector.pop_back();
 		}
 
 
 		difference1 = get_time_in_ms() - current_time;
 		cout << "Totaling vector: " << total << endl;
   
 		one_sec_delay();
 
 		current_time = get_time_in_ms();
 		total = 0;
 	
 		for (unsigned int num = 0; num != 10000; ++num)
 		{
 			the_struct = data_stack.back();
 			total += the_struct.number;
 			data_stack.pop();
 		}

 	
 		difference2 = get_time_in_ms() - current_time;
 		cout << "Totaling stack: " << total << endl << endl;
 		cout << "Milliseconds to pop all elements and add to total:" << endl;
 		cout << "Vector: " << difference1 << endl << "Stack: " << difference2 << endl << endl;
 		cout << "Press Enter to continue";
 	
 		cin.get();
		cout << endl << endl << endl << endl << endl << endl << endl << endl;
	}




	return 0;
}