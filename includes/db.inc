// ============================================
//   db.inc — MySQL Database Queries
//   RPG Login & Code System
// ============================================

#if defined _db_included
    #endinput
#endif
#define _db_included

#include <a_mysql>
#include "config.inc"

new MySQL:g_SQL;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CONNECT & CREATE TABLES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock DB_Connect()
{
    g_SQL = mysql_connect(DB_HOST, DB_USER, DB_PASS, DB_NAME);
    if(g_SQL == MYSQL_INVALID_HANDLE || mysql_errno(g_SQL) != 0)
    {
        print("[DB] ERROR: Failed to connect to database!");
        SendRconCommand("exit");
        return 0;
    }
    print("[DB] Connected to MySQL successfully.");
    DB_CreateTables();
    return 1;
}

stock DB_CreateTables()
{
    mysql_query(g_SQL,
        "CREATE TABLE IF NOT EXISTS `codes` ( \
        `id` INT AUTO_INCREMENT PRIMARY KEY, \
        `code` VARCHAR(15) UNIQUE NOT NULL, \
        `created_by` VARCHAR(32) DEFAULT 'System', \
        `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP \
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;"
    );

    mysql_query(g_SQL,
        "CREATE TABLE IF NOT EXISTS `slots` ( \
        `id` INT AUTO_INCREMENT PRIMARY KEY, \
        `code` VARCHAR(15) NOT NULL, \
        `slot_number` TINYINT NOT NULL, \
        `is_unlocked` TINYINT DEFAULT 0, \
        `is_temp_locked` TINYINT DEFAULT 0, \
        FOREIGN KEY (`code`) REFERENCES `codes`(`code`) ON DELETE CASCADE \
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;"
    );

    mysql_query(g_SQL,
        "CREATE TABLE IF NOT EXISTS `characters` ( \
        `id` INT AUTO_INCREMENT PRIMARY KEY, \
        `code` VARCHAR(15) NOT NULL, \
        `slot_number` TINYINT NOT NULL, \
        `char_name` VARCHAR(32) UNIQUE NOT NULL, \
        `password` VARCHAR(64) NOT NULL, \
        `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (`code`) REFERENCES `codes`(`code`) ON DELETE CASCADE \
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;"
    );

    print("[DB] Tables created/verified successfully.");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CODES QUERIES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock bool:DB_CodeExists(const code[])
{
    new query[128], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `id` FROM `codes` WHERE `code` = '%e' LIMIT 1", code);
    result = mysql_query(g_SQL, query);
    new bool:exists = (cache_num_rows() > 0);
    cache_delete(result);
    return exists;
}

stock DB_InsertCode(const code[], const admin_name[])
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `codes` (`code`, `created_by`) VALUES ('%e', '%e')",
        code, admin_name);
    mysql_query(g_SQL, query);

    // slot 1 مفتوح تلقائياً، 2-5 مقفلة
    for(new i = 1; i <= MAX_SLOTS; i++)
    {
        new slot_query[256];
        mysql_format(g_SQL, slot_query, sizeof(slot_query),
            "INSERT INTO `slots` (`code`, `slot_number`, `is_unlocked`) VALUES ('%e', %d, %d)",
            code, i, (i == 1) ? 1 : 0);
        mysql_query(g_SQL, slot_query);
    }
}

stock DB_DeleteCode(const code[])
{
    new query[128];
    mysql_format(g_SQL, query, sizeof(query),
        "DELETE FROM `codes` WHERE `code` = '%e'", code);
    mysql_query(g_SQL, query);
    // CASCADE يحذف slots و characters تلقائياً
}

stock DB_GetAllCodes(out_result[][])
{
    new Cache:result = mysql_query(g_SQL, "SELECT `code` FROM `codes` ORDER BY `created_at` DESC");
    new count = cache_num_rows(), i;
    for(i = 0; i < count && i < 100; i++)
        cache_get_value(i, "code", out_result[i], 16);
    cache_delete(result);
    return count;
}

