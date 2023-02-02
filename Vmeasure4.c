/*
 *  File Name: Vmeasure4.c
 *
 *  Bullet Verocity Measuring Sub
 * 
 * 2022.04.24       Ver.8.2
 * 2023.01.29       ver.9.0
 * 
 *  @ Signal Measurement Timer
 *      初速: PT1-2      (Photo Transistor)
 *      終速: PT3-4     
 *      着弾時間: PT1-4 
 *          PT1 -----CLCIN0 -- CLC1に割り当て信号反転
 *          PT2 -----CLCIN1 -- CLC2
 *          PT3 -----CLCIN4 -- CLC3
 * 
 *          PT4_LAN--CLCIN5 ---CLC4-CLC8EN=0でLAN 　信号切り替え
 *          PT4_WiFi-CLCIN2 -|          EN=1でWIFI
 * 
 *          CLC5 -PT1 or PT2
 *          CLC6 -PT3 or PT4
 * 
 *          初速:CLC5OUT -TMR1GATE -TMR1GIF割込 & TMR1タイマー値
 *          着速:CLC6OUT -TMR3GATE -TMR3GIF割込 & TMR3タイマー値
 *          着弾時間:SMT1タイムオブフライトモード
 *                  PT1とPT4のライズエッジ間の時間を測定
 *                  CLC1OUT -SMT1WIN 
 *                  CLC4OUT -SMT1SIG
 *                  rise edge to rise edge
 *                  -SMT1PRAIF割込 & SMT1CPRタイマー値
 * 
 *                  SMT1,TMR1,TMR3とも
 *                  FOSC/4 = 16MHz Xtal 分解能0.0625usec 
 *                   精度+-30ppm = +-0.003%
 *                  オーバーフロー
 *                   SMT1:24bit     --- 1.048576sec　　//@30mくらいまでは対応??
 *                   TMR1,TMR3:16bit--- 0.004096sec
 *
 * 
 * 
 * ver.1.00 2020.08.09  BulletVerocityHost12より移植しサブルーチン化
 * 2021.01.02   ヘッダ内での定義（間違ったやり方）を移動
 * 2021.01.14   センサ3-4距離修正76.2->75.82mm
 * 2021.01.16   センサ3-4距離修正75.82->75.85mm
 * 2021.01.17   センサ3-4距離修正75.85->75.83mm
 * 2021.01.25   センサ割込内からタイマ値readサブ呼出廃止。レジスタを直接処理。
 * 2021.05.11   ビデオシンクLED (TMR3)-> LAN2pin:VE_CONTROL1 
 * 
 * V7
 * 2021.08.22   時間測定方法をtimer1からSMTに変更
 * 2021.09.20   SMT動作OKになったのでセンサ距離調整LEN12=61.8mm,LEN34=76.6mm
 * 2021.09.21   TMR1,3,5廃止。ビデオシンクTMR3->PWM3_16BIT 6Hz,duty7%(60fps)  240fpsのときは24Hz duty7%
 * 2021.09.22   モーション表示関係は全てmotion.cへ移動し、Vmeasureと完全に分けた
 * 2021.09.25   モーションデータSD書き出しをSDcard.cへ移動
 * 2021.09.28   ショットインジケータをフォントからボックス描画へ
 * 2021.10.02   センサ3-4距離修正76.6->76.2mm
 * 2021.10.02   センサ3-4距離修正76.2->76.4mm
 * 2021.10.13   センサ入力に正規の順序以外の入力があった時はタイムアウト
 * 
 * V8
 * 2022.04.24   V7 Vmeasure2.cより移植
 *              終速計から電子ターゲットに変更の対応　PT3なし　通信で着弾位置とPT4補正時間を取得
 * V8_2
 * 2022.04.25   初速の測定をSMT1のタイムオフフライトモードのライズエッジ間で測る。
 *              着弾時間はTMR1GATEとCAP1で測る。(電子ターゲットモード)
 * 2022.04.28   着弾時間をSMT1で。初速をTMR1とCCP1で着速をTMR3とCCP2で。PT1-4をCLC1-4へいれて反転と入力時割込。
 * 2022.04.30   CCPモジュールの入力にCLCを使うと動作しないみたい????
 *              Timer Gate Single Pulse & Toggle Combined Mode. PT1-2:TMR1+CLC5,PT3-4:TMR3+CLC6
 *              速度1000m/sec以上の時OVER表示
 * 2022.05.03   無限待ち対策TMR0タイムアウト追加
 * 2022.08.20   v0チャート　データ数40（バッファ数）以上のとき表示スクロールしないのバグフィクス
 * 2022.10.15   v0初速センサをXCORTECHと高速ストロボの選択式にSW3で切り替え
 * 2022.10.19   ストロボ撮影時のログ項目追加
 * 
 * 
 * 
 * 
*/

#include "header.h"
#include "Vmeasure4.h"


//constant value
#define     XCORTECH            0       //初速計
#define     STROBE              1       //高速ストロボセンサ

#define     LEN12_mm_XCORTECH   62.0    //センサ1～2間距離[mm]        機械寸法 62.0mm XCORTECH初速計
#define     LEN12_mm_STROBE     12.0    //高速ストロボの初速センサ[mm]  機械寸法 12.0mm
#define     LEN34_mm            77.1    //センサ3～4間距離[mm]        機械寸法 76.2mmセンサ中心間距離、溝寸法77.0mm

//display  ドット位置 x:COL[列] y:ROW[行]
//V8からタイトルバー無しの寸法
#define     COL_SHOT        12      //2倍角フォント12x16
#define     ROW_SHOT        2 
#define     COL_INDI        3       //標準フォント相当6x8
#define     ROW_INDI        6
#define     COL_MODE        170
#define     ROW_MODE        4
#define     COL_V0SENS12    232
#define     ROW_V0SENS12    4
//
#define     COL_V0          0       //2倍角フォント12x16
#define     ROW_V0          20
#define     COL_V0_CHART    0
#define     ROW_V0_CHART    50
#define     COL_ERROR       156
#define     ROW_ERROR       20
#define     COL_VE          156     //標準フォント6x8
#define     ROW_VE          30
#define     COL_TIME        132
#define     ROW_TIME        40
#define     COL_TARGET      2
#define     ROW_TARGET      40

