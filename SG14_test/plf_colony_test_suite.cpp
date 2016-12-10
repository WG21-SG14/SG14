#include <vector>
#include <iostream>
#include <algorithm>

#include "plf_colony.h"


#if defined(_MSC_VER)
	#define PLF_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1600
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1700
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
	#elif _MSC_VER == 1800
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
		#define PLF_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_INITIALIZER_LIST_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
	#define PLF_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4 // 4.3 and below do not support initializer lists
			#define PLF_INITIALIZER_LIST_SUPPORT
		#elif __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_INITIALIZER_LIST_SUPPORT
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
	#else // Assume type traits and initializer support for non-GCC compilers
		#define PLF_INITIALIZER_LIST_SUPPORT
		#define PLF_TYPE_TRAITS_SUPPORT
	#endif

	#define PLF_ALLOCATOR_TRAITS_SUPPORT
	#define PLF_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
	#define PLF_MOVE_SEMANTICS_SUPPORT
	#define PLF_NOEXCEPT noexcept
#else
	#define PLF_FORCE_INLINE
	#define PLF_NOEXCEPT throw()
#endif


namespace
{
    void title1(const char *title_text)
    {
        std::cout << std::endl << std::endl << std::endl << "*** " << title_text << " ***" << std::endl;
        std::cout << "===========================================" << std::endl << std::endl << std::endl;
    }

    void title2(const char *title_text)
    {
        std::cout << std::endl << std::endl << "--- " << title_text << " ---" << std::endl << std::endl;
    }


    void failpass(const char *test_type, bool condition)
    {
        std::cout << test_type << ": ";

        if (condition)
        {
            std::cout << "Pass" << std::endl;
        }
        else
        {
            std::cout << "Fail" << std::endl;
            std::cin.get();
            abort();
        }
    }


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
}


