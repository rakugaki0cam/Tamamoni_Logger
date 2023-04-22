/*
 * データをSDカードへCSVで出力する
 * File:   SDcard.h
 * Author: R.Isobe
 * Comments:
 * Revision history: 
 * 2020.08.30
 * 
 */


// This is a guard condition so that contents of this file are not included
// more than once.

#ifndef SDCARD_H
#define	SDCARD_H


//GLOBAL

//書き出すデータのフォーマット
#define     LEN_BLL     (14 + 1)        //ストップマーク + 1
extern char bullet_CSVdata[][LEN_BLL];  //SDカードへ出すデータの収納用[項目数][データ長]

enum data_csv_item                      //CSVデータの項目並び
{ 
    SHOT,
    DATE,
    TIME,
    TEMP,
    HUMIDITY,
    BARO_PRESS,
    DISTANCE,
    AIRSOFT_TYPE,       //エアソフトタイプ
    V0_DEVICE,          //初速測定装置
    BB_MASS,            //BB弾質量
    BB_TYPE,            //BB弾種類
    F_EXTRACT,          //抜弾抵抗力
    V0_TIME,            //初速時間(12mm)
    V0,
    VE,
    IMPACT_TIME,
    TARGET_AIM_H,       //ターゲット狙点高さ
    TARGET_X,
    TARGET_Y,
    TARGET_CTC,
    ELEVATION,
    TILT,
    RTCC_LAG,           //撃った瞬間から次のRTCC割り込みまでの時間　デバッグ
    DEVICE_MODE,        //10文字
    ERR_STATUS,         //14文字
    BULLET_ITEM_NUM,    //項目の数 項目を増やしても定義時の添字の値を直さなくても良い
};


#define     LEN_MOT     (10 + 1)        //10文字なので11必要(ストップマーク)
extern char motion_CSVdata[][LEN_MOT];  //2021.01.21 上下、左右のブレを追加

enum    motion_csv_item                 //CSVデータの並び
{ 
    TIMEACCEL,
    GX,
    GY,
    GZ,
    TIMEGYRO,
    ROLL,
    PITCH,
    YAW,
    BLUR_RL,
    BLUR_UD,
    MPU_ERROR,
    MOTION_ITEM_NUM, //項目の数 項目を増やしても定義時の添字の値を直さなくても良い
};


/*
enum    SD_error
{
    SD_CARD_OK,
    NO_SD_CARD,
    MOUNT_BAD,
    FILE_NAME
};
*/

extern char     filename_bullet[];          //ファイルネーム Bullet LOG
extern char     filename_motion[];          //ファイルネーム Motion LOG
extern bool     ShotHeader_write_flag;      //SDファイルにヘッダーを書き込み済みかのフラグ
//extern bool     data_saved_flag;


//
void    generate_filename_date(void);
uint8_t  shotheader_sd_write(void);
uint8_t  shot_log_sd_write(void);
uint8_t  motion_log_sd_write(void);
void    DEBUG_time_stamp_save(void);

uint8_t  SDcard_open(char *);
void    SDcard_write(void);
void    SDcard_close(void);
void    SDcard_false(void);
void    SDcard_false_erase(void);


#endif	//SDCARD_H

