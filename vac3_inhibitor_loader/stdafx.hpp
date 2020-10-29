#pragma once

#define WIN32_LEAN_AND_MEAN

// includes
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <fstream>

#include <shared_mutex>
#include <filesystem>
#include <sstream>

#include <Shlwapi.h>
#include <shellapi.h>

#pragma comment( lib, "Shell32.lib" )
#pragma comment( lib, "Shlwapi.lib" )

// global defs
#include "log.hpp"
#include "util.hpp"
#include "memory.hpp"