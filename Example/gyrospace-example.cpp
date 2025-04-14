#include <iostream>
#include "header/GyroSpace/GyroSpace.h"

class GyroProcessor {
public:
    GyroProcessor(float yawSens, float pitchSens, float rollSens)
        : yawSensitivity(yawSens), pitchSensitivity(pitchSens), rollSensitivity(rollSens) {
    }

    Vector3 processGyro(Vector3 rawGyro, int mode) {
        switch (mode) {
        case 1: // Local Space
            return GyroSpace::TransformToLocalSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                yawSensitivity, pitchSensitivity, rollSensitivity, 0.075f);
        case 2: // Player Space
            return GyroSpace::TransformToPlayerSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                GyroSpace::GetGravityVector(), yawSensitivity, pitchSensitivity, rollSensitivity);
        case 3: // World Space
            return GyroSpace::TransformToWorldSpace(rawGyro.x, rawGyro.y, rawGyro.z,
                GyroSpace::GetGravityVector(), yawSensitivity, pitchSensitivity, rollSensitivity);
        default:
            std::cerr << "Invalid mode selected.\n";
            return Vector3{ 0.0f, 0.0f, 0.0f };
        }
    }

private:
    float yawSensitivity;
    float pitchSensitivity;
    float rollSensitivity;
};

int main() {
    // Simulated raw gyro input values (replace with real sensor data)
    Vector3 rawGyro{ 10.0f, 5.0f, 3.0f };

    // Create processor with adjustable sensitivity
    GyroProcessor processor(1.0f, 1.0f, 1.0f);

    // Select gyro mode (1 = Local, 2 = Player, 3 = World)
    int mode = 2;

    // Process transformation
    Vector3 transformedGyro = processor.processGyro(rawGyro, mode);

    // Print results
    std::cout << "Transformed Gyro Output - X: " << transformedGyro.x
        << ", Y: " << transformedGyro.y
        << ", Z: " << transformedGyro.z << std::endl;

    return 0;
}
