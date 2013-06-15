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
    Refresh();
}

void ServerInfoPanel::DrawInfo(wxPaintDC &dc)
{
    int dy = m_bounds.GetTop();
    int dx = m_bounds.GetLeft();
    wxSize ext = dc.GetTextExtent("H");
    wxFont infoFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));

    dc.SetFont(infoFont);

    for (int i = 0; i < m_serverInfo.Count(); i++)
    {
        wxString token = m_serverInfo[i];
        if (token.StartsWith("#"))
            continue;

        wxArrayString kv = wxStringTokenize(token, ":");
        token.Replace("_", " ");
        token.MakeCapitalized();
        dc.DrawText(kv[0], dx, dy);

        dy += ext.GetHeight();

//        for (int n = 0; n < N_SERVER_ATTRIBUTES; n++)
//        {
//            if (kv[0] == SERVER_ATTRIBUTES[n])
//            {
//                token.Replace("_", " ");
//                token.MakeCapitalized();

//            }
//        }
    }
}

void ServerInfoPanel::UpdateInfo(const wxArrayString &serverInfo)
{
    m_serverInfo = serverInfo;
    Refresh();
}