//
#define     NUM_SHOTS       20
//センサーステータス
#define     SENSOR1_PORT    PORTAbits.RA0
#define     SENSOR2_PORT    PORTAbits.RA1
#define     SENSOR3_PORT    PORTAbits.RA2
#define     SENSOR4_PORT    PORTAbits.RA3



//global
device_connect_t    target_mode;    ////////////local化したい
measure_status_t    measure_status; ////////////local化したい
shot_data_t         shot_data[NUM_SHOTS];
uint8_t             sensor_type;


//local
const shot_data_t  clear_shot_data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 999.9, 999.9, 0, VMEASURE_READY};
//クリア値: 着弾座標(x,y)=(0,0)は、ど真ん中の意味なので、クリア値はマトの外の値(999.9,999.9)とする。
uint16_t    shot = 0;               //ショット数
uint8_t     shot_buf_pointer = 0;   //ショットメモリバッファ用ポインタ
uint8_t     len12_mm;               //初速センサ1-2の距離[mm]
char        v0device[15];           //初速装置の種類
    
    
//割込&動作ステータス ー 動作シーケンスのチェック用
typedef struct {
    bool    sensor1on;
    bool    sensor2on;
    bool    sensor3on;
    bool    sensor4on;
    bool    v0timer;
    bool    vetimer;
    bool    impacttimer;
    bool    tmr0ovf;
    bool    tmr1ovf;
    bool    tmr3ovf;
    bool    smt1ovf;
    bool    motion;
    bool    uart;
} interrupt_status_t;

interrupt_status_t  int_status;
const interrupt_status_t   clear_status = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//エラーメッセージ
typedef enum {
    SENSOR_OK,
    SENSOR2_ERROR,
    SENSOR3_ERROR,
    SENSOR4_ERROR,
    SENSOR34_ERROR,
    IMPACT_ERROR,
    MOTION_ERROR,
    RECEIVE_ERROR,
    TARGET_CALC_ERROR,        
    TARGET_ERROR,        
    ERROR_CLEAR,
    NUM_SENSOR_ERROR,  //宣言数  =　メッセージ配列添字数
} vmeasure_error_t;

#define     ERR_LEN     (14 + 1)    //エンドマーク分+1
const char vmeasure_err_mes[NUM_SENSOR_ERROR][ERR_LEN] = { 
    "              ",
    " Sens2 Timeout",
    " Sens3 Timeout",
    " Sens4 Timeout",
    "Sens34 Timeout",
    "Impact Timeout",
    "  Motion Error",
    "    Rx Timeout",
    "Target CalcErr",
    " Target Rx Err",
    " ?ERROR CLEAR?",
};


//
void vmeasure_initialize(void){
    //速度測定初期設定
    //SMT1 timer値
#define     TIME_IMPACT     0x00f423ff      //16000000 = 1sec 着弾時間1-4間
    
    uint8_t i;
    
    //overflow
    TMR0_SetInterruptHandler(timer0_timeout);
    TMR1_SetInterruptHandler(timer1_timeout);
    TMR3_SetInterruptHandler(timer3_timeout);
    //timer gate
    TMR1_SetGateInterruptHandler(v0_timer_gate);
    TMR3_SetGateInterruptHandler(ve_timer_gate);
    //CLC1-4割込はMCCのコード内に記述
    //SMT1割込はMCCのコード内に記述
    
    //全ショットデータバッファをクリア
    for (i = 0; i < NUM_SHOTS; i++){
        shot_data[i] = clear_shot_data;
    }
    
    SMT1_SetPeriod((uint32_t)TIME_IMPACT);  //SMT timer overflow値
}


void set_v0sensor(bool change){
    //センサ1-2の距離をセット
    //change:0 = print only　右上
    //change:1 = change & print
    char sen12_str[] = "X";
    
    if (change == 1){
        //センサ設定をトグル切り替え
        switch(sensor_type){
            //センサ3種以上にも対応
            case XCORTECH:
                sensor_type = STROBE;
                sprintf(tmp_string, "v0:STROBE 12mm    ");
                break;
            case STROBE:
            default:
                sensor_type = XCORTECH;
                sprintf(tmp_string, "v0:XCORTECH 62mm  ");
                break;
        }        
        DATAEE_WriteByte(V0SENS12_ADDRESS, sensor_type);
        LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1); 
    }
    
    switch(sensor_type){
        case STROBE:
            len12_mm = LEN12_mm_STROBE;
            sprintf(sen12_str, "S");
            sprintf(v0device, "STROBE  ");      //ログ出力用
            break;
        default:
            sensor_type = XCORTECH;     //書き込み後の最初はEEP ROMが0xFFになっている模様
            DATAEE_WriteByte(V0SENS12_ADDRESS, sensor_type);
        case XCORTECH:
            len12_mm = LEN12_mm_XCORTECH;
            sprintf(sen12_str, "X");
            sprintf(v0device, "XCORTECH");
            break;
    }
    LCD_Printf(COL_V0SENS12, ROW_V0SENS12 , sen12_str, 1, YELLOW, 1); 
    while(SW3_PORT == SW_ON){
        //キーが離されるのを待つ
    }
    
}


void vmeasure_ready(void){
    //測定リセット(測定前クリア)
    
    //Timer stop
    TMR0_StopTimer();
    TMR1_StopTimer();
    TMR3_StopTimer();    
    SMT1_DataAcquisitionDisable();
    PWM1_16BIT_Disable();           //VideoSyncLED off
    //割込クリア
    PIR3bits.CCP1IF = 0;
    PIR8bits.CCP2IF = 0;
    PIR0bits.CLC1IF = 0;
    PIR6bits.CLC2IF = 0;
    PIR7bits.CLC3IF = 0;
    PIR9bits.CLC4IF = 0;
    PIR3bits.TMR0IF = 0;
    PIR3bits.TMR1IF = 0;
    PIR3bits.TMR1GIF = 0;
    PIR5bits.TMR3IF = 0;
    PIR5bits.TMR3GIF = 0;
    PIR1bits.SMT1PRAIF = 0;
    PIR1bits.SMT1IF = 0;
    //
    print_indicator(INDI_OFF);      //測定インジケータオフ
    //clear
    sleep_count_reset();
    shot_data[shot_buf_pointer] = clear_shot_data;  //次のデータメモリをクリア データメモリはリングバッファ
    int_status = clear_status;                      //シーケンス用割込フラグクリア
    rx_buffer_clear_rs485();                        //遅延タイマーあり（マトからのデータ読み捨て）///////////
    rx_buffer_clear_esp32();                        //遅延タイマーあり（マトからのデータ読み捨て）///////////
    motion_clear();                                 //最後にクリア(motion_gate=1の関係で)

#ifdef  TIMING_LOG                  //debug
    time_log = 0;
#endif
    
    //Timer set
    TMR0_Reload();                  //TMR0 Reload
    TMR1_Reload();
    TMR1_StartTimer();              //TMR1 Enable
    TMR3_Reload();
    TMR3_StartTimer();              //TMR3 Enable
    //Timer ready
    SMT1_DataAcquisitionEnable();           //SMT1 GO (SMT1->0　ready)
    TMR1_StartSinglePulseAcquisition();     //TMR1 Gate GO (Ready))
}


