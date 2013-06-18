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

wxString ServerInfoPanel::AdjustText(const wxString &text, wxPaintDC &dc, int maxExtent)
{
    wxString tmp(text);
    wxSize ext = dc.GetTextExtent(text);
    if (maxExtent > 0)
    {
        while (ext.GetWidth() > maxExtent)
        {
            tmp = tmp.RemoveLast();
            ext = dc.GetTextExtent(tmp);
        }

        if (text.Length() > tmp.Length())
            tmp = tmp.RemoveLast(3) + wxT("...");
    }

    return tmp;
}

void ServerInfoPanel::DrawInfo(wxPaintDC &dc)
{
    int dy = m_bounds.GetTop();
    int dx = m_bounds.GetLeft();

    wxFont infoFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    wxFont headingFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);

    int n = m_serverInfo.GetCount();

    // calculate the longest key extent
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
    int bottomLimit = m_bounds.GetBottom() - infoDy;

    int cx = m_bounds.GetLeft() + longestExtent;
    int maxValueAvailableSpace = m_bounds.GetRight() - cx;

    if (m_selectedGroups.GetCount() == 0)
    {
        dx = m_bounds.GetLeft() + (m_bounds.GetWidth() - dc.GetTextExtent(NO_DATA_SHOWING).GetWidth())/2;
        dy = m_bounds.GetTop() + (m_bounds.GetHeight() - infoDy)/2;
        dc.DrawText(AdjustText(NO_DATA_SHOWING, dc, m_bounds.GetWidth()), dx, dy);

        return;
    }

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
                // ensure visible label
                if (dy < bottomLimit)
                {
                    dc.SetFont(headingFont);
                    dc.DrawText(token, dx, dy);
                    dy += headingDy;
                }
                ++i;
            }
        }
        else
        {
            // ensure visible label
            if (dy < bottomLimit)
            {
                dc.SetFont(infoFont);
                wxArrayString kv = wxStringTokenize(token, ":");
                if (kv.GetCount() == 2)
                {
                    wxString key = kv[0];
                    key.Replace("_", " ");
                    key.MakeCapitalized();
                    dc.DrawText(key, dx, dy);
                    dc.DrawText(AdjustText(kv[1], dc, maxValueAvailableSpace), cx, dy);
                }

                dy += infoDy;
            }
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
