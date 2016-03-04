// cpp_entity_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "entity.h"
#include "hermite.h"

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

float hermite(float t, float p1, float p2, float n1, float n2)
{
	float t2 = t*t*t;
	float t3 = t2*t;
	float h1 = 2*t3 - 3*t2 + 1;
	float h2 = -2*t3 + 3*t2;             
	float h3 = t3 - 2 * t2 + t;        
	float h4 = t3 - t2;              
	return h1*p1 + h2*p2 + h3*n1 + h4*n2;
}

const long long entity_hermite::type = 3LL;


class entity_hermite_impl : public entity_hermite
{
	float m_p1;
	float m_p2;
	float m_n1;
	float m_n2;
public:
	const static int type = 3;
	entity_hermite_impl(float p1, float p2, float n1, float n2)
		: m_p1(p1)
		, m_p2(p2)
		, m_n1(n1)
		, m_n2(n2)		
	{

	}
	virtual ~entity_hermite_impl() {}

	int GetType() const override
	{
		return type;
	}

	virtual void Update(float t) const override
	{
#ifdef PRINT
		cout << "hermite ";
		cout << hermite(t, m_p1, m_p2, m_n1, m_n2);
#endif
		dummyOut[dummyOutIndex % ARRAY_SIZE(dummyOut)] = hermite(t, m_p1, m_p2, m_n1, m_n2);
		dummyOutIndex++;
	}
};

entity_hermite* create_entity_hermite(float p1, float p2, float n1, float n2)
{
	return new entity_hermite_impl(p1, p2, n1, n2);
}