uint8_t vmeasure(void){
    //玉速度測定
    static vmeasure_error_t  err = SENSOR_OK;   //staticしないと値が保管されていない
    uint8_t answ;
    uint8_t i;
    float   rx_data_f[3];   //電子ターゲット着弾データ受取用
    uint16_t ctc_num;       //戻り値
    uint8_t c2 = 0;
    uint8_t tmp2;
    
    motion_data_read();     //モーションデータの読み出し
    
    switch(shot_data[shot_buf_pointer].status){
        case VMEASURE_READY:
            //測定開始待ち
            angle_level_disp();            //傾斜表示
            //
            if ((int_status.sensor1on == 1) &&
               ((V0_MODE        == target_mode) ||
                (V0_MOTION_MODE == target_mode) ||
                (V0_VE_MODE     == target_mode) ||
                (V0_TARGET_MODE == target_mode))){
                
                TMR0_StartTimer();                      //タイムアウト検出用
                TMR3_StartSinglePulseAcquisition();     //TMR3 Gate GO (Ready)終速用
                LED_LEFT_SetHigh();                     //LED両点灯 
                LED_RIGHT_SetHigh();
                shot_data[shot_buf_pointer].status = SENSOR1_ON_START;
            }
            if ((int_status.sensor4on == 1) &&
                (TARGET_ONLY_MODE == target_mode)){
                
                TMR0_StartTimer();                                  //タイムアウト検出用
                shot_data[shot_buf_pointer].status = TARGET4_ON_START;
            }
            break;
            
    //Sensor1-2////////////////////////
        case SENSOR1_ON_START:
            //測定スタート
            print_indicator(INDI_ON);
            sleep_count_reset();
            shot_data[shot_buf_pointer].shot = shot;
            //TMR1 Gate start (hardware CLC5)
            //SMT1 time of Flight start (hardware CLC1)
            shot_data[shot_buf_pointer].status = SMT1_START;
            break;
            
        case SMT1_START:    //検出はしていない
            //printf("\nSen1ON_START,");
            shot_data[shot_buf_pointer].status = SHOT_DISP;
            break;
            
        case SHOT_DISP:
            //ショット番号表示
            print_shot(WHITE);
            shot_data[shot_buf_pointer].status = TIMER1_START;
            break;
            
        case TIMER1_START:   //検出はしていない
            if (int_status.sensor2on == 1){
                //printf("Sen2_ON\n");
                shot_data[shot_buf_pointer].status = SENSOR2_ON;
            }
        case SENSOR2_ON:
            if (int_status.v0timer == 1){
                shot_data[shot_buf_pointer].status = V0_TIMER_12_GET;
            }
            if (int_status.tmr1ovf == 1){
                printf("Tmr1_OVF\n");
                shot_data[shot_buf_pointer].status = V0_TIMEOUT;
            }
            if (int_status.smt1ovf == 1){
                printf("Smt1_OVF\n");
                shot_data[shot_buf_pointer].status = V0_TIMEOUT;
            }
            if (int_status.tmr0ovf == 1){
                //予備のタイムアウト検出
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = V0_TIMEOUT;
            }
            break;
            
        case V0_TIMER_12_GET:
            //ゲートタイマストップ値を取得
            shot_data[shot_buf_pointer].v0_timer = TMR1_ReadTimer();
            if (shot_data[shot_buf_pointer].v0_timer != 0){
                shot_data[shot_buf_pointer].t0_sec = shot_data[shot_buf_pointer].v0_timer * (float)TIME_CONV_TMR1;
                shot_data[shot_buf_pointer].v0_mps = (float)(len12_mm) / 1000 / shot_data[shot_buf_pointer].t0_sec;  
#if DEBUG_STROBE_V0     //ストロボの測定値と合わない??????デバッグ必要  
                printf("V0_Timer: 0x%04lx  ", shot_data[shot_buf_pointer].v0_timer);
                printf("t0:%8.3fusec  ", shot_data[shot_buf_pointer].t0_sec * 1000000);
                printf("v0:%8.2fm/sec\n", shot_data[shot_buf_pointer].v0_mps);
#endif
            }
            //初速表示
            print_v0(WHITE);

            switch(target_mode){
                case V0_MODE:
                case V0_MOTION_MODE:    
                    //ログデータ代入
                    print_ve(BLACK);
                    print_impact_time(BLACK);
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
                case V0_VE_MODE:
                    shot_data[shot_buf_pointer].status = WAIT_FLIGHT23;
                    break;
                case V0_TARGET_MODE:
                    //2行目消去とデータ代入
                    print_ve(BLACK);
                    shot_data[shot_buf_pointer].status = WAIT_IMPACT_TIMER;
                    break;
                default:
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
            }
            break;
            

    //Sensor3-4////////////////////////////
        case WAIT_FLIGHT23:
            if (int_status.sensor3on == 1){
                //printf("Sen3_ON,");
                shot_data[shot_buf_pointer].status = SENSOR3_ON;
            }
        case SENSOR3_ON:            
        case TIMER3_START:      //検出はしていない
            if (int_status.sensor4on == 1){
                shot_data[shot_buf_pointer].status = SENSOR4_ON;
                //printf("Sen4_ON,\n");
            }
        case SENSOR4_ON:
            if (int_status.vetimer == 1){
                shot_data[shot_buf_pointer].status = VE_TIMER_34_GET;
            }
            if (int_status.tmr3ovf == 1){
                printf("Tmr3_OVF\n");
                shot_data[shot_buf_pointer].status = VE_TIMEOUT;
            }
            if (int_status.smt1ovf == 1){
                printf("Smt1_OVF\n");
                shot_data[shot_buf_pointer].status = VE_TIMEOUT;
            }
            if (int_status.tmr0ovf == 1){
                //予備のタイムアウト検出
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = VE_TIMEOUT;
            }
            break;
        
         case VE_TIMER_34_GET:
            //ゲートタイマストップ値を取得
            shot_data[shot_buf_pointer].ve_timer = TMR3_ReadTimer();
            //printf("Ve_Timer:%4lxh\n", shot_data[shot_buf_pointer].ve_timer);
            if (shot_data[shot_buf_pointer].ve_timer != 0){
                shot_data[shot_buf_pointer].te_sec = shot_data[shot_buf_pointer].ve_timer * (float)TIME_CONV_TMR3;
                shot_data[shot_buf_pointer].ve_mps = (float)(LEN34_mm) / 1000 / shot_data[shot_buf_pointer].te_sec;
            }
            //着速表示
            print_ve(WHITE);
            shot_data[shot_buf_pointer].status = WAIT_IMPACT_TIMER;
            break;
                

    //Impact///////////////////////////
        case WAIT_IMPACT_TIMER:
            //着弾待ち    
            if (int_status.impacttimer == 1){
                shot_data[shot_buf_pointer].status = TIME_IMPACT_GET;
            }
            if (int_status.smt1ovf == 1){
                printf("Smt1_OVF\n");
                shot_data[shot_buf_pointer].status = IMPACT_TIMEOUT;
            }
            if (int_status.tmr0ovf == 1){
                //予備のタイムアウト検出
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = IMPACT_TIMEOUT;
            }            
            break;
            
        case TIME_IMPACT_GET:
            //Time-of-Flight Measurement Modeにてセンサ14ライズエッジ間の時間を取得
            shot_data[shot_buf_pointer].impact_timer = SMT1CPR;
            shot_data[shot_buf_pointer].t_imp_msec = shot_data[shot_buf_pointer].impact_timer * (float)TIME_CONV_SMT1 * 1000;
    
            //printf("Imp_Timer:%4lxh\n", shot_data[shot_buf_pointer].impact_timer);
            //着弾時間表示
            print_impact_time(WHITE);           //電子ターゲットの時補正必要
            shot_data[shot_buf_pointer].status = MEASURE_DONE;
            break;
        
    
    //Target ONLY mode///////////////////////// 
        case TARGET4_ON_START:
            //初速計無しターゲットオンリーモード
            print_indicator(INDI_ON);
            sleep_count_reset();
            shot_data[shot_buf_pointer].shot = shot;
            TMR0_StartTimer();                                  //タイムアウト検出用
            print_shot(WHITE);
            //printf("\nTaget4_ON_START,\n");
            //2~4行目消去&代入
            print_v0(BLACK);
            print_ve(BLACK);
            print_impact_time(BLACK);
            shot_data[shot_buf_pointer].status = MOTION_DONE;
            break;
            
    //timeout
        case V0_TIMEOUT:
            //センサ2タイムアウト
            print_shot(RED);
            print_v0(RED);
            err = SENSOR2_ERROR;
            printf("s#%d Sen2 TIMEOUT\n", shot + 1);
            
            switch(target_mode){
                case V0_MODE:
                    //2～3行目消去
                    print_ve(BLACK);
                    print_impact_time(BLACK);
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
                case V0_VE_MODE:
                    shot_data[shot_buf_pointer].status = WAIT_FLIGHT23;
                    break;
                case V0_TARGET_MODE:
                    //2行目消去
                    print_ve(BLACK);
                    shot_data[shot_buf_pointer].status = WAIT_IMPACT_TIMER;
                    break;
                default:
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
            }
            break;
            
        case VE_TIMEOUT:
            //センサ4タイムアウト
            print_shot(RED);
            print_ve(RED);
            print_impact_time(BLACK);
            if ((int_status.sensor3on == 0) && (int_status.sensor4on == 0)){
                err = SENSOR34_ERROR;
                printf("s#%d Sen34 TIMEOUT\n", shot + 1);
            }else if (int_status.sensor3on == 0){
                err = SENSOR3_ERROR;
                printf("s#%d Sen3 TIMEOUT\n", shot + 1);
            }else {
                err = SENSOR4_ERROR;
                printf("s#%d Sen4 TIMEOUT\n", shot + 1);
            }
            shot_data[shot_buf_pointer].status = MEASURE_DONE;
            break;
    
        case IMPACT_TIMEOUT:
            print_shot(RED);
            print_v0(WHITE);
            print_ve(WHITE);    //V0_VE＿MODE以外では無効になるようにサブルーチン内にて処理
            print_impact_time(RED);
            err = IMPACT_ERROR;
            printf("s#%d Imp TIMEOUT\n", shot + 1);
            shot_data[shot_buf_pointer].status = MEASURE_DONE;
            break;
            
    //**************************//////////////////
        case MEASURE_DONE:
            //弾速測定完了
#ifdef  TIMING_LOG                  //debug
            DEBUG_timing_log(8);    //Stage 8            
#endif
            shot_data[shot_buf_pointer].status = WAIT_MOTION;
            break;
            
        case WAIT_MOTION:
            //撃った後のモーション測定完了まで待つ
            if ((motion_gate == 0) && (motion_message_remain == 0)){
                //モーションメッセージ読込が完了
                int_status.motion = 1;
                shot_data[shot_buf_pointer].status = MOTION_DONE;
                break;                                                  
            }
            if (int_status.tmr0ovf == 1){
                //予備のタイムアウト検出
                printf("s#%d Motion TIMEOUT\n", shot + 1);
                err = MOTION_ERROR;
                shot_data[shot_buf_pointer].status = MOTION_DONE;
            }            
            break;
            
        case MOTION_DONE:
            //全測定終了
            //Timer stop
            TMR1_StopTimer();
            TMR3_StopTimer();
            SMT1_DataAcquisitionDisable();
            PWM1_16BIT_Disable();           //VideoSyncLED off
            
#ifdef  TIMING_LOG              //debug
            DEBUG_timing_log(21);       //complete
#endif            
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //電子ターゲットの時
                shot_data[shot_buf_pointer].status = RECEIVE_DATA;
                break;
            }
            //電子ターゲットではない時
            if (V0_MODE == target_mode){
                //初速集計モードの時
                shot_data[shot_buf_pointer].status = V0_CHART;
                break;
            }
            //モーショングラフ表示へ
            shot_data[shot_buf_pointer].status = MOTION_GRAPH;
            break;
        
        //TARGET_GRAPH    
        case RECEIVE_DATA:
            //電子ターゲットからのデータ受信処理
            if (V0_TARGET_MODE == target_mode){////////////////////////////////////RS485/ESP32WiFi////////////////////////
                answ = data_uart_receive_rs485(rx_data_f);    //ノンブロッキング処理
            }else{
                answ = data_uart_receive_esp32(rx_data_f);    //ノンブロッキング処理
            }
            if (RX_OK == answ){
                //データが正常に受信された時
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = rx_data_f[0];
                shot_data[shot_buf_pointer].impact_y = rx_data_f[1];
                shot_data[shot_buf_pointer].impact_offset_usec = rx_data_f[2]; 
                shot_data[shot_buf_pointer].status = DATA_RECIEVED;
                break;
            }
            if (CALC_ERROR == answ){
                //データが計算できなかったとき
                err = TARGET_CALC_ERROR;
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                __delay_ms(300);                                //targetからのデータに重ならないように
                printf("s#%d target calc err\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
                break;
            }
            if (RX_ERROR == answ){
                //ターゲットデータがエラーだった時
                err = TARGET_ERROR;
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                __delay_ms(300);                                //targetからのデータに重ならないように
                printf("s#%d tamamoni rx err\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
                break;
            }
            
            //answ = RX_READINGの時　データ受信中
            
            if (int_status.tmr0ovf == 1){
                //タイムアウト検出
                err = RECEIVE_ERROR;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                printf("s#%d tamamoni rx tout\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
 
                while (UART2_is_rx_ready()){
                    //捨て読み
                    tmp2 = UART2_Read();
                    c2++;
                    if (c2 > 64){
                        //無限ループ対策
                        break;
                    }
                }
            }
            break;

        case DATA_RECIEVED:
            if(V0_TARGET_MODE == target_mode){
                //補正計算してimpact time再表示
                shot_data[shot_buf_pointer].t_imp_msec += shot_data[shot_buf_pointer].impact_offset_usec / 1000;    //オフセット値はマイナス値でくる
                print_impact_time(WHITE);
            }        
            shot_data[shot_buf_pointer].status = GRAPH_DRAW;
            break;
            
        case GRAPH_DRAW:
            //着弾位置表示
#define ONE_POINT_DRAW  0
#define RESET_NONE      0
            
            shot_data[shot_buf_pointer].ctc_max = 
            impact_plot_graph(shot, shot_data[shot_buf_pointer].impact_x, shot_data[shot_buf_pointer].impact_y, ONE_POINT_DRAW, RESET_NONE, &ctc_num);


            
            
/////////////////////////////
            
            //画面外のことあり///////////////
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
           
        //v0 MODE   
        case V0_CHART:
            //初速集計チャート&平均計算
            print_v0_chart(0);  //0:通常表示
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
            
        //MOTION_GRAPH
        case MOTION_GRAPH:
            //モーショングラフ表示
            motion_graph_initialize();      //グラフ初期化

            //モーションの計算//////////////////////////////////このへんSDcard.cから分けたい//////////
            //モーションのグラフ
            
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
            
            
        //************************////////////////////////////////////////    
        case TARGET_DISP:
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //電子ターゲットの時
                print_target_xy(AQUA);  //着弾座標
            }else {
                print_target_xy(BLACK); //消去
            }
            shot_data[shot_buf_pointer].status = CTOC_PRINT;
            break;
            
        case CTOC_PRINT:
            //最大着弾センタtoセンタを表示
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //電子ターゲットの時
                print_target_ctc(shot_data[shot_buf_pointer].ctc_max, ctc_num, ctc_color);       ///////ctc_colorはtarget_graph.c内で決定される　localにしたい
////////////////////////////

            }else {
                //ターゲット表示では無い時
                print_target_ctc(0, 0, BLACK); //ログデータ代入のみ。画面消去しない。　(画面消去したい時はctcをゼロ以外にして呼ぶ)
            }
            shot_data[shot_buf_pointer].status = ERROR_DISP;
            break;
            
        case ERROR_DISP:
            //エラーの有無に関わらず処理
            print_error(err);
            if (SENSOR_OK == err){
#if DEBUG
                printf("shot#%d OK!\n", shot + 1);
#endif
            }
            shot_data[shot_buf_pointer].status = ANGLE_LEVEL;
            break;
            
        case ANGLE_LEVEL:
            //撃ち出し前静止時(ringbuf_countの位置)の傾斜角度データ(shot_log用)を計算し代入するため =フル表示
            if (TARGET_ONLY_MODE == target_mode){
                //初速計なしのとき、モーションのタイミングが不明
                sprintf( &bullet_CSVdata[TILT]     [0], "         ");
                sprintf( &bullet_CSVdata[ELEVATION][0], "         ");
            }else {
                angle_level_tilt_print(ringbuf_count, 1);
            }
            shot_data[shot_buf_pointer].status = SD_SAVE_SHOT;
            break;
            
        case SD_SAVE_SHOT:
            //ショットログをSDカードへ書き込み
            answ = shot_log_sd_write();
            if (answ != 0){
                //SD書き込みエラー
                //なんか目立つ方法ないか?????????????????????????????????
            }
            shot_data[shot_buf_pointer].status = SD_SAVE_MOTION;
            break;
            
        case SD_SAVE_MOTION:
            //モーションログをSDカードへ書き込み
#ifdef  MOTION_DISP_OFF_V8
            if (TARGET_ONLY_MODE != target_mode){///////////////////////////////
                motion_save();/////////////////////////////////
            }
#endif
            shot_data[shot_buf_pointer].status = COMPLETE;
            break;
            
        case COMPLETE:
            //次のショットのための終了処理
#ifdef  TIMING_LOG                  //debug
    DEBUG_time_stamp_save();        //デバグ用タイムスタンプをSDカードへ書き込み
#endif
            //測定&表示処理完了
            shot++;
            shot_buf_pointer++;                         //ショット数+1                 
            if (shot_buf_pointer >= NUM_SHOTS){
                shot_buf_pointer = 0;                   //0はクリア用のデータを保管してある
            }
            LED_RIGHT_SetLow();
            LED_LEFT_SetLow();
            err = SENSOR_OK;
            vmeasure_ready();               //測定リセット
            break;
            
        //*** FAULT ********************    
        default:
            //不明なエラー
            print_shot(MAGENTA);
            err = ERROR_CLEAR;
            print_error(err);
            printf("??unexpected process??\n");
            //LED FLASH
            for (i = 0; i < 10; i++){
                LED_RIGHT_SetHigh();
                LED_LEFT_SetHigh();
                __delay_ms(20);   
                LED_RIGHT_SetLow();
                LED_LEFT_SetLow();
                __delay_ms(80);   
            }
            //ショット数は進めない
            vmeasure_ready();               //測定リセット
            break;
            
    }
    return shot_data[shot_buf_pointer].status;
}