namespace sg14_test
{

void plf_colony_test_suite()
{
	using namespace std;
	using namespace plf;


	unsigned int looper = 0;


	while (++looper != 50)
	{
		{
			title1("Colony");
			title2("Test Basics");
			
			colony<int *> p_colony;
			
			failpass("Colony empty", p_colony.empty());
			
			int ten = 10;
			p_colony.insert(&ten);
			
			failpass("Colony not-empty", !p_colony.empty());

			title2("Iterator tests");
			
			failpass("Begin() working", **p_colony.begin() == 10);
			failpass("End() working", p_colony.begin() != p_colony.end());
			

			p_colony.clear();

			failpass("Begin = End after clear", p_colony.begin() == p_colony.end());

			int twenty = 20;
			
			for (unsigned int temp = 0; temp != 200; ++temp)
			{
				p_colony.insert(&ten);
				p_colony.insert(&twenty);
			}
			
			unsigned int total = 0, numtotal = 0;
			
			for(colony<int *>::iterator the_iterator = p_colony.begin(); the_iterator != p_colony.end(); ++the_iterator)
			{
				++total;
				numtotal += **the_iterator;
			}
			
			failpass("Iteration count test", total == 400);
			failpass("Iterator access test", numtotal == 6000);

			colony<int *>::iterator plus_twenty = p_colony.begin();
			p_colony.advance(plus_twenty, 20);
			colony<int *>::iterator plus_two_hundred = p_colony.begin();
			p_colony.advance(plus_two_hundred, 200);
			
			failpass("Iterator + distance test", p_colony.distance(p_colony.begin(), plus_twenty) == 20);
			failpass("Iterator - distance test", p_colony.distance(plus_two_hundred, p_colony.begin()) == -200);
			
			colony<int *>::iterator next_iterator = p_colony.next(p_colony.begin(), 5);
			colony<int *>::const_iterator prev_iterator = p_colony.prev(p_colony.cend(), 300);
			
			failpass("Iterator next test", p_colony.distance(p_colony.begin(), next_iterator) == 5);
			failpass("Const iterator prev test", p_colony.distance(p_colony.cend(), prev_iterator) == -300);
			#if defined(__cplusplus) && __cplusplus >= 201402L
				colony<int *>::iterator prev_iterator2 = p_colony.prev(p_colony.end(), 300);
				failpass("Iterator/Const iterator equality operator test", prev_iterator == prev_iterator2);
			#endif
			
			
			colony<int *> p_colony2 = p_colony;
			colony<int *> p_colony3(p_colony);
			
			failpass("Copy test", p_colony2.size() == 400);
			failpass("Copy construct test", p_colony3.size() == 400);

			failpass("Equality operator test", p_colony == p_colony2);
			failpass("Equality operator test 2", p_colony2 == p_colony3);
			
			p_colony2.insert(&ten);
			
			failpass("Inequality operator test", p_colony2 != p_colony3);

			numtotal = 0;
			total = 0;
			
			for (colony<int *>::reverse_iterator the_iterator = p_colony.rbegin(); the_iterator != p_colony.rend(); ++the_iterator)
			{
				++total;
				numtotal += **the_iterator;
			}


			failpass("Reverse iteration count test", total == 400);
			failpass("Reverse iterator access test", numtotal == 6000);
			
			colony<int *>::reverse_iterator r_iterator = p_colony.rbegin();
			p_colony.advance(r_iterator, 50);
			
			failpass("Reverse iterator advance and distance test", p_colony.distance(p_colony.rbegin(), r_iterator) == -50);

			colony<int *>::reverse_iterator r_iterator2 = p_colony.next(r_iterator, 2);

			failpass("Reverse iterator next and distance test", p_colony.distance(p_colony.rbegin(), r_iterator2) == -52);

			numtotal = 0;
			total = 0;

			for(colony<int *>::iterator the_iterator = p_colony.begin(); the_iterator < p_colony.end(); p_colony.advance(the_iterator, 2))
			{
				++total;
				numtotal += **the_iterator;
			}

			failpass("Multiple iteration test", total == 200);
			failpass("Multiple iteration access test", numtotal == 2000);

			numtotal = 0;
			total = 0;

			for(colony<int *>::const_iterator the_iterator = p_colony.cbegin(); the_iterator != p_colony.cend(); ++the_iterator)
			{
				++total;
				numtotal += **the_iterator;
			}

			failpass("Const_iterator test", total == 400);
			failpass("Const_iterator access test", numtotal == 6000);


			numtotal = 0;
			total = 0;

			for(colony<int *>::const_reverse_iterator the_iterator = --colony<int *>::const_reverse_iterator(p_colony.crend()); the_iterator != colony<int *>::const_reverse_iterator(p_colony.crbegin()); --the_iterator)
			{
				++total;
				numtotal += **the_iterator;
			}

			failpass("Const_reverse_iterator -- test", total == 399);
			failpass("Const_reverse_iterator -- access test", numtotal == 5980);

			total = 0;
			
			for(colony<int *>::iterator the_iterator = ++colony<int *>::iterator(p_colony.begin()); the_iterator < p_colony.end(); ++the_iterator)
			{
				++total;
				the_iterator = p_colony.erase(the_iterator);
			}

			failpass("Partial erase iteration test", total == 200);
			failpass("Post-erase size test", p_colony.size() == 200);
			
			const unsigned int temp_capacity = static_cast<unsigned int>(p_colony.capacity());
			p_colony.shrink_to_fit();
			failpass("Shrink_to_fit test", p_colony.capacity() < temp_capacity);
			failpass("Shrink_to_fit test 2", p_colony.capacity() == 200);
			
			total = 0;

			for(colony<int *>::reverse_iterator the_iterator = p_colony.rbegin(); the_iterator != p_colony.rend(); ++the_iterator)
			{
				colony<int *>::iterator it = the_iterator.base();
				the_iterator = p_colony.erase(--it);
				++total;
			}

			failpass("Full erase reverse iteration test", total == 200);
			failpass("Post-erase size test", p_colony.size() == 0);

			for (unsigned int temp = 0; temp != 200; ++temp)
			{
				p_colony.insert(&ten);
				p_colony.insert(&twenty);
			}
			
			total = 0;

			for(colony<int *>::iterator the_iterator = --colony<int *>::iterator(p_colony.end()); the_iterator != p_colony.begin(); --the_iterator)
			{
				++total;
			}

			failpass("Negative iteration test", total == 399);


			total = 0;

			for(colony<int *>::iterator the_iterator = --(colony<int *>::iterator(p_colony.end())); the_iterator != p_colony.begin(); p_colony.advance(the_iterator, -2))
			{
				++total;
			}

			failpass("Negative multiple iteration test", total == 200);
			
			#ifdef PLF_MOVE_SEMANTICS_SUPPORT
				p_colony2 = std::move(p_colony);
				failpass("Move test", p_colony2.size() == 400);
			#else
				p_colony2 = p_colony;
			#endif

			p_colony3 = p_colony2;
			
			failpass("Copy test 2", p_colony3.size() == 400);
			
			p_colony2.insert(&ten);

			p_colony2.swap(p_colony3);

			failpass("Swap test", p_colony2.size() == p_colony3.size() - 1);
			failpass("max_size() test", p_colony2.max_size() > p_colony2.size());
			
		}

		
		{
			title2("Insert and Erase tests");
			
			colony<int> i_colony;

			for (unsigned int temp = 0; temp != 500000; ++temp)
			{
				i_colony.insert(temp);
			}
			
			
			failpass("Size after insert test", i_colony.size() == 500000);


			colony<int>::iterator found_item = std::find(i_colony.begin(), i_colony.end(), 5000);;
			
			failpass("std::find iterator test", *found_item == 5000);
			
			
			colony<int>::reverse_iterator found_item2 = std::find(i_colony.rbegin(), i_colony.rend(), 5000);;
			
			failpass("std::find reverse_iterator test", *found_item2 == 5000);
			
			
			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end(); ++the_iterator)
			{
				the_iterator = i_colony.erase(the_iterator);
			}

			failpass("Erase alternating test", i_colony.size() == 250000);

			do
			{
				for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
				{
					if ((xor_rand() & 7) == 0)
					{
						the_iterator = i_colony.erase(the_iterator);
					}
					else
					{
						++the_iterator;
					}
				}
				
			} while (!i_colony.empty());
			
			failpass("Erase randomly till-empty test", i_colony.size() == 0);


			i_colony.clear();
			i_colony.change_minimum_group_size(10000);
			
			for (unsigned int temp = 0; temp != 30000; ++temp)
			{
				i_colony.insert(1);
			}
			
			failpass("Size after reinitialize + insert test", i_colony.size() == 30000);

			unsigned short count2 = 0;
			
			do
			{
				for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
				{
					if ((xor_rand() & 7) == 0)
					{
						the_iterator = i_colony.erase(the_iterator);
						++count2;
					}
					else
					{
						++the_iterator;
					}
				}
				
			} while (count2 < 15000);
			
			failpass("Erase randomly till half-empty test", i_colony.size() == 30000u - count2);

			for (unsigned int temp = 0; temp != count2; ++temp)
			{
				i_colony.insert(1);
			}
			
			failpass("Size after reinsert test", i_colony.size() == 30000);




			unsigned int sum = 0;

			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
			{
				if (++sum == 3)
				{
					sum = 0;
					the_iterator = i_colony.erase(the_iterator);
				}
				else
				{
					i_colony.insert(1);
					++the_iterator;
				}
			}
			
			failpass("Alternating insert/erase test", i_colony.size() == 45001);

			
			do
			{
				for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
				{
					if ((xor_rand() & 3) == 0)
					{
						++the_iterator;
						i_colony.insert(1);
					}
					else
					{
						the_iterator = i_colony.erase(the_iterator);
					}
				}
			} while (!i_colony.empty());;
			
			failpass("Random insert/erase till empty test", i_colony.size() == 0);

			
			for (unsigned int temp = 0; temp != 500000; ++temp)
			{
				i_colony.insert(10);
			}

			failpass("Insert post-erase test", i_colony.size() == 500000);
			colony<int>::iterator the_iterator = i_colony.begin();
			i_colony.advance(the_iterator, 250000);
			

			for (; the_iterator != i_colony.end();)
			{
				the_iterator = i_colony.erase(the_iterator);
			}
			
			failpass("Large multi-increment iterator test", i_colony.size() == 250000);

			
			for (unsigned int temp = 0; temp != 250000; ++temp)
			{
				i_colony.insert(10);
			}
			
			colony<int>::iterator end_iterator = i_colony.end();
			i_colony.advance(end_iterator, -250000);
			
			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != end_iterator;)
			{
				the_iterator = i_colony.erase(the_iterator);
			}

			failpass("Large multi-decrement iterator test", i_colony.size() == 250000);

			
			for (unsigned int temp = 0; temp != 250000; ++temp)
			{
				i_colony.insert(10);
			}
			
			
			unsigned int total = 0;
			
			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end(); ++the_iterator)
			{
				total += *the_iterator;
			}

