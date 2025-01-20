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
    TARGET_MENU, //SET_TIME,
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
    {13*B2X,17*B2Y, 7*B2X, B2Y},    //targetmenu
    //ボタン高さ2
    { 7*B2X,12*B2Y, 3*B2X, B2Y*2},    //up  
    { 7*B2X,16*B2Y, 3*B2X, B2Y*2},    //down
    { 1*B2X,12*B2Y, 3*B2X, B2Y*2},    //+  
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
    "TargetM",
    //中間高さに表示
    {0x20, 0x80, 0x00},     //↑
    {0x20, 0x81, 0x00},     //↓
    " + ",
    " - ",
};

//手入力セットアップ値
//前回値をeepromから読み込み
//global
int16_t     bbmass_g = 280;     //x1000値 mg    0x0118

//local
static int8_t      dist_m = 7;         //m +   0x07
static int16_t     dist_mm = 500;      //mm    0x01f4
static uint8_t     gun_num = 3;        //      0x03
static uint8_t     bb_num = 1;         //      0x01
static int16_t     f_extract = 195;    //gf    0x00c3
//target
static uint8_t     target_id = 1;       //target選択
static int8_t      offset_h = -15;      //target側の初期値にあわせないといけない
static int8_t      aim_h = 74;
static int16_t     bright = 90;
//target command
const  uint8_t  str_com_target_id[] = "TARGET_ID";
const  uint8_t  str_com_offset[] = "OFFSET";
const  uint8_t  str_com_aimpoint[] = "AIMPOINT";
const  uint8_t  str_com_bright[] = "BRIGHT";

//setting
#define DIST_M_MIN  0
#define DIST_M_MAX  99
#define DIST_MM_MIN 0
#define DIST_MM_MAX 999
#define BB_G_MIN    100
#define BB_G_MAX    500
#define EXT_F_MIN   0
#define EXT_F_MAX   600
//target
#define TARGET_MIN      1
#define TARGET_MAX      2
#define OFFSET_MIN      -40
#define OFFSET_MAX      35
#define AIMPOINT_MIN    30
#define AIMPOINT_MAX    120
#define BRIGHTNESS_MIN  0
#define BRIGHTNESS_MAX  250


