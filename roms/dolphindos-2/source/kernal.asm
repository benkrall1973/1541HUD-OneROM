; =============================================
; DolphinDOS 2 - C64 Kernal ROM
; ACME assembler format
; =============================================

!cpu 6502
* = $5684

    sta $56                                  ; $5684

    jsr $bc0f                                ; $5686
    lda $61                                  ; $5689
    cmp #$88                                 ; $568b
    bcc $5692                                ; $568d
    jsr $bad4                                ; $568f
    jsr $bccc                                ; $5692
    lda $07                                  ; $5695
    clc                                      ; $5697
    adc #$81                                 ; $5698
    beq $568f                                ; $569a
    sec                                      ; $569c
    sbc #$01                                 ; $569d
    pha                                      ; $569f
    ldx #$05                                 ; $56a0
    lda $69,x                                ; $56a2
    ldy $61,x                                ; $56a4
    sta $61,x                                ; $56a6
    sty $69,x                                ; $56a8
    dex                                      ; $56aa
    bpl $56a2                                ; $56ab
    lda $56                                  ; $56ad
    sta $70                                  ; $56af
    jsr $b853                                ; $56b1
    jsr $bfb4                                ; $56b4
    lda #$c4                                 ; $56b7
    ldy #$bf                                 ; $56b9
    jsr $e059                                ; $56bb
    lda #$00                                 ; $56be
    sta $6f                                  ; $56c0
    pla                                      ; $56c2
    jsr $bab9                                ; $56c3
    rts                                      ; $56c6
    sta $71                                  ; $56c7
    sty $72                                  ; $56c9
    jsr $bbca                                ; $56cb
    lda #$57                                 ; $56ce
    jsr $ba28                                ; $56d0
    jsr $e05d                                ; $56d3
    lda #$57                                 ; $56d6
    ldy #$00                                 ; $56d8
    jmp $ba28                                ; $56da
    sta $71                                  ; $56dd
    sty $72                                  ; $56df
    jsr $bbc7                                ; $56e1
    lda ($71),y                              ; $56e4
    sta $67                                  ; $56e6
    ldy $71                                  ; $56e8
    iny                                      ; $56ea
    tya                                      ; $56eb
    bne $56f0                                ; $56ec
    inc $72                                  ; $56ee
    sta $71                                  ; $56f0
    ldy $72                                  ; $56f2
    jsr $ba28                                ; $56f4
    lda $71                                  ; $56f7
    ldy $72                                  ; $56f9
    clc                                      ; $56fb
    adc #$05                                 ; $56fc
    bcc $5701                                ; $56fe
    iny                                      ; $5700
    sta $71                                  ; $5701
    sty $72                                  ; $5703
    jsr $b867                                ; $5705
    lda #$5c                                 ; $5708
    ldy #$00                                 ; $570a
    dec $67                                  ; $570c
    bne $56f4                                ; $570e
    rts                                      ; $5710
    tya                                      ; $5711
    and $44,x                                ; $5712
    !byte $7a                                ; $5714 (undefined opcode)
    brk                                      ; $5715
    pla                                      ; $5716
    plp                                      ; $5717
    lda ($46),y                              ; $5718
    brk                                      ; $571a
    jsr $bc2b                                ; $571b
    bmi $5757                                ; $571e
    bne $5742                                ; $5720
    jsr $fff3                                ; $5722
    stx $22                                  ; $5725
    sty $23                                  ; $5727
    ldy #$04                                 ; $5729
    lda ($22),y                              ; $572b
    sta $62                                  ; $572d
    iny                                      ; $572f
    lda ($22),y                              ; $5730
    sta $64                                  ; $5732
    ldy #$08                                 ; $5734
    lda ($22),y                              ; $5736
    sta $63                                  ; $5738
    iny                                      ; $573a
    lda ($22),y                              ; $573b
    sta $65                                  ; $573d
    jmp $e0e3                                ; $573f
    lda #$8b                                 ; $5742
    ldy #$00                                 ; $5744
    jsr $bba2                                ; $5746
    lda #$8d                                 ; $5749
    ldy #$e0                                 ; $574b
    jsr $ba28                                ; $574d
    lda #$92                                 ; $5750
    ldy #$e0                                 ; $5752
    jsr $b867                                ; $5754
    ldx $65                                  ; $5757
    lda $62                                  ; $5759
    sta $65                                  ; $575b
    stx $62                                  ; $575d
    ldx $63                                  ; $575f
    lda $64                                  ; $5761
    sta $63                                  ; $5763
    stx $64                                  ; $5765
    lda #$00                                 ; $5767
    sta $66                                  ; $5769
    lda $61                                  ; $576b
    sta $70                                  ; $576d
    lda #$80                                 ; $576f
    sta $61                                  ; $5771
    jsr $b8d7                                ; $5773
    ldx #$8b                                 ; $5776
    ldy #$00                                 ; $5778
    jmp $bbd4                                ; $577a
    tax                                      ; $577d
    bne $5782                                ; $577e
    ldx #$1e                                 ; $5780
    jmp $a437                                ; $5782
    pha                                      ; $5785
    lda $0294                                ; $5786
    beq $578c                                ; $5789
    tax                                      ; $578b
    pla                                      ; $578c
    jmp $ffba                                ; $578d
    jsr $ffd2                                ; $5790
    bcs $577d                                ; $5793
    rts                                      ; $5795
    jsr $ffcf                                ; $5796
    jmp $f775                                ; $5799
    jsr $e4ad                                ; $579c
    bcs $577d                                ; $579f
    rts                                      ; $57a1
    jsr $ffc6                                ; $57a2
    bcs $577d                                ; $57a5
    rts                                      ; $57a7
    jsr $ffe4                                ; $57a8
    bcs $577d                                ; $57ab
    rts                                      ; $57ad
    jmp $f2be                                ; $57ae
    jsr $b7f7                                ; $57b1
    lda #$e1                                 ; $57b4
    pha                                      ; $57b6
    lda #$46                                 ; $57b7
    pha                                      ; $57b9
    lda $030f                                ; $57ba
    pha                                      ; $57bd
    lda $030c                                ; $57be
    ldx $030d                                ; $57c1
    ldy $030e                                ; $57c4
    plp                                      ; $57c7
    jmp ($0014)                              ; $57c8
    php                                      ; $57cb
    sta $030c                                ; $57cc
    stx $030d                                ; $57cf
    sty $030e                                ; $57d2
    pla                                      ; $57d5
    sta $030f                                ; $57d6
    rts                                      ; $57d9
    jsr $e1d4                                ; $57da
    ldx $2d                                  ; $57dd
    ldy $2e                                  ; $57df
    lda #$2b                                 ; $57e1
    jsr $ffd8                                ; $57e3
    bcs $577d                                ; $57e6
    rts                                      ; $57e8
    lda #$01                                 ; $57e9
    bit $00a9                                ; $57eb
    sta $0a                                  ; $57ee
    jsr $e1d4                                ; $57f0
    lda $0a                                  ; $57f3
    ldx $2b                                  ; $57f5
    ldy $2c                                  ; $57f7
    jsr $ffd5                                ; $57f9
    bcs $5855                                ; $57fc
    lda $0a                                  ; $57fe
    beq $5819                                ; $5800
    ldx #$1c                                 ; $5802
    jsr $ffb7                                ; $5804
    and #$10                                 ; $5807
    bne $5822                                ; $5809
    lda $7a                                  ; $580b
    cmp #$02                                 ; $580d
    beq $5818                                ; $580f
    lda #$64                                 ; $5811
    ldy #$a3                                 ; $5813
    jmp $ab1e                                ; $5815
    rts                                      ; $5818
    jsr $ffb7                                ; $5819
    and #$bf                                 ; $581c
    beq $5825                                ; $581e
    ldx #$1d                                 ; $5820
    jmp $a437                                ; $5822
    lda $7b                                  ; $5825
    cmp #$02                                 ; $5827
    bne $5839                                ; $5829
    jmp $f58e                                ; $582b
    rol $76a9                                ; $582e
    ldy #$a3                                 ; $5831
    jsr $ab1e                                ; $5833
    jmp $a52a                                ; $5836
    jsr $a68e                                ; $5839
    jsr $a533                                ; $583c
    jmp $a677                                ; $583f
    jsr $e219                                ; $5842
    jsr $ffc0                                ; $5845
    bcs $5855                                ; $5848
    rts                                      ; $584a
    jsr $e219                                ; $584b
    lda $49                                  ; $584e
    jsr $ffc3                                ; $5850
    bcc $5818                                ; $5853
    jmp $e0f9                                ; $5855
    lda #$00                                 ; $5858
    jsr $ffbd                                ; $585a
    ldx #$08                                 ; $585d
    ldy #$01                                 ; $585f
    jsr $e101                                ; $5861
    jsr $e206                                ; $5864
    jsr $e257                                ; $5867
    jsr $f14a                                ; $586a
    jsr $e200                                ; $586d
    ldy #$00                                 ; $5870
    stx $49                                  ; $5872
    jsr $ffba                                ; $5874
    jsr $f14a                                ; $5877
    jsr $e200                                ; $587a
    txa                                      ; $587d
    tay                                      ; $587e
    ldx $49                                  ; $587f
    jmp $ffba                                ; $5881
    jsr $e20e                                ; $5884
    jmp $b79e                                ; $5887
    jsr $0079                                ; $588a
    bne $5891                                ; $588d
    pla                                      ; $588f
    pla                                      ; $5890
    rts                                      ; $5891
    jsr $aefd                                ; $5892
    jsr $0079                                ; $5895
    bne $5891                                ; $5898
    jmp $af08                                ; $589a
    lda #$00                                 ; $589d
    jsr $ffbd                                ; $589f
    jsr $e211                                ; $58a2
    jsr $b79e                                ; $58a5
    stx $49                                  ; $58a8
    txa                                      ; $58aa
    ldx #$08                                 ; $58ab
    ldy #$0f                                 ; $58ad
    jsr $e101                                ; $58af
    jsr $e206                                ; $58b2
    jsr $e200                                ; $58b5
    stx $4a                                  ; $58b8
    ldy #$00                                 ; $58ba
    lda $49                                  ; $58bc
    cpx #$03                                 ; $58be
    bne $58c3                                ; $58c0
    dey                                      ; $58c2
    jsr $ffba                                ; $58c3
    jsr $e206                                ; $58c6
    jsr $e200                                ; $58c9
    txa                                      ; $58cc
    tay                                      ; $58cd
    ldx $4a                                  ; $58ce
    lda $49                                  ; $58d0
    jsr $ffba                                ; $58d2
    jsr $e206                                ; $58d5
    jsr $e20e                                ; $58d8
    jsr $ad9e                                ; $58db
    jsr $b6a3                                ; $58de
    ldx $22                                  ; $58e1
    ldy $23                                  ; $58e3
    jmp $ffbd                                ; $58e5
    lda #$e0                                 ; $58e8
    ldy #$e2                                 ; $58ea
    jsr $b867                                ; $58ec
    jsr $bc0c                                ; $58ef
    lda #$e5                                 ; $58f2
    ldy #$e2                                 ; $58f4
    ldx $6e                                  ; $58f6
    jsr $bb07                                ; $58f8
    jsr $bc0c                                ; $58fb
    jsr $bccc                                ; $58fe
    lda #$00                                 ; $5901
    sta $6f                                  ; $5903
    jsr $b853                                ; $5905
    lda #$ea                                 ; $5908
    ldy #$e2                                 ; $590a
    jsr $b850                                ; $590c
    lda $66                                  ; $590f
    pha                                      ; $5911
    bpl $5921                                ; $5912
    jsr $b849                                ; $5914
    lda $66                                  ; $5917
    bmi $5924                                ; $5919
    lda $12                                  ; $591b
    eor #$ff                                 ; $591d
    sta $12                                  ; $591f
    jsr $bfb4                                ; $5921
    lda #$ea                                 ; $5924
    ldy #$e2                                 ; $5926
    jsr $b867                                ; $5928
    pla                                      ; $592b
    bpl $5931                                ; $592c
    jsr $bfb4                                ; $592e
    lda #$ef                                 ; $5931
    ldy #$e2                                 ; $5933
    jmp $e043                                ; $5935
    jsr $bbca                                ; $5938
    lda #$00                                 ; $593b
    sta $12                                  ; $593d
    jsr $e26b                                ; $593f
    ldx #$4e                                 ; $5942
    ldy #$00                                 ; $5944
    jsr $e0f6                                ; $5946
    lda #$57                                 ; $5949
    ldy #$00                                 ; $594b
    jsr $bba2                                ; $594d
    lda #$00                                 ; $5950
    sta $66                                  ; $5952
    lda $12                                  ; $5954
    jsr $e2dc                                ; $5956
    lda #$4e                                 ; $5959
    ldy #$00                                 ; $595b
    jmp $bb0f                                ; $595d
    pha                                      ; $5960
    jmp $e29d                                ; $5961
    sta ($49,x)                              ; $5964
    !byte $0f                                ; $5966 (undefined opcode)
    !byte $da                                ; $5967 (undefined opcode)
    ldx #$83                                 ; $5968
    eor #$0f                                 ; $596a
    !byte $da                                ; $596c (undefined opcode)
    ldx #$7f                                 ; $596d
    brk                                      ; $596f
    brk                                      ; $5970
    brk                                      ; $5971
    brk                                      ; $5972
    ora $84                                  ; $5973
    inc $1a                                  ; $5975
    and $861b                                ; $5977
    plp                                      ; $597a
    !byte $07                                ; $597b (undefined opcode)
    !byte $fb                                ; $597c (undefined opcode)
    sed                                      ; $597d
    !byte $87                                ; $597e (undefined opcode)
    sta $8968,y                              ; $597f
    ora ($87,x)                              ; $5982
    !byte $23                                ; $5984 (undefined opcode)
    and $df,x                                ; $5985
    sbc ($86,x)                              ; $5987
    lda $5d                                  ; $5989
    !byte $e7                                ; $598b (undefined opcode)
    plp                                      ; $598c
    !byte $83                                ; $598d (undefined opcode)
    eor #$0f                                 ; $598e
    !byte $da                                ; $5990 (undefined opcode)
    ldx #$a5                                 ; $5991
    ror $48                                  ; $5993
    bpl $599a                                ; $5995
    jsr $bfb4                                ; $5997
    lda $61                                  ; $599a
    pha                                      ; $599c
    cmp #$81                                 ; $599d
    bcc $59a8                                ; $599f
    lda #$bc                                 ; $59a1
    ldy #$b9                                 ; $59a3
    jsr $bb0f                                ; $59a5
    lda #$3e                                 ; $59a8
    ldy #$e3                                 ; $59aa
    jsr $e043                                ; $59ac
    pla                                      ; $59af
    cmp #$81                                 ; $59b0
    bcc $59bb                                ; $59b2
    lda #$e0                                 ; $59b4
    ldy #$e2                                 ; $59b6
    jsr $b850                                ; $59b8
    pla                                      ; $59bb
    bpl $59c1                                ; $59bc
    jmp $bfb4                                ; $59be
    rts                                      ; $59c1
    !byte $0b                                ; $59c2 (undefined opcode)
    ror $b3,x                                ; $59c3
    !byte $83                                ; $59c5 (undefined opcode)
    lda $79d3,x                              ; $59c6
    asl $a6f4,x                              ; $59c9
    sbc $7b,x                                ; $59cc
    !byte $83                                ; $59ce (undefined opcode)
    !byte $fc                                ; $59cf (undefined opcode)
    bcs $59e2                                ; $59d0
    !byte $7c                                ; $59d2 (undefined opcode)
    !byte $0c                                ; $59d3 (undefined opcode)
    !byte $1f                                ; $59d4 (undefined opcode)
    !byte $67                                ; $59d5 (undefined opcode)
    dex                                      ; $59d6
    !byte $7c                                ; $59d7 (undefined opcode)
    dec $cb53,x                              ; $59d8
    cmp ($7d,x)                              ; $59db
    !byte $14                                ; $59dd (undefined opcode)
    !byte $64                                ; $59de (undefined opcode)
    bvs $5a2d                                ; $59df
    adc $eab7,x                              ; $59e1
    eor ($7a),y                              ; $59e4
    adc $3063,x                              ; $59e6
    dey                                      ; $59e9
    ror $927e,x                              ; $59ea
    !byte $44                                ; $59ed (undefined opcode)
    sta $7e3a,y                              ; $59ee
    jmp $91cc                                ; $59f1
    !byte $c7                                ; $59f4 (undefined opcode)
    !byte $7f                                ; $59f5 (undefined opcode)
    tax                                      ; $59f6
    tax                                      ; $59f7
    tax                                      ; $59f8
    !byte $13                                ; $59f9 (undefined opcode)
    sta ($00,x)                              ; $59fa
    brk                                      ; $59fc
    brk                                      ; $59fd
    brk                                      ; $59fe
    jsr $ffcc                                ; $59ff
    lda #$00                                 ; $5a02
    sta $13                                  ; $5a04
    jsr $a67a                                ; $5a06
    cli                                      ; $5a09
    ldx #$80                                 ; $5a0a
    jmp ($0300)                              ; $5a0c
    txa                                      ; $5a0f
    bmi $5a15                                ; $5a10
    jmp $f79c                                ; $5a12
    jmp $a474                                ; $5a15
    jsr $e453                                ; $5a18
    jsr $e3bf                                ; $5a1b
    jsr $e422                                ; $5a1e
    ldx #$fb                                 ; $5a21
    txs                                      ; $5a23
    bne $5a0a                                ; $5a24
    inc $7a                                  ; $5a26
    bne $5a2c                                ; $5a28
    inc $7b                                  ; $5a2a
    lda $ea60                                ; $5a2c
    cmp #$3a                                 ; $5a2f
    bcs $5a3d                                ; $5a31
    cmp #$20                                 ; $5a33
    beq $5a26                                ; $5a35
    sec                                      ; $5a37
    sbc #$30                                 ; $5a38
    sec                                      ; $5a3a
    sbc #$d0                                 ; $5a3b
    rts                                      ; $5a3d
    !byte $80                                ; $5a3e (undefined opcode)
    !byte $4f                                ; $5a3f (undefined opcode)
    !byte $c7                                ; $5a40 (undefined opcode)
    !byte $52                                ; $5a41 (undefined opcode)
    cli                                      ; $5a42
    lda #$4c                                 ; $5a43
    sta $54                                  ; $5a45
    sta $0310                                ; $5a47
    lda #$48                                 ; $5a4a
    ldy #$b2                                 ; $5a4c
    sta $0311                                ; $5a4e
    sty $0312                                ; $5a51
    lda #$91                                 ; $5a54
    ldy #$b3                                 ; $5a56
    sta $05                                  ; $5a58
    sty $06                                  ; $5a5a
    lda #$aa                                 ; $5a5c
    ldy #$b1                                 ; $5a5e
    sta $03                                  ; $5a60
    sty $04                                  ; $5a62
    ldx #$1c                                 ; $5a64
    lda $e3a2,x                              ; $5a66
    sta $73,x                                ; $5a69
    dex                                      ; $5a6b
    bpl $5a66                                ; $5a6c
    lda #$03                                 ; $5a6e
    sta $53                                  ; $5a70
    lda #$00                                 ; $5a72
    sta $68                                  ; $5a74
    sta $13                                  ; $5a76
    sta $18                                  ; $5a78
    ldx #$01                                 ; $5a7a
    stx $01fd                                ; $5a7c
    stx $01fc                                ; $5a7f
    ldx #$19                                 ; $5a82
    stx $16                                  ; $5a84
    sec                                      ; $5a86
    jsr $ff9c                                ; $5a87
    stx $2b                                  ; $5a8a
    sty $2c                                  ; $5a8c
    sec                                      ; $5a8e
    jsr $ff99                                ; $5a8f
    stx $37                                  ; $5a92
    sty $38                                  ; $5a94
    stx $33                                  ; $5a96
    sty $34                                  ; $5a98
    ldy #$00                                 ; $5a9a
    tya                                      ; $5a9c
    sta ($2b),y                              ; $5a9d
    inc $2b                                  ; $5a9f
    bne $5aa5                                ; $5aa1
    inc $2c                                  ; $5aa3
    rts                                      ; $5aa5
    lda $2b                                  ; $5aa6
    ldy $2c                                  ; $5aa8
    jsr $a408                                ; $5aaa
    lda #$73                                 ; $5aad
    ldy #$e4                                 ; $5aaf
    jsr $ab1e                                ; $5ab1
    lda $37                                  ; $5ab4
    sec                                      ; $5ab6
    sbc $2b                                  ; $5ab7
    tax                                      ; $5ab9
    lda $38                                  ; $5aba
    sbc $2c                                  ; $5abc
    jsr $bdcd                                ; $5abe
    lda #$60                                 ; $5ac1
    ldy #$e4                                 ; $5ac3
    jsr $ab1e                                ; $5ac5
    jmp $a644                                ; $5ac8
    !byte $8b                                ; $5acb (undefined opcode)
    !byte $e3                                ; $5acc (undefined opcode)
    !byte $83                                ; $5acd (undefined opcode)
    ldy $7c                                  ; $5ace
    lda $1a                                  ; $5ad0
    !byte $a7                                ; $5ad2 (undefined opcode)
    cpx $a7                                  ; $5ad3
    stx $ae                                  ; $5ad5
    ldx #$0b                                 ; $5ad7
    lda $e447,x                              ; $5ad9
    sta $0300,x                              ; $5adc
    dex                                      ; $5adf
    bpl $5ad9                                ; $5ae0
    bmi $5b3b                                ; $5ae2
    jsr $4142                                ; $5ae4
    !byte $53                                ; $5ae7 (undefined opcode)
    eor #$43                                 ; $5ae8
    !byte $20,$42,$59                        ; $5aea (data: " BY")
    !byte $54                                ; $5aed (undefined opcode)
    eor $53                                  ; $5aee
    jsr $5246                                ; $5af0
    eor $45                                  ; $5af3
    ora $9300                                ; $5af5
    ora $2020                                ; $5af8
    jsr $2a20                                ; $5afb
    rol                                    ; $5afe
    rol                                    ; $5aff
    rol                                    ; $5b00
    jsr $4f43                                ; $5b01
    eor $4f4d                                ; $5b04
    !byte $44                                ; $5b07 (undefined opcode)
    !byte $4f                                ; $5b08 (undefined opcode)
    !byte $52                                ; $5b09 (undefined opcode)
    eor $20                                  ; $5b0a
    rol $34,x                                ; $5b0c
    jsr $4142                                ; $5b0e
    !byte $53                                ; $5b11 (undefined opcode)
    eor #$43                                 ; $5b12
    jsr $3256                                ; $5b14
    jsr $2a2a                                ; $5b17
    rol                                    ; $5b1a
    rol                                    ; $5b1b
    ora $200d                                ; $5b1c
    !byte $44                                ; $5b1f (undefined opcode)
    !byte $4f                                ; $5b20 (undefined opcode)
    jmp $4850                                ; $5b21
    eor #$4e                                 ; $5b24
    !byte $44                                ; $5b26 (undefined opcode)
    !byte $4f                                ; $5b27 (undefined opcode)
    !byte $53                                ; $5b28 (undefined opcode)
    jsr $2e32                                ; $5b29
    bmi $5b4e                                ; $5b2c
    jsr $8100                                ; $5b2e
    pha                                      ; $5b31
    jsr $ffc9                                ; $5b32
    tax                                      ; $5b35
    pla                                      ; $5b36
    bcc $5b3a                                ; $5b37
    txa                                      ; $5b39
    rts                                      ; $5b3a
    lda #$87                                 ; $5b3b
    ldx #$f3                                 ; $5b3d
    sta $0297                                ; $5b3f
    stx $0298                                ; $5b42
    rts                                      ; $5b45
    sei                                      ; $5b46
    cld                                      ; $5b47
    txs                                      ; $5b48
    inx                                      ; $5b49
    txa                                      ; $5b4a
    sta $01                                  ; $5b4b
    sta $0100,x                              ; $5b4d
    inx                                      ; $5b50
    bne $5b4d                                ; $5b51
    inc $f1                                  ; $5b53
    bne $5b4d                                ; $5b55
    lda #$37                                 ; $5b57
    sta $01                                  ; $5b59
    jmp $fcef                                ; $5b5b
    lda $0286                                ; $5b5e
    sta ($f3),y                              ; $5b61
    rts                                      ; $5b63
    adc #$02                                 ; $5b64
    ldy $91                                  ; $5b66
    iny                                      ; $5b68
    bne $5b6f                                ; $5b69
    cmp $a1                                  ; $5b6b
    bne $5b66                                ; $5b6d
    rts                                      ; $5b6f