			failpass("Re-insert post-heavy-erasure test", total == 5000000);
			

			end_iterator = i_colony.end();
			i_colony.advance(end_iterator, -50001);
			colony<int>::iterator begin_iterator = i_colony.begin();
			i_colony.advance(begin_iterator, 300000);

			for (colony<int>::iterator the_iterator = begin_iterator; the_iterator != end_iterator;)
			{
				the_iterator = i_colony.erase(the_iterator);
			}

			failpass("Non-end decrement + erase test", i_colony.size() == 350001);
			

			for (unsigned int temp = 0; temp != 100000; ++temp)
			{
				i_colony.insert(10);
			}
			
			begin_iterator = i_colony.begin();
			i_colony.advance(begin_iterator, 300001);
			
			
			for (colony<int>::iterator the_iterator = begin_iterator; the_iterator != i_colony.end();)
			{
				the_iterator = i_colony.erase(the_iterator);
			}
			
			failpass("Non-beginning increment + erase test", i_colony.size() == 300001);

			colony<int>::iterator temp_iterator = i_colony.begin();
			i_colony.advance(temp_iterator, 2); // Advance test 1

			unsigned int index = static_cast<unsigned int>(i_colony.get_index_from_iterator(temp_iterator));
			failpass("Advance + iterator-to-index test", index == 2);

