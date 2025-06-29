#pragma once

// Windows 기본
#include <windows.h>
#include <wrl/client.h>          // Microsoft::WRL::ComPtr
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

// 표준 라이브러리
#include <memory>
#include <vector>
#include <string>
#include <map>

// 엔진 헤더
#include "Game.h"
#include "Renderer.h"
#include "Window.h"
#include "InputManager.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
