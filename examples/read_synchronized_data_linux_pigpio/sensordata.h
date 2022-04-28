// Written by K. M. Knausgård 2022-04
#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <cstdint>

#include "../../bmi08x.h"

#include "vector3d.h"

class SensorData
{
public:
    SensorData()
        : vector3D_{} // Zero-initialize aggregate (https://en.cppreference.com/w/cpp/language/value_initialization)
    {

    };

    inline bmi08x_sensor_data* c_struct()
    {
        return &c_struct_;
    }

    inline Vector3D<int16_t> getRawData() const
    {
        return vector3D_;
    }

private:

    union
    {
        Vector3D<int16_t> vector3D_;
        bmi08x_sensor_data c_struct_; // Struct with 3x int16_t.
    };

};


#endif // SENSORDATA_H
