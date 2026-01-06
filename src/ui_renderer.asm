.include "x16.inc"
.include "zeropage.inc"

.export __focus_id
.export __mouse_data

.export __ui_type
.export __ui_global_pos_x
.export __ui_global_pos_y
.export __ui_position_x 
.export __ui_position_y 
.export __ui_size_x
.export __ui_size_y
.export __ui_palette
.export __ui_var_ptr_low
.export __ui_var_ptr_high
.export __ui_var_val
.export __ui_var_old_val
.export __ui_var_1
.export __ui_var_2
.export __ui_var_3
.export __ui_var_4
.export __ui_rend_func_low
.export __ui_rend_func_high
.export __ui_on_mouse
.export __ui_on_key_low
.export __ui_on_key_high

.export __ui_parent
.export __ui_no_of_children
.export __ui_first_child
.export __ui_last_child
.export __ui_next_sib
.export __ui_prev_sib

.export __initialize_mouse
.export __get_mouse_input

.export __clear_ui_layer
.export __draw_ui
.export __draw_ui_element
.export __update_ui_element_position

.export __empty_draw_func
.export __draw_ui_text
.export __draw_ui_box
.export __draw_ui_hline
.export __draw_ui_vline
.export __draw_ui_icon
.export __draw_ui_slider
.export __draw_ui_display_b16
.export __draw_ui_display_b10

.export __test_on_mouse_func
.export __press_toggle_button_mouse_func

__MAX_UI_ELEMENTS = 30

__focus_id: .byte $00
__mouse_data = ZP_MOUSE

__ui_type: .res __MAX_UI_ELEMENTS, $00
__ui_global_pos_x: .res __MAX_UI_ELEMENTS, $00
__ui_global_pos_y: .res __MAX_UI_ELEMENTS, $00
__ui_position_x: .res __MAX_UI_ELEMENTS, $00 
__ui_position_y: .res __MAX_UI_ELEMENTS, $00 
__ui_size_x: .res __MAX_UI_ELEMENTS, $00
__ui_size_y: .res __MAX_UI_ELEMENTS, $00
__ui_palette: .res __MAX_UI_ELEMENTS, $F0
__ui_var_ptr_low: .res __MAX_UI_ELEMENTS, $00
__ui_var_ptr_high: .res __MAX_UI_ELEMENTS, $00
__ui_var_val: .res __MAX_UI_ELEMENTS, $00
__ui_var_old_val: .RES __MAX_UI_ELEMENTS, $00
__ui_var_1: .res __MAX_UI_ELEMENTS, $00
__ui_var_2: .res __MAX_UI_ELEMENTS, $00
__ui_var_3: .res __MAX_UI_ELEMENTS, $00
__ui_var_4: .res __MAX_UI_ELEMENTS, $00
__ui_rend_func_low: .res __MAX_UI_ELEMENTS, $00
__ui_rend_func_high: .res __MAX_UI_ELEMENTS, $00
__ui_on_mouse: .res __MAX_UI_ELEMENTS*2, $00
__ui_on_key_low: .res __MAX_UI_ELEMENTS, $00
__ui_on_key_high: .res __MAX_UI_ELEMENTS, $00

__ui_parent: .res __MAX_UI_ELEMENTS, $00
__ui_no_of_children: .res __MAX_UI_ELEMENTS, $00
__ui_first_child: .res __MAX_UI_ELEMENTS, $00
__ui_last_child: .res __MAX_UI_ELEMENTS, $00
__ui_next_sib: .res __MAX_UI_ELEMENTS, $00
__ui_prev_sib: .res __MAX_UI_ELEMENTS, $00

__initialize_mouse:
    lda #0
    ldx #(640/8)
    ldy #(480/8)
    jsr MOUSE_CONFIG
    rts

__get_mouse_input:
    ldx #ZP_MOUSE
    jsr MOUSE_GET
    sta ZP_MOUSE+4

    ; set mouse position
        stz VERA_ctrl
        lda #<(VRAM_sprattr+2)
        sta VERA_addr_low
        lda #>(VRAM_sprattr+2)
        sta VERA_addr_high
        lda #%00010001
        sta VERA_addr_bank

        lda ZP_MOUSE+0
        sta VERA_data0
        lda ZP_MOUSE+1
        sta VERA_data0
        lda ZP_MOUSE+2
        sta VERA_data0
        lda ZP_MOUSE+3
        sta VERA_data0
    
    rts

