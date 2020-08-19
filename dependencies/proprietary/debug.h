#pragma once
#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"

#include <iostream>

#define Assert(expression) if( !(expression) ) *(int*)0 = 1
#define out(val) std::cout << ' ' << val << '\n'
#define stop std::cin.get()
#define print printf

#define vecout(vec) out("x: " << vec.x << " y: " << vec.y << " z: " << vec.z)

#define ERROR(str) out("ERROR: " << str)