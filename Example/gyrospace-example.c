// Generic function for handling gyro transformations
Vector3 ProcessGyroInput(Vector3 rawGyro, int mode) {
    Vector3 outputGyro;
    Vector3 gravityVector = GetGravityVector(); // Retrieve global gravity internally

    switch (mode) {
    case 1: // Local Space Mode
        outputGyro = TransformToLocalSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            0.0f // Coupling factor for local space
        );
        break;
    case 2: // Player Space Mode
        outputGyro = TransformToPlayerSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector // Removed sensitivity parameters
        );
        break;
    case 3: // World Space Mode
        outputGyro = TransformToWorldSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector // Removed sensitivity parameters
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

    // Select gyro mode (1 = Local, 2 = Player, 3 = World)
    int mode = 2;

    // Print selected mode for debugging
    printf("Applying Gyro Transformation (Mode %d)...\n", mode);

    // Apply transformation
    Vector3 transformedGyro = ProcessGyroInput(rawGyro, mode);

    // Print transformation confirmation
    printf("Gyro Transformation Applied!\n");

    // Print results
    printf("Transformed Gyro Output - X: %f, Y: %f, Z: %f\n", transformedGyro.x, transformedGyro.y, transformedGyro.z);

    return 0;
}
