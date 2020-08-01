#pragma once

#include "dlib_windows.h"

#include <string>

dlib_handle_t load_dlib_or_halt(std::string_view lib_name);
dfunc_handle_t load_dfunc_or_halt(dlib_handle_t lib_handle, std::string_view func_name);
void unload_dlib(dlib_handle_t lib_handle);