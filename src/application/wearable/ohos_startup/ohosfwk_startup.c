/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ohosfwk_startup
 * Create: 2025-11
 */

#include "common_def.h"
#include "debug_print.h"
#include "system_init.h"
#include "ohosfwk_startup.h"
#ifdef ENABLE_UIKIT
#include "graphic_service_wrapper.h"
#endif
#ifdef SUPPORT_POWER_MANAGER
#include "power_display_service.h"
#endif

#ifdef ENABLE_UIKIT
#include "soc_lcd_api.h"
#endif

#ifdef CFG_DRIVERS_NANDFLASH
#include "nand_fmc.h"
#endif

void ohos_show_logo(void)
{
#ifdef ENABLE_UIKIT
    /* 未接lcd时不能调用图形相关类的实例 */
    if (!uapi_is_lcd_connect()) {
        return;
    }
#endif

#ifdef ENABLE_UIKIT
    InitGraphicService();
#endif
#ifdef SUPPORT_POWER_MANAGER
    const power_display_svr_api_t *display_api = power_display_svr_get_api();
    display_api->turn_on_screen();
    display_api->set_screen_set_keepon_timeout(5000); // 5000ms
#endif
#ifdef ENABLE_UIKIT
    ShowGraphicLogo();
#endif
}

void ohos_startup(void)
{
#ifdef ENABLE_UIKIT
    /* 未接lcd时不能调用图形相关类的实例 */
    if (!uapi_is_lcd_connect()) {
        return;
    }
#endif
    OHOS_SystemInit();
}
