# GyroSpace-and-Play

A plug-and-play header solution (for C and C++ projects) for implementing Axis orientations (also known as Gyro Space orientation) for your project that uses Motion Sensor Aiming/Gyro Aiming to your game, 

# What is this repository all about?

In 2023, two sourceport devs contemplated Gyro Space orientation code, thinking it would be very hard to implement while not being unintuitive/returns on QoL. I even wished for an easy plug-and-play solution, although GamepadMotionHelpers already provides that. In 2025: I got bored and decided to play around with Bing AI for a fun experiment, coming off the heels of the work from UnleashedRecomp project. I originally created it for Yamagi Quake 2, but my ambition grew, and I decided to repurpose it into a generic header. 

This repository is intended to make it a very plug-and-play implementation of Gyro Space orientation derived from [GyroWiki's Player Space Gyro article](http://gyrowiki.jibbsmart.com/blog:player-space-gyro-and-alternatives-explained), while including Matrix, Gravity Vectors and Dynamic Orientation. This code is designed with C projects in mind, but is compatible with C++ Projects.

All you need is a subset of codes to implement, and now you have Gyro Space support!


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
            0.0f // Roll coupling factor for local space
        );
        break;
    case 2: // Player Space Mode
        outputGyro = TransformToPlayerSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector
        );
        break;
    case 3: // World Space Mode
        outputGyro = TransformToWorldSpace(
            rawGyro.x, rawGyro.y, rawGyro.z,
            gravityVector
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

# Q&A

## 1. I'm working on my Gyro Aiming implementation from scratch, but I want to have a robust Gyro Space implementation.

If you're building a Gyro Aiming implementation on a C Project from scratch, without using any repository that already comes with Motion Sensor implementation? This repository will come included with it.


## 2. I'm working on my Gyro Aiming implementation using GamepadMotionHelpers header, can I still use Gyro Space and Play header with it?

No, and I highly recommend using GHM instead for a more complete system. However: there are plans for GamepadMotionHelper support as an option, making it more akin to an Abstraction Layer on top of GamepadMotionHelper.
However, [it's still a work-in-progress](https://github.com/AL2009man/GyroSpace-and-Play/pull/1).

## 3. Is this project truly compatible with C++ Projects?

While this header was originally designed around C Projects; I tried ensuring it works on a C++ project, but I have yet to battle test it. Refer to https://github.com/AL2009man/GyroSpace-and-Play/issues/2

## 4. Can I use this header using a different library?

While this project is best suited for SDL Input API; The header is generic enough to allow support for others like PlayStation Input API, Microsoft GameInput, JoyShockLibrary, etc.

## 5. Where do I start implementing Gyro Orientation, but I don't know where to begin?

Let's begin by using SDL Inputs, specifically: SDL3 Input.

First off: start by implementing Yaw and Roll axis to your Gyro Axis menu first. Once you have tested them: your Yaw and Roll modes will be used as your basis. Make sure you already have Yaw, Roll, Pitcha axis and the X, Y, Z Camera axis ready in advance.

After that, start by making a case number for Loacl Space, Player Space and World Space alongside this codeset:

```Vector3 [local/player/world]Gyro = TransformTo[Local/Player/World]Space(```

then: place `event.gsensor.data` portions down below. 

Something like this:

```
event.gsensor.data[1]
event.gsensor.data[0]
event.gsensor.data[2]
```

Near the end, you'd need to place either:

* for Local Space: just make sure you place `0.0f` at the end, this will cover the coupling factor
* for Player/World Space:, make sure `GetGravityVector()` is placed at the end instead.

And lastly: place `gyro_yaw/roll/pitch` (or whatever name that your game engine handles it's gyro input) alongside the `[local/player/world]Gyro`, also depends on the naming scheme you went with.

And now you're done, but if you want a clearer picture on what would a Gyro Space 

Here's a codeset from [Yamagi Quake 2](https://github.com/yquake2/yquake2), which uses SDL's Gamepad API to handle Motion Sensors. This will give you a good idea on how that approach could be done.

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


# Credits

* Jibb Smart - for creating and providing a guideline on making and improving orientation code! (and also [GyroWiki](http://gyrowiki.jibbsmart.com/), [JoyShockMapper](https://github.com/Electronicks/JoyShockMapper) and [GamepadMotionHelpers](https://github.com/JibbSmart/GamepadMotionHelpers)!) If it weren't for you: this project wouldn't happened!
* HilariousCrow - for being the inspiration for the Dynamic Orientation system.
* CasperH and BenjaminLSR ([Handheld Companion](https://github.com/Valkirie/HandheldCompanion)) - for providing advice, while also serving as the inspiration for the Dynamic Orientation system
* Protocultor - for setting the foundation for this entire header. :P
