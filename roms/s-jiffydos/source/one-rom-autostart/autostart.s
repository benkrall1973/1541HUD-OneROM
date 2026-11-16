; 1541 boot shim for the S-JiffyDOS RAMBoard profile.
; Runs from the E000 ROM page in the temporary Auto Start profile.
; The relocated RBCP code executes from the 1541's built-in RAM.

.setcpu "6502"
.macpack longbranch

ROM_C_PAGE = $C000
ROM_E_PAGE = $E000
RAM_SOURCE = $A000
RAM_TARGET = $8000
SHADOW_END = $C000
RAM_TARGET_END = $A000

RBCP_CMD_PAGE = $C000
RBCP_TOKEN = $C102
RBCP_PROGRESS = $C104
RBCP_STATUS = $C105
RBCP_DATA = $C108

RBCP_COMPLETE = $BB
RBCP_STATUS_OK = $CC

RBCP_GRP_CONTROL = $00
RBCP_GRP_READ = $01
RBCP_GRP_MODIFY = $02
RBCP_GRP_RESET = $AA

RBCP_CMD_ENTER_CMD_RESP = $01
RBCP_CMD_GET_RAM_SLOT_INFO_ALL = $03
RBCP_CMD_LOAD_SLOT = $02
RBCP_CMD_SWITCH_AND_EXIT = $04
RBCP_CMD_RESET = $AA

; RBCP flash slot numbers omit plugin entries and are zero based. The
; bootloader is flash slot 0. Override at assembly time if profile order moves
; the hidden runtime; the default preserves the original r11 layout.
.ifndef RBCP_RUNTIME_FLASH_SLOT
RBCP_RUNTIME_FLASH_SLOT = $0A
.endif

; ZP used by the ROM-to-RAM relocation stub.
ZP_SRC_LO = $F8
ZP_SRC_HI = $F9
ZP_DST_LO = $FA
ZP_DST_HI = $FB
ZP_LEN_LO = $FC
ZP_LEN_HI = $FD

; ZP used by the relocated RBCP client.
RBCP_OLD_TOKEN = $F0
RBCP_TMP = $F1
RBCP_TARGET_RAM = $F2
RBCP_COUNT = $F3

.segment "BOOT"

reset_entry:
    sei
    cld
    ldx #$FF
    txs

    ; Copy the S-Jiffy shadow page from the temporary A000 mapping into the
    ; RAMBoard at 8000-9FFF. The source page is part of the Auto Start profile.
    lda #<RAM_SOURCE
    sta ZP_SRC_LO
    lda #>RAM_SOURCE
    sta ZP_SRC_HI
    lda #<RAM_TARGET
    sta ZP_DST_LO
    lda #>RAM_TARGET
    sta ZP_DST_HI
    ldy #$00
@copy_shadow:
    lda (ZP_SRC_LO),y
    sta (ZP_DST_LO),y
    iny
    bne @copy_shadow
    inc ZP_SRC_HI
    inc ZP_DST_HI
    lda ZP_DST_HI
    cmp #>RAM_TARGET_END
    bne @copy_shadow

    ; Verify the complete 8 KiB copy before changing the served ROM profile.
    lda #<RAM_SOURCE
    sta ZP_SRC_LO
    lda #>RAM_SOURCE
    sta ZP_SRC_HI
    lda #<RAM_TARGET
    sta ZP_DST_LO
    lda #>RAM_TARGET
    sta ZP_DST_HI
    ldy #$00
@verify_shadow:
    lda (ZP_SRC_LO),y
    cmp (ZP_DST_LO),y
    bne boot_copy_failed
    iny
    bne @verify_shadow
    inc ZP_SRC_HI
    inc ZP_DST_HI
    lda ZP_DST_HI
    cmp #>RAM_TARGET_END
    bne @verify_shadow

    ; Relocate all self-modifying RBCP code out of ROM into 1541 RAM.
    lda #<__CODE_LOAD__
    sta ZP_SRC_LO
    lda #>__CODE_LOAD__
    sta ZP_SRC_HI
    lda #<__CODE_RUN__
    sta ZP_DST_LO
    lda #>__CODE_RUN__
    sta ZP_DST_HI
    lda #<__CODE_SIZE__
    sta ZP_LEN_LO
    lda #>__CODE_SIZE__
    sta ZP_LEN_HI
    ldy #$00
