#include <iostream>

#include "plf_stack.h"


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



#ifdef PLF_VARIADICS_SUPPORT
	struct perfect_forwarding_test
	{
		const bool success;

		perfect_forwarding_test(int&& perfect1, int& perfect2)
			: success(true)
		{
			perfect2 = 1;
		}

		template <typename T, typename U>
		perfect_forwarding_test(T&& imperfect1, U&& imperfect2)
			: success(false)
		{}
	};
#endif
}


namespace sg14_test
{


void plf_stack_test_suite()
{
	using namespace std;
	using namespace plf;


	unsigned int looper = 0;


	while (++looper != 50)
	{
		{
			title1("Stack");
			title1("Test basics");

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

			stack<unsigned int> i_stack6(i_stack, i_stack3.get_allocator());

			failpass("Allocator-extended copy constructor test", i_stack6.size() == 250000);

			i_stack3.reserve(400000);

			failpass("Reserve test", i_stack3.size() == 250000);


			#ifdef PLF_MOVE_SEMANTICS_SUPPORT
				stack<unsigned int> i_stack4;
				i_stack4 = std::move(i_stack3);
				failpass("Move equality operator test", i_stack2 == i_stack4);
				stack<unsigned int> i_stack5(std::move(i_stack4), i_stack3.get_allocator());

				failpass("Allocator-extended move-construct test", i_stack5.size() == 250000);

				i_stack3 = std::move(i_stack5);
				
			#else
				failpass("Equality operator test", i_stack2 == i_stack3);
			#endif

			failpass("Copy test", i_stack2.size() == 250000);
			failpass("Equality operator test 2", i_stack == i_stack2);
			
			i_stack2.push(5);
			i_stack2.swap(i_stack3);
			
			failpass("Swap test", i_stack2.size() == i_stack3.size() - 1);

			swap(i_stack2, i_stack3);
			
			failpass("Swap test 2", i_stack3.size() == i_stack2.size() - 1);

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
				if ((xor_rand() & 3) == 0)
				{
					i_stack.push(10);
				}
				else
				{
					i_stack.pop();
				}
			} while (!i_stack.empty());;

			failpass("Randomly pop/push till empty test", i_stack.size() == 0);

			#ifdef PLF_VARIADICS_SUPPORT
				i_stack.emplace(20);
				failpass("Emplace test", i_stack.size() == 1);
			#endif
		}

		{
			title2("Stack Special Case Tests");

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


		#ifdef PLF_VARIADICS_SUPPORT
		{
			title2("Perfect Forwarding tests");

			stack<perfect_forwarding_test> pf_stack;

			int lvalue = 0;
			int &lvalueref = lvalue;

			pf_stack.emplace(7, lvalueref);

			failpass("Perfect forwarding test", pf_stack.top().success);
			failpass("Perfect forwarding test 2", lvalueref == 1);
		}
		#endif
	}

	title1("Test Suite PASS - Press ENTER to Exit");
	cin.get();
}

}