//サブルーチン

void sensor_connect_check(void){
    //ターゲット接続チェックと測定モードの表示
    static uint8_t  temp = 0xff;
    
    target_mode = (uint8_t)(SENSOR4_PORT << 3) | (uint8_t)(SENSOR3_PORT << 2)
                | (uint8_t)(SENSOR2_PORT << 1) | (uint8_t)SENSOR1_PORT;
    
    if (target_mode != temp){
        print_target_mode(INDIGO);
        temp = target_mode;
    }
}
    
    
void print_indicator(indicator_status_t stat){
    //測定中インジケータ表示
    if (stat == INDI_ON){
        //color = MAGENTA;
        LCD_SetColor(0xff, 0x00, 0xff);
    }else{
        //color = CYAN;
        LCD_SetColor(0x00, 0xff, 0xff);
    }
    LCD_DrawFillBox(COL_INDI, ROW_INDI, (COL_INDI + 5), (ROW_INDI + 5));

#ifdef  TIMING_LOG              //debug
    DEBUG_timing_log(15);       //Shot indi return
#endif
}


void print_shot(uint8_t color){
    //ショット数の表示と代入
    sprintf(tmp_string, "%-3d", shot + 1);     //左詰め
    LCD_Printf(COL_SHOT + 5 * 12, ROW_SHOT, tmp_string, 2, color, 1);
    sprintf(&bullet_CSVdata[SHOT][0], "%5d", shot + 1);
}


