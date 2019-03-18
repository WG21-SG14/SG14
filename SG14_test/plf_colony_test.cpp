#define PLF_COLONY_TEST_DEBUG

#if defined(_MSC_VER)
	#define PLF_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_NOEXCEPT throw()
		#define PLF_NOEXCEPT_SWAP(the_allocator)
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1600
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
		#define PLF_NOEXCEPT_SWAP(the_allocator)
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1700
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
		#define PLF_NOEXCEPT_SWAP(the_allocator)
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1800
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT throw()
		#define PLF_NOEXCEPT_SWAP(the_allocator)
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
		#define PLF_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_INITIALIZER_LIST_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L
	#define PLF_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_VARIADICS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || __GNUC__ < 4
			#define PLF_NOEXCEPT throw()
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_NOEXCEPT_SWAP(the_allocator)
		#elif __GNUC__ < 6
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept
		#else // C++17 support
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif

	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#if __GLIBCXX__ >= 20080606 	// libstdc++ 4.2 and below do not support variadic templates
			#define PLF_VARIADICS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20090421 	// libstdc++ 4.3 and below do not support initializer lists
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#elif __GLIBCXX__ >= 20120322
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_NOEXCEPT noexcept
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept
		#else
			#define PLF_NOEXCEPT throw()
			#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_NOEXCEPT_SWAP(the_allocator)
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
	#elif defined(_LIBCPP_VERSION) // No type trait support in libc++ to date
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_INITIALIZER_LIST_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal:value)
		#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept
	#else // Assume type traits and initializer support for non-GCC compilers and standard libraries
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_VARIADICS_SUPPORT
		#define PLF_INITIALIZER_LIST_SUPPORT
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal:value)
		#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept
	#endif

	#define PLF_MOVE_SEMANTICS_SUPPORT
#else
	#define PLF_FORCE_INLINE
	#define PLF_NOEXCEPT throw()
	#define PLF_NOEXCEPT_SWAP(the_allocator)
	#define PLF_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
#endif


#include <algorithm> // std::find
#include <cstdio> // log redirection, printf
#include <cstdlib> // abort
#include <functional> // std::greater
#include <vector> // range-insert testing

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#include "plf_colony.h"


namespace
{

    inline void failpass(const char *test_type, bool condition)
    {
        if (!condition)
        {
				printf("%s: Fail\n", test_type);
				getchar();
            abort();
        }
    }

	void title1(const char *)
	{
	}

	void title2(const char *)
	{
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



#ifdef PLF_VARIADICS_SUPPORT
	struct perfect_forwarding_test
	{
		const bool success;

		perfect_forwarding_test(int&& /*perfect1*/, int& perfect2)
			: success(true)
		{
			perfect2 = 1;
		}

		template <typename T, typename U>
		perfect_forwarding_test(T&& /*imperfect1*/, U&& /*imperfect2*/)
			: success(false)
		{}
	};


	struct small_struct
	{
		double *empty_field_1;
		double unused_number;
		unsigned int empty_field2;
		double *empty_field_3;
		int number;
		unsigned int empty_field4;

		small_struct(const int num) PLF_NOEXCEPT: number(num) {};
	};

#endif
}


namespace sg14_test
{

void plf_colony_test()
{
	using namespace std;
	using namespace plf;


	unsigned int looper = 0;

	while (++looper != 25)
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
			
			int total = 0, numtotal = 0;
			
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
			
			
			colony<int *> p_colony2;
			p_colony2 = p_colony;
			colony<int *> p_colony3(p_colony);
			colony<int *> p_colony4(p_colony2, p_colony2.get_allocator());
			
			colony<int *>::iterator it1 = p_colony.begin();
			colony<int *>::const_iterator cit(it1);
			
			failpass("Copy test", p_colony2.size() == 400);
			failpass("Copy construct test", p_colony3.size() == 400);
			failpass("Allocator-extended copy construct test", p_colony4.size() == 400);
		

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
			
			failpass("Reverse iterator advance and distance test", p_colony.distance(p_colony.rbegin(), r_iterator) == 50);

			colony<int *>::reverse_iterator r_iterator2 = p_colony.next(r_iterator, 2);

			failpass("Reverse iterator next and distance test", p_colony.distance(p_colony.rbegin(), r_iterator2) == 52);

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

				p_colony.insert(&ten);

				failpass("Insert to post-moved-colony test", p_colony.size() == 1);

				colony<int *> p_colony5(p_colony2);
				colony<int *> p_colony6(std::move(p_colony5), p_colony2.get_allocator());
				
				failpass("Allocator-extended move construct test", p_colony6.size() == 400);
			#else
				p_colony2 = p_colony;
			#endif

			p_colony3 = p_colony2;
			
			failpass("Copy test 2", p_colony3.size() == 400);
			
			p_colony2.insert(&ten);

			p_colony2.swap(p_colony3);

			failpass("Swap test", p_colony2.size() == p_colony3.size() - 1);

			swap(p_colony2, p_colony3);

			failpass("Swap test 2", p_colony3.size() == p_colony2.size() - 1);

			failpass("max_size() test", p_colony2.max_size() > p_colony2.size());
			
		}

		
		{
			title2("Insert and Erase tests");
			
			colony<int> i_colony;

			for (int temp = 0; temp != 500000; ++temp)
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
			colony<int>::iterator it2 = i_colony.begin();
			i_colony.advance(it2, 250000);
			

			for (; it2 != i_colony.end();)
			{
				it2 = i_colony.erase(it2);
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

			
			for (int temp = 0; temp != 250000; ++temp)
			{
				i_colony.insert(10);
			}
			
			
			int total = 0;
			
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

				unsigned int internal_loop_counter = 0;

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

					++internal_loop_counter;
				}
			}

