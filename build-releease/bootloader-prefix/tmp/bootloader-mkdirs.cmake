# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/vl0011/sdk/esp-idf/components/bootloader/subproject"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/tmp"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/src/bootloader-stamp"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/src"
  "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/vl0011/CLionProjects/iot-project-device/build-releease/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
