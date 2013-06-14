#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include "hiredis/hiredis.h"

enum {
    REDIS_NONE_VALUE,
    REDIS_INT_VALUE,
    REDIS_STR_VALUE
};

#define N_SERVER_ATTRIBUTES 5

const wxString SERVER_ATTRIBUTES[N_SERVER_ATTRIBUTES] = {
    "os",
    "arch_bits",
    "uptime_in_seconds",
    "connected_clients",
    "used_memory"
};

class RedisSimpleValue
{
private:
    wxString    m_strValue;
    int m_intValue;
    int m_valueType;

public:

    RedisSimpleValue()
    {
        m_valueType = REDIS_NONE_VALUE;
    }

    RedisSimpleValue(int intValue)
    {
        m_valueType = REDIS_INT_VALUE;
        m_intValue = intValue;
    }

    RedisSimpleValue(const wxString& strValue)
    {
        m_valueType = REDIS_STR_VALUE;
        m_strValue = strValue;
    }

    RedisSimpleValue(const RedisSimpleValue& rhs)
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

class RedisSystemStatus
{
private:
    long    m_used;
    long    m_peak;
    long    m_rss;
    double  m_usedCpuSys;
    double  m_usedCpuUser;

public:
    RedisSystemStatus()
    {
        m_used = -1;
        m_peak = -1;
        m_rss = -1;
    }

    RedisSystemStatus(const RedisSystemStatus& rhs)
    {
        m_used = rhs.GetUsed();
        m_peak = rhs.GetPeak();
        m_rss = rhs.GetRss();
    }

    void SetCPUSys(double value)
    {
        m_usedCpuSys = value;
    }

    double GetCPUSys()
    {
        return m_usedCpuSys;
    }

    void SetCPUUser(double value)
    {
        m_usedCpuUser = value;
    }

    double GetCPUUser()
    {
        return m_usedCpuUser;
    }

    void SetUsed(long value)
    {
        m_used = value;
    }

    long GetUsed() const
    {
        return m_used;
    }

    void SetRss(long value)
    {
        m_rss = value;
    }

    long GetRss() const
    {
        return m_rss;
    }

    void SetPeak(long value)
    {
        m_peak = value;
    }

    long GetPeak() const
    {
        return m_peak;
    }
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
    void HandleKeysResponse(redisReply **response, size_t length);
    wxString ArrayReplyToString(redisReply **response, size_t length, int indent=-1);

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

    RedisSimpleValue ExecuteCommand(const wxString& key);

    RedisSimpleValue GetValue(const wxString& key);
    bool SetValue(const wxString& key, const RedisSimpleValue& value);
    bool DeleteKey(const wxString& key);
    bool SelectDb(uint db);
    bool Expire(const wxString& key, int seconds);

    int FindKeys(const wxString& keyPatterns);
    wxArrayString& GetKeysResult() { return m_redisKeys; }

    const wxString& GetRemoteHost() { return m_remoteHost; }
    const wxString& GetTitle() { return m_title; }
    int GetRemotePort() { return m_remotePort; }
    wxString GetLastError() { return m_lastError; }

    wxString GetServerInfo();
    RedisSystemStatus GetMemoryStatus();
};

#endif // REDISCONNECTION_H
