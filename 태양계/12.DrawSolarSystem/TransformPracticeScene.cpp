#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

class Object
{
    Object() = delete;
    Object(const Object&) = delete;
    void operator=(const Object&) = delete;
public:
    Object(ComPtr<ID2D1Bitmap1>& bitmap)
    {
        m_BitmapPtr = bitmap;

        ++s_Objectid;
        m_name += std::to_wstring(s_Objectid); // ID�� �̸��� �߰�

        m_renderTM = MYTM::MakeRenderMatrix(true);

        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

        //m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
        //m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
    }
    ~Object() = default;

    void Update(float deltaTime)
    {
        if (m_isSelfRotation)
        {
            m_transform.Rotate(deltaTime * 36.f); // �ڱ� ȸ��
        }
    }

    void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
    {
        static  D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        D2D1::ColorF boxColor = D2D1::ColorF::LightGray;

        if (m_isLeader) boxColor = D2D1::ColorF::Red;
        else if (m_isSelected) boxColor = D2D1::ColorF::HotPink;

        testRender.SetTransform(finalTM);
        testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

        D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);

        testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
    }

    void SetPosition(const Vec2& pos)
    {
        m_transform.SetPosition(pos);
    }

    void Move(const Vec2& offset)
    {
        m_transform.Translate(offset);
    }

    void Rotate(float angle)
    {
        m_transform.Rotate(angle);
    }

    void ToggleSelected()
    {
        m_isSelected = !m_isSelected;
    }

    bool IsSelected() const
    {
        return m_isSelected;
    }

    void ToggleSelfRotation()
    {
        m_isSelfRotation = !m_isSelfRotation;
    }

    bool IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM)
    {
        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        finalTM.Invert();

        // 2) ���� ��ǥ�� ����Ʈ ��ȯ
        D2D1_POINT_2F localPt = finalTM.TransformPoint(worldPoint);

        // 3) ���� �簢�� ����
        // (0,0) ~ (width, height) ������ �ִٸ� ��Ʈ!
        // m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

        std::cout << "Object::IsHitTest: localPt = ("
            << localPt.x << ", " << localPt.y << ")" << std::endl;

        std::cout << "Object::IsHitTest: m_rect = ("
            << m_rect.left << ", " << m_rect.top << ", "
            << m_rect.right << ", " << m_rect.bottom << ")" << std::endl;

        // 4) ���� �������� �˻�
        return MYTM::IsPointInRect(localPt, m_rect);
    }

    D2DTM::Transform* GetTransform()
    {
        return &m_transform;
    }

    void SetParent(Object* parent)
    {
        assert(parent != nullptr);

        if (nullptr != m_transform.GetParent())
        {
            // �̹� �θ� �ִٸ� �θ� ���踦 �����մϴ�.
            m_transform.DetachFromParent();
        }

        m_transform.SetParent(parent->GetTransform());
    }

    void DetachFromParent()
    {
        m_transform.DetachFromParent();
    }

    void SetLeader(bool isLeader)
    {
        m_isLeader = isLeader;
    }

private:
    D2DTM::Transform m_transform;

    MAT3X2F m_renderTM; // ������ ��ȯ ���

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false; // ���� �ڽ� ����

    bool m_isSelfRotation = false; // �ڱ� ȸ�� ����

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    static int s_Objectid;
};

int Object::s_Objectid = 0; // static ��� ���� �ʱ�ȭ

