#include "Vector&Matrix.h"

#include <algorithm>

#include <cmath>

namespace {
	Matrix4x4 MakeOBBWorldMatrix(const OBB& obb) {
		Matrix4x4 worldMatrix{};
		worldMatrix.matrix[0][0] = obb.orientations[0].x;
		worldMatrix.matrix[0][1] = obb.orientations[0].y;
		worldMatrix.matrix[0][2] = obb.orientations[0].z;
		worldMatrix.matrix[0][3] = 0.0f;

		worldMatrix.matrix[1][0] = obb.orientations[1].x;
		worldMatrix.matrix[1][1] = obb.orientations[1].y;
		worldMatrix.matrix[1][2] = obb.orientations[1].z;
		worldMatrix.matrix[1][3] = 0.0f;

		worldMatrix.matrix[2][0] = obb.orientations[2].x;
		worldMatrix.matrix[2][1] = obb.orientations[2].y;
		worldMatrix.matrix[2][2] = obb.orientations[2].z;
		worldMatrix.matrix[2][3] = 0.0f;

		worldMatrix.matrix[3][0] = obb.center.x;
		worldMatrix.matrix[3][1] = obb.center.y;
		worldMatrix.matrix[3][2] = obb.center.z;
		worldMatrix.matrix[3][3] = 1.0f;

		return worldMatrix;
	}
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.matrix[0][0] + vector.y * matrix.matrix[1][0] + vector.z * matrix.matrix[2][0] + matrix
		.matrix[3][0];
	result.y = vector.x * matrix.matrix[0][1] + vector.y * matrix.matrix[1][1] + vector.z * matrix.matrix[2][1] + matrix
		.matrix[3][1];
	result.z = vector.x * matrix.matrix[0][2] + vector.y * matrix.matrix[1][2] + vector.z * matrix.matrix[2][2] + matrix
		.matrix[3][2];

	result.x /= vector.x * matrix.matrix[0][3] + vector.y * matrix.matrix[1][3] + vector.z * matrix.matrix[2][3] +
		matrix.matrix[3][3];
	result.y /= vector.x * matrix.matrix[0][3] + vector.y * matrix.matrix[1][3] + vector.z * matrix.matrix[2][3] +
		matrix.matrix[3][3];
	result.z /= vector.x * matrix.matrix[0][3] + vector.y * matrix.matrix[1][3] + vector.z * matrix.matrix[2][3] +
		matrix.matrix[3][3];

	return result;
}


