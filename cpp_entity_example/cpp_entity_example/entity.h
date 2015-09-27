#pragma once

class entity
{
public:
	const long long* m_typedata;
	entity(const long long* typedata) :m_typedata(typedata) {}

	virtual void Update(float t) const = 0;
	virtual int GetType() const = 0;
	virtual ~entity() {}
};