#define AIR_GUN_NUM     17
char air_gun_text[AIR_GUN_NUM][13] = { //max12文字+1stopcode
    "dummy       ",
    "vfc M40A3   ",
    "ares M40A6  ",
    "VSR10-G     ",
    "VSR10Gstrobe",
    "VSR10-ProSna",
    "MagpulPro700",
    "AMOEBA AS-01",
    "S&T M40A1   ",
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
    "HIT CALL pla",
    "NovritschBio",
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
    button_setup_t  bsel = RETURN;      //選択中のボタン
    uint8_t         touch_cnt = 0;      //タッチが連続しているカウント
    uint16_t        notouch_cnt = 0;    //タッチしていないカウント
    uint16_t        plus_val = 1;       //数値変化分
    uint8_t         color_val = WHITE;  //数値の表示色
    static char     string_val[16];     //メニュー数値書き込み&前回値保持
    
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
                //plus_val = 1;/////////////////////////うまく使うと大送り後の保持に活かせるかも？たくさん行きすぎた後の修正が大変
            }else{
                //連続タッチ中
                touch_cnt ++;
                __delay_ms(50);
                if (touch_cnt > 100){
                    touch_cnt = 100;
                }
                if (touch_cnt > 10){
                    //変化ステップを増やす
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
                    if(color_val == LEMON){
                        plus_val = 1;
                        color_val = WHITE;
                        LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);      //プラス10解除
                    }
                    bsel = bnum;
                    //ボタン選択表示(黄色)
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    __delay_ms(300);
                    break;
                    
                case UP:
                    //項目間の移動　上へ
                    button_select_disp(bsel, 0, button_setup, NUM_BUTTON_SETUP);
                    if(color_val == LEMON){
                        plus_val = 1;
                        color_val = WHITE;
                        LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);      //プラス10解除
                    }
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
                    if(color_val == LEMON){
                        plus_val = 1;
                        color_val = WHITE;
                        LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);      //プラス10解除
                    }
                    bsel++;
                    if(bsel > RETURN){
                        bsel = RETURN;
                    }
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    __delay_ms(300);
                    break;
                    
                case PLUS:
                    if (plus_val > 1){
                        //プラス分が大きい時は少しゆっくりに
                        __delay_ms(100);
                        color_val = LEMON;
                    }else{
                        color_val = WHITE;
                    }
                    
                    switch(bsel){
                        case DIST_M:
                            dist_m += plus_val;
                            if (dist_m > DIST_M_MAX){
                                dist_m = DIST_M_MAX;
                            }
                            sprintf(string_val, "%2d", dist_m);
                            break;
                        case DIST_MM:
                            dist_mm += plus_val;
                            if (dist_mm > DIST_MM_MAX){
                                dist_mm -= (DIST_MM_MAX + 1);
                            }
                            sprintf(string_val, "%03d", dist_mm);
                            break;
                        case AIRSOFT:
                            gun_num++;
                            color_val = WHITE;
                            if (gun_num >= AIR_GUN_NUM){
                                gun_num = 1;
                            }
                            sprintf(string_val, "%s", air_gun_text[gun_num]);
                            __delay_ms(300);
                            break;    
                        case BB_G:
                            bbmass_g += plus_val;
                            if (bbmass_g > BB_G_MAX){
                                bbmass_g = BB_G_MAX;
                            }
                            sprintf(string_val, "%5.3f", (float)bbmass_g / 1000);
                            break;
                        case BB_NAME:
                            bb_num++;
                            color_val = WHITE;
                            if (bb_num >= BB_TYPE_NUM){
                                bb_num = 1;
                            }
                            sprintf(string_val, "%s", bb_type_text[bb_num]);
                            __delay_ms(300);
                            break;
                        case NUKIDAN:
                            f_extract+= plus_val;
                            if (f_extract > EXT_F_MAX){
                                f_extract = EXT_F_MAX;
                            }
                            sprintf(string_val, "%3d", f_extract);
                            break;
                        case RETURN:
                            button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                            write_rom_setup();
                            __delay_ms(100);
                            return;                 //menuを抜ける
                            break;
                        default:
                            sprintf(string_val, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);
                    break;
                    
                case MINUS:
                    if (plus_val > 1){
                        //プラス分が大きい時は少しゆっくりに
                        __delay_ms(100);
                        color_val = LEMON;
                    }else{
                        color_val = WHITE;
                    }
                    
                    switch(bsel){
                        case DIST_M:
                            dist_m -= plus_val;
                            if (dist_m < DIST_M_MIN){
                                dist_m = DIST_M_MIN;
                            }
                            sprintf(string_val, "%2d", dist_m);
                            break;
                        case DIST_MM:
                            dist_mm -= plus_val;
                            if (dist_mm < DIST_MM_MIN){
                                dist_mm += (DIST_MM_MAX + 1);
                            }
                            sprintf(string_val, "%03d", dist_mm);
                            break;
                        case AIRSOFT:
                            gun_num--;
                            color_val = WHITE;
                            if (gun_num < 1){
                                gun_num = AIR_GUN_NUM - 1;
                            }
                            sprintf(string_val, "%s", air_gun_text[gun_num]);
                            __delay_ms(300);
                            break;    
                        case BB_G:
                            bbmass_g -= plus_val;
                            if (bbmass_g < BB_G_MIN){
                                bbmass_g = BB_G_MIN;
                            }
                            sprintf(string_val, "%5.3f", (float)bbmass_g / 1000);
                            break;
                        case BB_NAME:
                            bb_num--;
                            color_val = WHITE;
                            if (bb_num < 1){
                                bb_num = BB_TYPE_NUM - 1;
                            }
                            __delay_ms(300);
                            sprintf(string_val, "%s", bb_type_text[bb_num]);
                            break;
                        case NUKIDAN:
                            f_extract -= plus_val;
                            if (f_extract < EXT_F_MIN){
                                f_extract = EXT_F_MIN;
                            }
                            sprintf(string_val, "%3d", f_extract);
                            break;
                        case RETURN:
                            button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                            write_rom_setup();
                            __delay_ms(100);
                            return;                 //menuを抜ける
                            break;
                        default:
                            sprintf(string_val, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);
                    break;
                    
                case RETURN:
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    write_rom_setup();
                    __delay_ms(100);
                    return;                 //menuを抜ける
                    break;
                case TARGET_MENU:   //SET_TIME:
                    button_select_disp(bsel, 1, button_setup, NUM_BUTTON_SETUP);
                    write_rom_setup();
                    __delay_ms(100);
                    target_menu();
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
            //タッチ無し
            if (touch_cnt != 0){
                notouch_cnt = 1;
                touch_cnt = 0;     //連続タッチ終わり
            }
            if (notouch_cnt != 0){
                __delay_ms(1);
                notouch_cnt++;
                if((notouch_cnt > 1000) && (color_val == LEMON)){
                    //タッチ切れて少し間が生じたらプラス分を1に戻す。
                    //それまでは前のプラス10分を活かす
                    plus_val = 1;
                    color_val = WHITE;
                    LCD_Printf(button_setup[bsel][0], button_setup[bsel][1], string_val, 2, color_val, 1);      //プラス10解除を知らせる
                    notouch_cnt = 0;
                }
            }
        }   //if

    }   //while
}


