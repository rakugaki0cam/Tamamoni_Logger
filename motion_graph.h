/*  motion_graph.h
 * 
 *  motion graph
 * 
 *  2021.10.10
 * motion.c���O���t�\�����𕪗�
 * 
*/

#ifndef MOTION_GRAPH_H
#define MOTION_GRAPH_H


//global
#define     MOTION_X0     (DISPLAY_WIDTH / 2)   //�O���t���_0�ʒu
#define     MOTION_Y0     (DISPLAY_HEIGHT / 2)


void    motion_graph_initialize(void);
void    motion_clear_screen(void);
void    motion_plot_graph(float, float, float);


#endif //MOTION_GRAPH_H