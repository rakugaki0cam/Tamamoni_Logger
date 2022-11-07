/* 
 * File:   motion_graph.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 * Motion DATA process
 * Graph 
 * 
 * Revision history: 
 * 2021.10.10   motion.cよりグラフ表示部を分離
 * 2021.10.10   横表示幅を広くした
 *
 * V8
 * 2022.04.24   モーション表示を切り離し
 * 2022.05.08   モーションプロットをターゲットモードによってオンオフ
 *              グラフのクリア範囲をターゲットに合わせる
 * 
 * 
*/

#include "math.h"///////////////
#include "header.h"
#include "motion_graph.h"


#define     MOTION_DX           (DISPLAY_WIDTH / 2)     //表示の大きさ(1/2だと画面全幅表示)
#define     MOTION_DY           ((DISPLAY_WIDTH / 2) - 10)
#define     MOTION_DY_OFFSET   4
#define     MOTION_X_MIN        MOTION_X0 - MOTION_DX           //表示の範囲
#define     MOTION_Y_MIN        MOTION_Y0 - MOTION_DY + MOTION_DY_OFFSET
#define     MOTION_X_MAX        MOTION_X0 + MOTION_DX
#define     MOTION_Y_MAX        MOTION_Y0 + MOTION_DY + MOTION_DY_OFFSET

#define     NUM_MIL         2
#define     MIL_SCALE_X     30      //1目盛(MIL)のドット数
#define     MIL_SCALE_Y     40
#define     PLOT_CIRCLE_R   2       //グラフプロット点の円半径


uint16_t    x_before, y_before;             //motionラインの前回値


void motion_graph_initialize(void){
    //モーショングラフ画面の初期化
    //撃つたび毎回
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
    //クリア
    motion_clear_screen();
    //枠
    LCD_SetColor(0x80, 0x80, 0x80);                 //LIGHTGRAY
    LCD_DrawBox(MOTION_X0, MOTION_Y_MIN, MOTION_X0, MOTION_Y_MAX);        //y軸線
    LCD_DrawBox(MOTION_X_MIN, MOTION_Y0, MOTION_X_MAX, MOTION_Y0);        //x軸線
    
    LCD_SetColor(0x40, 0x40, 0x40);                 //GRAY
    LCD_DrawBox(x0, y0, x1, y1);                    //外枠
    LCD_DrawBox(x0 + MIL_SCALE_X, y0 + MIL_SCALE_Y, x1 - MIL_SCALE_X, y1 - MIL_SCALE_Y);//内枠

    LCD_Printf(x1 - MIL_SCALE_X - 6, y0 - 3      , MIL_1, 1, WHITE, 1);
    LCD_Printf(x1 - 6     , y0 - 3      , MIL_2, 1, WHITE, 1);
    LCD_Printf(x0 - 12    , y0 + MIL_SCALE_Y - 3 , MIL_3, 1, WHITE, 1);
    LCD_Printf(x0 - 12    , y0 - 3      , MIL_4, 1, WHITE, 1);
    
    x_before = MOTION_X0;
    y_before = MOTION_Y0;
}


void motion_clear_screen(void){
    //クリア
    LCD_SetColor(0x0, 0x0, 0x0);                    //BLACK
    LCD_DrawFillBox(MOTION_X_MIN, MOTION_Y_MIN - 4, MOTION_X_MAX, MOTION_Y_MAX);
}


void motion_plot_graph(float sigmay, float sigmaz, float shot_offset){
    //モーションデータをグラフにする　<- motion_print_gyro() <-　motion_log_sd_write()から呼び出し
    //shot_offsetは、玉が発射タイミングの時のオフセット割合、発射の時でない時は負数
    
    uint16_t    x1, y1;
    uint16_t    xs, ys;
    uint8_t     ky, kz;         //ジャイロモニターの時のグラフ表示　移動量を1/10に。　単位10MIL

#ifdef GYRO_MONITOR_TEST
    //+-10MIL表示
    ky = 5;
    kz = 5;
#else
    //+-2MIL表示
    //ky = 1;
    //kz = 1;
                
    //UD +-6MIL, RL +-2MIL表示
    ky = 3;
    kz = 1;
#endif
                
    //表示範囲からはみだすかのチェック
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
        //発射時のレッドマーク
        //xs = (uint16_t)(((x1 - x_before) * shot_offset) + x_before);    //座標はプラスしかないためuint16_tにしている
        //??????uint とintでwarning出たり出なかったり
        xs = (uint16_t)(((int16_t)x1 - (int16_t)x_before) * shot_offset + (int16_t)x_before);    //座標はプラスしかないためuint16_tにしている
        ys = (uint16_t)(((int16_t)y1 - (int16_t)y_before) * shot_offset + (int16_t)y_before);    //x1-x_beforeは負もあるのでint16_tが必要
        LCD_SetColor(0xff, 0x00, 0x8f);         //MAGENTA
        LCD_DrawFillCircle(xs, ys, PLOT_CIRCLE_R + 2);  //大きい円
        
        //debug
        //LCD
        //sprintf(tmp_string, "%5.3f", shot_offset);
        //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, RED, 1); 
        //debugger
        //printf("k:%5.3f", shot_offset\n");
    }

    //前回値として保存
    x_before = x1;
    y_before = y1;
    
}