Matrix4x4 MakeTranslationMatrix(const Vector3& translation) {
	Matrix4x4 MakeTransla;
	MakeTransla.matrix[0][0] = 1.0f;
	MakeTransla.matrix[0][1] = 0.0f;
	MakeTransla.matrix[0][2] = 0.0f;
	MakeTransla.matrix[0][3] = 0.0f;

	MakeTransla.matrix[1][0] = 0.0f;
	MakeTransla.matrix[1][1] = 1.0f;
	MakeTransla.matrix[1][2] = 0.0f;
	MakeTransla.matrix[1][3] = 0.0f;

	MakeTransla.matrix[2][0] = 0.0f;
	MakeTransla.matrix[2][1] = 0.0f;
	MakeTransla.matrix[2][2] = 1.0f;
	MakeTransla.matrix[2][3] = 0.0f;

	MakeTransla.matrix[3][0] = translation.x;
	MakeTransla.matrix[3][1] = translation.y;
	MakeTransla.matrix[3][2] = translation.z;
	MakeTransla.matrix[3][3] = 1.0f;

	return MakeTransla;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 MakeScale;
	MakeScale.matrix[0][0] = scale.x;
	MakeScale.matrix[0][1] = 0.0f;
	MakeScale.matrix[0][2] = 0.0f;
	MakeScale.matrix[0][3] = 0.0f;
	MakeScale.matrix[1][0] = 0.0f;
	MakeScale.matrix[1][1] = scale.y;
	MakeScale.matrix[1][2] = 0.0f;
	MakeScale.matrix[1][3] = 0.0f;
	MakeScale.matrix[2][0] = 0.0f;
	MakeScale.matrix[2][1] = 0.0f;
	MakeScale.matrix[2][2] = scale.z;
	MakeScale.matrix[2][3] = 0.0f;
	MakeScale.matrix[3][0] = 0.0f;
	MakeScale.matrix[3][1] = 0.0f;
	MakeScale.matrix[3][2] = 0.0f;
	MakeScale.matrix[3][3] = 1.0f;
	return MakeScale;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 affineMatrix;
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXMatrixYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 translateMatrix = MakeTranslationMatrix(translate);
	affineMatrix = Multiply(Multiply(scaleMatrix, rotateXMatrixYZMatrix), translateMatrix);
	return affineMatrix;
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                uint32_t color) {
	constexpr uint32_t kSegmentCount = 16;
	constexpr float kLonEvery = 2.0f * 3.14159265f / static_cast<float>(kSegmentCount);
	constexpr float kLatEvery = 3.14159265f / static_cast<float>(kSegmentCount);

	for (uint32_t latIndex = 0; latIndex < kSegmentCount; ++latIndex) {
		float lat = -3.14159265f / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSegmentCount; ++lonIndex) {
			float lon = kLonEvery * lonIndex;

			Vector3 a{
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon),
			};
			Vector3 b{
				sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon),
				sphere.center.y + sphere.radius * std::sin(lat + kLatEvery),
				sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon),
			};
			Vector3 c{
				sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery),
				sphere.center.y + sphere.radius * std::sin(lat),
				sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery),
			};

			Vector3 screenA = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 screenB = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 screenC = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);
			(void)(color);
		}
	}
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float dotProduct = Dot(v1, v2);
	float lengthSquared = Dot(v2, v2);
	if (lengthSquared == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	return Multiply(dotProduct / lengthSquared, v2);
}


Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 segmentVector = Subtract(segment.diff, segment.origin);
	Vector3 pointVector = Subtract(point, segment.origin);
	float t = Dot(pointVector, segmentVector) / Dot(segmentVector, segmentVector);

	return Add(segment.origin, Multiply(t, segmentVector));
}

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(Subtract(s1.center, s2.center));
	return distance <= s1.radius + s2.radius;
}

bool PlaneIsCollision(const Sphere& sphere, const Plane& plane) {
	float distance = Dot(sphere.center, plane.normal) - plane.distance;
	return std::fabs(distance) <= sphere.radius;
}

Vector3 Perpendicular(const Vector3& v) {
	if (v.x != 0.0f || v.y != 0.0f) {
		return {-v.y, v.x, 0.0f};
	}
	return {0.0f, -v.z, v.y};
}

void DrawPlane(
	const Plane& plane,
	const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix,
	uint32_t color
) {
	Vector3 center = Multiply(plane.distance, plane.normal);

	Vector3 perpendicular[4];
	perpendicular[0] = Normalize(Perpendicular(plane.normal));
	perpendicular[1] = {
		-perpendicular[0].x,
		-perpendicular[0].y,
		-perpendicular[0].z
	};

	perpendicular[2] = Cross(plane.normal, perpendicular[0]);
	perpendicular[3] = {
		-perpendicular[2].x,
		-perpendicular[2].y,
		-perpendicular[2].z
	};

	Vector3 points[4];

	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendicular[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(
			Transform(point, viewProjectionMatrix),
			viewportMatrix
		);
	}
	(void)(color);
}


bool SegmentIsCollsion(const Segment& segment, const Plane& plane) {
	float distanceOrigin = Dot(segment.origin, plane.normal) - plane.distance;
	float distanceDiff = Dot(Add(segment.origin, segment.diff), plane.normal) - plane.distance;
	return (distanceOrigin * distanceDiff) <= 0.0f;
}

