#pragma once

#include <cmath>
#include "Matrix4.h"
#include "Vector3.h"


class Camera {
public:
	enum CameraControlMode {
		AUTO,
		HAND
	};

	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
		position = Vector3{ 0.0f,0.0f,0.0f };
		controlMode = HAND;
		target = position;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		roll = 0.0f;
		this->position = position;
		controlMode = HAND;
		target = position;
	}
	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	Matrix4 BuildViewMatrix();
	Matrix4 BuildViewMatrixWithoutTranlation();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRoll() { return roll; }
	void SetRoll(const float r) { roll = r; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }
	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }
	CameraControlMode GetMode() { return controlMode; }
	void SetMode(const CameraControlMode mode) { target = position; lerpV = 0.0f; controlMode = mode; }
	void SetTarget(const Vector3& tar) { lerpV = 0.0f; target = tar; }
protected:
	CameraControlMode controlMode;
	float yaw;
	float pitch;
	float roll;
	Vector3 position; // Set to 0,0,0 by Vector3 constructor ;)
	Vector3 target;
	float lerpV;


	void AutoUpdate(float dt);
	void HandUpdate(float dt);

	void SmoothMove(const Vector3& target);
	float Lerp(const float a, const float b, const float t);
};

