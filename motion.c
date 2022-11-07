/* 
 * File:   motion.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 * Motion DATA process
 * 
 * 
 * Revision history: 
 * 2020.08.09   MPU6050Q10.cより移植　I2Cの記述が変更
 * 2020.08.12   セルフテストを追加
 * 2020.09.05   表示の変更、データブロックのcsv出力追加
 * 2020.09.12   サンプリングレートをあげる 125->250Hz
 * 2020.09.15   2軸傾斜角計算
 * 2020.09.16   オフセット、ゲイン、取付オフセット修正
 * 2020.09.16   3軸傾斜角計算
 * 2020.09.28   リングバッファ128にして0.5秒ほどのモーションデータを測定(平均と表示は制限を入れた)
 * 2021.01.12   長期モーションログの時と通常時の切り替えをマクロにした　=> LONG_TIME_RECORD
 * 2021.01.16   モーションデータショット後の数を8に
 * 2021.01.17   モーションデータショット後の数を7に
 * 2021.01.20   モーション処理部とセンサー部を分離
 * 2021.01.22   ブロック表示、ジャイロリアルタイム表示なし、グラフ表示に集約
 * 2021.05.04   ジャイロ方向確認のためのリアルタイム表示  #define GYRO_MONITOR_TEST
 * 2021.05.05   取り付け角度補正値合わせ
 * 2021.05.06   取り付け角度補正はここではダメ。傾斜LEDの動作角度が合わなくなる。-> MPU6050.cのaccel_offsetでやる
 * 2021.07.23   傾斜LED点灯 0.42° → 0.25°
 * 2021.07.24   傾斜LED点灯 0.25° → 0.20°
 * 2021.09.20   motion_timer_offset TMR5->SMT1 uint16_t->uint32_t
 * 2021.09.25   MPU6050割込とデータの読み込み周りのチェック、修正
 * 2021.09.28   tiltLED平均値の割算をシフトに変更し処理時間削減　/16 -> >>4 32bitでもOKみたい
 * 2021.10.02   センサーデータ読み出しをFIFOに変更
 *              傾斜LED点灯 0.20° → 0.30°
 * 2021.10.02   motion_data_countとmotion_gateをまとめて_countを削除
 * 2021.10.08   ブレの計算、角速度は変化するので前回値との平均で計算(台形)
 * 2021.10.09   ブレの計算、変数の型の指示違いで5%ほど小さくなっていた。(浮動小数点→整数) (float)を付ける位置の間違い。
 *              計算後のところにつけてもダメ。計算前の変数の頭につける。
 * 2021.10.10   グラフ表示部をmotion_graph.cへ分離
 * 
 * 
*/

#include "math.h"
#include "header.h"
#include "motion.h"


//display
#define     COL_TILT        0       //傾斜角度
#define     ROW_TILT        280
#define     COL_ERROR       (MOTION_X0 - 16)
#define     ROW_ERROR       (MOTION_Y0 - 64)

//ジャイロ値のテスト表示
//status
#define     GYRO_DISP_EN    1       //ジャイロの表示をする
#define     GYRO_DISP_DIS   0       //ジャイロの表示をしない
#define     GYRO_MONITOR_TEST_no    //ジャイロの向き確認のため。ハード変更の時必要。



//global
uint8_t     motion_message_remain = 0;              //モーションメッセージ読込残数
uint8_t     motion_gate = 1;                        //モーションセンサ　メッセージ更新の可否　1:更新可 0:不可
uint32_t    motion_timer_offset;                    //センサ1オンとモーションデータ読み込みタイミングのズレ(タイマー値)
int32_t     sum_accel_v2[3], sum_gyro_v2[3];        //平均v2用
uint8_t     sum_count_v2;

//local
uint8_t     motion_disp_count = 0;          //加速度表示間引き用カウンタ
uint8_t     motion_disp_countup;            //表示間隔カウントアップ値
uint8_t     led_sample_count;               //傾斜LEDの平均サンプル数
uint16_t    fifo_data_at_shot;              //撃った瞬間でのセンサFIFOデータ数
float       Gx, Gy, Gz;                     //加速度計算値


void motion_initialize(void){
    //初期設定　
    //センサー初期化後に設定
    motion_disp_countup = (uint8_t)(200000 / sample_period);    //傾き表示周期200,000usec　　弾速測定開始していない時のみ表示を更新
    
    led_sample_count = SAMPLE;
    if (led_sample_count > RINGBUF_MAX){
        led_sample_count = RINGBUF_MAX;
    }
}