@relocate:
    lda (ZP_SRC_LO),y
    sta (ZP_DST_LO),y
    iny
    bne @relocate_page
    inc ZP_SRC_HI
    inc ZP_DST_HI
@relocate_page:
    lda ZP_LEN_LO
    bne @relocate_dec_lo
    dec ZP_LEN_HI
@relocate_dec_lo:
    dec ZP_LEN_LO
    lda ZP_LEN_LO
    ora ZP_LEN_HI
    bne @relocate
    jmp __CODE_RUN__

boot_copy_failed:
    ; Leave the temporary profile active for recovery through the HUD/USB.
    sei
@failed:
    jmp @failed

.segment "CODE"

.import __CODE_LOAD__, __CODE_RUN__, __CODE_SIZE__

autostart_main:
    jsr rbcp_reset
    jsr enter_cmd_response
    jcs boot_failed

    ; Ask Host-Control how many working slots exist and which is active.
    lda #RBCP_GRP_READ
    ldx #RBCP_CMD_GET_RAM_SLOT_INFO_ALL
    jsr issue_no_args
    jcs boot_failed
    lda RBCP_DATA
    cmp #$02
    jcc boot_failed
    lda RBCP_DATA+1
    beq @use_slot_one
    lda #$00
    beq @save_target
@use_slot_one:
    lda #$01
@save_target:
    sta RBCP_TARGET_RAM

    ; Copy the staged S-Jiffy runtime set into an inactive OneROM RAM slot.
    lda RBCP_TARGET_RAM
    ldx #RBCP_RUNTIME_FLASH_SLOT
    jsr issue_load_slot
    jcs boot_failed

    ; Atomically switch to the RAM copy and leave RBCP. This terminal command
    ; intentionally has no response header. Give the OneROM command processor
    ; time to finish, then emulate a 1541 reset by jumping through its new
    ; profile's reset vector. The code doing the jump is in drive RAM, so it
    ; survives the ROM-page change.
    lda RBCP_TARGET_RAM
    sta cmd_arg0
    lda #RBCP_GRP_CONTROL
    sta cmd_group
    lda #RBCP_CMD_SWITCH_AND_EXIT
    sta cmd_cmd
    jsr send_group_cmd
    lda cmd_arg0
    jsr send_byte
    jsr pause_short
    jsr pause_short
    jmp ($FFFC)
@halt:
    jmp @halt

; ---------------------------------------------------------------------------
; Minimal RBCP host implementation for a 24-pin Fire-24G serving 2364 chips.
; All 13 address pins are observed, so each command byte is one address apart.
; ---------------------------------------------------------------------------

rbcp_reset:
    ; Flush partial command frames, reset command state, then reset again from
    ; a known knock boundary as recommended by the RBCP specification.
    ldx #$05
@flush:
    lda #RBCP_GRP_RESET
    jsr send_byte
    lda #RBCP_CMD_RESET
    jsr send_byte
    dex
    bne @flush
    jsr pause_short
    lda #RBCP_GRP_RESET
    jsr send_byte
    lda #RBCP_CMD_RESET
    jsr send_byte
    jsr pause_short
    jsr knock
    lda #RBCP_GRP_RESET
    jsr send_byte
    lda #RBCP_CMD_RESET
    jsr send_byte
    jmp pause_short

enter_cmd_response:
    lda RBCP_TOKEN
    sta RBCP_OLD_TOKEN
    jsr knock
    lda #RBCP_GRP_CONTROL
    jsr send_byte
    lda #RBCP_CMD_ENTER_CMD_RESP
    jsr send_byte
    ; page-relative C0 = 0, back-channel offset $0100, size $0200,
    ; progress $BB and status $CC. Nine arguments total.
    lda #$00
    jsr send_byte
    lda #$00
    jsr send_byte
    lda #$00
    jsr send_byte
    lda #$01
    jsr send_byte
    lda #$00
    jsr send_byte
    lda #$00
    jsr send_byte
    lda #$02
    jsr send_byte
    lda #RBCP_COMPLETE
    jsr send_byte
    lda #RBCP_STATUS_OK
    jsr send_byte
    jsr wait_token_short
    bcs @failed
    jsr wait_progress_long
    bcs @failed
    lda RBCP_STATUS
    cmp #RBCP_STATUS_OK
    bne @failed
    clc
    rts
