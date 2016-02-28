#include "SG14_test.h"

#include "ring.h"

#include <cassert>
#include <future>
#include <mutex>
#include <condition_variable>
#include <iostream>

using sg14::static_ring;
using sg14::dynamic_ring;

void sg14_test::static_ring_test()
{
	static_ring<int, 5> Q;

	Q.push(7);
	Q.push(3);
	assert(Q.size() == 2);
	assert(Q.front() == 7);

	Q.pop();
	assert(Q.size() == 1);

	Q.push(18);
	auto Q2 = Q;
	assert(Q2.front() == 3);
	assert(Q2.back() == 18);

	auto Q3 = std::move(Q2);
	assert(Q3.front() == 3);
	assert(Q3.back() == 18);

	static_ring<int, 5> Q4(Q3);
	assert(Q4.front() == 3);
	assert(Q4.back() == 18);

	static_ring<int, 5> Q5(std::move(Q3));
	assert(Q5.front() == 3);
	assert(Q5.back() == 18);
	assert(Q5.size() == 2);

	Q5.pop();
	Q5.pop();
	assert(Q5.empty());

	static_ring<int, 5> Q6;
	Q6.push(6);
	Q6.push(7);
	Q6.push(8);
	Q6.push(9);
	Q6.emplace(10);
	Q6.swap(Q5);
	assert(Q6.empty());
	assert(Q5.size() == 5);
	assert(Q5.front() == 6);
	assert(Q5.back() == 10);
}


void sg14_test::dynamic_ring_test()
{
	dynamic_ring<int> Q(8);

	assert(Q.push(7));
	assert(Q.push(3));
	assert(Q.size() == 2);
	assert(Q.front() == 7);

	Q.pop();
	assert(Q.size() == 1);

	assert(Q.push(18));
	auto Q2 = Q;
	assert(Q2.front() == 3);
	assert(Q2.back() == 18);

	auto Q3 = std::move(Q2);
	assert(Q3.front() == 3);
	assert(Q3.back() == 18);

	dynamic_ring<int> Q4(Q3);
	assert(Q4.front() == 3);
	assert(Q4.back() == 18);

	dynamic_ring<int> Q5(std::move(Q3));
	assert(Q5.front() == 3);
	assert(Q5.back() == 18);
	assert(Q5.size() == 2);

	Q5.pop();
	Q5.pop();
	assert(Q5.empty());

	dynamic_ring<int> Q6(8);
	assert(Q6.push(6));
	assert(Q6.push(7));
	assert(Q6.push(8));
	assert(Q6.push(9));
	Q6.emplace(10);
	Q6.swap(Q5);
	assert(Q6.empty());
	assert(Q5.size() == 5);
	assert(Q5.front() == 6);
	assert(Q5.back() == 10);
}


void sg14_test::thread_communication_test()
{
	static_ring<int, 10> buffer;
	std::mutex m;
	std::condition_variable cv;

	auto ci = std::async(std::launch::async, [&]()
	{
		int val = 0;
		do
		{
			std::cin >> val;
			{
				std::lock_guard<std::mutex> lg(m);
				buffer.push(val);
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
			buffer.pop();
			lk.unlock();
		} while (val != -1);
	});
}