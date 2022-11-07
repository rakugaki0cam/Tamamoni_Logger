/* 
 * File:   motion_graph.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 * Motion DATA process
 * Graph 
 * 
 * Revision history: 
 * 2021.10.10   motion.c���O���t�\�����𕪗�
 * 2021.10.10   ���\�������L������
 *
 * V8
 * 2022.04.24   ���[�V�����\����؂藣��
 * 2022.05.08   ���[�V�����v���b�g���^�[�Q�b�g���[�h�ɂ���ăI���I�t
 *              �O���t�̃N���A�͈͂��^�[�Q�b�g�ɍ��킹��
 * 
 * 
*/

#include "math.h"///////////////
#include "header.h"
#include "motion_graph.h"


#define     MOTION_DX           (DISPLAY_WIDTH / 2)     //�\���̑傫��(1/2���Ɖ�ʑS���\��)
#define     MOTION_DY           ((DISPLAY_WIDTH / 2) - 10)
#define     MOTION_DY_OFFSET   4
#define     MOTION_X_MIN        MOTION_X0 - MOTION_DX           //�\���͈̔�
#define     MOTION_Y_MIN        MOTION_Y0 - MOTION_DY + MOTION_DY_OFFSET
#define     MOTION_X_MAX        MOTION_X0 + MOTION_DX
#define     MOTION_Y_MAX        MOTION_Y0 + MOTION_DY + MOTION_DY_OFFSET

#define     NUM_MIL         2
#define     MIL_SCALE_X     30      //1�ڐ�(MIL)�̃h�b�g��
#define     MIL_SCALE_Y     40
#define     PLOT_CIRCLE_R   2       //�O���t�v���b�g�_�̉~���a


uint16_t    x_before, y_before;             //motion���C���̑O��l


void motion_graph_initialize(void){
    //���[�V�����O���t��ʂ̏�����
    //�����і���
#ifdef GYRO_MONITOR_TEST
    #define     MIL_1   "5"
    #define     MIL_2   "10MIL"
#else
                //+-2MIL
    //#define     MIL_1   "1.0"
    //#define     MIL_2   "2.0MIL"
                //+-8MIL, +-2MIL
    #define     MIL_1   "1.0"
    #define     MIL_2   "2.0MIL"
    #define     MIL_3   "3.0"
    #define     MIL_4   "6.0MIL"
#endif

    uint16_t x0, y0, x1, y1;
    
    x0 = MOTION_X0 - NUM_MIL * MIL_SCALE_X;
    y0 = MOTION_Y0 - NUM_MIL * MIL_SCALE_Y;
    x1 = MOTION_X0 + NUM_MIL * MIL_SCALE_X;
    y1 = MOTION_Y0 + NUM_MIL * MIL_SCALE_Y;
    //�N���A
    motion_clear_screen();
    //�g
    LCD_SetColor(0x80, 0x80, 0x80);                 //LIGHTGRAY
    LCD_DrawBox(MOTION_X0, MOTION_Y_MIN, MOTION_X0, MOTION_Y_MAX);        //y����
    LCD_DrawBox(MOTION_X_MIN, MOTION_Y0, MOTION_X_MAX, MOTION_Y0);        //x����
    
    LCD_SetColor(0x40, 0x40, 0x40);                 //GRAY
    LCD_DrawBox(x0, y0, x1, y1);                    //�O�g
    LCD_DrawBox(x0 + MIL_SCALE_X, y0 + MIL_SCALE_Y, x1 - MIL_SCALE_X, y1 - MIL_SCALE_Y);//���g

    LCD_Printf(x1 - MIL_SCALE_X - 6, y0 - 3      , MIL_1, 1, WHITE, 1);
    LCD_Printf(x1 - 6     , y0 - 3      , MIL_2, 1, WHITE, 1);
    LCD_Printf(x0 - 12    , y0 + MIL_SCALE_Y - 3 , MIL_3, 1, WHITE, 1);
    LCD_Printf(x0 - 12    , y0 - 3      , MIL_4, 1, WHITE, 1);
    
    x_before = MOTION_X0;
    y_before = MOTION_Y0;
}


void motion_clear_screen(void){
    //�N���A
    LCD_SetColor(0x0, 0x0, 0x0);                    //BLACK
    LCD_DrawFillBox(MOTION_X_MIN, MOTION_Y_MIN - 4, MOTION_X_MAX, MOTION_Y_MAX);
}


void motion_plot_graph(float sigmay, float sigmaz, float shot_offset){
    //���[�V�����f�[�^���O���t�ɂ���@<- motion_print_gyro() <-�@motion_log_sd_write()����Ăяo��
    //shot_offset�́A�ʂ����˃^�C�~���O�̎��̃I�t�Z�b�g�����A���˂̎��łȂ����͕���
    
    uint16_t    x1, y1;
    uint16_t    xs, ys;
    uint8_t     ky, kz;         //�W���C�����j�^�[�̎��̃O���t�\���@�ړ��ʂ�1/10�ɁB�@�P��10MIL

#ifdef GYRO_MONITOR_TEST
    //+-10MIL�\��
    ky = 5;
    kz = 5;
#else
    //+-2MIL�\��
    //ky = 1;
    //kz = 1;
                
    //UD +-6MIL, RL +-2MIL�\��
    ky = 3;
    kz = 1;
#endif
                
    //�\���͈͂���݂͂������̃`�F�b�N
    if ((sigmay > (NUM_MIL * ky)) || (sigmay < (-NUM_MIL * ky))){
        return;
    }
    if ((sigmaz > ((NUM_MIL * 2) * kz)) || (sigmaz < ((-NUM_MIL * 2) * kz))){
        return;
    }
    
    x1 = (uint16_t)(sigmaz * MIL_SCALE_X / kz + MOTION_X0); 
    y1 = (uint16_t)(-sigmay * MIL_SCALE_Y / ky + MOTION_Y0);
    
    LCD_SetColor(0xff, 0xff, 0x00);             //YELLOW
    LCD_DrawFillCircle(x1, y1, PLOT_CIRCLE_R);

    LCD_SetColor(0xff, 0xff, 0x00);             //YELLOW
    LCD_DrawLine(x_before, y_before, x1, y1);   //line
    
    if (shot_offset >= 0){
        //���ˎ��̃��b�h�}�[�N
        //xs = (uint16_t)(((x1 - x_before) * shot_offset) + x_before);    //���W�̓v���X�����Ȃ�����uint16_t�ɂ��Ă���
        //??????uint ��int��warning�o����o�Ȃ�������
        xs = (uint16_t)(((int16_t)x1 - (int16_t)x_before) * shot_offset + (int16_t)x_before);    //���W�̓v���X�����Ȃ�����uint16_t�ɂ��Ă���
        ys = (uint16_t)(((int16_t)y1 - (int16_t)y_before) * shot_offset + (int16_t)y_before);    //x1-x_before�͕�������̂�int16_t���K�v
        LCD_SetColor(0xff, 0x00, 0x8f);         //MAGENTA
        LCD_DrawFillCircle(xs, ys, PLOT_CIRCLE_R + 2);  //�傫���~
        
        //debug
        //LCD
        //sprintf(tmp_string, "%5.3f", shot_offset);
        //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, RED, 1); 
        //debugger
        //printf("k:%5.3f", shot_offset\n");
    }

    //�O��l�Ƃ��ĕۑ�
    x_before = x1;
    y_before = y1;
    
}
