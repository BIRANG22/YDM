#pragma once
#include <string>
#include <vector>
struct Rect
{
    int x = 0, y = 0, w = 0, h = 0;
};

struct FrameData
{
    std::string filename;
    Rect frame;
    bool rotated = false;
    bool trimmed = false;
    Rect spriteSourceSize;
    struct { int w = 0, h = 0; } sourceSize;
    int duration = 0;
};

struct Tag
{
    std::string name;
    int from = 0;
    int to = 0;
    std::string direction;
};

struct SliceKey
{
    int frame = 0;
    Rect bounds;
    struct { int x = 0, y = 0; } pivot;
};

struct Slice
{
    std::string name;
    std::vector<SliceKey> keys;
};

struct AsepriteData {
    std::vector<FrameData> frames;
    std::vector<Tag>       frameTags;
    std::vector<Slice>     slices;
};

//struct AnimationClips
//{
//    std::vector<AnimationClip> m_clips;
//};
// ���� �Դϴ�. �� �̷��� ������ �ʿ�� �����ϴ�.
struct Frame {
    D2D1_RECT_U srcRect;
    float       duration = 0.f;     // seconds

    float Width()  const { return srcRect.right - srcRect.left; }
    float Height() const { return srcRect.bottom - srcRect.top; }

    D2D1_RECT_F ToRectF() const {
        return D2D1::RectF(
            static_cast<float>(srcRect.left),
            static_cast<float>(srcRect.top),
            static_cast<float>(srcRect.right),
            static_cast<float>(srcRect.bottom));
    }
};

// AnimationClip: ������ ������ + �ؽ�ó(immutable)
class AnimationClip
{
public:
    AnimationClip() = default;
    ~AnimationClip() = default;

    // �ؽ�ó ��Ʈ ���� (AssetManager���� �� ���� ȣ��)
    void SetBitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> sheet);

    void ReserveFrames(size_t n) { m_frames.reserve(n); }
    // AsepriteParser ��� �������� �� ���� ä�� ����
    void AddFrame(const Frame& frame);

    void SetName(const std::string& n) { m_name = n; }

    // ������ ������ ��ȸ
    const std::string& GetName()         const { return m_name; }
    const std::vector<Frame>& GetFrames()       const;
    float GetTotalDuration()const;
    ID2D1Bitmap1* GetBitmap()       const;
private:
    std::string                              m_name;
    std::vector<Frame>                       m_frames;
    float                                    m_totalDuration = 0.f;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>     m_sheet;
};

struct StaticImage
{
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    D2D1_POINT_2F position = { 0.0f, 0.0f }; // �⺻ ��ġ
};

class SpriteAnimator
{
public:
    SpriteAnimator() = default;

    ~SpriteAnimator() = default;

    bool IsValid() const;

    // ����� Ŭ�� ���� (nullptr ��� �� �÷��� ����)
    void SetClip(const AnimationClip* clip);

    // ���� ��� ���� Ŭ�� ��ȸ
    const AnimationClip* GetClip() const;

    // ���� ���(true) / ��ȸ ���(false)
    void SetLooping(bool loop);

    // deltaTime�� ���ؼ� elapsed ����
    void Update(float deltaTime);

    // ���� ��� ���� ������ �ε��� ��ȸ
    const Frame& GetCurrentFrame() const;

    // ��� ��ġ(��) ��ȸ/����
    float GetElapsed() const;
    void  SetElapsed(float t);

private:
    const AnimationClip* m_clip = nullptr;  // immutable clip ������
    float                m_elapsed = 0.f;      // ��� ��ġ
    bool                 m_loop = true;     // ���� ��� ����
};