void target_menu(void) {
    //ターゲットの設定メニュー
    
//impact_plot_graph呼出時の引数
#define DUMMY       0 
#define REDRAW_NONE 0
#define RESET_DONE  1     
    
    //ボタン名称
    typedef enum {
        DUMMY_B,
        TARGET_ID_SET,
        OFFSET_SET,        
        AIM_SET,
        TARGET_ID,
        OFFSET,
        AIMPOINT,
        BRIGHTNESS,
        SET_RETURN,
        CANCEL,
        SET_TIME,
        UP,
        DOWN,
        PLUS,
        MINUS,
        NUM_BUTTON_TARGET, //宣言数  =　メッセージ配列添字数     
    } button_target_t;
    
#define BUTTON_TARGET_LEN     NUM_BUTTON_TARGET
    
    //メニューのボタン位置の設定
    const uint16_t button_target[BUTTON_TARGET_LEN][4] = {
      //{ 　　x0,  　y0, 　横幅, 高さ}, 
        {   240,   320,     0,   0},    //dummy
        {17*B2X, 2*B2Y, 3*B2X, B2Y},    //target id set
        {17*B2X, 4*B2Y, 3*B2X, B2Y},    //offset set
        {17*B2X, 6*B2Y, 3*B2X, B2Y},    //aimpoint set
        {10*B2X, 2*B2Y, 3*B2X, B2Y},    //target id
        {10*B2X, 4*B2Y, 3*B2X, B2Y},    //offset y
        {10*B2X, 6*B2Y, 3*B2X, B2Y},    //aimpoint
        {12*B2X, 8*B2Y, 3*B2X, B2Y},    //brightness
        {14*B2X,11*B2Y, 6*B2X, B2Y},    //ok (set & return)
        {14*B2X,14*B2Y, 6*B2X, B2Y},    //cansel
        {13*B2X,17*B2Y, 7*B2X, B2Y},    //set time
        //ボタン高さ2
        { 7*B2X,12*B2Y, 3*B2X, B2Y*2},  //up  
        { 7*B2X,16*B2Y, 3*B2X, B2Y*2},  //down
        { 1*B2X,12*B2Y, 3*B2X, B2Y*2},  //+  
        { 1*B2X,16*B2Y, 3*B2X, B2Y*2},  //- 
    };
    
    //ボタンの表示テキスト 12文字まで
    const char bu_text_target[BUTTON_TARGET_LEN][13] = {
        "",             //dummy
        "SET",
        "SET",
        "SET",
        "   ",
        "    mm",
        "    mm",
        "",
        " O K  ",
        "CANCEL",
        "setTIME",
        //中間高さに表示
        {0x20, 0x80, 0x00},     //↑
        {0x20, 0x81, 0x00},     //↓
        " + ",
        " - ",
    };

    button_target_t but_num;
    button_target_t but_sel = CANCEL;   //初期選択ボタン
    uint16_t        but_y;
    uint8_t         i;
    uint8_t         target_id_tmp;     //ボタン操作中の値
    int8_t          offset_tmp;         //ボタン操作中の値
    int8_t          aim_tmp;            //ボタン操作中の値


    menu_clear_screen();
    sprintf(tmp_string, "-- TARGET CONFIG ---");
    LCD_Printf( 0*B2X, 0*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "TARGET #");
    LCD_Printf( 0*B2X, 2*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "offsetH");
    LCD_Printf( 0*B2X, 4*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "AimPointH");
    LCD_Printf( 0*B2X, 6*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "Brightness");
    LCD_Printf( 0*B2X, 8*B2Y, tmp_string, 2, YELLOW, 1);
    
    //ボタンの文字を表示
    for(i = 1; i < NUM_BUTTON_TARGET; i++){
        sprintf(tmp_string, "%s", bu_text_target[i]);
        but_y = button_target[i][1];
        if (button_target[i][3] == B2Y*2){
            //ボタンの文字をボタンの中間高さ位置に表示
            but_y += B1Y;
        }
        LCD_Printf(button_target[i][0], but_y, tmp_string, 2, YELLOW, 1);
    }
    //設定値を表示
    target_id_tmp = target_id;
    offset_tmp = offset_h;
    aim_tmp = aim_h;  
    sprintf(tmp_string, "%3d", target_id);
    LCD_Printf(10*B2X, 2*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%3d", offset_tmp);
    LCD_Printf(10*B2X, 4*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%3d", aim_tmp);
    LCD_Printf(10*B2X, 6*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "%3d", bright);
    LCD_Printf(12*B2X, 8*B2Y, tmp_string, 2, WHITE, 1);
    //ボタンを表示
    button_init(button_target, NUM_BUTTON_TARGET);
    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);   //最初のボタンセレクトの表示
    
    while(1){
        if (sw1_int_flag == 1){
            power_switch();         //ちょん押しでリターン、長押しでパワーオフ
            sw1_int_flag = 0;
            //write_rom_setup();
            return;                 //menuを抜ける  
        }
        
        if (RTC_int_flag == 1){
            footer_rewrite(1);      //1:setupからの呼び出し
            RTC_int_flag = 0;///////////////クリアしないとなぜかうまくいかない
        }

        if (TOUCH_INT_PORT == 0){
            //タッチあり          
            but_num = button_number(button_target, NUM_BUTTON_TARGET);
                    
            switch(but_num){
                case IDLE:
                    //idle
                    break;
                case TARGET_ID:
                case OFFSET:
                case AIMPOINT:
                case BRIGHTNESS:
                    //前回の位置を消去(枠グレイ表示)
                    button_select_disp(but_sel, 0, button_target, NUM_BUTTON_TARGET);
                    but_sel = but_num;
                    //ボタン選択表示(黄色)
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    __delay_ms(300);
                    break;
                    
                case UP:
                    //項目間の移動　上へ
                    button_select_disp(but_sel, 0, button_target, NUM_BUTTON_TARGET);
                    but_sel--;
                    if(but_sel < TARGET_ID){
                        but_sel = TARGET_ID;
                    }
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    __delay_ms(300);
                    break;
                    
                case DOWN:
                    //項目間の移動　下へ
                    button_select_disp(but_sel, 0, button_target, NUM_BUTTON_TARGET);
                    but_sel++;
                    if(but_sel > CANCEL){
                        but_sel = CANCEL;
                    }
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    __delay_ms(300);
                    break;
                    
                case PLUS:
                    switch(but_sel){
                        case TARGET_ID:
                            target_id_tmp += 1;
                            if (target_id_tmp > TARGET_MAX){
                                target_id_tmp = TARGET_MAX;
                            }
                            sprintf(tmp_string, "%3d", target_id_tmp);
                            break;
                        case OFFSET:
                            offset_tmp += 1;
                            if (offset_tmp > OFFSET_MAX){
                                offset_tmp = OFFSET_MAX;
                            }
                            sprintf(tmp_string, "%3d", offset_tmp);
                            break;
                        case AIMPOINT:
                            aim_tmp++;
                            if (aim_tmp > AIMPOINT_MAX){
                                aim_tmp = AIMPOINT_MAX;
                            }
                            sprintf(tmp_string, "%3d", aim_tmp);
                            break;
                        case BRIGHTNESS:
                            bright += 10;
                            if (bright > BRIGHTNESS_MAX){
                                bright = BRIGHTNESS_MAX;
                            }
                            sprintf(tmp_string, "%3d", bright);
                            //電子ターゲットにコマンドを送る
                            command_uart_send((uint8_t*)str_com_bright, (float)bright);
                            __delay_ms(200);
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_target[but_sel][0], button_target[but_sel][1], tmp_string, 2, WHITE, 1);
                    __delay_ms(60);
                    
                    break;
                    
                case MINUS:
                    switch(but_sel){
                        case TARGET_ID:
                            target_id_tmp -= 1;
                            if (target_id_tmp < TARGET_MIN){
                                target_id_tmp = TARGET_MIN;
                            }
                            sprintf(tmp_string, "%3d", target_id_tmp);
                            break;
                        case OFFSET:
                            offset_tmp -= 1;
                            if (offset_tmp < OFFSET_MIN){
                                offset_tmp = OFFSET_MIN;
                            }
                            sprintf(tmp_string, "%3d", offset_tmp);
                            break;
                        case AIMPOINT:
                            aim_tmp--;
                            if (aim_tmp < AIMPOINT_MIN){
                                aim_tmp = AIMPOINT_MIN;
                            }
                            sprintf(tmp_string, "%3d", aim_tmp);
                            break;
                        case BRIGHTNESS:
                            bright -= 10;
                            if (bright < BRIGHTNESS_MIN){
                                bright = BRIGHTNESS_MIN;
                            }
                            sprintf(tmp_string, "%3d", bright);                   
                            //電子ターゲットにコマンドを送る
                            command_uart_send((uint8_t*)str_com_bright, (float)bright);
                            __delay_ms(200);
                            break;    
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_target[but_sel][0], button_target[but_sel][1], tmp_string, 2, WHITE, 1);
                    __delay_ms(60);
                    break;
                    
                case TARGET_ID_SET:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    target_id = target_id_tmp;
                    //ESPにコマンドを送る
                    target_num_send();
                    target_set_up_command();
                    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE);    //ターゲットデータをクリア 
                    __delay_ms(500);
                    write_rom_setup();
                    break;
                    
                case OFFSET_SET:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    offset_h = offset_tmp;
                    //電子ターゲットにコマンドを送る
                    command_uart_send((uint8_t*)str_com_offset, (float)offset_h);
                    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE);    //ターゲットデータをクリア 
                    __delay_ms(500);
                    write_rom_setup();
                    break;
                    
                case AIM_SET:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    aim_h = aim_tmp;
                    //電子ターゲットにコマンドを送る
                    command_uart_send((uint8_t*)str_com_aimpoint, (float)aim_h);
                    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE);    //ターゲットデータをクリア
                    __delay_ms(500);
                    write_rom_setup();
                    break;
                    
                case SET_RETURN:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    //電子ターゲットにコマンドを送る
                    target_id = target_id_tmp;
                    offset_h = offset_tmp;
                    aim_h = aim_tmp;
                    
                    esp_command_uart_send((uint8_t*)str_com_target_id, (float)target_id);
                    __delay_ms(200);    //つづけざまに送ると無効になるみたい
                    //command_uart_send((uint8_t*)str_com_offset, (float)offset_h);
                    //__delay_ms(200);    //つづけざまに送ると無効になるみたい
                    //command_uart_send((uint8_t*)str_com_aimpoint, (float)aim_h);
                    //__delay_ms(200);
                    impact_plot_graph(DUMMY, DUMMY, DUMMY, REDRAW_NONE, RESET_DONE);    //ターゲットデータをクリア
                    __delay_ms(200);
                    
                    write_rom_setup();
                    return;                 //menuを抜ける
                    break;
                    
                case CANCEL:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    __delay_ms(200);
                    return;                 //menuを抜ける
                    break;
                    
                case SET_TIME:
                    button_select_disp(but_sel, 1, button_target, NUM_BUTTON_TARGET);
                    __delay_ms(200);
                    RTC8900_time_set();     //時計合わせ
                    return;                 //menuを抜ける
                    break;
                    
                default:
                    break;
            }
        
#if DEBUG_BUTTON == 1
        //button select
        sprintf(tmp_string, "bu.sel = %2d", but_sel);
        LCD_Printf(COL_BUTTON_SEL, ROW_BUTTON_SEL, tmp_string, 1, RED, 1);
#endif
        }   //if

    }   //while
}



