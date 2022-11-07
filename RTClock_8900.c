/* 
 * File:   RTClock_8900.c
 * Author: IsobeR
 * Comments: PIC18F26K83 MCC
 *           PIC18F57Q43 MCC
 * 
 * リアルタイムクロック
 * I2C
 * 　RX-8900
 * 
 * Revision history: 
 * 2020.08.09   I2Cの記述が変更
 * 2020.08.14   時計合わせはMPLABXインラインデバッグを使用
 * 2021.02.22   電源断検出ビット計算間違い修正
 * 2021.09.20   RTC_timing TMR5->SMT1 uint16_t->uint32_t
 * 2021.09.25   RTC_disp()更新箇所だけ表示のモードを追加
 * 2021.12.19   RTC_disp()時の更新表示されないバグフィクス
 * 2022.10.21   時刻セットを32MZより移植
 * 2022.10.23   時刻セットをメニューにも使えるように整理
 * 
 */

#include "header.h"
#include "RTClock_8900.h"

#define     RTC_TIMING_DEBUG_no
#define     ROW_RTCLOCK     310
#define     COL_RTCLOCK     110
//BCD-decimal
#define decimaltobcd(x)     (((x / 10) << 4) + ((x - ((x / 10) * 10))))
#define bcdtodecimal(x)     ((x & 0xF0) >> 4) * 10 + (x & 0x0F)


//global
uint32_t    RTC_timing;         //センサー1オンから直後の測定値更新までの時間
bool        RTC_int_flag = 0;    //割り込みフラグ　時刻更新　1秒毎                
uint16_t    sleep_count;        //スリープ用タイマーカウント
int8_t      low_battery_count;  //低電圧検出後のカウンタ

//曜日用文字列変数
char week[4];


uint8_t RTC_initialize(void){
    uint8_t     dd[7];
    
    __delay_ms(100);
    printf("RTCC INIT...");
    
    //////送信＆受信確認をしてエラーを検出
    
    if ((I2C1_Read1ByteRegister(RX8900_ADDRESS, 0x1e) & 0b00000010) != 0){
    //if ((i2c_master_read1byte(RX8900_ADDRESS, 0x1e) & 0b00000010) != 0){
    
        //bit1 VLF=1の時は電源断があったものと思われるので　全ての初期設定を実施
        printf("RESET\n");
        //I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x08);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-1Hz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1e, 0x00);    //reset all flag
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1f, 0x61);    //reset bit7-6:CSEL-2sec, bit5:UIE-UpdateINT ON,　bit0:RESET
        
        I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
  
        __delay_ms(1000);
        RTC8900_time_set();          //日付時刻セットへ
        return 1;
        
    }else {
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        //i2c_master_write1byte(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1e, 0x00);    //reset all flag
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1f, 0x60);    //reset bit7-6:CSEL-2sec, bit5:UIE-UpdateINT ON
    }
    IOCCF2_SetInterruptHandler(RTC_interrupt);
    printf("OK!\n");
    return 0;
}


void RTC_interrupt(void){
    //時刻更新割り込み
#if RTC_TIMING_DEBUG
    RTC_timing = SMT1_GetTimerValue();  //センサー1オンから直後の測定値更新までの時間=debug
#endif
    sleep_count ++;                     //スリープまでのタイマーカウント
    low_battery_count--;                //低電圧検出してからのカウントダウンタイマー
    RTC_int_flag = 1;
}


void RTC8900_time_get(uint8_t *time_data){
    //タイム取得
    //データはBCD2桁　　　例. 21秒だったら0x21
    //time  0:sec, 1:min, 2:hour, 3:week (sun=1, mon=2, tue=4, wed=8, thu=16, fri=32, sat=64)
    //date  4:day, 5:month, 6:year(-2000)
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, time_data, 7);
}


