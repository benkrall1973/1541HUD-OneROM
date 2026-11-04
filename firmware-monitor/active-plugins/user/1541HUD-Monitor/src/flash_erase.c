#include "plugin.h"
#include "flash_erase.h"

/* This function is copied to RAM before XIP is disabled. Keep it self
 * contained: no globals, literals, or helpers fetched from flash. */
ORA_SECTION(".flash_erase_fn") __attribute__((noinline))
void hud1541_flash_commit(flash_exit_xip_fn_t exit_xip,
    flash_range_erase_fn_t erase, flash_range_program_fn_t program,
    flash_flush_cache_fn_t flush, flash_select_xip_read_mode_fn_t select_xip,
    uint32_t erase_offset, uint32_t program_offset, const uint8_t *page,
    uint32_t erase_size, uint32_t page_size, uint8_t clkdiv) {
 uint32_t primask;
 __asm volatile("mrs %0, primask \n\t cpsid i" : "=r"(primask) :: "memory");
 exit_xip();
 erase(erase_offset, erase_size, 65536u, 0xD8u);
 program(program_offset, page, page_size);
 select_xip(3u, clkdiv);
 flush();
 __asm volatile("msr primask, %0" :: "r"(primask) : "memory");
}
