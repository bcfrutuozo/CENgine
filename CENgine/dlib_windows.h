#pragma once

#include <windows.h>

typedef HMODULE dlib_handle_t;
typedef FARPROC dfunc_handle_t;

constexpr auto LIBS_PATH_ENV_VAR{"PATH"};