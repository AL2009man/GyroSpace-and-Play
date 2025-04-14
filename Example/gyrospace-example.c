#include <stdio.h>
#include "header/GyroSpace/GyroSpace.h"

// Generic function for handling gyro transformations
Vector3 ProcessGyroInput(Vector3 rawGyro, Vector3 gravityVector, float yawSensitivity, float pitchSensitivity, float rollSensitivity, int mode) {
    Vector3 outputGyro;
    switch (mode) {
    case 1: // Local Space Mode
        outputGyro = TransformToLocalSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            yawSensitivity, pitchSensitivity, rollSensitivity, 0.075f
        );
        break;
    case 2: // Player Space Mode
        outputGyro = TransformToPlayerSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector, yawSensitivity, pitchSensitivity, rollSensitivity
        );
        break;
    case 3: // World Space Mode
        outputGyro = TransformToWorldSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector, yawSensitivity, pitchSensitivity, rollSensitivity
        );
        break;
    default:
        printf("Error: Invalid mode selected.\n");
        outputGyro = Vec3_New(0.0f, 0.0f, 0.0f);
    }
    return outputGyro;
}

// Example usage with simulated gyro input
int main() {
    // Simulated raw gyro input values (replace with actual sensor data)
    Vector3 rawGyro = Vec3_New(10.0f, 5.0f, 3.0f);
    Vector3 gravityVector = GetGravityVector(); // Retrieve global gravity

    // Sensitivity settings (fully configurable)
    float yawSensitivity = 1.0f;
    float pitchSensitivity = 1.0f;
    float rollSensitivity = 1.0f;

    // Select gyro mode (1 = Local, 2 = Player, 3 = World)
    int mode = 2;

    // Apply transformation
    Vector3 transformedGyro = ProcessGyroInput(rawGyro, gravityVector, yawSensitivity, pitchSensitivity, rollSensitivity, mode);

    // Print results
    printf("Transformed Gyro Output - X: %f, Y: %f, Z: %f\n", transformedGyro.x, transformedGyro.y, transformedGyro.z);

    return 0;
}
