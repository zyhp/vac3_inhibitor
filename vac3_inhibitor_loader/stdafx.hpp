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

#include "Shlwapi.h"
#pragma comment( lib, "Shlwapi.lib" )

#include "shellapi.h"
#pragma comment( lib, "Shell32.lib" )

// global defs
#include "log.hpp"
#include "util.hpp"
#include "memory.hpp"

// path to blackbone x86 lib
#include "D:/Lib/bbx86/BlackBone/BlackBone/PE/PEImage.h"
#include "D:/Lib/bbx86/BlackBone/BlackBone/Process/Process.h"
#include "D:/Lib/bbx86/BlackBone/BlackBone/Misc/DynImport.h"

#ifdef _DEBUG
#pragma comment ( lib, "D:/Lib/bbx86/BlackBone/Lib/Debug/BlackBone.lib" )
#else
#pragma comment ( lib, "D:/Lib/bbx86/BlackBone/Lib/Release/BlackBone.lib" )
#endif