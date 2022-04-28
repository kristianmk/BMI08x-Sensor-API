// Written by K. M. Knausgård 2022-04
// Some parts based on Bosch example "read_synchronized_data_mcu".

#include <cstdint>
#include <iostream>


#include "vector3d.h"
#include "bmi08ximu.h"


namespace constants {
    const double seaLevelGravity = 9.80665;
}


int main()
{
    Bmi08xImu imu;

    imu.init();


    // TODO(KMK) replace with pigpio interrupt!
    //coines_attach_interrupt(COINES_SHUTTLE_PIN_20, bmi08x_data_sync_int, COINES_PIN_INTERRUPT_FALLING_EDGE);


    // Run synchronous for some time:
    imu.enableDataSynchronizationInterrupt();
    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < 0.1)
    {
        if (imu.dataAvailable())
        {
            imu.readSynchronous();

            const auto acclRawValue = imu.accelerationRaw();
            const auto gyroRawValue = imu.gyroRaw();

            imu.getMeasurementsRaw();
        }

        // Could add a small delay here..
    }

    imu.disableDataSynchronizationInterrupt();

    // TODO(KMK): Remember checking disable interrupt in pigpio.

    return 0;
}
