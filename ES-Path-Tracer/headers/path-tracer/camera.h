#ifndef __GUARD_CAMERA_H__
#define __GUARD_CAMERA_H__

#include <stdexcept>

#include "../geometry/point3.h"
#include "../geometry/vector3.h"

class Camera {
public:
	Camera(
		const Point3& position,
		const Vector3& look_at,
		const Vector3& up,
		double near)
		: m_position(position),
		m_look_at(look_at.normalize()),
		m_up(0),
		m_left(0),
		m_near(near)
	{
		if (near <= 0)
			throw std::invalid_argument("near must be positive");
		
		m_left = cross_prod(up, look_at).normalize();
		m_up = cross_prod(look_at, m_left).normalize();
	}

	// Acessor functions
	Point3& position() { return m_position; }
	Vector3& look_at() { return m_look_at; }
	Vector3& up() { return m_up; }
	Vector3& left() { return m_left; }
	double& near() { return m_near; }
	// Constant acessor functions
	const Point3& position() const { return m_position; }
	const Vector3& look_at() const { return m_look_at; }
	const Vector3& up() const { return m_up; }
	const Vector3& left() const { return m_left; }
	const double& near() const { return m_near; }

private:
	Point3 m_position;
	Vector3 m_look_at;
	Vector3 m_left;
	Vector3 m_up;
	double m_near;
};

#endif
