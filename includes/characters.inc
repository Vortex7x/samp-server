// ============================================
//   characters.inc — Character System
//   RPG Login & Code System
// ============================================

#if defined _characters_included
    #endinput
#endif
#define _characters_included

#include "config.inc"
#include "db.inc"
#include "webhook.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PLAYER SESSION DATA
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum E_CHAR_DATA
{
    bool:char_loaded,
    char_slot,
    char_name[CHAR_NAME_MAX],
    char_pass_tries,
    char_selected_slot,        // الخانة اللي الإداري شغال عليها
    char_target_code[16]       // الكود المؤقت للإداري
}
new g_CharData[MAX_PLAYERS][E_CHAR_DATA];

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// RESET ON CONNECT / DISCONNECT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Char_ResetData(playerid)
{
    g_CharData[playerid][char_loaded]       = false;
    g_CharData[playerid][char_slot]         = 0;
    g_CharData[playerid][char_name]         = "";
    g_CharData[playerid][char_pass_tries]   = 0;
    g_CharData[playerid][char_selected_slot]= 0;
    g_CharData[playerid][char_target_code]  = "";
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// GETTERS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock bool:Char_IsLoaded(playerid)
    return g_CharData[playerid][char_loaded];

stock Char_GetSlot(playerid)
    return g_CharData[playerid][char_slot];

stock Char_GetName(playerid, out_name[], len)
    format(out_name, len, "%s", g_CharData[playerid][char_name]);

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PASSWORD HASH
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Char_HashPassword(const password[], out_hash[], len)
{
    // جديد — MD5 موجود في MySQL plugin
MD5(password, out_hash, len);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CREATE CHARACTER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Char_Create(playerid, const code[], slot, const char_name[], const password[])
{
    // تحقق من الاسم
    if(strlen(char_name) < 3 || strlen(char_name) > CHAR_NAME_MAX)
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] Name must be 3-32 characters.");
        return;
    }

    // تحقق من الباسورد
    if(strlen(password) < 4)
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] Password must be at least 4 characters.");
        return;
    }

    // تحقق ان الاسم مش مكرر
    if(DB_CharExists(char_name))
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] This character name is already taken.");
        return;
    }

    // هاش الباسورد و احفظ
    new hashed[65];
    Char_HashPassword(password, hashed, sizeof(hashed));
    DB_InsertCharacter(code, slot, char_name, hashed);

    // لود الشخصية مباشرة
    g_CharData[playerid][char_loaded]   = true;
    g_CharData[playerid][char_slot]     = slot;
    format(g_CharData[playerid][char_name], CHAR_NAME_MAX, "%s", char_name);

    new msg[128];
    format(msg, sizeof(msg), "[Char] ✅ Character '%s' created in slot %d!", char_name, slot);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    Char_SpawnPlayer(playerid);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// LOGIN TO CHARACTER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Char_TryLogin(playerid, const code[], slot, const password[])
{
    new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
    DB_GetCharInSlot(code, slot, db_name, db_pass);

    if(strlen(db_name) == 0)
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] No character found in this slot.");
        return;
    }

    new hashed[65];
    Char_HashPassword(password, hashed, sizeof(hashed));

    if(!strcmp(hashed, db_pass, true))
    {
        // باسورد صح
        g_CharData[playerid][char_loaded]   = true;
        g_CharData[playerid][char_slot]     = slot;
        g_CharData[playerid][char_pass_tries] = 0;
        format(g_CharData[playerid][char_name], CHAR_NAME_MAX, "%s", db_name);

        new msg[64];
        format(msg, sizeof(msg), "[Char] Welcome back, %s!", db_name);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Char_SpawnPlayer(playerid);
    }
    else
    {
        // باسورد غلط
        g_CharData[playerid][char_pass_tries]++;

        if(g_CharData[playerid][char_pass_tries] >= MAX_PASSWORD_TRIES)
        {
            SendClientMessage(playerid, COLOR_RED, "[Char] Too many wrong attempts. Kicked.");
            Kick(playerid);
            return;
        }

        new msg[128];
        format(msg, sizeof(msg), "[Char] Wrong password! Attempt %d/%d",
            g_CharData[playerid][char_pass_tries], MAX_PASSWORD_TRIES);
        SendClientMessage(playerid, COLOR_RED, msg);
    }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SPAWN PLAYER AFTER LOGIN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Char_SpawnPlayer(playerid)
{
    // هنا تحط إحداثيات الـ spawn الافتراضية للسيرفر
    SetSpawnInfo(playerid, 0, 0, 1958.33, 1343.12, 15.36, 269.15, 0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);

    // تعيين اسم الشخصية بدل الكود
    SetPlayerName(playerid, g_CharData[playerid][char_name]);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ADMIN: DELETE CHARACTER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
new g_DelCharCode[MAX_PLAYERS][16];
new g_DelCharSlot[MAX_PLAYERS];

stock Char_Admin_AskDelete(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_ADMIN_DELETECHAR, DIALOG_STYLE_INPUT,
        "Delete Character — Enter Code",
        "Enter the player code:", "Next", "Cancel");
}

stock Char_Admin_AskDeleteSlot(playerid, const code[])
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] Code not found.");
        return;
    }
    format(g_DelCharCode[playerid], 16, "%s", code);

    ShowPlayerDialog(playerid, DIALOG_ADMIN_DELETECHAR + 1, DIALOG_STYLE_INPUT,
        "Delete Character — Enter Slot",
        "Enter slot number (1-5):", "Next", "Cancel");
}

stock Char_Admin_ConfirmDelete(playerid, slot)
{
    new code[16];
    format(code, 16, "%s", g_DelCharCode[playerid]);

    new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
    DB_GetCharInSlot(code, slot, db_name, db_pass);

    if(strlen(db_name) == 0)
    {
        SendClientMessage(playerid, COLOR_RED, "[Char] No character in this slot.");
        return;
    }

    g_DelCharSlot[playerid] = slot;

    new msg[128];
    format(msg, sizeof(msg),
        "Delete character '%s' from slot %d?\n(Slot will remain open but empty)",
        db_name, slot);
    ShowPlayerDialog(playerid, DIALOG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "Confirm Delete", msg, "Yes, Delete", "Cancel");
}

stock Char_Admin_ExecuteDelete(playerid)
{
    new code[16];
    format(code, 16, "%s", g_DelCharCode[playerid]);
    new slot = g_DelCharSlot[playerid];

    new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
    DB_GetCharInSlot(code, slot, db_name, db_pass);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));

    DB_DeleteCharacter(code, slot);
    Webhook_CharDeleted(code, slot, db_name, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Char] ✅ Character '%s' deleted from slot %d.", db_name, slot);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}