; =============================================================================
; PARALLEL_SEND_BYTE - Send one byte via parallel port
; Entry: A = byte to send
; Uses: $DD01 (parallel data), $DD00 (handshake via CLK line bit 2)
; =============================================================================
    sta $dd01      ; Write byte to parallel data port (directly to 1541) ; $5b70
    lda $dd0d      ; Read CIA#2 ICR to clear any pending FLAG interrupt ; $5b73
    lda $dd00      ; Read current port A state ; $5b76
    and #$fb       ; Clear bit 2 (CLK OUT low = "data ready") ; $5b79
    sta $dd00      ; Signal to drive: byte is ready on port ; $5b7b
    ora #$04       ; Set bit 2 (CLK OUT high = "idle/done") ; $5b7e
    sta $dd00      ; Complete handshake cycle ; $5b80
    rts                                      ; $5b83
    ldx #$00                                 ; $5b84
    ldy #$dc                                 ; $5b86
    rts                                      ; $5b88
    ldx #$28                                 ; $5b89
    ldy #$19                                 ; $5b8b
    rts                                      ; $5b8d
    bcs $5b97                                ; $5b8e
    stx $d6                                  ; $5b90
    sty $d3                                  ; $5b92
    jsr $e56c                                ; $5b94
    ldx $d6                                  ; $5b97
    ldy $d3                                  ; $5b99
    rts                                      ; $5b9b
    jsr $e5a0                                ; $5b9c
    lda #$00                                 ; $5b9f
    sta $0291                                ; $5ba1
    sta $cf                                  ; $5ba4
    lda #$48                                 ; $5ba6
    sta $028f                                ; $5ba8
    lda #$eb                                 ; $5bab
    sta $0290                                ; $5bad
    lda #$04                                 ; $5bb0
    sta $028b                                ; $5bb2
    sta $0288                                ; $5bb5
    lda #$01                                 ; $5bb8
    sta $0286                                ; $5bba
    lda #$0a                                 ; $5bbd
    sta $0289                                ; $5bbf
    lda #$0c                                 ; $5bc2
    sta $cd                                  ; $5bc4
    sta $cc                                  ; $5bc6
    lda $0288                                ; $5bc8
    ora #$80                                 ; $5bcb
    tay                                      ; $5bcd
    lda #$00                                 ; $5bce
    tax                                      ; $5bd0
    sty $d9,x                                ; $5bd1
    clc                                      ; $5bd3
    adc #$28                                 ; $5bd4
    bcc $5bd9                                ; $5bd6
    iny                                      ; $5bd8
    inx                                      ; $5bd9
    cpx #$1a                                 ; $5bda
    bne $5bd1                                ; $5bdc
    lda #$ff                                 ; $5bde
    sta $d9,x                                ; $5be0
    ldx #$18                                 ; $5be2
    jsr $e9ff                                ; $5be4
    dex                                      ; $5be7
    bpl $5be4                                ; $5be8
    ldy #$00                                 ; $5bea
    sty $d3                                  ; $5bec
    sty $d6                                  ; $5bee
    ldx $d6                                  ; $5bf0
    lda $d3                                  ; $5bf2
    ldy $d9,x                                ; $5bf4
    bmi $5c00                                ; $5bf6
    clc                                      ; $5bf8
    adc #$28                                 ; $5bf9
    sta $d3                                  ; $5bfb
    dex                                      ; $5bfd
    bpl $5bf4                                ; $5bfe
    jsr $e9f0                                ; $5c00
    lda #$27                                 ; $5c03
    inx                                      ; $5c05
    ldy $d9,x                                ; $5c06
    bmi $5c10                                ; $5c08
    clc                                      ; $5c0a
    adc #$28                                 ; $5c0b
    inx                                      ; $5c0d
    bpl $5c06                                ; $5c0e
    sta $d5                                  ; $5c10
    jmp $ea24                                ; $5c12
    jmp $fba9                                ; $5c15
    jmp $e101                                ; $5c18
    jmp $f894                                ; $5c1b
    jsr $e5a0                                ; $5c1e
    jmp $e566                                ; $5c21
    lda #$03                                 ; $5c24
    sta $9a                                  ; $5c26
    lda #$00                                 ; $5c28
    sta $99                                  ; $5c2a
    ldx #$2f                                 ; $5c2c
    lda $ecb8,x                              ; $5c2e
    sta $cfff,x                              ; $5c31
    dex                                      ; $5c34
    bne $5c2e                                ; $5c35
    rts                                      ; $5c37
    ldy $0277                                ; $5c38
    ldx #$00                                 ; $5c3b
    lda $0278,x                              ; $5c3d
    sta $0277,x                              ; $5c40
    inx                                      ; $5c43
    cpx $c6                                  ; $5c44
    bne $5c3d                                ; $5c46
    dec $c6                                  ; $5c48
    tya                                      ; $5c4a
    cli                                      ; $5c4b
    clc                                      ; $5c4c
    rts                                      ; $5c4d
    jsr $e716                                ; $5c4e
    lda $c6                                  ; $5c51
    sta $cc                                  ; $5c53
    sta $0292                                ; $5c55
    beq $5c51                                ; $5c58
    sei                                      ; $5c5a
    lda $cf                                  ; $5c5b
    beq $5c6b                                ; $5c5d
    lda $ce                                  ; $5c5f
    ldx $0287                                ; $5c61
    ldy #$00                                 ; $5c64
    sty $cf                                  ; $5c66
    jsr $ea13                                ; $5c68
    jsr $f533                                ; $5c6b
    cmp #$83                                 ; $5c6e
    bne $5c82                                ; $5c70
    ldx #$09                                 ; $5c72
    sei                                      ; $5c74
    stx $c6                                  ; $5c75
    lda $ece6,x                              ; $5c77
    sta $0276,x                              ; $5c7a
    dex                                      ; $5c7d
    bne $5c77                                ; $5c7e
    beq $5c51                                ; $5c80
    cmp #$0d                                 ; $5c82
    bne $5c4e                                ; $5c84
    ldy $d5                                  ; $5c86
    sty $d0                                  ; $5c88
    lda ($d1),y                              ; $5c8a
    cmp #$20                                 ; $5c8c
    bne $5c93                                ; $5c8e
    dey                                      ; $5c90
    bne $5c8a                                ; $5c91
    iny                                      ; $5c93
    sty $c8                                  ; $5c94
    ldy #$00                                 ; $5c96
    sty $0292                                ; $5c98
    sty $d3                                  ; $5c9b
    sty $d4                                  ; $5c9d
    lda $c9                                  ; $5c9f
    bmi $5cbe                                ; $5ca1
    ldx $d6                                  ; $5ca3
    jsr $fd9c                                ; $5ca5
    cpx $c9                                  ; $5ca8
    bne $5cbe                                ; $5caa
    lda $ca                                  ; $5cac
    sta $d3                                  ; $5cae
    cmp $c8                                  ; $5cb0
    bcc $5cbe                                ; $5cb2
    bcs $5ce1                                ; $5cb4
    tya                                      ; $5cb6
    pha                                      ; $5cb7
    txa                                      ; $5cb8
    pha                                      ; $5cb9
    lda $d0                                  ; $5cba
    beq $5c51                                ; $5cbc
    ldy $d3                                  ; $5cbe
    lda ($d1),y                              ; $5cc0
    sta $d7                                  ; $5cc2
    and #$3f                                 ; $5cc4
    asl $d7                                  ; $5cc6
    bit $d7                                  ; $5cc8
    bpl $5cce                                ; $5cca
    ora #$80                                 ; $5ccc
    bcc $5cd4                                ; $5cce
    ldx $d4                                  ; $5cd0
    bne $5cd8                                ; $5cd2
    bvs $5cd8                                ; $5cd4
    ora #$40                                 ; $5cd6
    inc $d3                                  ; $5cd8
    jsr $e684                                ; $5cda
    cpy $c8                                  ; $5cdd
    bne $5cf8                                ; $5cdf
    lda #$00                                 ; $5ce1
    sta $d0                                  ; $5ce3
    lda #$0d                                 ; $5ce5
    ldx $99                                  ; $5ce7
    cpx #$03                                 ; $5ce9
    beq $5cf3                                ; $5ceb
    ldx $9a                                  ; $5ced
    cpx #$03                                 ; $5cef
    beq $5cf6                                ; $5cf1
    jsr $e716                                ; $5cf3
    lda #$0d                                 ; $5cf6
    sta $d7                                  ; $5cf8
    pla                                      ; $5cfa
    tax                                      ; $5cfb
    pla                                      ; $5cfc
    tay                                      ; $5cfd
    lda $d7                                  ; $5cfe
    cmp #$de                                 ; $5d00
    bne $5d06                                ; $5d02
    lda #$ff                                 ; $5d04
    clc                                      ; $5d06
    rts                                      ; $5d07
    cmp #$22                                 ; $5d08
    bne $5d14                                ; $5d0a
    lda $d4                                  ; $5d0c
    eor #$01                                 ; $5d0e
    sta $d4                                  ; $5d10
    lda #$22                                 ; $5d12
    rts                                      ; $5d14
    ora #$40                                 ; $5d15
    ldx $c7                                  ; $5d17
    beq $5d1d                                ; $5d19
    ora #$80                                 ; $5d1b
    ldx $d8                                  ; $5d1d
    beq $5d23                                ; $5d1f
    lsr $d4                                  ; $5d21
    ldx $0286                                ; $5d23
    jsr $ea13                                ; $5d26
    jsr $e6b6                                ; $5d29
    lda $d8                                  ; $5d2c
    beq $5d32                                ; $5d2e
    dec $d8                                  ; $5d30
    pla                                      ; $5d32
    tay                                      ; $5d33
    pla                                      ; $5d34
    tax                                      ; $5d35
    pla                                      ; $5d36
    clc                                      ; $5d37
    cli                                      ; $5d38
    rts                                      ; $5d39
    jsr $e8b3                                ; $5d3a
    inc $d3                                  ; $5d3d
    lda $d5                                  ; $5d3f
    cmp $d3                                  ; $5d41
    bcs $5d84                                ; $5d43
    cmp #$4f                                 ; $5d45
    beq $5d7b                                ; $5d47
    lda $0292                                ; $5d49
    beq $5d51                                ; $5d4c
    jmp $e967                                ; $5d4e
    ldx $d6                                  ; $5d51
    cpx #$19                                 ; $5d53
    bcc $5d5e                                ; $5d55
    jsr $e8ea                                ; $5d57
    dec $d6                                  ; $5d5a
    ldx $d6                                  ; $5d5c
    asl $d9,x                                ; $5d5e
    lsr $d9,x                                ; $5d60
    inx                                      ; $5d62
    lda $d9,x                                ; $5d63
    ora #$80                                 ; $5d65
    sta $d9,x                                ; $5d67
    dex                                      ; $5d69
    lda $d5                                  ; $5d6a
    clc                                      ; $5d6c
    adc #$28                                 ; $5d6d
    sta $d5                                  ; $5d6f
    lda $d9,x                                ; $5d71
    bmi $5d78                                ; $5d73
    dex                                      ; $5d75
    bne $5d71                                ; $5d76
    jmp $e9f0                                ; $5d78
    dec $d6                                  ; $5d7b
    jsr $e87c                                ; $5d7d
    lda #$00                                 ; $5d80
    sta $d3                                  ; $5d82
    rts                                      ; $5d84
    ldx $d6                                  ; $5d85
    bne $5d8f                                ; $5d87
    stx $d3                                  ; $5d89
    pla                                      ; $5d8b
    pla                                      ; $5d8c
    bne $5d2c                                ; $5d8d
    dex                                      ; $5d8f
    stx $d6                                  ; $5d90
    jsr $e56c                                ; $5d92
    ldy $d5                                  ; $5d95
    sty $d3                                  ; $5d97
    rts                                      ; $5d99
    pha                                      ; $5d9a
    sta $d7                                  ; $5d9b
    txa                                      ; $5d9d
    pha                                      ; $5d9e
    tya                                      ; $5d9f
    pha                                      ; $5da0
    lda #$00                                 ; $5da1
    sta $d0                                  ; $5da3
    ldy $d3                                  ; $5da5
    lda $d7                                  ; $5da7
    bpl $5dae                                ; $5da9
    jmp $e7d4                                ; $5dab
    cmp #$0d                                 ; $5dae
    bne $5db5                                ; $5db0
    jmp $e891                                ; $5db2
    cmp #$20                                 ; $5db5
    bcc $5dc9                                ; $5db7
    cmp #$60                                 ; $5db9
    bcc $5dc1                                ; $5dbb
    and #$df                                 ; $5dbd
    bne $5dc3                                ; $5dbf
    and #$3f                                 ; $5dc1
    jsr $e684                                ; $5dc3
    jmp $e693                                ; $5dc6
    ldx $d8                                  ; $5dc9
    beq $5dd0                                ; $5dcb
    jmp $e697                                ; $5dcd
    cmp #$14                                 ; $5dd0
    bne $5e02                                ; $5dd2
    tya                                      ; $5dd4
    bne $5ddd                                ; $5dd5
    jsr $e701                                ; $5dd7
    jmp $e773                                ; $5dda
    jsr $e8a1                                ; $5ddd
    dey                                      ; $5de0
    sty $d3                                  ; $5de1
    jsr $ea24                                ; $5de3
    iny                                      ; $5de6
    lda ($d1),y                              ; $5de7
    dey                                      ; $5de9
    sta ($d1),y                              ; $5dea
    iny                                      ; $5dec
    lda ($f3),y                              ; $5ded
    dey                                      ; $5def
    sta ($f3),y                              ; $5df0
    iny                                      ; $5df2
    cpy $d5                                  ; $5df3
    bne $5de6                                ; $5df5
    lda #$20                                 ; $5df7
    sta ($d1),y                              ; $5df9
    lda $0286                                ; $5dfb
    sta ($f3),y                              ; $5dfe
    bpl $5e4f                                ; $5e00
    ldx $d4                                  ; $5e02
    beq $5e09                                ; $5e04
    jmp $e697                                ; $5e06
    cmp #$12                                 ; $5e09
    bne $5e0f                                ; $5e0b
    sta $c7                                  ; $5e0d
    cmp #$13                                 ; $5e0f
    bne $5e16                                ; $5e11
    jsr $e566                                ; $5e13
    cmp #$1d                                 ; $5e16
    bne $5e31                                ; $5e18
    iny                                      ; $5e1a
    jsr $e8b3                                ; $5e1b
    sty $d3                                  ; $5e1e
    dey                                      ; $5e20
    cpy $d5                                  ; $5e21
    bcc $5e2e                                ; $5e23
    dec $d6                                  ; $5e25
    jsr $e87c                                ; $5e27
    ldy #$00                                 ; $5e2a
    sty $d3                                  ; $5e2c
    jmp $e6ae                                ; $5e2e
    cmp #$11                                 ; $5e31
    bne $5e52                                ; $5e33
    clc                                      ; $5e35
    tya                                      ; $5e36
    adc #$28                                 ; $5e37
    tay                                      ; $5e39
    inc $d6                                  ; $5e3a
    cmp $d5                                  ; $5e3c
    bcc $5e2c                                ; $5e3e
    beq $5e2c                                ; $5e40
    dec $d6                                  ; $5e42
    sbc #$28                                 ; $5e44
    bcc $5e4c                                ; $5e46
    sta $d3                                  ; $5e48
    bne $5e44                                ; $5e4a
    jsr $e87c                                ; $5e4c
    jmp $e6ae                                ; $5e4f
    jsr $e8cb                                ; $5e52
    jmp $fa6b                                ; $5e55
    and #$7f                                 ; $5e58
    cmp #$7f                                 ; $5e5a
    bne $5e60                                ; $5e5c
    lda #$5e                                 ; $5e5e
    cmp #$20                                 ; $5e60
    bcc $5e67                                ; $5e62
    jmp $e691                                ; $5e64
    cmp #$0d                                 ; $5e67
    bne $5e6e                                ; $5e69
    jmp $e891                                ; $5e6b
    ldx $d4                                  ; $5e6e
    bne $5eb1                                ; $5e70
    cmp #$14                                 ; $5e72
    bne $5ead                                ; $5e74
    ldy $d5                                  ; $5e76
    lda ($d1),y                              ; $5e78
    cmp #$20                                 ; $5e7a
    bne $5e82                                ; $5e7c
    cpy $d3                                  ; $5e7e
    bne $5e89                                ; $5e80
    cpy #$4f                                 ; $5e82
    beq $5eaa                                ; $5e84
    jsr $e965                                ; $5e86
    ldy $d5                                  ; $5e89
    jsr $ea24                                ; $5e8b
    dey                                      ; $5e8e
    lda ($d1),y                              ; $5e8f
    iny                                      ; $5e91
    sta ($d1),y                              ; $5e92
    dey                                      ; $5e94
    lda ($f3),y                              ; $5e95
    iny                                      ; $5e97
    sta ($f3),y                              ; $5e98
    dey                                      ; $5e9a
    cpy $d3                                  ; $5e9b
    bne $5e8e                                ; $5e9d
    lda #$20                                 ; $5e9f
    sta ($d1),y                              ; $5ea1
    lda $0286                                ; $5ea3
    sta ($f3),y                              ; $5ea6
    inc $d8                                  ; $5ea8
    jmp $e6ae                                ; $5eaa
    ldx $d8                                  ; $5ead
    beq $5eb6                                ; $5eaf
    ora #$40                                 ; $5eb1
    jmp $e697                                ; $5eb3
    cmp #$11                                 ; $5eb6
    bne $5ed0                                ; $5eb8
    ldx $d6                                  ; $5eba
    beq $5ef5                                ; $5ebc
    dec $d6                                  ; $5ebe
    lda $d3                                  ; $5ec0
    sec                                      ; $5ec2
    sbc #$28                                 ; $5ec3
    bcc $5ecb                                ; $5ec5
    sta $d3                                  ; $5ec7
    bpl $5ef5                                ; $5ec9
    jsr $e56c                                ; $5ecb
    bne $5ef5                                ; $5ece
    cmp #$12                                 ; $5ed0
    bne $5ed8                                ; $5ed2
    lda #$00                                 ; $5ed4
    sta $c7                                  ; $5ed6
    cmp #$1d                                 ; $5ed8
    bne $5eee                                ; $5eda
    tya                                      ; $5edc
    beq $5ee8                                ; $5edd
    jsr $e8a1                                ; $5edf
    dey                                      ; $5ee2
    sty $d3                                  ; $5ee3
    jmp $e6ae                                ; $5ee5
    jsr $e701                                ; $5ee8
    jmp $e6ae                                ; $5eeb
    cmp #$13                                 ; $5eee
    bne $5ef8                                ; $5ef0
    jsr $e544                                ; $5ef2
    jmp $e6ae                                ; $5ef5
    ora #$80                                 ; $5ef8
    jsr $e8cb                                ; $5efa
    jmp $ec4f                                ; $5efd
    lsr $c9                                  ; $5f00
    ldx $d6                                  ; $5f02
    inx                                      ; $5f04
    cpx #$19                                 ; $5f05
    bne $5f0c                                ; $5f07
    jsr $e8ea                                ; $5f09
    lda $d9,x                                ; $5f0c
    bpl $5f04                                ; $5f0e
    stx $d6                                  ; $5f10
    jmp $e56c                                ; $5f12
    ldx #$00                                 ; $5f15
    stx $d8                                  ; $5f17
    stx $c7                                  ; $5f19
    stx $d4                                  ; $5f1b
    stx $d3                                  ; $5f1d
    jsr $e87c                                ; $5f1f
    jmp $e6ae                                ; $5f22
    ldx #$02                                 ; $5f25
    lda #$00                                 ; $5f27
    cmp $d3                                  ; $5f29
    beq $5f34                                ; $5f2b
    clc                                      ; $5f2d
    adc #$28                                 ; $5f2e
    dex                                      ; $5f30
    bne $5f29                                ; $5f31
    rts                                      ; $5f33
    dec $d6                                  ; $5f34
    rts                                      ; $5f36
    ldx #$02                                 ; $5f37
    lda #$27                                 ; $5f39
    cmp $d3                                  ; $5f3b
    beq $5f46                                ; $5f3d
    clc                                      ; $5f3f
    adc #$28                                 ; $5f40
    dex                                      ; $5f42
    bne $5f3b                                ; $5f43
    rts                                      ; $5f45
    ldx $d6                                  ; $5f46
    cpx #$19                                 ; $5f48
    beq $5f4e                                ; $5f4a
    inc $d6                                  ; $5f4c
    rts                                      ; $5f4e
    ldx #$0f                                 ; $5f4f
    cmp $e8da,x                              ; $5f51
    beq $5f5a                                ; $5f54
    dex                                      ; $5f56
    bpl $5f51                                ; $5f57
    rts                                      ; $5f59
    stx $0286                                ; $5f5a
    rts                                      ; $5f5d
    bcc $5f65                                ; $5f5e
    !byte $1c                                ; $5f60 (undefined opcode)
    !byte $9f                                ; $5f61 (undefined opcode)
    !byte $9c                                ; $5f62 (undefined opcode)
    asl $9e1f,x                              ; $5f63
    sta ($95,x)                              ; $5f66
    stx $97,y                                ; $5f68
    tya                                      ; $5f6a
    sta $9b9a,y                              ; $5f6b
    lda $ac                                  ; $5f6e
    pha                                      ; $5f70
    lda $ad                                  ; $5f71
    pha                                      ; $5f73
    lda $ae                                  ; $5f74
    pha                                      ; $5f76
    lda $af                                  ; $5f77
    pha                                      ; $5f79
    ldx #$ff                                 ; $5f7a
    dec $d6                                  ; $5f7c
    dec $c9                                  ; $5f7e
    lda #$7f                                 ; $5f80
    sta $dc00                                ; $5f82
    lda $dc01                                ; $5f85
    and #$24                                 ; $5f88
    eor #$24                                 ; $5f8a
    beq $5fa2                                ; $5f8c
    eor #$04                                 ; $5f8e
    beq $5f85                                ; $5f90
    lda $dc01                                ; $5f92
    and #$24                                 ; $5f95
    eor #$04                                 ; $5f97
    bne $5f92                                ; $5f99
    sta $c6                                  ; $5f9b
    lda $aabd                                ; $5f9d
    cmp ($cf,x)                              ; $5fa0
    inx                                      ; $5fa2
    jsr $e9f0                                ; $5fa3
    cpx #$18                                 ; $5fa6
    bcs $5fb6                                ; $5fa8
    lda $ecf1,x                              ; $5faa
    sta $ac                                  ; $5fad
    lda $da,x                                ; $5faf
    jsr $e9c8                                ; $5fb1
    bmi $5fa2                                ; $5fb4
    jsr $e9ff                                ; $5fb6
    ldx #$00                                 ; $5fb9
    lda $d9,x                                ; $5fbb
    and #$7f                                 ; $5fbd
    ldy $da,x                                ; $5fbf
    bpl $5fc5                                ; $5fc1
    ora #$80                                 ; $5fc3
    sta $d9,x                                ; $5fc5
    inx                                      ; $5fc7
    cpx #$18                                 ; $5fc8
    bne $5fbb                                ; $5fca
    lda $f1                                  ; $5fcc
    ora #$80                                 ; $5fce
    sta $f1                                  ; $5fd0
    lda $d9                                  ; $5fd2
    ora #$80                                 ; $5fd4
    sta $d9                                  ; $5fd6
    inc $d6                                  ; $5fd8
    ldx $d6                                  ; $5fda
    pla                                      ; $5fdc
    sta $af                                  ; $5fdd
    pla                                      ; $5fdf
    sta $ae                                  ; $5fe0
    pla                                      ; $5fe2
    sta $ad                                  ; $5fe3
    pla                                      ; $5fe5
    sta $ac                                  ; $5fe6
    rts                                      ; $5fe8
    ldx $d6                                  ; $5fe9
    inx                                      ; $5feb
    lda $d9,x                                ; $5fec
    bpl $5feb                                ; $5fee
    stx $02a5                                ; $5ff0
    cpx #$18                                 ; $5ff3
    beq $6005                                ; $5ff5
    bcc $6005                                ; $5ff7
    jsr $e8ea                                ; $5ff9
    ldx $02a5                                ; $5ffc
    dex                                      ; $5fff
    dec $d6                                  ; $6000
    jmp $e6da                                ; $6002
    lda $ac                                  ; $6005
    pha                                      ; $6007
    lda $ad                                  ; $6008
    pha                                      ; $600a
    lda $ae                                  ; $600b
    pha                                      ; $600d
    lda $af                                  ; $600e
    pha                                      ; $6010
    ldx #$19                                 ; $6011
    dex                                      ; $6013
    jsr $e9f0                                ; $6014
    cpx $02a5                                ; $6017
    bcc $602a                                ; $601a
    beq $602a                                ; $601c
    lda $ecef,x                              ; $601e
    sta $ac                                  ; $6021
    lda $d8,x                                ; $6023
    jsr $e9c8                                ; $6025
    bmi $6013                                ; $6028
    jsr $e9ff                                ; $602a
    ldx #$17                                 ; $602d
    cpx $02a5                                ; $602f
    bcc $6043                                ; $6032
    lda $da,x                                ; $6034
    and #$7f                                 ; $6036
    ldy $d9,x                                ; $6038
    bpl $603e                                ; $603a
    ora #$80                                 ; $603c
    sta $da,x                                ; $603e
    dex                                      ; $6040
    bne $602f                                ; $6041
    ldx $02a5                                ; $6043
    jsr $e6da                                ; $6046
    jmp $e958                                ; $6049
    and #$03                                 ; $604c
    ora $0288                                ; $604e
    sta $ad                                  ; $6051
    jsr $e9e0                                ; $6053
    ldy #$27                                 ; $6056
    lda ($ac),y                              ; $6058
    sta ($d1),y                              ; $605a
    lda ($ae),y                              ; $605c
    sta ($f3),y                              ; $605e
    dey                                      ; $6060
    bpl $6058                                ; $6061
    rts                                      ; $6063
    jsr $ea24                                ; $6064
    lda $ac                                  ; $6067
    sta $ae                                  ; $6069
    lda $ad                                  ; $606b
    and #$03                                 ; $606d
    ora #$d8                                 ; $606f
    sta $af                                  ; $6071
    rts                                      ; $6073
    lda $ecf0,x                              ; $6074
    sta $d1                                  ; $6077
    lda $d9,x                                ; $6079
    and #$03                                 ; $607b
    ora $0288                                ; $607d
    sta $d2                                  ; $6080
    rts                                      ; $6082
    ldy #$27                                 ; $6083
    jsr $e9f0                                ; $6085
    jsr $ea24                                ; $6088
    jsr $e4da                                ; $608b
    lda #$20                                 ; $608e
    sta ($d1),y                              ; $6090
    dey                                      ; $6092
    bpl $608b                                ; $6093
    rts                                      ; $6095
    nop                                      ; $6096
    tay                                      ; $6097
    lda #$02                                 ; $6098
    sta $cd                                  ; $609a
    jsr $ea24                                ; $609c
    tya                                      ; $609f
    ldy $d3                                  ; $60a0
    sta ($d1),y                              ; $60a2
    txa                                      ; $60a4
    sta ($f3),y                              ; $60a5
    rts                                      ; $60a7
    lda $d1                                  ; $60a8
    sta $f3                                  ; $60aa
    lda $d2                                  ; $60ac
    and #$03                                 ; $60ae
    ora #$d8                                 ; $60b0
    sta $f4                                  ; $60b2
    rts                                      ; $60b4
    jsr $ffea                                ; $60b5
    lda $cc                                  ; $60b8
    bne $60e5                                ; $60ba
    dec $cd                                  ; $60bc
    bne $60e5                                ; $60be
    lda #$14                                 ; $60c0
    sta $cd                                  ; $60c2
    ldy $d3                                  ; $60c4
    lsr $cf                                  ; $60c6
    ldx $0287                                ; $60c8
    lda ($d1),y                              ; $60cb
    bcs $60e0                                ; $60cd
    inc $cf                                  ; $60cf
    sta $ce                                  ; $60d1
    jsr $ea24                                ; $60d3
    lda ($f3),y                              ; $60d6
    sta $0287                                ; $60d8
    ldx $0286                                ; $60db
    lda $ce                                  ; $60de
    eor #$80                                 ; $60e0
    jsr $ea1c                                ; $60e2
    lda $01                                  ; $60e5
    and #$10                                 ; $60e7
    beq $60f5                                ; $60e9
    ldy #$00                                 ; $60eb
    sty $c0                                  ; $60ed
    lda $01                                  ; $60ef
    ora #$20                                 ; $60f1
    bne $60fd                                ; $60f3
    lda $c0                                  ; $60f5
    bne $60ff                                ; $60f7
    lda $01                                  ; $60f9
    and #$1f                                 ; $60fb
    sta $01                                  ; $60fd
    jsr $ea87                                ; $60ff
    lda $dc0d                                ; $6102
    pla                                      ; $6105
    tay                                      ; $6106
    pla                                      ; $6107
    tax                                      ; $6108
    pla                                      ; $6109
    rti                                      ; $610a
    lda #$00                                 ; $610b
    sta $028d                                ; $610d
    ldy #$40                                 ; $6110
    sty $cb                                  ; $6112
    sta $dc00                                ; $6114
    ldx $dc01                                ; $6117
    cpx #$ff                                 ; $611a
    beq $617f                                ; $611c
    tay                                      ; $611e
    lda #$81                                 ; $611f
    sta $f5                                  ; $6121
    lda #$eb                                 ; $6123
    sta $f6                                  ; $6125
    lda #$fe                                 ; $6127
    sta $dc00                                ; $6129
    ldx #$08                                 ; $612c
    pha                                      ; $612e
    lda $dc01                                ; $612f
    cmp $dc01                                ; $6132
    bne $612f                                ; $6135
    lsr                                    ; $6137
    bcs $6150                                ; $6138
    pha                                      ; $613a
    lda ($f5),y                              ; $613b
    cmp #$05                                 ; $613d
    bcs $614d                                ; $613f
    cmp #$03                                 ; $6141
    beq $614d                                ; $6143
    ora $028d                                ; $6145
    sta $028d                                ; $6148
    bpl $614f                                ; $614b
    sty $cb                                  ; $614d
    pla                                      ; $614f
    iny                                      ; $6150
    cpy #$41                                 ; $6151
    bcs $6160                                ; $6153
    dex                                      ; $6155
    bne $6137                                ; $6156
    sec                                      ; $6158
    pla                                      ; $6159
    rol                                    ; $615a
    sta $dc00                                ; $615b
    bne $612c                                ; $615e
    pla                                      ; $6160
    jmp ($028f)                              ; $6161
    ldy $cb                                  ; $6164
    lda ($f5),y                              ; $6166
    tax                                      ; $6168
    cpy $c5                                  ; $6169
    beq $6174                                ; $616b
    lda #$10                                 ; $616d
    jsr $fb11                                ; $616f
    bne $61aa                                ; $6172
    and #$7f                                 ; $6174
    bit $028a                                ; $6176
    bmi $6191                                ; $6179
    bvs $61c6                                ; $617b
    cmp #$7f                                 ; $617d
    beq $61aa                                ; $617f
    cmp #$0f                                 ; $6181
    bcc $6189                                ; $6183
    cmp #$15                                 ; $6185
    bcc $6191                                ; $6187
    cmp #$1d                                 ; $6189
    beq $6191                                ; $618b
    cmp #$20                                 ; $618d
    bne $61c6                                ; $618f
    ldy $028c                                ; $6191
    beq $619b                                ; $6194
    dec $028c                                ; $6196
    bne $61c6                                ; $6199
    dec $028b                                ; $619b
    bne $61c6                                ; $619e
    ldy #$04                                 ; $61a0
    sty $028b                                ; $61a2
    ldy $c6                                  ; $61a5
    dey                                      ; $61a7
    bpl $61c6                                ; $61a8
    ldy $cb                                  ; $61aa
    sty $c5                                  ; $61ac
    ldy $028d                                ; $61ae
    sty $028e                                ; $61b1
    cpx #$fd                                 ; $61b4
    bcs $61c6                                ; $61b6
    txa                                      ; $61b8
    ldx $c6                                  ; $61b9
    cpx $0289                                ; $61bb
    bcs $61c6                                ; $61be
    sta $0277,x                              ; $61c0
    inx                                      ; $61c3
    stx $c6                                  ; $61c4
    lda #$7f                                 ; $61c6
    sta $dc00                                ; $61c8
    rts                                      ; $61cb
    lda $028d                                ; $61cc
    cmp #$03                                 ; $61cf
    bne $61e8                                ; $61d1
    cmp $028e                                ; $61d3
    beq $61c6                                ; $61d6
    lda $0291                                ; $61d8
    bmi $61fa                                ; $61db
    lda $d018                                ; $61dd
    eor #$02                                 ; $61e0
    sta $d018                                ; $61e2
    jmp $eb76                                ; $61e5
    asl                                    ; $61e8
    cmp #$08                                 ; $61e9
    bcc $61ef                                ; $61eb
    lda #$06                                 ; $61ed
    tax                                      ; $61ef
    lda $eb79,x                              ; $61f0
    sta $f5                                  ; $61f3
    lda $eb7a,x                              ; $61f5
    sta $f6                                  ; $61f8
    jmp $eae0                                ; $61fa
    sta ($eb,x)                              ; $61fd
    !byte $c2                                ; $61ff (undefined opcode)
    !byte $eb                                ; $6200 (undefined opcode)
    !byte $03                                ; $6201 (undefined opcode)
    cpx $ec78                                ; $6202
    !byte $14                                ; $6205 (undefined opcode)
    ora $881d                                ; $6206
    sta $86                                  ; $6209
    !byte $87                                ; $620b (undefined opcode)
    ora ($33),y                              ; $620c
    !byte $57                                ; $620e (undefined opcode)
    eor ($34,x)                              ; $620f
    !byte $5a                                ; $6211 (undefined opcode)
    !byte $53                                ; $6212 (undefined opcode)
    eor $01                                  ; $6213
    and $52,x                                ; $6215
    !byte $44                                ; $6217 (undefined opcode)
    rol $43,x                                ; $6218
    lsr $54                                  ; $621a
    cli                                      ; $621c
    !byte $37                                ; $621d (undefined opcode)
    eor $3847,y                              ; $621e
    !byte $42                                ; $6221 (undefined opcode)
    pha                                      ; $6222
    eor $56,x                                ; $6223
    and $4a49,y                              ; $6225
    bmi $6277                                ; $6228
    !byte $4b                                ; $622a (undefined opcode)
    !byte $4f                                ; $622b (undefined opcode)
    lsr $502b                                ; $622c
    jmp $2e2d                                ; $622f
    !byte $3a                                ; $6232 (undefined opcode)
    rti                                      ; $6233
    bit $2a5c                                ; $6234
    !byte $3b                                ; $6237 (undefined opcode)
    !byte $13                                ; $6238 (undefined opcode)
    ora ($3d,x)                              ; $6239
    lsr $312f,x                              ; $623b
    !byte $5f                                ; $623e (undefined opcode)
    !byte $04                                ; $623f (undefined opcode)
    !byte $32                                ; $6240 (undefined opcode)
    jsr $5102                                ; $6241
    !byte $03                                ; $6244 (undefined opcode)
    !byte $ff                                ; $6245 (undefined opcode)
    sty $8d,x                                ; $6246
    sta $898c,x                              ; $6248
    txa                                      ; $624b
    !byte $8b                                ; $624c (undefined opcode)
    sta ($23),y                              ; $624d
    !byte $d7                                ; $624f (undefined opcode)
    cmp ($24,x)                              ; $6250
    !byte $da                                ; $6252 (undefined opcode)
    !byte $d3                                ; $6253 (undefined opcode)
    cmp $01                                  ; $6254
    and $d2                                  ; $6256
    cpy $26                                  ; $6258
    !byte $c3                                ; $625a (undefined opcode)
    dec $d4                                  ; $625b
    cld                                      ; $625d
    !byte $27                                ; $625e (undefined opcode)
    cmp $28c7,y                              ; $625f
    !byte $c2                                ; $6262 (undefined opcode)
    iny                                      ; $6263
    cmp $d6,x                                ; $6264
    and #$c9                                 ; $6266
    dex                                      ; $6268
    bmi $6238                                ; $6269
    !byte $cb                                ; $626b (undefined opcode)
    !byte $cf                                ; $626c (undefined opcode)
    dec $d0db                                ; $626d
    cpy $3edd                                ; $6270
    !byte $5b                                ; $6273 (undefined opcode)
    tsx                                      ; $6274
    !byte $3c                                ; $6275 (undefined opcode)
    lda #$c0                                 ; $6276
    eor $0193,x                              ; $6278
    and $3fde,x                              ; $627b
    and ($5f,x)                              ; $627e
    !byte $04                                ; $6280 (undefined opcode)
    !byte $22                                ; $6281 (undefined opcode)
    ldy #$02                                 ; $6282
    cmp ($83),y                              ; $6284
    !byte $ff                                ; $6286 (undefined opcode)
    !byte $10,$8d                            ; $6287 (bpl $6215)
    sta $808f,x                              ; $6289
    !byte $82                                ; $628c (undefined opcode)
    sty $91                                  ; $628d
    stx $b3,y                                ; $628f
    bcs $622a                                ; $6291
    lda $b1ae                                ; $6293
    ora ($98,x)                              ; $6296
    !byte $b2                                ; $6298 (undefined opcode)
    ldy $bc99                                ; $6299
    !byte $bb                                ; $629c (undefined opcode)
    !byte $a3                                ; $629d (undefined opcode)
    lda $b79a,x                              ; $629e
    lda $9b                                  ; $62a1
    !byte $bf                                ; $62a3 (undefined opcode)
    ldy $b8,x                                ; $62a4
    ldx $a229,y                              ; $62a6
    lda $30,x                                ; $62a9
    !byte $a7                                ; $62ab (undefined opcode)
    lda ($b9,x)                              ; $62ac
    tax                                      ; $62ae
    ldx $af                                  ; $62af
    ldx $dc,y                                ; $62b1
    rol $a45b,x                              ; $62b3
    !byte $3c                                ; $62b6 (undefined opcode)
    tay                                      ; $62b7
    !byte $df                                ; $62b8 (undefined opcode)
    eor $0193,x                              ; $62b9
    and $3fde,x                              ; $62bc
    sta ($5f,x)                              ; $62bf
    !byte $04                                ; $62c1 (undefined opcode)
    sta $a0,x                                ; $62c2
    !byte $02                                ; $62c4 (undefined opcode)
    !byte $ab                                ; $62c5 (undefined opcode)
    !byte $83                                ; $62c6 (undefined opcode)
    !byte $ff                                ; $62c7 (undefined opcode)
    cmp #$0e                                 ; $62c8
    bne $62d3                                ; $62ca
    lda $d018                                ; $62cc
    ora #$02                                 ; $62cf
    bne $62dc                                ; $62d1
    cmp #$8e                                 ; $62d3
    bne $62e2                                ; $62d5
    lda $d018                                ; $62d7
    and #$fd                                 ; $62da
    sta $d018                                ; $62dc
    jmp $e6ae                                ; $62df
    cmp #$08                                 ; $62e2
    bne $62ed                                ; $62e4
    lda #$80                                 ; $62e6
    ora $0291                                ; $62e8
    bmi $62f6                                ; $62eb
    cmp #$09                                 ; $62ed
    bne $62df                                ; $62ef
    lda #$7f                                 ; $62f1
    and $0291                                ; $62f3
    sta $0291                                ; $62f6
    jmp $e6ae                                ; $62f9
    !byte $0f                                ; $62fc (undefined opcode)
    inc $ffff,x                              ; $62fd
    !byte $ff                                ; $6300 (undefined opcode)
    !byte $ff                                ; $6301 (undefined opcode)
    !byte $ff                                ; $6302 (undefined opcode)
    !byte $ff                                ; $6303 (undefined opcode)
    !byte $1c                                ; $6304 (undefined opcode)
    !byte $17                                ; $6305 (undefined opcode)
    ora ($9f,x)                              ; $6306
    !byte $1a                                ; $6308 (undefined opcode)
    !byte $13                                ; $6309 (undefined opcode)
    ora $ff                                  ; $630a
    !byte $9c                                ; $630c (undefined opcode)
    !byte $12                                ; $630d (undefined opcode)
    !byte $04                                ; $630e (undefined opcode)
    asl $0603,x                              ; $630f
    !byte $14                                ; $6312 (undefined opcode)
    clc                                      ; $6313
    !byte $1f                                ; $6314 (undefined opcode)
    ora $9e07,y                              ; $6315
    !byte $02                                ; $6318 (undefined opcode)
    php                                      ; $6319
    ora $16,x                                ; $631a
    !byte $12                                ; $631c (undefined opcode)
    ora #$0a                                 ; $631d
    !byte $92                                ; $631f (undefined opcode)
    ora $0f0b                                ; $6320
    asl $10ff                                ; $6323
    !byte $0c                                ; $6326 (undefined opcode)
    !byte $ff                                ; $6327 (undefined opcode)
    !byte $ff                                ; $6328 (undefined opcode)
    !byte $1b                                ; $6329 (undefined opcode)
    brk                                      ; $632a
    !byte $ff                                ; $632b (undefined opcode)
    !byte $1c                                ; $632c (undefined opcode)
    sbc $021d,x                              ; $632d
    !byte $ff                                ; $6330 (undefined opcode)
    !byte $1f                                ; $6331 (undefined opcode)
    asl $90ff,x                              ; $6332
    asl $ff                                  ; $6335
    ora $ff                                  ; $6337
    !byte $ff                                ; $6339 (undefined opcode)
    !byte $11,$ff                            ; $633a (ora ($ff),y - zeropage wrap)
    !byte $ff                                ; $633c (undefined opcode)
    brk                                      ; $633d
    brk                                      ; $633e
    brk                                      ; $633f
    brk                                      ; $6340
    brk                                      ; $6341
    brk                                      ; $6342
    brk                                      ; $6343
    brk                                      ; $6344
    brk                                      ; $6345
    brk                                      ; $6346
    brk                                      ; $6347
    brk                                      ; $6348
    brk                                      ; $6349
    brk                                      ; $634a
    brk                                      ; $634b
    brk                                      ; $634c
    brk                                      ; $634d
    !byte $9b                                ; $634e (undefined opcode)
    !byte $37                                ; $634f (undefined opcode)
    brk                                      ; $6350
    brk                                      ; $6351
    brk                                      ; $6352
    php                                      ; $6353
    brk                                      ; $6354
    !byte $14                                ; $6355 (undefined opcode)
    !byte $0f                                ; $6356 (undefined opcode)
    brk                                      ; $6357
    brk                                      ; $6358
    brk                                      ; $6359
    brk                                      ; $635a
    brk                                      ; $635b
    brk                                      ; $635c
    asl $0106                                ; $635d
    !byte $02                                ; $6360 (undefined opcode)
    !byte $03                                ; $6361 (undefined opcode)
    !byte $04                                ; $6362 (undefined opcode)
    brk                                      ; $6363
    ora ($02,x)                              ; $6364
    !byte $03                                ; $6366 (undefined opcode)
    !byte $04                                ; $6367 (undefined opcode)
    ora $06                                  ; $6368
    !byte $07                                ; $636a (undefined opcode)
    !byte $4c,$4f,$61                        ; $636b (data: "LOa")
    ora $5393                                ; $636e
    eor $0d53,y                              ; $6371
    brk                                      ; $6374
    plp                                      ; $6375
    bvc $63f0                                ; $6376
    ldy #$c8                                 ; $6378
    beq $6394                                ; $637a
    rti                                      ; $637c
    pla                                      ; $637d
    bcc $6338                                ; $637e
    cpx #$08                                 ; $6380
    bmi $63dc                                ; $6382
    !byte $80                                ; $6384 (undefined opcode)
    tay                                      ; $6385
    bne $6380                                ; $6386
    !byte $20,$48,$70                        ; $6388 (data: " Hp")
    tya                                      ; $638b
    cpy #$09                                 ; $638c
    rti                                      ; $638e
    bit $2009                                ; $638f
    bit $24a9                                ; $6392
    pha                                      ; $6395
    bit $94                                  ; $6396
    bpl $63a4                                ; $6398
    sec                                      ; $639a
    ror $a3                                  ; $639b
    jsr $ed40                                ; $639d
    lsr $94                                  ; $63a0
    lsr $a3                                  ; $63a2
    pla                                      ; $63a4
    sta $95                                  ; $63a5
    sei                                      ; $63a7
    jsr $f951                                ; $63a8
    cmp #$24                                 ; $63ab
    bne $63b2                                ; $63ad
    jmp $f91a                                ; $63af
