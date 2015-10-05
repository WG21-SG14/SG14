#include <iostream>
#include <algorithm>
#include <cstdio> // log redirection
#include <cstdlib> // rand
#include <ctime> // timer
#include "plf_colony.h"


#define TITLE1(title_text) \
	std::cout << std::endl << std::endl << std::endl << "*** " << title_text << " ***" << std::endl; \
	std::cout << "===========================================" << std::endl << std::endl << std::endl; 

#define TITLE2(title_text) \
	std::cout << std::endl << std::endl << "--- " << title_text << " ---" << std::endl << std::endl; 

	
#define PASS std::cout << "Pass" << std::endl;

#define FAIL std::cout << "Fail" << std::endl;

#define FAILPASS(test_type, condition) \
	std::cout << test_type << ": "; \
	\
	if (condition) \
	{ \
		PASS \
	} \
	else \
	{ \
		FAIL \
		std::cin.get(); \
		abort(); \
	}



namespace sg14_test
{

void plf_test_suite()
{
	srand(clock()); // Note: using random numbers to avoid CPU predictive

	using namespace std;
	using namespace plf;


	unsigned int looper = 0;
	
	while (++looper != 50)
	{

	{
		TITLE1("Colony")
		TITLE2("Test Basics")
		
		colony<int *> p_colony;
		
		FAILPASS("Colony empty", p_colony.empty())
		
		int ten = 10;
		p_colony.insert(&ten);
		
		FAILPASS("Colony not-empty", !p_colony.empty())
	
		TITLE2("Iterator tests")
		
		FAILPASS("Begin() working", **p_colony.begin() == 10)
		FAILPASS("End() working", p_colony.begin() != p_colony.end())
		

		p_colony.clear();

		FAILPASS("Begin = End after clear", p_colony.begin() == p_colony.end())

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
		
		FAILPASS("Iteration count test", total == 400)
		FAILPASS("Iterator access test", numtotal == 6000)

		FAILPASS("Iterator + Iterator test", (p_colony.begin() + (p_colony.begin() + 20)) == 20);
		FAILPASS("Iterator - Iterator test", ((p_colony.begin() + 200) - p_colony.begin() == 200));
		
		colony<int *> p_colony2 = p_colony;
		colony<int *> p_colony3(p_colony);
		
		FAILPASS("Copy test", p_colony2.size() == 400)
		FAILPASS("Copy construct test", p_colony3.size() == 400)
		
		FAILPASS("Equality operator test", p_colony == p_colony2)
		FAILPASS("Equality operator test 2", p_colony2 == p_colony3)
		
		p_colony2.insert(&ten);
		
		FAILPASS("Inequality operator test", p_colony2 != p_colony3)

		numtotal = 0;
		total = 0;
		
		for (colony<int *>::reverse_iterator the_iterator = p_colony.rbegin(); the_iterator != p_colony.rend(); ++the_iterator)
		{
			++total;
			numtotal += **the_iterator;
		}


		FAILPASS("Reverse iteration count test", total == 400)
		FAILPASS("Reverse iterator access test", numtotal == 6000)
		
		numtotal = 0;
		total = 0;

		for(colony<int *>::iterator the_iterator = p_colony.begin(); the_iterator < p_colony.end(); the_iterator += 2)
		{
			++total;
			numtotal += **the_iterator;
		}

		FAILPASS("Multiple iteration test", total == 200)
		FAILPASS("Multiple iteration access test", numtotal == 2000)

		numtotal = 0;
		total = 0;

		for(colony<int *>::const_iterator the_iterator = p_colony.cbegin(); the_iterator != p_colony.cend(); ++the_iterator)
		{
			++total;
			numtotal += **the_iterator;
		}

		FAILPASS("Const_iterator test", total == 400)
		FAILPASS("Const_iterator access test", numtotal == 6000)


		numtotal = 0;
		total = 0;

		for(colony<int *>::const_reverse_iterator the_iterator = p_colony.crend() - 1; the_iterator != p_colony.crbegin() - 1; --the_iterator)
		{
			++total;
			numtotal += **the_iterator;
		}

		FAILPASS("Const_reverse_iterator -- test", total == 400)
		FAILPASS("Const_reverse_iterator -- access test", numtotal == 6000)

		total = 0;
		
		for(colony<int *>::iterator the_iterator = p_colony.begin() + 1; the_iterator < p_colony.end(); ++the_iterator)
		{
			++total;
			the_iterator = p_colony.erase(the_iterator);
		}

		FAILPASS("Partial erase iteration test", total == 200)
		FAILPASS("Post-erase size test", p_colony.size() == 200)

		total = 0;

		for(colony<int *>::reverse_iterator the_iterator = p_colony.rbegin(); the_iterator != p_colony.rend(); ++the_iterator)
		{
			colony<int *>::iterator it = the_iterator.base();
			the_iterator = p_colony.erase(--it);
			++total;
		}

		FAILPASS("Full erase reverse iteration test", total == 200)
		FAILPASS("Post-erase size test", p_colony.size() == 0)

		for (unsigned int temp = 0; temp != 200; ++temp)
		{
			p_colony.insert(&ten);
			p_colony.insert(&twenty);
		}
		
		total = 0;

		for(colony<int *>::iterator the_iterator = p_colony.end() - 1; the_iterator != p_colony.begin(); --the_iterator)
		{
			++total;
		}
		
		FAILPASS("Negative iteration test", total == 399)


		total = 0;

		for(colony<int *>::iterator the_iterator = p_colony.end() - 1; the_iterator != p_colony.begin(); the_iterator -= 2)
		{
			++total;
		}

		FAILPASS("Negative multiple iteration test", total == 200)
	}
	
	
	{
		TITLE1("Insert and Erase tests")
		
		colony<int> i_colony;

		for (unsigned int temp = 0; temp != 500000; ++temp)
		{
			i_colony.insert(temp);
		}
		
		
		FAILPASS("Size after insert test", i_colony.size() == 500000)


		colony<int>::iterator found_item = std::find(i_colony.begin(), i_colony.end(), 5000);
		
		FAILPASS("std::find iterator test", *found_item == 5000)
		
		
		colony<int>::reverse_iterator found_item2 = std::find(i_colony.rbegin(), i_colony.rend(), 5000);
		
		FAILPASS("std::find reverse_iterator test", *found_item2 == 5000)
		
		
		for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end(); ++the_iterator)
		{
			the_iterator = i_colony.erase(the_iterator);
		}

		FAILPASS("Erase alternating test", i_colony.size() == 250000)

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
					++the_iterator;
				}
			}
			
		} while (!i_colony.empty());
		
		FAILPASS("Erase randomly till-empty test", i_colony.size() == 0)


		i_colony.reinitialize(10000);
		
		for (unsigned int temp = 0; temp != 30000; ++temp)
		{
			i_colony.insert(1);
		}
		
		FAILPASS("Size after reinitialize + insert test", i_colony.size() == 30000)


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
		
		FAILPASS("Alternating insert/erase test", i_colony.size() == 45001)

		
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
		} while (!i_colony.empty());
		
		FAILPASS("Random insert/erase till empty test", i_colony.size() == 0)

		
		for (unsigned int temp = 0; temp != 500000; ++temp)
		{
			i_colony.insert(10);
		}
		
		FAILPASS("Insert post-erase test", i_colony.size() == 500000)

		for (colony<int>::iterator the_iterator = i_colony.begin() + 250000; the_iterator != i_colony.end();)
		{
			the_iterator = i_colony.erase(the_iterator);
		}
		
		FAILPASS("Large multi-increment iterator test", i_colony.size() == 250000)

		
		for (unsigned int temp = 0; temp != 250000; ++temp)
		{
			i_colony.insert(10);
		}
		
		colony<int>::iterator end_iterator = i_colony.end() - 250000;
		
		for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != end_iterator;)
		{
			the_iterator = i_colony.erase(the_iterator);
		}

		FAILPASS("Large multi-decrement iterator test", i_colony.size() == 250000)

		
		for (unsigned int temp = 0; temp != 250000; ++temp)
		{
			i_colony.insert(10);
		}
		
		
		unsigned int total = 0;
		
		for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end(); ++the_iterator)
		{
			total += *the_iterator;
		}

		FAILPASS("Re-insert post-heavy-erasure test", total == 5000000)
		

		end_iterator = i_colony.end() - 1;
		end_iterator -= 50000;

		for (colony<int>::iterator the_iterator = i_colony.begin() + 300000; the_iterator != end_iterator;)
		{
			the_iterator = i_colony.erase(the_iterator);
		}

		FAILPASS("Non-end decrement + erase test", i_colony.size() == 350001)
		

		for (unsigned int temp = 0; temp != 100000; ++temp)
		{
			i_colony.insert(10);
		}
		
		colony<int>::iterator begin_iterator = i_colony.begin() + 2;
		begin_iterator += 299998;
		
		
		for (colony<int>::iterator the_iterator = begin_iterator; the_iterator != i_colony.end();)
		{
			the_iterator = i_colony.erase(the_iterator);
		}
		
		FAILPASS("Non-beginning increment + erase test", i_colony.size() == 300000)
		
		for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
		{
			the_iterator = i_colony.erase(the_iterator);
		}
		
		FAILPASS("Final erase test", i_colony.empty())
		
		
		i_colony.reinitialize(3);

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
		
		FAILPASS("Multiple sequential small insert/erase commands test", count == i_colony.size())
		
	}



	{
		TITLE1("Stack Tests")

		stack<unsigned int> i_stack(50);
		
		for (unsigned int temp = 0; temp != 250000; ++temp)
		{
			i_stack.push(10);
		}
		
		FAILPASS("Multipush test", i_stack.size() == 250000)
		
		stack<unsigned int> i_stack2 = i_stack;
		stack<unsigned int> i_stack3(i_stack);
		
		FAILPASS("Copy test", i_stack2.size() == 250000)
		FAILPASS("Copy constructor test", i_stack3.size() == 250000)

		FAILPASS("Equality operator test", i_stack == i_stack2);
		FAILPASS("Equality operator test 2", i_stack2 == i_stack3);
		
		
		unsigned int total = 0;
		
		for (unsigned int temp = 0; temp != 200000; ++temp)
		{
			total += i_stack.back();
			i_stack.pop();
		}

		FAILPASS("Multipop test", i_stack.size() == 50000)
		FAILPASS("Back() test", total == 2000000)

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
		} while (!i_stack.empty());
		
		FAILPASS("Randomly pop/push till empty test", i_stack.size() == 0)
	}

	}

	TITLE1("Test Suite PASS - Press ENTER to Exit")
	cin.get();

	return 0;
}

}