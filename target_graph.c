/* 
 * File:   target_graph.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 * Electril TARGET
 * Graph 
 * 
 * Revision history: 
 * V8
 * 2022.04.24   �V�K
 * 2022.05.22   ���e�f�[�^�o�b�t�@���O���t���W����x100��int16_t�l�ɕύX(�Z���^�[�`�Z���^�[�v�Z�̂���)
 * 2022.10.16   �^�[�Q�b�g���Z�b�g(CtoC=0)��ǉ�
 * 
 * 
*/

#include "math.h"   ///////////////
#include "header.h"
#include "target_graph.h"


#define     TARGET_X0           ((DISPLAY_WIDTH / 2) + 0)   //�O���t���_0�ʒu
#define     TARGET_Y0           ((DISPLAY_HEIGHT / 2) + 0)
#define     TARGET_DX           (DISPLAY_WIDTH / 2)     //�\���̑傫��(1/2���Ɖ�ʑS���\��)
#define     TARGET_DY           ((DISPLAY_WIDTH / 2) - 10)
#define     TARGET_DY_OFFSET    4
#define     TARGET_X_MIN        TARGET_X0 - TARGET_DX   //�\���͈̔�
#define     TARGET_Y_MIN        TARGET_Y0 - TARGET_DY + TARGET_DY_OFFSET
#define     TARGET_X_MAX        TARGET_X0 + TARGET_DX
#define     TARGET_Y_MAX        TARGET_Y0 + TARGET_DY + TARGET_DY_OFFSET
#define     TARGET_D            (DISPLAY_WIDTH / 4)     //�^�[�Q�b�g�~�̑傫���s�N�Z��240/4=60
#define     ROW_TARGET          40                      //Vmeasure3.c��x�Ay�\�����C���ʒu���

//APS�^�[�Q�b�g �P��[mm]
#define     R_NUM       8       //�~�̐�
#define     APS_D_X     10.5    //�~���@mm
#define     APS_D_10p1  19.5    //10�_����
#define     APS_D_10    20.3    //10�_
#define     APS_D_9p9   21.0    //10�_�O��
#define     APS_D_8     34.5
#define     APS_D_5     49.5
#define     APS_WIDTH   91      //����
#define     APS_HIGHTU  67      //�㑤
#define     APS_HIGHTD  61      //����
//mm->pixel
#define     SCALE       (float)(TARGET_D / APS_D_5) //mm���s�N�Z���Ɋ��Z
//APS�}�g����pixel�T�C�Y
#define     APS_X_MIN   (TARGET_X0 - SCALE * (APS_WIDTH / 2))
#define     APS_Y_MIN   (TARGET_Y0 - SCALE * APS_HIGHTU)
#define     APS_X_MAX   (TARGET_X0 + SCALE * (APS_WIDTH / 2))
#define     APS_Y_MAX   (TARGET_Y0 + SCALE * APS_HIGHTD)
//���Sx�}�[�N
#define     POINT       "x"

//��
#define     BB          4                   //���e�~���@mm
#define     PLOT_D      (float)(SCALE * BB) //�O���t�v���b�g�_�̉~���a

//impact_plot_graph�ďo���̈���
#define DUMMY       0 
#define REDRAW_NONE 0
#define REDRAW      1
#define RESET_NONE  0  
#define RESET_DONE  1     


//GLOBAL
uint8_t     ctc_color;      //////////////////////////local�ɂ�����



void target_graph_initialize(void){
    //�^�[�Q�b�g��ʂ̏����\��
    //1���E���h�̊ԁA�ێ��B
    uint8_t i;
    uint16_t    taget_circle[R_NUM] = {
                    (uint16_t)(SCALE * APS_D_X),
                    (uint16_t)(SCALE * APS_D_10p1),
                    (uint16_t)(SCALE * APS_D_10),
                    (uint16_t)(SCALE * APS_D_9p9),
                    (uint16_t)(SCALE * APS_D_8),
                    (uint16_t)(SCALE * APS_D_5)
                };
    
    //�N���A
    target_clear_screen();
    //�g
    //APS���̑傫��
    LCD_SetColor(0xff, 0xff, 0xff);                     //WHITE
    LCD_DrawFillBox((uint16_t)APS_X_MIN, (uint16_t)APS_Y_MIN, (uint16_t)APS_X_MAX, (uint16_t)APS_Y_MAX);
    //��
    LCD_SetColor(0x80, 0x80, 0x80);                     //GREY
    LCD_DrawBox(TARGET_X0, TARGET_Y_MIN, TARGET_X0, TARGET_Y_MAX);    //y����
    LCD_DrawBox(TARGET_X_MIN, TARGET_Y0, TARGET_X_MAX, TARGET_Y0);    //x����
    
    
    for (i = 0; i < R_NUM; i++ ){
        //���S�~��`��
        LCD_SetColor(0x00, 0x00, 0x00);                 //BLACK
        LCD_DrawCircle(TARGET_X0, TARGET_Y0, taget_circle[i] / 2);

    }
    //���S��'x'
    LCD_Printf(TARGET_X0 - 2, TARGET_Y0 - 4, POINT, 1, BLACK, 0);
    
    //���e�_�ĕ`�� 
    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW, RESET_NONE, DUMMY);
    
}