void RTC_disp(bool full){
    //日付、時刻データを読み込み、表示
    uint8_t         dd[7];
    static uint8_t  dd_before[7] = {0,0,0,0,0,0,0};
    uint8_t         i;
    
    
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
    if (full == 1){
        //全部表示モード
        sprintf(tmp_string, "20%02x/%02x/%02x  %02x:%02x:%02x", dd[6], dd[5], dd[4], dd[2], dd[1], dd[0]);    //time_strはglobal
        LCD_Printf(COL_RTCLOCK, ROW_RTCLOCK, tmp_string, 1, WHITE, 1); 
    }else{
        //変化したところだけ書くモード
        //time
        for (i = 0; i <= 2; i++){
            if (dd_before[i] != dd[i]){
                sprintf(tmp_string, "%02x", dd[i]);
                LCD_Printf((COL_RTCLOCK + 108 - 18 * i), ROW_RTCLOCK, tmp_string, 1, WHITE, 1);
            }
        }
        //date
        for (i = 4; i <= 6; i++){
            if (dd_before[i] != dd[i]){
                sprintf(tmp_string, "%02x", dd[i]);
                LCD_Printf((COL_RTCLOCK + 120 - 18 * i), ROW_RTCLOCK, tmp_string, 1, WHITE, 1);
            }
        }
        for (i = 0; i < 7; i++){
            dd_before[i] = dd[i];
        }
    }
#if DEBUGGER_1
    //UART Tx
    printf("%s\n", tmp_string);
#endif
    //csv
    sprintf(&bullet_CSVdata[DATE][0], "20%02x/%02x/%02x", dd[6], dd[5], dd[4]);
    sprintf(&bullet_CSVdata[TIME][0], "%02x:%02x:%02x", dd[2], dd[1], dd[0]);
#if RTC_TIMING_DEBUG
    //撃った瞬間から次に更新した時までの時間(測定中に割り込みが入ったかどうかの確認だけ)=debug
    sprintf(&bullet_CSVdata[RTCC_LAG][0], "%10.3fmsec", (float)RTC_timing * TIME_CONV1 / 1000);
#endif
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(16);       //RTCC return
#endif
}


void RTC_get_date(uint8_t * dd){
    //ファイルネーム用年月日の取得
    //配列で返し
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
}


/*
void RTC_set(void){
    //時計合わせ
    uint8_t dd[7], i;
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10,dd,7);
    sprintf(tmp_string, "20%02x/%02x/%02x  %02x:%02x:%02x", dd[6], dd[5], dd[4], dd[2], dd[1], dd[0]);
    LCD_Printf(50, 100, tmp_string, 1, WHITE, 1); 
    sprintf(tmp_string, "set time to use MPLABX");
    LCD_Printf(50, 150, tmp_string, 1, RED, 1); 

    __delay_ms(1000);
    dd[0] = 0x00;//sec BCD
    dd[1] = 0x25;//min
    dd[2] = 0x22;//hour
    dd[3] = 0x06;//week bit6:sat 5:fri 4:tue 3:wed 2:tue 1:mon 0:sun
    dd[4] = 0x23;//day
    dd[5] = 0x04;//month
    dd[6] = 0x22;//year
    // ↓ ここにブレークポイントを設定して時計を見ながら合わせる
    for (i = 0; i < 7; i++){                                            //時計を合わせる時はここにブレークポイントを設定する。
        I2C1_Write1ByteRegister(RX8900_ADDRESS, (0x10 + i) , dd[i]);
    }
}
*/


