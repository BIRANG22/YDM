#pragma once
#include "INC_Windows.h"
#include <iostream>
#include <math.h>

namespace simplegeo
{
    class ShapeBase
    {
    public:
        virtual void Draw(HDC hdc) = 0;
        virtual ~ShapeBase() {}
        //virtual bool Contains(int x, int y) { return 0; }
    };

    class Circle : public ShapeBase
    {
    public:
        Circle(int centerX, int centerY, int radius, COLORREF color)
            : m_centerX(centerX), m_centerY(centerY), m_radius(radius), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            // 펜 생성 및 선택
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            // 브러시는 내부 채우지 않도록 NULL_BRUSH 사용
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

            Ellipse(hdc,
                m_centerX - m_radius, m_centerY - m_radius,
                m_centerX + m_radius, m_centerY + m_radius);

            // 이전 객체 복원 및 펜 삭제
            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hPen);
        }
        bool Contains(int x, int y) const
        {
            int dx = x - m_centerX;
            int dy = y - m_centerY;
            int distanceSquared = sqrt(dx * dx + dy * dy);
            return distanceSquared < (2 * m_radius);
        }
    private:
        int m_centerX, m_centerY, m_radius;
        COLORREF m_color;
    };


    class RectangleShape : public ShapeBase
    {
    public:
        RectangleShape(int left, int top, int right, int bottom, COLORREF color)
            : m_left(left), m_top(top), m_right(right), m_bottom(bottom), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

            Rectangle(hdc, m_left, m_top, m_right, m_bottom);

            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hPen);
        }
        bool Contains(int x1, int y1, int x2, int y2) const
        {
            /*int dx = x - (m_right - m_left);
            int dy = y - (m_bottom - m_top);
            int distanceSquared = sqrt(dx * dx + dy * dy);
            return distanceSquared < (2 * m_left);*/
            return (x2 >= m_left && x1 <= m_right &&
                y2 >= m_top && y1 <= m_bottom);
        }
        /*bool IsOverlapping(int x1, int y1, int x2, int y2) const
        {
            return !(x2 < m_left ||
                x1 > m_right ||
                y2 < m_top ||
                y1 > m_bottom);
        }*/
    private:
        int m_left, m_top, m_right, m_bottom;
        COLORREF m_color;
    };


    class Line : public ShapeBase
    {
    public:
        Line(int startX, int startY, int endX, int endY, COLORREF color)
            : m_startX(startX), m_startY(startY), m_endX(endX), m_endY(endY), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, m_startX, m_startY, NULL);
            LineTo(hdc, m_endX, m_endY);

            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    private:
        int m_startX, m_startY, m_endX, m_endY;
        COLORREF m_color;
    };

    constexpr int MAX_SHAPES = 100; // 최대 도형 개수
    class GeoShapeManager
    {
    public:
        GeoShapeManager() = default;
        ~GeoShapeManager()
        {
            for (int i = 0; i < MAX_SHAPES; ++i)
            {
                if (m_shapes[i] != nullptr)
                {
                    delete m_shapes[i];
                    m_shapes[i] = nullptr;
                }
            }
        }

        void AddCircle(int centerX, int centerY, int radius, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            m_shapes[m_shapeCount] = new Circle(centerX, centerY, radius, color);
            ++m_shapeCount;
        }

        void AddRectangle(int left, int top, int right, int bottom, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            m_shapes[m_shapeCount] = new RectangleShape(left, top, right, bottom, color);
            ++m_shapeCount;
        }

        void AddLine(int startX, int startY, int endX, int endY, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            m_shapes[m_shapeCount] = new Line(startX, startY, endX, endY, color);
            ++m_shapeCount;
        }

        void Remove(int x, int y)
        {
            for (int i = 0; i < m_shapeCount; ++i)
            {
                RectangleShape* rect = dynamic_cast<RectangleShape*>(m_shapes[i]);
                Circle* circle = dynamic_cast<Circle*>(m_shapes[i]);
                
                if (circle && circle->Contains(x, y) || rect && rect->Contains(x-10, y-10, x + 10, y + 10))
                {
                    delete m_shapes[i];

                    // 앞으로 당기기
                    for (int j = i; j < m_shapeCount - 1; ++j)
                    {
                        m_shapes[j] = m_shapes[j + 1];
                    }

                    m_shapes[m_shapeCount - 1] = nullptr;
                    --m_shapeCount;

                    --i;
                    //return; // 하나만 삭제
                }
            }
            

            //for (int i = 0; i < m_shapeCount; ++i)
            //{
            //    
            //    
            //        delete m_shapes[i];

            //        // 앞으로 땡기기
            //        for (int j = i; j < m_shapeCount - 1; ++j)
            //            m_shapes[j] = m_shapes[j + 1];

            //        m_shapes[m_shapeCount - 1] = nullptr;
            //        --m_shapeCount;
            //        std::cout << "bb";
            //        return; // 한 개만 제거하고 끝
            //    
            //}
        }

        void Draw(HDC hdc)
        {
            for (int i = 0; i < MAX_SHAPES; ++i)
            {
                if (m_shapes[i] == nullptr) break;

                m_shapes[i]->Draw(hdc);
            }
        }

    private:
        ShapeBase* m_shapes[MAX_SHAPES] = { nullptr, };
        int m_shapeCount = 0;
    };

    extern GeoShapeManager g_GeoShapeManager;
}