; =============================================================================
; PARALLEL_MODE_ENTRY - Initialize parallel transfer mode for fast load
; Sets DATA OUT high to signal parallel mode to drive
; =============================================================================
    lda $dd00      ; Read CIA#2 port A       ; $63b2
    ora #$08       ; Set bit 3 (DATA OUT high) ; $63b5
    sta $dd00      ; Signal: "C64 ready for parallel mode" ; $63b7
    sei ; Disable interrupts for timing-critical code ; $63ba
    jsr $ee8e      ; IEC bus timing setup    ; $63bb
    jsr $ee97      ; Release CLK line        ; $63be
    jsr $eeb3      ; Additional IEC setup    ; $63c1
    sei ; Ensure interrupts still off        ; $63c4
    jmp $f96f      ; Continue to standard load routine ; $63c5
; =============================================================================
; SERIAL_BIT_RECEIVE - Fallback bit-by-bit receive (when parallel unavailable)
; Receives 8 bits via IEC bus CLK/DATA lines, stores in $95
; =============================================================================
    jsr $eea9      ; Wait for CLK edge       ; $63c8
    bcs $6431      ; Error: timeout          ; $63cb
    jsr $ee85      ; Release DATA line       ; $63cd
    bit $a3        ; Check handshake state   ; $63d0
    bpl $63de      ; Skip sync if not needed ; $63d2
    jsr $eea9      ; Wait for sync pulse high ; $63d4
    bcc $63d4                                ; $63d7
    jsr $eea9      ; Wait for sync pulse low ; $63d9
    bcs $63d9                                ; $63dc
    jsr $eea9      ; Wait for data ready signal ; $63de
    bcc $63de                                ; $63e1
    jsr $ee8e      ; Prepare for bit reception ; $63e3
    lda #$08       ; 8 bits to receive       ; $63e6
    sta $a5        ; Store bit counter       ; $63e8