void target_data_reset(void){
    //�^�[�Q�b�g�f�[�^�����Z�b�g����B�@CtoC��0mm
    //SW2������
    uint8_t     i;
    device_connect_t    targetmode;

    targetmode = target_mode_get();
    if ((TARGET_ONLY_MODE != targetmode) && (V0_TARGET_MODE != targetmode)){
        return;
    }
    
    sprintf(tmp_string, "TARGET CLEAR?     ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1); 
    //1�b������
    for (i = 0; i < 15; i++){
        __delay_ms(100);
        if (SW2_PORT == SW_OFF){
            sprintf(tmp_string, "                  ");
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
            return;
        }
    }
    //�d�q�^�[�Q�b�g�ɃN���A�R�}���h�𑗂�
    uint8_t clear[] = "CLEAR";
    command_uart_send_esp32(clear);     //WiFi
    command_uart_send_rs485(clear);     //LAN                                    //LAN�Őڑ��̎����K�v
    
    //�^�[�Q�b�g���N���A
    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE, DUMMY);
    target_graph_initialize();
    sprintf(tmp_string, "DONE!             ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
    __delay_ms(1000);
    sprintf(tmp_string, "                  ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);

    while(SW2_PORT == SW_ON){
        //�L�[���������̂�҂�
    }
    
}



void target_clear_screen(void){
    //�N���A
    LCD_SetColor(0x00, 0x00, 0x00);                     //BLACK
    //LCD_DrawFillBox(TARGET_X_MIN, TARGET_Y_MIN, TARGET_X_MAX, TARGET_Y_MAX); 
    LCD_DrawFillBox(TARGET_X_MIN, ROW_TARGET, TARGET_X_MAX, TARGET_Y_MAX); 
}


float impact_plot_graph(uint16_t shot, float x0, float y0, bool redraw, bool reset, uint16_t* n){
    //���e�_�\��
    //�����@sh:�V���b�g#, x0,y0:���e���W
    //�I�v�V�����@redraw:�O���t�ĕ`��,  reset:CtoC�v�Z���Z�b�g
    //�v���b�g�f�[�^���o�b�t�@�[�֕ۑ�
    //�߂�l:CtoC[mm]
    //      n:num CtoC�̃T���v�����@�|�C���^�ɂĎ󂯓n��
    
#define NUM_BUFFER  50
   
    static uint16_t shot_buf[NUM_BUFFER];
    static int16_t  x_buf[NUM_BUFFER], y_buf[NUM_BUFFER];   //x100�l
    static uint8_t  po = 0;         //�o�b�t�@�̃|�C���^
    static uint16_t num = 0;        //�f�[�^��
    static float    ctc_max = 0;    //(�ő咅�e�Ԋux100mm)�O2 �\���̎��Ƀ��[�g����
    float           ctc_tmp;
    uint8_t         i, i_max;
    int8_t          b;
    
    if (reset == 1){
        //�^�[�Q�b�g�����Z�b�g����(�ˌ����d�؂蒼����������)
        //���̃��[�`�����ł������Z�b�g�ł��Ȃ�����
        po = 0;
        num = 0;
        *n = num;       //�|�C���^�󂯓n��
        ctc_max = 0;    //CtoC���[���ɂ���
        return 0;
    }
    
    if (redraw == 0){
        //ReDraw = 0: �ꒅ�e�_��`��
        
        //�O��̒��e�����F��
        if (num > 0){
            b = (int8_t)po - 1;
            if (b < 0){
                b = NUM_BUFFER - 1;
            }
            draw_impact_point(x_buf[b], y_buf[b], YELLOW);
        }
           
        //�f�[�^�o�b�t�@�ւ̕ۊǗp16�r�b�g�l�ւ̌v�Z�ƃ������֋L��
        shot_buf[po] = shot;
        x_buf[po] = (int16_t)(x0 * 100);    //�����_2���܂ł�100�{�l�@-327.68~327.67
        y_buf[po] = (int16_t)(y0 * 100);
        draw_impact_point(x_buf[po], y_buf[po], RED);
        
        //�Z���^to�Z���^�̌v�Z
        ctc_color = CYAN;
        for (i = 0; i < po; i++){
            ctc_tmp = (float)(x_buf[po] - x_buf[i]) * (float)(x_buf[po] - x_buf[i]);  
            ctc_tmp += (float)(y_buf[po] - y_buf[i]) * (float)(y_buf[po] - y_buf[i]);
            if (ctc_tmp > ctc_max){
                //�X�V(���e���L���Ȃ���)��
                ctc_max = ctc_tmp;
                ctc_color = RED;
            }
        }

        //print_target_ctc((float)(sqrt(ctc_max) / 100), color);//////////////////////

        
        //�������|�C���^��i�߂�
        po++;
        if (po >= NUM_BUFFER){
            po = 0;
        }
        //�f�[�^����i�߂�
        num++;
        if (num >= NUM_BUFFER){
            num = NUM_BUFFER - 1;    //�f�[�^���͏���Ŏ~�܂�B
        }
        
    }else {
        //ReDraw = 1:�ĕ`��
        if (num > po){
            //�f�[�^�����|�C���^�l���傫��(�o�b�t�@�͂����ς�)
            i_max = NUM_BUFFER;
            //�o�b�t�@�[���̑S�f�[�^��`��
        }else {
            i_max = po;
            //�|�C���^�����ЂƂO�܂ł̃f�[�^��`��
        }
        for (i = 0; i < i_max; i++ ){
            draw_impact_point(x_buf[i], y_buf[i], YELLOW);
        }
    }
    *n = num;       //�|�C���^�󂯓n��
    return (float)(sqrt(ctc_max) / 100);
}


void    draw_impact_point(int16_t x_x100, int16_t y_x100, uint8_t color){
    //���e�_��`��
    //x_x100,y_x100�͏����_�ȉ�2����100�{�l�@-327.68�`327.67
    uint16_t    draw_x, draw_y;     //LCD��̍��W
    
    draw_x = (uint16_t)(TARGET_X0 + SCALE * ((float)x_x100 / 100));    //LCD x+ �� ���e x+ �� 
    draw_y = (uint16_t)(TARGET_Y0 - SCALE * ((float)y_x100 / 100));    //LCD y+ �� ���e y+ ��

    
    //�\���͈͂���݂͂������̃`�F�b�N
    if ((draw_x > (TARGET_X_MAX - PLOT_D)) || (draw_x < (TARGET_X_MIN + PLOT_D))){
        //��ʊO
        return;
    }
    if ((draw_y > (TARGET_Y_MAX - PLOT_D)) || (draw_y < (TARGET_Y_MIN + PLOT_D))){
        //��ʊO
        return;
    }
    
    //���e�_
    if (RED == color){
        LCD_SetColor(0xff, 0x00, 0x00);             //RED
    }else if(ORANGE == color){
        LCD_SetColor(0xff, 0xdf, 0x00);             //ORANGE
    }else if(YELLOW == color){
        LCD_SetColor(0xff, 0xff, 0x00);             //YELLOW

    }
    
    LCD_DrawFillCircle(draw_x, draw_y, (uint16_t)(PLOT_D / 2));
    //���ǂ�
    if ((draw_x < APS_X_MIN) || (draw_x > APS_X_MAX) || (draw_y < APS_Y_MIN) || (draw_y > APS_Y_MAX)){
        //APS�}�g���O(���o�b�N)
        LCD_SetColor(0x7f, 0x7f, 0x7f);             //GREY
    }else {
        //APS�}�g����(���o�b�N)
        LCD_SetColor(0x00, 0x00, 0x00);             //BLACK
    }
    LCD_DrawCircle(draw_x, draw_y, (uint16_t)(PLOT_D / 2));
}