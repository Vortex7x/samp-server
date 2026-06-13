// ============================================
//   main.pwn — Main Gamemode Entry Point
//   RPG Login & Code System
// ============================================

#include <a_samp>
#include <a_mysql>
#include <a_http>
#include <zcmd>
#include <sscanf2>

#include "includes/config.inc"
#include "includes/db.inc"
#include "includes/webhook.inc"
#include "includes/codes.inc"
#include "includes/slots.inc"
#include "includes/characters.inc"
#include "includes/login.inc"
#include "includes/admin_cmds.inc"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// GAMEMODE INIT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnGameModeInit()
{
    DB_Connect();
    print("[Main] RPG Login System loaded.");
    print("[Main] Code System | Slot System | Character System — READY");
    return 1;
}

public OnGameModeExit()
{
    mysql_close(g_SQL);
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PLAYER CONNECT / DISCONNECT
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnPlayerConnect(playerid)
{
    Char_ResetData(playerid);
    Login_OnConnect(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Login_DestroyScreen(playerid);
    Char_ResetData(playerid);
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// TEXTDRAW CLICK
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    if(!Char_IsLoaded(playerid))
        Login_OnClick(playerid, playertextid);
    return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
    if(clickedid == Text:INVALID_TEXT_DRAW)
    {
        if(!Char_IsLoaded(playerid))
            Login_ShowScreen(playerid);
    }
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DIALOG RESPONSE — ROUTER
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    // Login dialogs
    Login_OnDialogResponse(playerid, dialogid, response, inputtext);

    // Admin dialogs
    AdminCmds_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);

    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PLAYER SPAWN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnPlayerSpawn(playerid)
{
    if(!Char_IsLoaded(playerid))
    {
        // لو حاول يـ spawn قبل اختيار شخصية
        Login_ShowScreen(playerid);
        return 0;
    }

    TogglePlayerControllable(playerid, true);
    return 1;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// PREVENT COMMANDS BEFORE LOGIN
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!Char_IsLoaded(playerid))
    {
        SendClientMessage(playerid, COLOR_RED, "[!] You must select a character first.");
        return 1;
    }
    return 0;
}