void motion_clear(void){
    //測定リセット　モーション関係
    uint8_t i;
    
    motion_timer_offset = 0;    //撃った後の最初のデータの読込タイミング
    motion_disp_count = 0;      //画面表示間引き用カウンタ

    for (i = 0; i <= RINGBUF_MAX; i++){
        motion_log_remaindata[i] = 0;
    }
    //sigmay = 0;                 //ブレ量は積算値なのでクリアしておく
    //sigmaz = 0;
    
    MPU6050_fifo_reset();       //↓リセットの後、ゼロにする。
    motion_gate = 1;            //MPU6050データ読込可

}


void motion_data_interrupt(void){
    //モーションデータ更新割込より
    if (motion_gate > AFTER_SHOT_COUNT){
        //PT1センサオンでAFTER_SHOT_COUNT+1にされている
        //測定スタート後の最初のデータ更新の時刻を取得
        motion_timer_offset = SMT1TMR;  //SMT1_GetTimerValue()着弾までの距離が短い時タイマーが止まっているかも。
        motion_gate = AFTER_SHOT_COUNT;
        
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(11);       //MPU-time offset
#endif
    }
    
    if (motion_gate != 0){
        motion_message_remain++;           //読み込みメッセージ残数をインクリメント
        if (measure_status != VMEASURE_READY){
            //測定中の時だけカウントダウン
            motion_gate--;
        }
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(10);       //MPU6050interrupt
        if (motion_gate == 0){
            DEBUG_timing_log(12);   //MotionGate close
        }
#endif
 
    }
    motion_disp_count++;        //表示間引き用カウンタ
}


void motion_data_read(void){
    //モーションデータをセンサーから読み出し   <- vmeasure()@vmeasure.c
    uint16_t fifo_count;

    if (motion_message_remain == 0){
        //読み込みデータ無し
        //ここで止めないとデータが更新してしまってダメ。motion_gateで止めた意味がなくなってしまう。
//#ifdef  TIMING_LOG              //debug///////////////////////////////////////////////入りすぎる
        //DEBUG_timing_log(22);       //motion return
//#endif 
        return;
    }
     
    fifo_count = MPU6050_read_fifo();  //センサーFIFOから読み込み
    
    switch ((fifo_count >> 12) & 0x000f){   //上位4ビットはステータス
        case 0:
            //read OK
            break;
        case 1:
            //no data
            break;
        case 2:
            //バッファ半分512バイト以上が使用されている
            if (measure_status == VMEASURE_READY){
                //測定中でない時は表示警告
                //sprintf(tmp_string, "%4d", (fifo_count & 0x0fff));
                //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, YELLOW, 1);   //表示に時間がかかりすぎ、オーバーフローする
            }else {
                //いる分だけ残して、あとは読み出して捨てておきたい。
            }
            break;
        case 3:
            //FIFOオーバーフロー
            //FIFOはリングバッファになっているのでオーバーフローすると古い側のデータが消える
#ifdef  TIMING_LOG                  //debug
            DEBUG_timing_log(20);   //FIFO overflow
#endif            
            if (measure_status == VMEASURE_READY){
                //測定中でない時はリセット
                MPU6050_fifo_reset();
                //sprintf(tmp_string, "FIFO reset");
                //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, RED, 1);    //表示が消せない
            }else {
                //オーバーフローエラー
            }
            break;
    }
}


void angle_level_disp(void){
    //傾き角度とLEDの表示
    //測定サイクルスタートしていない時if(measure_stage == 0)の処理
    angle_level_disp_average_v2(GYRO_DISP_DIS);      //モーションデータを1行表示　　　平均数が多いバージョン
    
    angle_level_tiltLED();                           //傾きledを点灯
#if 0
    if(MPU6050_check_fifo_remain()){
        //fifoと残データ数が合わなくなることがある
        sprintf(tmp_string, "Rem+1");
        LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, CYAN, 1);
    }
#endif
    
}
    

void motion_save(void){
    //モーショングラフの表示とSDカードへ書き出し
    //motion_graph_initialize();      //グラフ初期化
    motion_log_sd_write();
}


