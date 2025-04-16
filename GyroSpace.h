/*
 * =======================================================================
 *
 * Gyro Space to Play - A plug-and-play Gyro Space Transformer code
 *
 * Provides functionality for transforming gyro inputs into Local Space,
 * Player Space, and World Space, while handling sensitivity adjustments
 * and gravity alignment. Compatible with both C and C++ environments.
 *
 * Based on the work by Jibb Smart (JoyShockMapper, GamepadMotionHelpers,
 * Fortnite v.19.30's Gyro Aim/Flick Stick implementation)
 *
 * =======================================================================
 */

#ifndef GYROSPACE_HPP
#define GYROSPACE_HPP

#ifdef __cplusplus
#include <cmath>
#include <cstdio>
#include <cstdbool>
#include <cstdint>
extern "C" {
#else
#include "math.h"
#include "stdbool.h"
#include "stdint.h"
#endif

 // Ensure `s32` and `u32` are correctly defined
#ifndef s32
#define s32 int32_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

// Apply correct boolean type definitions
#ifndef bool
#define bool s32
#endif

#ifndef ubool
#define ubool u32
#endif

#ifndef EPSILON
#define EPSILON 1e-5
#endif

 // ---- Debugging and Logging ----
#ifdef ENABLE_DEBUG_LOGS
#ifdef __cplusplus
#include <iostream>
#define DEBUG_LOG(fmt, ...) std::cout << fmt
#else
#define DEBUG_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif
#else
#define DEBUG_LOG(fmt, ...)
#endif

// ---- Type Definitions ----

typedef struct {
	float x, y, z;
} Vector3;

typedef struct {
	float m[4][4];
} Matrix4;

// ---- Utility Functions ----

/**
 * Clamps a value between a minimum and maximum.
 */
static inline float clamp(float value, float min, float max) {
	return (value > max) ? max : (value < min) ? min : value;
}

/**
 * Creates a new vector with given x, y, z values.
 */
static inline Vector3 Vec3_New(float x, float y, float z) {
	return (Vector3) { x, y, z };
}

/**
 * Adds two vectors component-wise.
 */
static inline Vector3 Vec3_Add(Vector3 a, Vector3 b) {
	return Vec3_New(a.x + b.x, a.y + b.y, a.z + b.z);
}

/**
 * Subtracts one vector from another.
 */
static inline Vector3 Vec3_Subtract(Vector3 a, Vector3 b) {
	return Vec3_New(a.x - b.x, a.y - b.y, a.z - b.z);
}

/**
 * Scales a vector by a scalar value.
 */
static inline Vector3 Vec3_Scale(Vector3 v, float scalar) {
	return Vec3_New(v.x * scalar, v.y * scalar, v.z * scalar);
}

/**
 * Computes the dot product of two vectors.
 */
static inline float Vec3_Dot(Vector3 a, Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * Computes the cross product of two vectors.
 */
static inline Vector3 Vec3_Cross(Vector3 a, Vector3 b) {
	return Vec3_New(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

/**
 * Computes the magnitude (length) of a vector.
 */
static inline float Vec3_Magnitude(Vector3 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

/**
 * Normalizes a vector (scales it to unit length).
 */
static inline Vector3 Vec3_Normalize(Vector3 v) {
	float magnitude = Vec3_Magnitude(v);
	if (magnitude < EPSILON) {
		DEBUG_LOG("Warning: Attempted to normalize a near-zero vector.\n");
		return Vec3_New(0.0f, 0.0f, 0.0f);
	}
	return Vec3_Scale(v, 1.0f / magnitude);
}

/**
 * Checks if a vector is near zero (all components close to zero).
 */
static inline bool Vec3_IsZero(Vector3 v) {
	return (fabsf(v.x) < EPSILON && fabsf(v.y) < EPSILON && fabsf(v.z) < EPSILON);
}

/**
 * Performs linear interpolation between two vectors.
 */
static inline Vector3 Vec3_Lerp(Vector3 a, Vector3 b, float t) {
	return Vec3_Add(a, Vec3_Scale(Vec3_Subtract(b, a), t));
}

/**
 * Reflects a vector against a normal.
 */
static inline Vector3 Vec3_Reflect(Vector3 v, Vector3 normal) {
	return Vec3_Subtract(v, Vec3_Scale(normal, 2.0f * Vec3_Dot(v, normal)));
}

// ---- Matrix Operations ----

/**
 * Returns an identity matrix.
 */
static inline Matrix4 Matrix4_Identity() {
	Matrix4 matrix = { {
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
	} };
	return matrix;
}

/**
 * Multiplies a matrix by a vector (row-major order).
 */
static inline Vector3 MultiplyMatrixVector(Matrix4 matrix, Vector3 vector) {
	// Assume w = 1.0 for 3D vectors
	float w = 1.0f;

	return Vec3_New(
		matrix.m[0][0] * vector.x + matrix.m[1][0] * vector.y + matrix.m[2][0] * vector.z + matrix.m[3][0] * w,
		matrix.m[0][1] * vector.x + matrix.m[1][1] * vector.y + matrix.m[2][1] * vector.z + matrix.m[3][1] * w,
		matrix.m[0][2] * vector.x + matrix.m[1][2] * vector.y + matrix.m[2][2] * vector.z + matrix.m[3][2] * w
	);
}

// ---- Global Gravity Vector Management ----

/**
 * Global gravity vector (default set to (0, 1, 0)).
 */
static Vector3 gravNorm = { 0.0f, 1.0f, 0.0f };

/**
 * Updates the global gravity vector using sensor fusion.
 */
static inline void UpdateGravityVector(Vector3 accel, Vector3 gyroRotation, float fusionFactor) {
	// Validate fusionFactor
	if (fusionFactor < 0.0f || fusionFactor > 1.0f) {
		DEBUG_LOG("Error: Invalid fusionFactor (%f). Must be between 0.0 and 1.0.\n", fusionFactor);
		return;
	}

	// Validate inputs for NaN values
	if (isnan(accel.x) || isnan(accel.y) || isnan(accel.z) ||
		isnan(gyroRotation.x) || isnan(gyroRotation.y) || isnan(gyroRotation.z)) {
		DEBUG_LOG("Error: NaN values detected in sensor inputs. Skipping update.\n");
		return;
	}

	// Normalize the accelerometer input to ensure valid gravity direction
	Vector3 accelNorm = Vec3_Normalize(accel);

	// Rotate existing gravity vector using gyroscope
	Vector3 rotatedGravity = Vec3_Add(gravNorm, Vec3_Cross(gyroRotation, gravNorm));

	// Blend the rotated gravity vector with accelerometer data using linear interpolation
	gravNorm = Vec3_Lerp(rotatedGravity, accelNorm, fusionFactor);

	// Normalize the updated gravity vector
	if (!Vec3_IsZero(gravNorm)) {
		gravNorm = Vec3_Normalize(gravNorm);
	}
	else {
		DEBUG_LOG("Warning: Gravity vector became near-zero. Resetting to default (0,1,0).\n");
		gravNorm = Vec3_New(0.0f, 1.0f, 0.0f);
	}

	DEBUG_LOG("UpdateGravityVector called - gravNorm = (%f, %f, %f)\n", gravNorm.x, gravNorm.y, gravNorm.z);
}

/**
 * Sets the gravity vector manually and ensures normalization.
 */
static inline void SetGravityVector(float x, float y, float z) {
	// Validate inputs
	if (isnan(x) || isnan(y) || isnan(z)) {
		DEBUG_LOG("Error: Gravity vector contains NaN values. Resetting to default (0,1,0).\n");
		gravNorm = Vec3_New(0.0f, 1.0f, 0.0f);
		return;
	}

	Vector3 newGravNorm = Vec3_New(x, y, z);

	// Ensure gravity vector isn't near-zero
	if (Vec3_Magnitude(newGravNorm) < EPSILON) {
		DEBUG_LOG("Warning: Gravity vector magnitude too small. Retaining previous value.\n");
		return;
	}

	// Normalize and update the gravity vector
	gravNorm = Vec3_Normalize(newGravNorm);

	DEBUG_LOG("SetGravityVector called - gravNorm = (%f, %f, %f)\n", gravNorm.x, gravNorm.y, gravNorm.z);
}

/**
 * Retrieves the current normalized gravity vector.
 */
static inline Vector3 GetGravityVector(void) {
	DEBUG_LOG("GetGravityVector called - gravNorm = (%f, %f, %f)\n", gravNorm.x, gravNorm.y, gravNorm.z);
	return gravNorm;
}

/**
 * Simulates receiving new sensor data and updates the gravity vector.
 */
void ProcessSensorData(Vector3 accel, Vector3 gyroRotation, float fusionFactor) {
	// Call UpdateGravityVector with the new sensor data
	UpdateGravityVector(accel, gyroRotation, fusionFactor);
}

/**
 * Simulates manually setting the gravity vector.
 */
void AdjustGravityVectorManually(float x, float y, float z) {
	// Call SetGravityVector with the provided values
	SetGravityVector(x, y, z);
}

// ---- Gyro Space Transformations ----

#ifdef __cplusplus
extern "C" {
#endif

	// Function declarations for gyro space transformations
	Vector3 TransformToLocalSpace(float yaw, float pitch, float roll,
		float yawSensitivity, float pitchSensitivity,
		float rollSensitivity, float couplingFactor);

	Vector3 TransformToPlayerSpace(float yaw_input, float pitch_input, float roll_input,
		Vector3 gravNorm, float yawSensitivity, float pitchSensitivity, float rollSensitivity);

	Vector3 TransformToWorldSpace(float yaw_input, float pitch_input, float roll_input,
		Vector3 gravNorm, float yawSensitivity, float pitchSensitivity, float rollSensitivity);

#ifdef __cplusplus
}
#endif


// Dynamic Orientation Adjustment

/**
 * Dynamically adjusts gyro space transformation based on controller orientation.
 */
Vector3 TransformWithDynamicOrientation(float yaw_input, float pitch_input, float roll_input,
	float yawSensitivity, float pitchSensitivity, float rollSensitivity, float couplingFactor) {

	static float smoothedTiltFactor = 1.0f; // Persistent variable to store the smoothed tilt factor
	Vector3 gravNorm = GetGravityVector();

	// ---- Compute Tilt Factor ----
	float tiltFactor = powf(fabsf(gravNorm.y), 0.75f); // Exponential smoothing for sharper transition
	float blendAmount = clamp(tiltFactor, 0.0f, 1.0f);

	// ---- Smooth the Tilt Factor ----
	const float smoothingFactor = 0.1f; // Adjust this value for faster/slower transitions
	smoothedTiltFactor = smoothedTiltFactor + smoothingFactor * (blendAmount - smoothedTiltFactor);

	// ---- Adjust Dynamic Sensitivity ----
	float dynamicYaw = yaw_input * yawSensitivity; // Focus on yaw for yaw adjustments

	// Refine dynamicPitch to include dynamic scaling
	float dynamicPitch = (smoothedTiltFactor * pitch_input * pitchSensitivity) + ((1.0f - smoothedTiltFactor) * pitch_input);

	// Refine dynamicRoll to retain more sensitivity
	float dynamicRoll = (smoothedTiltFactor * roll_input * rollSensitivity) + ((1.0f - smoothedTiltFactor) * roll_input);

	// ---- Debug Logs ----
	DEBUG_LOG("Dynamic Orientation Adjustment:\n");
	DEBUG_LOG("  Roll Input: %f, Yaw Input: %f, Pitch Input: %f\n", roll_input, yaw_input, pitch_input);
	DEBUG_LOG("  Gravity Vector: (%f, %f, %f)\n", gravNorm.x, gravNorm.y, gravNorm.z);
	DEBUG_LOG("  Tilt Factor: %f, Smoothed Tilt Factor: %f\n", tiltFactor, smoothedTiltFactor);
	DEBUG_LOG("  Dynamic Yaw: %f, Dynamic Pitch: %f, Dynamic Roll: %f\n", dynamicYaw, dynamicPitch, dynamicRoll);

	// ---- Apply Local Space transformation first ----
	Vector3 localGyro = TransformToLocalSpace(dynamicYaw, dynamicPitch, dynamicRoll, yawSensitivity, pitchSensitivity, rollSensitivity, couplingFactor);

	// ---- Further transformation into either Player or World Space ----
	Vector3 adjustedGyro = (gravNorm.y > 0.5f)
		? TransformToPlayerSpace(localGyro.x, localGyro.y, localGyro.z, gravNorm, yawSensitivity, pitchSensitivity, rollSensitivity)
		: TransformToWorldSpace(localGyro.x, localGyro.y, localGyro.z, gravNorm, yawSensitivity, pitchSensitivity, rollSensitivity);

	// ---- Return final transformed vector ----
	return adjustedGyro;
}

// Gyro Space Transformation Functions

/**
 * Transforms gyro inputs to Local Space
 */
Vector3 TransformToLocalSpace(float yaw, float pitch, float roll,
	float yawSensitivity, float pitchSensitivity,
	float rollSensitivity, float couplingFactor) {

	// ---- Persistent Smoothed Tilt Factor ----
	static float smoothedTiltFactor = 1.0f;

	// ---- Compute Tilt Factor ----
	float tiltFactor = powf(fabsf(GetGravityVector().y), 0.75f);
	float blendAmount = clamp(tiltFactor, 0.0f, 1.0f);

	// ---- Smooth the Tilt Factor ----
	const float smoothingFactor = 0.1f; // Adjust for faster/slower transitions
	smoothedTiltFactor = smoothedTiltFactor + smoothingFactor * (blendAmount - smoothedTiltFactor);

	// ---- Adjust roll to compensate for yaw-roll coupling ----
	float adjustedRoll = (smoothedTiltFactor * roll * rollSensitivity) - (yaw * couplingFactor);
	DEBUG_LOG("Adjusted Roll (Yaw-Roll Coupling): %f\n", adjustedRoll);

	// ---- Apply individual sensitivity scaling with smoothed tilt factor ----
	Vector3 rawGyro = Vec3_New(
		(yaw * yawSensitivity) - adjustedRoll,
		(smoothedTiltFactor * pitch * pitchSensitivity) + ((1.0f - smoothedTiltFactor) * pitch),
		(smoothedTiltFactor * roll * rollSensitivity) + ((1.0f - smoothedTiltFactor) * roll)
	);
	DEBUG_LOG("Raw Gyro (After Sensitivity Scaling): (%f, %f, %f)\n", rawGyro.x, rawGyro.y, rawGyro.z);

	// ---- Refined Roll Drift Prevention ----
	rawGyro.z = -rawGyro.z * (1.0f - couplingFactor);
	DEBUG_LOG("Raw Gyro (After Roll Drift Prevention): (%f, %f, %f)\n", rawGyro.x, rawGyro.y, rawGyro.z);

	// ---- Return the transformed vector ----
	return rawGyro;
}

/**
 * Transforms gyro inputs to Player Space
 */
Vector3 TransformToPlayerSpace(float yaw_input, float pitch_input, float roll_input,
	Vector3 gravNorm, float yawSensitivity, float pitchSensitivity, float rollSensitivity) {

	// ---- Validate Gravity Vector ----
	if (Vec3_IsZero(gravNorm)) {
		DEBUG_LOG("Warning: Gravity vector is near-zero. Using default gravity vector.\n");
		gravNorm = Vec3_New(0.0f, 1.0f, 0.0f);
	}

	// ---- Compute Tilt Factor for Dynamic Orientation Adjustment ----
	static float smoothedTiltFactor = 1.0f;
	float tiltFactor = powf(fabsf(gravNorm.y), 0.75f);
	float blendAmount = clamp(tiltFactor, 0.0f, 1.0f);

	// Smooth the tilt factor
	const float smoothingFactor = 0.1f;
	smoothedTiltFactor = smoothedTiltFactor + smoothingFactor * (blendAmount - smoothedTiltFactor);

	// ---- Define Yaw Relaxation Factor ----
	float yawRelaxFactor = 1.41;

	// ---- Adjust Inputs Dynamically ----
	float worldYaw = yaw_input * gravNorm.y + roll_input * gravNorm.z;
	float localYaw = (smoothedTiltFactor * yaw_input * yawSensitivity) + ((1.0f - smoothedTiltFactor) * roll_input * rollSensitivity);
	float adjustedYaw = worldYaw * yawRelaxFactor + localYaw * (1.0f - yawRelaxFactor);

	// Refine pitch and roll adjustments
	float adjustedPitch = (smoothedTiltFactor * pitch_input * pitchSensitivity) + ((1.0f - smoothedTiltFactor) * pitch_input);
	float adjustedRoll = (smoothedTiltFactor * roll_input * rollSensitivity) + ((1.0f - smoothedTiltFactor) * yaw_input * yawSensitivity);

	// ---- Flip roll BEFORE matrix transformation ----
	adjustedRoll = -adjustedRoll;

	// ---- Apply Player View Matrix ----
	Matrix4 playerViewMatrix = Matrix4_Identity();
	Vector3 adjustedGyro = Vec3_New(adjustedYaw, adjustedPitch, adjustedRoll);
	Vector3 playerGyro = MultiplyMatrixVector(playerViewMatrix, adjustedGyro);

	// ---- Return the transformed vector ----
	return playerGyro;
}

/**
 * Transforms gyro inputs to World Space
 */
Vector3 TransformToWorldSpace(float yaw_input, float pitch_input, float roll_input,
	Vector3 gravNorm, float yawSensitivity, float pitchSensitivity, float rollSensitivity) {

	// ---- Validate Gravity Vector ----
	if (Vec3_IsZero(gravNorm)) {
		DEBUG_LOG("Warning: Gravity vector is near-zero. Using default gravity vector.\n");
		gravNorm = Vec3_New(0.0f, 1.0f, 0.0f);
	}

	// ---- Compute Tilt Factor for Dynamic Orientation Adjustment ----
	static float smoothedTiltFactor = 1.0f;
	float tiltFactor = powf(fabsf(gravNorm.y), 0.75f);
	float blendAmount = clamp(tiltFactor, 0.0f, 1.0f);

	// Smooth the tilt factor
	const float smoothingFactor = 0.1f;
	smoothedTiltFactor = smoothedTiltFactor + smoothingFactor * (blendAmount - smoothedTiltFactor);

	// ---- Adjust Inputs Dynamically ----
	Vector3 rawGyro = Vec3_New(
		(smoothedTiltFactor * pitch_input * pitchSensitivity) + ((1.0f - smoothedTiltFactor) * pitch_input),
		-(yaw_input * yawSensitivity),
		(smoothedTiltFactor * roll_input * rollSensitivity) + ((1.0f - smoothedTiltFactor) * yaw_input * yawSensitivity)
	);

	// ---- Flip Roll BEFORE Gravity Alignment ----
	rawGyro.z = -rawGyro.z;

	// ---- Align Pitch Using Gravity ----
	float gravDotPitch = Vec3_Dot(gravNorm, Vec3_New(1.0f, 0.0f, 0.0f));
	Vector3 pitchAxis = Vec3_Subtract(Vec3_New(1.0f, 0.0f, 0.0f), Vec3_Scale(gravNorm, gravDotPitch));
	if (!Vec3_IsZero(pitchAxis)) {
		pitchAxis = Vec3_Normalize(pitchAxis);
	}

	// ---- Apply Gravity-Based Roll Adjustment ----
	float gravDotRoll = Vec3_Dot(gravNorm, Vec3_New(0.0f, 0.0f, 1.0f));
	Vector3 rollAxis = Vec3_Subtract(Vec3_New(0.0f, 0.0f, 1.0f), Vec3_Scale(gravNorm, gravDotRoll));
	if (!Vec3_IsZero(rollAxis)) {
		rollAxis = Vec3_Normalize(rollAxis);
	}

	// ---- Calculate Transformed Values ----
	Vector3 worldGyro = Vec3_New(
		-Vec3_Dot(rawGyro, gravNorm),
		Vec3_Dot(rawGyro, pitchAxis),
		Vec3_Dot(rawGyro, rollAxis)
	);

	// ---- Return the transformed vector ----
	return worldGyro;
}

#ifdef __cplusplus
}
#endif

#endif // GYROSPACE_H