LAYER_WIDTH = 64
LAYER_HEIGHT = 64
RENDER_FUNC_ADDR = ZP_PTR_7
__clear_ui_layer:
    stz VERA_ctrl
    sta VERA_addr_low
    stx VERA_addr_high
    lda #(%00010001)
    sta VERA_addr_bank

    lda #$F0
    ldx #LAYER_WIDTH
    @row_loop:
        ldy #LAYER_HEIGHT
        @column_loop:
        ; sta VERA_data0
        stz VERA_data0
        sta VERA_data0

        dey
        bne @column_loop

    dex
    bne @row_loop

    rts




__indirect_jump:
    jmp (RENDER_FUNC_ADDR)

__draw_ui:
    ldx #0
    @ui_element_loop:
    lda __ui_type,x
    beq @continue_element_loop

    lda __ui_rend_func_low,x
    sta RENDER_FUNC_ADDR
    lda __ui_rend_func_high,x
    sta RENDER_FUNC_ADDR+1
    txa
    phx
    jsr __indirect_jump
    plx

    @continue_element_loop:
    inx
    cpx #__MAX_UI_ELEMENTS
    bne @ui_element_loop

    rts

__empty_draw_func:
    rts

.macro set_ui_vera_data_port
    stz VERA_ctrl
    lda BOX_POS_X
    asl
    sta VERA_addr_low
    lda BOX_POS_Y
    sta VERA_addr_high
    lda #(%00010001)
    sta VERA_addr_bank 
.endmacro

.macro init_draw_ui
    tax

    lda __ui_palette, x
    sta PAL

    lda __ui_global_pos_x, x
    sta BOX_POS_X
    lda __ui_global_pos_y, x
    sta BOX_POS_Y

    set_ui_vera_data_port
.endmacro

.macro execute_draw_func
    lda __ui_rend_func_low,x
    sta RENDER_FUNC_ADDR
    lda __ui_rend_func_high,x
    sta RENDER_FUNC_ADDR+1
    phx
    txa
    jsr __indirect_jump
    plx
.endmacro

X_POS_STACK = GOLD_RAM+32
Y_POS_STACK = GOLD_RAM+$200+32
__update_ui_element_position:
    tay

    lda __ui_type,y
    beq __end_position_loop
    
    lda __ui_parent,y
    tax
    lda __ui_global_pos_x,x
    sta X_POS_STACK
    lda __ui_global_pos_y,x
    sta Y_POS_STACK

    tya
    tax
    ldy #0

    __set_global_position:
        lda __ui_type,x
        beq __end_position_loop

        lda X_POS_STACK,y
        clc
        adc __ui_position_x,x
        sta __ui_global_pos_x,x
        
        lda Y_POS_STACK,y
        clc
        adc __ui_position_y,x
        sta __ui_global_pos_y,x

        iny
        lda __ui_global_pos_x,x
        sta X_POS_STACK,y
        lda __ui_global_pos_y,x
        sta Y_POS_STACK,y

        lda __ui_first_child,x
        beq @__end_set_pos_loop

        @set_child_pos_loop:
            pha
            tax
            jsr __set_global_position
            plx
            lda __ui_next_sib,x
            beq @__end_set_pos_loop
            jmp @set_child_pos_loop

        @__end_set_pos_loop:
            dey
            rts

    __end_position_loop:
        rts

FIRST_UI_ID = ZP_PTR_11
__draw_ui_element:
    tax ;transfer the ui element id to x 
    sta FIRST_UI_ID
    
    lda __ui_type,x
    beq @end_render_loop


    @child_render_loop:
    ; render ui element
        lda __ui_rend_func_low,x
        sta RENDER_FUNC_ADDR
        lda __ui_rend_func_high,x
        sta RENDER_FUNC_ADDR+1
        phx
        txa
        jsr __indirect_jump
        plx

    ; go to the next ui element
    lda __ui_first_child,x
    beq @skip_drawing_first_child
    tax
    jmp @child_render_loop

    @skip_drawing_first_child:
    lda __ui_next_sib,x
    beq @skip_drawing_sibling
    tax
    jmp @child_render_loop


    @skip_drawing_sibling:
    lda __ui_parent,x
    beq @end_render_loop
    cmp FIRST_UI_ID
    beq @end_render_loop
    tax
    lda __ui_next_sib,x
    beq @end_render_loop
    tax
    jmp @child_render_loop
    
    @end_render_loop:
    rts