void RTC8900_time_set(void) {
    //時計合わせ　タッチボタン
    
    //画面表示
#define     B2X     12      //2倍角文字　12x16pixel
#define     B2Y     16
    
#define     COL_BUTTON_SEL  0
#define     ROW_BUTTON_SEL  (8*33)
    
    //ボタン名称
    typedef enum {
        IDLE,
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        SECOND,
        PLUS,
        MINUS,
        NEXT,
        BACK,
        SET,
        CANCEL,
        NUM_BUTTON_TIME, //宣言数  =　メッセージ配列添字数     
    } button_time_t;
    
    
#define     BUTTON_TIME_LEN    13
    //時計合わせのボタン位置の設定
    const uint16_t button_time[BUTTON_TIME_LEN][4] = {
      //{ x0,  y0, 横幅, 高さ}, 
        {   240,   320,     0,   0},    //dummy
        { 1*B2X, 2*B2Y, 4*B2X, B2Y},    
        { 8*B2X, 2*B2Y, 2*B2X, B2Y},    
        {13*B2X, 2*B2Y, 2*B2X, B2Y},    
        { 3*B2X, 4*B2Y, 2*B2X, B2Y},    
        { 8*B2X, 4*B2Y, 2*B2X, B2Y},    
        {13*B2X, 4*B2Y, 2*B2X, B2Y},    
        { 3*B2X, 7*B2Y, 3*B2X, B2Y},    
        { 7*B2X, 7*B2Y, 3*B2X, B2Y},    
        {11*B2X, 7*B2Y, 3*B2X, B2Y},    
        {15*B2X, 7*B2Y, 3*B2X, B2Y},    
        { 3*B2X, 9*B2Y, 5*B2X, B2Y},    
        {11*B2X, 9*B2Y, 6*B2X, B2Y}     
    };

    
    uint8_t tx_data[8];
    uint8_t time_data[7];
    //tmp_stringはグローバルで定義されている

    uint8_t i;
    button_time_t   bnum;
    uint8_t bsel = MINUTE;  //選択中のボタン
    
    int8_t  year, month, day, wnum;
    int8_t  hour, minute, second;
    //月毎の日数 end day of mounth
    uint8_t edom[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};     //添字と月を一致させてる。0月は0
    

    RTC8900_time_get(time_data);
    year =  bcdtodecimal(time_data[6]);
    month = bcdtodecimal(time_data[5]);
    day =   bcdtodecimal(time_data[4]);
    wnum =  time_data[3];
    hour =   bcdtodecimal(time_data[2]);
    minute = bcdtodecimal(time_data[1]);
    //second = bcdtodecimal(time_data[0]);
    second = 0;
    wnum = youbi(year, month, day);

    //初期画面
    menu_clear_screen();
    sprintf(tmp_string, "*** set a clock ****");
    LCD_Printf(0*B2X, 0*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "20%02d / %02d / %02d %s", year, month, day, week);
    LCD_Printf( 1*B2X, 2*B2Y, tmp_string, 2, WHITE, 1);
#define     COL_WEEK    16*B2X
#define     ROW_WEEK     2*B2Y
    sprintf(tmp_string, "%02d : %02d : %02d", hour, minute, second);
    LCD_Printf( 3*B2X, 4*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "+   -   %c   %c", 0x7e,0x7f);
    //→ ←  ↑ ↓  0x7e~0x81　半角矢印
    LCD_Printf(4*B2X, 7*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "SET    CANCEL");
    LCD_Printf(4*B2X, 9*B2Y, tmp_string, 2, YELLOW, 1);
    
    button_init(button_time, NUM_BUTTON_TIME);
    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);   //最初の表示

    
    //main loop
    while(1){
        
        if (sw1_int_flag == 1){
            power_switch();        //ちょん押しでリターン、長押しでパワーオフ
            sw1_int_flag = 0;
            return;             //set_timeを抜ける  
        }
        
        if (TOUCH_INT_PORT == 0){
            //タッチあり
            bnum = button_number(button_time, NUM_BUTTON_TIME);
            
            switch(bnum){
                case IDLE:
                    //idle
                    break;
                case YEAR:
                case MONTH:
                case DAY:
                case HOUR:
                case MINUTE:
                case SECOND:
                    //前回の位置を消去(枠グレイ表示)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel = bnum;
                    //ボタン選択表示(黄色)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case PLUS:
                    switch(bsel){
                        case YEAR:
                            year++;
                            if (year > 50){
                                year = 20;
                            }
                            if ((year % 4) == 0){
                                //うるう年の2月の日数変更
                                edom[2] = 29;
                            }else{
                                edom[2] = 28;
                            }
                            sprintf(tmp_string, "20%02d", year);
                            break;
                        case MONTH:
                            month++;
                            if (month > 12){
                                month = 1;
                            }
                            sprintf(tmp_string, "%02d", month);
                            break;
                        case DAY:
                            day++;
                            if (day > edom[month]){
                                day = 1;
                            }
                            sprintf(tmp_string, "%02d", day);
                            break;
                        case HOUR:
                            hour++;
                            if (hour > 23){
                                hour = 0;
                            }
                            sprintf(tmp_string, "%02d", hour);
                            break;
                        case MINUTE:
                            minute++;
                            if (minute > 59){
                                minute = 0;
                            }
                            sprintf(tmp_string, "%02d", minute);
                            break;
                        case SECOND:
                            if (second == 0){
                                second = 30;
                            }else{
                                second = 0;
                            }
                            sprintf(tmp_string, "%02d", second);
                            __delay_ms(500);
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                        }
                    //ボタン内テキスト
                    LCD_Printf(button_time[bsel][0], button_time[bsel][1], tmp_string, 2, WHITE, 1);
                    //曜日の再計算
                    wnum = youbi(year, month, day);
                    sprintf(tmp_string, "%s", week);
                    LCD_Printf(COL_WEEK, ROW_WEEK, tmp_string, 2, WHITE, 1);   
                    break;
                case MINUS:
                    switch(bsel){
                        case YEAR:
                            year--;
                            if (year < 20){
                                year = 50;
                            }
                            if ((year % 4) == 0){
                                //うるう年の2月の日数変更
                                edom[2] = 29;
                            }else{
                                edom[2] = 28;
                            }
                            sprintf(tmp_string, "20%02d", year);
                            break;
                        case MONTH:
                            month--;
                            if (month < 1){
                                month = 12;
                            }
                            sprintf(tmp_string, "%02d", month);
                            break;
                        case DAY:
                            day--;
                            if (day < 1){
                                day = edom[month];
                            }
                            sprintf(tmp_string, "%02d", day);
                            break;
                        case HOUR:
                            hour--;
                            if (hour < 0){
                                hour = 23;
                            }
                            sprintf(tmp_string, "%02d", hour);
                            break;
                        case MINUTE:
                            minute--;
                            if (minute < 0 ){
                                minute = 59;
                            }
                            sprintf(tmp_string, "%02d", minute);
                            break;
                        case SECOND:
                            if (second == 0){
                                second = 30;
                            }else{
                                second = 0;
                            }
                            sprintf(tmp_string, "%02d", second);
                            __delay_ms(500);
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //ボタン内テキスト再描画
                    LCD_Printf(button_time[bsel][0], button_time[bsel][1], tmp_string, 2, WHITE, 1);
                    //曜日の再計算
                    wnum = youbi(year, month, day);
                    sprintf(tmp_string, "%s", week);
                    LCD_Printf(COL_WEEK, ROW_WEEK, tmp_string, 2, WHITE, 1);   
                    break;
                case NEXT:
                    //前回の位置を消去(枠グレイ表示)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel++;
                    if(bsel > MINUTE){
                        bsel = MINUTE;
                    }
                    //ボタン選択表示(黄色)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case BACK:
                    //前回の位置を消去(枠グレイ表示)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel--;
                    if(bsel < YEAR){
                        bsel = YEAR;
                    }
                    //ボタン選択表示(黄色)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case SET:
                    //タイムをセット
                    tx_data[0] = decimaltobcd(second + 1);  //sec BCD タイミング合わせのため+1
                    tx_data[1] = decimaltobcd(minute);      //min
                    tx_data[2] = decimaltobcd(hour);        //hour
                    tx_data[3] = 1 << wnum;                 //week bit6(0x40):sat bit5(0x20):fri bit4(0x10):tue bit3(0x8):wed bit2(0x4):tue bit1(0x2):mon bit0(0x1):sun
                    tx_data[4] = decimaltobcd(day);         //day
                    tx_data[5] = decimaltobcd(month);       //month
                    tx_data[6] = decimaltobcd(year);        //year
                    __delay_ms(500);                        //タイミング合わせ
                    for (i = 0; i < 7; i++){
                        I2C1_Write1ByteRegister(RX8900_ADDRESS, (0x10 + i) , tx_data[i]);
                    }

                    sprintf(tmp_string, "DATE&TIME SET!");
                    LCD_Printf(2*B2X, 11*B2Y, tmp_string, 2, PINK, 1);
                    __delay_ms(1500);
                    menu_clear_screen();
                    return;
                case CANCEL:
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    menu_clear_screen();
                    return;
                default:
                    break;
            
            }   //switch
            
            //button select  ///debug
            sprintf(tmp_string, "bu.sel = %2d", bsel);
            LCD_Printf(COL_BUTTON_SEL, ROW_BUTTON_SEL, tmp_string, 1, RED, 1);
            
        }   //if
            
    }  //while
    
    
}


uint8_t youbi(uint8_t y, uint8_t m, uint8_t d){
    //日付から曜日を求める　ツェラー(Zeller)の公式
    uint8_t w;
    
    if (m < 3) {
    //閏年があるため、1年の起点を3月として計算
         y--;
         m += 12;
     }
     w = (y + y / 4 - y / 100 + y / 400 + (13 * m + 8) / 5 + d) % 7;
     
     switch(w){
        case 0:
            sprintf(week, "sun");
            break;
        case 1:
            sprintf(week, "mon");
            break;
        case 2:
            sprintf(week, "tue");
            break;
        case 3:
            sprintf(week, "wed");
            break;
        case 4:
            sprintf(week, "thu");
            break;
        case 5:
            sprintf(week, "fri");
            break;
        case 6:
            sprintf(week, "sat");
    }
     
    return w;

}
