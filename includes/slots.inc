// ============================================
//   slots.inc — Slot Management System
//   RPG Login & Code System
// ============================================

#if defined _slots_included
    #endinput
#endif
#define _slots_included

#include "config.inc"
#include "db.inc"
#include "webhook.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PLAYER TEMP DATA
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
new g_SlotTargetCode[MAX_PLAYERS][16];   // الكود اللي الإداري شغال عليه
new g_SlotTargetSlot[MAX_PLAYERS];       // الخانة اللي اختارها الإداري

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// OPEN SLOT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Slots_AskCode_Open(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_ADMIN_OPENSLOT, DIALOG_STYLE_INPUT,
        "Open Slot — Enter Code",
        "Enter the player code to open a slot for:", "Next", "Cancel");
}

stock Slots_ShowLockedSlots(playerid, const code[])
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Slots] Code not found.");
        return;
    }

    new locked_slots[5], locked_count;
    DB_GetLockedSlots(code, locked_slots, locked_count);

    if(locked_count == 0)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Slots] All slots are already open for this code.");
        return;
    }

    format(g_SlotTargetCode[playerid], 16, "%s", code);

    // نبني الـ dialog بالخانات المقفولة فقط
    new dialog_str[256];
    for(new i = 0; i < locked_count; i++)
    {
        new line[32];
        format(line, sizeof(line), "Slot %d\n", locked_slots[i]);
        strcat(dialog_str, line);
    }

    ShowPlayerDialog(playerid, DIALOG_ADMIN_OPENSLOT + 1, DIALOG_STYLE_LIST,
        "Select Slot to Open", dialog_str, "Open", "Cancel");
}

stock Slots_ExecuteOpen(playerid, slot)
{
    new code[16];
    format(code, 16, "%s", g_SlotTargetCode[playerid]);

    DB_SetSlotUnlocked(code, slot, true);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    Webhook_SlotOpened(code, slot, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Slots] ✅ Slot %d opened for code: %s", slot, code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// TEMP LOCK SLOT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Slots_TempLock(playerid, const code[], slot)
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Slots] Code not found.");
        return;
    }

    DB_SetSlotTempLocked(code, slot, true);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    Webhook_SlotLocked(code, slot, true, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Slots] 🔒 Slot %d temp locked for code: %s", slot, code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// UNLOCK TEMP LOCKED SLOT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Slots_Unlock(playerid, const code[], slot)
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Slots] Code not found.");
        return;
    }

    DB_SetSlotTempLocked(code, slot, false);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    Webhook_SlotLocked(code, slot, false, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Slots] 🔓 Slot %d unlocked for code: %s", slot, code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CHANGE CHARACTER PASSWORD
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
new g_ChangePassCode[MAX_PLAYERS][16];
new g_ChangePassSlot[MAX_PLAYERS];

stock Slots_AskChangePass(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_ADMIN_CHANGEPASS, DIALOG_STYLE_INPUT,
        "Change Password — Enter Code",
        "Enter the player code:", "Next", "Cancel");
}

stock Slots_AskChangePassSlot(playerid, const code[])
{
    if(!DB_CodeExists(code))
    {
        SendClientMessage(playerid, COLOR_RED, "[Slots] Code not found.");
        return;
    }
    format(g_ChangePassCode[playerid], 16, "%s", code);

    ShowPlayerDialog(playerid, DIALOG_ADMIN_CHANGEPASS + 1, DIALOG_STYLE_INPUT,
        "Change Password — Enter Slot",
        "Enter slot number (1-5):", "Next", "Cancel");
}

stock Slots_AskNewPassword(playerid, slot)
{
    g_ChangePassSlot[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_ADMIN_CHANGEPASS + 2, DIALOG_STYLE_INPUT,
        "Change Password — New Password",
        "Enter the new password:", "Confirm", "Cancel");
}

stock Slots_ExecuteChangePass(playerid, const new_pass[])
{
    new code[16];
    format(code, 16, "%s", g_ChangePassCode[playerid]);
    new slot = g_ChangePassSlot[playerid];

    new hashed[65];
    // جديد
MD5(new_pass, hashed, sizeof(hashed));

    DB_ChangeCharPassword(code, slot, hashed);

    new admin_name[32];
    GetPlayerName(playerid, admin_name, sizeof(admin_name));
    Webhook_PasswordChanged(code, slot, admin_name);

    new msg[128];
    format(msg, sizeof(msg), "[Slots] ✅ Password changed for slot %d | code: %s", slot, code);
    SendClientMessage(playerid, COLOR_GREEN, msg);
}