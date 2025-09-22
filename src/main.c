#include <stdio.h>
#include <cbm.h>

#include "utils.h"
#include "ui_utils.h"
#include "overlay_render_utils.h"
#include "paint_ui.h"
#include "paint_canvas.h"
#include "history_stack.h"

#include "sprite_tools/draw_tools_utils.h"
#include "sprite_tools/brush_tool.h"
#include "sprite_tools/tool_ui.h"
#include "sprite_tools/tool_globals.h"

#define LFN 15
#define DEVICE 8
#define SA 2
#define MODE 0
char filename[] = "dripzero.bmx";
void load_bmx_file(){
    u16 vram_addr = SPRITE_VRAM_DATA_ADDR;
    u8 ram_bank = 2;
    cbm_open(LFN, DEVICE, SA, filename);
    cbm_read(LFN, GOLD_RAM_ADDR, 32);

    // load palette --------------------------------------------------
    RAM_BANK_SEL = 1;
    if((*bmx_no_pals) != 0) cbm_read(LFN, RAM_BANK_ADDR, ((u16)(*bmx_no_pals))*2);
    else cbm_read(LFN, RAM_BANK_ADDR, 256*2);
    _transfer_pal_to_vera();

    // load sprite data --------------------------------------------------
    RAM_BANK_SEL = ram_bank;
    _image_data_size = ((*bmx_width)*(*bmx_height)) >> (3- (*bmx_vera_bit_depth));
    while(_image_data_size > 0x2000){
        cbm_read(LFN, RAM_BANK_ADDR, 0x2000);
        _transfer_sprite_to_vram(0x2000, vram_addr, 0);
        vram_addr += 0x2000;
        ram_bank += 1;
        RAM_BANK_SEL = ram_bank;
        if(_image_data_size > 0x2000){ 
            _image_data_size -= 0x2000;
        }
        else{ 
            break;
        }
    }

    cbm_read(LFN, RAM_BANK_ADDR, _image_data_size);
    _transfer_sprite_to_vram(_image_data_size, vram_addr, 0);

    cbm_close(LFN);
}

void save_bmx_file(){
    cbm_open(LFN, DEVICE, SA, filename);


    cbm_close(LFN);
}

void handle_keyboard_input(){
    if (keycode) {

        if(keycode == 26) undo_last_history_node();
        else if(keycode == 25) restore_last_history_node();

        else if(keycode == 73) change_tool(EYEDROPPER_TOOL);
        else if(keycode == 66) change_tool(DRAW_TOOL);

        else if(keycode == 83) save_bmx_file();
        // printf("PETSCII Code %u\n", keycode);
    }
}

u8 timer = 0;

int main(){
    _init_irq_handler();
    _init_screen_mode();
    set_layer_config();
    _initialize_mouse();
    initialize_paint_ui();
    
    load_bmx_file();
    init_canvas_vera_sprites();
    _render_palette_sprites();

    _clear_ui_layer(0x10000);
    _init_overlay_display();
    _clear_overlay_display();
    _update_ui_element_position(0);
    _draw_ui_element(0);
    
    _draw_canvas_to_screen();
    set_pal_icon_sprites();

    HIS_STACK_ADDR = (u32)0x0004A000;

    _current_tool = 7;

    while(1){
        _wait_for_nmi();
        overlay_routines();

        _get_mouse_input();
        parse_mouse_input();
        get_keycode();
        handle_keyboard_input();
        tool_handler();
        tool_ui_handler();

        update_ui_elements_from_ptr();
    }
}

// #include <stdio.h>

// unsigned char keycode1;

// void main() {
//     while(1) {
//         asm("jsr $FFE4");
//         asm("sta %v", keycode1);

//         if (keycode1) {
//             printf("PETSCII Code %u\n", keycode1);
//         }
//     }
// }