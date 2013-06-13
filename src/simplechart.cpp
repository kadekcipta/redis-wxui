#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <limits.h>
#include <math.h>
#include "simplechart.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(ChartValueList)
WX_DEFINE_LIST(ChartCollection)

TimeLogChart::TimeLogChart(wxWindow *parent, int id):
    wxPanel(parent, id)
{
    InitDefaults();
    SetDoubleBuffered(true);
    Connect(wxEVT_PAINT, wxPaintEventHandler(TimeLogChart::OnPaint));
    Connect(wxEVT_SIZE, wxSizeEventHandler(TimeLogChart::OnSize));
}

TimeLogChart::~TimeLogChart()
{
    delete m_valueFont;
    delete m_chartCollection;
}

void TimeLogChart::InitDefaults()
{
    m_chartCollection = new ChartCollection();
    m_valueFont = new wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));

    m_xMinValue = 1.0;
    m_xMaxValue = 60.0;
    m_xDivs = 6;

    m_yMinValue = 0.0;
    m_yMaxValue = 100.0;
//    m_yMaxValue = (double)INT_MAX;
    m_yDivs = 6;

    AddChart(wxEmptyString);

    AddChartValue(0, 1.0);
    AddChartValue(0, 1.0);
    AddChartValue(0, 12.0);
    AddChartValue(0, 12.0);
    AddChartValue(0, 20.0);
    AddChartValue(0, 1.0);
    AddChartValue(0, 12.0);
    AddChartValue(0, 12.0);
    AddChartValue(0, 20.0);
    AddChartValue(0, 20.0);
//    AddChartValue(0, 20.0);
//    AddChartValue(0, 50.0);

//    AddChart(wxEmptyString);

//    AddChartValue(1, 1.0);
//    AddChartValue(1, 12.0);
//    AddChartValue(1, 12.0);
//    AddChartValue(1, 20.0);
//    AddChartValue(1, 1.0);
//    AddChartValue(1, 12.0);
//    AddChartValue(1, 12.0);
//    AddChartValue(1, 20.0);
//    AddChartValue(1, 20.0);
//    AddChartValue(1, 20.0);
}

void TimeLogChart::AddChart(const wxString &label)
{
    m_chartCollection->Append(new ChartValueList());
}

void TimeLogChart::AddChartValue(int chartIndex, double value)
{
    ChartValueList *list = m_chartCollection->operator [](chartIndex);
    list->push_back(new double(value));
}

void TimeLogChart::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    DrawBackground(dc);
    DrawAxis(dc);
    DrawCharts(dc);
}

void TimeLogChart::OnSize(wxSizeEvent &evt)
{
    m_backgroundBounds = GetClientRect();
    m_axisBounds = GetClientRect();
    m_axisBounds.Deflate(wxSize(100, 50));

    Refresh();
}

int TimeLogChart::XValueToPixel(double value)
{
    return (int)round(((value-m_xMinValue)/(m_xMaxValue - m_xMinValue)) * (m_axisBounds.GetWidth()));
}

int TimeLogChart::YValueToPixel(double value)
{
    return (int)round(((value-m_yMinValue)/(m_yMaxValue - m_yMinValue)) * (m_axisBounds.GetHeight()));
}

void TimeLogChart::DrawAxis(wxPaintDC &dc)
{
    const int MARKER_OFFSET = 5;

    dc.SetBackground(wxBrush(wxColour(0x666666)));
    dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_DOT));

    int rightBound = m_axisBounds.GetLeft() + XValueToPixel(m_xMaxValue-m_xMinValue);
    int bottomBound = m_axisBounds.GetTop() + YValueToPixel(m_yMaxValue-m_yMinValue);
    int xSteps = XValueToPixel((m_xMaxValue - m_xMinValue)/m_xDivs);
    int dx = m_axisBounds.GetLeft();
    int xValue = m_xMinValue;
    int xValueSteps = (m_xMaxValue-m_xMinValue)/m_xDivs;

    wxFont savedFont = dc.GetFont();
    dc.SetFont(*m_valueFont);

    // draw horizontal divisions from right to left
    for (int n = 0; n <= m_xDivs; n++)
    {
        dc.DrawLine(dx, m_axisBounds.GetTop(), dx, bottomBound);
        // draw axis marker
        wxString s = wxString::Format(wxT("%d"), xValue);
        wxSize ext = dc.GetTextExtent(s);
        dc.DrawText(s, dx - ext.GetWidth()/2, bottomBound + MARKER_OFFSET);
        dx += xSteps;
        xValue += xValueSteps;
    }

    int yValue = m_yMaxValue;
    int yValueSteps = (m_yMinValue-m_yMaxValue)/m_yDivs;

    int ySteps = YValueToPixel((m_yMaxValue - m_yMinValue)/m_yDivs);
    int dy = m_axisBounds.GetTop();

    // draw vertical divisions
    for (int n = 0; n <= m_yDivs; n++)
    {
        dc.DrawLine(m_axisBounds.GetLeft(), dy, rightBound, dy);
        wxString s = wxString::Format(wxT("%5.1f"), (double)yValue);
        wxSize ext = dc.GetTextExtent(s);

        dc.DrawText(s, m_axisBounds.GetLeft()-ext.GetWidth() - MARKER_OFFSET, dy - ext.GetHeight()/2);

        dy += ySteps;
        yValue += yValueSteps;
    }

    dc.SetFont(savedFont);
}

void TimeLogChart::DrawBackground(wxPaintDC &dc)
{
    dc.SetPen(*wxGREY_PEN);
    dc.SetBackground(wxBrush(wxColour(0xffffff)));
    dc.DrawRectangle(m_backgroundBounds);
}

void TimeLogChart::DrawCharts(wxPaintDC &dc)
{
    if (m_chartCollection == NULL && m_chartCollection->GetCount() == 0)
        return;

    for (int i = 0; i < m_chartCollection->GetCount(); i++)
    {
        ChartValueList *list =  m_chartCollection->Item(i)->GetData();
        ChartValueList::reverse_iterator iter;

        int dx = m_axisBounds.GetLeft();
        int xSteps = m_axisBounds.GetWidth()/(m_xMaxValue-m_xMinValue);
//        wxSafeShowMessage("TEST", wxString::Format("%d", xSteps));

        wxPoint points[list->GetCount()];
        int n = 0;

        dc.SetPen(*wxGREEN_PEN);

        for (iter = list->rbegin(); iter != list->rend(); ++iter)
        {
            double *v = *iter;
            int yValue = YValueToPixel(*v);
            points[n] = wxPoint(dx, m_axisBounds.GetBottom()-yValue);
            dx += xSteps;
            n++;

//            wxSafeShowMessage("TEST", wxString::Format("%f", *v));
        }

        dc.DrawSpline(list->GetCount(), points);
    }
}