CHAR_PTR = ZP_PTR_4
CURR_CHAR = ZP_PTR_5
VERA_ADDR_OFF = ZP_PTR_6  
__draw_ui_text:
    ldy #0
    init_draw_ui

    ; get a pointer to the text from ui variables
    lda __ui_var_ptr_low, x
    sta CHAR_PTR
    lda __ui_var_ptr_high, x
    sta CHAR_PTR+1

    @text_render_loop:
    ; getting character
    lda (CHAR_PTR),y
    beq @end_text_render_loop
    sec
    sbc #32
    sta CURR_CHAR

    ; setting character onto layer
    lda CURR_CHAR
    sta VERA_data0
    lda PAL
    sta VERA_data0

    iny
    jmp @text_render_loop
    @end_text_render_loop:

    rts

BOX_POS_X = ZP_PTR_1
BOX_POS_Y = ZP_PTR_1+1
BOX_SIZE_X = ZP_PTR_2
BOX_SIZE_Y = ZP_PTR_2+1
PAL = ZP_PTR_3
TOP_LEFT_CORNER = 73
TOP_EDGE = 74
TOP_RIGHT_CORNER = 75
LEFT_EDGE = 76
RIGHT_EDGE = 77
BOT_LEFT_CORNER = 78
BOT_EDGE = 79
BOT_RIGHT_CORNER = 80
__draw_ui_box:
    tax
    ; save ui position and size data
    lda __ui_palette, x
    sta PAL

    lda __ui_global_pos_x, x
    sta BOX_POS_X
    lda __ui_global_pos_y, x
    sta BOX_POS_Y 
    lda __ui_size_y, x
    dec
    dec
    tay 
    lda __ui_size_x, x
    dec
    dec 
    sta BOX_SIZE_X

    stz VERA_ctrl
    lda BOX_POS_X
    asl
    sta VERA_addr_low
    lda BOX_POS_Y
    sta VERA_addr_high
    inc
    sta BOX_POS_Y
    lda #(%00010001)
    sta VERA_addr_bank 

    lda #TOP_LEFT_CORNER
    sta VERA_data0
    lda PAL
    sta VERA_data0

    ldx BOX_SIZE_X
    beq @skip_top_loop
    @top_loop:
        lda #TOP_EDGE
        sta VERA_data0
        lda PAL
        sta VERA_data0
    dex
    bne @top_loop
    @skip_top_loop:

    lda #TOP_RIGHT_CORNER
    sta VERA_data0
    lda PAL
    sta VERA_data0
    
    tya
    beq @skip_column_loop
    @box_column_loop:
        ; set up the data port position based on the text position
        stz VERA_ctrl
        lda BOX_POS_X
        asl
        sta VERA_addr_low
        lda BOX_POS_Y
        sta VERA_addr_high
        inc 
        sta BOX_POS_Y
        lda #(%00010001)
        sta VERA_addr_bank 

        lda #LEFT_EDGE
        sta VERA_data0
        lda PAL
        sta VERA_data0

        ldx BOX_SIZE_X
        beq @skip_row_loop
        lda PAL
        @box_row_loop:
            stz VERA_data0
            sta VERA_data0
        dex
        bne @box_row_loop
        @skip_row_loop:

        lda #RIGHT_EDGE
        sta VERA_data0
        lda PAL
        sta VERA_data0

    dey
    bne @box_column_loop
    @skip_column_loop:


    stz VERA_ctrl
    lda BOX_POS_X
    asl
    sta VERA_addr_low
    lda BOX_POS_Y
    sta VERA_addr_high
    inc
    sta BOX_POS_Y
    lda #(%00010001)
    sta VERA_addr_bank 

    lda #BOT_LEFT_CORNER
    sta VERA_data0
    lda PAL
    sta VERA_data0

    ldx BOX_SIZE_X
    beq @skip_bottom_loop
    @bot_loop:
        lda #BOT_EDGE
        sta VERA_data0
        lda PAL
        sta VERA_data0
    dex
    bne @bot_loop
    @skip_bottom_loop:

    lda #BOT_RIGHT_CORNER
    sta VERA_data0
    lda PAL
    sta VERA_data0

    rts