void angle_level_tiltLED(void){
    //左右の傾きをLEDで警告    
//#define   TILT_WARNING_THR    120     //点灯の閾値 ±0.42度 (1度 = accely_gain(16384設計値) x 3.1416 / 180 = 286)
//#define   TILT_WARNING_THR    57      //  ±0.20度
#define     TILT_WARNING_THR    86      //  ±0.30度

    uint8_t     i;
    int32_t     tilt_angle = 0;
    
    for (i = 0; i < led_sample_count; i++){                     //合計
        tilt_angle += rawdata_accel[WORLD_Y][i];   //accel y(16bit)
    }
    tilt_angle = (tilt_angle >> SHIFT) - accel_offset[WORLD_Y];     //平均値
    
    if (tilt_angle + TILT_WARNING_THR <= 0){
        //左が低い
        LED_RIGHT_SetLow();
        LED_LEFT_SetHigh();
    }else if (tilt_angle - TILT_WARNING_THR >= 0){
        //右が低い
        LED_RIGHT_SetHigh();
        LED_LEFT_SetLow();
    }else{
        //消灯
        LED_RIGHT_SetLow();
        LED_LEFT_SetLow();
    }
    
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(5);        //Tilt LED return
#endif
}


uint8_t angle_level_disp_average_v2(uint8_t gyroOn){
    //傾斜角度を平均化して表示
    //表示周期中の全データで平均を計算　0.2secの時に約50値の平均
    
    //平均値は添字最後ringbuf_maxの位置に収納。(リングバッファは添字0~max-1で回る)
    uint8_t         axis;    
    static uint8_t   disp_stage = 0;
    
    //ジャイロ値のテスト表示
#ifdef GYRO_MONITOR_TEST
    gyroOn = 1;
#endif
            
    switch(disp_stage){
        case 0:
        case 1:
            //表示のタイミング待ち
            //その間は積算中
            if (motion_disp_count >= motion_disp_countup){
                disp_stage = 2;
            }
            break;
        case 2:
            //平均の計算
            for (axis = 0; axis < 3; axis++){
                // average -> [RINGBUF_MAX]
                //rawdata_accel[axis][RINGBUF_MAX] = (int16_t)(sum_accel_v2[axis] / sum_count_v2);
                //rawdata_gyro [axis][RINGBUF_MAX] = (int16_t)(sum_gyro_v2 [axis] / sum_count_v2);
            
                rawdata_accel[axis][RINGBUF_MAX] = sum_accel_v2[axis] / sum_count_v2;
                rawdata_gyro [axis][RINGBUF_MAX] = sum_gyro_v2 [axis] / sum_count_v2;
            }
            disp_stage = 3;
            break;
        case 3:
            //角度の表示
            if (angle_level_tilt_print(RINGBUF_MAX, 0) == 0){    //値だけ表示
                //表示完了するまでループ
                disp_stage = 4;
            }
            break;
        case 4:
            //ジャイロの表示の有無
            if (gyroOn == 1){
                disp_stage = 5;
            }else {
                disp_stage = 6;
            }
            break;
        case 5:
            //ジャイロの表示
            if (motion_print_gyro(RINGBUF_MAX, 0, 0, 0) == 0){    //gyro無し, not_full_disp, not_start
                //表示完了するまでループ
                disp_stage = 6;
            }
            break;
        case 6:
            //表示完了後の処理
            //clear
            for (axis = 0; axis < 3; axis++){
                sum_accel_v2[axis] = 0; 
                sum_gyro_v2 [axis] = 0; 
            }
            sum_count_v2 = 0;
            motion_disp_count = 0;
            disp_stage = 0;
            break;
    }
    
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(6);        //Disp ave return
#endif
    return disp_stage;
}


void motion_print_accel(uint8_t point, bool csv){
    //加速度計算とログ値代入　Gx:前後 Gy:左右 Gz:上下
    //表示は無し
    Gx = (rawdata_accel[WORLD_X][point] - accel_offset[WORLD_X]) / (float)accel_conv[WORLD_X] * DIRECTION_AX;  
    Gy = (rawdata_accel[WORLD_Y][point] - accel_offset[WORLD_Y]) / (float)accel_conv[WORLD_Y] * DIRECTION_AY;
    Gz = (rawdata_accel[WORLD_Z][point] - accel_offset[WORLD_Z]) / (float)accel_conv[WORLD_Z] * DIRECTION_AZ;
    
    //csv代入
    if (csv == 1){
        sprintf( &motion_CSVdata[GX][0],"%9.4f", Gx * 9.8);        //単位　m/sec2
        sprintf( &motion_CSVdata[GY][0],"%9.4f", Gy * 9.8);
        sprintf( &motion_CSVdata[GZ][0],"%9.4f", Gz * 9.8);
    }
}


