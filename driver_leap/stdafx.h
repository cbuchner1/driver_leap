#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SDKDDKVer.h>
#include <Windows.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <numeric>
#include <atomic>
#include <mutex>
#include <thread>

#include "openvr_driver.h"
#include "LeapC.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/intersect.hpp"

#include "pugixml.hpp"
