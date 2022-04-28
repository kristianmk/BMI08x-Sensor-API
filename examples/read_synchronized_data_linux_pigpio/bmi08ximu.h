// Written by K. M. Knausgård 2022-04
// Some parts based on Bosch example "read_synchronized_data_mcu".
#ifndef BMI08XIMU_H
#define BMI08XIMU_H

#include <cstdint>

#include "../../bmi08x.h"
#include "../common/common.h"

#include "vector3d.h"
#include "sensordata.h"


class Imu
{
public:
    //virtual void activate() = 0;
    //virtual void deactivate() = 0;

    virtual void printMeasurements() = 0;
};


struct ImuMeasurementRaw
{
    ImuMeasurementRaw(uint32_t timeOfValidity_us, Vector3D<int16_t> linearAccelerationRaw, Vector3D<int16_t> angularRateRaw)
        : timeOfValidity_us(timeOfValidity_us)
        , linearAccelerationRaw(linearAccelerationRaw)
        , angularRateRaw(angularRateRaw)
    {
        // Do nothing.
    }

    uint32_t timeOfValidity_us;
    Vector3D<int16_t> linearAccelerationRaw;
    Vector3D<int16_t> angularRateRaw;
};


class Bmi08xImu : public Imu
{
public:

    Bmi08xImu() noexcept;

    // Imu interface
    virtual void printMeasurements() override;

    void init();

    bool dataAvailable() {
        return dataAvailable_;
    }

    void readSynchronous()
    {

        const int8_t cmdResult = bmi08a_get_synchronized_data(accelRawData_.c_struct(), gyroRawData_.c_struct(), &device_);


        if (BMI08X_OK == cmdResult)
        {
            dataAvailable_ = true;
        }
    }


    inline ImuMeasurementRaw getMeasurementsRaw()
    {
        return ImuMeasurementRaw(timeOfValidity_, accelRawData_.getRawData(), gyroRawData_.getRawData());
    }

    inline Vector3D<int16_t> accelerationRaw()
    {
        return accelRawData_.getRawData();
    }

    inline Vector3D<int16_t> gyroRaw()
    {
        return gyroRawData_.getRawData();
    }

    int8_t enableDataSynchronizationInterrupt();

    int8_t disableDataSynchronizationInterrupt();


    // Function receiving interrupt
    inline void dataSynchronizationInterruptCallback()
    {
        dataSynchInt_ = true;
    }


private:
    bool active_;
    bool dataAvailable_;
    unsigned char dataSynchInt_;

    struct bmi08x_dev device_;
    struct bmi08x_data_sync_cfg synchronizationConfig_;
    struct bmi08x_int_cfg interruptConfig_;

    uint64_t timeOfValidity_;
    SensorData accelRawData_;
    SensorData gyroRawData_;
};



#endif // BMI08XIMU_H
