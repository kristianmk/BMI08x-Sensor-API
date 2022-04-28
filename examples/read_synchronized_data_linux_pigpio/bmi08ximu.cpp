// Written by K. M. Knausgård 2022-04
// Some parts based on Bosch example "read_synchronized_data_mcu".
#include "bmi08ximu.h"
#include <cstdint>
#include <iostream>

Bmi08xImu::Bmi08xImu() noexcept
    : active_(false)
    , dataAvailable_(false)
    , dataSynchInt_(0U)
    , device_()
    , synchronizationConfig_()
    , interruptConfig_()
    , accelRawData_()
    , gyroRawData_()
    , timeOfValidity_()
{
    // Do nothing
}

void Bmi08xImu::printMeasurements()
{
}

void Bmi08xImu::init()
{
    int8_t cmdResult;

    cmdResult = bmi08x_interface_init(&device_, BMI08X_SPI_INTF, BMI085_VARIANT);

    if (BMI08X_OK != cmdResult)
    {
        std::cout << "Could not initialize interface.." << std::endl;
        return;
    }

    // Init gyros
    cmdResult = bmi08a_init(&device_);

    if (BMI08X_OK != cmdResult)
    {
        std::cout << "Could not initialize gyros.." << std::endl;
        return;
    }

    // Init accelerometers
    cmdResult = bmi08g_init(&device_);

    if (BMI08X_OK != cmdResult)
    {
        std::cout << "Could not initialize gyros.." << std::endl;
        return;
    }

    std::cout << "BMI08x init OK" << std::endl;

    // Configure accelerometers:
    cmdResult = bmi08a_soft_reset(&device_);

    device_.read_write_len = 32;

    std::cout << "Set synchronized feature configuration" << std::endl;
    cmdResult = bmi08a_load_config_file(&device_);

    // Power mode
    device_.accel_cfg.power = BMI08X_ACCEL_PM_ACTIVE;
    cmdResult = bmi08a_set_power_mode(&device_);

    if (cmdResult != BMI08X_OK)
    {
        std::cout << "Could not set accl power mode.." << std::endl;
        return;
    }

    device_.gyro_cfg.power = BMI08X_GYRO_PM_NORMAL;
    cmdResult = bmi08g_set_power_mode(&device_);


    if (cmdResult != BMI08X_OK)
    {
        std::cout << "Could not set gyro power mode.." << std::endl;
        return;
    }


    // Configure range
    switch (device_.variant)
    {
        case BMI085_VARIANT:
            device_.accel_cfg.range = BMI085_ACCEL_RANGE_16G;
            break;
        case BMI088_VARIANT:
            device_.accel_cfg.range = BMI088_ACCEL_RANGE_24G;
            break;
        default:
            std::cout << "Your BMI variant is bad.." << std::endl;
            return;
    }

    device_.gyro_cfg.range = BMI08X_GYRO_RANGE_2000_DPS;

    synchronizationConfig_.mode = BMI08X_ACCEL_DATA_SYNC_MODE_400HZ;


    cmdResult = bmi08a_configure_data_synchronization(synchronizationConfig_, &device_);


    std::cout << "Data synchronization " << (cmdResult == BMI08X_OK ? "enabled successfully" : "command failed") << std::endl;


}

