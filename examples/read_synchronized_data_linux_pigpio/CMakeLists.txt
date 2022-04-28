cmake_minimum_required(VERSION 3.5)

project(read_synchronized_data_linux_pigpio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(read_synchronized_data_linux_pigpio main.cpp sensordata.cpp sensordata.h vector3d.h bmi08ximu.cpp bmi08ximu.h ../common/coines.h)


target_include_directories(read_synchronized_data_linux_pigpio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)