TransformPracticeScene::~TransformPracticeScene()
{
    for (auto& box : m_Objects)
    {
        delete box;
    }
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
    constexpr int defaultGameObjectCount = 100;

    m_Objects.reserve(defaultGameObjectCount);

    m_hWnd = hWnd;

    SetWindowText(m_hWnd,
        L"������ �¾�踦 ����� �ּ���. ���� ��Ģ�� ���� �մϴ�. ^^;;");

    std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¿��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/sun.png", *m_BitmapPtr.GetAddressOf());

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/sun.png", *m_SunBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/earth.jpg", *m_EarthBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/moon.png", *m_MoonBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mercury.jpg", *m_MercuryBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Venus.jpg", *m_VenusBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mars.jpg", *m_MarsBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Jupiter.jpg", *m_JupiterBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Saturn.jpg", *m_SaturnBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Uranus.jpg", *m_UranusBitmap.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Neptune.jpg", *m_NeptuneBitmap.GetAddressOf());

    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        D2D1_POINT_2F center = { w / 2, h / 2 };

        m_UnityCamera.SetScreenSize(w, h);

        MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
        cameraTM.Invert();

        D2D1_POINT_2F worldPt = cameraTM.TransformPoint(center);

        Object* pSun = new Object(m_SunBitmap);
        Object* pEarth = new Object(m_EarthBitmap);
        Object* pMoon = new Object(m_MoonBitmap);
        Object* pMercury = new Object(m_MercuryBitmap);
        Object* pVenus = new Object(m_VenusBitmap);
        Object* pMars = new Object(m_MarsBitmap);
        Object* pJupiter = new Object(m_JupiterBitmap);
        Object* pSaturn = new Object(m_SaturnBitmap);
        Object* pUranus = new Object(m_UranusBitmap);
        Object* pNeptune = new Object(m_NeptuneBitmap);

        pSun->GetTransform()->SetPosition(Vec2(worldPt.x, worldPt.y));
        pSun->GetTransform()->SetScale(Vec2(0.8f, 0.8f));

        pMercury->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 1, worldPt.y));
        pMercury->GetTransform()->SetScale(Vec2(0.3f, 0.3f));

        pVenus->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 2, worldPt.y));
        pVenus->GetTransform()->SetScale(Vec2(0.3f, 0.3f));

        pEarth->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 3, worldPt.y));
        pEarth->GetTransform()->SetScale(Vec2(0.3f, 0.3f));

        pMoon->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 3.5f, worldPt.y));
        pMoon->GetTransform()->SetScale(Vec2(0.15f, 0.15f));

        pMars->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 4, worldPt.y));
        pMars->GetTransform()->SetScale(Vec2(0.3f, 0.3f));

        pJupiter->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 5, worldPt.y));
        pJupiter->GetTransform()->SetScale(Vec2(0.45f, 0.45f)); 

        pSaturn->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 6, worldPt.y));
        pSaturn->GetTransform()->SetScale(Vec2(0.42f, 0.42f));

        pUranus->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 7, worldPt.y));
        pUranus->GetTransform()->SetScale(Vec2(0.33f, 0.33f));

        pNeptune->GetTransform()->SetPosition(Vec2(worldPt.x + 60.f * 8, worldPt.y));
        pNeptune->GetTransform()->SetScale(Vec2(0.33f, 0.33f));


        m_Objects.push_back(pSun);
        m_Objects.push_back(pEarth);
        m_Objects.push_back(pMoon);
        m_Objects.push_back(pMercury);
        m_Objects.push_back(pVenus);
        m_Objects.push_back(pMars);
        m_Objects.push_back(pJupiter);
        m_Objects.push_back(pSaturn);
        m_Objects.push_back(pUranus);
        m_Objects.push_back(pNeptune);

        pMoon->SetParent(pEarth);

        pMercury->SetParent(pSun);
        pVenus->SetParent(pSun);
        pEarth->SetParent(pSun);
        pMars->SetParent(pSun);
        pJupiter->SetParent(pSun);
        pSaturn->SetParent(pSun);
        pUranus->SetParent(pSun);
        pNeptune->SetParent(pSun);
        
    }
    //UpdateRelationship();
    //SetBoxSelfRotation();
}

