/* 
 * File:   menu.c
 * Author: IsobeR
 * Comments: PIC18F57Q43 MCC
 *
 * menu process
 * 
 *  2022.10.19
 * 
 */

#include "header.h"
#include "menu.h"

//表示の範囲
#define     MENU_X_MIN      0
#define     MENU_Y_MIN      0
#define     MENU_X_MAX      239
#define     MENU_Y_MAX      299
   
//文字サイズ  2倍角文字　12x16pixel
#define     B2X     12
#define     B2Y     16
#define     B1Y     8   //中間高さ
//デバッグ画面表示
#define     COL_BUTTON_SEL  (6*24)
#define     ROW_BUTTON_SEL  (8*35)
    
//ボタン名称  enumは定数なのでメモリはくわない
typedef enum {
    IDLE,
    DIST_M,
    DIST_MM,        
    AIRSOFT,
    BB_G,
    BB_NAME,
    NUKIDAN,
    RETURN,
    POWEROFF,
    SET_TIME,
    UP,
    DOWN,
    PLUS,
    MINUS,
    NUM_BUTTON_SETUP, //宣言数  =　メッセージ配列添字数     
} button_setup_t;
#define BUTTON_SETUP_LEN     NUM_BUTTON_SETUP

//
//メニューのボタン位置の設定
const uint16_t button_setup[BUTTON_SETUP_LEN][4] = {
  //{ 　　x0,  　y0, 　横幅, 高さ}, 
    {   240,   320,     0,   0},    //dummy
    { 9*B2X, 1*B2Y, 2*B2X, B2Y},    //dist m
    {12*B2X, 1*B2Y, 3*B2X, B2Y},    //dist mm
    { 8*B2X, 3*B2Y,12*B2X, B2Y},    //airsoft
    { 8*B2X, 5*B2Y, 5*B2X, B2Y},    //bb g
    { 8*B2X, 7*B2Y,12*B2X, B2Y},    //bb name
    { 8*B2X, 9*B2Y, 3*B2X, B2Y},    //nukidan
    {14*B2X,11*B2Y, 6*B2X, B2Y},    //return
    {12*B2X,14*B2Y, 8*B2X, B2Y},    //poweroff
    {13*B2X,17*B2Y, 7*B2X, B2Y},    //settime
    //ボタン高さ2
    { 7*B2X,11*B2Y, 3*B2X, B2Y*2},    //up  
    { 7*B2X,16*B2Y, 3*B2X, B2Y*2},    //down
    { 1*B2X,11*B2Y, 3*B2X, B2Y*2},    //+  
    { 1*B2X,16*B2Y, 3*B2X, B2Y*2},    //- 
};

//ボタンの表示テキスト 10文字まで
const char bu_text_setup[BUTTON_SETUP_LEN][11] = {
    "",             //dummy
    "  .",
    "    m",
    "",
    "      g",
    "",
    "    gf",
    "RETURN",
    "PowerOFF",
    "SetTIME",
    //中間高さに表示
    {0x20, 0x80, 0x00},     //↑
    {0x20, 0x81, 0x00},     //↓
    " + ",
    " - ",
};

//手入力セットアップ値
//前回値をeepromから読み込み
//global
int16_t     bbmass_g = 250;     //x1000値 mg    0x00fa
//local
int8_t      dist_m = 7;         //m +   0x07
int16_t     dist_mm = 500;      //mm    0x01f4
uint8_t     gun_num = 2;        //      0x02
uint8_t     bb_num = 7;         //      0x07
int16_t     f_extract = 195;    //gf    0x00c3


#define DIST_M_MIN  0
#define DIST_M_MAX  99
#define DIST_MM_MIN 0
#define DIST_MM_MAX 999
#define BB_G_MIN    100
#define BB_G_MAX    500
#define EXT_F_MIN   0
#define EXT_F_MAX   600

#define AIR_GUN_NUM     14
char air_gun_text[AIR_GUN_NUM][13] = { //max12文字+1stopcode
    "dummy       ",
    "vfc M40A3   ",
    "ares M40A6  ",
    "VSR10-G     ",
    "VSR10Gstrobe",
    "VSR10       ",
    "vfc M110 GBB",
    "vfc M4A1 GBB",
    "vfc M27 GBB ",
    "vfc MP5 GBB ",
    "P99 U10motor",
    "M1911 air   ",
    "SR635 motor ",
    "others      ",
};

