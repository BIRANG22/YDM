#pragma once

#include "NzWndBase.h"
#include "GameTimer.h"
#include "AnimationClip.h"
#include "AssetManager.h"

#include <wrl/client.h>


class GameTimer;

namespace sample
{
    class D2DRenderer;
}

class TestMainApp : public NzWndBase
{
public:
    TestMainApp() = default;
    virtual ~TestMainApp() = default;

    bool Initialize();
    void Run();
    void Finalize();

    bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

private:

    void UpdateTime();
    void UpdateInput();
    void UpdateLogic();

    void Render();
    void RenderImGUI();

    void LoadAssets();

    void OnResize(int width, int height) override;
    void OnClose() override;

    void BrowseForFolder();
    void UpdateFileList();

    std::shared_ptr<sample::D2DRenderer> m_Renderer = nullptr;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_singleImage;

    GameTimer       m_GameTimer;
    std::unique_ptr<AssetManager> m_assetManager;

    std::vector<SpriteAnimator> m_animators; // 여러 애니메이션 관리
    std::vector<StaticImage> m_staticImages; //이미지 관리

    std::string     m_pathInput;
    std::wstring    m_folderPath;
    std::wstring    m_selectedFile;

    std::wstring    m_selectedAssetKey;

    std::vector<SpriteAnimator> m_curSprites;

    std::vector<std::wstring> m_fileList;

    bool m_showFolderPanel = false;
    bool m_bChangedFile = false;
};
