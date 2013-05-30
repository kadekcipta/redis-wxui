#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "hiredis/hiredis.h"

class RedisConnection
{
private:
    wxString    m_remoteHost;
    wxString    m_title;
    int         m_remotePort;

    redisContext    *m_redisContext;

private:
    bool Connect();
    void Disconnect();

public:
    RedisConnection(const wxString& remoteHost, int remotePort=6379, const wxString& title="");
    virtual ~RedisConnection();

    const wxString& GetRemoteHost() { return m_remoteHost; }
    const wxString& GetTitle() { return m_title; }
    int GetRemotePort() { return m_remotePort; }
};

#endif // REDISCONNECTION_H