; --- Receive loop: get 8 bits from $DD00 bit 7 ---
    lda $dd00      ; Read port (bit 7 = data from drive) ; $63ea
    cmp $dd00      ; Wait for stable read    ; $63ed
    bne $63ea      ; Loop until two reads match ; $63f0
    asl          ; Shift bit 7 into carry  ; $63f2
    bcc $6434      ; Carry clear (bit 7 was 0): EOI/last-byte condition ; $63f3
    ror $95        ; Rotate carry into result byte ; $63f5
    bcs $63fe      ; Branch based on bit value ; $63f7
    jsr $eea0      ; Acknowledge: pull DATA low ; $63f9
    bne $6401                                ; $63fc
    jsr $ee97      ; Or: release CLK         ; $63fe
    jsr $ee85      ; Release DATA line       ; $6401
    nop ; Timing delay (4 cycles)            ; $6404
    nop                                      ; $6405
    nop                                      ; $6406
    nop                                      ; $6407
    lda $dd00      ; Read port for next handshake ; $6408
    and #$df       ; Clear bit 5             ; $640b
    ora #$10       ; Set bit 4 (acknowledge received) ; $640d
    sta $dd00                                ; $640f
    dec $a5        ; Decrement bit counter   ; $6412
    bne $63ea      ; Loop until 8 bits received ; $6414
    lda #$04                                 ; $6416
    sta $dc07                                ; $6418
    lda #$19                                 ; $641b
    sta $dc0f                                ; $641d
    lda $dc0d                                ; $6420
    lda $dc0d                                ; $6423
    and #$02                                 ; $6426
    bne $6434                                ; $6428
    jsr $eea9                                ; $642a
    bcs $6423                                ; $642d
    cli                                      ; $642f
    rts                                      ; $6430
    lda #$80                                 ; $6431
    bit $03a9                                ; $6433
    jsr $fe1c                                ; $6436
    cli                                      ; $6439
    clc                                      ; $643a
    bcc $6487                                ; $643b
    sta $95                                  ; $643d
    jsr $f8dd                                ; $643f
    lda $dd00                                ; $6442
    and #$f7                                 ; $6445
    sta $dd00                                ; $6447
    rts                                      ; $644a
    sta $95                                  ; $644b
    jsr $f8dd                                ; $644d
    sei                                      ; $6450
    jsr $f76a                                ; $6451
    jsr $edbe                                ; $6454
    jsr $ee85                                ; $6457
    jsr $eea9                                ; $645a
    bmi $645a                                ; $645d
    cli                                      ; $645f
    rts                                      ; $6460
    bit $94                                  ; $6461
    bmi $646a                                ; $6463
    sec                                      ; $6465
    ror $94                                  ; $6466
    bne $646f                                ; $6468
    pha                                      ; $646a
    jsr $ed40                                ; $646b
    pla                                      ; $646e
    sta $95                                  ; $646f
    clc                                      ; $6471
    rts                                      ; $6472
    sei                                      ; $6473
    jsr $ee8e                                ; $6474
    lda $dd00                                ; $6477
    ora #$08                                 ; $647a
    sta $dd00                                ; $647c
    lda #$5f                                 ; $647f
    bit $3fa9                                ; $6481
    jsr $ed11                                ; $6484
    jsr $edbe                                ; $6487
    txa                                      ; $648a
    ldx #$0a                                 ; $648b
    dex                                      ; $648d
    bne $648d                                ; $648e
    tax                                      ; $6490
    jsr $ee85                                ; $6491
    jmp $ee97                                ; $6494
    sei                                      ; $6497
    lda #$00                                 ; $6498
    sta $a5                                  ; $649a
    jmp $f841                                ; $649c
    jsr $eea9                                ; $649f
    bpl $649f                                ; $64a2
    lda #$01                                 ; $64a4
    sta $dc07                                ; $64a6
    lda #$19                                 ; $64a9
    sta $dc0f                                ; $64ab
    jsr $ee97                                ; $64ae
    lda $dc0d                                ; $64b1
    lda $dc0d                                ; $64b4
    and #$02                                 ; $64b7
    bne $64c2                                ; $64b9
    jsr $eea9                                ; $64bb
    bmi $64b4                                ; $64be
    bpl $64da                                ; $64c0
    lda $a5                                  ; $64c2
    beq $64cb                                ; $64c4
    lda #$02                                 ; $64c6
    jmp $edb2                                ; $64c8
    jsr $eea0                                ; $64cb
    jsr $ee85                                ; $64ce
    lda #$40                                 ; $64d1
    jsr $fe1c                                ; $64d3
    inc $a5                                  ; $64d6
    bne $64a4                                ; $64d8
; =============================================================================
; PARALLEL_FAST_RECEIVE - Receive byte via parallel port bit 7 handshake
; Uses two-phase clocking: wait for bit7 high (data), then wait for bit7 low
; Result stored in $A4, returned in A
; =============================================================================
    lda #$08       ; 8 bits to receive       ; $64da
    sta $a5        ; Store in bit counter    ; $64dc
; --- Receive bit loop ---
    lda $dd00      ; Read CIA#2 port A       ; $64de
    cmp $dd00      ; Double-read for stability (debounce) ; $64e1
    bne $64de      ; Retry if reads don't match ; $64e4
    asl          ; Shift bit 7 into carry  ; $64e6
    bpl $64de      ; Wait until bit 7 is HIGH (data ready) ; $64e7
    ror $a4        ; Rotate carry (the data bit) into result ; $64e9
; --- Wait for clock phase 2 (bit 7 goes low) ---
    lda $dd00      ; Read port again         ; $64eb
    cmp $dd00      ; Stability check         ; $64ee
    bne $64eb      ; Retry if unstable       ; $64f1
    asl          ; Shift bit 7 into carry  ; $64f3
    bmi $64eb      ; Wait until bit 7 is LOW (next bit ready) ; $64f4
    dec $a5        ; Decrement bit counter   ; $64f6
    bne $64de      ; Loop until 8 bits received ; $64f8
    jsr $eea0      ; Pull DATA low (acknowledge byte) ; $64fa
    bit $90        ; Check status flags      ; $64fd
    bvc $6504      ; Skip if no overflow     ; $64ff
    jsr $ee06      ; Handle EOI condition    ; $6501
    lda $a4        ; Return received byte in A ; $6504
    cli ; Re-enable interrupts               ; $6506
    clc ; Clear carry (success)              ; $6507
    rts                                      ; $6508
; =============================================================================
; HANDSHAKE HELPER ROUTINES - Manipulate $DD00 bits for IEC/parallel signaling
; =============================================================================
; CLR_ATN - Clear bit 4 (ATN OUT low)
    lda $dd00                                ; $6509
    and #$ef       ; Clear bit 4             ; $650c
    sta $dd00                                ; $650e
    rts                                      ; $6511
; SET_ATN - Set bit 4 (ATN OUT high)
    lda $dd00                                ; $6512
    ora #$10       ; Set bit 4               ; $6515
    sta $dd00                                ; $6517
    rts                                      ; $651a
; CLR_BIT5 - Clear bit 5 (directly mapped to user port)
    lda $dd00                                ; $651b
    and #$df       ; Clear bit 5             ; $651e
    sta $dd00                                ; $6520
    rts                                      ; $6523
; SET_BIT5 - Set bit 5 (directly mapped to user port)
    lda $dd00                                ; $6524
    ora #$20       ; Set bit 5               ; $6527
    sta $dd00                                ; $6529
    rts                                      ; $652c
; WAIT_STABLE_READ - Read $DD00 until stable, return bit 7 in carry
    lda $dd00      ; Read port               ; $652d
    cmp $dd00      ; Compare with second read ; $6530
    bne $652d      ; Loop until stable       ; $6533
    asl          ; Shift bit 7 into carry  ; $6535
    rts                                      ; $6536