void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                 uint32_t color) {
	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);
	(void)(color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix,
                  uint32_t color) {
	Vector3 screenVertices[3];
	for (int32_t index = 0; index < 3; ++index) {
		screenVertices[index] = Transform(Transform(triangle.vertices[index], viewProjectionMatrix), viewportMatrix);
	}
	(void)(color);
}

bool TriangleToSegmentIsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 p = ClosestPoint(segment.origin, segment);

	Vector3 cross01 = Cross(Subtract(triangle.vertices[1], triangle.vertices[0]), Subtract(p, triangle.vertices[0]));
	Vector3 cross12 = Cross(Subtract(triangle.vertices[2], triangle.vertices[1]), Subtract(p, triangle.vertices[1]));
	Vector3 cross20 = Cross(Subtract(triangle.vertices[0], triangle.vertices[2]), Subtract(p, triangle.vertices[2]));

	if (Dot(cross01, cross12) >= 0.0f && Dot(cross12, cross20) >= 0.0f && Dot(cross20, cross01) >= 0.0f) {
		return true;
	}


	return false;
}


void DrawAABB(
	const AABB& aabb,
	const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix,
	uint32_t color
) {
	Vector3 vertices[8] = {
		{aabb.min.x, aabb.min.y, aabb.min.z}, // 0
		{aabb.max.x, aabb.min.y, aabb.min.z}, // 1
		{aabb.max.x, aabb.max.y, aabb.min.z}, // 2
		{aabb.min.x, aabb.max.y, aabb.min.z}, // 3

		{aabb.min.x, aabb.min.y, aabb.max.z}, // 4
		{aabb.max.x, aabb.min.y, aabb.max.z}, // 5
		{aabb.max.x, aabb.max.y, aabb.max.z}, // 6
		{aabb.min.x, aabb.max.y, aabb.max.z}, // 7
	};

	Vector3 screenVertices[8];

	Matrix4x4 matrix = Multiply(viewProjectionMatrix, viewportMatrix);

	for (int i = 0; i < 8; i++) {
		screenVertices[i] = Transform(vertices[i], matrix);
	}

	int32_t edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
	};

	for (int i = 0; i < 12; i++) {
		const Vector3& start = screenVertices[edges[i][0]];
		const Vector3& end = screenVertices[edges[i][1]];
		(void)(start);
		(void)(end);
	}
	(void)(color);
	(void)(screenVertices);
}


bool AABBToAABBIsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}


bool AABBToSphereIsCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 closestPoint(std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
	                     std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
	                     std::clamp(sphere.center.z, aabb.min.z, aabb.max.z));

	float distance = Length(Subtract(closestPoint, sphere.center));
	return distance <= sphere.radius;
}

bool AABBIntersectsSegment(const AABB& aabb, const Segment& segment) {
	Vector3 start = segment.origin;
	Vector3 end = Add(segment.origin, segment.diff);

	float tMin = 0.0f;
	float tMax = 1.0f;

	Vector3 dir = segment.diff;

	constexpr float epsilon = 0.00001f;

	float min[3] = {aabb.min.x, aabb.min.y, aabb.min.z};
	float max[3] = {aabb.max.x, aabb.max.y, aabb.max.z};
	float p[3] = {start.x, start.y, start.z};
	float d[3] = {dir.x, dir.y, dir.z};

	for (int i = 0; i < 3; i++) {
		if (std::fabs(d[i]) < epsilon) {
			if (p[i] < min[i] || p[i] > max[i]) {
				return false;
			}
		}
		else {
			float t1 = (min[i] - p[i]) / d[i];
			float t2 = (max[i] - p[i]) / d[i];

			if (t1 > t2) {
				std::swap(t1, t2);
			}

			tMin = (std::max)(tMin, t1);
			tMax = (std::min)(tMax, t2);


			if (tMin > tMax) {
				return false;
			}
		}
	}

	return true;
}