uint8_t angle_level_tilt_print(uint8_t point, bool full_disp){
    //傾斜角度の計算と表示とログ値代入
    //full_dispの時は分割処理しない
#define     TRIPLE_AXIS_TILT_CAL      //SINGLE, DUAL, TRIPLE
    
    static uint8_t  disp_stage = 0;
    static float    anglex, angley, anglez; //傾斜角度計算値

    
    do{
        switch(disp_stage){
            case 0:
            case 1:
                //加速度計算値(Gx, Gy, Gz)が必要　
                motion_print_accel(point, 0);   //csv代入無し
                disp_stage = 2;
                break;
            case 2:
                //3軸での傾斜計算
                //anglex = (float)(atan((float)Gx / sqrt((float)Gy * Gy + (float)Gz * Gz)) * 180 / 3.1416); 
                //angley = (float)(atan((float)Gy / sqrt((float)Gx * Gx + (float)Gz * Gz)) * 180 / 3.1416);
                //anglez = (float)(atan(sqrt((float)Gx * Gx + (float)Gy * Gy) / (float)Gz) * 180 / 3.1416);
                anglex = atan((float)Gx / sqrt((float)Gy * Gy + (float)Gz * Gz)) * 180 / 3.1416; 
                angley = atan((float)Gy / sqrt((float)Gx * Gx + (float)Gz * Gz)) * 180 / 3.1416;
                anglez = atan(sqrt((float)Gx * Gx + (float)Gy * Gy) / (float)Gz) * 180 / 3.1416;
                //anglez < 0の時 ///////////////???

                disp_stage = 3;
                break;
            case 3:
                //フル表示のときのみ
                //LCD
                if(full_disp == 1){
                    angle_level_tilt_disp_init();
                }
                disp_stage = 4;
                break;
            case 4:
                //表示
                //LCD
                sprintf(tmp_string, "%6.1f", angley);               ////これを発射のときに固定表示にできないか?/////////
                //→ ←  0x7e~0x7f　半角矢印
                LCD_Printf((COL_TILT + 2 * 12), ROW_TILT, tmp_string, 2, WHITE, 1); //倍角で2文字右から
                disp_stage = 5;
                break;
            case 5:
                //表示
                //LCD
                sprintf(tmp_string, "%6.1f", anglex);
                //↑ ↓  0x80~0x81　半角矢印
                LCD_Printf((COL_TILT + 13 * 12), ROW_TILT, tmp_string, 2, WHITE, 1); //倍角で13文字右から
                disp_stage = 6;
            case 6:
                //csv
                sprintf( &bullet_CSVdata[TILT]     [0], "%9.3f", angley);
                sprintf( &bullet_CSVdata[ELEVATION][0], "%9.3f", anglex);
                disp_stage = 0;
                break;
        }
        
    } while((full_disp == 1) && (disp_stage != 0));
    
#ifdef  TIMING_LOG              //debug
    DEBUG_timing_log(7);        //Disp tilt return
#endif
    return disp_stage;
}


void angle_level_tilt_disp_init(void){
    sprintf(tmp_string, "%c%c      %c  %c%c      %c", 0x7f, 0x7e, DEG, 0x80, 0x81, DEG);
    //→ ←  ↑ ↓  0x7e~0x81　半角矢印
    LCD_Printf(COL_TILT, ROW_TILT, tmp_string, 2, WHITE, 1);
}


