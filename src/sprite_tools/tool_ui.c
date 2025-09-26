#include <stdio.h>

#include "tool_ui.h"
#include "../paint_canvas.h"
#include "../paint_ui.h"
#include "../utils.h"
#include "../ui_utils.h"
#include "tool_globals.h"

u8 slider;
u8 context_parent_id;
char shape_text[] = "shape";
char size_text[] = "size";
u8 text_id1 = 0;
void brush_ui_handler(){
    u8 icon_id;
    u8 shape_text_ui_id;

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 1, 1, 2, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, shape_text);

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 1, 5, 2, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, size_text);

    #define ROUND_BRUSH_ADDR 132
    icon_id = create_ui_element(context_parent_id, UI_TOGGLE_BUT, 3, 2, 2, 2, _draw_ui_icon, _press_toggle_button_mouse_func);
    init_icon_element(icon_id, ROUND_BRUSH_ADDR, &brush_type, 1);
    icon_id = create_ui_element(context_parent_id, UI_TOGGLE_BUT, 1, 2, 2, 2, _draw_ui_icon, _press_toggle_button_mouse_func);
    init_icon_element(icon_id, ROUND_BRUSH_ADDR-4, &brush_type, 0);


    slider = create_ui_element(context_parent_id, UI_SLIDER, 1, 6, 8, 1, _draw_ui_slider, slider_on_mouse_func);
    init_slider_element(slider, 2, 1, &brush_size);
}

u8 red_pal_col;
u8 green_pal_col;
u8 blue_pal_col;
pal_edit_mouse_func(u8 ui_id){
    u8 pal_byte1 = 0;
    u8 pal_byte2 = 0;
    u32 vram_colour_addr = VRAM_palette;
    slider_on_mouse_func(ui_id);

    pal_byte1 = (green_pal_col<<4) + blue_pal_col;
    pal_byte2 = red_pal_col;

    vram_colour_addr += ((u16)_primary_colour)<<1;
    VERA_ctrl = 0;
    VERA_addr_low = vram_colour_addr;
    VERA_addr_high = vram_colour_addr>>8;
    VERA_addr_bank = (vram_colour_addr>>16) | 0b00010000;

    VERA_data0 = pal_byte1;
    VERA_data0 = pal_byte2;
}

void return_from_pal_edit(u8 ui_id){
    _current_tool = tool_before_pal_edit;
    enable_mouse_funcs = 0;
}

char check_text[] = {122, 0};
void palette_edit_ui_handler(){
    u32 vram_colour_addr = VRAM_palette;
    u8 pal_byte1 = 0;
    u8 pal_byte2 = 0;

    vram_colour_addr += ((u16)_primary_colour)<<1;
    VERA_ctrl = 0;
    VERA_addr_low = vram_colour_addr;
    VERA_addr_high = vram_colour_addr>>8;
    VERA_addr_bank = (vram_colour_addr>>16) | 0b00010000;

    pal_byte1 = VERA_data0;
    pal_byte2 = VERA_data0;

    red_pal_col = pal_byte2;
    green_pal_col = pal_byte1>>4;
    blue_pal_col = pal_byte1 & 0b00001111;
    
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 1, 1, 1, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, "colour");
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 2, 3, 1, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, "r");
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 2, 4, 1, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, "g");
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 2, 5, 1, 1, _draw_ui_text, NULL);
    init_text_element(text_id1, "b");

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 3, 3, 1, 1, _draw_ui_display_b16, NULL);
    init_variable_display(text_id1, &red_pal_col);
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 3, 4, 1, 1, _draw_ui_display_b16, NULL);
    init_variable_display(text_id1, &green_pal_col);
    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 3, 5, 1, 1, _draw_ui_display_b16, NULL);
    init_variable_display(text_id1, &blue_pal_col);



    slider = create_ui_element(context_parent_id, UI_SLIDER, 5, 3, 4, 1, _draw_ui_slider, pal_edit_mouse_func);
    init_slider_element(slider, 0, 1, &red_pal_col);
    slider = create_ui_element(context_parent_id, UI_SLIDER, 5, 4, 4, 1, _draw_ui_slider, pal_edit_mouse_func);
    init_slider_element(slider, 0, 1, &green_pal_col);
    slider = create_ui_element(context_parent_id, UI_SLIDER, 5, 5, 4, 1, _draw_ui_slider, pal_edit_mouse_func);
    init_slider_element(slider, 0, 1, &blue_pal_col);

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 8, 6, 1, 1, _draw_ui_text, return_from_pal_edit);
    init_text_element(text_id1, check_text);

    _render_colour_sprite(_primary_colour, 1, 0x5000);
    _set_sprite_attribute(8, 0x15000>>5, 1, 240, 104, 0b00001100, 0, 0);
    _set_sprite_attribute(9, 0x15000>>5, 1, 240, 104+8, 0b00001100, 0, 0);
    _set_sprite_attribute(10, 0x15000>>5, 1, 240, 104+16, 0b00001100, 0, 0);
}

u8 old_tool = LINE_TOOL;
u8 redraw_tool_ui = 0;
void tool_ui_handler(){
    if(old_tool != _current_tool || redraw_tool_ui){
        redraw_tool_ui = 0;
        point_selected = 0;
        if(context_parent_id != 0){
            delete_ui_element(context_parent_id);
            _clear_sprite_attribute(8);
            _clear_sprite_attribute(9);
            _clear_sprite_attribute(10);
        }
        
        context_parent_id = create_ui_element(context_container_id, UI_BOX, 0, 0, 10, 8, _empty_draw_func, NULL);
        if(_current_tool == DRAW_TOOL) brush_ui_handler();
        if(_current_tool == PAL_EDIT_TOOL) palette_edit_ui_handler();

        _update_ui_element_position(context_container_id);
        _draw_ui_element(context_container_id);
    }
    old_tool = _current_tool;
}