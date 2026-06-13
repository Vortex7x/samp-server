// ============================================
//   webhook.inc — Discord Webhook via HTTP
//   RPG Login & Code System
// ============================================

#if defined _webhook_included
    #endinput
#endif
#define _webhook_included

#include <a_http>
#include "config.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// INTERNAL SEND FUNCTION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static stock Webhook_Send(const url[], const json_body[])
{
    new host[128], path[256];

    // استخراج الـ host و path من الـ URL
    // مثال: discord.com/api/webhooks/xxx
    new pos = strfind(url, "/", false, 8); // بعد https://
    if(pos == -1) return;

    strmid(host, url, 8, pos, sizeof(host));
    strmid(path, url, pos, strlen(url), sizeof(path));

    new headers[256];
    format(headers, sizeof(headers), "Content-Type: application/json\r\n");

    HTTP(0, HTTP_POST, host, path, json_body, "Webhook_Response", headers);
}

forward Webhook_Response(index, response_code, data[]);
public Webhook_Response(index, response_code, data[])
{
    // 204 = Discord webhook success
    if(response_code != 204)
        printf("[Webhook] Warning: Response code %d", response_code);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CODE MANAGEMENT WEBHOOKS → CHANNEL_1
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Webhook_CodeCreated(const code[], const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"✅ New Code Created\",\"color\":3447003,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Created By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        code, admin_name);
    Webhook_Send(WEBHOOK_CODES, body);
}

stock Webhook_CodeDeleted(const code[], const admin_name[], chars_count)
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"🗑️ Code Deleted\",\"color\":15158332,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Characters Deleted\",\"value\":\"%d\",\"inline\":true},\
        {\"name\":\"Deleted By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        code, chars_count, admin_name);
    Webhook_Send(WEBHOOK_CODES, body);
}

stock Webhook_CodeSearch(const char_name[], const found_code[], const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"🔍 Code Search\",\"color\":10181046,\
        \"fields\":[\
        {\"name\":\"Character\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Code Found\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Searched By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        char_name, found_code, admin_name);
    Webhook_Send(WEBHOOK_CODES, body);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CHARACTER MANAGEMENT WEBHOOKS → CHANNEL_2
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Webhook_SlotOpened(const code[], slot, const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"🔓 Slot Opened\",\"color\":3066993,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Slot\",\"value\":\"%d\",\"inline\":true},\
        {\"name\":\"Opened By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        code, slot, admin_name);
    Webhook_Send(WEBHOOK_CHARS, body);
}

stock Webhook_CharDeleted(const code[], slot, const char_name[], const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"🗑️ Character Deleted\",\"color\":15158332,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Slot\",\"value\":\"%d\",\"inline\":true},\
        {\"name\":\"Character\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Deleted By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        code, slot, char_name, admin_name);
    Webhook_Send(WEBHOOK_CHARS, body);
}

stock Webhook_SlotLocked(const code[], slot, bool:locked, const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"%s Slot %s\",\"color\":%d,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Slot\",\"value\":\"%d\",\"inline\":true},\
        {\"name\":\"By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        locked ? "🔒" : "🔓",
        locked ? "Temp Locked" : "Unlocked",
        locked ? 15158332 : 3066993,
        code, slot, admin_name);
    Webhook_Send(WEBHOOK_CHARS, body);
}

stock Webhook_PasswordChanged(const code[], slot, const admin_name[])
{
    new body[512];
    format(body, sizeof(body),
        "{\"embeds\":[{\"title\":\"🔑 Password Changed\",\"color\":16776960,\
        \"fields\":[\
        {\"name\":\"Code\",\"value\":\"%s\",\"inline\":true},\
        {\"name\":\"Slot\",\"value\":\"%d\",\"inline\":true},\
        {\"name\":\"Changed By\",\"value\":\"%s\",\"inline\":true}\
        ]}]}",
        code, slot, admin_name);
    Webhook_Send(WEBHOOK_CHARS, body);
}