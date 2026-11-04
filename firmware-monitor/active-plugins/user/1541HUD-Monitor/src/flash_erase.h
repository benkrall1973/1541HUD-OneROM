#ifndef HUD1541_FLASH_ERASE_H
#define HUD1541_FLASH_ERASE_H

#include <stdint.h>

typedef void (*flash_exit_xip_fn_t)(void);
typedef void (*flash_range_erase_fn_t)(uint32_t, uint32_t, uint32_t, uint8_t);
typedef void (*flash_flush_cache_fn_t)(void);
typedef void (*flash_select_xip_read_mode_fn_t)(uint8_t, uint8_t);
typedef void (*flash_range_program_fn_t)(uint32_t, const uint8_t *, uint32_t);
typedef void (*connect_internal_flash_fn_t)(void);

typedef void (*hud1541_flash_commit_fn_t)(flash_exit_xip_fn_t, flash_range_erase_fn_t,
    flash_range_program_fn_t, flash_flush_cache_fn_t, flash_select_xip_read_mode_fn_t,
    uint32_t, uint32_t, const uint8_t *, uint32_t, uint32_t, uint8_t);

void hud1541_flash_commit(flash_exit_xip_fn_t, flash_range_erase_fn_t,
    flash_range_program_fn_t, flash_flush_cache_fn_t, flash_select_xip_read_mode_fn_t,
    uint32_t, uint32_t, const uint8_t *, uint32_t, uint32_t, uint8_t);
#endif
