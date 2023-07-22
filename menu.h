/*  menu.h
 * 
 * ƒƒjƒ…[
 * 
 *  2022.10.19
 * 
 */


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MENU_H
#define	MENU_H

extern  int16_t     bbmass_g;


void    setup_menu(void);
void    touch_menu(void);
void    target_menu(void);
//
void    target_set_up_command(void);
void    menu_clear_screen(void);
void    set_setup(void);
void    read_rom_setup(void);
void    write_rom_setup(void);

int8_t  target_offset_h(void);
    
#endif	/* MENU_H */