ICON_ADDR = ZP_PTR_4

__draw_ui_icon:
    init_draw_ui

    lda __ui_var_1, x
    sta ICON_ADDR

    lda ICON_ADDR
    sta VERA_data0
    lda PAL
    sta VERA_data0
    inc ICON_ADDR

    lda ICON_ADDR
    sta VERA_data0
    lda PAL
    sta VERA_data0
    inc ICON_ADDR

    inc BOX_POS_Y
    set_ui_vera_data_port

    lda ICON_ADDR
    sta VERA_data0
    lda PAL
    sta VERA_data0
    inc ICON_ADDR

    lda ICON_ADDR
    sta VERA_data0
    lda PAL
    sta VERA_data0

    rts

__draw_ui_hline:
    tax
    lda __ui_size_x, x
    beq @end_hline
    tay

    stz VERA_ctrl
    lda __ui_global_pos_x, x
    asl
    sta VERA_addr_low
    lda __ui_global_pos_y, x
    sta VERA_addr_high
    lda #(%00010001)
    sta VERA_addr_bank

    lda #105
    @hline_loop:
        sta VERA_data0
        stz VERA_data0
    dey
    bne @hline_loop

    @end_hline:
    rts

__draw_ui_vline:
    tax
    lda __ui_size_y, x
    beq @end_vline
    tay

    stz VERA_ctrl
    lda __ui_global_pos_x, x
    asl
    sta VERA_addr_low
    lda __ui_global_pos_y, x
    sta VERA_addr_high
    lda #(%10010001)
    sta VERA_addr_bank

    lda #106
    @first_vline_loop:
        sta VERA_data0
    dey
    bne @first_vline_loop
    
    stz VERA_ctrl
    lda __ui_global_pos_x, x
    asl
    inc
    sta VERA_addr_low
    lda __ui_global_pos_y, x
    sta VERA_addr_high
    lda #(1 + (9<<4))
    sta VERA_addr_bank

    lda __ui_size_x, x
    tay
    @second_vline_loop:
        stz VERA_data0
    dey
    bne @second_vline_loop

    @end_vline:
    rts


SLIDER_GFX_ADDR = 81
SLIDER_PTR_ADDR = ZP_PTR_1
SLIDER_VAL = ZP_PTR_2
__draw_ui_slider:
    init_draw_ui

    lda __ui_var_ptr_low, x
    sta SLIDER_PTR_ADDR
    lda __ui_var_ptr_high, x
    sta SLIDER_PTR_ADDR+1

    lda __ui_var_2, x
    tay    

    lda (SLIDER_PTR_ADDR)
    @shift_slider_loop:
    cpy #0
    beq @end_shift_slider
    dey 
        asl
    jmp @shift_slider_loop
    @end_shift_slider:

    clc
    adc __ui_var_1, x

    sta SLIDER_VAL


    lda __ui_size_x, x
    tay

    @draw_slider_full_col_loop: 
    lda SLIDER_VAL
    cmp #8
    bmi @end_full_loop
        lda #(SLIDER_GFX_ADDR+8)
        sta VERA_data0
        lda PAL
        sta VERA_data0 

    lda SLIDER_VAL
    sec
    sbc #8
    sta SLIDER_VAL

    dey
    bne @draw_slider_full_col_loop

    cpy #0
    beq @skip_empty_tiles

    @end_full_loop:
    
    ; draw semi filled tile
        lda #SLIDER_GFX_ADDR
        clc
        adc SLIDER_VAL
        sta VERA_data0
        lda PAL
        sta VERA_data0 
    
    dey
    beq @skip_empty_tiles


    @draw_slider_empty_col_loop: 
        lda #SLIDER_GFX_ADDR
        sta VERA_data0
        lda PAL
        sta VERA_data0 

    dey
    bne @draw_slider_empty_col_loop

    @skip_empty_tiles:

    rts


__draw_empty_ui:
    rts

