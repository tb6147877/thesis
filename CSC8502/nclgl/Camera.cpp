#include "Camera.h"
#include "Window.h"

void Camera::UpdateCamera(float dt) {

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)
		|| Window::GetKeyboard()->KeyDown(KEYBOARD_A)
		|| Window::GetKeyboard()->KeyDown(KEYBOARD_S)
		|| Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{
		SetMode(HAND);
	}

	switch (controlMode)
	{
	case AUTO:
		AutoUpdate(dt);
		break;

	case HAND:
		HandUpdate(dt);
		break;
	}


}

void Camera::AutoUpdate(float dt) {
	Vector3 min{ target - position };
	lerpV += dt * 0.0001f;
	//std::cout << "dt:" << dt << "\n";
	if (dt >= 0.5f)
	{
		return;
	}
	float velocity = 50.0f * dt;
	if ((abs(min.x) <= 0.01f && abs(min.y) <= 0.01f && abs(min.z) <= 0.01f) || min.Length() <= velocity)
	{
		SetMode(HAND);
		target = position;
		return;
	}
	else {
		float x = Lerp(position.x, target.x, lerpV);
		float y = Lerp(position.y, target.y, lerpV);
		float z = Lerp(position.z, target.z, lerpV);
		position = Vector3{ x,y,z };
	}

	//rotate
	//Y-axis
	Vector3 xAxis{ 1.0f,0.0f,0.0f };
	Vector3 norMin{ min.Normalised() };
	float cos_tar_yaw = Vector3::Dot(xAxis, norMin);
	float rad = acos(cos_tar_yaw);
	float tar_yaw = RadToDeg(rad);
	tar_yaw -= 90.0f;//target yaw need minus 90 every time, because it's a rule of opengl, camera default face negetive z-axis.
	//std::cout << tar_yaw << "\n";
	if (abs(yaw - tar_yaw) < 1.0f)
	{
		yaw = tar_yaw;
	}
	else {
		float temp = Lerp(yaw, tar_yaw, lerpV);
		yaw = temp;
	}


	//X-axis
	Vector3 yAxis{ 0.0f,1.0f,0.0f };
	float sin_tar_pitch = norMin.y / norMin.Length();
	float rad_pitch = asin(sin_tar_pitch);
	float tar_pitch = RadToDeg(rad_pitch);
	//std::cout << tar_pitch << "\n";
	if (abs(pitch - tar_pitch) < 1.0f)
	{
		pitch = tar_pitch;
	}
	else {
		float temp = Lerp(pitch, tar_pitch, lerpV);
		pitch = temp;
	}

}


void Camera::HandUpdate(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);
	//roll -= (Window::GetMouse()->GetRelativePosition().x);//增加一个围绕z轴的旋转

	if (pitch > 90.0f)
	{
		pitch = 90.0f;
	}
	if (pitch < -90.0f)
	{
		pitch < -90.0f;
	}

	if (yaw < 0) {
		yaw += 360.0f;

	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	float velocity = 50.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * velocity;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * velocity;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * velocity;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * velocity;

	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += velocity;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= velocity;
	}
	/*std::cout << "yaw" << yaw << "\n";
	std::cout << "position" << position << "\n";
	std::cout << "pitch" << pitch << "\n";
	std::cout << "==================\n";*/
}

Matrix4 Camera::BuildViewMatrix() {
	//这里交换两个旋转矩阵的乘法位置不会造成结果的改变
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);

	//return Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *Matrix4::Translation(-position);
};

Matrix4 Camera::BuildViewMatrixWithoutTranlation() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0));
}

void Camera::SmoothMove(const Vector3& target) {

}

float Camera::Lerp(const float a, const float b, const float t) {
	if (t > 1 || t < 0)
	{
		std::cout << "Para Error! Cannot lerp!\n";
		return 0.0f;
	}
	return a + t * (b - a);
}


