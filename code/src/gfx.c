#include "gfx.h"
#include "draw.h"
#include "input.h"
#include "3ds/svc.h"
#include "z3D/z3D.h"
#include "dungeon_rewards.h"
#include "rHeap.h"
#include "custom_models.h"
#include "objects.h"
#include "title_screen.h"

static u8 GfxInit = 0;

static void Gfx_DrawChangeMenuPrompt(void) {
    Draw_DrawString(10, SCREEN_BOT_HEIGHT - 20, COLOR_TITLE, "Press L/R to change menu");
}

static void Gfx_DrawDungeonItems(void) {
    for (u32 dungeonId = 0; dungeonId <= DUNGEON_GERUDO_FORTRESS; ++dungeonId) {
        s32 keys = (gSaveContext.dungeonKeys[dungeonId] >= 0) ? gSaveContext.dungeonKeys[dungeonId] : 0;
        Draw_DrawFormattedString(10, 10 + (dungeonId * SPACING_Y), COLOR_WHITE, "%-25s %s: %d %s",
            DungeonNames[dungeonId], "Small Keys", keys, gSaveContext.dungeonItems[dungeonId] & 1 ? "Boss Key" : "");
    }
    Gfx_DrawChangeMenuPrompt();
    Draw_FlushFramebuffer();
}

static void Gfx_DrawDungeonRewards(void) {
    for (u32 dungeonId = 0; dungeonId <= DUNGEON_SHADOW_TEMPLE; ++dungeonId) {
        Draw_DrawFormattedString(10, 10 + (dungeonId * SPACING_Y), COLOR_WHITE, "%-30s - %s",
            DungeonNames[dungeonId], DungeonReward_GetName(dungeonId));
    }
    Gfx_DrawChangeMenuPrompt();
    Draw_FlushFramebuffer();
}

static const void (*menu_draw_funcs[])(void) = {
    Gfx_DrawDungeonItems,
    Gfx_DrawDungeonRewards,
};

static void Gfx_ShowMenu(void) {
    static s32 curMenuIdx = 0;
    u32 pressed = 0;

    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();

    do {
        if (pressed & BUTTON_B) {
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            break;
        } else if (pressed & BUTTON_R1) {
            curMenuIdx++;
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
        } else if (pressed & BUTTON_L1) {
            curMenuIdx--;
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
        }

        if (curMenuIdx < 0) {
            curMenuIdx = (ARR_SIZE(menu_draw_funcs) - 1);
        } else if (curMenuIdx >= ARR_SIZE(menu_draw_funcs)) {
            curMenuIdx = 0;
        }

        menu_draw_funcs[curMenuIdx]();
        pressed = Input_WaitWithTimeout(1000);

    } while(true);
}

void Gfx_Init(void) {
    Draw_SetupFramebuffer();

    // Setup the title screen logo edits
    gObjectTable[330].size = 0xA5CB0;
    gActorOverlayTable[0x171].initInfo->init = EnMag_rInit;

    GfxInit = 1;
}

void Gfx_Update(void) {
    if (!GfxInit) {
        Gfx_Init();
    }

    if(gSaveContext.gameMode == 0 && rInputCtx.cur.sel) {
        Gfx_ShowMenu();
        svcSleepThread(1000 * 1000 * 300LL);
    }
}
