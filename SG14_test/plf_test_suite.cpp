#include <iostream>
#include <cstdio> // log redirection
#include <cstdlib> // rand
#include <ctime> // clock()
#include "plf_colony.h"


#define TITLE1(title_text) \
	std::cout << std::endl << std::endl << std::endl << "*** " << title_text << " ***" << std::endl; \
	std::cout << "===========================================" << std::endl << std::endl << std::endl;


#define TITLE2(title_text) \
	std::cout << std::endl << std::endl << "--- " << title_text << " ---" << std::endl << std::endl;


#define FAILPASS(test_type, condition) \
	std::cout << test_type << ": "; \
	\
	if (condition) \
	{ \
		std::cout << "Pass" << std::endl; \
	} \
	else \
	{ \
		std::cout << "Fail" << std::endl; \
		std::cin.get(); \
	}


namespace sg14_test
{

	void plf_test_suite()
	{
		freopen("error.log", "w", stderr);
		srand(clock()); // Note: using random numbers to avoid CPU predictive

		using namespace std;
		using namespace plf;


		unsigned int looper = 0;

		TITLE1("plf::colony and plf::stack tests - these tests will repeat 50 times in order to cover a wide variety of randomised scenarios. Please press ENTER.")
			cin.get();

		while (++looper != 50)
		{

			{
				TITLE1("Colony")
					TITLE2("Test Basics")

					colony<int *> p_colony;

				FAILPASS("Colony empty", p_colony.empty())

					int ten = 10;
				p_colony.add(&ten);

				FAILPASS("Colony not-empty", !p_colony.empty())

					TITLE2("Iterator tests")

					FAILPASS("Begin() working", **p_colony.begin() == 10)
					FAILPASS("End() working", p_colony.begin() != p_colony.end())


					p_colony.clear();

				FAILPASS("Begin = End after clear", p_colony.begin() == p_colony.end())

					int twenty = 20;

				for (unsigned int temp = 0; temp != 200; ++temp)
				{
					p_colony.add(&ten);
					p_colony.add(&twenty);
				}

				unsigned int total = 0, numtotal = 0;

				for (colony<int *>::iterator the_iterator = p_colony.begin(); the_iterator != p_colony.end(); ++the_iterator)
				{
					++total;
					numtotal += **the_iterator;
				}

				FAILPASS("Iteration count test", total == 400)
					FAILPASS("Iterator access test", numtotal == 6000)

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

				for (colony<int *>::iterator the_iterator = p_colony.begin(); the_iterator < p_colony.end(); the_iterator += 2)
				{
					++total;
					numtotal += **the_iterator;
				}

				FAILPASS("Multiple iteration test", total == 200)
					FAILPASS("Multiple iteration access test", numtotal == 2000)

					total = 0;

				for (colony<int *>::iterator the_iterator = p_colony.begin() + 1; the_iterator < p_colony.end(); ++the_iterator)
				{
					++total;
					the_iterator = p_colony.erase(the_iterator);
				}

				FAILPASS("Partial erase iteration test", total == 200)
					FAILPASS("Post-erase size test", p_colony.size() == 200)

					total = 0;

				for (colony<int *>::reverse_iterator the_iterator = p_colony.rbegin(); the_iterator != p_colony.rend();)
				{
					the_iterator = p_colony.erase(the_iterator.base() - 1);
					++the_iterator;
					++total;
				}

				FAILPASS("Full erase reverse iteration test", total == 200)
					FAILPASS("Post-erase size test", p_colony.size() == 0)

					for (unsigned int temp = 0; temp != 200; ++temp)
					{
						p_colony.add(&ten);
						p_colony.add(&twenty);
					}

				total = 0;

				for (colony<int *>::iterator the_iterator = p_colony.end() - 1; the_iterator != p_colony.begin(); --the_iterator)
				{
					++total;
				}

				FAILPASS("Negative iteration test", total == 399)


					total = 0;

				for (colony<int *>::iterator the_iterator = p_colony.end() - 1; the_iterator != p_colony.begin(); the_iterator -= 2)
				{
					++total;
				}

				FAILPASS("Negative multiple iteration test", total == 200)
			}


			{
				TITLE1("Add and Erase tests")

					colony<int> i_colony;

				for (unsigned int temp = 0; temp != 500000; ++temp)
				{
					i_colony.add(temp);
				}

				FAILPASS("Size after add test", i_colony.size() == 500000)

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
						i_colony.add(1);
					}

					FAILPASS("Size after reinitialize + add test", i_colony.size() == 30000)


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
							i_colony.add(1);
							++the_iterator;
						}
					}

					FAILPASS("Alternating add/erase test", i_colony.size() == 45001)


						do
						{
							for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end();)
							{
								if (rand() % 3 == 0)
								{
									++the_iterator;
									i_colony.add(1);
								}
								else
								{
									the_iterator = i_colony.erase(the_iterator);
								}
							}
						} while (!i_colony.empty());

						FAILPASS("Random add/erase till empty test", i_colony.size() == 0)


							for (unsigned int temp = 0; temp != 500000; ++temp)
							{
								i_colony.add(10);
							}

						FAILPASS("Add post-erase test", i_colony.size() == 500000)

							for (colony<int>::iterator the_iterator = i_colony.begin() + 250000; the_iterator != i_colony.end();)
							{
								the_iterator = i_colony.erase(the_iterator);
							}

						FAILPASS("Large multi-increment iterator test", i_colony.size() == 250000)


							for (unsigned int temp = 0; temp != 250000; ++temp)
							{
								i_colony.add(10);
							}

						colony<int>::iterator end_iterator = i_colony.end() - 250000;

						for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != end_iterator;)
						{
							the_iterator = i_colony.erase(the_iterator);
						}

						FAILPASS("Large multi-decrement iterator test", i_colony.size() == 250000)


							for (unsigned int temp = 0; temp != 250000; ++temp)
							{
								i_colony.add(10);
							}


						unsigned int total = 0;

						for (colony<int>::iterator the_iterator = i_colony.begin(); the_iterator != i_colony.end(); ++the_iterator)
						{
							total += *the_iterator;
						}

						FAILPASS("Re-add post-heavy-erasure test", total == 5000000)


							end_iterator = i_colony.end() - 1;
						end_iterator -= 50000;

						for (colony<int>::iterator the_iterator = i_colony.begin() + 300000; the_iterator != end_iterator;)
						{
							the_iterator = i_colony.erase(the_iterator);
						}

						FAILPASS("Non-end decrement + erase test", i_colony.size() == 350001)


							for (unsigned int temp = 0; temp != 100000; ++temp)
							{
								i_colony.add(10);
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
									i_colony.add(1);
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

						FAILPASS("Multiple sequential small add/erase commands test", count == i_colony.size())

			}



			{
				TITLE1("Stack Tests")

					stack<unsigned int> i_stack(50);

				for (unsigned int temp = 0; temp != 250000; ++temp)
				{
					i_stack.push(10);
				}

				FAILPASS("Multipush test", i_stack.size() == 250000)


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
	}

}