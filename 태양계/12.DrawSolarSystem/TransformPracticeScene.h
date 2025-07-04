#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>

class Object;

class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();

    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents();

    void OnMouseLButtonDown(D2D1_POINT_2F point) override;

    void OnMouseRButtonDown(D2D1_POINT_2F point) override;

    void AddBoxObjects(D2D1_POINT_2F point);

    void ClearBoxObjects();

    void SelectBoxObject(D2D1_POINT_2F point);

    void SetBoxSelfRotation();

    void UpdateRelationship();

    UnityCamera m_UnityCamera;

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    ComPtr<ID2D1Bitmap1> m_SunBitmap;
    ComPtr<ID2D1Bitmap1> m_EarthBitmap;
    ComPtr<ID2D1Bitmap1> m_MoonBitmap;
    ComPtr<ID2D1Bitmap1> m_MercuryBitmap;
    ComPtr<ID2D1Bitmap1> m_VenusBitmap;
    ComPtr<ID2D1Bitmap1> m_MarsBitmap;
    ComPtr<ID2D1Bitmap1> m_JupiterBitmap;
    ComPtr<ID2D1Bitmap1> m_SaturnBitmap;
    ComPtr<ID2D1Bitmap1> m_UranusBitmap;
    ComPtr<ID2D1Bitmap1> m_NeptuneBitmap;

    std::vector<Object*> m_Objects;


    std::list<Object*> m_SelectedObjects; // 선택된 박스 객체들
};

