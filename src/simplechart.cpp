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

WX_DEFINE_LIST(XValueList)
WX_DEFINE_LIST(ChartCollection)

TimeLogChart::TimeLogChart(wxWindow *parent, int id, const wxString& title):
    wxPanel(parent, id), m_title(title)
{
    InitDefaults();
    SetDoubleBuffered(true);
    Connect(wxEVT_PAINT, wxPaintEventHandler(TimeLogChart::OnPaint));
    Connect(wxEVT_SIZE, wxSizeEventHandler(TimeLogChart::OnSize));
}

TimeLogChart::~TimeLogChart()
{
    delete m_valueFont;
    delete m_titleFont;
    delete m_chartCollection;
}

void TimeLogChart::AdjustMaxValueAxis()
{
    if (m_chartCollection == NULL && m_chartCollection->GetCount() == 0)
        return;

    double maxValue = 0;
    for (int i = 0; i < m_chartCollection->GetCount(); i++)
    {

        ChartValueList *list =  m_chartCollection->Item(i)->GetData();
        ChartValueList::iterator iter;

        for (iter = list->begin(); iter != list->end(); ++iter)
        {
            double *v = *iter;
            if (*v > maxValue) {
                maxValue = *v;
            }
        }
    }

    if (maxValue > 100)
    {
        m_maxValue = maxValue * 1.5;
    }

    Refresh();
}

void TimeLogChart::InitDefaults()
{
    m_chartCollection = new ChartCollection();
    m_valueFont = new wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));
    m_titleFont = new wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier 10 Pitch"));

    m_logDuration = 60;
    m_logTickFrequency = 10;

    m_minValue = 0.0;
    m_maxValue = 100.0;
    m_valueDivisions = 5;
}

void TimeLogChart::AddChart(const wxString &title, const wxColour &color)
{
    if (m_chartCollection != NULL)
        m_chartCollection->Append(new ChartValueList(title, color));
}

void TimeLogChart::AddChartValue(int chartIndex, double value)
{
    if (m_chartCollection != NULL)
    {
        ChartValueList *list = m_chartCollection->operator [](chartIndex);
        list->push_back(new double(value));
        if (list->GetCount() > m_logDuration) {
            list->pop_front();
        }

        AdjustMaxValueAxis();
    }
}

void TimeLogChart::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    DrawAxis(dc);
    DrawTitle(dc);
    DrawCharts(dc);
    DrawLegends(dc);
}

void TimeLogChart::OnSize(wxSizeEvent &evt)
{
    m_backgroundBounds = GetClientRect();
    m_axisBounds = GetClientRect();
    m_axisBounds.Deflate(wxSize(10, 10));
    m_axisBounds.SetWidth(m_axisBounds.GetWidth() - 80);
    m_axisBounds.SetHeight(m_axisBounds.GetHeight() -50);
    m_axisBounds.Offset(80, 20);

    Refresh();
}

int TimeLogChart::ValueToPixel(double value)
{
    return (int)floor(((value-m_minValue)/(m_maxValue - m_minValue)) * (m_axisBounds.GetHeight()));
}

void TimeLogChart::DrawLegends(wxPaintDC &dc)
{
    const int LEGEND_LENGTH = 20;

    if (m_chartCollection == NULL && m_chartCollection->GetCount() == 0)
        return;

    dc.SetFont(*m_valueFont);
    wxSize ext = dc.GetTextExtent("H");
    int dy = GetClientRect().GetBottom() - ext.GetHeight();
    int dx = m_axisBounds.GetLeft();

    for (int i = 0; i < m_chartCollection->GetCount(); i++)
    {
        ChartValueList *list =  m_chartCollection->Item(i)->GetData();
        dc.SetPen(wxPen(list->GetColour(), 3));
        ext = dc.GetTextExtent(list->GetTitle());
        dc.DrawLine(dx, dy, dx + LEGEND_LENGTH, dy);
        dx += LEGEND_LENGTH + 5;
        dc.DrawText(list->GetTitle(), dx, dy - ext.GetHeight()/2);
        dx += ext.GetWidth() + 20;
    }
}

void TimeLogChart::DrawTitle(wxPaintDC &dc)
{
    wxRect titleRect = GetClientRect();
    titleRect.SetHeight(m_axisBounds.GetTop());
    titleRect.Deflate(m_axisBounds.GetLeft(), 5);
    dc.SetFont(*m_titleFont);

    dc.DrawText(m_title, titleRect.GetTopLeft());
}

void TimeLogChart::DrawAxis(wxPaintDC &dc)
{
    const int MARKER_OFFSET = 5;

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(m_axisBounds);
    dc.SetPen(wxPen(wxColour(*wxBLACK), 1, wxPENSTYLE_DOT));

    int topBound = m_axisBounds.GetTop();
    int leftBound = m_axisBounds.GetLeft();
    int rightBound = m_axisBounds.GetRight();
    int bottomBound = m_axisBounds.GetBottom();
    int dx = leftBound;

    wxFont savedFont = dc.GetFont();
    dc.SetFont(*m_valueFont);
    int n = m_logDuration -1;

    for (int i = 0; i <= n; i++)
    {
        dx = leftBound + (int)floor(i*m_axisBounds.GetWidth() / (double)n);
        if (i % m_logTickFrequency == 0)
        {
            dc.DrawLine(dx, topBound, dx, bottomBound);

            // draw axis marker
            wxString s = wxString::Format(wxT("%d secs"), (m_logDuration-i));
            wxSize ext = dc.GetTextExtent(s);
            dc.DrawText(s, dx - ext.GetWidth()/2, bottomBound + MARKER_OFFSET);
        }
    }

    dc.DrawLine(dx, topBound, dx, bottomBound);

    int yValue = m_maxValue;
    int yValueSteps = (m_minValue-m_maxValue)/m_valueDivisions;
    int ySteps = ValueToPixel((m_maxValue - m_minValue)/m_valueDivisions);
    int dy = topBound;

    for (int n = 0; n < m_valueDivisions; n++)
    {
        dc.DrawLine(leftBound, dy, rightBound, dy);
        wxString s = wxString::Format(wxT("%5.1fM"), (double)yValue/1048576);
        wxSize ext = dc.GetTextExtent(s);
        dc.DrawText(s, m_axisBounds.GetLeft()-ext.GetWidth() - MARKER_OFFSET, dy - ext.GetHeight()/2);

        dy += ySteps;
        yValue += yValueSteps;
    }

    dc.DrawLine(leftBound, bottomBound, rightBound, bottomBound);
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
        int nValues = list->GetCount();
        if (nValues > 1)
        {
            ChartValueList::reverse_iterator iter;
            dc.SetPen(wxPen(list->GetColour()));

            int dx = m_axisBounds.GetRight();
            wxPoint points[nValues];
            int n = 0;

            for (iter = list->rbegin(); iter != list->rend(); ++iter)
            {
                dx = m_axisBounds.GetRight() - (int)floor(n*m_axisBounds.GetWidth() / (double)(m_logDuration-1));
                double *v = *iter;
                int yValue = ValueToPixel(*v);
                points[n] = wxPoint(dx, m_axisBounds.GetBottom()-yValue);
                n++;
    //            wxSafeShowMessage("TEST", wxString::Format("%d", dx));
            }

            dc.DrawSpline(list->GetCount(), points);
        }
    }
}
