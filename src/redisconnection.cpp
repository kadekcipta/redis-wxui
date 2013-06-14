#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

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

        timeval tv = { 5, 0 };
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

RedisSimpleValue RedisConnection::GetValue(const wxString& key)
{
    if (!IsConnected())
        return RedisSimpleValue();

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "GET %s", key.GetData().AsChar());
    if (reply != NULL) {
        switch (reply->type)
        {
        case REDIS_REPLY_STRING:
            return RedisSimpleValue(wxString(reply->str));
            break;

        case REDIS_REPLY_INTEGER:
            return RedisSimpleValue(reply->integer);
            break;
        }

        freeReplyObject(reply);
    }

    return RedisSimpleValue();
}

RedisSimpleValue RedisConnection::ExecuteCommand(const wxString& command)
{
    if (!IsConnected())
        return RedisSimpleValue();

    wxString _command(command);
    if (_command.Trim(false).Upper().StartsWith("MONITOR"))
        return RedisSimpleValue();

    redisReply *reply = (redisReply*)redisCommand(m_redisContext, command.GetData().AsChar());
    if (reply != NULL) {

        RedisSimpleValue ret;

        switch (reply->type)
        {
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_ERROR:
            ret = RedisSimpleValue(wxString(reply->str));
            break;

        case REDIS_REPLY_ARRAY:
            ret = RedisSimpleValue(ArrayReplyToString(reply->element, reply->elements));
            break;

        case REDIS_REPLY_NIL:
            ret = RedisSimpleValue("(nil)");
            break;

        case REDIS_REPLY_INTEGER:
            ret = RedisSimpleValue(reply->integer);
            break;
        }

        freeReplyObject(reply);

        return ret;
    }

    return RedisSimpleValue();
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

bool RedisConnection::SetValue(const wxString &key, const RedisSimpleValue &value)
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

wxString RedisConnection::ArrayReplyToString(redisReply **response, size_t length, int indent)
{
    wxString tmp;
    wxString indentStr;
    if (indent > 0)
    {
        for (int i = 0; i < indent; i++)
            indentStr.Append("\t");
    }

    redisReply **rep = response;
    for (int i = 0; i < length; i++)
    {
        redisReply *r = *rep;

        switch ((*rep)->type) {
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_STRING:
            tmp.Append(indentStr);
            tmp.Append(wxString(r->str));
            tmp.Append("\n");
            break;

        case REDIS_REPLY_INTEGER:
            tmp.Append(indentStr);
            tmp.Append(wxString::Format("(integer) %d", (int)r->integer));
            tmp.Append("\n");
            break;

        case REDIS_REPLY_ARRAY:
            tmp.Append(ArrayReplyToString((*rep)->element, (*rep)->elements, indent + 1));
            break;
        }

        ++rep;
    }

    return tmp;
}

void RedisConnection::HandleKeysResponse(redisReply **response, size_t length)
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

int RedisConnection::FindKeys(const wxString& keyFilter)
{
    if (m_redisContext == NULL)
        return -1;

    wxString _keyFilter = keyFilter;
    _keyFilter.Trim();

    if (keyFilter == "")
        _keyFilter = "*";

    int nKeys = 0;
    redisReply *reply = (redisReply*)redisCommand(m_redisContext, "KEYS %s", _keyFilter.GetData().AsChar());
    if (reply != NULL && reply->type == REDIS_REPLY_ARRAY)
    {
        HandleKeysResponse(reply->element, reply->elements);
        nKeys = (int)reply->elements;
        freeReplyObject(reply);
    }

    GetServerInfo();

    return nKeys;
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

RedisMemoryStatus RedisConnection::GetMemoryStatus()
{
    RedisMemoryStatus status;

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
            if (kv.GetCount() == 2)
            {
                long v = -1;
                wxString sValue = kv[1];
                sValue.ToLong(&v);
                if (kv[0] == "used_memory")
                {
                    status.SetUsed(v);
                }
                else if (kv[0] == "used_memory_peak")
                {
                    status.SetPeak(v);
                }
            }
        }
    }

    return status;
}