; SHORT_DELAY - ~500 cycle delay loop
    txa ; Save X                             ; $6537
    ldx #$64       ; 100 iterations          ; $6538
    dex                                      ; $653a
    bne $653a      ; Loop (5 cycles * 100 = 500 cycles) ; $653b
    tax ; Restore X                          ; $653d
    rts                                      ; $653e
    jsr $f5d2                                ; $653f
    cpx #$02                                 ; $6542
    beq $654e                                ; $6544
    ldy #$00                                 ; $6546
    lda ($bb),y                              ; $6548
    cmp #$24                                 ; $654a
    bne $6551                                ; $654c
    jmp $f4f3                                ; $654e
    ldx $dc0c                                ; $6551
    bpl $654e                                ; $6554
    ldy #$51                                 ; $6556
    jsr $f0eb                                ; $6558
    jsr $f76f                                ; $655b
    jsr $efef                                ; $655e
    beq $654e                                ; $6561
    lda $9d                                  ; $6563
    bpl $6572                                ; $6565
    ldy #$2a                                 ; $6567
    jsr $f12f                                ; $6569
    jsr $f890                                ; $656c
    jsr $f12f                                ; $656f
    lda $ae                                  ; $6572
    sta $c3                                  ; $6574
    lda $af                                  ; $6576
    sta $c4                                  ; $6578
    jsr $f5a1                                ; $657a
    tax                                      ; $657d
    bit $dd00                                ; $657e
    bvs $65cd                                ; $6581
    bit $dd0d                                ; $6583
    beq $657e                                ; $6586
    bit $dd01                                ; $6588
    bit $91                                  ; $658b
    bpl $65af                                ; $658d
    ldy #$00                                 ; $658f
    lda $93                                  ; $6591
    bne $65b5                                ; $6593
    txa                                      ; $6595
    bit $dd00                                ; $6596
    bvs $65cd                                ; $6599
    bit $dd0d                                ; $659b
    beq $6596                                ; $659e
    lda $dd01                                ; $65a0
    sta ($ae),y                              ; $65a3
    inc $ae                                  ; $65a5
    bne $6595                                ; $65a7
    inc $af                                  ; $65a9
    bit $91                                  ; $65ab
    bmi $6591                                ; $65ad
    jsr $f736                                ; $65af
    jmp $f636                                ; $65b2
    txa                                      ; $65b5
    bit $dd00                                ; $65b6
    bvs $65cd                                ; $65b9
    bit $dd0d                                ; $65bb
    beq $65b6                                ; $65be
    lda $dd01                                ; $65c0
    cmp ($ae),y                              ; $65c3
    bne $65ea                                ; $65c5
    inc $ae                                  ; $65c7
    bne $65b5                                ; $65c9
    beq $65a9                                ; $65cb
    jsr $ee97                                ; $65cd
    ldy #$40                                 ; $65d0
    txa                                      ; $65d2
    bit $dd0d                                ; $65d3
    bne $65e2                                ; $65d6
    inx                                      ; $65d8
    bne $65d3                                ; $65d9
    ldy #$42                                 ; $65db
    jsr $ef5e                                ; $65dd
    bcc $65b2                                ; $65e0
    sty $90                                  ; $65e2
    jsr $f740                                ; $65e4
    jmp $f5a9                                ; $65e7
    jsr $f736                                ; $65ea
    lda #$50                                 ; $65ed
    sta $90                                  ; $65ef
    bne $65e7                                ; $65f1
    jsr $eddd                                ; $65f3
    lda $90                                  ; $65f6
    beq $65ff                                ; $65f8
    ldy #$00                                 ; $65fa
    jmp $f624                                ; $65fc
    ldx $dc0c                                ; $65ff
    bpl $65fa                                ; $6602
    ldy #$5a                                 ; $6604
    jsr $f0eb                                ; $6606
    jsr $ee8e                                ; $6609
    jsr $efef                                ; $660c
    beq $65fa                                ; $660f
    ldy $ac                                  ; $6611
    sty $dd01                                ; $6613
    jsr $fcd1                                ; $6616
    ldx $ad                                  ; $6619
    bcs $6664                                ; $661b
    jsr $f5a1                                ; $661d
    lda #$00                                 ; $6620
    sta $ac                                  ; $6622
    lda $ad                                  ; $6624
    cmp $af                                  ; $6626
    beq $665c                                ; $6628
    bne $662f                                ; $662a
    stx $dd01                                ; $662c
    lda ($ac),y                              ; $662f
    tax                                      ; $6631
    iny                                      ; $6632
    bne $6647                                ; $6633
    inc $ad                                  ; $6635
    bit $91                                  ; $6637
    bmi $6641                                ; $6639
    jsr $efe0                                ; $663b
    jmp $f636                                ; $663e
    lda $ad                                  ; $6641
    cmp $af                                  ; $6643
    beq $6660                                ; $6645
    lda #$10                                 ; $6647
    bit $dd0d                                ; $6649
    bne $662c                                ; $664c
    bit $dd00                                ; $664e
    bpl $6649                                ; $6651
    jsr $f958                                ; $6653
    jmp $f63f                                ; $6656
    jsr $f5a1                                ; $6659
    lda ($ac),y                              ; $665c
    tax                                      ; $665e
    iny                                      ; $665f
    cpy $ae                                  ; $6660
    bne $6659                                ; $6662
    jsr $ff3b                                ; $6664
    jsr $ee85                                ; $6667
    stx $dd01                                ; $666a
    jsr $ff3b                                ; $666d
    jmp $efcf                                ; $6670
    stx $dc0c                                ; $6673
    lda #$10                                 ; $6676
    tax                                      ; $6678
    bit $dd0d                                ; $6679
    bne $6681                                ; $667c
    inx                                      ; $667e
    bne $6679                                ; $667f
    rts                                      ; $6681
    lda #$cb                                 ; $6682
    php                                      ; $6684
    pha                                      ; $6685
    txa                                      ; $6686
    pha                                      ; $6687
    tya                                      ; $6688
    pha                                      ; $6689
    sei                                      ; $668a
    cld                                      ; $668b
    ldx #$05                                 ; $668c
    pla                                      ; $668e
    sta $0229,x                              ; $668f
    dex                                      ; $6692
    bpl $668e                                ; $6693
    tsx                                      ; $6695
    stx $022f                                ; $6696
    lda #$c0                                 ; $6699
    sta $9d                                  ; $669b
    lda #$52                                 ; $669d
    bne $66cc                                ; $669f
    lda $0316                                ; $66a1
    cmp #$66                                 ; $66a4
    bne $66af                                ; $66a6
    lda $0317                                ; $66a8
    cmp #$fe                                 ; $66ab
    beq $668a                                ; $66ad
    lda #$10                                 ; $66af
    jmp ($0316)                              ; $66b1
    lda #$3f                                 ; $66b4
    jsr $ffd2                                ; $66b6
    ldx $022f                                ; $66b9
    txs                                      ; $66bc
    cli                                      ; $66bd
    jsr $fec2                                ; $66be
    jsr $ffcf                                ; $66c1
    cmp #$2e                                 ; $66c4
    beq $66c1                                ; $66c6
    cmp #$20                                 ; $66c8
    beq $66c1                                ; $66ca
    ldx #$0e                                 ; $66cc
    dex                                      ; $66ce
    bmi $66b4                                ; $66cf
    cmp $f227,x                              ; $66d1
    bne $66ce                                ; $66d4
    txa                                      ; $66d6
    asl                                    ; $66d7
    tax                                      ; $66d8
    lda $f8b0,x                              ; $66d9
    pha                                      ; $66dc
    lda $f8af,x                              ; $66dd
    pha                                      ; $66e0
    rts                                      ; $66e1
    jsr $f063                                ; $66e2
    sta $fa                                  ; $66e5
    lda #$10                                 ; $66e7
    sta $f9                                  ; $66e9
    jsr $f06e                                ; $66eb
    asl $f9                                  ; $66ee
    bcc $66ee                                ; $66f0
    jsr $ffcf                                ; $66f2
    cmp #$20                                 ; $66f5
    beq $66f2                                ; $66f7
    cmp #$30                                 ; $66f9
    bcc $66b4                                ; $66fb
    cmp #$47                                 ; $66fd
    bcs $66b4                                ; $66ff
    cmp #$3a                                 ; $6701
    bcc $670b                                ; $6703
    cmp #$41                                 ; $6705
    bcc $66b4                                ; $6707
    sbc #$08                                 ; $6709
    sbc #$2f                                 ; $670b
    ora $f9                                  ; $670d
    sta $f9                                  ; $670f
    rts                                      ; $6711
    jsr $2e3b                                ; $6712
    !byte $0d,$43,$5a                        ; $6715 (data: CR "CZ")
    eor #$44                                 ; $6718
    !byte $42                                ; $671a (undefined opcode)
    and $4e56                                ; $671b
    jsr $4320                                ; $671e
    bvc $6743                                ; $6721
    jsr $5350                                ; $6723
    !byte $20,$52,$59                        ; $6726 (data: " RY")
    !byte $20,$52,$58                        ; $6729 (data: " RX")
    jsr $4143                                ; $672c
    jsr $5352                                ; $672f
    jsr $2020                                ; $6732
    jsr $000d                                ; $6735
    eor ($5f,x)                              ; $6738
    adc ($7f,x)                              ; $673a
    cpy #$c1                                 ; $673c
    !byte $db                                ; $673e (undefined opcode)
    cpx #$ff                                 ; $673f
    ora $2f49                                ; $6741
    !byte $4f                                ; $6744 (undefined opcode)
    jsr $5245                                ; $6745
    !byte $52                                ; $6748 (undefined opcode)
    !byte $4f                                ; $6749 (undefined opcode)
    !byte $52                                ; $674a (undefined opcode)
    jsr $0da3                                ; $674b
    !byte $53                                ; $674e (undefined opcode)
    eor $41                                  ; $674f
    !byte $52                                ; $6751 (undefined opcode)
    !byte $43                                ; $6752 (undefined opcode)
    pha                                      ; $6753
    eor #$4e                                 ; $6754
    !byte $47                                ; $6756 (undefined opcode)
    ldy #$46                                 ; $6757
    !byte $4f                                ; $6759 (undefined opcode)
    !byte $52                                ; $675a (undefined opcode)
    ldy #$0d                                 ; $675b
    bvc $67ab                                ; $675d
    eor ($59,x)                              ; $675f
    !byte $bf                                ; $6761 (undefined opcode)
    !byte $52                                ; $6762 (undefined opcode)
    eor $43                                  ; $6763
    rol $50                                  ; $6765
    !byte $4c,$41,$59                        ; $6767 (data: "LAY")
    !byte $bf                                ; $676a (undefined opcode)
    jsr $2da4                                ; $676b
    ldy $20                                  ; $676e
    sei                                      ; $6770
    sbc ($20),y                              ; $6771
    clv                                      ; $6773
    sbc ($2c),y                              ; $6774
    !byte $0d,$dd,$60                        ; $6776 (data)
    ldx #$35                                 ; $6779
    ldy #$f2                                 ; $677b
    lda $93                                  ; $677d
    beq $6785                                ; $677f
    inx                                      ; $6781
    lda #$01                                 ; $6782
    bit $02a9                                ; $6784
    jmp $ffbd                                ; $6787
    ora $4f4c                                ; $678a
    eor ($44,x)                              ; $678d
    eor #$4e                                 ; $678f
    !byte $c7                                ; $6791 (undefined opcode)
    ora $4153                                ; $6792
    lsr $49,x                                ; $6795
    lsr $a047                                ; $6797
    ora $4556                                ; $679a
    !byte $52                                ; $679d (undefined opcode)
    eor #$46                                 ; $679e
    eor $4e49,y                              ; $67a0
    !byte $c7                                ; $67a3 (undefined opcode)
    ora $4f46                                ; $67a4
    eor $4e,x                                ; $67a7
    !byte $44                                ; $67a9 (undefined opcode)
    ldy #$0d                                 ; $67aa
    !byte $4f                                ; $67ac (undefined opcode)
    !byte $4b                                ; $67ad (undefined opcode)
    sta $9d24                                ; $67ae
    bpl $67c0                                ; $67b1
    lda $f0bd,y                              ; $67b3
    php                                      ; $67b6
    and #$7f                                 ; $67b7
    jsr $ffd2                                ; $67b9
    iny                                      ; $67bc
    plp                                      ; $67bd
    bpl $67b3                                ; $67be
    clc                                      ; $67c0
    rts                                      ; $67c1
    lda $99                                  ; $67c2
    bne $67ea                                ; $67c4
    lda $c6                                  ; $67c6
    beq $67c0                                ; $67c8
    sei                                      ; $67ca
    jmp $e5b4                                ; $67cb
    jsr $0079                                ; $67ce
    cmp #$2c                                 ; $67d1
    beq $67da                                ; $67d3
    pla                                      ; $67d5
    pla                                      ; $67d6
    jmp $a8f8                                ; $67d7
    rts                                      ; $67da
    lda $99                                  ; $67db
    bne $67ea                                ; $67dd
    lda $d3                                  ; $67df
    sta $ca                                  ; $67e1
    lda $d6                                  ; $67e3
    sta $c9                                  ; $67e5
    jmp $e632                                ; $67e7
    cmp #$03                                 ; $67ea
    bne $67f7                                ; $67ec
    sta $d0                                  ; $67ee
    lda $d5                                  ; $67f0
    sta $c8                                  ; $67f2
    jmp $e632                                ; $67f4
    bcs $6831                                ; $67f7
    jmp $f713                                ; $67f9
    jsr $f676                                ; $67fc
    lda #$58                                 ; $67ff
    jsr $eddd                                ; $6801
    tya                                      ; $6804
    jsr $eddd                                ; $6805
    jmp $edfe                                ; $6808
    ldy #$00                                 ; $680b
    jsr $fb9e                                ; $680d
    lda ($f9),y                              ; $6810
    jsr $f898                                ; $6812
    iny                                      ; $6815
    dex                                      ; $6816
    bne $680d                                ; $6817
    rts                                      ; $6819
    brk                                      ; $681a
    brk                                      ; $681b
    brk                                      ; $681c
    !byte $03                                ; $681d (undefined opcode)
    !byte $27                                ; $681e (undefined opcode)
    brk                                      ; $681f
    cpx #$00                                 ; $6820
    !byte $20,$00,$60                        ; $6822 (data)
    !byte $80                                ; $6825 (undefined opcode)
    rts                                      ; $6826
    rti                                      ; $6827
    sta ($85,x)                              ; $6828
    ldx $4c                                  ; $682a
    !byte $4b                                ; $682c (undefined opcode)
    !byte $fc                                ; $682d (undefined opcode)
    jmp $f2a9                                ; $682e
    lda $90                                  ; $6831
    beq $6839                                ; $6833
    lda #$0d                                 ; $6835
    clc                                      ; $6837
    rts                                      ; $6838
    jmp $ee13                                ; $6839
    lda $dd03                                ; $683c
    sta $0296                                ; $683f
    lda #$ff                                 ; $6842
    sta $dd03                                ; $6844
    lda $dd01                                ; $6847
    sta $0295                                ; $684a
    rts                                      ; $684d
    pha                                      ; $684e
    lda $9a                                  ; $684f
    cmp #$03                                 ; $6851
    bne $6859                                ; $6853
    pla                                      ; $6855
    jmp $e716                                ; $6856
    bcc $685f                                ; $6859
    pla                                      ; $685b
    jmp $eddd                                ; $685c
    pla                                      ; $685f
    jmp $f713                                ; $6860
    ldx #$08                                 ; $6863
    ldy #$00                                 ; $6865
    jsr $e101                                ; $6867
    jsr $ffcf                                ; $686a
    cmp #$22                                 ; $686d
    beq $6881                                ; $686f
    cmp #$20                                 ; $6871
    beq $686a                                ; $6873
    cmp #$0d                                 ; $6875
    beq $6888                                ; $6877
    cmp #$22                                 ; $6879
    beq $688a                                ; $687b
    sta $0200,y                              ; $687d
    iny                                      ; $6880
    jsr $ffcf                                ; $6881
    cpy #$29                                 ; $6884
    bne $6875                                ; $6886
    stx $b9                                  ; $6888
    tya                                      ; $688a
    ldx #$00                                 ; $688b
    ldy #$02                                 ; $688d
    jmp $ffbd                                ; $688f
    jsr $f30f                                ; $6892
    beq $689a                                ; $6895
    jmp $f701                                ; $6897
    jsr $f31f                                ; $689a
    lda $ba                                  ; $689d
    beq $68a7                                ; $689f
    cmp #$03                                 ; $68a1
    bcc $6860                                ; $68a3
    bne $68bb                                ; $68a5
    sta $99                                  ; $68a7
    clc                                      ; $68a9
    rts                                      ; $68aa
    !byte $52                                ; $68ab (undefined opcode)
    eor $3a58                                ; $68ac
    !byte $3b                                ; $68af (undefined opcode)
    !byte $47                                ; $68b0 (undefined opcode)
    !byte $53                                ; $68b1 (undefined opcode)
    jmp $4056                                ; $68b2
    eor ($57,x)                              ; $68b5
    ldy #$0d                                 ; $68b7
    !byte $3a                                ; $68b9 (undefined opcode)
    rol                                    ; $68ba
    tax                                      ; $68bb
    jsr $ed09                                ; $68bc
    lda $b9                                  ; $68bf
    bpl $68c9                                ; $68c1
    jsr $edcc                                ; $68c3
    jmp $f248                                ; $68c6
    jsr $edc7                                ; $68c9
    txa                                      ; $68cc
    bit $90                                  ; $68cd
    bpl $68a7                                ; $68cf
    jmp $f707                                ; $68d1
    jsr $f30f                                ; $68d4
    beq $68dc                                ; $68d7
    jmp $f701                                ; $68d9
    jsr $f31f                                ; $68dc
    lda $ba                                  ; $68df
    bne $68e6                                ; $68e1
    jmp $f70d                                ; $68e3
    cmp #$03                                 ; $68e6
    bcc $68a3                                ; $68e8
    bne $68fd                                ; $68ea
    sta $9a                                  ; $68ec
    clc                                      ; $68ee
    rts                                      ; $68ef
    ldy #$00                                 ; $68f0
    jsr $f063                                ; $68f2
    sei                                      ; $68f5
    sta ($f7),y                              ; $68f6
    iny                                      ; $68f8
    dex                                      ; $68f9
    bne $68f2                                ; $68fa
    rts                                      ; $68fc
    tax                                      ; $68fd
    jsr $ed0c                                ; $68fe
    lda $b9                                  ; $6901
    bpl $690a                                ; $6903
    jsr $edbe                                ; $6905
    bne $690d                                ; $6908
    jsr $edb9                                ; $690a
    txa                                      ; $690d
    bit $90                                  ; $690e
    bpl $68ec                                ; $6910
    jmp $f707                                ; $6912
    jsr $f314                                ; $6915
    beq $691c                                ; $6918
    clc                                      ; $691a
    rts                                      ; $691b
    jsr $f31f                                ; $691c
    txa                                      ; $691f
    pha                                      ; $6920
    lda $ba                                  ; $6921
    beq $6975                                ; $6923
    cmp #$03                                 ; $6925
    beq $6975                                ; $6927
    bcs $6972                                ; $6929
    bcc $6975                                ; $692b
    pha                                      ; $692d
    and #$60                                 ; $692e
    bne $6939                                ; $6930
    inc $c7                                  ; $6932
    pla                                      ; $6934
    clc                                      ; $6935
    adc #$40                                 ; $6936
    bit $68                                  ; $6938
    jsr $ffd2                                ; $693a
    lda #$00                                 ; $693d
    sta $c7                                  ; $693f
    rts                                      ; $6941
    bne $6958                                ; $6942
    lda $c3                                  ; $6944
    sta $14                                  ; $6946
    sbc $2b                                  ; $6948
    ldx $c4                                  ; $694a
    stx $15                                  ; $694c
    cpx $2c                                  ; $694e
    bne $695b                                ; $6950
    tax                                      ; $6952
    bne $695b                                ; $6953
    jmp $a871                                ; $6955
    jsr $f1a7                                ; $6958
    ldy $15                                  ; $695b
    bne $696f                                ; $695d
    lda $14                                  ; $695f
    cmp #$10                                 ; $6961
    bcs $696f                                ; $6963
    sty $14                                  ; $6965
    asl                                    ; $6967
    asl                                    ; $6968
    asl                                    ; $6969
    asl                                    ; $696a
    sta $15                                  ; $696b
    beq $693e                                ; $696d
    jmp $e130                                ; $696f
    jsr $f642                                ; $6972
    pla                                      ; $6975
    tax                                      ; $6976
    dec $98                                  ; $6977
    cpx $98                                  ; $6979
    beq $6991                                ; $697b
    ldy $98                                  ; $697d
    lda $0259,y                              ; $697f
    sta $0259,x                              ; $6982
    lda $0263,y                              ; $6985
    sta $0263,x                              ; $6988
    lda $026d,y                              ; $698b
    sta $026d,x                              ; $698e
    clc                                      ; $6991
    rts                                      ; $6992
    lda #$00                                 ; $6993
    sta $90                                  ; $6995
    txa                                      ; $6997
    ldx $98                                  ; $6998
    dex                                      ; $699a
    bmi $69b2                                ; $699b
    cmp $0259,x                              ; $699d
    bne $699a                                ; $69a0
    rts                                      ; $69a2
    lda $0259,x                              ; $69a3
    sta $b8                                  ; $69a6
    lda $0263,x                              ; $69a8
    sta $ba                                  ; $69ab
    lda $026d,x                              ; $69ad
    sta $b9                                  ; $69b0
    rts                                      ; $69b2
    lda #$00                                 ; $69b3
    sta $98                                  ; $69b5
    ldx #$03                                 ; $69b7
    cpx $9a                                  ; $69b9
    bcs $69c0                                ; $69bb
    jsr $edfe                                ; $69bd
    cpx $99                                  ; $69c0
    bcs $69c7                                ; $69c2
    jsr $edef                                ; $69c4
    stx $9a                                  ; $69c7
    lda #$00                                 ; $69c9
    sta $99                                  ; $69cb
    rts                                      ; $69cd
    ldx $b8                                  ; $69ce
    bne $69d5                                ; $69d0
    jmp $f70a                                ; $69d2
    jsr $f30f                                ; $69d5
    bne $69dd                                ; $69d8
    jmp $f6fe                                ; $69da
    ldx $98                                  ; $69dd
    cpx #$0a                                 ; $69df
    bcc $69e6                                ; $69e1
    jmp $f6fb                                ; $69e3
    inc $98                                  ; $69e6
    lda $b8                                  ; $69e8
    sta $0259,x                              ; $69ea
    lda $b9                                  ; $69ed
    ora #$60                                 ; $69ef
    sta $b9                                  ; $69f1
    sta $026d,x                              ; $69f3
    lda $ba                                  ; $69f6
    sta $0263,x                              ; $69f8
    beq $6a30                                ; $69fb
    cmp #$03                                 ; $69fd
    beq $6a30                                ; $69ff
    bcc $6a08                                ; $6a01
    jsr $f3d5                                ; $6a03
    bcc $6a30                                ; $6a06
    jmp $f713                                ; $6a08
; F-key string table
; F1: LOa CR Ru: CR (LOAD then RUN with colon separator)
    !byte $4c,$4f,$61                        ; $6a0b
    !byte $0d,$52,$75                        ; $6a0e
    !byte $3a,$0d                            ; $6a11
    !byte $00                                ; $6a13
; F2: Sy$0 LEFT (SYS$0 - monitor)
    !byte $53,$79,$24                        ; $6a14
    !byte $30,$9d                            ; $6a17
    !byte $00                                ; $6a19
; F3: CLR @$ CR (display directory)
    !byte $93,$40,$24                        ; $6a1a
    !byte $0d                                ; $6a1d
    !byte $00                                ; $6a1e
; F4: Ve CR (VERIFY)
    !byte $56,$65                            ; $6a1f
    !byte $0d                                ; $6a21
    !byte $00                                ; $6a22