#define     BB_TYPE_NUM        9
char bb_type_text[BB_TYPE_NUM][13] = {    //max12文字+1stopcode
    "dummy       ",
    "G&G Pla     ",
    "G&G Bio     ",
    "Marui Bio   ",
    "BLS Pla     ",
    "BLS Bio     ",
    "NovritschBio",
    "PM Bio      ",
    "others      ",
};


void    setup_menu(void){
    //設定メニュー
    //キーは長押し途中で離した状態
    uint8_t     i;
    uint16_t    by;

    menu_clear_screen();
    sprintf(tmp_string, "-- SET UP ----------");
    LCD_Printf( 0*B2X, 0*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "distance");
    LCD_Printf( 0*B2X, 1*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "airsoft");
    LCD_Printf( 0*B2X, 3*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "BB mass");
    LCD_Printf( 0*B2X, 5*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "BB type");
    LCD_Printf( 0*B2X, 7*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "nukidan");
    LCD_Printf( 0*B2X, 9*B2Y, tmp_string, 2, YELLOW, 1);
    
    //ボタンの文字を表示
    for(i = 1; i < NUM_BUTTON_SETUP; i++){
        sprintf(tmp_string, "%s", bu_text_setup[i]);
        by = button_setup[i][1];
        if (button_setup[i][3] == B2Y*2){
            //ボタンの文字をボタンの中間高さ位置に表示
            by += B1Y;
        }
        LCD_Printf(button_setup[i][0], by, tmp_string, 2, YELLOW, 1);
    }
    
    
    //設定値を表示
    sprintf(tmp_string, "%2d", dist_m);
    LCD_Printf(9*B2X, 1*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%03d", dist_mm);
    LCD_Printf(12*B2X, 1*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%s", air_gun_text[gun_num]);
    LCD_Printf(8*B2X, 3*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%5.3f", (float)bbmass_g / 1000);
    LCD_Printf(8*B2X, 5*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%s", bb_type_text[bb_num]);
    LCD_Printf(8*B2X, 7*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%3d", f_extract);
    LCD_Printf(8*B2X, 9*B2Y, tmp_string, 2, WHITE, 1);
    
    //ボタンを表示
    button_init(button_setup, NUM_BUTTON_SETUP);

    touch_menu();       //タッチメニュー
        
    //元の画面を再描画
    __delay_ms(500);
    menu_clear_screen();
    angle_level_tilt_disp_init();
    print_targetmode(INDIGO);      //sensor_connect_check()は状態変化がないとき表示されないので
    
}


void touch_menu(void) {
    //タッチメニュー
    button_setup_t  bnum;
    button_setup_t  bsel = RETURN;  //選択中のボタン
    uint8_t         touch_cnt = 0;  //タッチが連続しているカウント
    uint16_t        plus_val = 1;   //数値変化分
    
    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);   //最初のボタンセレクトの表示

    while(1){
        if (sw1_int_flag == 1){
            power_switch();         //ちょん押しでリターン、長押しでパワーオフ
            sw1_int_flag = 0;
            write_rom_setup();
            return;                 //menuを抜ける  
        }
        
        if (RTC_int_flag == 1){
            footer_rewrite(1);      //1:setupからの呼び出し
            RTC_int_flag = 0;///////////////クリアしないとなぜかうまくいかない
        }

        if (TOUCH_INT_PORT == 0){
            //タッチあり
            if(touch_cnt == 0){
                touch_cnt = 1;
                plus_val = 1;/////////////////////////うまく使うと大送り後の保持に活かせるかも？たくさん行きすぎた後の修正が大変
            }else{
                //連続タッチ中
                touch_cnt ++;
                if (touch_cnt > 100){
                    touch_cnt = 100;
                }
                if (touch_cnt > 10){
                    plus_val = 10;
                }
            }
            
            bnum = button_number(button_setup, NUM_BUTTON_SETUP);
                    
            switch(bnum){
                case IDLE:
                    //idle
                    break;
                case DIST_M:
                case DIST_MM:
                case AIRSOFT:
                case BB_G:
                case BB_NAME:
                case NUKIDAN:
                    //前回の位置を消去(枠グレイ表示)
                    button_select_disp(bsel, 0, button_setup, NUM_BUTTON_SETUP);
                    bsel = bnum;
                    //ボタン選択表示(黄色)
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    __delay_ms(300);
                    break;
                    
                case UP:
                    //項目間の移動　上へ
                    button_select_disp(bsel, 0, button_setup, NUM_BUTTON_SETUP);
                    bsel--;
                    if(bsel < DIST_M){
                        bsel = DIST_M;
                    }
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    __delay_ms(300);
                    break;
                    
                case DOWN:
                    //項目間の移動　下へ
                    button_select_disp(bsel, 0, button_setup, NUM_BUTTON_SETUP);
                    bsel++;
                    if(bsel > RETURN){
                        bsel = RETURN;
                    }
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    __delay_ms(300);
                    break;
                    
                case PLUS:
                    switch(bsel){
                        case DIST_M:
                            dist_m += plus_val;
                            if (dist_m > DIST_M_MAX){
                                dist_m = DIST_M_MAX;
                            }
                            sprintf(tmp_string, "%2d", dist_m);
                            break;
                        case DIST_MM:
                            dist_mm += plus_val;
                            if (dist_mm > DIST_MM_MAX){
                                dist_mm -= (DIST_MM_MAX + 1);
                            }
                            sprintf(tmp_string, "%03d", dist_mm);
                            break;
                        case AIRSOFT:
                            gun_num++;
                            if (gun_num >= AIR_GUN_NUM){
                                gun_num = 1;
                            }
                            sprintf(tmp_string, "%s", air_gun_text[gun_num]);
                            __delay_ms(200);
                            break;    
                        case BB_G:
                            bbmass_g += plus_val;
                            if (bbmass_g > BB_G_MAX){
                                bbmass_g = BB_G_MAX;
                            }
                            sprintf(tmp_string, "%5.3f", (float)bbmass_g / 1000);
                            break;
                        case BB_NAME:
                            bb_num++;
                            if (bb_num >= BB_TYPE_NUM){
                                bb_num = 1;
                            }
                            sprintf(tmp_string, "%s", bb_type_text[bb_num]);
                            __delay_ms(200);
                            break;
                        case NUKIDAN:
                            f_extract+= plus_val;
                            if (f_extract > EXT_F_MAX){
                                f_extract = EXT_F_MAX;
                            }
                            sprintf(tmp_string, "%3d", f_extract);
                            break;
                        case RETURN:
                            button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                            write_rom_setup();
                            __delay_ms(100);
                            return;                 //menuを抜ける
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], tmp_string, 2, WHITE, 1);
                    break;
                    
                case MINUS:
                    switch(bsel){
                        case DIST_M:
                            dist_m -= plus_val;
                            if (dist_m < DIST_M_MIN){
                                dist_m = DIST_M_MIN;
                            }
                            sprintf(tmp_string, "%2d", dist_m);
                            break;
                        case DIST_MM:
                            dist_mm -= plus_val;
                            if (dist_mm < DIST_MM_MIN){
                                dist_mm += (DIST_MM_MAX + 1);
                            }
                            sprintf(tmp_string, "%03d", dist_mm);
                            break;
                        case AIRSOFT:
                            gun_num--;
                            if (gun_num < 1){
                                gun_num = AIR_GUN_NUM - 1;
                            }
                            sprintf(tmp_string, "%s", air_gun_text[gun_num]);
                            __delay_ms(200);
                            break;    
                        case BB_G:
                            bbmass_g -= plus_val;
                            if (bbmass_g < BB_G_MIN){
                                bbmass_g = BB_G_MIN;
                            }
                            sprintf(tmp_string, "%5.3f", (float)bbmass_g / 1000);
                            break;
                        case BB_NAME:
                            bb_num--;
                            if (bb_num < 1){
                                bb_num = BB_TYPE_NUM - 1;
                            }
                            __delay_ms(200);
                            sprintf(tmp_string, "%s", bb_type_text[bb_num]);
                            break;
                        case NUKIDAN:
                            f_extract -= plus_val;
                            if (f_extract < EXT_F_MIN){
                                f_extract = EXT_F_MIN;
                            }
                            sprintf(tmp_string, "%3d", f_extract);
                            break;
                        case RETURN:
                            button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                            write_rom_setup();
                            __delay_ms(100);
                            return;                 //menuを抜ける
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], tmp_string, 2, WHITE, 1);
                    break;
                    
                case RETURN:
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    write_rom_setup();
                    __delay_ms(100);
                    return;                 //menuを抜ける
                    break;
                case SET_TIME:
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    write_rom_setup();
                    __delay_ms(100);
                    RTC8900_time_set();     //時計合わせ
                    return;                 //menuを抜ける
                    break;
                case POWEROFF:
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    write_rom_setup();
                    __delay_ms(100);
                    power_off();        //パワーオフ
                    break;
                default:
                    break;
            }
        
#if DEBUG_BUTTON == 1
        //button select
        sprintf(tmp_string, "bu.sel = %2d", bsel);
        LCD_Printf(COL_BUTTON_SEL, ROW_BUTTON_SEL, tmp_string, 1, RED, 1);
#endif
        }else{
            touch_cnt = 0;     //連続タッチ終わり
            plus_val = 1;
        }   //if

    }   //while
}


void    menu_clear_screen(void){
    //クリア
    LCD_SetColor(0x00, 0x00, 0x00);                     //BLACK
    LCD_DrawFillBox(MENU_X_MIN, MENU_Y_MIN, MENU_X_MAX, MENU_Y_MAX); 
}


void    set_setup(void){
    //マト距離とエアソフトガンの種類とBB弾の種類のセット
    sprintf(&bullet_CSVdata[DISTANCE][0], "%8.3f", (float)dist_mm / 1000 + dist_m);
    sprintf(&bullet_CSVdata[AIRSOFT_TYPE][0], "%s", air_gun_text[gun_num]);     //12文字
    sprintf(&bullet_CSVdata[BB_MASS][0], "%6.3f", (float)bbmass_g / 1000);
    sprintf(&bullet_CSVdata[BB_TYPE][0], "%s", bb_type_text[bb_num]);           //12文字
    sprintf(&bullet_CSVdata[F_EXTRACT][0], "%7d", f_extract);
    
}


void    read_rom_setup(void){
    //EEP ROMからセットアップデータを代入
    
    dist_m = (int8_t)DATAEE_ReadByte(DIST_M_ADRESS);
    if ((dist_m < DIST_M_MIN) || (dist_m > DIST_M_MAX)){
        dist_m = 10;
    }
    dist_mm = DATAEE_ReadByte(DIST_MM_ADRESS);
    dist_mm += DATAEE_ReadByte(DIST_MM_ADRESS+1) << 8;
    if ((dist_mm < DIST_MM_MIN) || (dist_mm > DIST_MM_MAX)){
        dist_mm = 0;
    }    
    gun_num = DATAEE_ReadByte(GUN_NUM_ADDRESS);
    if ((gun_num < 1) || (gun_num > AIR_GUN_NUM)){
        gun_num = 1;
    }  
    bbmass_g = DATAEE_ReadByte(BB_G_ADDRESS);
    bbmass_g += DATAEE_ReadByte(BB_G_ADDRESS+1) << 8;
    if ((bbmass_g < BB_G_MIN) || (bbmass_g > BB_G_MAX)){
        bbmass_g = 280;
    }    
    bb_num = DATAEE_ReadByte(BB_NUM_ADDRESS);
    if ((bb_num < 1) || (bb_num > BB_TYPE_NUM)){
        bb_num = 1;
    }  
    f_extract = DATAEE_ReadByte(NUKIDAN_ADDRESS);
    f_extract += DATAEE_ReadByte(NUKIDAN_ADDRESS+1) << 8;
    if ((f_extract < EXT_F_MIN) || (f_extract > EXT_F_MAX)){
        f_extract = 150;
    }    
    
    set_setup();
}


void    write_rom_setup(void){
    //EEPROMへセットアップデータを書き込み
    
    DATAEE_WriteByte(DIST_M_ADRESS, (uint8_t)dist_m);
    
    DATAEE_WriteByte(DIST_MM_ADRESS, (uint8_t)dist_mm);
    DATAEE_WriteByte(DIST_MM_ADRESS+1, (uint8_t)(dist_mm >> 8));
    
    DATAEE_WriteByte(GUN_NUM_ADDRESS, gun_num);
    
    DATAEE_WriteByte(BB_G_ADDRESS, (uint8_t)bbmass_g);
    DATAEE_WriteByte(BB_G_ADDRESS+1, (uint8_t)(bbmass_g >> 8));
    
    DATAEE_WriteByte(BB_NUM_ADDRESS,  bb_num);
    
    DATAEE_WriteByte(NUKIDAN_ADDRESS, (uint8_t)f_extract);
    DATAEE_WriteByte(NUKIDAN_ADDRESS+1, (uint8_t)(f_extract >> 8));
    
    set_setup();
}