uint8_t motion_print_gyro(uint8_t po, float time_at_shot, bool full_disp, bool start){
    //ジャイロ表示とログ値代入
    // x軸回りωxがロール、y軸回りωyがピッチ、z軸回りがωzがヨー
    //time_at_shotは、玉が発射タイミングの時のオフセット割合、発射の時でない時は負数
    //full_dispの時は分割処理しない
    //startの時は前回値、積算値をゼロ

    static uint8_t  disp_stage = 0;
    static float    sigmay, sigmaz;     //ブレ量 = 角速度 x 時間 の総和
    static float    omegax, omegay, omegaz;         //角速度計算値
    static float    omegay_before, omegaz_before;   //角速度前回値

    if (start == 1){
        omegay_before = 0;          //角速度前回値クリア
        omegaz_before = 0;
        sigmay = 0;                 //ブレ量:積算値クリア
        sigmaz = 0;
    }
    
    do{
        switch(disp_stage){
            case 0:
            case 1:
                //ジャイロの計算
                omegax = (rawdata_gyro[WORLD_X][po] - gyro_offset[WORLD_X]) / (float)gyro_conv[WORLD_X] * DIRECTION_GX;
                omegay = (rawdata_gyro[WORLD_Y][po] - gyro_offset[WORLD_Y]) / (float)gyro_conv[WORLD_Y] * DIRECTION_GY;
                omegaz = (rawdata_gyro[WORLD_Z][po] - gyro_offset[WORLD_Z]) / (float)gyro_conv[WORLD_Z] * DIRECTION_GZ;
                sprintf( &motion_CSVdata[ROLL] [0],"%9.4f", omegax);
                sprintf( &motion_CSVdata[PITCH][0],"%9.4f", omegay);
                sprintf( &motion_CSVdata[YAW]  [0],"%9.4f", omegaz);
                disp_stage = 2;
                break;
            case 2:
                //ブレ量の計算
                //ブレ量 = 角速度 x 時間　　単位:MIL
                //台形の定理を使う　角速度は変化しているので、平均角速度=(ω1+ω2)/2
                //1MIL:360度/6400 = 0.05625°
                //sample_period:usec => 1/1000000sec
                //sigmay = sigmay + (omegay + omegay_before) * (float)sample_period * ((float)6400 / 360 / 1000000 / 2);
                //6400÷360のところ　(float)(6400 / 360)ではダメ。17.78が整数17になってしまい5%ほど小さい値になってしまう。
                sigmay = sigmay + (omegay + omegay_before) * (float)sample_period * (float)0.000008888889;     //ピッチの総計が上下のブレ量
                sigmaz = sigmaz + (omegaz + omegaz_before) * (float)sample_period * (float)0.000008888889;     //ヨーの総計が左右のブレ量
                sprintf(&motion_CSVdata[BLUR_RL][0],"%9.4f", sigmaz);
                sprintf(&motion_CSVdata[BLUR_UD][0],"%9.4f", sigmay);
                disp_stage = 3;
                break;
            case 3:
                //グラフにプロット
                if ((V0_TARGET_MODE != target_mode) && (TARGET_ONLY_MODE != target_mode) && (V0_MODE != target_mode)){
                    //電子ターゲットではない時
                    motion_plot_graph(sigmay, sigmaz, time_at_shot);
                    //撃ち出し時のモーションはこの辺でしか計算できないかも/////////time_at_shotが前データとの比率位置//////////////
                }
                disp_stage = 4;
                break;
            case 4:
                //ジャイロのテスト表示
#ifdef  GYRO_MONITOR_TEST
                //LCD
                sprintf(tmp_string, "GYRO ro%6.1f pi%6.1f ya%6.1f", omegax, omegay, omegaz);    //　度/秒
                LCD_Printf(COL_TILT, ROW_TILT - 16, tmp_string, 1, GREEN, 1);
#endif  //GYRO_MONITOR_TEST
                
                omegay_before = omegay;     //前回値として保存
                omegaz_before = omegaz;
                disp_stage = 0;
                break;
        }
    } while((full_disp == 1) && (disp_stage != 0));
    
#ifdef  TIMING_LOG              //debug
    DEBUG_timing_log(9);        //Disp gyro return
#endif
    return disp_stage;
}


void motion_print_error(uint8_t data_point){
    //モーションデータ読込残数 
    //csv
    if (motion_log_remaindata[data_point] == 0){
        sprintf(&motion_CSVdata[MPU_ERROR][0], "OK       ");
    } else{
        //読み込み残数
        sprintf(&motion_CSVdata[MPU_ERROR][0], "%4dbytes", motion_log_remaindata[data_point]);
        
    }       
}


void motion_data_remain_check(uint8_t data_point){
    //最後のデータ読み込み後にFIFOカウントがゼロでない時
    //画面に警告表示
#ifdef  MOTION_DISP_OFF_V8
    if ((V0_TARGET_MODE == target_mode) || 
        (TARGET_ONLY_MODE == target_mode) || 
        (V0_MODE == target_mode)){////////////////////////
        return;
    }
#endif
    
    sprintf(tmp_string, "motion error %4dbytes", motion_log_remaindata[data_point]);/////////////いまいち　　デバッグ後消去した方がいいかも
    LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, MAGENTA, 1);
}


void motion_data_at_zero(void){
    //ショット時のMPUセンサFIFOデータ数
#ifdef  MOTION_DISP_OFF_V8
    if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
        return;
    }
#endif
    
    sprintf(tmp_string, "fifo@zero %4dbytes", fifo_data_at_shot);
    LCD_Printf(COL_ERROR, ROW_ERROR + 8, tmp_string, 1, YELLOW, 1);
}