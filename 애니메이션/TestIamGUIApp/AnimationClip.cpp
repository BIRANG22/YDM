#include "pch.h"
#include "AnimationClip.h"

// ---------------------- AnimationClip --------------------------


void AnimationClip::SetBitmap(Microsoft::WRL::ComPtr<ID2D1Bitmap1> sheet)
{
    m_sheet = std::move(sheet);
}

void AnimationClip::AddFrame(const Frame& frame)
{
    m_frames.push_back(frame);
    m_totalDuration += frame.duration;
}

const std::vector<Frame>& AnimationClip::GetFrames() const
{
    return m_frames;
}

float AnimationClip::GetTotalDuration() const
{
    return m_totalDuration;
}

ID2D1Bitmap1* AnimationClip::GetBitmap() const
{
    return m_sheet.Get();
}

// ---------------------- SpriteAnimator --------------------------

bool SpriteAnimator::IsValid() const
{
    return m_clip != nullptr;
}

void SpriteAnimator::SetClip(const AnimationClip* clip)
{
    m_clip = clip;
    m_elapsed = 0.f;
    std::cout << "[DEBUG] 애니메이션 클립 설정됨: " << clip->GetName() << std::endl;
    std::cout << "[DEBUG] 클립 프레임 수: " << clip->GetFrames().size() << std::endl;
}

const AnimationClip* SpriteAnimator::GetClip() const
{
    return m_clip;
}

void SpriteAnimator::SetLooping(bool loop)
{
    m_loop = loop;
}

void SpriteAnimator::Update(float deltaTime)
{
    if (!m_clip) { std::cout << "[ERROR] m_clip == nullptr (애니메이션 없음)" << std::endl; return; }
    else if (m_clip->GetFrames().empty()) std::cout << "[ERROR] 프레임 없음" << std::endl;
    else std::cout << "[DEBUG] 프레임 업데이트됨, 현재 인덱스: "  << std::endl;

    m_elapsed += deltaTime;
    float total = m_clip->GetTotalDuration();

    if (m_loop)
    {
        if (m_elapsed >= total)
            m_elapsed = std::fmod(m_elapsed, total);
    }
    else
    {
        if (m_elapsed > total)
            m_elapsed = total;
    }
}

const Frame& SpriteAnimator::GetCurrentFrame() const
{
    static Frame dummy{ {0,0,0,0}, 0.f };

    if (!m_clip || m_clip->GetFrames().empty())
        return dummy;

    float accum = 0.f;
    for (const auto& frame : m_clip->GetFrames())
    {
        accum += frame.duration;
        if (m_elapsed < accum)
            return frame;
    }

    return m_clip->GetFrames().back();
}

float SpriteAnimator::GetElapsed() const
{
    return m_elapsed;
}

void SpriteAnimator::SetElapsed(float t)
{
    m_elapsed = t;
}

