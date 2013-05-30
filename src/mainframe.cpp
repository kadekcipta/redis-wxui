#include "mainframe.h"
#include "serverpanel.h"
#include "querypanel.h"

MainFrame::MainFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(750, 450))
{
    InitializeMenubar();
    InitializeControls();

    Center();
}

void MainFrame::InitializeControls()
{
    m_queryPanel = new QueryPanel(this);
}

void MainFrame::InitializeMenubar()
{
    // creates menubar
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(ID_MENU_CONNECT, wxT("Connect..."));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_SETTINGS, wxT("Settings..."));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    m_helpMenu = new wxMenu();
    m_helpMenu->Append(ID_MENU_ABOUT, wxT("About..."));

    m_menubar = new wxMenuBar();
    m_menubar->Append(m_fileMenu, wxT("&File"));
    m_menubar->Append(m_helpMenu, wxT("&Help"));

    SetMenuBar(m_menubar);

    // wire events
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
}

void MainFrame::OnQuit(wxCommandEvent& evt)
{
    // TODO: cleanups resources
    Close();
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Are you sure to quit?"), wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

    int ret = dial->ShowModal();
    dial->Destroy();

    if (ret == wxID_YES)
    {
          Destroy();
    } else
    {
          evt.Veto();
    }
}
