#include "redisconnection.h"
#include <sys/time.h>
#include <wx/tokenzr.h>

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
    m_lastError = "";

    if (!IsConnected())
    {
        if (m_remoteHost == "")
            return false;

        timeval tv = { 1, 500000 };
        m_redisContext = (redisContext*)redisConnectWithTimeout(m_remoteHost.GetData().AsChar(), m_remotePort, tv);
        if (m_redisContext != NULL)
        {
            if (m_redisContext->err == REDIS_OK)
                return true;

            m_lastError = m_redisContext->errstr;
        }
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

RedisValue RedisConnection::GetValue(const wxString& key)
{
    if (!IsConnected())
        return RedisValue();

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "GET %s", key.GetData().AsChar());
    if (reply != NULL) {
        switch (reply->type)
        {
        case REDIS_REPLY_STRING:
            return RedisValue(wxString(reply->str));
            break;

        case REDIS_REPLY_INTEGER:
            return RedisValue(reply->integer);
            break;
        }

        freeReplyObject(reply);
    }

    return RedisValue();
}

void RedisConnection::IterateArrayResponse(redisReply **response, size_t length)
{
    m_redisKeys.Clear();
    redisReply **rep = response;
    for (int i = 0; i < length; i++)
    {
        if ((*rep)->type == REDIS_REPLY_STRING)
        {
            redisReply *r = *rep;
            wxString key(r->str);
            m_redisKeys.Add(key);
        }

        ++rep;
    }
}

int RedisConnection::FindKV(const wxString& keyPatterns)
{
    if (m_redisContext == NULL)
        return -1;


    wxString realPatterns = keyPatterns;
    realPatterns.Trim();

    if (keyPatterns == "")
    {
        realPatterns = "*";
    }

    int nRet = 0;
    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "KEYS %s", realPatterns.GetData().AsChar());
    if (reply != NULL)
    {
        switch (reply->type) {
        case REDIS_REPLY_STRING:
            break;

        case REDIS_REPLY_ARRAY:
            IterateArrayResponse(reply->element, reply->elements);
            break;

        default:
            break;
        }

        nRet = (int)reply->elements;

        freeReplyObject(reply);
    }

    return nRet;
}


wxString RedisConnection::GetServerInfo()
{
    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "INFO");
    if (reply != NULL)
    {
        wxArrayString tokens;

        if (reply->type == REDIS_REPLY_STRING)
        {
            tokens = wxStringTokenize(reply->str, "\r\n");
        }

        freeReplyObject(reply);

        for (int i = 0; i < tokens.Count(); i++)
        {
            wxSafeShowMessage("Info", tokens[i]);
        }

    }

    return "";
}
