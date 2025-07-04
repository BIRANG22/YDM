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
// 예시 입니다. 꼭 이렇게 구현할 필요는 없습니다.
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

// AnimationClip: 프레임 데이터 + 텍스처(immutable)
class AnimationClip
{
public:
    AnimationClip() = default;
    ~AnimationClip() = default;

    // 텍스처 시트 연결 (AssetManager에서 한 번만 호출)
    void SetBitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> sheet);

    void ReserveFrames(size_t n) { m_frames.reserve(n); }
    // AsepriteParser 등에서 프레임을 한 번만 채워 넣음
    void AddFrame(const Frame& frame);

    void SetName(const std::string& n) { m_name = n; }

    // 프레임 데이터 조회
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
    D2D1_POINT_2F position = { 0.0f, 0.0f }; // 기본 위치
};

class SpriteAnimator
{
public:
    SpriteAnimator() = default;

    ~SpriteAnimator() = default;

    bool IsValid() const;

    // 재생할 클립 설정 (nullptr 허용 → 플레이 중지)
    void SetClip(const AnimationClip* clip);

    // 현재 재생 중인 클립 조회
    const AnimationClip* GetClip() const;

    // 루프 재생(true) / 단회 재생(false)
    void SetLooping(bool loop);

    // deltaTime을 더해서 elapsed 갱신
    void Update(float deltaTime);

    // 현재 재생 중인 프레임 인덱스 조회
    const Frame& GetCurrentFrame() const;

    // 재생 위치(초) 조회/설정
    float GetElapsed() const;
    void  SetElapsed(float t);

private:
    const AnimationClip* m_clip = nullptr;  // immutable clip 데이터
    float                m_elapsed = 0.f;      // 재생 위치
    bool                 m_loop = true;     // 루프 재생 여부
};
