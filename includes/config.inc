// ============================================
//   config.inc — System Configuration
//   RPG Login & Code System
// ============================================

#if defined _config_included
    #endinput
#endif
#define _config_included

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DATABASE SETTINGS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define DB_HOST         "localhost"
#define DB_USER         "root"
#define DB_PASS         "your_password"
#define DB_NAME         "samp_rpg"
#define DB_PORT         3306

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DISCORD WEBHOOKS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define WEBHOOK_CODES   "https://discord.com/api/webhooks/CHANNEL_1_URL"
#define WEBHOOK_CHARS   "https://discord.com/api/webhooks/CHANNEL_2_URL"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ADMIN LEVELS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define ADMIN_LEVEL_CODES   5   // مطلوب لإدارة الأكواد
#define ADMIN_LEVEL_CHARS   5   // مطلوب لإدارة الشخصيات

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CODE SETTINGS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define CODE_MIN_LENGTH     9
#define CODE_MAX_LENGTH     15
#define CODE_CHARS          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CHARACTER SETTINGS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define MAX_SLOTS           5   // عدد الخانات لكل كود
#define MAX_PASSWORD_TRIES  3   // أقصى محاولات خاطئة
#define CHAR_NAME_MAX       32
#define CHAR_PASS_MAX       64

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// DIALOG IDs
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define DIALOG_CHAR_SELECT      100
#define DIALOG_CHAR_PASSWORD    101
#define DIALOG_CHAR_CREATE      102
#define DIALOG_CHAR_CREATE_PASS 103
#define DIALOG_ADMIN_CODES      104
#define DIALOG_ADMIN_DELETECODE 105
#define DIALOG_ADMIN_OPENSLOT   106
#define DIALOG_ADMIN_DELETECHAR 107
#define DIALOG_ADMIN_LOCKSLOT   108
#define DIALOG_ADMIN_UNLOCKSLOT 109
#define DIALOG_ADMIN_CHANGEPASS 110
#define DIALOG_CONFIRM          111

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// COLORS
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#define COLOR_RED       0xFF0000FF
#define COLOR_GREEN     0x00FF00FF
#define COLOR_YELLOW    0xFFFF00FF
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_GREY      0xAAAAAFF
#define COLOR_ADMIN     0xFF6600FF