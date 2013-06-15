#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/tokenzr.h>
#include "serverinfopanel.h"

ServerInfoPanel::ServerInfoPanel(wxWindow *parent, int id, const wxString &title):
    wxPanel(parent, id), m_title(title)
{
    SetDoubleBuffered(true);
    Connect(wxEVT_PAINT, wxPaintEventHandler(ServerInfoPanel::OnPaint));
    Connect(wxEVT_SIZE, wxSizeEventHandler(ServerInfoPanel::OnSize));
}

ServerInfoPanel::~ServerInfoPanel()
{
}

void ServerInfoPanel::OnPaint(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    DrawInfo(dc);
}

void ServerInfoPanel::OnSize(wxSizeEvent &evt)
{
    m_bounds = GetClientRect();
    m_bounds.Deflate(wxSize(3, 3));
    Refresh();
}

void ServerInfoPanel::DrawInfo(wxPaintDC &dc)
{
    int dy = m_bounds.GetTop();
    int dx = m_bounds.GetLeft();

    wxFont infoFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));
    wxFont headingFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier 10 Pitch"));

    int n = m_serverInfo.GetCount();

    // calculate the longext key extent
    int longestExtent = 0;
    for (int i = 0; i < n; i++)
    {
        wxArrayString kv = wxStringTokenize(m_serverInfo[i], ":");
        if (kv.GetCount() == 2)
        {
            int w = dc.GetTextExtent(kv[0]).GetWidth();
            if (w > longestExtent)
                longestExtent = w;
        }
    }

    dc.SetFont(headingFont);
    int headingDy = dc.GetTextExtent("H").GetHeight();
    dc.SetFont(infoFont);
    int infoDy = dc.GetTextExtent("H").GetHeight();

    int cx = m_bounds.GetLeft() + longestExtent;

    int i = 0;
    while (i < n )
    {
        wxString token = m_serverInfo[i];
        if (token.StartsWith(wxT("#")))
        {
            token = token.SubString(2, token.Length());

            if (m_selectedGroups.Index(token, false) == -1)
            {
                while (++i < n)
                {
                    if (m_serverInfo[i].StartsWith(wxT("#")))
                    {
                        break;
                    }
                }
            }
            else
            {
                dc.SetFont(headingFont);
                dc.DrawText(token, dx, dy);
                dy += headingDy;
                ++i;
            }
        }
        else
        {
            dc.SetFont(infoFont);
            wxArrayString kv = wxStringTokenize(token, ":");
            if (kv.GetCount() == 2)
            {
                wxString key = kv[0];
                key.Replace("_", " ");
                key.MakeCapitalized();
                dc.DrawText(key, dx, dy);
                dc.DrawText(kv[1], cx, dy);
            }

            dy += infoDy;
            ++i;
        }
    }
}

void ServerInfoPanel::SetSelectedGroups(const wxArrayString &selectedGroups)
{
    m_selectedGroups = selectedGroups;
    Refresh();
}

void ServerInfoPanel::UpdateInfo(const wxArrayString &serverInfo)
{
    m_serverInfo = serverInfo;
    Refresh();
}
