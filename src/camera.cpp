#include "Camera.h"

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
	m_position += direction;
}

void Camera::MoveInLocal(const vec3f& direction) noexcept
{
	auto localToWorld = mat4f::rotation(m_rotation.x, m_rotation.y, m_rotation.z);

	m_position += (localToWorld * direction.xyz1()).xyz();
}


void Camera::RotateTo(const linalg::vec3f& rotation) noexcept {
	m_rotation = rotation;
}


void Camera::Rotate(const linalg::vec3f& rotationDirections) noexcept {
	m_rotation += rotationDirections;
	
}


void Camera::ClampCameraPitch(const float min, const float max) noexcept {
	m_rotation.z = MATH_H::clamp<float>(m_rotation.z,min, max);
}



mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	auto t = mat4f::translation(-m_position);
	auto r = transpose(mat4f::rotation(m_rotation.x, m_rotation.y, m_rotation.z));

	return r*t;
}

mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}