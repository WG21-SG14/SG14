#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio> // log redirection
#include <cstdlib> // rand
#include <ctime> // timer
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


namespace sg14_test
{
	
void plf_test_suite()
{
	{
		time_t timer;
		time(&timer);
		srand((unsigned int)timer); // Note: using random numbers to avoid CPU prediction
	}

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
		title1("Insert and Erase tests");
		
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
				if (rand() % 5 == 0)
				{
					the_iterator = i_colony.erase(the_iterator);
				}
				else
				{
					colony<int>::iterator temp_iterator = the_iterator;
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
				if (rand() % 5 == 0)
				{
					the_iterator = i_colony.erase(the_iterator);
					++count2;
				}
				else
				{
					colony<int>::iterator temp_iterator = the_iterator;
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
				if (rand() % 3 == 0)
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
		i_colony.advance(begin_iterator, 300000);
		
		
		for (colony<int>::iterator the_iterator = begin_iterator; the_iterator != i_colony.end();)
		{
			the_iterator = i_colony.erase(the_iterator);
		}
		
		failpass("Non-beginning increment + erase test", i_colony.size() == 300000);
		

		colony<int>::iterator temp_iterator = i_colony.begin();
		i_colony.advance(temp_iterator, 500);
		
		unsigned int return_code = i_colony.erase(&(*temp_iterator));
		
		failpass("Pointer-based erase test", return_code == 0 && i_colony.size() == 299999);


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
				if (rand() % 5 == 0)
				{
					i_colony.insert(1);
					++count;
				}
			}

			for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
			{
				if (rand() % 5 == 0)
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
		title1("Different insertion-style tests");
		
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

	{
		title1("Stack Tests");

		stack<unsigned int> i_stack(50);
		
		for (unsigned int temp = 0; temp != 250000; ++temp)
		{
			i_stack.push(10);
		}
		
		failpass("Multipush test", i_stack.size() == 250000);
		
		stack<unsigned int> i_stack2;
		i_stack2 = i_stack;

		stack<unsigned int> i_stack3(i_stack);

		failpass("Copy constructor test", i_stack3.size() == 250000);

		i_stack3.reserve(400000);

		failpass("Reserve test", i_stack3.size() == 250000);


		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			stack<unsigned int> i_stack4 = std::move(i_stack3);
			failpass("Move equality operator test", i_stack2 == i_stack4);
            i_stack3 = std::move(i_stack4);
		#else
			failpass("Equality operator test", i_stack2 == i_stack3);
    	#endif

		failpass("Copy test", i_stack2.size() == 250000);
		failpass("Equality operator test 2", i_stack == i_stack2);
        
        i_stack2.push(5);
        i_stack2.swap(i_stack3);
        
		failpass("Swap test", i_stack2.size() == i_stack3.size() - 1);
		failpass("max_size() test", i_stack2.max_size() > i_stack2.size());
        

		unsigned int total = 0;

		const unsigned int temp_capacity = static_cast<unsigned int>(i_stack.capacity());

		for (unsigned int temp = 0; temp != 200000; ++temp)
		{
			total += i_stack.top();
			i_stack.pop();
		}

		failpass("Multipop test", i_stack.size() == 50000);
		failpass("top() test", total == 2000000);

		i_stack.shrink_to_fit();

		failpass("shrink_to_fit() test", temp_capacity != i_stack.capacity());


		do
		{
			if (rand() % 5 == 0)
			{
				i_stack.push(10);
			}
			else
			{
				i_stack.pop();
			}
		} while (!i_stack.empty());;
		
		failpass("Randomly pop/push till empty test", i_stack.size() == 0);
	}
	{
		title1("Stack Special Case Tests");

		stack<unsigned int> i_stack(50, 100);
		
		for (unsigned int temp = 0; temp != 256; ++temp)
		{
			i_stack.push(10);
		}
		
		stack<unsigned int> i_stack_copy(i_stack);
		
		int temp2 = 0;
		
		for (unsigned int temp = 0; temp != 256; ++temp)
		{
			temp2 += i_stack_copy.top();
			i_stack_copy.pop();
		}
		
		failpass("Stack copy special case test", temp2 == 2560);
	}
	}

	title1("Test Suite PASS - Press ENTER to Exit");
	cin.get();

  return;
}

}
