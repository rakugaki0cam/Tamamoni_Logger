/*  motion.h
 * 
 *  motion data process
 * 
 *  2021.01.20
 * MPU6050よりモーション処理部を分離
 * 
*/

#ifndef MOTION_H
#define MOTION_H


//motion log 記録データ数に関するもの
#define     SHIFT               4       //割り算の変わりにビットシフトするための値
#define     SAMPLE              16      //2＾4
#define     RINGBUF_MAX         SAMPLE  //リングバッファ=データ保存バイト数 　静止状態はセンサ1オンの20msec以上前を採用
#define     AFTER_SHOT_COUNT    7       //撃った後に記録するデータ数(撃つ前のデータ数は16-7=9個になる)
//#define     FIFO_OVERFLOW       0x1000  //オーバーフローの時

#define     LONG_TIME_RECORD_no
#ifdef      LONG_TIME_RECORD            //ブローバック動作等の長時間を記録したい場合
    #define RINGBUF_MAX         128
    #define AFTER_SHOT_COUNT    (RINGBUF_MAX - 8)
#endif


//グローバル変数
extern uint8_t     motion_message_remain;               //モーションメッセージ読み込み残数
extern uint8_t     motion_gate;                         //モーションセンサ　データ更新残数  Vmeasure2.c
extern uint32_t    motion_timer_offset;                 //センサ1オンとモーションデータ読み込みタイミングのズレ(タイマー値) @measure.c @motion.c
extern int32_t     sum_accel_v2[], sum_gyro_v2[];       //平均v2用
extern uint8_t     sum_count_v2;                     //平均するためのサンプル数


void    motion_initialize(void);
void    motion_clear(void);
void    motion_data_interrupt(void);
void    motion_data_read(void);         
void    angle_level_disp(void);
void    angle_level_tiltLED(void);
uint8_t angle_level_disp_average_v2(uint8_t);
void    motion_save(void);

void    motion_print_accel(uint8_t, bool);
uint8_t angle_level_tilt_print(uint8_t, bool);
void    angle_level_tilt_disp_init(void);
uint8_t motion_print_gyro(uint8_t, float, bool, bool);
void    motion_print_error(uint8_t);
void    motion_data_remain_check(uint8_t);
void    motion_data_at_zero(void);

#endif //MOTION_H