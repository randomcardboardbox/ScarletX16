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
    context_parent_id = create_ui_element(context_container_id, UI_BOX, 0, 0, 28, 7, _empty_draw_func, NULL);

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 21, 1, 2, 2, _draw_ui_text, NULL);
    init_text_element(text_id1, shape_text);

    text_id1 = create_ui_element(context_parent_id, UI_TEXT, 11, 1, 2, 2, _draw_ui_text, NULL);
    init_text_element(text_id1, size_text);

    #define ROUND_BRUSH_ADDR 132
    icon_id = create_ui_element(context_parent_id, UI_TOGGLE_BUT, 24, 3, 2, 2, _draw_ui_icon, _press_toggle_button_mouse_func);
    init_icon_element(icon_id, ROUND_BRUSH_ADDR, &brush_type, 1);
    icon_id = create_ui_element(context_parent_id, UI_TOGGLE_BUT, 22, 3, 2, 2, _draw_ui_icon, _press_toggle_button_mouse_func);
    init_icon_element(icon_id, ROUND_BRUSH_ADDR-4, &brush_type, 0);


    slider = create_ui_element(context_parent_id, UI_SLIDER, 11, 3, 4, 1, _draw_ui_slider, NULL);
    init_slider_element(slider, &brush_size);
}

u8 old_tool = LINE_TOOL;
void tool_ui_handler(){
    if(old_tool != _current_tool){
        if(context_parent_id != 0){
            delete_ui_element(context_parent_id);
        }

        if(_current_tool == DRAW_TOOL) brush_ui_handler();

        _update_ui_element_position(context_container_id);
        // _draw_ui_element(context_container_id);
    }
    old_tool = _current_tool;
}