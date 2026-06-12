#pragma once
#include "Matrix.h"
#include "Vector.h"

struct Sphere {
	Vector3 center;
	float radius;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Plane {
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertices[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};


//物理
struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

struct ConicalPendulum {
	Vector3 anchor;
	float length;
	float halfApexAngle;
	float angle;
	float angularVelocity;
};

struct Capsule {
	Segment segment;
	float radius;
};
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Matrix4x4 MakeTranslationMatrix(const Vector3& translation);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

//3次元アファン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                uint32_t color);

Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

bool IsCollision(const Sphere& s1, const Sphere& s2);
bool PlaneIsCollision(const Sphere& sphere, const Plane& plane);

Vector3 Perpendicular(const Vector3& v);


void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
               uint32_t color);

void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                 uint32_t color);

bool SegmentIsCollsion(const Segment& segment, const Plane& plane);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                  uint32_t color);

bool TriangleToSegmentIsCollision(const Triangle& triangle, const Segment& segment);

bool AABBToAABBIsCollision(const AABB& aabb1, const AABB& aabb2);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

bool AABBToSphereIsCollision(const AABB& aabb, const Sphere& sphere);

bool AABBIntersectsSegment(const AABB& aabb, const Segment& segment);

bool OBBToSphereIsCollision(const OBB& obb, const Sphere& sphere);

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
Ball Springsmire(const Ball& ball, const Spring& spring, float deltaTime);
