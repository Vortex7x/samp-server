// ============================================
//   login.inc — Character Selection Screen
//   RPG Login & Code System
// ============================================

#if defined _login_included
    #endinput
#endif
#define _login_included

#include "config.inc"
#include "db.inc"
#include "characters.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PLAYER LOGIN SESSION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum E_LOGIN_DATA
{
    login_code[16],
    login_selected_slot,
    bool:login_slot_empty
}
new g_LoginData[MAX_PLAYERS][E_LOGIN_DATA];

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// TEXTDRAW IDs للشاشة
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
new PlayerText:g_TD_Background[MAX_PLAYERS];
new PlayerText:g_TD_Title[MAX_PLAYERS];
new PlayerText:g_TD_Slots[MAX_PLAYERS][MAX_SLOTS];
new PlayerText:g_TD_Model[MAX_PLAYERS];
new PlayerText:g_TD_BtnEnter[MAX_PLAYERS];
new PlayerText:g_TD_BtnCreate[MAX_PLAYERS];
new PlayerText:g_TD_BtnDelete[MAX_PLAYERS];

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ON CONNECT — تحقق من الكود
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Login_OnConnect(playerid)
{
    new pname[24];
    GetPlayerName(playerid, pname, sizeof(pname));

    if(!DB_CodeExists(pname))
    {
        SendClientMessage(playerid, COLOR_RED, "[Login] Invalid code. You have been kicked.");
        Kick(playerid);
        return;
    }

    format(g_LoginData[playerid][login_code], 16, "%s", pname);

    TogglePlayerControllable(playerid, false);
    SetPlayerPos(playerid, 1958.33, 1343.12, 15.36);
    SetPlayerFacingAngle(playerid, 269.15);
    SetCameraBehindPlayer(playerid);

    Login_ShowScreen(playerid);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SHOW CHARACTER SELECTION SCREEN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Login_ShowScreen(playerid)
{
    new code[16];
    format(code, 16, "%s", g_LoginData[playerid][login_code]);

    Login_DestroyScreen(playerid);

    // ── Background ──
    g_TD_Background[playerid] = CreatePlayerTextDraw(playerid, 320.0, 240.0, "box");
    PlayerTextDrawBackgroundColor(playerid, g_TD_Background[playerid], 0x000000AA);
    PlayerTextDrawUseBox(playerid, g_TD_Background[playerid], true);
    PlayerTextDrawBoxColor(playerid, g_TD_Background[playerid], 0x000000AA);
    PlayerTextDrawTextSize(playerid, g_TD_Background[playerid], 500.0, 300.0);
    PlayerTextDrawShow(playerid, g_TD_Background[playerid]);

    // ── Title ──
    g_TD_Title[playerid] = CreatePlayerTextDraw(playerid, 140.0, 130.0, "SELECT CHARACTER");
    PlayerTextDrawFont(playerid, g_TD_Title[playerid], 1);
    PlayerTextDrawLetterSize(playerid, g_TD_Title[playerid], 0.4, 1.6);
    PlayerTextDrawColor(playerid, g_TD_Title[playerid], COLOR_WHITE);
    PlayerTextDrawSetShadow(playerid, g_TD_Title[playerid], 1);
    PlayerTextDrawShow(playerid, g_TD_Title[playerid]);

    // ── Slots (اليسار) ──
    for(new i = 1; i <= MAX_SLOTS; i++)
    {
        new bool:is_unlocked, bool:is_temp_locked;
        DB_GetSlotStatus(code, i, is_unlocked, is_temp_locked);

        new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
        new bool:has_char = false;

        if(is_unlocked && !is_temp_locked)
        {
            DB_GetCharInSlot(code, i, db_name, db_pass);
            has_char = (strlen(db_name) > 0);
        }

        new slot_text[64];
        if(!is_unlocked)
            format(slot_text, sizeof(slot_text), "~r~[LOCKED] Slot %d", i);
        else if(is_temp_locked)
            format(slot_text, sizeof(slot_text), "~o~[SUSPENDED] Slot %d", i);
        else if(has_char)
            format(slot_text, sizeof(slot_text), "~w~%s", db_name);
        else
            format(slot_text, sizeof(slot_text), "~g~[EMPTY] Slot %d", i);

        new Float:y_pos = 160.0 + (i - 1) * 22.0;
        g_TD_Slots[playerid][i-1] = CreatePlayerTextDraw(playerid, 145.0, y_pos, slot_text);
        PlayerTextDrawFont(playerid, g_TD_Slots[playerid][i-1], 1);
        PlayerTextDrawLetterSize(playerid, g_TD_Slots[playerid][i-1], 0.3, 1.2);
        PlayerTextDrawColor(playerid, g_TD_Slots[playerid][i-1], COLOR_WHITE);

        // الخانات المفتوحة فقط قابلة للضغط
        if(is_unlocked && !is_temp_locked)
        {
            PlayerTextDrawSetSelectable(playerid, g_TD_Slots[playerid][i-1], true);
        }

        PlayerTextDrawShow(playerid, g_TD_Slots[playerid][i-1]);
    }

    // ── 3D Model Preview (اليمين) ──
    g_TD_Model[playerid] = CreatePlayerTextDraw(playerid, 440.0, 180.0, "_");
    PlayerTextDrawFont(playerid, g_TD_Model[playerid], TEXT_DRAW_FONT_MODEL_PREVIEW);
    PlayerTextDrawTextSize(playerid, g_TD_Model[playerid], 80.0, 120.0);
    PlayerTextDrawSetPreviewModel(playerid, g_TD_Model[playerid], 0);
    PlayerTextDrawSetPreviewRot(playerid, g_TD_Model[playerid], 0.0, 0.0, 0.0, 1.0);
    PlayerTextDrawShow(playerid, g_TD_Model[playerid]);

    // ── Buttons ──
    g_TD_BtnEnter[playerid] = CreatePlayerTextDraw(playerid, 145.0, 295.0, "ENTER");
    PlayerTextDrawFont(playerid, g_TD_BtnEnter[playerid], 1);
    PlayerTextDrawLetterSize(playerid, g_TD_BtnEnter[playerid], 0.3, 1.2);
    PlayerTextDrawColor(playerid, g_TD_BtnEnter[playerid], COLOR_GREEN);
    PlayerTextDrawSetSelectable(playerid, g_TD_BtnEnter[playerid], true);
    PlayerTextDrawShow(playerid, g_TD_BtnEnter[playerid]);

    g_TD_BtnCreate[playerid] = CreatePlayerTextDraw(playerid, 210.0, 295.0, "CREATE");
    PlayerTextDrawFont(playerid, g_TD_BtnCreate[playerid], 1);
    PlayerTextDrawLetterSize(playerid, g_TD_BtnCreate[playerid], 0.3, 1.2);
    PlayerTextDrawColor(playerid, g_TD_BtnCreate[playerid], COLOR_YELLOW);
    PlayerTextDrawSetSelectable(playerid, g_TD_BtnCreate[playerid], true);
    PlayerTextDrawShow(playerid, g_TD_BtnCreate[playerid]);

    g_TD_BtnDelete[playerid] = CreatePlayerTextDraw(playerid, 275.0, 295.0, "DELETE");
    PlayerTextDrawFont(playerid, g_TD_BtnDelete[playerid], 1);
    PlayerTextDrawLetterSize(playerid, g_TD_BtnDelete[playerid], 0.3, 1.2);
    PlayerTextDrawColor(playerid, g_TD_BtnDelete[playerid], COLOR_RED);
    PlayerTextDrawSetSelectable(playerid, g_TD_BtnDelete[playerid], true);
    PlayerTextDrawShow(playerid, g_TD_BtnDelete[playerid]);

    SelectTextDraw(playerid, 0xFFFFFFFF);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DESTROY SCREEN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Login_DestroyScreen(playerid)
{
    if(g_TD_Background[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_Background[playerid]);
    if(g_TD_Title[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_Title[playerid]);
    if(g_TD_Model[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_Model[playerid]);
    if(g_TD_BtnEnter[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_BtnEnter[playerid]);
    if(g_TD_BtnCreate[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_BtnCreate[playerid]);
    if(g_TD_BtnDelete[playerid] != PlayerText:INVALID_TEXT_DRAW)
        PlayerTextDrawDestroy(playerid, g_TD_BtnDelete[playerid]);

    for(new i = 0; i < MAX_SLOTS; i++)
    {
        if(g_TD_Slots[playerid][i] != PlayerText:INVALID_TEXT_DRAW)
            PlayerTextDrawDestroy(playerid, g_TD_Slots[playerid][i]);
    }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ON TEXTDRAW CLICK
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock Login_OnClick(playerid, PlayerText:playertextid)
{
    new code[16];
    format(code, 16, "%s", g_LoginData[playerid][login_code]);

    // ضغط على خانة
    for(new i = 0; i < MAX_SLOTS; i++)
    {
        if(playertextid == g_TD_Slots[playerid][i])
        {
            new slot = i + 1;
            g_LoginData[playerid][login_selected_slot] = slot;

            // تحديث الـ 3D model preview
            new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
            DB_GetCharInSlot(code, slot, db_name, db_pass);

            new bool:has_char = (strlen(db_name) > 0);
            g_LoginData[playerid][login_slot_empty] = !has_char;

            // عرض skin المناسب
            new skin = has_char ? GetPlayerSkin(playerid) : 0;
            PlayerTextDrawSetPreviewModel(playerid, g_TD_Model[playerid], skin);
            PlayerTextDrawShow(playerid, g_TD_Model[playerid]);
            return;
        }
    }

    // زر ENTER
    if(playertextid == g_TD_BtnEnter[playerid])
    {
        new slot = g_LoginData[playerid][login_selected_slot];
        if(slot == 0)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "[Login] Please select a slot first.");
            return;
        }

        new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
        DB_GetCharInSlot(code, slot, db_name, db_pass);

        if(strlen(db_name) == 0)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "[Login] This slot is empty. Create a character first.");
            return;
        }

        CancelSelectTextDraw(playerid);
        Login_DestroyScreen(playerid);

        ShowPlayerDialog(playerid, DIALOG_CHAR_PASSWORD, DIALOG_STYLE_PASSWORD,
            "Character Password",
            "Enter your character password:", "Login", "Back");
        return;
    }

    // زر CREATE
    if(playertextid == g_TD_BtnCreate[playerid])
    {
        new slot = g_LoginData[playerid][login_selected_slot];
        if(slot == 0)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "[Login] Please select an empty slot first.");
            return;
        }

        new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
        DB_GetCharInSlot(code, slot, db_name, db_pass);

        if(strlen(db_name) > 0)
        {
            SendClientMessage(playerid, COLOR_RED, "[Login] This slot already has a character.");
            return;
        }

        CancelSelectTextDraw(playerid);
        Login_DestroyScreen(playerid);

        ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE, DIALOG_STYLE_INPUT,
            "Create Character",
            "Enter your character name:", "Next", "Back");
        return;
    }

    // زر DELETE
    if(playertextid == g_TD_BtnDelete[playerid])
    {
        new slot = g_LoginData[playerid][login_selected_slot];
        if(slot == 0)
        {
            SendClientMessage(playerid, COLOR_YELLOW, "[Login] Please select a slot first.");
            return;
        }

        new db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
        DB_GetCharInSlot(code, slot, db_name, db_pass);

        if(strlen(db_name) == 0)
        {
            SendClientMessage(playerid, COLOR_RED, "[Login] No character in this slot to delete.");
            return;
        }

        new msg[128];
        format(msg, sizeof(msg), "Delete character '%s'?\nYou will need to enter your password to confirm.", db_name);
        ShowPlayerDialog(playerid, DIALOG_CONFIRM, DIALOG_STYLE_MSGBOX,
            "Delete Character", msg, "Yes", "No");
        return;
    }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DIALOG RESPONSES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static new g_TempCharName[MAX_PLAYERS][CHAR_NAME_MAX];

stock Login_OnDialogResponse(playerid, dialogid, response, inputtext[])
{
    new code[16];
    format(code, 16, "%s", g_LoginData[playerid][login_code]);
    new slot = g_LoginData[playerid][login_selected_slot];

    switch(dialogid)
    {
        // ── باسورد الدخول ──
        case DIALOG_CHAR_PASSWORD:
        {
            if(!response) { Login_ShowScreen(playerid); return; }
            Char_TryLogin(playerid, code, slot, inputtext);
        }

        // ── إنشاء شخصية - اسم ──
        case DIALOG_CHAR_CREATE:
        {
            if(!response) { Login_ShowScreen(playerid); return; }
            format(g_TempCharName[playerid], CHAR_NAME_MAX, "%s", inputtext);
            ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE_PASS, DIALOG_STYLE_PASSWORD,
                "Create Character — Password",
                "Enter a password for your character:", "Create", "Back");
        }

        // ── إنشاء شخصية - باسورد ──
        case DIALOG_CHAR_CREATE_PASS:
        {
            if(!response) { Login_ShowScreen(playerid); return; }
            Char_Create(playerid, code, slot, g_TempCharName[playerid], inputtext);
        }

        // ── تأكيد حذف شخصية (من اللاعب نفسه) ──
        case DIALOG_CONFIRM:
        {
            if(response)
            {
                new admin_name[32], db_name[CHAR_NAME_MAX], db_pass[CHAR_PASS_MAX];
                GetPlayerName(playerid, admin_name, sizeof(admin_name));
                DB_GetCharInSlot(code, slot, db_name, db_pass);
                DB_DeleteCharacter(code, slot);
                Webhook_CharDeleted(code, slot, db_name, admin_name);
                SendClientMessage(playerid, COLOR_GREEN, "[Login] Character deleted.");
                Login_ShowScreen(playerid);
            }
            else Login_ShowScreen(playerid);
        }
    }
}