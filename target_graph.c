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
 * 2022.04.24   新規
 * 2022.05.22   着弾データバッファをグラフ座標からx100のint16_t値に変更(センター～センター計算のため)
 * 2022.10.16   ターゲットリセット(CtoC=0)を追加
 * 
 * 
*/

#include "math.h"   ///////////////
#include "header.h"
#include "target_graph.h"


#define     TARGET_X0           ((DISPLAY_WIDTH / 2) + 0)   //グラフ原点0位置
#define     TARGET_Y0           ((DISPLAY_HEIGHT / 2) + 0)
#define     TARGET_DX           (DISPLAY_WIDTH / 2)     //表示の大きさ(1/2だと画面全幅表示)
#define     TARGET_DY           ((DISPLAY_WIDTH / 2) - 10)
#define     TARGET_DY_OFFSET    4
#define     TARGET_X_MIN        TARGET_X0 - TARGET_DX   //表示の範囲
#define     TARGET_Y_MIN        TARGET_Y0 - TARGET_DY + TARGET_DY_OFFSET
#define     TARGET_X_MAX        TARGET_X0 + TARGET_DX
#define     TARGET_Y_MAX        TARGET_Y0 + TARGET_DY + TARGET_DY_OFFSET
#define     TARGET_D            (DISPLAY_WIDTH / 4)     //ターゲット円の大きさピクセル240/4=60
#define     ROW_TARGET          40                      //Vmeasure3.cのx、y表示ライン位置より

//APSターゲット 単位[mm]
#define     R_NUM       8       //円の数
#define     APS_D_X     10.5    //円寸法mm
#define     APS_D_10p1  19.5    //10点内側
#define     APS_D_10    20.3    //10点
#define     APS_D_9p9   21.0    //10点外側
#define     APS_D_8     34.5
#define     APS_D_5     49.5
#define     APS_WIDTH   91      //横幅
#define     APS_HIGHTU  67      //上側
#define     APS_HIGHTD  61      //下側
//mm->pixel
#define     SCALE       (float)(TARGET_D / APS_D_5) //mmをピクセルに換算
//APSマト紙のpixelサイズ
#define     APS_X_MIN   (TARGET_X0 - SCALE * (APS_WIDTH / 2))
#define     APS_Y_MIN   (TARGET_Y0 - SCALE * APS_HIGHTU)
#define     APS_X_MAX   (TARGET_X0 + SCALE * (APS_WIDTH / 2))
#define     APS_Y_MAX   (TARGET_Y0 + SCALE * APS_HIGHTD)
//中心xマーク
#define     POINT       "x"

//玉
#define     BB          4                   //着弾円寸法mm
#define     PLOT_D      (float)(SCALE * BB) //グラフプロット点の円半径

//impact_plot_graph呼出時の引数
#define DUMMY       0 
#define REDRAW_NONE 0
#define REDRAW      1
#define RESET_NONE  0  
#define RESET_DONE  1     


//GLOBAL
uint8_t     ctc_color;      //////////////////////////localにしたい



void target_graph_initialize(void){
    //ターゲット画面の初期表示
    //1ラウンドの間、維持。
    uint8_t i;
    uint16_t    taget_circle[R_NUM] = {
                    (uint16_t)(SCALE * APS_D_X),
                    (uint16_t)(SCALE * APS_D_10p1),
                    (uint16_t)(SCALE * APS_D_10),
                    (uint16_t)(SCALE * APS_D_9p9),
                    (uint16_t)(SCALE * APS_D_8),
                    (uint16_t)(SCALE * APS_D_5)
                };
    
    //クリア
    target_clear_screen();
    //枠
    //APS紙の大きさ
    LCD_SetColor(0xff, 0xff, 0xff);                     //WHITE
    LCD_DrawFillBox((uint16_t)APS_X_MIN, (uint16_t)APS_Y_MIN, (uint16_t)APS_X_MAX, (uint16_t)APS_Y_MAX);
    //軸
    LCD_SetColor(0x80, 0x80, 0x80);                     //GREY
    LCD_DrawBox(TARGET_X0, TARGET_Y_MIN, TARGET_X0, TARGET_Y_MAX);    //y軸線
    LCD_DrawBox(TARGET_X_MIN, TARGET_Y0, TARGET_X_MAX, TARGET_Y0);    //x軸線
    
    
    for (i = 0; i < R_NUM; i++ ){
        //同心円を描く
        LCD_SetColor(0x00, 0x00, 0x00);                 //BLACK
        LCD_DrawCircle(TARGET_X0, TARGET_Y0, taget_circle[i] / 2);

    }
    //中心の'x'
    LCD_Printf(TARGET_X0 - 2, TARGET_Y0 - 4, POINT, 1, BLACK, 0);
    
    //着弾点再描画 
    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW, RESET_NONE, DUMMY);
    
}


