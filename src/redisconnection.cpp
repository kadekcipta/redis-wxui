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

bool RedisConnection::SelectDb(uint db)
{
    if (!IsConnected())
        return false;

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "SELECT %d", db);
    bool ret = reply != NULL && reply->type != REDIS_REPLY_ERROR;

    if (reply != NULL) {
        freeReplyObject(reply);
    }

    return ret;
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

wxString RedisConnection::CheckReply(redisReply *reply) const
{
    wxString ret(wxEmptyString);

    if (reply != NULL) {
        switch (reply->type)
        {
        case REDIS_REPLY_ERROR:
            ret = wxString(reply->str, reply->len);
            break;

        case REDIS_REPLY_STATUS:
            ret = wxString(reply->str, reply->len);
            break;

        }
    }

    return wxEmptyString;
}

bool RedisConnection::DeleteKey(const wxString &key)
{
    if (!IsConnected())
        return false;

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "DEL %s", key.GetData().AsChar());
    bool ret = reply != NULL && reply->type != REDIS_REPLY_ERROR;

    if (reply != NULL) {
        freeReplyObject(reply);
    }

    return ret;
}

bool RedisConnection::SetValue(const wxString &key, const RedisValue &value)
{
    if (!IsConnected())
        return false;

    redisReply *reply = NULL;

    if (value.GetValueType() == REDIS_INT_VALUE)
        reply = (redisReply*)redisCommand(m_redisContext, "SET %s %d", key.GetData().AsChar(), value.GetIntValue());
    else if (value.GetValueType() == REDIS_STR_VALUE)
        reply = (redisReply*)redisCommand(m_redisContext, "SET %s %s", key.GetData().AsChar(), value.GetStrValue().GetData().AsChar());

    bool ret = reply != NULL && reply->type != REDIS_REPLY_ERROR;

    if (reply != NULL) {
        freeReplyObject(reply);
    }

    return ret;
}

bool RedisConnection::Expire(const wxString &key, int seconds)
{
    if (!IsConnected())
        return false;

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "EXPIRE %s %d", key.GetData().AsChar(), seconds);
    bool ret = reply != NULL && reply->type != REDIS_REPLY_ERROR;

    if (reply != NULL) {
        freeReplyObject(reply);
    }

    return ret;
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
        realPatterns = "*";

    int nRet = 0;
    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "KEYS %s", realPatterns.GetData().AsChar());
    if (reply != NULL && reply->type == REDIS_REPLY_ARRAY)
    {
        IterateArrayResponse(reply->element, reply->elements);
        nRet = (int)reply->elements;
        freeReplyObject(reply);
    }

    GetServerInfo();

    return nRet;
}


wxString RedisConnection::GetServerInfo()
{
    wxString tmp;

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
            wxString token = tokens[i];
            if (token.StartsWith("#"))
                continue;

            wxArrayString kv = wxStringTokenize(token, ":");

            for (int n = 0; n < N_SERVER_ATTRIBUTES; n++)
            {
                if (kv[0] == SERVER_ATTRIBUTES[n])
                {
                    token.Replace("_", " ");
                    token.MakeCapitalized();
                    tmp.Append(token).Append(" ");
                }
            }
        }
    }

    return tmp;
}