; F5: Ru: CR (RUN:)
    !byte $52,$75,$3a                        ; $6a23
    !byte $0d                                ; $6a26
    !byte $00                                ; $6a27
; F6: SAv"@: (SAVE)
    !byte $53,$41,$76                        ; $6a28
    !byte $22,$40,$3a                        ; $6a2b
    !byte $00                                ; $6a2e
; F7: (intercepted at $6bd5 - opens UCI menu instead)
    !byte $93                                ; $6a2f
    clc                                      ; $6a30 (relocated branch target for clc+rts)
    rts                                      ; $6a31
    !byte $0d                                ; $6a32
    !byte $00                                ; $6a33
; F8: @X CR
    !byte $40,$58                            ; $6a34
    !byte $0d                                ; $6a36
    !byte $00                                ; $6a37
; =============================================================================
; UCI Menu: F7 handler - opens Ultimate menu via UCI freeze command
; Hooked from F-key dispatch at $6bd5 (jmp $6a38)
; Uses Control Target ($04) with Freeze command ($05)
; =============================================================================
    cpx #$07           ; F7 key?              ; $6a38
    beq $6a42          ; Yes: UCI freeze      ; $6a3a
; --- Not F5: restore original dispatch flow ---
    lda $0298          ; Original instruction from $6bd5 ; $6a3c
    jmp $F554          ; Back to beq $6c10 at $6bd8 (tests Z flag) ; $6a3f
; --- UCI freeze: accept previous state, send Control Target + Freeze ---
    lda #$02                                  ; $6a42
    sta $DF1C          ; Accept: reset Data Last → Idle ; $6a44
    lsr                ; A=$01 (reused for push later) ; $6a47
    sta $DC00          ; Deselect keyboard row 0 (hides F7 from menu) ; $6a48
    ldx #$04           ; Control Target       ; $6a4b
    stx $DF1D          ; Send target byte     ; $6a4d
    inx                ; X=$05 (Freeze cmd)   ; $6a50
    stx $DF1D          ; Send command byte    ; $6a51
    sta $DF1C          ; Push command, A=$01 (CPU freezes after this) ; $6a54
    pla                ; Balance stack (from F-key dispatch) ; $6a57
    rts                                       ; $6a58
    lda $b9                                  ; $6a59
    bmi $6a30                                ; $6a5b
    ldy $b7                                  ; $6a5d
    beq $6a30                                ; $6a5f
    lda #$00                                 ; $6a61
    sta $90                                  ; $6a63
    lda $ba                                  ; $6a65
    jsr $ed0c                                ; $6a67
    lda $b9                                  ; $6a6a
    ora #$f0                                 ; $6a6c
    jsr $edb9                                ; $6a6e
    lda $90                                  ; $6a71
    bpl $6a7a                                ; $6a73
    pla                                      ; $6a75
    pla                                      ; $6a76
    jmp $f707                                ; $6a77
    lda $b7                                  ; $6a7a
    beq $6a8a                                ; $6a7c
    ldy #$00                                 ; $6a7e
    lda ($bb),y                              ; $6a80
    jsr $eddd                                ; $6a82
    iny                                      ; $6a85
    cpy $b7                                  ; $6a86
    bne $6a80                                ; $6a88
    jmp $f654                                ; $6a8a
    ldx #$25                                 ; $6a8d
    lda $f08d,x                              ; $6a8f
    jsr $ffd2                                ; $6a92
    dex                                      ; $6a95
    bne $6a8f                                ; $6a96
    lda #$2b                                 ; $6a98
    sta $f9                                  ; $6a9a
    lda #$02                                 ; $6a9c
    sta $fa                                  ; $6a9e
    ldx #$05                                 ; $6aa0
    jsr $f187                                ; $6aa2
    jsr $fb9e                                ; $6aa5
    lda $0229                                ; $6aa8
    ldx $022a                                ; $6aab
    jsr $f894                                ; $6aae
    jsr $fb9e                                ; $6ab1
    ldx #$08                                 ; $6ab4
    ldy $022b                                ; $6ab6
    tya                                      ; $6ab9
    asl                                    ; $6aba
    tay                                      ; $6abb
    lda #$30                                 ; $6abc
    adc #$00                                 ; $6abe
    jsr $ffd2                                ; $6ac0
    dex                                      ; $6ac3
    bne $6ab9                                ; $6ac4
    beq $6b0d                                ; $6ac6
    lda #$2b                                 ; $6ac8
    sta $f7                                  ; $6aca
    lda #$02                                 ; $6acc
    sta $f8                                  ; $6ace
    ldx #$05                                 ; $6ad0
    jsr $f26c                                ; $6ad2
    beq $6aa5                                ; $6ad5
    lda #$00                                 ; $6ad7
    bit $01a9                                ; $6ad9
    pha                                      ; $6adc
    jsr $f1df                                ; $6add
    lda $d3                                  ; $6ae0
    cmp $c8                                  ; $6ae2
    bcs $6aed                                ; $6ae4
    jsr $f05e                                ; $6ae6
    tax                                      ; $6ae9
    ldy $fa                                  ; $6aea
    bit $b9e6                                ; $6aec
    pla                                      ; $6aef
    jsr $ffd5                                ; $6af0
    lda $90                                  ; $6af3
    and #$10                                 ; $6af5
    beq $6b0d                                ; $6af7
    jmp $f030                                ; $6af9
    jsr $f1df                                ; $6afc
    jsr $f72c                                ; $6aff
    jsr $f05e                                ; $6b02
    tax                                      ; $6b05
    ldy $fa                                  ; $6b06
    lda #$f7                                 ; $6b08
    jsr $ffd8                                ; $6b0a
    jmp $f035                                ; $6b0d
    jsr $f1df                                ; $6b10
    jsr $fba6                                ; $6b13
    jmp $f035                                ; $6b16
    lsr                                    ; $6b19
    bne $6b33                                ; $6b1a
    lda #$08                                 ; $6b1c
    sta $ba                                  ; $6b1e
    bne $6b3c                                ; $6b20
    stx $c3                                  ; $6b22
    sty $c4                                  ; $6b24
    jmp ($0330)                              ; $6b26
    sta $93                                  ; $6b29
    lda #$00                                 ; $6b2b
    sta $90                                  ; $6b2d
    lda $ba                                  ; $6b2f
    bne $6b36                                ; $6b31
    jmp $f713                                ; $6b33
    cmp #$03                                 ; $6b36
    beq $6b33                                ; $6b38
    bcc $6b19                                ; $6b3a
    ldy $b7                                  ; $6b3c
    bne $6b43                                ; $6b3e
    jsr $f0f5                                ; $6b40
    ldx $b9                                  ; $6b43
    jsr $f5af                                ; $6b45
    lda #$60                                 ; $6b48
    sta $b9                                  ; $6b4a
    jsr $f3d5                                ; $6b4c
    lda $ba                                  ; $6b4f
    jsr $ed09                                ; $6b51
    lda $b9                                  ; $6b54
    jsr $edc7                                ; $6b56
    jsr $ee13                                ; $6b59
    sta $ae                                  ; $6b5c
    lda $90                                  ; $6b5e
    lsr                                    ; $6b60
    lsr                                    ; $6b61
    bcs $6bb4                                ; $6b62
    jsr $ee13                                ; $6b64
    sta $af                                  ; $6b67
    txa                                      ; $6b69
    bne $6b74                                ; $6b6a
    lda $c3                                  ; $6b6c
    sta $ae                                  ; $6b6e
    lda $c4                                  ; $6b70
    sta $af                                  ; $6b72
    jmp $eebb                                ; $6b74
    lda #$fd                                 ; $6b77
    and $90                                  ; $6b79
    sta $90                                  ; $6b7b
    jsr $ffe1                                ; $6b7d
    bne $6b85                                ; $6b80
    jmp $f633                                ; $6b82
    jsr $ee13                                ; $6b85
    tax                                      ; $6b88
    lda $90                                  ; $6b89
    lsr                                    ; $6b8b
    lsr                                    ; $6b8c
    bcs $6b77                                ; $6b8d
    txa                                      ; $6b8f
    ldy $93                                  ; $6b90
    beq $6ba0                                ; $6b92
    ldy #$00                                 ; $6b94
    cmp ($ae),y                              ; $6b96
    beq $6ba2                                ; $6b98
    lda #$50                                 ; $6b9a
    jsr $fe1c                                ; $6b9c
    bit $ae91                                ; $6b9f
    inc $ae                                  ; $6ba2
    bne $6ba8                                ; $6ba4
    inc $af                                  ; $6ba6
    bit $90                                  ; $6ba8
    bvc $6b77                                ; $6baa
    jsr $edef                                ; $6bac
    jsr $f642                                ; $6baf
    bcc $6c2d                                ; $6bb2
    jmp $f704                                ; $6bb4
    jsr $e5b4                                ; $6bb7
    pha                                      ; $6bba
    cmp #$03                                 ; $6bbb
    beq $6c08                                ; $6bbd
    lda $d4                                  ; $6bbf
    ora $d8                                  ; $6bc1
    bne $6c10                                ; $6bc3
    tya                                      ; $6bc5
    ldx #$0c                                 ; $6bc6
    cmp $fc3e,x                              ; $6bc8
    beq $6bd5                                ; $6bcb
    dex                                      ; $6bcd
    bne $6bc8                                ; $6bce
    jsr $feca                                ; $6bd0
    pla                                      ; $6bd3
    rts                                      ; $6bd4
    jmp $F3B4      ; Hook: check for F7 (UCI menu) ; $6bd5
    beq $6c10                                ; $6bd8
    sta $f8                                  ; $6bda
    lda $0297                                ; $6bdc
    sta $f7                                  ; $6bdf
    ldy #$ff                                 ; $6be1
    dex                                      ; $6be3
    beq $6bf0                                ; $6be4
    iny                                      ; $6be6
    lda ($f7),y                              ; $6be7
    beq $6be3                                ; $6be9
    bne $6be6                                ; $6beb
    jsr $e716                                ; $6bed
    iny                                      ; $6bf0
    lda ($f7),y                              ; $6bf1
    beq $6c06                                ; $6bf3
    cmp #$0d                                 ; $6bf5
    beq $6bfd                                ; $6bf7
    cpx #$00                                 ; $6bf9
    beq $6bed                                ; $6bfb
    sei                                      ; $6bfd
    sta $0277,x                              ; $6bfe
    inx                                      ; $6c01
    cpx #$0a                                 ; $6c02
    bne $6bf0                                ; $6c04
    stx $c6                                  ; $6c06
    ldx #$00                                 ; $6c08
    stx $c7                                  ; $6c0a
    stx $d4                                  ; $6c0c
    stx $d8                                  ; $6c0e
    pla                                      ; $6c10
    rts                                      ; $6c11
    lda $2b                                  ; $6c12
    cmp $c3                                  ; $6c14
    bne $6c22                                ; $6c16
    lda $2c                                  ; $6c18
    cmp $c4                                  ; $6c1a
    bne $6c22                                ; $6c1c
    stx $2d                                  ; $6c1e
    sty $2e                                  ; $6c20
    jmp $e1ab                                ; $6c22
    jsr $ff3b                                ; $6c25
    stx $dd01                                ; $6c28
    rts                                      ; $6c2b
    bit $18                                  ; $6c2c
    ldx $ae                                  ; $6c2e
    ldy $af                                  ; $6c30
    rts                                      ; $6c32
    lda $9d                                  ; $6c33
    bpl $6c55                                ; $6c35
    ldy #$0c                                 ; $6c37
    jsr $f12f                                ; $6c39
    lda $b7                                  ; $6c3c
    beq $6c55                                ; $6c3e
    ldy #$17                                 ; $6c40
    jsr $f12f                                ; $6c42
    ldy $b7                                  ; $6c45
    beq $6c55                                ; $6c47
    ldy #$00                                 ; $6c49
    lda ($bb),y                              ; $6c4b
    jsr $ffd2                                ; $6c4d
    iny                                      ; $6c50
    cpy $b7                                  ; $6c51
    bne $6c4b                                ; $6c53
    rts                                      ; $6c55
    ldy #$49                                 ; $6c56
    lda $93                                  ; $6c58
    beq $6c5e                                ; $6c5a
    ldy #$59                                 ; $6c5c
    jmp $f12b                                ; $6c5e
    stx $ae                                  ; $6c61
    sty $af                                  ; $6c63
    tax                                      ; $6c65
    lda $00,x                                ; $6c66
    sta $c1                                  ; $6c68
    lda $01,x                                ; $6c6a
    sta $c2                                  ; $6c6c
    jmp ($0332)                              ; $6c6e
    lda $ba                                  ; $6c71
    bne $6c78                                ; $6c73
    jmp $f713                                ; $6c75
    cmp #$03                                 ; $6c78
    beq $6c75                                ; $6c7a
    bcc $6cdd                                ; $6c7c
    lda #$61                                 ; $6c7e
    sta $b9                                  ; $6c80
    ldy $b7                                  ; $6c82
    bne $6c89                                ; $6c84
    jmp $f710                                ; $6c86
    jsr $f3d5                                ; $6c89
    jsr $f68f                                ; $6c8c
    lda $ba                                  ; $6c8f
    jsr $ed0c                                ; $6c91
    lda $b9                                  ; $6c94
    jsr $edb9                                ; $6c96
    ldy #$00                                 ; $6c99
    jsr $fb8e                                ; $6c9b
    lda $ac                                  ; $6c9e
    jsr $eddd                                ; $6ca0
    lda $ad                                  ; $6ca3
    jmp $ef6f                                ; $6ca5
    jsr $fcd1                                ; $6ca8
    bcs $6cc3                                ; $6cab
    lda ($ac),y                              ; $6cad
    jsr $eddd                                ; $6caf
    jsr $ffe1                                ; $6cb2
    bne $6cbe                                ; $6cb5
    jsr $f642                                ; $6cb7
    lda #$00                                 ; $6cba
    sec                                      ; $6cbc
    rts                                      ; $6cbd
    jsr $fcdb                                ; $6cbe
    bne $6ca8                                ; $6cc1
    jsr $edfe                                ; $6cc3
    bit $b9                                  ; $6cc6
    bmi $6cdb                                ; $6cc8
    lda $ba                                  ; $6cca
    jsr $ed0c                                ; $6ccc
    lda $b9                                  ; $6ccf
    and #$ef                                 ; $6cd1
    ora #$e0                                 ; $6cd3
    jsr $edb9                                ; $6cd5
    jsr $edfe                                ; $6cd8
    clc                                      ; $6cdb
    rts                                      ; $6cdc
    lsr                                    ; $6cdd
    !byte $90,$95                            ; $6cde (bcc $6c74)
    lda #$08                                 ; $6ce0
    sta $ba                                  ; $6ce2
    !byte $d0,$98                            ; $6ce4 (bne $6c7d)
    jsr $f05e                                ; $6ce6
    ldy $022e                                ; $6ce9
    ldx $022d                                ; $6cec
    lda $022b                                ; $6cef
    pha                                      ; $6cf2
    lda $022c                                ; $6cf3
    plp                                      ; $6cf6
    jmp ($00f9)                              ; $6cf7
    lda #$00                                 ; $6cfa
    sta $90                                  ; $6cfc
    lda $ba                                  ; $6cfe
    jsr $ed0c                                ; $6d00
    lda #$6f                                 ; $6d03
    jsr $edb9                                ; $6d05
    lda $90                                  ; $6d08
    bpl $6d11                                ; $6d0a
    pla                                      ; $6d0c
    pla                                      ; $6d0d
    jmp $f707                                ; $6d0e
    clc                                      ; $6d11
    rts                                      ; $6d12
    lda $9d                                  ; $6d13
    bpl $6d12                                ; $6d15
    ldy #$51                                 ; $6d17
    jsr $f12f                                ; $6d19
    jmp $f5c1                                ; $6d1c
    ldx #$00                                 ; $6d1f
    inc $a2                                  ; $6d21
    bne $6d2b                                ; $6d23
    inc $a1                                  ; $6d25
    bne $6d2b                                ; $6d27
    inc $a0                                  ; $6d29
    sec                                      ; $6d2b
    lda $a2                                  ; $6d2c
    sbc #$01                                 ; $6d2e
    lda $a1                                  ; $6d30
    sbc #$1a                                 ; $6d32
    lda $a0                                  ; $6d34
    sbc #$4f                                 ; $6d36
    bcc $6d40                                ; $6d38
    stx $a0                                  ; $6d3a
    stx $a1                                  ; $6d3c
    stx $a2                                  ; $6d3e
    lda $dc01                                ; $6d40
    cmp $dc01                                ; $6d43
    bne $6d40                                ; $6d46
    tax                                      ; $6d48
    bmi $6d5e                                ; $6d49
    ldx #$bd                                 ; $6d4b
    stx $dc00                                ; $6d4d
    ldx $dc01                                ; $6d50
    cpx $dc01                                ; $6d53
    bne $6d50                                ; $6d56
    sta $dc00                                ; $6d58
    inx                                      ; $6d5b
    bne $6d60                                ; $6d5c
    sta $91                                  ; $6d5e
    rts                                      ; $6d60
    sei                                      ; $6d61
    lda $a2                                  ; $6d62
    ldx $a1                                  ; $6d64
    ldy $a0                                  ; $6d66
    sei                                      ; $6d68
    sta $a2                                  ; $6d69
    stx $a1                                  ; $6d6b
    sty $a0                                  ; $6d6d
    cli                                      ; $6d6f
    rts                                      ; $6d70
    lda $91                                  ; $6d71
    cmp #$7f                                 ; $6d73
    bne $6d7e                                ; $6d75
    php                                      ; $6d77
    jsr $ffcc                                ; $6d78
    sta $c6                                  ; $6d7b
    plp                                      ; $6d7d
    rts                                      ; $6d7e
    lda #$01                                 ; $6d7f
    bit $02a9                                ; $6d81
    bit $03a9                                ; $6d84
    bit $04a9                                ; $6d87
    bit $05a9                                ; $6d8a
    bit $06a9                                ; $6d8d
    bit $07a9                                ; $6d90
    bit $08a9                                ; $6d93
    bit $09a9                                ; $6d96
    pha                                      ; $6d99
    jsr $ffcc                                ; $6d9a
    ldy #$00                                 ; $6d9d
    bit $9d                                  ; $6d9f
    bvc $6dad                                ; $6da1
    jsr $f12f                                ; $6da3
    pla                                      ; $6da6
    pha                                      ; $6da7
    ora #$30                                 ; $6da8
    jsr $ffd2                                ; $6daa
    pla                                      ; $6dad
    sec                                      ; $6dae
    rts                                      ; $6daf
    jsr $f05e                                ; $6db0
    sta $f7                                  ; $6db3
    lda $fa                                  ; $6db5
    sta $f8                                  ; $6db7
    rts                                      ; $6db9
    jsr $ee97                                ; $6dba
    bit $dd01                                ; $6dbd
    and #$40                                 ; $6dc0
    beq $6dba                                ; $6dc2
    lda $c4                                  ; $6dc4
    cmp #$08                                 ; $6dc6
    bcc $6dd1                                ; $6dc8
    lda $9d                                  ; $6dca
    bpl $6dd1                                ; $6dcc
    jsr $f890                                ; $6dce
    jmp $f958                                ; $6dd1
    ldy #$63                                 ; $6dd4
    jsr $f12f                                ; $6dd6
    ldy #$05                                 ; $6dd9
    lda ($b2),y                              ; $6ddb
    jsr $ffd2                                ; $6ddd
    iny                                      ; $6de0
    cpy #$15                                 ; $6de1
    bne $6ddb                                ; $6de3
    lda $a1                                  ; $6de5
    jsr $e4e0                                ; $6de7
    nop                                      ; $6dea
    clc                                      ; $6deb
    dey                                      ; $6dec
    rts                                      ; $6ded
    bit $dc0c                                ; $6dee
    bpl $6df6                                ; $6df1
    inc $dd03                                ; $6df3
    jmp $eea0                                ; $6df6
    bcs $6e44                                ; $6df9
    cmp #$0d                                 ; $6dfb
    bne $6e43                                ; $6dfd
    lda #$00                                 ; $6dff
    sta $0200,x                              ; $6e01
    bit $9d                                  ; $6e04
    bpl $6e1b                                ; $6e06
    ldx #$ff                                 ; $6e08
    inx                                      ; $6e0a
    lda $0200,x                              ; $6e0b
    cmp #$20                                 ; $6e0e
    beq $6e0a                                ; $6e10
    cmp #$40                                 ; $6e12
    bne $6e1b                                ; $6e14
    lda #$22                                 ; $6e16
    sta $0200,x                              ; $6e18
    pla                                      ; $6e1b
    pla                                      ; $6e1c
    jmp $aacf                                ; $6e1d
    eor #$0b                                 ; $6e20
    beq $6e27                                ; $6e22
    jmp $a43a                                ; $6e24
    tay                                      ; $6e27
    pla                                      ; $6e28
    cmp #$a7                                 ; $6e29
    bne $6e24                                ; $6e2b
    pla                                      ; $6e2d
    lda ($7a),y                              ; $6e2e
    cmp #$22                                 ; $6e30
    beq $6e3b                                ; $6e32
    cmp #$40                                 ; $6e34
    bne $6e47                                ; $6e36
    jsr $0073                                ; $6e38
    jsr $e1d4                                ; $6e3b
    jsr $e591                                ; $6e3e
    bcs $6e44                                ; $6e41
    rts                                      ; $6e43
    jmp $e0f9                                ; $6e44
    cmp #$26                                 ; $6e47
    bne $6e24                                ; $6e49
    jsr $0073                                ; $6e4b
    bne $6e53                                ; $6e4e
    jmp $e4b7                                ; $6e50
    cmp #$22                                 ; $6e53
    bne $6e68                                ; $6e55
    jsr $e1d4                                ; $6e57
    lda #$00                                 ; $6e5a
    jsr $ffd5                                ; $6e5c
    bcs $6e44                                ; $6e5f
    lda $c3                                  ; $6e61
    ldx $c4                                  ; $6e63
    jmp $e4bb                                ; $6e65
    cmp #$ac                                 ; $6e68
    bne $6e81                                ; $6e6a
    iny                                      ; $6e6c
    sta ($2b),y                              ; $6e6d
    jsr $a533                                ; $6e6f
    lda $22                                  ; $6e72
    adc #$02                                 ; $6e74
    sta $2d                                  ; $6e76
    lda $23                                  ; $6e78
    adc #$00                                 ; $6e7a
    sta $2e                                  ; $6e7c
    jmp $e1ab                                ; $6e7e
    jsr $f1a7                                ; $6e81
    ldy #$2a                                 ; $6e84
    jsr $f12f                                ; $6e86
    lda $15                                  ; $6e89
    tay                                      ; $6e8b
    ldx $14                                  ; $6e8c
    jsr $f894                                ; $6e8e
    lda #$3d                                 ; $6e91
    jsr $ffd2                                ; $6e93
    tya                                      ; $6e96
    jmp $bdcd                                ; $6e97
    rts                                      ; $6e9a
    jsr $f82e                                ; $6e9b
    beq $6eba                                ; $6e9e
    ldy #$1b                                 ; $6ea0
    jsr $f12f                                ; $6ea2
    jsr $f8d0                                ; $6ea5
    jsr $f82e                                ; $6ea8
    bne $6ea5                                ; $6eab
    ldy #$6a                                 ; $6ead
    jmp $f12f                                ; $6eaf
    lda #$10                                 ; $6eb2
    bit $01                                  ; $6eb4
    bne $6eba                                ; $6eb6
    bit $01                                  ; $6eb8
    clc                                      ; $6eba
    rts                                      ; $6ebb
    jsr $f82e                                ; $6ebc
    beq $6eba                                ; $6ebf
    ldy #$21                                 ; $6ec1
    bne $6ea2                                ; $6ec3
    bit $dc0c                                ; $6ec5
    bpl $6f0e                                ; $6ec8
    stx $a5                                  ; $6eca
    bit $dd00                                ; $6ecc
    bvc $6ecc                                ; $6ecf
    lda $dd00                                ; $6ed1
    and #$df                                 ; $6ed4
    sta $dd00                                ; $6ed6
    ldx #$05                                 ; $6ed9
    bit $dd00                                ; $6edb
    bvc $6efd                                ; $6ede
    dex                                      ; $6ee0
    bne $6edb                                ; $6ee1
    jsr $eea0                                ; $6ee3
    lda #$40                                 ; $6ee6
    jsr $fe1c                                ; $6ee8
    jsr $ee97                                ; $6eeb
    ldx #$05                                 ; $6eee
    bit $dd00                                ; $6ef0
    bvc $6efd                                ; $6ef3
    dex                                      ; $6ef5
    bne $6ef0                                ; $6ef6
    lda #$02                                 ; $6ef8
    jmp $f9b7                                ; $6efa
    ldx $dd01                                ; $6efd
    ora #$20                                 ; $6f00
    sta $dd00                                ; $6f02
    stx $a4                                  ; $6f05
    ldx $a5                                  ; $6f07
    lda $a4                                  ; $6f09
    clc                                      ; $6f0b
    cli                                      ; $6f0c
    rts                                      ; $6f0d
    jsr $ee85                                ; $6f0e
    jmp $ee1b                                ; $6f11
    ldx $ae                                  ; $6f14
    lda $af                                  ; $6f16
    jsr $f898                                ; $6f18
    txa                                      ; $6f1b
    pha                                      ; $6f1c
    lsr                                    ; $6f1d
    lsr                                    ; $6f1e
    lsr                                    ; $6f1f
    lsr                                    ; $6f20
    jsr $f8a3                                ; $6f21
    pla                                      ; $6f24
    and #$0f                                 ; $6f25
    clc                                      ; $6f27
    adc #$30                                 ; $6f28
    cmp #$3a                                 ; $6f2a
    bcc $6f30                                ; $6f2c
    adc #$06                                 ; $6f2e
    jmp $ffd2                                ; $6f30
    php                                      ; $6f33
    !byte $f4                                ; $6f34 (undefined opcode)
    !byte $e3                                ; $6f35 (undefined opcode)
    sbc $a473,y                              ; $6f36
    !byte $53                                ; $6f39 (undefined opcode)
    !byte $fa                                ; $6f3a (undefined opcode)
    !byte $43                                ; $6f3b (undefined opcode)
    !byte $f4                                ; $6f3c (undefined opcode)
    adc ($f6,x)                              ; $6f3d
    !byte $77                                ; $6f3f (undefined opcode)
    !byte $f4                                ; $6f40 (undefined opcode)
    !byte $52                                ; $6f41 (undefined opcode)
    !byte $f4                                ; $6f42 (undefined opcode)
    eor $f4,x                                ; $6f43
    !byte $8b                                ; $6f45 (undefined opcode)
    !byte $f4                                ; $6f46 (undefined opcode)
    sbc ($f9,x)                              ; $6f47
    lda #$fc                                 ; $6f49
    !byte $f3                                ; $6f4b (undefined opcode)
    sbc $fa02,y                              ; $6f4c
    ora ($0a,x)                              ; $6f4f
    !byte $64                                ; $6f51 (undefined opcode)
    inx                                      ; $6f52
    bpl $6f75                                ; $6f53
    sbc ($ff,x)                              ; $6f55
    clc                                      ; $6f57
    bne $6f60                                ; $6f58
    jsr $fc93                                ; $6f5a
    sec                                      ; $6f5d
    pla                                      ; $6f5e
    pla                                      ; $6f5f
    rts                                      ; $6f60
    bit $dc0c                                ; $6f61
    bvs $6f94                                ; $6f64
    pha                                      ; $6f66
    jsr $ed36                                ; $6f67
    sei                                      ; $6f6a
    pla                                      ; $6f6b
    and #$f0                                 ; $6f6c
    cmp #$e0                                 ; $6f6e
    beq $6f92                                ; $6f70
    stx $a5                                  ; $6f72
    ldx #$19                                 ; $6f74
    lda #$10                                 ; $6f76
    bit $dd0d                                ; $6f78
    dex                                      ; $6f7b
    beq $6f90                                ; $6f7c
    bit $dd01                                ; $6f7e
    bit $dd0d                                ; $6f81
    beq $6f7b                                ; $6f84
    asl $dc0c                                ; $6f86
    sec                                      ; $6f89
    ror $dc0c                                ; $6f8a
    jsr $f1b8                                ; $6f8d
    ldx $a5                                  ; $6f90
    cli                                      ; $6f92
    rts                                      ; $6f93
    and #$0f                                 ; $6f94
    cmp #$07                                 ; $6f96
    bne $6fd3                                ; $6f98
    lda #$43                                 ; $6f9a
    bne $6fd0                                ; $6f9c
    jsr $f1b8                                ; $6f9e
    txa                                      ; $6fa1
    pha                                      ; $6fa2
    lda $dc0c                                ; $6fa3
    and #$02                                 ; $6fa6
    bne $6fcc                                ; $6fa8
    lda $dd0d                                ; $6faa
    lda #$16                                 ; $6fad
    jsr $e4ec                                ; $6faf
    ldx #$13                                 ; $6fb2
    stx $a5                                  ; $6fb4
    lda $dd0d                                ; $6fb6
    and #$10                                 ; $6fb9
    bne $6fcc                                ; $6fbb
    dex                                      ; $6fbd
    bne $6fb6                                ; $6fbe
    dec $a5                                  ; $6fc0
    bne $6fb6                                ; $6fc2
    pla                                      ; $6fc4
    tax                                      ; $6fc5
    jsr $f958                                ; $6fc6
    jmp $ed2e                                ; $6fc9
    pla                                      ; $6fcc
    tax                                      ; $6fcd
    lda #$42                                 ; $6fce
    sta $dc0c                                ; $6fd0
    bne $7036                                ; $6fd3
    bit $dc0c                                ; $6fd5
    bvs $6fdc                                ; $6fd8
    bpl $6ff2                                ; $6fda
    pha                                      ; $6fdc
    lda $dc0c                                ; $6fdd
    and #$02                                 ; $6fe0
    sta $dc0c                                ; $6fe2
    lda $0296                                ; $6fe5
    sta $dd03                                ; $6fe8
    lda $0295                                ; $6feb
    sta $dd01                                ; $6fee
    pla                                      ; $6ff1
    rts                                      ; $6ff2
