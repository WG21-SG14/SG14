#pragma once
class entity_lerp_slow : public entity
{
public:
	const static long long type;
	entity_lerp_slow() :entity(&type) {}

	virtual int GetType() const override = 0;
	virtual void Update(float t) const override = 0;
};

class entity_lerp_fast : public entity
{
public:
	const static long long type;
	entity_lerp_fast() :entity(&type) {}

	virtual int GetType() const override = 0;
	virtual void Update(float t) const override = 0;
	static void UpdateAll(float t);
};

class entity_lerp_fast_impl : public entity_lerp_fast
{
public:
	const static long long type;

	entity_lerp_fast_impl(float s, float d);
	virtual ~entity_lerp_fast_impl();
	int GetType() const override;
	void Update(float t) const override;
	static void UpdateAll(float t);
};


entity_lerp_fast* create_entity_lerp_fast(float p1, float p2);
entity_lerp_slow* create_entity_lerp_slow(float p1, float p2);
