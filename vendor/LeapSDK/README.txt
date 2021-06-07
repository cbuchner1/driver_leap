Leap Motion SDK
================================================================================

Package contents:

  LeapSDK
    |- 3rdparty      -- External dependencies used by some of the samples.
    |- docs          -- API reference documentation & guidelines.
    |- include       -- API headers.
    |- lib           -- API libraries, both dynamic and static.
    |- samples       -- Various samples demonstrating several different usages.

Requirements:
    1. Running requires
      - Leap Motion Software (https://developer.leapmotion.com/get-started/)
    2. Building Samples requires
      - CMake 3.10+ (https://cmake.org/)
      - Microsoft Visual Studio 15+

Installation:

    1. Copy the LeapSDK folder to a suitable location on your computer.

Usage:
  1. For CMake projects
    - Ensure LeapSDK is in a directory considered as a prefix by find_package.
        (https://cmake.org/cmake/help/v3.10/command/find_package.html)
      - or -
    - directly set LeapSDK_DIR to <install_dir>/LeapSDK/lib/cmake/LeapSDK
      - or -
    - Pass the LeapSDK's path to find_package with the PATHS option.
    - call find_package(LeapSDK 4.0 [PATHS ...]).
    - call target_link_libraries(<your project> PUBLIC|PRIVATE LeapSDK::LeapC).
    - Ensure LeapC.dll is in your dynamic library search path.
      - A popular option is to add a post-build step that copies it to your
        project's output directory.
  2. For non-CMake projects
    - Use a C/C++ compiler such as MSVC, Clang or GCC.
    - Add LeapSDK/include to the compiler include search paths.
    - Either add a linker reference to LeapC.lib or dynamically
      load LeapC.dll.

Building Samples:
  1. Open CMake using LeapSDK/samples as the source directory
  2. Select a build directory (often LeapSDK/samples/build) to use
  3. Configure & Generate CMake with the generator of your choice
  4. Open and build the CMake generated project files. For more help, see
    the CMake documentation.

Resources:

  1. The Leap Motion Developer Portal (https://developer.leapmotion.com)
     provides examples, community forums, Leap Motion news, and documentation
     to help you to learn how to develop applications using the Leap Motion
     SDK.
  2. C# and Unity bindings (https://github.com/leapmotion/UnityModules)
  3. C++ bindings matching the old API (https://github.com/leapmotion/LeapCxx)

--------------------------------------------------------------------------------
Copyright © 2012-2018 Leap Motion, Inc. All rights reserved.

Use subject to the terms of the Leap Motion SDK Agreement available at
https://developer.leapmotion.com/sdk_agreement.