//****** sub **********************

void    target_num_send(void){
    //ESP32へIDナンバーを送りペアリング
    esp_command_uart_send((uint8_t*)str_com_target_id, (float)target_id);
    __delay_ms(500);
    sprintf(tmp_string, "WiFi->Target#%d    ", target_id);  //18文字
    LCD_Printf(COL_WARNING, ROW_WARNING1, tmp_string, 1, PINK, 1);
    
}


uint8_t target_id_read(void){
    //接続中のターゲット#をかえす
    return target_id;
}


void    target_set_up_command(void){
    //ターゲットへコマンドを送り初期化
    command_uart_send((uint8_t*)str_com_offset, (float)offset_h);
    __delay_ms(200);
    command_uart_send((uint8_t*)str_com_aimpoint, (float)aim_h);
    __delay_ms(200);
    command_uart_send((uint8_t*)str_com_bright, (float)bright);
    //__delay_ms(200);
    //target_lcd_clear_command();
}


void    menu_clear_screen(void){
    //クリア
    LCD_SetColor(0x00, 0x00, 0x00);                     //BLACK
    LCD_DrawFillBox(MENU_X_MIN, MENU_Y_MIN, MENU_X_MAX, MENU_Y_MAX); 
}


void    set_setup(void){
    //マト距離とエアソフトガンの種類とBB弾の種類のログデータへのセット
    
    sprintf(&bullet_CSVdata[DISTANCE][0],     "%8.3f", (float)dist_mm / 1000 + dist_m);
    sprintf(&bullet_CSVdata[AIRSOFT_TYPE][0], "%s",    air_gun_text[gun_num]);  //12文字
    sprintf(&bullet_CSVdata[BB_MASS][0],      "%6.3f", (float)bbmass_g / 1000);
    sprintf(&bullet_CSVdata[BB_TYPE][0],      "%s",    bb_type_text[bb_num]);   //12文字
    sprintf(&bullet_CSVdata[F_EXTRACT][0],    "%7d",   f_extract);
    sprintf(&bullet_CSVdata[TARGET_AIM_H][0], "%5d",   aim_h);

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
    
    //target
    target_id = (uint8_t)DATAEE_ReadByte(TARGET_ID_NUM);
    if ((target_id < TARGET_MIN) || (target_id > TARGET_MAX)){
        target_id = 1;
    }
    offset_h = (int8_t)DATAEE_ReadByte(TARGET_OFFSET_Y);
    if ((offset_h < OFFSET_MIN) || (offset_h > OFFSET_MAX)){
        offset_h = -15;
    }
    aim_h = (int8_t)DATAEE_ReadByte(TARGET_AIM_Y);
    if ((aim_h < AIMPOINT_MIN) || (aim_h > AIMPOINT_MAX)){
        aim_h = 74;
    }  
    bright = (int16_t)DATAEE_ReadByte(TARGET_BRIGHT);
    bright += (int16_t)DATAEE_ReadByte(TARGET_BRIGHT+1) << 8;
    if ((bright < BRIGHTNESS_MIN) || (bright > BRIGHTNESS_MAX)){
        bright = 90;
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
    
    //target
    DATAEE_WriteByte(TARGET_ID_NUM, (uint8_t)target_id);
    DATAEE_WriteByte(TARGET_OFFSET_Y, (uint8_t)offset_h);
    DATAEE_WriteByte(TARGET_AIM_Y, (uint8_t)aim_h);
    DATAEE_WriteByte(TARGET_BRIGHT, (uint8_t)bright);
    DATAEE_WriteByte(TARGET_BRIGHT+1, (uint8_t)(bright >> 8));
        
    set_setup();
}



int8_t  target_offset_h(void){
    //ターゲットのy方向の座標表示のオフセット値
    //タマモニメニューで設定してESP-NOWでターゲットESP32へ表示オフセット値をコマンド送信。
    //ターゲットPIC32は着弾位置座標しか取り扱わないので、LANケーブル（有線）でデータが送られる時は表示オフセット情報が入ってこない
    //有線でデータを受け取った時は表示オフセット値を加える必要がある。
    //変数をグローバル化したくないので関数で返す。
    return offset_h;
}