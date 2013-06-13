#ifndef SIMPLECHART_H
#define SIMPLECHART_H

class wxPanel;
class wxPaintEvent;
class wxSizeEvent;
class wxPaintDC;

class SimpleChart : public wxPanel
{
private:
    int m_xMax;
    int m_xMin;
    int m_xDivs;

    long m_yMax;
    long m_yMin;
    int m_yDivs;

    void InitDefaults();

    void DrawBackground(wxPaintDC &dc, wxRect bounds);
    void DrawAxis(wxPaintDC &dc, wxRect bounds);
public:
    SimpleChart(wxWindow *parent, int id);

    void OnSize(wxSizeEvent& evt);
    void OnPaint(wxPaintEvent& evt);
};

#endif // SIMPLECHART_H