int8_t Bmi08xImu::enableDataSynchronizationInterrupt()
{
    int8_t cmdResult = BMI08X_OK;

    // Accelerometer interrupt configuration
    // Enable channel 1
    interruptConfig_.accel_int_config_1.int_channel = BMI08X_INT_CHANNEL_1;
    interruptConfig_.accel_int_config_1.int_type = BMI08X_ACCEL_SYNC_INPUT;
    interruptConfig_.accel_int_config_1.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;
    interruptConfig_.accel_int_config_1.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.accel_int_config_1.int_pin_cfg.enable_int_pin = BMI08X_ENABLE;

    // Enable channel 2
    interruptConfig_.accel_int_config_2.int_channel = BMI08X_INT_CHANNEL_2;
    interruptConfig_.accel_int_config_2.int_type = BMI08X_ACCEL_INT_SYNC_DATA_RDY;
    interruptConfig_.accel_int_config_2.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;
    interruptConfig_.accel_int_config_2.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.accel_int_config_2.int_pin_cfg.enable_int_pin = BMI08X_ENABLE;

    // Gyro interrupt pin configuration
    // Enable channel 3
    interruptConfig_.gyro_int_config_1.int_channel = BMI08X_INT_CHANNEL_3;
    interruptConfig_.gyro_int_config_1.int_type = BMI08X_GYRO_INT_DATA_RDY;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.enable_int_pin = BMI08X_ENABLE;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;

    // Enable channel 4
    interruptConfig_.gyro_int_config_2.int_channel = BMI08X_INT_CHANNEL_4;
    interruptConfig_.gyro_int_config_2.int_type = BMI08X_GYRO_INT_DATA_RDY;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.enable_int_pin = BMI08X_DISABLE;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;

    // Send command.
    cmdResult = bmi08a_set_data_sync_int_config(&interruptConfig_, &device_);

    return cmdResult;
}


int8_t Bmi08xImu::disableDataSynchronizationInterrupt()
{
    int8_t cmdResult;

    synchronizationConfig_.mode = BMI08X_ACCEL_DATA_SYNC_MODE_OFF;

    cmdResult = bmi08a_configure_data_synchronization(synchronizationConfig_, &device_);
    // Comment in reference implementation says wait for 150 ms, and it is taken care of inside the
    // function, but the function seems to use 150 ms: dev->delay_us(100000, dev->intf_ptr_accel);

    if (cmdResult != BMI08X_OK)
    {
        return cmdResult;
    }


    // Accelerometer interrupt pin configuration
    // Disable channel 1
    interruptConfig_.accel_int_config_1.int_channel = BMI08X_INT_CHANNEL_1;
    interruptConfig_.accel_int_config_1.int_type = BMI08X_ACCEL_SYNC_INPUT;
    interruptConfig_.accel_int_config_1.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;
    interruptConfig_.accel_int_config_1.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.accel_int_config_1.int_pin_cfg.enable_int_pin = BMI08X_DISABLE;

    // Disable channel 2
    interruptConfig_.accel_int_config_2.int_channel = BMI08X_INT_CHANNEL_2;
    interruptConfig_.accel_int_config_2.int_type = BMI08X_ACCEL_INT_SYNC_DATA_RDY;
    interruptConfig_.accel_int_config_2.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;
    interruptConfig_.accel_int_config_2.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.accel_int_config_2.int_pin_cfg.enable_int_pin = BMI08X_DISABLE;

    // Gyro interrupt pin configuration
    // Disable channel 3
    interruptConfig_.gyro_int_config_1.int_channel = BMI08X_INT_CHANNEL_3;
    interruptConfig_.gyro_int_config_1.int_type = BMI08X_GYRO_INT_DATA_RDY;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;
    interruptConfig_.gyro_int_config_1.int_pin_cfg.enable_int_pin = BMI08X_DISABLE;

    // Disable channel 4
    interruptConfig_.gyro_int_config_2.int_channel = BMI08X_INT_CHANNEL_4;
    interruptConfig_.gyro_int_config_2.int_type = BMI08X_GYRO_INT_DATA_RDY;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.enable_int_pin = BMI08X_DISABLE;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.lvl = BMI08X_INT_ACTIVE_HIGH;
    interruptConfig_.gyro_int_config_2.int_pin_cfg.output_mode = BMI08X_INT_MODE_PUSH_PULL;

    // Disable interrupt synch. pin
    cmdResult = bmi08a_set_data_sync_int_config(&interruptConfig_, &device_);


    return cmdResult;

}

