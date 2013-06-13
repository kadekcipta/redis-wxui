#ifndef SIMPLECHART_H
#define SIMPLECHART_H

#include <wx/list.h>

class wxPanel;
class wxPaintEvent;
class wxSizeEvent;
class wxPaintDC;
class wxRect;
class wxFont;
class ChartValueList;

WX_DECLARE_LIST(double, ChartValueList);
WX_DECLARE_LIST(ChartValueList, ChartCollection);

class TimeLogChart : public wxPanel
{
private:
    ChartCollection *m_chartCollection;

    double m_xMaxValue;
    double m_xMinValue;
    int m_xDivs;

    double m_yMaxValue;
    double m_yMinValue;
    int m_yDivs;
    wxFont *m_valueFont;

    wxRect m_axisBounds;
    wxRect m_backgroundBounds;

    void InitDefaults();

    int XValueToPixel(double value);
    int YValueToPixel(double value);

    void AddChart(const wxString& label);
    void AddChartValue(int chartIndex, double value);

    void DrawBackground(wxPaintDC &dc);
    void DrawAxis(wxPaintDC &dc);
    void DrawCharts(wxPaintDC &dc);
public:
    TimeLogChart(wxWindow *parent, int id);
    virtual ~TimeLogChart();

    void OnSize(wxSizeEvent& evt);
    void OnPaint(wxPaintEvent& evt);
};

#endif // SIMPLECHART_H