@failed:
    sec
    rts

issue_no_args:
    sta cmd_group
    stx cmd_cmd
    jsr save_token
    jsr send_group_cmd
    jsr wait_token_short
    bcs @failed
    jsr wait_progress_long
    bcs @failed
    lda RBCP_STATUS
    cmp #RBCP_STATUS_OK
    bne @failed
    clc
    rts
@failed:
    sec
    rts

issue_one_arg:
    sta cmd_group
    stx cmd_cmd
    jsr save_token
    jsr send_group_cmd
    lda cmd_arg0
    jsr send_byte
    jsr wait_token_short
    bcs @failed
    jsr wait_progress_long
    bcs @failed
    lda RBCP_STATUS
    cmp #RBCP_STATUS_OK
    bne @failed
    clc
    rts
@failed:
    sec
    rts

issue_three_args:
    sta cmd_group
    stx cmd_cmd
    jsr save_token
    jsr send_group_cmd
    lda cmd_arg0
    jsr send_byte
    lda cmd_arg1
    jsr send_byte
    lda cmd_arg2
    jsr send_byte
    jsr wait_token_short
    bcs @failed
    jsr wait_progress_long
    bcs @failed
    lda RBCP_STATUS
    cmp #RBCP_STATUS_OK
    bne @failed
    clc
    rts
@failed:
    sec
    rts

issue_load_slot:
    stx cmd_arg1
    sta cmd_arg0
    ldx #RBCP_CMD_LOAD_SLOT
    lda #RBCP_GRP_MODIFY
    sta cmd_group
    stx cmd_cmd
    jsr save_token
    jsr send_group_cmd
    lda cmd_arg0
    jsr send_byte
    lda cmd_arg1
    jsr send_byte
    jsr wait_token_short
    bcs @failed
    jsr wait_progress_long
    bcs @failed
    lda RBCP_STATUS
    cmp #RBCP_STATUS_OK
    bne @failed
    clc
    rts
@failed:
    sec
    rts

send_group_cmd:
    lda cmd_group
    jsr send_byte
    lda cmd_cmd
    jmp send_byte

save_token:
    lda RBCP_TOKEN
    sta RBCP_OLD_TOKEN
    rts

wait_token_short:
    ldx #$FF
@loop:
    lda RBCP_TOKEN
    cmp RBCP_OLD_TOKEN
    bne @ok
    dex
    bne @loop
    sec
    rts
@ok:
    clc
    rts

wait_progress_long:
    ldy #$FF
@outer:
    ldx #$FF
@inner:
    lda RBCP_PROGRESS
    cmp #RBCP_COMPLETE
    beq @ok
    dex
    bne @inner
    dey
    bne @outer
    sec
    rts
@ok:
    clc
    rts

send_byte:
    sta send_read+1
send_read:
    lda RBCP_CMD_PAGE
    rts

knock:
    lda #$21
    jsr send_byte
    lda #$52
    jsr send_byte
    lda #$42
    jsr send_byte
    lda #$43
    jsr send_byte
    lda #$50
    jsr send_byte
    lda #$21
    jmp send_byte

pause_short:
    ldy #$08
@outer:
    ldx #$FF
@inner:
    dex
    bne @inner
    dey
    bne @outer
    rts

; The 1541 will be left in reset-recoverable state if OneROM refuses a command.
boot_failed:
    sei
@failed:
    jmp @failed

cmd_group: .byte 0
cmd_cmd:   .byte 0
cmd_arg0:  .byte 0
cmd_arg1:  .byte 0
cmd_arg2:  .byte 0
cmd_arg3:  .byte 0
cmd_arg4:  .byte 0
cmd_arg5:  .byte 0
cmd_arg6:  .byte 0

.segment "VECTORS"
    .word reset_entry, reset_entry, reset_entry

