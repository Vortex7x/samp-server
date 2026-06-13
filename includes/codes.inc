// ============================================
//   codes.inc — Code Generation & Management
//   RPG Login & Code System
// ============================================

#if defined _codes_included
    #endinput
#endif
#define _codes_included

#include "config.inc"
#include "db.inc"
#include "webhook.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CODE GENERATION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock GenerateUniqueCode(out_code[])
{
    new charset[] = CODE_CHARS;
    new charset_len = strlen(charset);
    new attempts = 0;

    do {
        // طول عشوائي بين 9 و 15
        new length = CODE_MIN_LENGTH + random(CODE_MAX_LENGTH - CODE_MIN_LENGTH + 1);
        new bool:has_underscore = false;

        for(new i = 0; i < length; i++)
        {
            new idx = random(charset_len);
            out_code[i] = charset[idx];
            if(charset[idx] == '_') has_underscore = true;
        }
        out_code[length] = '\0';

        // نضمن وجود underscore واحد على الأقل
        if(!has_underscore)
        {
            new pos = random(length);
            out_code[pos] = '_';
        }

        attempts++;
        if(attempts > 100)
        {
            print("[Codes] ERROR: Could not generate unique code after 100 attempts!");
            out_code[0] = '\0';
            return 0;
        }
    }
    while(DB_CodeExists(out_code));

    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CREATE CODE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Codes_Create(playerid)
{
    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));

    new code[16];
    if(!GenerateUniqueCode(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Code] Failed to generate unique code. Try again.");
        return;
    }

    DB_InsertCode(code, admin_name);
    Webhook_CodeCreated(code, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Code] ✅ New code created: %s (Slot 1 auto-unlocked)", code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SHOW ALL CODES DIALOG
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Codes_ShowAll(playerid)
{
    new codes[100][16];
    new count = DB_GetAllCodes(codes);

    if(count == 0)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Code] No codes found.");
        return;
    }

    new dialog_str[4096];
    for(new i = 0; i < count; i++)
    {
        // جيب الشخصيات اللي في كل كود
        new char_names[5][32], char_count;
        DB_GetCharsByCode(codes[i], char_names, char_count);

        new chars_str[200];
        if(char_count == 0)
        {
            format(chars_str, sizeof(chars_str), "No characters");
        }
        else
        {
            for(new j = 0; j < char_count; j++)
            {
                if(j > 0) strcat(chars_str, ", ");
                strcat(chars_str, char_names[j]);
            }
        }

        new line[256];
        format(line, sizeof(line), "%s\t%s\n", codes[i], chars_str);
        strcat(dialog_str, line);
    }

    ShowPlayerDialog(playerid, DIALOG_ADMIN_CODES, DIALOG_STYLE_TABLIST_HEADERS,
        "All Codes", dialog_str, "Close", "");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DELETE CODE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// نخزن الكود اللي ناوي الإداري يحذفه
new g_PendingDeleteCode[MAX_PLAYERS][16];

stock Codes_AskDelete(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_ADMIN_DELETECODE, DIALOG_STYLE_INPUT,
        "Delete Code",
        "Enter the code you want to delete:\n(This will delete the code and ALL characters inside it)",
        "Confirm", "Cancel");
}

stock Codes_ConfirmDelete(playerid, const code[])
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Code] Code not found.");
        return;
    }

    format(g_PendingDeleteCode[playerid], 16, "%s", code);

    new msg[128];
    format(msg, sizeof(msg),
        "Are you sure you want to delete code: %s?\n(All characters will be permanently deleted)", code);
    ShowPlayerDialog(playerid, DIALOG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "Confirm Delete", msg, "Yes, Delete", "Cancel");
}

stock Codes_ExecuteDelete(playerid)
{
    new code[16];
    format(code, 16, "%s", g_PendingDeleteCode[playerid]);

    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Code] Code no longer exists.");
        return;
    }

    // عد الشخصيات قبل الحذف للـ webhook
    new char_names[5][32], char_count;
    DB_GetCharsByCode(code, char_names, char_count);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));

    // طرد اللاعب لو هو أونلاين بهذا الكود
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(IsPlayerConnected(i))
        {
            new pname[24];
            GetPlayerName(i, pname, sizeof(pname));
            if(!strcmp(pname, code, true))
                Kick(i);
        }
    }

    DB_DeleteCode(code);
    Webhook_CodeDeleted(code, admin_name, char_count);

    new msg[128];
    format(msg, sizeof(msg), "[Code] ✅ Code %s deleted with %d characters.", code, char_count);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    g_PendingDeleteCode[playerid][0] = '\0';
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SEARCH BY CHARACTER NAME
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Codes_SearchByChar(playerid, const char_name[])
{
    new found_code[16];
    DB_GetCodeByCharName(char_name, found_code);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));

    if(strlen(found_code) == 0)
    {
        new msg[128];
        format(msg, sizeof(msg), "[Code] Character '%s' not found.", char_name);
        SendClientMessage(playerid, COLOR_RED, msg);
        return;
    }

    Webhook_CodeSearch(char_name, found_code, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Code] Character '%s' belongs to code: %s", char_name, found_code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}