; =============================================================================
; PARALLEL_TRANSFER_CORE - Core parallel byte send routine
; Entry: $95 = byte to send, X = saved across call
; Exit: C=0 success, C=1 error, A=$80 on timeout
; Uses $DD00 bit 4 for handshake, $DD01 for data
; =============================================================================
    stx $a5        ; Save X register         ; $6ff3
    lda $dc0c      ; Read CIA#1 serial port register ; $6ff5
    bpl $7043      ; Branch if bit 7 clear (not in parallel mode) ; $6ff8
    lda $dd00      ; Read CIA#2 port A       ; $6ffa
    bmi $7039      ; Abort if DATA IN high (bit 7 set): unexpected bus state ; $6ffd
    and #$ef       ; Clear bit 4 (signal "ready to send") ; $6fff
    sta $dd00                                ; $7001
    bit $a3        ; Check handshake mode flag ; $7004
    bpl $7019      ; Skip sync wait if not needed ; $7006
; --- Wait for drive acknowledgment (bit 7 high) ---
    bit $dd00      ; Test bit 7              ; $7008
    bpl $7008      ; Loop until drive signals ready ; $700b
    ldx #$1e       ; Timeout counter (30)    ; $700d
    lda $dd00      ; Read port               ; $700f
    bpl $7019      ; Continue if bit 7 went low ; $7012
    dex ; Decrement timeout                  ; $7014
    bne $700f      ; Loop until timeout      ; $7015
    beq $7025      ; Timeout: skip data write ; $7017
; --- Wait for drive ready signal (bit 7 high) ---
    lda $dd00      ; Read port               ; $7019
    bpl $7019      ; Wait for bit 7 high     ; $701c
; --- Send byte via parallel port ---
    ldx $95        ; Get byte to send        ; $701e
    stx $dd01      ; WRITE BYTE TO PARALLEL PORT ; $7020
    ldx #$1e       ; Timeout counter (30)    ; $7023
; --- Signal data ready and wait for acknowledge ---
    ora #$10       ; Set bit 4 (signal "data valid") ; $7025
    sta $dd00                                ; $7027
    lda #$03       ; Error code if timeout   ; $702a
    dex ; Decrement timeout counter          ; $702c
    bmi $703b      ; Timeout expired: error  ; $702d
    bit $dd00      ; Test bit 7              ; $702f
    bmi $702c      ; Wait for bit 7 low (drive acknowledged) ; $7032
; --- Success exit ---
    ldx $a5        ; Restore X register      ; $7034
    clc ; Clear carry (success)              ; $7036
    cli ; Re-enable interrupts               ; $7037
    rts                                      ; $7038
