#include "SG14_test.h"

#include "ring.h"

#include <array>
#include <mutex>
#include <future>
#include <iostream>

void sg14_test::ring_test()
{
	std::array<int, 5> A;
	std::array<int, 5> B;

	sg14::ring_span<int> Q(std::begin(A), std::end(A));

	Q.push_back(7);
	Q.push_back(3);
	assert(Q.size() == 2);
	assert(Q.front() == 7);

	Q.pop_front();
	assert(Q.size() == 1);

	Q.push_back(18);
	auto Q3 = std::move(Q);
	assert(Q3.front() == 3);
	assert(Q3.back() == 18);

	sg14::ring_span<int> Q5(std::move(Q3));
	assert(Q5.front() == 3);
	assert(Q5.back() == 18);
	assert(Q5.size() == 2);

	Q5.pop_front();
	Q5.pop_front();
	assert(Q5.empty());

	sg14::ring_span<int> Q6(std::begin(B), std::end(B));
	Q6.push_back(6);
	Q6.push_back(7);
	Q6.push_back(8);
	Q6.push_back(9);
	Q6.emplace_back(10);
	Q6.swap(Q5);
	assert(Q6.empty());
	assert(Q5.size() == 5);
	assert(Q5.front() == 6);
	assert(Q5.back() == 10);

	puts("Ring test completed.\n");
}

void sg14_test::thread_communication_test()
{
	std::array<int, 10> A;
	sg14::ring_span<int> buffer(std::begin(A), std::end(A));
	std::mutex m;
	std::condition_variable cv;

	puts("Ring example: thread communication.\nEnter some numbers, enter -1 to quit.\n");

	auto ci = std::async(std::launch::async, [&]()
	{
		int val = 0;
		do
		{
			std::cin >> val;
			{
				std::lock_guard<std::mutex> lg(m);
				buffer.push_back(val);
				cv.notify_one();
			}
		} while (val != -1);
	});

	auto po = std::async(std::launch::async, [&]()
	{
		int val = 0;
		do
		{
			std::unique_lock<std::mutex> lk(m);
			cv.wait(lk);
			val = buffer.front();
			std::cout << val << std::endl;
			buffer.pop_front();
			lk.unlock();
		} while (val != -1);
		puts("Ring example completed.\n");
	});
}