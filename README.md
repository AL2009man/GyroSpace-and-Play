# GyroSpace-and-Play

A plug-and-play header solution (for C and C++ Project) on implementing Gyro Space for your project. 


## Example Code:

If you'd like to have a good idea on how to implement Gyro Space orientation codeset, here's a basic example of what a generic gyro aiming's Axis Selection code could looks like:

### C Project:

```
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
```

### C++ Projects:

```
class GyroProcessor {
public:
    GyroProcessor() {}

    Vector3 processGyro(Vector3 rawGyro, int mode) {
        Vector3 gravityVector = GetGravityVector(); // Retrieve gravity vector internally

        switch (mode) {
        case 1: // Local Space
            return TransformToLocalSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                0.0f // Coupling factor for local space
            );
        case 2: // Player Space
            return TransformToPlayerSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                gravityVector
            );
        case 3: // World Space
            return TransformToWorldSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                gravityVector
            );
        default:
            std::cerr << "Error: Invalid mode selected.\n";
            return Vec3_New(0.0f, 0.0f, 0.0f); // Consistent vector initialization
        }
    }
};

int main() {
    // Simulated raw gyro input values (replace with real sensor data)
    Vector3 rawGyro = Vec3_New(10.0f, 5.0f, 3.0f);

    // Create processor without sensitivity parameters
    GyroProcessor processor;

    // Select gyro mode (1 = Local, 2 = Player, 3 = World)
    int mode = 2;

    // Debug output for mode selection
    std::cout << "Applying Gyro Transformation (Mode " << mode << ")..." << std::endl;

    // Process transformation
    Vector3 transformedGyro = processor.processGyro(rawGyro, mode);

    // Print results
    std::cout << "Transformed Gyro Output - X: " << transformedGyro.x
        << ", Y: " << transformedGyro.y
        << ", Z: " << transformedGyro.z << std::endl;

    return 0;
}

```

As a reminder: these two will give you a idea on how to implement Gyro Space code, but if you want a live implementation, here's a codeset from [Yamagi Quake 2](https://github.com/yquake2/yquake2), which uses SDL's Gamepad API to handle Motion Sensors.


### Live Example (C Project):
```
switch ((int)gyro_turning_axis->value) {
    case 0:  // Yaw mode
        gyro_yaw = event.gsensor.data[1] - gyro_calibration_y->value;  // Yaw
        gyro_pitch = event.gsensor.data[0] - gyro_calibration_x->value;  // Pitch
        break;

    case 1:  // Roll mode
        gyro_yaw = -(event.gsensor.data[2] - gyro_calibration_z->value);  // Roll
        gyro_pitch = event.gsensor.data[0] - gyro_calibration_x->value;  // Pitch
        break;

    case 2: // Local Space mode
    {
        Vector3 localGyro = TransformToLocalSpace(
            event.gsensor.data[1] - gyro_calibration_y->value,
            event.gsensor.data[0] - gyro_calibration_x->value,
            event.gsensor.data[2] - gyro_calibration_z->value,
            0.0f
        );

        gyro_yaw = localGyro.x;
        gyro_pitch = localGyro.y;
        gyro_roll = localGyro.z;
    }
    break;

    case 3: // Player Space mode
    {
        Vector3 playerGyro = TransformToPlayerSpace(
            event.gsensor.data[1] - gyro_calibration_y->value,
            event.gsensor.data[0] - gyro_calibration_x->value,
            event.gsensor.data[2] - gyro_calibration_z->value,
            GetGravityVector()
        );

        gyro_yaw = playerGyro.x;
        gyro_pitch = playerGyro.y;
        gyro_roll = playerGyro.z;
    }
    break;

    case 4: // World Space mode
    {
        Vector3 worldGyro = TransformToWorldSpace(
            event.gsensor.data[1] - gyro_calibration_y->value,
            event.gsensor.data[0] - gyro_calibration_x->value,
            event.gsensor.data[2] - gyro_calibration_z->value,
            GetGravityVector()
        );

        gyro_yaw = worldGyro.x;
        gyro_pitch = worldGyro.y;
        gyro_roll = worldGyro.z;
    }
    break;

    default:
        gyro_yaw = gyro_pitch = 0;  // Reset for unsupported modes
        break;
}

```