void TransformPracticeScene::Tick(float deltaTime)
{
    for (auto& box : m_Objects)
    {
        box->Update(deltaTime);
    }

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // ī�޶� ��ġ ������ ��Ʈ����

    MAT3X2F finalTM = renderTM * cameraTM;

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // ī�޶� ��ġ ǥ��
    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

    for (auto& box : m_Objects)
    {
        box->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();

    SetBoxSelfRotation();
}

void TransformPracticeScene::OnResize(int width, int height)
{
    m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
    // Ŭ����
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        ClearBoxObjects();
    }

    if (InputManager::Instance().GetKeyPressed(VK_F2))
    {
        SetBoxSelfRotation();
    }


    // ī�޶� �̵� ó��, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17���ʹ� structured binding�� ����Ͽ� �� �����ϰ� ǥ���� �� �ֽ��ϴ�.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    // ù��° ���õ� �ڽ��� �̵�
    static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
      { 'D', {  1.f,  0.f } }, // DŰ�� ������ �̵�
      { 'A', { -1.f,  0.f } }, // AŰ�� ���� �̵�
      { 'W', {  0.f,  1.f } }, // WŰ�� ���� �̵�
      { 'S', {  0.f, -1.f } }, // SŰ�� �Ʒ��� �̵�
    };

    for (auto& [vk, dir] : kBoxMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_SelectedObjects.front()->Move(dir);
        }
    }

    // ù��° ���õ� �ڽ��� ȸ��
    if (InputManager::Instance().GetKeyDown(VK_SPACE) && !m_SelectedObjects.empty())
    {
        m_SelectedObjects.front()->Rotate(1.f); // ���� ������ ȸ��
    }
}

void TransformPracticeScene::OnMouseLButtonDown(D2D1_POINT_2F pos)
{
    AddBoxObjects(pos);
}

void TransformPracticeScene::OnMouseRButtonDown(D2D1_POINT_2F pos)
{
    SelectBoxObject(pos);
}

void TransformPracticeScene::AddBoxObjects(D2D1_POINT_2F point)
{
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    cameraTM.Invert();

    D2D1_POINT_2F worldPt = cameraTM.TransformPoint(point);

    Object* pNewBox = new Object(m_BitmapPtr);

    pNewBox->SetPosition(Vec2(worldPt.x, worldPt.y));

    m_Objects.push_back(pNewBox);
}

void TransformPracticeScene::ClearBoxObjects()
{
    for (auto& box : m_Objects)
    {
        delete box;
    }

    m_Objects.clear();

    m_SelectedObjects.clear();
}

void TransformPracticeScene::SelectBoxObject(D2D1_POINT_2F point)
{
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    std::list<Object*> oldSelectedList = std::move(m_SelectedObjects);

    std::cout << "size of oldSelectedList: " << oldSelectedList.size() << std::endl;


    for (auto& box : m_Objects)
    {
        if (box->IsHitTest(point, cameraTM))
        {
            box->ToggleSelected();

            if (box->IsSelected()) m_SelectedObjects.push_back(box); // ���� ���õ� �ڽ� �߰�
        }
    }

    // ���� ���� ���� �����ϰ� ���õ� �ڽ� ��� �籸��
    for (auto it = oldSelectedList.crbegin(); it != oldSelectedList.crend(); ++it)
    {
        (*it)->DetachFromParent();

        if ((*it)->IsSelected())
        {
            m_SelectedObjects.push_front(*it);
        }
        else
        {
            (*it)->SetLeader(false);
        }

    }

    std::cout << "size of m_SelectedBoxObjects: " << m_SelectedObjects.size() << std::endl;

    // ���� ���� ������Ʈ
    UpdateRelationship();
}


void TransformPracticeScene::SetBoxSelfRotation()
{
    for (auto& box : m_Objects)
    {
        if (false == box->IsSelected())
        {
            box->ToggleSelfRotation();
        }
    }
}


void TransformPracticeScene::UpdateRelationship()
{
    auto it = m_SelectedObjects.begin();

    if (it == m_SelectedObjects.end()) return; // ���õ� �ڽ��� ������ ����

    (*it)->SetLeader(true); // ù��° �ڽ��� ������ ����

    if (m_SelectedObjects.size() < 2) return; // ���õ� �ڽ��� 2�� �̸��̸� ����

    while (it != m_SelectedObjects.end() && std::next(it) != m_SelectedObjects.end())
    {
        Object* parent = *it;
        Object* child = *(std::next(it));

        child->SetParent(parent);

        it++;

        std::cout << "�θ�: " << parent->GetTransform()->GetPosition().x << ", "
            << parent->GetTransform()->GetPosition().y
            << " �ڽ�: " << child->GetTransform()->GetPosition().x << ", "
            << child->GetTransform()->GetPosition().y << std::endl;
    }
}

