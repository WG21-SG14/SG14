#pragma once
class entity_lerp_slow : public entity
{
public:
	const static long long type = 1LL;
	entity_lerp_slow() :entity(&type) {}

	virtual int GetType() const override = 0;
	virtual void Update(float t) const override = 0;
};

class entity_lerp_fast : public entity
{
public:
	const static long long type = 2LL;
	entity_lerp_fast() :entity(&type) {}

	virtual int GetType() const override = 0;
	virtual void Update(float t) const override = 0;
	static void UpdateAll(float t);
};

entity_lerp_fast* create_entity_lerp_fast(float p1, float p2);
entity_lerp_slow* create_entity_lerp_slow(float p1, float p2);