void print_v0(uint8_t color){
    //先側センサー　初速 m/sec　表示と代入
    
    sprintf(&bullet_CSVdata[V0_DEVICE][0], "%s", v0device);
    sprintf(&bullet_CSVdata[V0_TIME][0], "      ");    //バッファをクリア
    sprintf(&bullet_CSVdata[V0][0], "         ");
    
    
    if (color == BLACK){
        sprintf(tmp_string, "  --- ");  //v0表示を消す = エラーの時(センサ2タイムアウト等)
        
    }else if(shot_data[shot_buf_pointer].v0_timer == 0){
        sprintf(tmp_string, " xxxx ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].v0_mps > 999){
        sprintf(tmp_string, " OVER ");
        color = RED;
    }else {
        //正常値の時
        sprintf(&bullet_CSVdata[V0_TIME][0], "%6.2f", shot_data[shot_buf_pointer].t0_sec * 1000000);    //usec
        
        sprintf(tmp_string, "%6.2f", shot_data[shot_buf_pointer].v0_mps);
        sprintf(&bullet_CSVdata[V0][0], "%9.4f", shot_data[shot_buf_pointer].v0_mps);
        
    }
    LCD_Printf(COL_V0 + 3 * 12, ROW_V0, tmp_string, 2, color, 1);
}


void print_v0_chart(bool reset){
    //初速モード m/sec　の時、一覧表示と統計計算
    //reset=0:通常, 1:データをクリアしてリセット
    typedef struct {
        uint16_t    shot;                   //ショット#
        float       v0_mps;                 //初速[m/sec]
    } v0_data_t;
    
#define     BUF_NUM     40      //初速記憶数    40
#define     ROW_NUM     20      //初速表示行数   20

    static v0_data_t    buf[BUF_NUM];
    static uint8_t      p = 0;          //ポインタ(リングバッファ))
    static float        sum_v0 = 0;     //平均のサム
    static uint16_t     n = 0;          //平均計算のための有効データ数
    uint8_t             i;
    int8_t              i_min, i_max;
    float               v0, joule;
    
    if (reset == 1){
        //データリセット
        p = 0;
        sum_v0 = 0;
        n = 0;
        return;
    }
    
    //バッファへ代入
    buf[p].shot = shot_data[shot_buf_pointer].shot;
    buf[p].v0_mps = shot_data[shot_buf_pointer].v0_mps;
    if ((buf[p].v0_mps != 0) && (buf[p].v0_mps < 999)){
        //正常値の時は平均計算に加える
        sum_v0 += buf[p].v0_mps;
        n++;
    }
    
    
    //一覧表示
    i_max = ROW_NUM;
    if ((p < ROW_NUM - 1) && (n <= (p + 1))){
        //データ数が少ない時
        i_max = (int8_t)p + 1;
    }
    
    //BB質量
    sprintf(tmp_string, "BB %5.3fg", (float)bbmass_g / 1000);
    LCD_Printf(180, ROW_V0_CHART + 16, tmp_string, 1, WHITE, 1);
    //平均
    if (n == 0){
        //データ数0の時
        sprintf(tmp_string, "ave  ----m/s(n=0)");
    }else {
        sprintf(tmp_string, "ave%6.2fm/s(n=%d)", (sum_v0 / n), n);
    }
    LCD_Printf(COL_V0_CHART, ROW_V0_CHART, tmp_string, 2, CYAN, 1);
    //チャート
    for (i = 0; i < i_max; i++){
        //0~19行を表示
        if ((p < ROW_NUM - 1) && (n <= (p + 1))){
            //データ数が少ない時
            i_min = (int8_t)i;
        }else{
            i_min = (p + 1) - ROW_NUM + (int8_t)i;
            if (i_min < 0){
                i_min += BUF_NUM; 
                if (i_min >= BUF_NUM){    //添字が超える
                    i_min--;              //ので1マイナスでなぜかうごいてはいる///////////////////
                }
            }
        }
        
        if(buf[i_min].v0_mps == 0){
            sprintf(tmp_string, "#%03d   xxxx              ", buf[i_min].shot + 1);
        }else if (buf[i_min].v0_mps > 999){
            sprintf(tmp_string, "#%03d   OVER              ", buf[i_min].shot + 1);
            }else {
            //正常値の時
            v0 = buf[i_min].v0_mps;
            if ((v0 > 0) && (v0 < 150)){
                joule = (float)bbmass_g * v0 * v0 / 2000000;
            }
            sprintf(tmp_string, "#%03d %6.2f m/s (%5.3f J)", buf[i_min].shot + 1, v0, joule);
        }
        LCD_Printf(COL_V0_CHART, i * 10 + ROW_V0_CHART + 16, tmp_string, 1, WHITE, 1);
    }
    p++;
    if (p >= BUF_NUM){
        p = 0;
    }
    
}
   

void v0_data_reset(void){
    //v0データをリセットする。　平均→0
    //SW2長押し
    uint8_t     i;
    
    if (target_mode != V0_MODE){
        return;
    }
    
    sprintf(tmp_string, "v0 average RESET? ");
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
    
    //初速データをクリア
    print_v0_chart(1);
    motion_clear_screen();
    sprintf(tmp_string, "DONE!             ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
    __delay_ms(1000);
    sprintf(tmp_string, "                  ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
    
    while(SW2_PORT == SW_ON){
        //キーが離されるのを待つ
    }
    
}



void print_ve(uint8_t color){
    //後ろ側センサー　終速 m/sec　表示と代入 
    sprintf(&bullet_CSVdata[VE][0], "         ");  //バッファをクリア
    
    if (V0_VE_MODE != target_mode){
        //V0_VE_MODE以外では無効
        color = BLACK;
    }
    if (color == BLACK){
        sprintf(tmp_string, "  --- ");  //ve表示を消す = エラーの時(センサ3,4タイムアウト等)
        
    }else if(shot_data[shot_buf_pointer].ve_timer == 0){
        sprintf(tmp_string, " xxxx ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].ve_mps > 999){
        sprintf(tmp_string, " OVER ");
        color = RED;
    }else {
        //正常値の時 
        sprintf(tmp_string, "%6.2f", shot_data[shot_buf_pointer].ve_mps);
        sprintf(&bullet_CSVdata[VE][0], "%9.4f", shot_data[shot_buf_pointer].ve_mps);
    }
    LCD_Printf(COL_VE + 3 * 6, ROW_VE, tmp_string, 1, color, 1);
}


void print_impact_time(uint8_t color){
    //着弾時間 msec　表示と代入
    sprintf(&bullet_CSVdata[IMPACT_TIME][0], "            ");  //バッファをクリア

    if (color == BLACK){
        sprintf(tmp_string, "   ---- ");    //time表示を消去する
        
    }else if(shot_data[shot_buf_pointer].t_imp_msec == 0){
        sprintf(tmp_string, "   ---- ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].t_imp_msec > 9999){
        sprintf(tmp_string, "   OVER ");
        color = RED;
    }else {
        //正常値の時
        sprintf(tmp_string, "%8.3f", shot_data[shot_buf_pointer].t_imp_msec);
        sprintf(&bullet_CSVdata[IMPACT_TIME][0], "%12.6f", shot_data[shot_buf_pointer].t_imp_msec);
    }
    LCD_Printf(COL_TIME + 5 * 6, ROW_TIME, tmp_string, 1, color, 1);
}


void print_target_xy(uint8_t color){
    //着弾座標 mm,mm 表示と代入
    sprintf(&bullet_CSVdata[TARGET_X][0], "        ");     //バッファをクリア
    sprintf(&bullet_CSVdata[TARGET_Y][0], "        ");
        
    if (color == BLACK){
        sprintf(tmp_string, "                  ");  //表示を消去する
        
    }else if ((shot_data[shot_buf_pointer].impact_x >= 399.9) || (shot_data[shot_buf_pointer].impact_y >= 399.9)){
        //着弾位置エラーの時
        sprintf(tmp_string, "x: ---   y: ---   ");

    }else {
        //着弾OKの時
        sprintf(tmp_string, "x:%6.1f y:%6.1f", shot_data[shot_buf_pointer].impact_x, shot_data[shot_buf_pointer].impact_y);
        sprintf(&bullet_CSVdata[TARGET_X][0], "%8.3f", shot_data[shot_buf_pointer].impact_x);
        sprintf(&bullet_CSVdata[TARGET_Y][0], "%8.3f", shot_data[shot_buf_pointer].impact_y);
    }
    LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, color, 1);
}


void    print_target_ctc(float ctc, uint16_t num, uint8_t color){
    //最大着弾点間隔 mmとサンプル数　を表示
    sprintf(&bullet_CSVdata[TARGET_CTC][0], "        ");  //バッファをクリア
    if (color == BLACK){
        if (ctc == 0) {
            //表示を消去しない。
        }else {
            //ctcが0でない時は表示を消去する。
            sprintf(tmp_string, "                  ");  //表示を消去する
            LCD_Printf(COL_TARGET, ROW_TARGET + 10, tmp_string, 2, color, 1);
        }
    }else {
        sprintf(tmp_string, "CtoC%6.1fmm(n=%2d) ", ctc, num);
        sprintf(&bullet_CSVdata[TARGET_CTC][0], "%8.3f", shot_data[shot_buf_pointer].ctc_max);
        LCD_Printf(COL_TARGET, ROW_TARGET + 10, tmp_string, 2, color, 1);
    }
}


void print_target_mode(uint8_t color){
    //モードとパラメータの初期表示と代入
    sprintf(tmp_string, "SHOT#");
    LCD_Printf(COL_SHOT, ROW_SHOT, tmp_string, 2, WHITE, 1);
    
    switch(target_mode){
        case NO_DEVICE:     //0000* 接続無し   測定無し none
        case V0_ERROR1:     //0001e 初速計エラー
        case V0_ERROR2:     //0010e 初速計エラー
        case UNKNOWN:       //0100  未定
        case V0_ERROR3:     //0101e 初速計エラー
        case V0_ERROR4:     //0110e 初速計エラー
        case V0_ERROR5:     //1001e 初速計エラー
        case V0_ERROR6:     //1010e 初速計エラー
        case VE_ONLY_ERROR: //1100  初速計無しエラー
        case V0_ERROR7:     //1101e 初速計エラー
        case V0_ERROR8:     //1110e 初速計エラー
            color = PINK;   //エラーなモードは警告色表示
            LCD_Printf(COL_MODE, ROW_MODE, (char*)&target_mode_mes[target_mode][0], 1, color, 1);
            break;
            
        //motion
        case V0_MODE:           //0011* 初速計のみ　初速測定モード v0
        case V0_MOTION_MODE:    //0111* 初速計のみ　モーション表示モード v0
            sprintf(tmp_string, "v0:      m/s");
            LCD_Printf(COL_V0, ROW_V0, tmp_string, 2, WHITE, 1);
            sprintf(tmp_string, "            ");
            LCD_Printf(COL_VE, ROW_VE, tmp_string, 1, BLACK, 1);
            sprintf(tmp_string, "                 ");
            LCD_Printf(COL_TIME, ROW_TIME, tmp_string, 1, BLACK, 1);
            sprintf(tmp_string, "                  ");
            LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, BLACK, 1);
            motion_clear_screen();
            break;
            
        case V0_VE_MODE:        //1111* 初速、着速、着弾時間測定モード  v0,ve,time
            sprintf(tmp_string, "v0:      m/s");
            LCD_Printf(COL_V0, ROW_V0, tmp_string, 2, WHITE, 1);
            sprintf(tmp_string, "ve:      m/s");
            LCD_Printf(COL_VE, ROW_VE, tmp_string, 1, WHITE, 1);
            sprintf(tmp_string, "time:        msec");
            LCD_Printf(COL_TIME, ROW_TIME, tmp_string, 1, WHITE, 1);
            sprintf(tmp_string, "                  ");
            LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, BLACK, 1);
            motion_graph_initialize();
            break;
            
        //target
        case V0_TARGET_MODE:    //1011* 電子ターゲット　初速、着弾時間測定、着弾位置表示モード　v0,x,y,time
            target_graph_initialize();
            sprintf(tmp_string, "v0:      m/s");
            LCD_Printf(COL_V0, ROW_V0, tmp_string, 2, WHITE, 1);
            sprintf(tmp_string, "            ");
            LCD_Printf(COL_VE, ROW_VE, tmp_string, 1, BLACK, 1);
            sprintf(tmp_string, "time:        msec");
            LCD_Printf(COL_TIME, ROW_TIME, tmp_string, 1, WHITE, 1);
            sprintf(tmp_string, "x: ---   y: ---   ");
            LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, AQUA, 1);
            break;
            
        case TARGET_ONLY_MODE:  //1000* 初速無し電子ターゲットのみ　着弾位置表示モード 着弾x,y
            target_graph_initialize();
            sprintf(tmp_string, "            ");
            LCD_Printf(COL_V0, ROW_V0, tmp_string, 2, BLACK, 1);
            sprintf(tmp_string, "            ");
            LCD_Printf(COL_VE, ROW_VE, tmp_string, 1, BLACK, 1);
            sprintf(tmp_string, "                 ");
            LCD_Printf(COL_TIME, ROW_TIME, tmp_string, 1, BLACK, 1);
            sprintf(tmp_string, "x: ---   y: ---   ");
            LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, AQUA, 1);
            break;
            
        default:
            break;
    }
    LCD_Printf(COL_MODE, ROW_MODE, (char*)&target_mode_mes[target_mode][0], 1, color, 1);
    sprintf(&bullet_CSVdata[DEVICE_MODE][0], "%s", &target_mode_mes[target_mode][0]);
    
    set_v0sensor(0);    //初速センサーの表示

}


