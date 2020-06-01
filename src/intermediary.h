/*
* Copyright Mohamed Hamed. All rights reserved.
*
* NAME        : City-Building Game
* AUTHOR      : Mohamed Hamed
* START DATE  : 31 May 2020
* VERSION DATE: 31 May 2020
*/

#pragma once

#pragma comment(lib, "winmm")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "external/GLEW/glew32s")
#pragma comment(lib, "external/GLFW/glfw3")
#pragma comment(lib, "external/OpenAL/OpenAL32.lib")

#undef UNICODE
#pragma comment (lib, "Ws2_32.lib")

#define force_inline __forceinline // may only work for MSVC

#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"
#include <winsock2.h>// for networking
#include <ws2tcpip.h>
#include <Windows.h> //For Timing & Memory
#include <iostream> // for files and debugging
#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include "external/GLEW\glew.h" // for OpenGL functions
#include <external/GLFW\glfw3.h>// for window & input
#define GLM_FORCE_RADIAN
#define GLM_ENABLE_EXPERIMENTAL
#include <external/GLM/glm.hpp> //for math
#include <external/GLM/gtc/matrix_transform.hpp>
#include <external/GLM/gtc/type_ptr.hpp>
#include <external/GLM/gtc/quaternion.hpp> //for quaternions
#include <external/GLM/gtx/quaternion.hpp>
#include <external/GLM/gtc/noise.hpp> // for terrain generation
#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h> // for image loading
#include <external/OpenAL/al.h> // for audio
#include <external/OpenAL/alc.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_opengl3.h"
//#include "../imgui/imgui_impl_glfw.h"

#include "proprietary/types.h"
#include "proprietary/timer.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;
using glm::ivec2;
using glm::ivec3;
using glm::uvec2;

#define NULL 0

#define PI	  3.14159265359f
#define TWOPI 6.28318530718f

#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

#define vecmagnitude(vec) sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z))

#define Alloc(type, size) (type*)VirtualAlloc(0, (size) * sizeof(type), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
#define Free(ptr) VirtualFree(ptr, 0, MEM_RELEASE)

#define global static

#define Assert(expression) if( !(expression) ) *(int*)0 = 1;
#define out(val) std::cout << ' ' << val << '\n'
#define vecout(vec) std::cout << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z << '\n'
#define stop std::cin.get();
#define print printf