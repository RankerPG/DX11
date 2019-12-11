#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <tchar.h>
#include <cassert>

#include <vector>
#include <unordered_map>

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include "Functor.h"
#include "struct.h"

using namespace std;

using namespace DirectX;

using namespace PackedVector;

//
extern HWND g_hWnd;

extern XMMATRIX g_matView;
extern XMMATRIX g_matViewWorld;
extern XMMATRIX g_matProj;