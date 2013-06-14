#ifndef SIMPLECHART_H
#define SIMPLECHART_H

#include <wx/list.h>

class wxPanel;
class wxPaintEvent;
class wxSizeEvent;
class wxPaintDC;
class wxRect;
class wxFont;
class wxString;
//class XValueList;
class wxColour;
class ChartValueList;

WX_DECLARE_LIST(double, XValueList);

class ChartValueList: public XValueList
{
private:
    wxString m_title;
    wxColour m_color;
public:
    ChartValueList(const wxString& title, const wxColour &color):
        XValueList(), m_title(title), m_color(color)
    {

    }

    const wxString& GetTitle()
    {
        return m_title;
    }

    const wxColour& GetColour()
    {
        return m_color;
    }
};

WX_DECLARE_LIST(ChartValueList, ChartCollection);

class TimeLogChart : public wxPanel
{
private:
    ChartCollection *m_chartCollection;

    wxString m_title;
    int m_logTickFrequency;
    int m_logDuration; // in seconds

    wxString m_valueAxisFormat;
    double m_valueAxisScale;
    double m_maxValue;
    double m_minValue;
    int m_valueDivisions;
    wxFont *m_valueFont;
    wxFont *m_titleFont;

    wxRect m_axisBounds;
    wxRect m_backgroundBounds;

    void InitDefaults();

    int ValueToPixel(double value);



    void DrawBackground(wxPaintDC &dc);
    void DrawAxis(wxPaintDC &dc);
    void DrawCharts(wxPaintDC &dc);
    void DrawTitle(wxPaintDC &dc);
    void DrawLegends(wxPaintDC &dc);

    void AdjustMaxValueAxis();
public:
    TimeLogChart(wxWindow *parent, int id, const wxString& title);
    virtual ~TimeLogChart();

    void AddChart(const wxString& label, const wxColour &color);
    void AddChartValue(int chartIndex, double value);

    void OnSize(wxSizeEvent& evt);
    void OnPaint(wxPaintEvent& evt);

    void SetValueAxisScale(double scale);
    void SetValueAxisFormat(const wxString& format);
    void SetMaxValue(double value);
    double GetMaxValue();
};

#endif // SIMPLECHART_H