			failpass("Multiple sequential small insert/erase commands test", count == i_colony.size());
		}


		{
			title2("Range-erase tests");
		
			colony<int> i_colony;
			
			int counter = 0;

			for (; counter != 1000; ++counter)
			{
				i_colony.insert(counter);
			}
			
			
			colony<int>::iterator it1 = i_colony.begin(), it2 = i_colony.begin();
			
			i_colony.advance(it1, 500);
			i_colony.advance(it2, 800);
			
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Simple range-erase test 1", counter == 700 && i_colony.size() == 700);

		
			it1 = it2 = i_colony.begin();
			
			i_colony.advance(it1, 400);
			i_colony.advance(it2, 500); // This should put it2 past the point of previous erasures
			
			i_colony.erase(it1, it2);
			
			counter = 0;

			for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
			{
				++counter;
			}

			failpass("Simple range-erase test 2", counter == 600 && i_colony.size() == 600);

			

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
			
			if (i_colony.size() < 400)
			{
				for (counter = 0; counter != 400; ++counter)
				{
					i_colony.insert(counter);
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

			failpass("Range-erase with colony already third-erased, randomized erasures", counter == 400 && i_colony.size() == 400);



			unsigned int size, range1, range2, internal_loop_counter;

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

					if (i_colony.size() != static_cast<unsigned int>(counter))
					{
						printf("Fuzz-test range-erase randomly Fail: loop counter: %u, internal_loop_counter: %u.\n", loop_counter, internal_loop_counter);
						getchar(); 
						abort(); 
					}
					
					if (i_colony.size() != i_colony.group_size_sum())
					{
						printf("Fuzz-test range-erase randomly Fail - group_size_sum failure: loop counter: %u, internal_loop_counter: %u, size: %u, group_size_sum: %u.\n", loop_counter, internal_loop_counter, static_cast<unsigned int>(i_colony.size()), static_cast<unsigned int>(i_colony.group_size_sum()));
						getchar(); 
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

			failpass("Fuzz-test range-erase randomly until empty", i_colony.size() == 0);



			for (unsigned int loop_counter = 0; loop_counter != 50; ++loop_counter)
			{
				i_colony.clear();
				internal_loop_counter = 0;
				
				i_colony.insert(10000, 10); // fill-insert
				
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

					if (i_colony.size() != i_colony.group_size_sum())
					{
						printf("Fuzz-test range-erase + fill-insert randomly Fails during erase - group_size_sum failure: loop counter: %u, internal_loop_counter: %u, size: %u, group_size_sum: %u.\n", loop_counter, internal_loop_counter, static_cast<unsigned int>(i_colony.size()), static_cast<unsigned int>(i_colony.group_size_sum()));
						getchar(); 
						abort(); 
					}
					
					if (i_colony.size() != static_cast<unsigned int>(counter))
					{
						printf("Fuzz-test range-erase + fill-insert randomly Fails during erase: loop counter: %u, internal_loop_counter: %u.\n", loop_counter, internal_loop_counter);
						getchar(); 
						abort(); 
					}
					
					if (i_colony.size() > 100)
					{ // Test to make sure our stored erased_locations are valid & fill-insert is functioning properly in these scenarios
						const unsigned int extra_size = xor_rand() % 128; 
						i_colony.insert(extra_size, 5);

						if (i_colony.size() != i_colony.group_size_sum())
						{
							printf("Fuzz-test range-erase + fill-insert randomly Fails during insert - group_size_sum failure: loop counter: %u, internal_loop_counter: %u, size: %u, group_size_sum: %u.\n", loop_counter, internal_loop_counter, static_cast<unsigned int>(i_colony.size()), static_cast<unsigned int>(i_colony.group_size_sum()));
							getchar(); 
							abort(); 
						}
						
						if (i_colony.size() != static_cast<unsigned int>(counter) + extra_size)
						{
							printf("Fuzz-test range-erase + fill-insert randomly Fails during fill-insert: loop counter: %u, internal_loop_counter: %u.\n", loop_counter, internal_loop_counter);
							getchar(); 
							abort(); 
						}

						counter = 0;

						for (colony<int>::iterator it = i_colony.begin(); it != i_colony.end(); ++it)
						{
							++counter;
						}

						if (i_colony.size() != static_cast<unsigned int>(counter))
						{
							printf("Fuzz-test range-erase + fill-insert randomly Fails during counter-test fill-insert: loop counter: %u, internal_loop_counter: %u.\n", loop_counter, internal_loop_counter);
							getchar(); 
							abort(); 
						}
					}

					++internal_loop_counter;
				}
			}

			failpass("Fuzz-test range-erase + fill-insert randomly until empty", i_colony.size() == 0);

			i_colony.erase(i_colony.begin(), i_colony.end());

			failpass("Range-erase when colony is empty test (crash test)", i_colony.size() == 0);

			i_colony.insert(10, 1);

			i_colony.erase(i_colony.begin(), i_colony.begin());

			failpass("Range-erase when range is empty test (crash test)", i_colony.size() == 10);
		}


		{
			title2("Sort tests");

			colony<int> i_colony;
			
			i_colony.reserve(50000);

			for (unsigned int temp = 0; temp != 50000; ++temp)
			{
				i_colony.insert(xor_rand() & 65535);
			}
			
			i_colony.sort();
			
			bool sorted = true;
			int previous = 0;
			
			for (colony<int>::iterator current = i_colony.begin(); current != i_colony.end(); ++current)
			{
				if (previous > *current)
				{
					sorted = false;
					break;
				}
				
				previous = *current;
			}

			failpass("Less-than sort test", sorted);

			i_colony.sort(std::greater<int>());

			previous = 65536;
			
			for (colony<int>::iterator current = i_colony.begin(); current != i_colony.end(); ++current)
			{
				if (previous < *current)
				{
					sorted = false;
					break;
				}

				previous = *current;
			}
			
			failpass("Greater-than sort test", sorted);
		}			



		{
			title2("Different insertion-style tests");

			#ifdef PLF_INITIALIZER_LIST_SUPPORT
				colony<int> i_colony = {1, 2, 3};

				failpass("Initializer-list constructor test", i_colony.size() == 3);
			#else
				colony<int> i_colony(3, 1);
			#endif
			
			colony<int> i_colony2(i_colony.begin(), i_colony.end());
			
			failpass("Range constructor test", i_colony2.size() == 3);
			
			colony<int> i_colony3(5000, 2, 100, 1000);
			
			failpass("Fill construction test", i_colony3.size() == 5000);
			
			i_colony2.insert(500000, 5);
			
			failpass("Fill insertion test", i_colony2.size() == 500003);
			
			std::vector<int> some_ints(500, 2);
			
			i_colony2.insert(some_ints.begin(), some_ints.end());
			
			failpass("Range insertion test", i_colony2.size() == 500503);
			
			i_colony3.clear();
			i_colony2.clear();
			i_colony2.reserve(50000);
			i_colony2.insert(60000, 1);
			
			int total = 0;
			
			for (colony<int>::iterator it = i_colony2.begin(); it != i_colony2.end(); ++it)
			{
				total += *it;
			}
			
			failpass("Reserve + fill insert test", i_colony2.size() == 60000 && total == 60000);


			i_colony2.clear();
			i_colony2.reserve(5000);
			i_colony2.insert(60, 1);
			
			total = 0;
			
			for (colony<int>::iterator it = i_colony2.begin(); it != i_colony2.end(); ++it)
			{
				total += *it;
			}
			
			failpass("Reserve + fill insert test 2", i_colony2.size() == 60 && total == 60);

			i_colony2.insert(6000, 1);
			
			total = 0;
			
			for (colony<int>::iterator it = i_colony2.begin(); it != i_colony2.end(); ++it)
			{
				total += *it;
			}
			
			failpass("Reserve + fill + fill test", i_colony2.size() == 6060 && total == 6060);
			
			i_colony2.reserve(18000);
			i_colony2.insert(6000, 1);
			
			total = 0;
			
			for (colony<int>::iterator it = i_colony2.begin(); it != i_colony2.end(); ++it)
			{
				total += *it;
			}
			
			failpass("Reserve + fill + fill + reserve + fill test", i_colony2.size() == 12060 && total == 12060);

			
		}


		#ifdef PLF_VARIADICS_SUPPORT
		{
			title2("Perfect Forwarding tests");

			colony<perfect_forwarding_test> pf_colony;

			int lvalue = 0;
			int &lvalueref = lvalue;

			pf_colony.emplace(7, lvalueref);

			failpass("Perfect forwarding test", (*pf_colony.begin()).success);
			failpass("Perfect forwarding test 2", lvalueref == 1);
		}
		{
			title2("Basic emplace test");

			colony<small_struct> ss_colony;
			int total1 = 0, total2 = 0;

			for (int counter = 0; counter != 100; ++counter)
			{
				ss_colony.emplace(counter);
				total1 += counter;
			}
			
			for (colony<small_struct>::iterator it = ss_colony.begin(); it != ss_colony.end(); ++it)
			{
				total2 += it->number;
			}

			failpass("Basic emplace test", total1 == total2);
			failpass("Basic emplace test 2", ss_colony.size() == 100);
		}
		#endif


		{
			title2("Misc function tests");
			
			colony<int> colony1;
			colony1.change_group_sizes(50, 100);
			
			colony1.insert(27);
			
			failpass("Change_group_sizes min-size test", colony1.capacity() == 50);
			
			for (int counter = 0; counter != 100; ++counter)
			{
				colony1.insert(counter);
			}
			
			failpass("Change_group_sizes max-size test", colony1.capacity() == 200);
			
			colony1.reinitialize(200, 2000);
			
			colony1.insert(27);
			
			failpass("Reinitialize min-size test", colony1.capacity() == 200);
			
			for (int counter = 0; counter != 3300; ++counter)
			{
				colony1.insert(counter);
			}

			failpass("Reinitialize max-size test", colony1.capacity() == 5200);			

			colony1.change_group_sizes(500, 500);
			
			failpass("Change_group_sizes resize test", colony1.capacity() == 3500);
			
			colony1.change_minimum_group_size(200);
			colony1.change_maximum_group_size(200);
			
			failpass("Change_maximum_group_size resize test", colony1.capacity() == 3400);
			
		}
		
		{
			title2("Splice tests");
			
			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 20; ++number)
				{
					colony1.insert(number);
					colony2.insert(number + 20);
				}
				
				colony1.splice(colony2);
				
				int check_number = 0;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number++ != *current)
					{
						fail = true;
					}
				}
				
