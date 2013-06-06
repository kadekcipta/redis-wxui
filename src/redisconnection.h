#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "hiredis/hiredis.h"

enum {
    REDIS_NONE_VALUE,
    REDIS_INT_VALUE,
    REDIS_STR_VALUE
};

class RedisValue
{
private:
    wxString    m_strValue;
    int m_intValue;
    int m_valueType;

public:

    RedisValue()
    {
        m_valueType = REDIS_NONE_VALUE;
    }

    RedisValue(int intValue)
    {
        m_valueType = REDIS_INT_VALUE;
        m_intValue = intValue;
    }

    RedisValue(const wxString& strValue)
    {
        m_valueType = REDIS_STR_VALUE;
        m_strValue = strValue;
    }

    RedisValue(const RedisValue& rhs)
    {
        m_valueType = rhs.m_valueType;
        m_intValue = rhs.m_intValue;
        m_strValue = rhs.m_strValue;
    }


    void SetStrValue(const wxString& value) { m_strValue = value; }
    const wxString& GetStrValue() const { return m_strValue; }

    void SetIntValue(int value) { m_intValue = value; }
    int GetIntValue() const { return m_intValue; }

    void SetValueType(int valueType) { m_valueType = valueType; }
    int GetValueType() const { return m_valueType; }
};

class RedisConnection
{
private:
    wxString    m_remoteHost;
    wxString    m_title;
    int         m_remotePort;
    wxString    m_lastError;

    redisContext    *m_redisContext;
    wxArrayString   m_redisKeys;

private:
    void IterateArrayResponse(redisReply **response, size_t length);

public:
    RedisConnection(const wxString& remoteHost, int remotePort=6379, const wxString& title="");
    virtual ~RedisConnection();

    bool Connect();
    void Disconnect();
    bool IsConnected()
    {
        if (m_redisContext != NULL)
        {
            return (m_redisContext->flags & REDIS_CONNECTED) == REDIS_CONNECTED;
        }

        return false;
    }

    RedisValue GetValue(const wxString& key);
    bool SetValue(const wxString& key, const RedisValue& value);
    bool DeleteKey(const wxString& key);
    bool SelectDb(uint db);
    bool Expire(const wxString& key, int seconds);

    int FindKV(const wxString& keyPatterns);
    wxArrayString& GetKeysResult() { return m_redisKeys; }

    const wxString& GetRemoteHost() { return m_remoteHost; }
    const wxString& GetTitle() { return m_title; }
    int GetRemotePort() { return m_remotePort; }
    wxString GetLastError() { return m_lastError; }

    wxString GetServerInfo();
};

#endif // REDISCONNECTION_H
