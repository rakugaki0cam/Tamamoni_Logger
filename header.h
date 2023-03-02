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
#define     TIMING_LOG_no/////////////////�^�C�~���O���O����̉� SMT�^�C�}�[�����e�Ŏ~�܂�̂ŁA���O�^�C���擾���s��
#define     MOTION_DISP_OFF_V8          //V8 motion�ڈ�

//���p�����R�[�h
#define     DEG             0xdf        //deg  = '�';�@�x�̔��p�J�i
#define     DEG_C           0x8b        //degC = '��';�@�ێ��x�̔��p
//display
#define     ROW_WARNING1    300         //�E�H�[�j���O�\���s1�ʒu
#define     ROW_WARNING2    310         //�E�H�[�j���O�\���s2�ʒu
#define     COL_WARNING     0           //�E�H�[�j���O�\����ʒu
#define     COL_BATV        0           //�o�b�e���[�d��
#define     ROW_BATV        310
//status
#define     SW_ON           0           //�v���A�b�v�Ȃ̂�0V�̎����I��
#define     SW_OFF          1

//eep_rom address
typedef enum {
    V0SENS12_ADDRESS,           //�����Z���T���
    FILENAME_YEAR,              //�t�@�C���l�[���̐ڔ��������߂鎞�̃f�[�^
    FILENAME_MONTH,
    FILENAME_DAY,
    FILENAME_SUFFIX,
    DATA_OUTPUT,                //�f�[�^�������o���ꂽ��
    DIST_M_ADRESS,
    DIST_MM_ADRESS,             //2bytes
    GUN_NUM_ADDRESS     = 9,
    BB_G_ADDRESS,               //2bytes
    BB_NUM_ADDRESS      = 12,
    NUKIDAN_ADDRESS,            //2bytes        
} eeprom_address_t;

//�O���[�o���ϐ�
extern const char   title[];            //�v���O�����^�C�g���\���p          @colorLCD.c
extern const char   version[];          //�v���O�����^�C�g���\���p          @colorLCD.c
extern char         tmp_string[];       //sprintf�p������
extern uint8_t      dotRGB[];           //�ςł��Ȃ��̂ŁA�Ƃ肠����MAX width=320 *4 sd����f�[�^1���C���ǂݍ��ݗp
                                        //1280->640�ɂ�����_��SD�����Ȃ��Ȃ����B��ʃN���A����240x3=720�g���Ă���悤
extern FATFS        drive;              //FATfs�֘A�����Ő錾���Ȃ��ƃX�y�[�X�s���G���[�ŃR���p�C���ł��Ȃ�?
extern FIL          file;
extern UINT         actualLength;
extern bool         sw1_int_flag;       //SW1�����t���O


#ifdef  TIMING_LOG   //debug
extern uint8_t      time_log;           //�^�C�~���O�����O����
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