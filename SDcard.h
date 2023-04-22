/*
 * �f�[�^��SD�J�[�h��CSV�ŏo�͂���
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

//�����o���f�[�^�̃t�H�[�}�b�g
#define     LEN_BLL     (14 + 1)        //�X�g�b�v�}�[�N + 1
extern char bullet_CSVdata[][LEN_BLL];  //SD�J�[�h�֏o���f�[�^�̎��[�p[���ڐ�][�f�[�^��]

enum data_csv_item                      //CSV�f�[�^�̍��ڕ���
{ 
    SHOT,
    DATE,
    TIME,
    TEMP,
    HUMIDITY,
    BARO_PRESS,
    DISTANCE,
    AIRSOFT_TYPE,       //�G�A�\�t�g�^�C�v
    V0_DEVICE,          //�������葕�u
    BB_MASS,            //BB�e����
    BB_TYPE,            //BB�e���
    F_EXTRACT,          //���e��R��
    V0_TIME,            //��������(12mm)
    V0,
    VE,
    IMPACT_TIME,
    TARGET_AIM_H,       //�^�[�Q�b�g�_�_����
    TARGET_X,
    TARGET_Y,
    TARGET_CTC,
    ELEVATION,
    TILT,
    RTCC_LAG,           //�������u�Ԃ��玟��RTCC���荞�݂܂ł̎��ԁ@�f�o�b�O
    DEVICE_MODE,        //10����
    ERR_STATUS,         //14����
    BULLET_ITEM_NUM,    //���ڂ̐� ���ڂ𑝂₵�Ă���`���̓Y���̒l�𒼂��Ȃ��Ă��ǂ�
};


#define     LEN_MOT     (10 + 1)        //10�����Ȃ̂�11�K�v(�X�g�b�v�}�[�N)
extern char motion_CSVdata[][LEN_MOT];  //2021.01.21 �㉺�A���E�̃u����ǉ�

enum    motion_csv_item                 //CSV�f�[�^�̕���
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
    MOTION_ITEM_NUM, //���ڂ̐� ���ڂ𑝂₵�Ă���`���̓Y���̒l�𒼂��Ȃ��Ă��ǂ�
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

extern char     filename_bullet[];          //�t�@�C���l�[�� Bullet LOG
extern char     filename_motion[];          //�t�@�C���l�[�� Motion LOG
extern bool     ShotHeader_write_flag;      //SD�t�@�C���Ƀw�b�_�[���������ݍς݂��̃t���O
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