bool OBBToSphereIsCollision(const OBB& obb, const Sphere& sphere) {
	Matrix4x4 obbWorldMatrix = MakeOBBWorldMatrix(obb);
	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrixInverse);
	AABB aabbOBBLocal{
		.min{-obb.size.x, -obb.size.y, -obb.size.z},
		.max{obb.size.x, obb.size.y, obb.size.z},
	};
	Sphere sphereOBBLocal{
		.center{centerInOBBLocalSpace},
		.radius{sphere.radius},
	};

	return AABBToSphereIsCollision(aabbOBBLocal, sphereOBBLocal);
}

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 localVertices[8] = {
		{-obb.size.x, -obb.size.y, -obb.size.z},
		{obb.size.x, -obb.size.y, -obb.size.z},
		{obb.size.x, obb.size.y, -obb.size.z},
		{-obb.size.x, obb.size.y, -obb.size.z},
		{-obb.size.x, -obb.size.y, obb.size.z},
		{obb.size.x, -obb.size.y, obb.size.z},
		{obb.size.x, obb.size.y, obb.size.z},
		{-obb.size.x, obb.size.y, obb.size.z},
	};

	Vector3 screenVertices[8];
	Matrix4x4 obbWorldMatrix = MakeOBBWorldMatrix(obb);
	Matrix4x4 worldViewProjectionMatrix = Multiply(obbWorldMatrix, viewProjectionMatrix);
	Matrix4x4 worldViewProjectionViewportMatrix = Multiply(worldViewProjectionMatrix, viewportMatrix);

	for (int32_t index = 0; index < 8; ++index) {
		screenVertices[index] = Transform(localVertices[index], worldViewProjectionViewportMatrix);
	}

	int32_t edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
	};

	for (int32_t index = 0; index < 12; ++index) {
		const Vector3& start = screenVertices[edges[index][0]];
		const Vector3& end = screenVertices[edges[index][1]];
		(void)(start);
		(void)(end);
	}
	(void)(color);
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	constexpr float kGridHalfWidth = 2.0f;
	constexpr uint32_t kSubdivision = 10;
	constexpr float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + xIndex * kGridEvery;
		Vector3 start{x, 0.0f, -kGridHalfWidth};
		Vector3 end{x, 0.0f, kGridHalfWidth};

		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);

		uint32_t color = (std::fabs(x) < 0.0001f) ? 0xFFFFFFFF : 0xAAAAAAFF;
		(void)(color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + zIndex * kGridEvery;
		Vector3 start{-kGridHalfWidth, 0.0f, z};
		Vector3 end{kGridHalfWidth, 0.0f, z};

		Vector3 ndcStart = Transform(start, viewProjectionMatrix);
		Vector3 ndcEnd = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStart, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEnd, viewportMatrix);

		uint32_t color = (std::fabs(z) < 0.0001f) ? 0xFFFFFFFF : 0xAAAAAAFF;
		(void)(color);
	}
}


//========================================================
// 物理ゾーン
//===============-	===================================================

Ball Springsmire(const Ball& ball, const Spring& spring, float deltaTime) {
	Ball resultBall = ball;
	Vector3 diff = ball.position - spring.anchor;
	float length = Length(diff);
	if (length != 0.0f) {
		Vector3 direction = Normalize(diff);
		Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
		Vector3 displacement = length * (ball.position - restPosition);
		Vector3 restoringForce = -spring.stiffness * displacement;
		Vector3 force = restoringForce;
		resultBall.acceleration = force * (1.0f / ball.mass);
		//減衰抵抗を計算する
		Vector3 dampingForce = -spring.dampingCoefficient * ball.velocity;
		resultBall.acceleration += dampingForce * (1.0f / ball.mass);
	}

	resultBall.velocity += resultBall.acceleration * deltaTime;
	resultBall.position += resultBall.velocity * deltaTime;
	return resultBall;
}
