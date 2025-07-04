#include "pch.h"
#include "AnimationClip.h"
#include "AsepriteParser.h"
#include "D2DRender.h"
#include "AssetManager.h"
#include <wincodec.h>
#include <comdef.h>

AssetManager::AssetManager(ID2D1DeviceContext* dc)
    : m_dc(dc)
{
    //CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_wicFactory)
    );

    if (FAILED(hr))
    {
        throw std::runtime_error("WIC Imaging Factory 생성 실패");
    }
}

//path경로의 이미지 파일을 읽어서 비트맵으로 변환
Microsoft::WRL::ComPtr<ID2D1Bitmap1> AssetManager::LoadTexture(const std::filesystem::path& path)
{
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        path.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );
    if (FAILED(hr)) return nullptr;

    //첫번째 프레임 추출
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return nullptr;

    //포멧 변환 생성
    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return nullptr;

    //포맷 변환 초기화
    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr)) return nullptr;

    //비트맵으로 변환
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    hr = m_dc->CreateBitmapFromWicBitmap(//렌더링 대상
        converter.Get(),
        nullptr,
        &bitmap
    );
    if (FAILED(hr)) return nullptr;

    return bitmap;
}

//애니메이션 클립 목록을 key로 조회
const AssetManager::AnimationClips& AssetManager::GetClips(const std::wstring& key) const
{
    static AnimationClips empty;//빈 공간 생성

    auto it = m_clipsMap.find(key);//키 조회

    if (it != m_clipsMap.end())//존재 시 반환
    {
        return it->second;
    }

    return empty;
}

AssetManager::AnimationClips AssetManager::LoadAnimationSet(const std::filesystem::path& jsonPath)
{
    AnimationClips clips = AsepriteParser::Load(jsonPath);
    //.json 파일을 파싱하여 애니메이션 클립 데이터를 생성

    //유효성 검사
    if (clips.empty())
    {
        return{};
    }

    // json 파일과 같은 이름의 png 파일을 찾기 위해 확장자를 .png로 바꿈
    std::filesystem::path pngPath = jsonPath;
    pngPath.replace_extension(L".png");

    //PNG 파일 존재 확인
    if (!std::filesystem::exists(pngPath))
    {
        return{};
    }

    //텍스처 캐시 조회 준비
    auto it = m_textures.find(pngPath.wstring());
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;

    //비트맵 확보
    if (it != m_textures.end())
    {
        bitmap = it->second;//캐시에 있으면 그대로 사용
    }
    else
    {
        bitmap = LoadTexture(pngPath);//없으면 디스크에서 로드
        if (!bitmap)
        {
            return {};
        }

        m_textures[pngPath.wstring()] = bitmap;// 캐시에 넣어 재사용
    }

    //클립-비트맵 연결
    for (auto& [name, clip] : clips)
    {
        clip.SetBitmap(bitmap);//각 클립에 텍스처 붙이기
    }

    //클립 세트를 캐시에 저장
    std::filesystem::path keyPath = jsonPath;
    keyPath.replace_extension();//확장자제거
    m_clipsMap[keyPath.wstring()] = clips;

    return clips;
}
