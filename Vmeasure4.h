/*  BulletLogger V8
 *  Vmeasure3.h
 * 
 * 
 *  2022.04.24  V7 Vmeasure2.h���ڐA
 * 
 *  2023.06.26  TMR1,TMR2 prescaler 1:1 -> 1:2 �ᑬ�Ή�
 * 
 */

#ifndef VMEASURE3_H
#define VMEASURE3_H


//constant value
#define     TIME_CONV_SMT1      1 / _XTAL_FREQ * 4  //usec/1bit 64MHz/4 
#define     TIME_CONV_TMR1      1 / _XTAL_FREQ * 4 * 2 //usec/1bit 64MHz/4 prescaler1:2
#define     TIME_CONV_TMR3      1 / _XTAL_FREQ * 4 * 2 //usec/1bit 64MHz/4 prescaler1:2


//GLOBAL

//���胂�[�h(�}�g���ڑ��@��)4���[�h�̂ݗL��
typedef enum {
    NO_DEVICE,          //0000* �ڑ�����   ���薳�� none
    V0_ERROR1,          //0001e �����v�G���[
    V0_ERROR2,          //0010e �����v�G���[
    V0_MODE,            //0011* �����v�̂݁@�����W�v���[�h v0
    UNKNOWN,            //0100  ����
    V0_ERROR3,          //0101e �����v�G���[
    V0_ERROR4,          //0110e �����v�G���[
    V0_MOTION_MODE,     //0111* �����v�̂݁@���[�V�����\�����[�h v0
    TARGET_ONLY_MODE,   //1000* ���������d�q�^�[�Q�b�g�̂݁@���e�ʒu�\�����[�h ���ex,y
    V0_ERROR5,          //1001e �����v�G���[
    V0_ERROR6,          //1010e �����v�G���[
    V0_TARGET_MODE,     //1011* �d�q�^�[�Q�b�g�@�����A���e���ԑ���A���e�ʒu�\�����[�h�@v0,x,y,time
    VE_ONLY_ERROR,      //1100  �����v�����G���[
    V0_ERROR7,          //1101e �����v�G���[
    V0_ERROR8,          //1110e �����v�G���[
    V0_VE_MODE,         //1111* �����A�����A���e���ԑ��胂�[�h  v0,ve,time
    NUM_SENSOR_CONNECT, //�錾��  =�@���b�Z�[�W�z��Y����     
} device_connect_t;


//measure status
typedef enum {
    VMEASURE_READY,
    //v0
    SENSOR1_ON_START,
    SHOT_DISP,
    TIMER1_START,
    SMT1_START,        
    SENSOR2_ON,
    V0_TIMER_12_GET,
    //ve       
    WAIT_FLIGHT23,        
    SENSOR3_ON,
    TIMER3_START,
    SENSOR4_ON,
    VE_TIMER_34_GET,
    //inpact time       
    WAIT_IMPACT_TIMER,        
    TIME_IMPACT_GET, 
    //timeout
    V0_TIMEOUT,
    VE_TIMEOUT,
    IMPACT_TIMEOUT,
    //        
    TARGET4_ON_START,
    //        
    MEASURE_DONE,
    WAIT_MOTION,    
    MOTION_DONE,
    //
    RECEIVE_DATA,
    DATA_RECIEVED,
    TARGET_IMPACT_DRAW,
    //
    V0_CHART,        
    //
    MOTION_GRAPH,        
    //        
    IMPACT_POS_PRINT,
    CTOC_PRINT,        
    ERROR_DISP,        
    ANGLE_LEVEL,   
    SD_SAVE_SHOT,
    SD_SAVE_MOTION,
    //
    COMPLETE
} measure_status_t;

extern measure_status_t    measure_status;


//shot data
typedef struct {
    uint16_t    shot;                   //�V���b�g#
    //v0
    uint32_t    v0_timer;               //�����̃^�C�}�[�l
    float       t0_sec;                 //�����̎���[sec]
    float       v0_mps;                 //����[m/sec]]
    //ve
    uint32_t    ve_timer;               //�����̃^�C�}�[�l
    float       te_sec;                 //�����̎���[sec]
    float       ve_mps;                 //����[m/sec]
    //impact
    uint32_t    impact_timer;           //���e�^�C�}�[�l
    float       impact_offset_usec;     //���e���Ԃ̃Z���T�[���B�x�����ԕ␳��[usec]
    float       t_imp_msec;             //���e����[msec]
    //target
    float       impact_x;               //���e�ʒux[mm]
    float       impact_y;               //���e�ʒuy[mm]
    float       ctc_max;                //�Z���^to�Z���^[mm]
    //status
    measure_status_t   status;          //�X�e�[�^�X
} shot_data_t;

extern shot_data_t  shot_data[];        //�z��Y����shot#

//�^�[�Q�b�g�f�[�^�ʐM�o�H
typedef enum {
    NONE,
    RS485,                          //LAN�P�[�u����RS485 --- UART4
    ESP_NOW,                        //WiFi ���� ESP32 ESP-NOW --- UART2
    NUM_TARGET_COM_PATH,            //�錾��  =�@���b�Z�[�W�z��Y����
} target_path_t;

extern target_path_t   target_com_path;


//status
//���蒆�C���W�P�[�^�X�e�[�^�X
typedef enum {
    INDI_OFF,
    INDI_ON,
} indicator_status_t;

extern uint8_t  sensor_type;        //�����Z���T�^�C�v




//globalization
uint8_t target_mode_get(void);

//measure
void    vmeasure_initialize(void);
void    set_v0sensor(bool);
void    vmeasure_ready(void);
uint8_t vmeasure(void);
void    sensor_connect_check(void);

//display
void    print_indicator(indicator_status_t);
void    print_shot(uint8_t);
void    print_v0(uint8_t);
void    print_v0_chart(bool);
void    v0_data_reset(void);
void    print_ve(uint8_t);
void    print_impact_time(uint8_t);
void    print_target_xy(uint8_t);
void    print_target_ctc(float, uint16_t, uint8_t);
void    print_targetmode(uint8_t);
void    print_i_time(void);
void    print_error(uint8_t);

//debug
void    print_pulse(void);
void    print_timer_rawdata(void);

//sensor
void    detect_sensor1(void);
void    detect_sensor2(void);
void    detect_sensor3(void);
void    detect_sensor4(void);

void    v0_timer_gate(void);
void    ve_timer_gate(void);
void    v0_timer_capture(uint16_t);
void    ve_timer_capture(uint16_t);
void    impact_timer_capture(void);

void    timer0_timeout(void);
void    timer1_timeout(void);
void    timer3_timeout(void);
void    smt_timeout(void);

//


#endif //VMEASURE3_H