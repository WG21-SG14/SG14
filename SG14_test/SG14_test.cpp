// SG14_test.cpp : Defines the entry point for the console application.
//

#include "SG14_test.h"

#include "rolling_queue.h"

#include <cassert>


void sg14_test::rolling_queue_test()
{
	sg14::rolling_queue<int, 5> Q;

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

	sg14::rolling_queue<int, 5> Q4(Q3);
	assert(Q4.front() == 3);
	assert(Q4.back() == 18);

	sg14::rolling_queue<int, 5> Q5(std::move(Q3));
	assert(Q5.front() == 3);
	assert(Q5.back() == 18);
	assert(Q5.size() == 2);

	Q5.pop();
	Q5.pop();
	assert(Q5.empty());

	sg14::rolling_queue<int, 5> Q6;
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

