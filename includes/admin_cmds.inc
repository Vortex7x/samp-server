// ============================================
//   admin_cmds.inc — Admin Commands
//   RPG Login & Code System
// ============================================

#if defined _admin_cmds_included
    #endinput
#endif
#define _admin_cmds_included

#include <a_samp>
#include <zcmd>
#include "config.inc"
#include "codes.inc"
#include "slots.inc"
#include "characters.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HELPER — CHECK ADMIN LEVEL
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ** غيّر هذا حسب نظام الأدمن عندك **
static stock bool:IsAdmin(playerid, level)
{
    // مثال: return (g_AdminLevel[playerid] >= level);
    return IsPlayerAdmin(playerid); // RCON مؤقتاً
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CODE COMMANDS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// /createcode
CMD:createcode(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CODES))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    Codes_Create(playerid);
    return 1;
}

// /showcodes
CMD:showcodes(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CODES))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    Codes_ShowAll(playerid);
    return 1;
}

// /deletecode [code]
CMD:deletecode(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CODES))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    if(isnull(params))
    {
        Codes_AskDelete(playerid);
        return 1;
    }

    Codes_ConfirmDelete(playerid, params);
    return 1;
}

// /searchcode [character_name]
CMD:searchcode(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CODES))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    if(isnull(params))
        return SendClientMessage(playerid, COLOR_YELLOW, "[Usage] /searchcode [character_name]"), 1;

    Codes_SearchByChar(playerid, params);
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SLOT COMMANDS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// /openslot [code]
CMD:openslot(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CHARS))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    if(isnull(params))
    {
        Slots_AskCode_Open(playerid);
        return 1;
    }

    Slots_ShowLockedSlots(playerid, params);
    return 1;
}

// /lockslot [code] [slot]
CMD:lockslot(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CHARS))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    new code[16], slot;
    if(sscanf(params, "s[16]i", code, slot))
        return SendClientMessage(playerid, COLOR_YELLOW, "[Usage] /lockslot [code] [slot 1-5]"), 1;

    if(slot < 1 || slot > MAX_SLOTS)
        return SendClientMessage(playerid, COLOR_RED, "[!] Slot must be between 1 and 5."), 1;

    Slots_TempLock(playerid, code, slot);
    return 1;
}

// /unlockslot [code] [slot]
CMD:unlockslot(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CHARS))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    new code[16], slot;
    if(sscanf(params, "s[16]i", code, slot))
        return SendClientMessage(playerid, COLOR_YELLOW, "[Usage] /unlockslot [code] [slot 1-5]"), 1;

    if(slot < 1 || slot > MAX_SLOTS)
        return SendClientMessage(playerid, COLOR_RED, "[!] Slot must be between 1 and 5."), 1;

    Slots_Unlock(playerid, code, slot);
    return 1;
}

// /deletechar [code] [slot]
CMD:deletechar(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CHARS))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    new code[16], slot;
    if(sscanf(params, "s[16]i", code, slot))
    {
        Char_Admin_AskDelete(playerid);
        return 1;
    }

    if(slot < 1 || slot > MAX_SLOTS)
        return SendClientMessage(playerid, COLOR_RED, "[!] Slot must be between 1 and 5."), 1;

    Char_Admin_ConfirmDelete(playerid, slot);
    return 1;
}

// /changepasschar [code] [slot]
CMD:changepasschar(playerid, params[])
{
    if(!IsAdmin(playerid, ADMIN_LEVEL_CHARS))
        return SendClientMessage(playerid, COLOR_RED, "[!] You don't have permission."), 1;

    new code[16], slot;
    if(sscanf(params, "s[16]i", code, slot))
    {
        Slots_AskChangePass(playerid);
        return 1;
    }

    if(slot < 1 || slot > MAX_SLOTS)
        return SendClientMessage(playerid, COLOR_RED, "[!] Slot must be between 1 and 5."), 1;

    format(g_ChangePassCode[playerid], 16, "%s", code);
    Slots_AskNewPassword(playerid, slot);
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DIALOG RESPONSES للأدمن
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock AdminCmds_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        // حذف كود - إدخال الكود
        case DIALOG_ADMIN_DELETECODE:
        {
            if(!response) return;
            Codes_ConfirmDelete(playerid, inputtext);
        }

        // تأكيد حذف الكود
        case DIALOG_CONFIRM:
        {
            if(response) Codes_ExecuteDelete(playerid);
        }

        // فتح خانة - إدخال الكود
        case DIALOG_ADMIN_OPENSLOT:
        {
            if(!response) return;
            Slots_ShowLockedSlots(playerid, inputtext);
        }

        // فتح خانة - اختيار الخانة
        case DIALOG_ADMIN_OPENSLOT + 1:
        {
            if(!response) return;
            new locked_slots[5], locked_count;
            DB_GetLockedSlots(g_SlotTargetCode[playerid], locked_slots, locked_count);
            if(listitem < locked_count)
                Slots_ExecuteOpen(playerid, locked_slots[listitem]);
        }

        // حذف شخصية - إدخال الكود
        case DIALOG_ADMIN_DELETECHAR:
        {
            if(!response) return;
            Char_Admin_AskDeleteSlot(playerid, inputtext);
        }

        // حذف شخصية - إدخال الخانة
        case DIALOG_ADMIN_DELETECHAR + 1:
        {
            if(!response) return;
            Char_Admin_ConfirmDelete(playerid, strval(inputtext));
        }

        // تغيير باسورد - إدخال الكود
        case DIALOG_ADMIN_CHANGEPASS:
        {
            if(!response) return;
            Slots_AskChangePassSlot(playerid, inputtext);
        }

        // تغيير باسورد - إدخال الخانة
        case DIALOG_ADMIN_CHANGEPASS + 1:
        {
            if(!response) return;
            Slots_AskNewPassword(playerid, strval(inputtext));
        }

        // تغيير باسورد - الباسورد الجديد
        case DIALOG_ADMIN_CHANGEPASS + 2:
        {
            if(!response) return;
            Slots_ExecuteChangePass(playerid, inputtext);
        }
    }
}