stock DB_GetCodeByCharName(const char_name[], out_code[])
{
    new query[128], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `code` FROM `characters` WHERE `char_name` = '%e' LIMIT 1", char_name);
    result = mysql_query(g_SQL, query);
    if(cache_num_rows() > 0)
        cache_get_value(0, "code", out_code, 16);
    cache_delete(result);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SLOTS QUERIES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock DB_GetSlotStatus(const code[], slot, &bool:is_unlocked, &bool:is_temp_locked)
{
    new query[256], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `is_unlocked`, `is_temp_locked` FROM `slots` \
        WHERE `code` = '%e' AND `slot_number` = %d LIMIT 1", code, slot);
    result = mysql_query(g_SQL, query);
    if(cache_num_rows() > 0)
    {
        new u, t;
        cache_get_value_int(0, "is_unlocked", u);
        cache_get_value_int(0, "is_temp_locked", t);
        is_unlocked = bool:u;
        is_temp_locked = bool:t;
    }
    cache_delete(result);
}

stock DB_GetLockedSlots(const code[], out_slots[], &count)
{
    new query[256], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `slot_number` FROM `slots` \
        WHERE `code` = '%e' AND `is_unlocked` = 0", code);
    result = mysql_query(g_SQL, query);
    count = cache_num_rows();
    for(new i = 0; i < count; i++)
        cache_get_value_int(i, "slot_number", out_slots[i]);
    cache_delete(result);
}

stock DB_SetSlotUnlocked(const code[], slot, bool:unlocked)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `slots` SET `is_unlocked` = %d \
        WHERE `code` = '%e' AND `slot_number` = %d",
        unlocked ? 1 : 0, code, slot);
    mysql_query(g_SQL, query);
}

stock DB_SetSlotTempLocked(const code[], slot, bool:locked)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `slots` SET `is_temp_locked` = %d \
        WHERE `code` = '%e' AND `slot_number` = %d",
        locked ? 1 : 0, code, slot);
    mysql_query(g_SQL, query);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CHARACTERS QUERIES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
stock bool:DB_CharExists(const char_name[])
{
    new query[128], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `id` FROM `characters` WHERE `char_name` = '%e' LIMIT 1", char_name);
    result = mysql_query(g_SQL, query);
    new bool:exists = (cache_num_rows() > 0);
    cache_delete(result);
    return exists;
}

stock DB_GetCharInSlot(const code[], slot, out_name[], out_pass[])
{
    new query[256], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `char_name`, `password` FROM `characters` \
        WHERE `code` = '%e' AND `slot_number` = %d LIMIT 1", code, slot);
    result = mysql_query(g_SQL, query);
    if(cache_num_rows() > 0)
    {
        cache_get_value(0, "char_name", out_name, CHAR_NAME_MAX);
        cache_get_value(0, "password", out_pass, CHAR_PASS_MAX);
    }
    cache_delete(result);
}

stock DB_InsertCharacter(const code[], slot, const char_name[], const hashed_pass[])
{
    new query[512];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `characters` (`code`, `slot_number`, `char_name`, `password`) \
        VALUES ('%e', %d, '%e', '%e')",
        code, slot, char_name, hashed_pass);
    mysql_query(g_SQL, query);
}

stock DB_DeleteCharacter(const code[], slot)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "DELETE FROM `characters` WHERE `code` = '%e' AND `slot_number` = %d",
        code, slot);
    mysql_query(g_SQL, query);
}

stock DB_ChangeCharPassword(const code[], slot, const new_pass[])
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `characters` SET `password` = '%e' \
        WHERE `code` = '%e' AND `slot_number` = %d",
        new_pass, code, slot);
    mysql_query(g_SQL, query);
}

stock DB_GetCharsByCode(const code[], out_names[][32], &count)
{
    new query[256], Cache:result;
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `char_name` FROM `characters` WHERE `code` = '%e'", code);
    result = mysql_query(g_SQL, query);
    count = cache_num_rows();
    for(new i = 0; i < count; i++)
        cache_get_value(i, "char_name", out_names[i], 32);
    cache_delete(result);
}