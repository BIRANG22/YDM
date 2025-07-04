#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"

// 예시입니다.
class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
    //AssetManager() = default;
    AssetManager(ID2D1DeviceContext* dc);
    ~AssetManager() = default;

    AnimationClips LoadAnimationSet(const std::filesystem::path& jsonPath);

    const AnimationClips& GetClips(const std::wstring& key) const;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> LoadTexture(const std::filesystem::path& path);
private:
    ID2D1DeviceContext* m_dc = nullptr;
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips> m_clipsMap;
};

