.include "x16.inc"
.include "zeropage.inc"
; .include "canvas_utils.asm"

.import __x_axis

.export __draw_bitmap_canvas_to_screen

; TODO: 2 byte screen x and y
__zoom: .byte 2
__screen_x: .byte 32, 0
__screen_y: .byte 32, 0

SPRITE_VRAM_ADDR = $14000

CURR_COLOUR = ZP_PTR_1
CURR_SPRITE_X = ZP_PTR_2
CURR_SPRITE_Y = ZP_PTR_3
SPRITE_ADDR_INCR = ZP_PTR_4
CURR_SPRITE_ADDR = ZP_PTR_5
__draw_bitmap_canvas_to_screen:
    lda #1 
    sta VERA_ctrl
    stz VERA_addr_low
    stz VERA_addr_high
    lda  #(%00010000)
    sta VERA_addr_bank

    lda _bmx_width
    sta SPRITE_ADDR_INCR 

    ; offseting the sprite y by offset 
        lda __screen_y
        sta CURR_SPRITE_ADDR
        stz CURR_SPRITE_ADDR+1
        ldx __x_axis
        @offset_y_loop:
            asl CURR_SPRITE_ADDR
            rol CURR_SPRITE_ADDR+1
        dex
        bne @offset_y_loop    

        lda CURR_SPRITE_ADDR
        clc
        adc __screen_x
        sta CURR_SPRITE_ADDR
        lda CURR_SPRITE_ADDR+1
        adc #0
        sta CURR_SPRITE_ADDR+1

        lda CURR_SPRITE_ADDR
        clc
        adc #(<SPRITE_VRAM_ADDR)
        sta CURR_SPRITE_ADDR
        lda CURR_SPRITE_ADDR+1
        adc #(>SPRITE_VRAM_ADDR)
        sta CURR_SPRITE_ADDR+1

    ldy #(240)
    ldx __zoom
    lda __screen_y
    sta CURR_SPRITE_Y
    @vertical_loop:
        lda __screen_x
        sta  CURR_SPRITE_X
    
        phy
        phx
        ; set vera port for sprite row 
            stz VERA_ctrl
            lda CURR_SPRITE_ADDR
            sta VERA_addr_low
            lda CURR_SPRITE_ADDR+1
            sta VERA_addr_high
            lda  #(%00010000)
            
        ldy #1

        ldx #(0)
        @horizonal_loop_1:
            dey 
            bne @skip_change_col_1
                lda VERA_data0
                sta CURR_COLOUR
                ldy __zoom

                inc CURR_SPRITE_X
                lda CURR_SPRITE_X
                cmp _bmx_width
                bcc @skip_blank_row_1
                    stz CURR_COLOUR
                    dec CURR_SPRITE_X
                @skip_blank_row_1:
            @skip_change_col_1:
            
            lda CURR_COLOUR
            sta VERA_data1

        dex
        bne @horizonal_loop_1

        ldx #(64)
        @horizonal_loop_2:
            dey 
            bne @skip_change_col_2
                lda VERA_data0
                sta CURR_COLOUR
                ldy __zoom

                inc CURR_SPRITE_X
                lda CURR_SPRITE_X
                cmp _bmx_width
                bcc @skip_blank_row_2
                    stz CURR_COLOUR
                    dec CURR_SPRITE_X
                @skip_blank_row_2:
            @skip_change_col_2:
            
            lda CURR_COLOUR
            sta VERA_data1
        dex
        bne @horizonal_loop_2

        plx
        dex
        bne @skip_change_sprite_row
        ; setting sprite address to the next row
            lda CURR_SPRITE_ADDR
            clc
            adc SPRITE_ADDR_INCR
            sta CURR_SPRITE_ADDR
            lda CURR_SPRITE_ADDR+1
            adc #0
            sta CURR_SPRITE_ADDR+1

            ldx __zoom

            inc CURR_SPRITE_Y
            lda CURR_SPRITE_Y
            cmp _bmx_height
            beq @skip_to_blank
        @skip_change_sprite_row:

    ply
    dey 
    bne @vertical_loop

    @skip_to_blank:
    ; blank part at the bottom
    ply

    @blank_vertical_loop:
        ldx #(320/4)
        lda #0
        @blank_hor_loop:
            stz VERA_data1
            stz VERA_data1
            stz VERA_data1
            stz VERA_data1
        dex
        bne @blank_hor_loop

    dey
    bne @blank_vertical_loop

    rts