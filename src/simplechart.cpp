#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <limits.h>
#include "simplechart.h"

SimpleChart::SimpleChart(wxWindow *parent, int id):
    wxPanel(parent, id)
{
    InitDefaults();
    SetDoubleBuffered(true);
    Connect(wxEVT_PAINT, wxPaintEventHandler(SimpleChart::OnPaint));
    Connect(wxEVT_SIZE, wxSizeEventHandler(SimpleChart::OnSize));
}

void SimpleChart::InitDefaults()
{
    m_xMin = 0;
    m_xMax = 60;
    m_xDivs = 6;
    m_yMin = 0;
    m_yMax = INT_MAX;
    m_yDivs = 6;
}

void SimpleChart::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    wxRect bounds(GetClientRect());

    DrawBackground(dc, bounds);
    DrawAxis(dc, bounds);
}

void SimpleChart::OnSize(wxSizeEvent &evt)
{
    Refresh();
}

void SimpleChart::DrawAxis(wxPaintDC &dc, wxRect bounds)
{
    wxRect axisBounds = bounds;
    axisBounds.Deflate(wxSize(50, 50));
    dc.SetPen(*wxLIGHT_GREY_PEN);

    dc.SetBackground(wxBrush(wxColour(0x666666)));
    dc.DrawRectangle(axisBounds);

    int xSteps = axisBounds.GetWidth()/m_xDivs;
    int dx = axisBounds.GetLeft();

    dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));

    // draw horizontal divisions
    for (int n = 0; n < m_xDivs; n++)
    {
        dc.DrawLine(dx, axisBounds.GetTop(), dx, axisBounds.GetBottom());
        dx += xSteps;
    }

    int ySteps = axisBounds.GetHeight()/m_yDivs;
    int dy = axisBounds.GetTop();

    // draw vertical divisions
    for (int n = 0; n < m_yDivs; n++)
    {
        dc.DrawLine(axisBounds.GetLeft(), dy, axisBounds.GetRight(), dy);
        dy += ySteps;
    }
}

void SimpleChart::DrawBackground(wxPaintDC &dc, wxRect bounds)
{
    dc.SetPen(*wxGREY_PEN);
    dc.SetBackground(wxBrush(wxColour(0xffffff)));
    dc.DrawRectangle(bounds);
}
