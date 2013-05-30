#include "mainframe.h"
#include "serverpanel.h"
#include "querypanel.h"
#include "connectiondialog.h"

MainFrame::MainFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(550, 650))
{
    InitializeMenubar();
    InitializeControls();

    Center();
}

void MainFrame::InitializeControls()
{
    m_mainTab = new wxNotebook(this, ID_MAIN_TAB);
    RedisConnection *connection = new RedisConnection("127.0.0.1");
    m_queryPanel = new QueryPanel(m_mainTab, connection);
    m_mainTab->AddPage(m_queryPanel, connection->GetTitle());

    RedisConnection *connection2 = new RedisConnection("127.0.0.1");
    m_mainTab->AddPage(new QueryPanel(m_mainTab, connection2), connection2->GetTitle());
}

void MainFrame::InitializeMenubar()
{
    // creates menubar
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(ID_MENU_CONNECT, wxT("Connect..."));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_SETTINGS, wxT("Settings..."))->Enable(false);
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
    Connect(ID_MENU_CONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddConnection));
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
}

void MainFrame::AddConnection()
{
    ConnectionDialog dlg(this, wxT("Add Connection"));
    if (dlg.ShowModal() == wxID_OK)
    {

    }
}

void MainFrame::OnAddConnection(wxCommandEvent &evt)
{
    AddConnection();
}

void MainFrame::OnQuit(wxCommandEvent& evt)
{
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
