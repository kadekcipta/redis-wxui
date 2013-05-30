#include "redisconnection.h"

RedisConnection::RedisConnection(const wxString& remoteHost, int remotePort, const wxString& title):
    m_remoteHost(remoteHost), m_remotePort(remotePort), m_title(title), m_redisContext(NULL)
{
    if (m_title == "")
    {
        m_title = wxString::Format(wxT("%s:%d"), m_remoteHost, m_remotePort);
    }
}

RedisConnection::~RedisConnection()
{
    Disconnect();
}

bool RedisConnection::Connect()
{
    if (m_redisContext == NULL)
    {
        if (m_remoteHost == "")
            return false;

        m_redisContext = (redisContext*)redisConnect(m_remoteHost.GetData().AsChar(), m_remotePort);

        return m_redisContext != NULL;
    }

    return false;
}

void RedisConnection::Disconnect()
{
    if (m_redisContext != NULL)
    {
        redisFree(m_redisContext);
        m_redisContext = NULL;
    }
}