void print_error(uint8_t error){
    //エラーの代入と表示
    sprintf(&bullet_CSVdata[ERR_STATUS][0], "%s", &vmeasure_err_mes[error][0]);
    LCD_Printf(COL_ERROR, ROW_ERROR, (char*)&vmeasure_err_mes[error][0], 1, RED, 1);
}


//割込
//Sensor Interrupt
void detect_sensor1(void){
    //センサ1オン割込 CLC1 = 測定スタート
    if (shot_data[shot_buf_pointer].status != VMEASURE_READY){
        return;
    }
    //VideoSync LED PWM start
    PWM1CONbits.EN = 1; //PWM1_16BIT_Enable();
    
    motion_gate = AFTER_SHOT_COUNT + 1; //スタート後のカウント数をセット
    int_status.sensor1on = 1;

#ifdef  TIMING_LOG          //debug        
    time_log = 0;           //log start(たまに、すでにカウントが入っていることもあるため　ノイズ等)
    DEBUG_timing_log(1);    //PT1 return
#endif
}


void detect_sensor2(void){
    //センサ2オン割込 CLC2
    int_status.sensor2on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(2);    //PT2 return
#endif
}


void detect_sensor3(void){
    //センサ3オン割込 CLC3
    int_status.sensor3on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(3);    //PT3 return
#endif
}


void detect_sensor4(void){
    //センサ4オン割込 CLC4
    int_status.sensor4on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(4);    //PT4 return
#endif
}


//Timer Interrupt
void v0_timer_gate(void){
    //TMR1 GATE割込 ライズエッジ2回目でタイマーストップ=測定値
    int_status.v0timer = 1;
}


void ve_timer_gate(void){
    //TMR3 GATE割込 ライズエッジ2回目でタイマーストップ=測定値
    int_status.vetimer = 1;
}


void impact_timer_capture(void){
    //SMT1割込
    int_status.impacttimer = 1;
}


//timer overflow
void timer0_timeout(void){
    //TMR0オーバーフロー
    int_status.tmr0ovf = 1;
}


void timer1_timeout(void){
    //TMR1オーバーフロー
    int_status.tmr1ovf = 1;
}


void timer3_timeout(void){
    //TMR3オーバーフロー
    int_status.tmr3ovf = 1;
}


void smt_timeout(void){
    //SMT1タイマーオーバーフロー
    int_status.smt1ovf = 1;
}



/*
 End of File
*/