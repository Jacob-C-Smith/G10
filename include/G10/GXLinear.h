#pragma once
#include <G10/GXDebug.h>
#include <math.h>

// 3D vector
struct GXvec3_s {
	float x;
	float y;
	float z;
};
typedef struct GXvec3_s GXvec3_t;

// 4D vector
struct GXvec4_s {
	float x;
	float y;
	float z;
	float w;
};
typedef struct GXvec4_s GXvec4_t;

// 4x4 matrix
struct GXmat4_s {
	float a, b, c, d;
	float e, f, g, h;
	float i, j, k, l;
	float m, n, o, p;
};
typedef struct GXmat4_s GXmat4_t;

// Computes the cross product of 2 3D vectors
static inline GXvec3_t crossProductVec3           (GXvec3_t a, GXvec3_t b)
{
	return (GXvec3_t) {
		a.y* b.z - a.z * b.y,
		a.z* b.x - a.x * b.z,
		a.x* b.y - a.y * b.x
	};
}

// Normailizes a vector
static inline GXvec3_t normalize(GXvec3_t v)
{
	float vl = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return (GXvec3_t) { v.x / vl, v.y / vl, v.z / vl };
}

// Multiplies a matrix by a vector
static inline GXvec4_t mat4xvec4(GXmat4_t m, GXvec4_t v)
{
	return (GXvec4_t) {
		m.a* v.x + m.b * v.y + m.c * v.z + m.d * v.w,
		m.e* v.x + m.f * v.y + m.g * v.z + m.h * v.w,
		m.i* v.x + m.j * v.y + m.k * v.z + m.l * v.w,
		m.m* v.x + m.n * v.y + m.o * v.z + m.p * v.w
	};
}

// Multiplies a matrix by a matrix
static inline GXmat4_t mat4xmat4(GXmat4_t m, GXmat4_t n)
{
	return (GXmat4_t) {
		(m.a * n.a + m.b * n.e + m.c * n.i + m.d * n.m), (m.a * n.b + m.b * n.f + m.c * n.j + m.d * n.n), (m.a * n.c + m.b * n.g + m.c * n.k + m.d * n.o), (m.a * n.d + m.b * n.h + m.c * n.l + m.d * n.p),
		(m.e * n.a + m.f * n.e + m.g * n.i + m.h * n.m), (m.e * n.b + m.f * n.f + m.g * n.j + m.h * n.n), (m.e * n.c + m.f * n.g + m.g * n.k + m.h * n.o), (m.e * n.d + m.f * n.h + m.g * n.l + m.h * n.p),
		(m.i * n.a + m.j * n.e + m.k * n.i + m.l * n.m), (m.i * n.b + m.j * n.f + m.k * n.j + m.l * n.n), (m.i * n.c + m.j * n.g + m.k * n.k + m.l * n.o), (m.i * n.d + m.j * n.h + m.k * n.l + m.l * n.p),
		(m.m * n.a + m.n * n.e + m.o * n.i + m.p * n.m), (m.m * n.b + m.n * n.f + m.o * n.j + m.p * n.n), (m.m * n.c + m.n * n.g + m.o * n.k + m.p * n.o), (m.m * n.d + m.n * n.h + m.o * n.l + m.p * n.p)
	};
}

// Returns the identity matrix
static inline GXmat4_t identityMat4()
{
	return (GXmat4_t) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

// Computes a translation matrix from a translation vector
static inline GXmat4_t translationMat(GXvec3_t v)
{
	return (GXmat4_t) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		v.x, v.y, v.z, 1
	};
}

// Computes scale matrix from a scale vector
static inline GXmat4_t scaleMat(GXvec3_t v)
{
	return (GXmat4_t) {
		v.x, 0, 0, 0,
		0, v.y, 0, 0,
		0, 0, v.z, 0,
		0, 0, 0, 1
	};
}

// Computes a translationScale matrix from a translation vector and a scale vector
static inline GXmat4_t translationScaleMat(GXvec3_t translation, GXvec3_t scale)
{
	return (GXmat4_t) {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		translation.x, translation.y, translation.z, 1
	};
}

// Computes a rotation matrix from a rotation vector
static inline GXmat4_t rotationMatrixFromVec(GXvec3_t v)
{
	return (GXmat4_t) {
		cosf(v.x) + powf(v.x, 2) * (1 - cosf(v.x))   , v.x* v.y* (1 - cosf(v.y)) - v.z * sinf(v.y)  , v.x* v.z* (1 - cosf(v.z)) + v.y * sinf(v.z), 0,
		v.y * v.x * (1 - cosf(v.x)) + v.z * sinf(v.x), cosf(v.y) + powf(v.y, 2) * (1 - cosf(v.y))   , v.y* v.z* (1 - cosf(v.z)) - v.x * sinf(v.z), 0,
		v.y * v.x * (1 - cosf(v.x)) + v.z * sinf(v.x), v.y * v.x * (1 - cosf(v.x)) + v.z * sinf(v.x), cosf(v.x) + powf(v.x, 2) * (1 - cosf(v.x)) , 0,
		0, 0, 0, 1
	};
}

// Computes a model matrix from a location, rotation, and scale vector.
static inline GXmat4_t generateModelMatrixFromVec(GXvec3_t location, GXvec3_t rotation, GXvec3_t scale)
{
	return mat4xmat4(translationScaleMat(location,scale),rotationMatrixFromVec(rotation));
}