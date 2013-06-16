#ifndef SERVERINFOPANEL_H
#define SERVERINFOPANEL_H

#include "redisconnection.h"
#define NO_DATA_SHOWING wxT("No information to display")

class wxPanel;
class wxPaintEvent;
class wxSizeEvent;
class wxPaintDC;
class wxRect;
class wxFont;
class wxString;
class wxEmptyString;
class wxColour;
class wxWindow;

class ServerInfoPanel : public wxPanel
{
private:
    wxString        m_title;
    wxArrayString   m_serverInfo;
    wxArrayString   m_selectedGroups;
    wxRect          m_bounds;

    void DrawInfo(wxPaintDC &dc);
    wxString AdjustText(const wxString &text, wxPaintDC &dc, int maxExtent);
public:
    ServerInfoPanel(wxWindow *parent, int id, const wxString &title=wxEmptyString);
    virtual ~ServerInfoPanel();

    void UpdateInfo(const wxArrayString& serverInfo);
    void SetSelectedGroups(const wxArrayString &selectedGroups);

    void OnSize(wxSizeEvent& evt);
    void OnPaint(wxPaintEvent& evt);
};

#endif // SERVERINFOPANEL_H