; --- Error exit ---
    lda #$80       ; Device not present error ; $7039
    ldx $a5        ; Restore X               ; $703b
    jsr $f951      ; IEC bus cleanup         ; $703d
    jmp $edb2      ; Return with error status ; $7040
    lsr                                    ; $7043
    and #$20                                 ; $7044
    beq $7063                                ; $7046
    lda $95                                  ; $7048
    bcc $7057                                ; $704a
    ldx #$0a                                 ; $704c
    dex                                      ; $704e
    cmp $f0b3,x                              ; $704f
    bcc $704e                                ; $7052
    sbc $f19b,x                              ; $7054
    jsr $e4ec                                ; $7057
    lda #$10                                 ; $705a
    bit $dd0d                                ; $705c
    beq $705c                                ; $705f
    bne $7034                                ; $7061
    jmp $ed44                                ; $7063
    lda #$20                                 ; $7066
    sta $0299                                ; $7068
    lda $d3                                  ; $706b
    cmp $c8                                  ; $706d
    beq $7087                                ; $706f
    jsr $f05e                                ; $7071
    lda #$01                                 ; $7074
    bne $7091                                ; $7076
    ldx $fa                                  ; $7078
    dex                                      ; $707a
    lda $0299                                ; $707b
    cmp #$20                                 ; $707e
    bne $7085                                ; $7080
    dex                                      ; $7082
    dex                                      ; $7083
    dex                                      ; $7084
    stx $fa                                  ; $7085
    jsr $e566                                ; $7087
    lda #$0b                                 ; $708a
    jsr $ffd2                                ; $708c
    lda #$11                                 ; $708f
    sta $f7                                  ; $7091
    ldx $0299                                ; $7093
    cpx #$20                                 ; $7096
    php                                      ; $7098
    beq $709d                                ; $7099
    ldx #$3a                                 ; $709b
    jsr $fb97                                ; $709d
    ldx $f9                                  ; $70a0
    lda $fa                                  ; $70a2
    jsr $f894                                ; $70a4
    plp                                      ; $70a7
    beq $70b2                                ; $70a8
    ldx #$08                                 ; $70aa
    stx $0299                                ; $70ac
    jsr $f187                                ; $70af
    ldy #$00                                 ; $70b2
    jsr $fb9e                                ; $70b4
    lda ($f9),y                              ; $70b7
    jsr $f2a9                                ; $70b9
    iny                                      ; $70bc
    cpy $0299                                ; $70bd
    bne $70b7                                ; $70c0
    dec $f7                                  ; $70c2
    beq $70d5                                ; $70c4
    tya                                      ; $70c6
    clc                                      ; $70c7
    adc $f9                                  ; $70c8
    sta $f9                                  ; $70ca
    bcc $70d0                                ; $70cc
    inc $fa                                  ; $70ce
    jsr $ffe1                                ; $70d0
    bne $7093                                ; $70d3
    jmp $f035                                ; $70d5
    jsr $f72c                                ; $70d8
    ldx #$08                                 ; $70db
    jsr $f26c                                ; $70dd
    lda $f7                                  ; $70e0
    sta $f9                                  ; $70e2
    lda #$01                                 ; $70e4
    sta $f7                                  ; $70e6
    lda #$02                                 ; $70e8
    sta $d3                                  ; $70ea
    php                                      ; $70ec
    !byte $d0,$b1                            ; $70ed (bne $709f)
    cmp #$07                                 ; $70ef
    bne $7103                                ; $70f1
    tya                                      ; $70f3
    adc #$13                                 ; $70f4
    tay                                      ; $70f6
    cmp $d5                                  ; $70f7
    bcc $7100                                ; $70f9
    beq $7100                                ; $70fb
    jmp $e7b6                                ; $70fd
    jmp $e797                                ; $7100
    cmp #$0c                                 ; $7103
    bne $710c                                ; $7105
    jsr $ea04                                ; $7107
    bmi $713e                                ; $710a
    cmp #$02                                 ; $710c
    bne $711b                                ; $710e
    lda #$00                                 ; $7110
    sta $d3                                  ; $7112
    ldy #$18                                 ; $7114
    jsr $e56a                                ; $7116
    bmi $713e                                ; $7119
    cmp #$0b                                 ; $711b
    bne $7132                                ; $711d
    jsr $ea24                                ; $711f
    lda #$20                                 ; $7122
    sta ($d1),y                              ; $7124
    jsr $e4da                                ; $7126
    iny                                      ; $7129
    cpy $d5                                  ; $712a
    bcc $7122                                ; $712c
    beq $7122                                ; $712e
    bcs $713e                                ; $7130
    cmp #$01                                 ; $7132
    bne $7141                                ; $7134
    lda $028a                                ; $7136
    eor #$80                                 ; $7139
    sta $028a                                ; $713b
    jmp $e6ae                                ; $713e
    jmp $ec44                                ; $7141
    sta $f7                                  ; $7144
    stx $f8                                  ; $7146
    lda #$31                                 ; $7148
    sta $f9                                  ; $714a
    ldx #$04                                 ; $714c
    dec $f9                                  ; $714e
    lda #$2f                                 ; $7150
    sta $fa                                  ; $7152
    sec                                      ; $7154
    lda $f8                                  ; $7155
    sbc $f8cb,x                              ; $7157
    sta $f8                                  ; $715a
    lda $f7                                  ; $715c
    sbc $f196,x                              ; $715e
    sta $f7                                  ; $7161
    inc $fa                                  ; $7163
    bcs $7155                                ; $7165
    lda $f8                                  ; $7167
    adc $f8cb,x                              ; $7169
    sta $f8                                  ; $716c
    lda $f7                                  ; $716e
    adc $f196,x                              ; $7170
    sta $f7                                  ; $7173
    lda $fa                                  ; $7175
    cmp $f9                                  ; $7177
    beq $7180                                ; $7179
    jsr $ffd2                                ; $717b
    dec $f9                                  ; $717e
    dex                                      ; $7180
    beq $714e                                ; $7181
    bpl $7150                                ; $7183
    rts                                      ; $7185
    adc #$10                                 ; $7186
    tax                                      ; $7188
    txs                                      ; $7189
    lsr $dc00                                ; $718a
    ldx $dc01                                ; $718d
    inx                                      ; $7190
    bne $718d                                ; $7191
    dex                                      ; $7193
    rts                                      ; $7194
    sta $028c                                ; $7195
    sty $c5                                  ; $7198
    cpx #$fd                                 ; $719a
    beq $71b2                                ; $719c
    cpx #$fe                                 ; $719e
    bne $71b1                                ; $71a0
    ldy #$0e                                 ; $71a2
    lda $fb02,y                              ; $71a4
    pha                                      ; $71a7
    dey                                      ; $71a8
    bpl $71a4                                ; $71a9
    tsx                                      ; $71ab
    lda #$01                                 ; $71ac
    pha                                      ; $71ae
    txa                                      ; $71af
    pha                                      ; $71b0
    rts                                      ; $71b1
    jsr $ed0f                                ; $71b2
    lda $d018                                ; $71b5
    and #$02                                 ; $71b8
    beq $71be                                ; $71ba
    lda #$07                                 ; $71bc
    ora #$60                                 ; $71be
    jsr $edb9                                ; $71c0
    lda $0288                                ; $71c3
    sta $f8                                  ; $71c6
    lda #$00                                 ; $71c8
    sta $f7                                  ; $71ca
    sta $dc00                                ; $71cc
    ldx #$19                                 ; $71cf
    ldy #$00                                 ; $71d1
    lda #$0d                                 ; $71d3
    jsr $eddd                                ; $71d5
    dex                                      ; $71d8
    bmi $71e2                                ; $71d9
    bit $dc01                                ; $71db
    bmi $71e5                                ; $71de
    ldx #$ff                                 ; $71e0
    jmp $edfe                                ; $71e2
    lda ($f7),y                              ; $71e5
    sta $f9                                  ; $71e7
    and #$3f                                 ; $71e9
    asl $f9                                  ; $71eb
    bit $f9                                  ; $71ed
    bpl $71f3                                ; $71ef
    ora #$80                                 ; $71f1
    bvs $71f7                                ; $71f3
    ora #$40                                 ; $71f5
    jsr $eddd                                ; $71f7
    iny                                      ; $71fa
    cpy #$28                                 ; $71fb
    bne $71e5                                ; $71fd
    tya                                      ; $71ff
    clc                                      ; $7200
    adc $f7                                  ; $7201
    sta $f7                                  ; $7203
    bcc $71d1                                ; $7205
    inc $f8                                  ; $7207
    bne $71d1                                ; $7209
    jsr $1c20                                ; $720b
    inc $bc4c,x                              ; $720e
    inc $c2a5,x                              ; $7211
    sta $ad                                  ; $7214
    lda $c1                                  ; $7216
    sta $ac                                  ; $7218
    rts                                      ; $721a
    jsr $fec2                                ; $721b
    txa                                      ; $721e
    jmp $ffd2                                ; $721f
    lda #$20                                 ; $7222
    bit $0da9                                ; $7224
    jmp $ffd2                                ; $7227
    jsr $fba1                                ; $722a
    ldy $b7                                  ; $722d
    beq $724e                                ; $722f
    ldy #$00                                 ; $7231
    lda ($bb),y                              ; $7233
    cmp #$30                                 ; $7235
    bcc $7244                                ; $7237
    cmp #$3a                                 ; $7239
    bcs $7248                                ; $723b
    sbc #$2f                                 ; $723d
    sta $0294                                ; $723f
    clc                                      ; $7242
    rts                                      ; $7243
    cmp #$24                                 ; $7244
    beq $726c                                ; $7246
    jsr $f676                                ; $7248
    jsr $f3fa                                ; $724b
    lda $b9                                  ; $724e
    beq $7242                                ; $7250
    jsr $f676                                ; $7252
    lda $ba                                  ; $7255
    jsr $ed09                                ; $7257
    lda #$6f                                 ; $725a
    jsr $edc7                                ; $725c
    jsr $ee13                                ; $725f
    jsr $ffd2                                ; $7262
    bit $90                                  ; $7265
    bvc $725f                                ; $7267
    jmp $edef                                ; $7269
    sty $90                                  ; $726c
    ldx $b9                                  ; $726e
    sty $b9                                  ; $7270
    jsr $f3d5                                ; $7272
    lda $ba                                  ; $7275
    jsr $ed09                                ; $7277
    lda #$60                                 ; $727a
    jsr $edc7                                ; $727c
    txa                                      ; $727f
    tay                                      ; $7280
    ldx #$04                                 ; $7281
    bit $02a2                                ; $7283
    jsr $ee13                                ; $7286
    lda $90                                  ; $7289
    bne $72a8                                ; $728b
    dex                                      ; $728d
    bne $7286                                ; $728e
    jsr $ee13                                ; $7290
    tax                                      ; $7293
    jsr $ee13                                ; $7294
    jsr $fac0                                ; $7297
    lda #$20                                 ; $729a
    jsr $ffd2                                ; $729c
    jsr $ee13                                ; $729f
    beq $72b8                                ; $72a2
    ldx $90                                  ; $72a4
    beq $729c                                ; $72a6
    jsr $f642                                ; $72a8
    sty $b9                                  ; $72ab
    lda $90                                  ; $72ad
    and #$bf                                 ; $72af
    !byte $f0,$8f                            ; $72b1 (beq $7241)
    jsr $fba1                                ; $72b3
    !byte $d0,$96                            ; $72b6 (bne $724d)
    jsr $fba1                                ; $72b8
    jsr $ffe1                                ; $72bb
    bne $7284                                ; $72be
    jmp $f633                                ; $72c0
    sta $89                                  ; $72c3
    stx $8a                                  ; $72c5
    !byte $87                                ; $72c7 (undefined opcode)
    !byte $8b                                ; $72c8 (undefined opcode)
    dey                                      ; $72c9
    sty $8280                                ; $72ca
    sty $8f                                  ; $72cd
    eor #$24                                 ; $72cf
    beq $72d9                                ; $72d1
    jsr $ad8a                                ; $72d3
    jmp $b7f7                                ; $72d6
    sta $15                                  ; $72d9
    sta $14                                  ; $72db
    jsr $0073                                ; $72dd
    beq $7313                                ; $72e0
    bcc $72ea                                ; $72e2
    cmp #$41                                 ; $72e4
    bcc $7313                                ; $72e6
    sbc #$08                                 ; $72e8
    sbc #$2f                                 ; $72ea
    cmp #$10                                 ; $72ec
    bcs $72f5                                ; $72ee
    jsr $fc80                                ; $72f0
    bcc $72dd                                ; $72f3
    cmp #$5f                                 ; $72f5
    bne $7313                                ; $72f7
    lda #$0d                                 ; $72f9
    jsr $fc80                                ; $72fb
    sta $15                                  ; $72fe
    lda #$ef                                 ; $7300
    bne $72db                                ; $7302
    ldx #$03                                 ; $7304
    asl $14                                  ; $7306
    rol $15                                  ; $7308
    bcs $7314                                ; $730a
    dex                                      ; $730c
    bpl $7306                                ; $730d
    ora $14                                  ; $730f
    sta $14                                  ; $7311
    rts                                      ; $7313
    jmp $b248                                ; $7314
    php                                      ; $7317
    sei                                      ; $7318
    lda $d011                                ; $7319
    ora #$10                                 ; $731c
    sta $d011                                ; $731e
    jsr $fcca                                ; $7321
    lda #$7f                                 ; $7324
    sta $dc0d                                ; $7326
    jsr $fddd                                ; $7329
    plp                                      ; $732c
    rts                                      ; $732d
    jsr $f05e                                ; $732e
    jsr $f1df                                ; $7331
    clc                                      ; $7334
    tay                                      ; $7335
    dey                                      ; $7336
    bmi $733f                                ; $7337
    lda ($bb),y                              ; $7339
    sta ($f9),y                              ; $733b
    bcc $7336                                ; $733d
    lda $f9                                  ; $733f
    adc $b7                                  ; $7341
    tax                                      ; $7343
    lda $fa                                  ; $7344
    adc #$00                                 ; $7346
    jsr $f894                                ; $7348
    jmp $f035                                ; $734b
    lda $01                                  ; $734e
    ora #$20                                 ; $7350
    sta $01                                  ; $7352
    rts                                      ; $7354
    sec                                      ; $7355
    lda $ac                                  ; $7356
    sbc $ae                                  ; $7358
    lda $ad                                  ; $735a
    sbc $af                                  ; $735c
    rts                                      ; $735e
    inc $ac                                  ; $735f
    bne $7365                                ; $7361
    inc $ad                                  ; $7363
    rts                                      ; $7365
    ldx #$ff                                 ; $7366
    sei                                      ; $7368
    txs                                      ; $7369
    cld                                      ; $736a
    jsr $fe72                                ; $736b
    bne $7373                                ; $736e
    jmp ($8000)                              ; $7370
    stx $d016                                ; $7373
    jsr $fda3                                ; $7376
    jsr $fd50                                ; $7379
    jsr $fd15                                ; $737c
    jsr $ff5b                                ; $737f
    cli                                      ; $7382
    jmp ($a000)                              ; $7383
    ldx #$05                                 ; $7386
    lda $fd0f,x                              ; $7388
    cmp $8003,x                              ; $738b
    bne $7393                                ; $738e
    dex                                      ; $7390
    bne $7388                                ; $7391
    rts                                      ; $7393
    !byte $c3                                ; $7394 (undefined opcode)
    !byte $c2                                ; $7395 (undefined opcode)
    cmp $3038                                ; $7396
    ldx #$30                                 ; $7399
    ldy #$fd                                 ; $739b
    clc                                      ; $739d
    stx $c3                                  ; $739e
    sty $c4                                  ; $73a0
    ldy #$1f                                 ; $73a2
    lda ($c3),y                              ; $73a4
    bcc $73ad                                ; $73a6
    lda $0314,y                              ; $73a8
    sta ($c3),y                              ; $73ab
    sta $0314,y                              ; $73ad
    dey                                      ; $73b0
    bpl $73a4                                ; $73b1
    rts                                      ; $73b3
    and ($ea),y                              ; $73b4
    ror $fe                                  ; $73b6
    !byte $47                                ; $73b8 (undefined opcode)
    inc $f34a,x                              ; $73b9
    sta ($f2),y                              ; $73bc
    asl $50f2                                ; $73be
    !byte $f2                                ; $73c1 (undefined opcode)
    !byte $33                                ; $73c2 (undefined opcode)
    !byte $f3                                ; $73c3 (undefined opcode)
    !byte $57                                ; $73c4 (undefined opcode)
    sbc ($ca),y                              ; $73c5
    sbc ($ed),y                              ; $73c7
    inc $3e,x                                ; $73c9
    sbc ($2f),y                              ; $73cb
    !byte $f3                                ; $73cd (undefined opcode)
    ror $fe                                  ; $73ce
    lda $f4                                  ; $73d0
    sbc $a9f5                                ; $73d2
    brk                                      ; $73d5
    tay                                      ; $73d6
    sta $0002,y                              ; $73d7
    sta $0200,y                              ; $73da
    sta $023c,y                              ; $73dd
    iny                                      ; $73e0
    bne $73d7                                ; $73e1
    ldx #$3c                                 ; $73e3
    ldy #$03                                 ; $73e5
    stx $b2                                  ; $73e7
    sty $b3                                  ; $73e9
    tay                                      ; $73eb
    sta $dc0c                                ; $73ec
    lda #$80                                 ; $73ef
    ldx #$20                                 ; $73f1
    sta $c2                                  ; $73f3
    lda ($c1),y                              ; $73f5
    eor #$ff                                 ; $73f7
    sta ($c1),y                              ; $73f9
    cmp ($c1),y                              ; $73fb
    php                                      ; $73fd
    eor #$ff                                 ; $73fe
    sta ($c1),y                              ; $7400
    plp                                      ; $7402
    bne $740d                                ; $7403
    iny                                      ; $7405
    bne $73f5                                ; $7406
    inc $c2                                  ; $7408
    dex                                      ; $740a
    bne $73f5                                ; $740b
    tya                                      ; $740d
    tax                                      ; $740e
    ldy $c2                                  ; $740f
    clc                                      ; $7411
    jsr $fe2d                                ; $7412
    lda #$08                                 ; $7415
    sta $0282                                ; $7417
    lda #$04                                 ; $741a
    sta $0288                                ; $741c
    rts                                      ; $741f
    cpx $c9                                  ; $7420
    beq $741f                                ; $7422
    jmp $e6ed                                ; $7424
    lda #$7f                                 ; $7427
    sta $dc0d                                ; $7429
    sta $dd0d                                ; $742c
    sta $dc00                                ; $742f
    lda #$08                                 ; $7432
    sta $dc0e                                ; $7434
    sta $dd0e                                ; $7437
    sta $dc0f                                ; $743a
    sta $dd0f                                ; $743d
    ldx #$00                                 ; $7440
    stx $dc03                                ; $7442
    stx $dd03                                ; $7445
    stx $d418                                ; $7448
    dex                                      ; $744b
    stx $dc02                                ; $744c
    lda #$07                                 ; $744f
    sta $dd00                                ; $7451
    lda #$3f                                 ; $7454
    sta $dd02                                ; $7456
    lda #$e7                                 ; $7459
    sta $01                                  ; $745b
    lda #$2f                                 ; $745d
    sta $00                                  ; $745f
    lda $02a6                                ; $7461
    beq $7470                                ; $7464
    lda #$25                                 ; $7466
    sta $dc04                                ; $7468
    lda #$40                                 ; $746b
    jmp $fdf3                                ; $746d
    lda #$95                                 ; $7470
    sta $dc04                                ; $7472
    lda #$42                                 ; $7475
    sta $dc05                                ; $7477
    jmp $ff6e                                ; $747a
    sta $b7                                  ; $747d
    stx $bb                                  ; $747f
    sty $bc                                  ; $7481
    rts                                      ; $7483
    sta $b8                                  ; $7484
    stx $ba                                  ; $7486
    sty $b9                                  ; $7488
    rts                                      ; $748a
    lda $ba                                  ; $748b
    cmp #$02                                 ; $748d
    bne $749e                                ; $748f
    lda $0297                                ; $7491
    pha                                      ; $7494
    lda #$00                                 ; $7495
    sta $0297                                ; $7497
    pla                                      ; $749a
    rts                                      ; $749b
    sta $9d                                  ; $749c
    lda $90                                  ; $749e
    ora $90                                  ; $74a0
    sta $90                                  ; $74a2
    rts                                      ; $74a4
    sta $0285                                ; $74a5
    rts                                      ; $74a8
    bcc $74b1                                ; $74a9
    ldx $0283                                ; $74ab
    ldy $0284                                ; $74ae
    stx $0283                                ; $74b1
    sty $0284                                ; $74b4
    rts                                      ; $74b7
    bcc $74c0                                ; $74b8
    ldx $0281                                ; $74ba
    ldy $0282                                ; $74bd
    stx $0281                                ; $74c0
    sty $0282                                ; $74c3
    rts                                      ; $74c6
    sei                                      ; $74c7
    jmp ($0318)                              ; $74c8
    pha                                      ; $74cb
    txa                                      ; $74cc
    pha                                      ; $74cd
    tya                                      ; $74ce
    pha                                      ; $74cf
    lda #$7f                                 ; $74d0
    sta $dd0d                                ; $74d2
    ldy $dd0d                                ; $74d5
    bmi $753a                                ; $74d8
    jsr $fd02                                ; $74da
    bne $74e2                                ; $74dd
    jmp ($8002)                              ; $74df
    jsr $f6bc                                ; $74e2
    jsr $fe7e                                ; $74e5
    bne $753a                                ; $74e8
    jsr $fd15                                ; $74ea
    jsr $fda3                                ; $74ed
    jsr $e518                                ; $74f0
    jmp ($a002)                              ; $74f3
    jsr $fda3                                ; $74f6
    lda $dc01                                ; $74f9
    jsr $fe81                                ; $74fc
    jmp $fd02                                ; $74ff
    jsr $ffe1                                ; $7502
    rol                                    ; $7505
    bcc $74ea                                ; $7506
    rol                                    ; $7508
    bcc $74ed                                ; $7509
    rol                                    ; $750b
    rol                                    ; $750c
    bcs $7512                                ; $750d
    jmp $fcef                                ; $750f
    rol                                    ; $7512
    rol                                    ; $7513
    bcs $7527                                ; $7514
    jsr $fda3                                ; $7516
    jsr $e518                                ; $7519
    tsx                                      ; $751c
    inx                                      ; $751d
    inx                                      ; $751e
    txs                                      ; $751f
    cpx #$fa                                 ; $7520
    bcs $7538                                ; $7522
    jmp $f01d                                ; $7524
    rol                                    ; $7527
    bcs $7539                                ; $7528
    bmi $7539                                ; $752a
    ldx #$17                                 ; $752c
    lda $e4c2,x                              ; $752e
    sta $e8,x                                ; $7531
    dex                                      ; $7533
    bpl $752e                                ; $7534
    jmp $00e8                                ; $7536
    rts                                      ; $7539
    lda $02a1                                ; $753a
    sta $dd0d                                ; $753d
    pla                                      ; $7540
    tay                                      ; $7541
    pla                                      ; $7542
    tax                                      ; $7543
    pla                                      ; $7544
    rti                                      ; $7545
    jsr $fba1                                ; $7546
    lda #$2e                                 ; $7549
    jmp $ffd2                                ; $754b
    cmp #$04                                 ; $754e
    bne $7563                                ; $7550
    lda #$01                                 ; $7552
    ldx #$cd                                 ; $7554
    ldy #$eb                                 ; $7556
    jsr $ffbd                                ; $7558
    ldx #$08                                 ; $755b
    jsr $e101                                ; $755d
    jmp $fba6                                ; $7560
    tya                                      ; $7563
    beq $7556                                ; $7564
    eor #$18                                 ; $7566
    bne $757b                                ; $7568
    ldy #$06                                 ; $756a
    sta $0293,y                              ; $756c
    dey                                      ; $756f
    bpl $756c                                ; $7570
    ldy #$cc                                 ; $7572
    sta $0333,y                              ; $7574
    dey                                      ; $7577
    bne $7574                                ; $7578
    rts                                      ; $757a
    cpy #$0f                                 ; $757b
    bne $7596                                ; $757d
    ldx $0293                                ; $757f
    bmi $757a                                ; $7582
    ldy $d3                                  ; $7584
    dey                                      ; $7586
    bmi $757a                                ; $7587
    lda ($d1),y                              ; $7589
    sta $0380,x                              ; $758b
    inc $0293                                ; $758e
    lda #$14                                 ; $7591
    jmp $e716                                ; $7593
    cpy #$10                                 ; $7596
    bne $75b2                                ; $7598
    ldx $0293                                ; $759a
    beq $757a                                ; $759d
    lda #$94                                 ; $759f
    jsr $e716                                ; $75a1
    lda $d8                                  ; $75a4
    beq $757a                                ; $75a6
    dex                                      ; $75a8
    stx $0293                                ; $75a9
    lda $0380,x                              ; $75ac
    jmp $e69f                                ; $75af
    cpy #$16                                 ; $75b2
    !byte $d0,$c4                            ; $75b4 (bne $7579)
    lda $e535                                ; $75b6
    sta $0286                                ; $75b9
    jmp $e5a0                                ; $75bc
; =============================================================================
; WAIT_PARALLEL_HANDSHAKE - Wait for drive to signal via FLAG or bit 4
; Exit: Returns when either FLAG interrupt or $DD00 bit 4 goes high
; =============================================================================
    lda #$10       ; Bit mask for FLAG (bit 4 of ICR) ; $75bf
    bit $dd0d      ; Check CIA#2 interrupt control register ; $75c1
    bne $75cb      ; Exit if FLAG interrupt occurred ; $75c4
    bit $dd00      ; Check bit 4 of port A   ; $75c6
    bpl $75c1      ; Loop until bit 7 high (drive signaled) ; $75c9
    rts                                      ; $75cb
    pha                                      ; $75cc
    txa                                      ; $75cd
    pha                                      ; $75ce
    tya                                      ; $75cf
    pha                                      ; $75d0
    tsx                                      ; $75d1
    lda $0104,x                              ; $75d2
    and #$10                                 ; $75d5
    beq $75dc                                ; $75d7
    jmp $f01d                                ; $75d9
    jmp ($0314)                              ; $75dc
    jsr $e518                                ; $75df
    lda $d012                                ; $75e2
    bne $75e2                                ; $75e5
    lda $d019                                ; $75e7
    and #$01                                 ; $75ea
    sta $02a6                                ; $75ec
    jmp $fddd                                ; $75ef
    lda #$81                                 ; $75f2
    sta $dc0d                                ; $75f4
    lda $dc0e                                ; $75f7
    and #$80                                 ; $75fa
    ora #$11                                 ; $75fc
    sta $dc0e                                ; $75fe
    jmp $ee8e                                ; $7601
    !byte $03                                ; $7604 (undefined opcode)
    jmp $ff5b                                ; $7605
    jmp $fda3                                ; $7608
    jmp $fd50                                ; $760b
    jmp $fd15                                ; $760e
    jmp $fd1a                                ; $7611
    jmp $fe18                                ; $7614
    jmp $edb9                                ; $7617
    jmp $edc7                                ; $761a
    jmp $fe25                                ; $761d
    jmp $fe34                                ; $7620
    jmp $ea87                                ; $7623
    jmp $fe21                                ; $7626
    jmp $ee13                                ; $7629
    jmp $eddd                                ; $762c
    jmp $edef                                ; $762f
    jmp $edfe                                ; $7632
    jmp $ed0c                                ; $7635
    jmp $ed09                                ; $7638
    jmp $fe07                                ; $763b
    jmp $fe00                                ; $763e
    jmp $fdf9                                ; $7641
    jmp ($031a)                              ; $7644
    jmp ($031c)                              ; $7647
    jmp ($031e)                              ; $764a
    jmp ($0320)                              ; $764d
    jmp ($0322)                              ; $7650
    jmp ($0324)                              ; $7653
    jmp ($0326)                              ; $7656
    jmp $f49e                                ; $7659
    jmp $f5dd                                ; $765c
    jmp $f6e4                                ; $765f
    jmp $f6dd                                ; $7662
    jmp ($0328)                              ; $7665
    jmp ($032a)                              ; $7668
    jmp ($032c)                              ; $766b
    jmp $f69b                                ; $766e
    jmp $e505                                ; $7671
    jmp $e50a                                ; $7674
    jmp $e500                                ; $7677
    !byte $52                                ; $767a (undefined opcode)
    !byte $52                                ; $767b (undefined opcode)
    !byte $42                                ; $767c (undefined opcode)
    eor $fe43,y                              ; $767d
    !byte $e2                                ; $7680 (undefined opcode)
    !byte $fc                                ; $7681 (undefined opcode)
    pha                                      ; $7682
    !byte $ff                                ; $7683 (undefined opcode)
