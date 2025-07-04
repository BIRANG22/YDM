#include "pch.h"
#include <fstream>
#include "json.hpp"
#include "AsepriteParser.h"

static bool LoadAsepriteJson(const std::string& filePath, AsepriteData& outData)
{
    std::ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        std::cerr << "파일을 열 수 없습니다: " << filePath << '\n';
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    auto parsed = nlohmann::json::parse(content, nullptr, false);
    if (parsed.is_discarded())
    {
        std::cerr << "JSON 파싱 실패\n";
        return false;
    }

    const auto& root = parsed;
    auto& framesNode = root["frames"];

    if (framesNode.is_object())
    {
        for (auto it = framesNode.begin(); it != framesNode.end(); ++it)
        {
            const auto& f = it.value();
            FrameData fd;
            fd.filename = it.key();
            fd.frame.x = f["frame"]["x"];
            fd.frame.y = f["frame"]["y"];
            fd.frame.w = f["frame"]["w"];
            fd.frame.h = f["frame"]["h"];
            fd.rotated = f["rotated"];
            fd.trimmed = f["trimmed"];
            fd.spriteSourceSize.x = f["spriteSourceSize"]["x"];
            fd.spriteSourceSize.y = f["spriteSourceSize"]["y"];
            fd.spriteSourceSize.w = f["spriteSourceSize"]["w"];
            fd.spriteSourceSize.h = f["spriteSourceSize"]["h"];
            fd.sourceSize.w = f["sourceSize"]["w"];
            fd.sourceSize.h = f["sourceSize"]["h"];
            fd.duration = f["duration"];
            outData.frames.push_back(fd);
        }
    }
    else if (framesNode.is_array())
    {
        for (const auto& f : framesNode)
        {
            FrameData fd;
            fd.filename = f["filename"];
            fd.frame.x = f["frame"]["x"];
            fd.frame.y = f["frame"]["y"];
            fd.frame.w = f["frame"]["w"];
            fd.frame.h = f["frame"]["h"];
            fd.rotated = f["rotated"];
            fd.trimmed = f["trimmed"];
            fd.spriteSourceSize.x = f["spriteSourceSize"]["x"];
            fd.spriteSourceSize.y = f["spriteSourceSize"]["y"];
            fd.spriteSourceSize.w = f["spriteSourceSize"]["w"];
            fd.spriteSourceSize.h = f["spriteSourceSize"]["h"];
            fd.sourceSize.w = f["sourceSize"]["w"];
            fd.sourceSize.h = f["sourceSize"]["h"];
            fd.duration = f["duration"];
            outData.frames.push_back(fd);
        }
    }
    else
    {
        std::cerr << "'frames' 형식이 올바르지 않습니다.\n";
        return false;
    }

    if (root["meta"].contains("frameTags"))
    {
        for (const auto& t : root["meta"]["frameTags"])
        {
            Tag tag;
            tag.name = t["name"];
            tag.from = t["from"];
            tag.to = t["to"];
            tag.direction = t["direction"];
            outData.frameTags.push_back(tag);
        }
    }

    if (root["meta"].contains("slices"))
    {
        for (const auto& s : root["meta"]["slices"])
        {
            Slice slice;
            slice.name = s["name"];
            for (const auto& key : s["keys"])
            {
                SliceKey sk;
                sk.frame = key["frame"];
                sk.bounds.x = key["bounds"]["x"];
                sk.bounds.y = key["bounds"]["y"];
                sk.bounds.w = key["bounds"]["w"];
                sk.bounds.h = key["bounds"]["h"];
                sk.pivot.x = key["pivot"]["x"];
                sk.pivot.y = key["pivot"]["y"];
                slice.keys.push_back(sk);
            }
            outData.slices.push_back(slice);
        }
    }

    return true;
}
AnimationClips AsepriteParser::Load(const std::filesystem::path& jsonPath)
{
    // 1) JSON 로드
    AsepriteData data;
    if (!LoadAsepriteJson(jsonPath.string(), data))
    {
        std::cerr << "Failed to load Aseprite JSON: " << jsonPath << '\n';
        return {};
    }

    // 2) 태그별로 AnimationClip 생성
    AnimationClips clips;

    for (const auto& tag : data.frameTags)
    {
        AnimationClip clip;
        clip.SetName(tag.name);
        clip.ReserveFrames(tag.to - tag.from + 1);

        for (int i = tag.from; i <= tag.to; ++i)
        {
            if (i < 0 || i >= static_cast<int>(data.frames.size()))
                continue;

            const FrameData& src = data.frames[i];

            // frame.m_frame: Rect {x, y, w, h}
            D2D1_RECT_U rect = {
                static_cast<UINT32>(src.frame.x),
                static_cast<UINT32>(src.frame.y),
                static_cast<UINT32>(src.frame.x + src.frame.w),
                static_cast<UINT32>(src.frame.y + src.frame.h)
            };

            Frame frame;
            frame.srcRect = rect;
            frame.duration = src.duration / 1000.0f; // ms → s

            clip.AddFrame(frame);
        }

        clips.emplace_back(tag.name, std::move(clip));
    }

    return clips;
}
