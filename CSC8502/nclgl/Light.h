#pragma once

#include "Vector3.h"

class Light
{
public:
	enum LightType {
		Directional,
		Point,
		Spot
	};
	Light(const LightType type, const Vector3& pos, const float radius, const Vector3& color)
		:m_type{ type }, m_position{ pos }, m_radius{ radius }, m_color{color} {}
	~Light() {}

	LightType GetLightType() { return m_type; }
	Vector3 GetPosition() { 
		return m_position; 
	}

	void SetPosition(const Vector3& pos) {
		m_position = pos;
	}

	float GetRadius() { 
		return m_radius; 
	}
	Vector3 GetColor() { return m_color; }
private:
	LightType m_type;
	Vector3 m_position;
	float m_radius;
	Vector3 m_color;//diffuse, specular, ambient

};