void target_data_reset(void){
    //ターゲットデータをリセットする。　CtoC→0mm
    //SW2長押し
    uint8_t     i;
    device_connect_t    targetmode;

    targetmode = target_mode_get();
    if ((TARGET_ONLY_MODE != targetmode) && (V0_TARGET_MODE != targetmode)){
        return;
    }
    
    sprintf(tmp_string, "TARGET CLEAR?     ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1); 
    //1秒長押し
    for (i = 0; i < 15; i++){
        __delay_ms(100);
        if (SW2_PORT == SW_OFF){
            sprintf(tmp_string, "                  ");
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
            return;
        }
    }
    //電子ターゲットにクリアコマンドを送る
    uint8_t clear[] = "CLEAR";
    command_uart_send_esp32(clear);     //WiFi
    command_uart_send_rs485(clear);     //LAN                                    //LANで接続の時も必要
    
    //ターゲットをクリア
    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE, DUMMY);
    target_graph_initialize();
    sprintf(tmp_string, "DONE!             ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
    __delay_ms(1000);
    sprintf(tmp_string, "                  ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);

    while(SW2_PORT == SW_ON){
        //キーが離されるのを待つ
    }
    
}



void target_clear_screen(void){
    //クリア
    LCD_SetColor(0x00, 0x00, 0x00);                     //BLACK
    //LCD_DrawFillBox(TARGET_X_MIN, TARGET_Y_MIN, TARGET_X_MAX, TARGET_Y_MAX); 
    LCD_DrawFillBox(TARGET_X_MIN, ROW_TARGET, TARGET_X_MAX, TARGET_Y_MAX); 
}


float impact_plot_graph(uint16_t shot, float x0, float y0, bool redraw, bool reset, uint16_t* n){
    //着弾点表示
    //引数　sh:ショット#, x0,y0:着弾座標
    //オプション　redraw:グラフ再描画,  reset:CtoC計算リセット
    //プロットデータをバッファーへ保存
    //戻り値:CtoC[mm]
    //      n:num CtoCのサンプル数　ポインタにて受け渡し
    
#define NUM_BUFFER  50
   
    static uint16_t shot_buf[NUM_BUFFER];
    static int16_t  x_buf[NUM_BUFFER], y_buf[NUM_BUFFER];   //x100値
    static uint8_t  po = 0;         //バッファのポインタ
    static uint16_t num = 0;        //データ数
    static float    ctc_max = 0;    //(最大着弾間隔x100mm)＾2 表示の時にルートする
    float           ctc_tmp;
    uint8_t         i, i_max;
    int8_t          b;
    
    if (reset == 1){
        //ターゲットをリセットする(射撃を仕切り直ししたい時)
        //このルーチン内でしかリセットできないため
        po = 0;
        num = 0;
        *n = num;       //ポインタ受け渡し
        ctc_max = 0;    //CtoCをゼロにする
        return 0;
    }
    
    if (redraw == 0){
        //ReDraw = 0: 一着弾点を描画
        
        //前回の着弾を黄色に
        if (num > 0){
            b = (int8_t)po - 1;
            if (b < 0){
                b = NUM_BUFFER - 1;
            }
            draw_impact_point(x_buf[b], y_buf[b], YELLOW);
        }
           
        //データバッファへの保管用16ビット値への計算とメモリへ記憶
        shot_buf[po] = shot;
        x_buf[po] = (int16_t)(x0 * 100);    //小数点2桁までの100倍値　-327.68~327.67
        y_buf[po] = (int16_t)(y0 * 100);
        draw_impact_point(x_buf[po], y_buf[po], RED);
        
        //センタtoセンタの計算
        ctc_color = CYAN;
        for (i = 0; i < po; i++){
            ctc_tmp = (float)(x_buf[po] - x_buf[i]) * (float)(x_buf[po] - x_buf[i]);  
            ctc_tmp += (float)(y_buf[po] - y_buf[i]) * (float)(y_buf[po] - y_buf[i]);
            if (ctc_tmp > ctc_max){
                //更新(着弾が広くなった)時
                ctc_max = ctc_tmp;
                ctc_color = RED;
            }
        }

        //print_target_ctc((float)(sqrt(ctc_max) / 100), color);//////////////////////

        
        //メモリポインタを進める
        po++;
        if (po >= NUM_BUFFER){
            po = 0;
        }
        //データ数を進める
        num++;
        if (num >= NUM_BUFFER){
            num = NUM_BUFFER - 1;    //データ数は上限で止まる。
        }
        
    }else {
        //ReDraw = 1:再描画
        if (num > po){
            //データ数がポインタ値より大きい(バッファはいっぱい)
            i_max = NUM_BUFFER;
            //バッファー内の全データを描画
        }else {
            i_max = po;
            //ポインタよりもひとつ前までのデータを描画
        }
        for (i = 0; i < i_max; i++ ){
            draw_impact_point(x_buf[i], y_buf[i], YELLOW);
        }
    }
    *n = num;       //ポインタ受け渡し
    return (float)(sqrt(ctc_max) / 100);
}


void    draw_impact_point(int16_t x_x100, int16_t y_x100, uint8_t color){
    //着弾点を描写
    //x_x100,y_x100は小数点以下2桁の100倍値　-327.68～327.67
    uint16_t    draw_x, draw_y;     //LCD上の座標
    
    draw_x = (uint16_t)(TARGET_X0 + SCALE * ((float)x_x100 / 100));    //LCD x+ → 着弾 x+ → 
    draw_y = (uint16_t)(TARGET_Y0 - SCALE * ((float)y_x100 / 100));    //LCD y+ ↓ 着弾 y+ ↑

    
    //表示範囲からはみだすかのチェック
    if ((draw_x > (TARGET_X_MAX - PLOT_D)) || (draw_x < (TARGET_X_MIN + PLOT_D))){
        //画面外
        return;
    }
    if ((draw_y > (TARGET_Y_MAX - PLOT_D)) || (draw_y < (TARGET_Y_MIN + PLOT_D))){
        //画面外
        return;
    }
    
    //着弾点
    if (RED == color){
        LCD_SetColor(0xff, 0x00, 0x00);             //RED
    }else if(ORANGE == color){
        LCD_SetColor(0xff, 0xdf, 0x00);             //ORANGE
    }else if(YELLOW == color){
        LCD_SetColor(0xff, 0xff, 0x00);             //YELLOW

    }
    
    LCD_DrawFillCircle(draw_x, draw_y, (uint16_t)(PLOT_D / 2));
    //縁どり
    if ((draw_x < APS_X_MIN) || (draw_x > APS_X_MAX) || (draw_y < APS_Y_MIN) || (draw_y > APS_Y_MAX)){
        //APSマト紙外(黒バック)
        LCD_SetColor(0x7f, 0x7f, 0x7f);             //GREY
    }else {
        //APSマト紙内(白バック)
        LCD_SetColor(0x00, 0x00, 0x00);             //BLACK
    }
    LCD_DrawCircle(draw_x, draw_y, (uint16_t)(PLOT_D / 2));
}