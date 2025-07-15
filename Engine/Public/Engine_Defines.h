#pragma once

#pragma warning (disable : 4251)

#pragma warning(push)
#pragma warning (disable : 26495)
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning(pop)

#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <io.h>

using namespace std;

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

using namespace Engine;

#include "fmod.hpp"

using namespace FMOD;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


