.include "x16.inc"
.include "zeropage.inc"

.export __init_overlay_display
.export __clear_overlay_display
.export __draw_overlay_pixel
.export __draw_overlay_v_line
.export __draw_overlay_h_line
.export __draw_overlay_dotted_v_line
.export __draw_overlay_dotted_h_line

OVERLAY_WIDTH = 32
OVERLAY_HEIGHT = 26

OVERLAY_START_INDEX = 144
BLOCK_BTYE_SIZE = 16
DRAW_START_ADDR = (OVERLAY_START_INDEX*BLOCK_BTYE_SIZE)
CURR_OVERLAY_INDEX = ZP_PTR_1
__init_overlay_display:
    lda #($E0) ; loading the palette into the tile data
    sta CURR_OVERLAY_INDEX+1
    lda #OVERLAY_START_INDEX
    sta CURR_OVERLAY_INDEX


    ldx #0
    @row_loop:
    ldy #OVERLAY_WIDTH

    stz VERA_ctrl
    stz VERA_addr_low
    stx VERA_addr_high
    lda #(%00010001)
    sta VERA_addr_bank 

        @column_loop:
            lda CURR_OVERLAY_INDEX
            sta VERA_data0
            clc
            adc #1
            sta CURR_OVERLAY_INDEX

            lda CURR_OVERLAY_INDEX+1
            sta VERA_data0
            adc #0
            sta CURR_OVERLAY_INDEX+1
            
        dey
        bne @column_loop

    inx
    cpx #OVERLAY_HEIGHT
    bne @row_loop

    rts

__clear_overlay_display:
    stz VERA_ctrl
    lda #(<DRAW_START_ADDR)
    sta VERA_addr_low
    lda #(>DRAW_START_ADDR)
    sta VERA_addr_high
    lda #(%00010000)
    sta VERA_addr_bank
    
    ldx #OVERLAY_HEIGHT*4
    @row_loop:
        ldy #OVERLAY_WIDTH*4
        @column_loop:
            stz VERA_data0
        dey
        bne @column_loop
    dex
    bne @row_loop

    rts

;void _draw_overaly_pixel(u8 col, u8 x, u8 y)
DRAW_ADDR = ZP_PTR_1
X_POS = ZP_PTR_2
Y_POS = ZP_PTR_3
COLOUR = ZP_PTR_4
__draw_pixel_internal:
    ; selecting the first or second byte of a block row
        lda X_POS
        lsr
        lsr
        and #(%00000001)
        sta DRAW_ADDR

    ; selecting the row within a single block
        lda Y_POS
        and #(%00000111)
        asl
        clc
        adc DRAW_ADDR
        sta DRAW_ADDR

    ; selecting the block within a row
        asl X_POS
        lda #0
        adc #0
        sta DRAW_ADDR+1

        lda X_POS
        and #(%11110000)
        clc
        adc DRAW_ADDR
        sta DRAW_ADDR

    ; initialize address at the start of the overlay memory
        lda #(<DRAW_START_ADDR)
        clc
        adc DRAW_ADDR
        sta DRAW_ADDR
        lda #(>DRAW_START_ADDR)
        adc DRAW_ADDR+1
        sta DRAW_ADDR+1 

    ; selecting a block row
        lda Y_POS
        lsr
        lsr
        and #(%11111110)
        clc
        adc DRAW_ADDR+1
        sta DRAW_ADDR+1

    ; setting up the data port
        stz VERA_ctrl
        lda DRAW_ADDR
        sta VERA_addr_low
        lda DRAW_ADDR+1
        sta VERA_addr_high
        lda #(%00000000)
        sta VERA_addr_bank

    ; drawing pixel to the overlay
        lda VERA_data0
        ldx X_POS
        
        cpx #(0<<1)
        bne @skip_pix_one
        ora COLOUR+3
        jmp @place_pixel_in_vram

        @skip_pix_one:
        cpx #(1<<1)
        bne @skip_pix_two
        ora COLOUR+2
        jmp @place_pixel_in_vram

        @skip_pix_two:
        cpx #(2<<1)
        bne @skip_pix_three
        ora COLOUR+1
        jmp @place_pixel_in_vram

        @skip_pix_three:
        ora COLOUR

        @place_pixel_in_vram:
        sta VERA_data0

    rts

__draw_overlay_pixel:
    ; load variables into zero page 
        sta Y_POS

        lda (sp)
        sta X_POS
        inc sp

        lda (sp)
        sta COLOUR
        asl 
        asl 
        sta COLOUR+1
        asl 
        asl 
        sta COLOUR+2
        asl
        asl 
        sta COLOUR+3
        inc sp

    jsr __draw_pixel_internal

    rts

; void __draw_overlay_v_line(u8 col, u8 x, u8 y, u8 height)
__draw_overlay_v_line:
    rts

; void __draw_overlay_v_line(u8 col, u8 x, u8 y, u8 width)
__draw_overlay_h_line:
    rts

draw_overlay_dotted_v_line:
    rts
draw_overlay_dotted_h_line:
    rts