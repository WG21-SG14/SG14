// cpp_entity_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "entity.h"
#include "lerp.h"


#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <ctime>
using namespace std;

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
extern float dummyOut[100];
extern int dummyOutIndex;


float lerp(float t, float s, float d)
{
	return t * (s - d) + d;
}

const long long entity_lerp_slow::type = 1LL;


class entity_lerp_slow_impl : public entity_lerp_slow
{
	float m_s;
	float m_d;
public:
	const static int type = 1;

	entity_lerp_slow_impl(float s, float d)
		:m_s(s)
		, m_d(d)
	{}
	virtual ~entity_lerp_slow_impl() {}

	int GetType() const override
	{
		return type;
	}

	virtual void Update(float t) const override
	{
#ifdef PRINT
		cout << "lerp ";
		cout << lerp(t, m_s, m_d);
#endif
		dummyOut[dummyOutIndex % ARRAY_SIZE(dummyOut)] = lerp(t, m_s, m_d);
		dummyOutIndex++;
	}
};

struct Pos
{
	float x;
	float y;
};

const long long entity_lerp_fast::type = 2LL;


static std::vector<Pos> s_positions;

const long long entity_lerp_fast_impl::type = 2;

entity_lerp_fast_impl::entity_lerp_fast_impl(float s, float d)
{
	s_positions.push_back({ s,d });
}

 entity_lerp_fast_impl::~entity_lerp_fast_impl()
{
	s_positions.pop_back();
}

int  entity_lerp_fast_impl::GetType() const 
{
	return type;
}

void  entity_lerp_fast_impl::Update(float t) const 
{
	assert(0); // don't call. 
}

void  entity_lerp_fast_impl::UpdateAll(float t)
{
	for (auto& pos : s_positions)
	{
#ifdef PRINT
		cout << "fast_lerp ";
		cout << lerp(t, pos.x, pos.y);
		cout << endl;
#endif
		dummyOut[dummyOutIndex % ARRAY_SIZE(dummyOut)] = lerp(t, pos.x, pos.y);
		dummyOutIndex++;
	}
}

void entity_lerp_fast::UpdateAll(float t)
{
	entity_lerp_fast_impl::UpdateAll(t);
}

entity_lerp_fast* create_entity_lerp_fast(float p1, float p2)
{
	return new entity_lerp_fast_impl(p1, p2);
}


entity_lerp_slow* create_entity_lerp_slow(float p1, float p2)
{
	return new entity_lerp_slow_impl(p1, p2);
}

