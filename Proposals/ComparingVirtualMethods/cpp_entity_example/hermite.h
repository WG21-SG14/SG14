#pragma once
#include "entity.h"

class entity_hermite : public entity
{
public:
	const static long long type = 3LL;
	entity_hermite() :entity(&type) {}

	virtual int GetType() const override = 0;
	virtual void Update(float t) const override = 0;
};

entity_hermite* create_entity_hermite(float p1, float p2, float n1, float n2);