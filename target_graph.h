/*  target_graph.h
 * 
 *  target graph
 * 電子ターゲットの着弾点を表示
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TARGET_GRAPH_H
#define TARGET_GRAPH_H


//GLOBAL
extern uint8_t  ctoc_color;
extern uint16_t ctoc_num;           //着弾データサブからの戻り値
extern float    ctoc_max;

void    target_graph_initialize(void);
void    target_data_reset(void);
void    target_clear_screen(void);

float   impact_plot_graph(uint16_t, float, float, bool, bool);
void    draw_impact_point(int16_t, int16_t, uint8_t);
void    target_lcd_clear_command(void);

#endif //TARGET_GRAPH_H