			i_colony.erase(temp_iterator);
			temp_iterator = i_colony.begin(); // Check edge-case with advance when erasures present in initial group
			i_colony.advance(temp_iterator, 500);

			index = static_cast<unsigned int>(i_colony.get_index_from_iterator(temp_iterator));

			failpass("Advance + iterator-to-index test", index == 500);

			colony<int>::iterator temp2 = i_colony.get_iterator_from_pointer(&(*temp_iterator));
			
			failpass("Pointer-to-iterator test", temp2 != i_colony.end());
			
			temp2 = i_colony.get_iterator_from_index(500);
			
			failpass("Index-to-iterator test", temp2 == temp_iterator);


			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
			{
				the_iterator = i_colony.erase(the_iterator);
			}
			
			failpass("Total erase test", i_colony.empty());
			
			
			i_colony.clear();
			i_colony.change_minimum_group_size(3);

			const unsigned int temp_capacity2 = static_cast<unsigned int>(i_colony.capacity());
			i_colony.reserve(1000);
			failpass("Colony reserve test", temp_capacity2 != i_colony.capacity());
			failpass("Colony reserve test2", i_colony.capacity() == 1000);

			unsigned int count = 0;

			for (unsigned int loop1 = 0; loop1 != 50000; ++loop1)
			{
				for (unsigned int loop = 0; loop != 10; ++loop)
				{
					if ((xor_rand() & 7) == 0)
					{
						i_colony.insert(1);
						++count;
					}
				}

				for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
				{
					if ((xor_rand() & 7) == 0)
					{
						the_iterator = i_colony.erase(the_iterator);
						--count;
					}
					else
					{
						++the_iterator;
					}
				}
			}