				failpass("Small splice test 1", fail == false);
			}
			

			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 100; ++number)
				{
					colony1.insert(number);
					colony2.insert(number + 100);
				}
				
				colony1.splice(colony2);
				
				int check_number = 0;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number++ != *current)
					{
						fail = true;
					}
				}
				
				failpass("Small splice test 2", fail == false);
			}
			
			
			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 100000; ++number)
				{
					colony1.insert(number);
					colony2.insert(number + 100000);
				}
				
				colony1.splice(colony2);
				
				int check_number = 0;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number++ != *current)
					{
						fail = true;
					}
				}
				
				failpass("Large splice test 1", fail == false);
			}
			

			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 100; ++number)
				{
					colony1.insert(number);
					colony2.insert(number + 100);
				}
				
				
				for (colony<int>::iterator current = colony2.begin(); current != colony2.end();)
				{
					if ((xor_rand() & 7) == 0)
					{
						current = colony2.erase(current);
					}
					else
					{
						++current;
					}
				}

				
				colony1.splice(colony2);
				
				int check_number = -1;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number >= *current)
					{
						fail = true;
					}
					
					check_number = *current;
				}
				
				failpass("Erase + splice test 1", fail == false);
			}
			
			
			
			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 100; ++number)
				{
					colony1.insert(number);
					colony2.insert(number + 100);
				}
				

				
				for (colony<int>::iterator current = colony2.begin(); current != colony2.end();)
				{
					if ((xor_rand() & 3) == 0)
					{
						current = colony2.erase(current);
					}
					else
					{
						++current;
					}
				}
					
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end();)
				{
					if ((xor_rand() & 1) == 0)
					{
						current = colony1.erase(current);
					}
					else
					{
						++current;
					}
				}
					
				
				colony1.splice(colony2);
				
				int check_number = -1;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number >= *current)
					{
						fail = true;
					}
					
					check_number = *current;
				}
				
				failpass("Erase + splice test 2", fail == false);
			}



			{
				colony<int> colony1, colony2;
				
				colony1.change_group_sizes(200, 200);
				colony2.change_group_sizes(200, 200);
				
				for(int number = 0; number != 100; ++number)
				{
					colony1.insert(number + 150);
				}
				
				
				for(int number = 0; number != 150; ++number)
				{
					colony2.insert(number);
				}
				
				
				colony1.splice(colony2);
				
				int check_number = -1;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number >= *current)
					{
						fail = true;
					}
					
					check_number = *current;
				}
				
				failpass("Unequal size splice test 1", fail == false);
			}
			

			
			{
				colony<int> colony1, colony2;
				
				colony1.reinitialize(200, 200);
				colony2.reinitialize(200, 200);

				for(int number = 0; number != 100; ++number)
				{
					colony1.insert(100 - number);
				}
				
				
				for(int number = 0; number != 150; ++number)
				{
					colony2.insert(250 - number);
				}
				
				
				colony1.splice(colony2);
				
				int check_number = 255;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number < *current)
					{
						fail = true;
					}
					
					check_number = *current;
				}
				
				failpass("Unequal size splice test 2", fail == false);
			}
			
			
			
			{
				colony<int> colony1, colony2;
				
				for(int number = 0; number != 100000; ++number)
				{
					colony1.insert(number + 200000);
				}
				
				
				for(int number = 0; number != 200000; ++number)
				{
					colony2.insert(number);
				}
				
				for (colony<int>::iterator current = colony2.begin(); current != colony2.end();)
				{
					if ((xor_rand() & 1) == 0)
					{
						current = colony2.erase(current);
					}
					else
					{
						++current;
					}
				}
					
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end();)
				{
					if ((xor_rand() & 1) == 0)
					{
						current = colony1.erase(current);
					}
					else
					{
						++current;
					}
				}
				

				colony1.erase(--(colony1.end()));
				colony2.erase(--(colony2.end()));

				colony1.splice(colony2); // splice should swap the order at this point due to differences in numbers of unused elements at end of final group in each colony
				
				int check_number = -1;
				bool fail = false;
				
				for (colony<int>::iterator current = colony1.begin(); current != colony1.end(); ++current)
				{
					if (check_number >= *current)
					{
						fail = true;
						break;
					}
					
					check_number = *current;
				}
				
				failpass("Large unequal size + erase splice test 1", fail == false);


				do
				{
					for (colony<int>::iterator current = colony1.begin(); current != colony1.end();)
					{
						if ((xor_rand() & 3) == 0)
						{
							current = colony1.erase(current);
						}
						else if ((xor_rand() & 7) == 0)
						{
							colony1.insert(433);
							++current;
						}
						else
						{
							++current;
						}
					}
					
				} while (!colony1.empty());
				
				failpass("Post-splice insert-and-erase randomly till-empty test", colony1.size() == 0);
			}
		}
	}
}
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::plf_colony_test();
}
#endif
