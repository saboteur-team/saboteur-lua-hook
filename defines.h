#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include <functional>
#include <DirectXMath.h>
#include <detours.h>
#include <locale>
#include <codecvt>
#include "inifile.h"
#include <sstream>

using namespace DirectX;

#include "common.h"
#include "IDirect3D9Proxy.h"
#include "IDirect3DDevice9Proxy.h"

