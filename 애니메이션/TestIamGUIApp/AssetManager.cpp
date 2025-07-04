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
        throw std::runtime_error("WIC Imaging Factory ���� ����");
    }
}

//path����� �̹��� ������ �о ��Ʈ������ ��ȯ
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

    //ù��° ������ ����
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return nullptr;

    //���� ��ȯ ����
    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return nullptr;

    //���� ��ȯ �ʱ�ȭ
    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr)) return nullptr;

    //��Ʈ������ ��ȯ
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    hr = m_dc->CreateBitmapFromWicBitmap(//������ ���
        converter.Get(),
        nullptr,
        &bitmap
    );
    if (FAILED(hr)) return nullptr;

    return bitmap;
}

//�ִϸ��̼� Ŭ�� ����� key�� ��ȸ
const AssetManager::AnimationClips& AssetManager::GetClips(const std::wstring& key) const
{
    static AnimationClips empty;//�� ���� ����

    auto it = m_clipsMap.find(key);//Ű ��ȸ

    if (it != m_clipsMap.end())//���� �� ��ȯ
    {
        return it->second;
    }

    return empty;
}

AssetManager::AnimationClips AssetManager::LoadAnimationSet(const std::filesystem::path& jsonPath)
{
    AnimationClips clips = AsepriteParser::Load(jsonPath);
    //.json ������ �Ľ��Ͽ� �ִϸ��̼� Ŭ�� �����͸� ����

    //��ȿ�� �˻�
    if (clips.empty())
    {
        return{};
    }

    // json ���ϰ� ���� �̸��� png ������ ã�� ���� Ȯ���ڸ� .png�� �ٲ�
    std::filesystem::path pngPath = jsonPath;
    pngPath.replace_extension(L".png");

    //PNG ���� ���� Ȯ��
    if (!std::filesystem::exists(pngPath))
    {
        return{};
    }

    //�ؽ�ó ĳ�� ��ȸ �غ�
    auto it = m_textures.find(pngPath.wstring());
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;

    //��Ʈ�� Ȯ��
    if (it != m_textures.end())
    {
        bitmap = it->second;//ĳ�ÿ� ������ �״�� ���
    }
    else
    {
        bitmap = LoadTexture(pngPath);//������ ��ũ���� �ε�
        if (!bitmap)
        {
            return {};
        }

        m_textures[pngPath.wstring()] = bitmap;// ĳ�ÿ� �־� ����
    }

    //Ŭ��-��Ʈ�� ����
    for (auto& [name, clip] : clips)
    {
        clip.SetBitmap(bitmap);//�� Ŭ���� �ؽ�ó ���̱�
    }

    //Ŭ�� ��Ʈ�� ĳ�ÿ� ����
    std::filesystem::path keyPath = jsonPath;
    keyPath.replace_extension();//Ȯ��������
    m_clipsMap[keyPath.wstring()] = clips;

    return clips;
}
