#include "Vector.h"


#include <cmath>


Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float Length(const Vector3& v) {
	return std::sqrt(Dot(v, v));
}

Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	if (length == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	return Multiply(1.0f / length, v);
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	};
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	};
}

Vector3 operator*(const Vector3& v, float scalar) {
	return {
		v.x * scalar,
		v.y * scalar,
		v.z * scalar
	};
}

Vector3 operator*(float scalar, const Vector3& v) {
	return v * scalar;
}

Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	return {
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z
	};
}

Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

Vector3 Reflect(const Vector3& v, const Vector3& normal) {
	float dotProduct = Dot(v, normal);
	return v - 2.0f * dotProduct * normal;
}
