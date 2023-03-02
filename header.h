/*
 *  BulletLogger V5~V9
 *
 *  header.h
 * 
 * 
 *  2020.08.29
 * 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef HEADER_H
#define	HEADER_H


#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "BME280_v2.h"
#include "colorLCD.h"
#include "font.h"
#include "i2c.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c1_master_example.h"
#include "menu.h"
#include "motion.h"
#include "motion_graph.h"
#include "MPU6050.h"
#include "RTClock_8900.h"
#include "SDcard.h"
#include "spiDMA.h"
#include "target_graph.h"
#include "touch_2046.h"
//#include "tranceiver_rs485.h"
#include "tranceiver_uart.h"
#include "Vmeasure4.h"


#define     I2C1_Read1ByteRegister  i2c_master_read1byte
#define     I2C1_Write1ByteRegister i2c_master_write1byte
#define     I2C1_ReadDataBlock      i2c_master_readblock


//DEBUG
#define     TIMING_LOG_no/////////////////タイミングログ動作の可否 SMTタイマーが着弾で止まるので、ログタイム取得が不可
#define     MOTION_DISP_OFF_V8          //V8 motion目印

//半角文字コード
#define     DEG             0xdf        //deg  = 'ﾟ';　度の半角カナ
#define     DEG_C           0x8b        //degC = '℃';　摂氏度の半角
//display
#define     ROW_WARNING1    300         //ウォーニング表示行1位置
#define     ROW_WARNING2    310         //ウォーニング表示行2位置
#define     COL_WARNING     0           //ウォーニング表示列位置
#define     COL_BATV        0           //バッテリー電圧
#define     ROW_BATV        310
//status
#define     SW_ON           0           //プルアップなので0Vの時がオン
#define     SW_OFF          1

//eep_rom address
typedef enum {
    V0SENS12_ADDRESS,           //初速センサ種類
    FILENAME_YEAR,              //ファイルネームの接尾辞を決める時のデータ
    FILENAME_MONTH,
    FILENAME_DAY,
    FILENAME_SUFFIX,
    DATA_OUTPUT,                //データが書き出されたか
    DIST_M_ADRESS,
    DIST_MM_ADRESS,             //2bytes
    GUN_NUM_ADDRESS     = 9,
    BB_G_ADDRESS,               //2bytes
    BB_NUM_ADDRESS      = 12,
    NUKIDAN_ADDRESS,            //2bytes        
} eeprom_address_t;

//グローバル変数
extern const char   title[];            //プログラムタイトル表示用          @colorLCD.c
extern const char   version[];          //プログラムタイトル表示用          @colorLCD.c
extern char         tmp_string[];       //sprintf用文字列
extern uint8_t      dotRGB[];           //可変できないので、とりあえずMAX width=320 *4 sdからデータ1ライン読み込み用
                                        //1280->640にしたらダメSD動かなくなった。画面クリア時に240x3=720使っているよう
extern FATFS        drive;              //FATfs関連ここで宣言しないとスペース不足エラーでコンパイルできない?
extern FIL          file;
extern UINT         actualLength;
extern bool         sw1_int_flag;       //SW1割込フラグ


#ifdef  TIMING_LOG   //debug
extern uint8_t      time_log;           //タイミングをログする
extern uint8_t      time_point[];
extern uint24_t     time_stamp[];
extern uint8_t      time_remain[];
//
void    DEBUG_timing_log(uint8_t);
#endif


void    footer_rewrite(bool);
void    input_mmi(void);
void    detect_SW1(void);
void    Stop_until_SW1(void);
uint8_t BatV_disp(bool);

//void main_menu(void);
void    sleep_count_reset(void);
void    power_switch(void);
void    power_off(void);
void    shut_down(void);
void    low_battery(void);
void    Low_power(void);
void    read_rom_main(void);

#endif	//HEADER_H