			failpass("Multiple sequential small insert/erase commands test", count == i_colony.size());
		}

		{
			title2("Range-erase tests");
		
			colony<int> i_colony;
			
			for (int counter = 0; counter != 1000; ++counter)
			{
				i_colony.insert(counter);
			}
			
			
			colony<int>::iterator it1 = i_colony.begin(), it2 = i_colony.begin();
			
			i_colony.advance(it1, 500);
			i_colony.advance(it2, 800);
			
			i_colony.erase(it1, it2);
			
			unsigned int counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Simply range-erase test 1", counter == 700 && i_colony.size() == 700);

		
			it1 = it2 = i_colony.begin();
			
			i_colony.advance(it1, 400);
			i_colony.advance(it2, 500); // This should put it2 past the point of previous erasures
			
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Simply range-erase test 2", counter == 600 && i_colony.size() == 600);

			

			it2 = it1 = i_colony.begin();
			
			i_colony.advance(it1, 4);
			i_colony.advance(it2, 9); // This should put it2 past the point of previous erasures
			
			i_colony.erase(it1, it2);

			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Simple range-erase test 3", counter == 595 && i_colony.size() == 595);

			


			it2 = it1 = i_colony.begin();
			
			i_colony.advance(it2, 50); 
			
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Range-erase from begin() test 1", counter == 545 && i_colony.size() == 545);




			it1 = i_colony.begin();
			it2 = i_colony.end();
			
			i_colony.advance(it1, 345); // Test erasing and validity when it removes the final group in colony
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Range-erase to end() test 1", counter == 345 && i_colony.size() == 345);



			i_colony.clear();

			for (counter = 0; counter != 3000; ++counter)
			{
				i_colony.insert(counter);
			}
			
			for (colony<int>::iterator it = i_colony.begin(); it < i_colony.end(); ++it)
			{
				it = i_colony.erase(it);
			}
			
			it2 = it1 = i_colony.begin();
			
			i_colony.advance(it1, 4);
			i_colony.advance(it2, 600);
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Range-erase with colony already half-erased, alternating erasures", counter == 904 && i_colony.size() == 904);



			i_colony.clear();

			for (counter = 0; counter != 3000; ++counter)
			{
				i_colony.insert(counter);
			}
			
			for (colony<int>::iterator it = i_colony.begin(); it < i_colony.end(); ++it)
			{
				if ((xor_rand() & 1) == 0)
				{
					it = i_colony.erase(it);
				}
			}
			
			it1 = i_colony.begin();
			it2 = i_colony.end();
			
			i_colony.advance(it1, 400);
			i_colony.erase(it1, it2);

			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Range-erase with colony already third-erased, randomizes erasures", counter == 400 && i_colony.size() == 400);



			unsigned int size, range1, range2, internal_loop_counter;

			std::cout << "Fuzz-test range-erase randomly until empty: ";

			for (unsigned int loop_counter = 0; loop_counter != 50; ++loop_counter)
			{
				i_colony.clear();

				for (counter = 0; counter != 1000; ++counter)
				{
					i_colony.insert(counter);
				}
				
				internal_loop_counter = 0;

				while (!i_colony.empty())
				{
					it2 = it1 = i_colony.begin();

					size = static_cast<unsigned int>(i_colony.size());
					range1 = xor_rand() % size;
					range2 = range1 + 1 + (xor_rand() % (size - range1));
					i_colony.advance(it1, range1);
					i_colony.advance(it2, range2);

					i_colony.erase(it1, it2);

					counter = 0;

					for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
					{
						++counter;
					}

					if (i_colony.size() != counter)
					{
						std::cout << "Fail. loop counter: " << loop_counter << ", internal_loop_counter: " << internal_loop_counter << "." << std::endl;
						std::cin.get(); 
						abort(); 
					}
					
					if (i_colony.size() > 2)
					{ // Test to make sure our stored erased_locations are valid
						i_colony.insert(1);
						i_colony.insert(10);
					}
	
					++internal_loop_counter;
				}
			}

			if (i_colony.size() == 0)
			{
				std::cout << "Pass\n";
			}
		}

		{
			title2("Different insertion-style tests");

			#ifdef PLF_INITIALIZER_LIST_SUPPORT
				colony<int> i_colony = {1, 2, 3};
			#else
				colony<int> i_colony(3, 1);
			#endif
			
			failpass("Initializer-list constructor test", i_colony.size() == 3);
			
			colony<int> i_colony2(i_colony.begin(), i_colony.end());
			
			failpass("Range-based constructor test", i_colony2.size() == 3);
			
			colony<int> i_colony3(5000, 2, 100, 1000);
			
			failpass("Fill construction test", i_colony3.size() == 5000);
			
			i_colony2.insert(500000, 5);
			
			failpass("Fill insertion test", i_colony2.size() == 500003);
			
			std::vector<int> some_ints(500, 2);
			
			i_colony2.insert(some_ints.begin(), some_ints.end());
			
			failpass("Fill insertion test", i_colony2.size() == 500503);
		}
	}

	title1("Test Suite PASS - Press ENTER to Exit");
	cin.get();
}

}
