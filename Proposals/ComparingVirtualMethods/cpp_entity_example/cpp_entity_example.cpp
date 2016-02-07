// cpp_entity_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "entity.h"
#include "lerp.h"
#include "hermite.h"

#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <ctime>
#include <algorithm>
using namespace std;

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
float dummyOut[100];
int dummyOutIndex = 0;
vector<std::chrono::duration<double, std::ratio<1, 1000>>> gSlowSimpleUpdateExampleTimers;
vector<std::chrono::duration<double, std::ratio<1, 1000>>> gSlowComplicatedUpdateExampleTimers;
vector<std::chrono::duration<double, std::ratio<1, 1000>>> gFastUpdateExampleTimers;


class mytimer
{
public:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
	mytimer()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}
	std::chrono::duration<double> stop()
	{
		m_end = std::chrono::high_resolution_clock::now();
		return m_end - m_start;
	}
};


void SlowUpdateExample()
{
#ifdef PRINT
	cout << "SlowUpdateExample" << endl;
#endif
	default_random_engine generator;
	uniform_real_distribution<float> distribution(0, 1);

	int number_of_lerp = 400;
	int number_of_hermite = 1000;
	vector<long long> create_types;
	for (int i = 0; i < number_of_lerp; i++)
	{
		create_types.emplace_back(entity_lerp_slow::type);
	}
	for (int i = 0; i < number_of_hermite; i++)
	{
		create_types.emplace_back(entity_hermite::type);
	}

	shuffle(create_types.begin(), create_types.end(), generator);

	vector<unique_ptr<entity>> entity_vec;
	for (auto &create_type : create_types)
	{
		if (create_type == entity_hermite::type)
		{
			unique_ptr<entity> a(create_entity_hermite(distribution(generator), distribution(generator), distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
		else
		{
			unique_ptr<entity> a(create_entity_lerp_slow(distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
	}

	{
		// change styles for document 
		mytimer timer;
		for (float t = 0.0f; t < 1.0; t += 0.05f) {
			for (auto &a : entity_vec) {
				// slow version each object has a virtual update.
				a->Update(t);
			}
		}
		gSlowSimpleUpdateExampleTimers.emplace_back(timer.stop());
	}
}

void SlowComplicatedUpdateExample()
{
#ifdef PRINT
	cout << "SlowComplicatedUpdateExample" << endl;
#endif
	default_random_engine generator;
	uniform_real_distribution<float> distribution(0, 1);

	int number_of_lerp = 400;
	int number_of_hermite = 1000;
	vector<long long> create_types;
	for (int i = 0; i < number_of_lerp; i++)
	{
		create_types.emplace_back(entity_lerp_fast::type);
	}
	for (int i = 0; i < number_of_hermite; i++)
	{
		create_types.emplace_back(entity_hermite::type);
	}

	shuffle(create_types.begin(), create_types.end(), generator);

	vector<unique_ptr<entity>> entity_vec;
	for (auto &create_type : create_types)
	{
		if (create_type == entity_hermite::type)
		{
			unique_ptr<entity> a(create_entity_hermite(distribution(generator), distribution(generator), distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
		else
		{
			unique_ptr<entity> a(create_entity_lerp_fast(distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
	}


	{
		// change styles for document 
		mytimer timer;
		for (float t = 0.0f; t < 1.0; t += 0.05f) {
			for (auto &a : entity_vec) {
				// if we have a fast loop for this object don't update
				// so in this case we on the hermite entity but not the lerp ones. 
				if (a->GetType() != entity_lerp_fast::type)	{
					a->Update(t);
				}
			}
			// fast loop with no virtual functions and maybe a different data format. 
			entity_lerp_fast::UpdateAll(t);
		}
		gSlowComplicatedUpdateExampleTimers.emplace_back(timer.stop());
	}
}


void FastUpdateExampleTimers()
{
#ifdef PRINT
	cout << "FastUpdateExampleTimers" << endl;
#endif
	default_random_engine generator;
	uniform_real_distribution<float> distribution(0, 1);

	int number_of_lerp = 400;
	int number_of_hermite = 1000;
	vector<long long> create_types;
	for (int i = 0; i < number_of_lerp; i++)
	{
		create_types.emplace_back(entity_lerp_fast::type);
	}
	for (int i = 0; i < number_of_hermite; i++)
	{
		create_types.emplace_back(entity_hermite::type);
	}

	shuffle(create_types.begin(), create_types.end(), generator);

	vector<unique_ptr<entity>> entity_vec;
	for (auto &create_type : create_types)
	{
		if (create_type == entity_hermite::type)
		{
			unique_ptr<entity> a(create_entity_hermite(distribution(generator), distribution(generator), distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
		else
		{
			unique_ptr<entity> a(create_entity_lerp_fast(distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
	}


	{
		// change styles for document 
		mytimer timer;
		for (float t = 0.0f; t < 1.0; t += 0.05f) {
			for (auto &a : entity_vec) {
				// if we have a fast loop for this object don't update
				// so in this case we on the hermite entity but not the lerp ones. 
				if (*a->m_typedata != entity_lerp_fast::type) {
					a->Update(t);
				}
			}
			// fast loop with no virtual functions and maybe a different data format. 
			entity_lerp_fast::UpdateAll(t);
		}
		gFastUpdateExampleTimers.emplace_back(timer.stop());
	}
}


void MethodPointerUpdateExampleTimers()
{
#ifdef PRINT
	cout << "MethodPointerUpdateExampleTimers" << endl;
#endif
	default_random_engine generator;
	uniform_real_distribution<float> distribution(0, 1);

	int number_of_lerp = 400;
	int number_of_hermite = 1000;
	vector<long long> create_types;
	for (int i = 0; i < number_of_lerp; i++)
	{
		create_types.emplace_back(entity_lerp_fast::type);
	}
	for (int i = 0; i < number_of_hermite; i++)
	{
		create_types.emplace_back(entity_hermite::type);
	}

	shuffle(create_types.begin(), create_types.end(), generator);

	vector<unique_ptr<entity>> entity_vec;
	for (auto &create_type : create_types)
	{
		if (create_type == entity_hermite::type)
		{
			unique_ptr<entity> a(create_entity_hermite(distribution(generator), distribution(generator), distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
		else
		{
			unique_ptr<entity> a(create_entity_lerp_fast(distribution(generator), distribution(generator)));
			entity_vec.emplace_back(std::move(a));
		}
	}


	{
		// change styles for document 
		mytimer timer;
		// make a typedef to avoid mistakes
		typedef  void (entity_lerp_fast::*memfun)(float y) const;
		// create a member function that points to update function that I have a fast loop for 
		memfun mf = &entity_lerp_fast::Update;
		// The GCC extention looks like you can do this. 
		// typedef  void(*as_normfun)(entity *_this, float y);
		// as_normfun nf = (as_normfun)&entity::Update;
		for (float t = 0.0f; t < 1.0; t += 0.05f) {
			for (auto &a : entity_vec) {
				
				// if we have a fast loop for this object don't update
				// so in this case we on the hermite entity but not the lerp ones. 
				//if (mf != &((*a).*mf)) {
					a->Update(t);
				//}
			}
			// fast loop with no virtual functions and maybe a different data format. 
			entity_lerp_fast::UpdateAll(t);
		}
		gFastUpdateExampleTimers.emplace_back(timer.stop());
	}
}


int main()
{
	SlowUpdateExample();
	SlowComplicatedUpdateExample();
	FastUpdateExampleTimers();
	MethodPointerUpdateExampleTimers();

	for (auto a : dummyOut)
	{
		cout << "dummyOut " << a << endl;
	}

	for (auto & t : gSlowSimpleUpdateExampleTimers)
	{
		cout << "gSlowUpdateExample ms " << t.count() << endl;
	}
	for (auto & t : gSlowComplicatedUpdateExampleTimers)
	{
		cout << "gSlowComplicatedUpdateExampleTimers ms " << t.count() << endl;
	}
	for (auto & t : gFastUpdateExampleTimers)
	{
		cout << "gFastUpdateExampleTimers ms " << t.count() << endl;
	}
	return 0;
}

