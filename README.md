# Driver Leap [![Release](http://img.shields.io/github/release/SDraw/driver_leap.svg)](../../releases/latest)
Fork with updated vendor libraries.
  
## Installation (for users)
* Install [Orion (v4)](https://developer.leapmotion.com/sdk-leap-motion-controller) or [Gemini (v5)](https://developer.leapmotion.com/gemini-v5-preview)
* Extract [latest release archive](../../releases/latest) to `<SteamVR_folder>/drivers`
* Add line in section `steamvr` of `<Steam_folder>/config/steamvr.vrsettings` file:
```JSON
"activateMultipleDrivers": true,
```

## Usage
### Settings
Driver settings are configurated by editing `resources/settings.xml`. Available settings:
* `emulatedController`: emulated controllers type. Can be `vive`, `index` or `oculus`. `index` by default.
* `rightHand/leftHand`: sets enabling of virtual controller for specific hand. `true` by default.
* `orientation`: Leap Motion controller mouting type. Can be `hmd` or `desktop`. `hmd` by default.
* `trackingLevel`: skeleton tracking style for OpenVR. Can be `partial` or `full`. `partial` by default.
* `desktopOffset`: global position offset from head in `desktop` orientation.
* `leftHandOffset/rightHandOffset`: local offset position for specific hand controller.
* `leftHandOffsetRotation/rightHandOffsetRotation`: local offset rotation for specific hand controller.
* `handsReset`: marks controllers as out of range if hand for controller isn't detected by Leap Motion. `false` by default.
* `interpolation`: enables internal Leap Motion data capture interpolation. `false` by default.
* `useVelocity`: enables velocity data from Leap Motion for hands. `false` by default.

### Gestures
List of hands gestures that are used in tracking:
* **Grab:** bending of middle, ring and pinky fingers.
* **Trigger:** bending of index finger.
* **Thumb press:** touching of middle segment of index finger by thumb.
* **Opisthenar touch:** tounching of opisthenar by index finger of opposite hand.
* **Palm touch:** touching of palm by index finger of opposite hand.
* **Thumb touch:** touching of thumb finger by index finger of opposite hand.
* **Middle touch:** touching of middle finger by index finger of opposite hand.
* **Palm UV:** pointing index finger of opposite hand to palm.

### Input list
#### Vive wand emulation
* **System** -> **Opisthenar touch**
* **Menu** -> **Palm touch**
* **Grip** -> **Grab**
* **Trigger** -> **Trigger**
* **Touchpad touch** -> slight **Thumb press**
* **Touchpad press** -> full **Thumb press**
* **Touchpad XY** -> **Palm UV**

#### Index controller emulation
* **System** -> **Opisthenar touch**
* **Grip** -> **Grab**
* **Trigger** -> **Trigger**
* **A** -> **Palm touch**
* **B** -> **Middle touch**
* **Touchpad touch** -> slight **Thumb press**
* **Touchpad press** -> full **Thumb press**
* **Touchpad XY** -> **Palm UV**
* **Thumbstick** -> **Thumb touch**
* **Thumbstick XY** -> Not implemented due to lack of free gestures

#### Oculus Touch emulation
* **System** -> **Opisthenar touch**
* **Trigger** -> **Trigger**
* **Grip** -> **Grab**
* **X/A** -> **Middle touch**
* **Y/B** -> **Palm touch**
* **Thumbstick touch** -> slight **Thumb press**
* **Thumbstick press** -> full **Thumb press**
* **Thumbstick XY** -> **Palm UV**

## Notes
Currently there is a strange behaviour of tracking problems that affect AMD and few Intel systems. If you're encountering with tracking problems, it's adviced to choose different release with higher `vs####`, or build driver on your system. Refer to **Building** section below.

## Building (for developers)
* Clone repository with `git`:
```
git clone https://github.com/SDraw/driver_leap.git
```
* Initialize submodules:
```
cd driver_leap
git submodule update --init --depth=1
```
* Open `driver_leap.sln` solution in Visual Studio (2013 and up)
* Build your platform:
  * x64 - build output is in `bin/win64`
  * x86 - build output is in `bin/win32`
* Copy build files to `<SteamVR_folder>/drivers/leap/bin/<your_platform>`:
  * `driver_leap.dll`
  * `leap_monitor.exe`  
  **Note:** There are post-build events for projects to copy build files directly to SteamVR driver folder that can be enabled manually.
* Copy additional shared libraries to `<SteamVR_folder>/drivers/leap/bin/<your_platform>`:
  * `vendor/LeapSDK/bin/<your_platform>/LeapC.dll`
  * `vendor/openvr/bin/<your_platform>/openvr_api.dll`
* Copy `resources` folder from solution root to `<SteamVR_folder>/drivers/leap`. 
* Copy `driver.vrdrivermanifest` file from solution root to `<SteamVR_folder>/drivers/leap`.