HEX_PLACES: .byte $10, $01
DEC_PLACES: .byte 100, 10, 1
VAR_VAL = ZP_PTR_4
DIS_VAR_PTR = ZP_PTR_5
CURR_DIGIT = ZP_PTR_6
CURR_PLACE = ZP_PTR_7
HAS_SKIPPED_ZERO = ZP_PTR_8
DIS_PAL = ZP_PTR_9
__draw_ui_display_b16:
    init_draw_ui
    lda #($F0)
    stz VERA_data0
    sta VERA_data0
    stz VERA_data0
    sta VERA_data0

    set_ui_vera_data_port

    lda __ui_palette, x
    sta DIS_PAL

    stz HAS_SKIPPED_ZERO

    lda __ui_var_ptr_low, x
    sta DIS_VAR_PTR
    lda __ui_var_ptr_high, x
    sta DIS_VAR_PTR+1

    lda (DIS_VAR_PTR)
    sta VAR_VAL

    ldy #0
    @places_loop:
        lda HEX_PLACES, y
        sta CURR_PLACE

        lda VAR_VAL
        stz CURR_DIGIT
        @counting_loop:
        cmp CURR_PLACE
        bmi @stop_counting
            sec
            sbc CURR_PLACE
            inc CURR_DIGIT
        jmp @counting_loop
        @stop_counting:

        sta VAR_VAL

        lda CURR_DIGIT
        bne @start_drawing_digit

        ldx HAS_SKIPPED_ZERO
        bne @start_drawing_digit

        cpy #1
        beq @start_drawing_digit
         
        jmp @skip_drawing_zero

        @start_drawing_digit:
        inc HAS_SKIPPED_ZERO
        cmp #10
        bpl @alpha_offset
        clc
        adc #16
        jmp @draw_char_to_scr

        @alpha_offset:
        clc
        adc #(33-10)

        @draw_char_to_scr:
        sta VERA_data0
        lda DIS_PAL
        sta VERA_data0
        @skip_drawing_zero:
    iny
    cpy #2
    bne @places_loop

    rts

__draw_ui_display_b10:
    init_draw_ui
    lda #($F0)
    stz VERA_data0
    sta VERA_data0
    stz VERA_data0
    sta VERA_data0

    set_ui_vera_data_port

    lda __ui_palette, x
    sta DIS_PAL

    stz HAS_SKIPPED_ZERO

    lda __ui_var_ptr_low, x
    sta DIS_VAR_PTR
    lda __ui_var_ptr_high, x
    sta DIS_VAR_PTR+1

    lda (DIS_VAR_PTR)
    sta VAR_VAL

    ldy #0
    @places_loop:
        lda DEC_PLACES, y
        sta CURR_PLACE

        lda VAR_VAL
        stz CURR_DIGIT
        @counting_loop:
        cmp CURR_PLACE
        bmi @stop_counting
            sec
            sbc CURR_PLACE
            inc CURR_DIGIT
        jmp @counting_loop
        @stop_counting:

        sta VAR_VAL

        lda CURR_DIGIT
        bne @start_drawing_digit

        ldx HAS_SKIPPED_ZERO
        bne @start_drawing_digit

        cpy #2
        beq @start_drawing_digit
         
        jmp @skip_drawing_zero

        @start_drawing_digit:
        inc HAS_SKIPPED_ZERO
        clc
        adc #16
        sta VERA_data0
        lda DIS_PAL
        sta VERA_data0
        @skip_drawing_zero:
    iny
    cpy #3
    bne @places_loop

    rts

__test_on_mouse_func:
    brk
    rts

__update_toggle_button_container:
    rts

VAR_PTR = ZP_PTR_1
__press_toggle_button_mouse_func:
    tax

    lda __ui_var_ptr_low,x
    sta VAR_PTR
    lda __ui_var_ptr_high,x
    sta VAR_PTR+1

    lda __ui_var_val,x
    sta (VAR_PTR)

    lda __ui_parent,x
    phx
    tax
    lda __ui_first_child,x
    @toggle_off_button_loop:
        tax
        lda #$F0
        sta __ui_palette,x
        lda __ui_next_sib,x
        bne @toggle_off_button_loop
    plx

    lda #$E0
    sta __ui_palette,x
    
    lda __ui_parent,x
    jsr __draw_ui_element

    rts