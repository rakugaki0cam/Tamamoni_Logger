/*
 *  File Name: Vmeasure4.c
 *
 *  Bullet Verocity Measuring Sub
 * 
 * 2022.04.24       Ver.8.2
 * 2023.01.29       ver.9.0
 * 
 *  @ Signal Measurement Timer
 *      ����: PT1-2      (Photo Transistor)
 *      �I��: PT3-4     
 *      ���e����: PT1-4 
 *          PT1 -----CLCIN0 -- CLC1�Ɋ��蓖�ĐM�����]
 *          PT2 -----CLCIN1 -- CLC2
 *          PT3 -----CLCIN4 -- CLC3
 * 
 *          PT4_LAN--CLCIN5 ---CLC4-CLC8EN=0��LAN �@�M���؂�ւ�
 *          PT4_WiFi-CLCIN2 -|          EN=1��WIFI
 * 
 *          CLC5 -PT1 or PT2
 *          CLC6 -PT3 or PT4
 * 
 *          ����:CLC5OUT -TMR1GATE -TMR1GIF���� & TMR1�^�C�}�[�l
 *          ����:CLC6OUT -TMR3GATE -TMR3GIF���� & TMR3�^�C�}�[�l
 *          ���e����:SMT1�^�C���I�u�t���C�g���[�h
 *                  PT1��PT4�̃��C�Y�G�b�W�Ԃ̎��Ԃ𑪒�
 *                  CLC1OUT -SMT1WIN 
 *                  CLC4OUT -SMT1SIG
 *                  rise edge to rise edge
 *                  -SMT1PRAIF���� & SMT1CPR�^�C�}�[�l
 * 
 *                  SMT1,TMR1,TMR3�Ƃ�
 *                  FOSC/4 = 16MHz Xtal ����\0.0625usec 
 *                   ���x+-30ppm = +-0.003%
 *                  �I�[�o�[�t���[
 *                   SMT1:24bit     --- 1.048576sec�@�@//@30m���炢�܂ł͑Ή�??
 *                   TMR1,TMR3:16bit--- 0.004096sec
 *
 * 
 * 
 * ver.1.00 2020.08.09  BulletVerocityHost12���ڐA���T�u���[�`����
 * 2021.01.02   �w�b�_���ł̒�`�i�Ԉ���������j���ړ�
 * 2021.01.14   �Z���T3-4�����C��76.2->75.82mm
 * 2021.01.16   �Z���T3-4�����C��75.82->75.85mm
 * 2021.01.17   �Z���T3-4�����C��75.85->75.83mm
 * 2021.01.25   �Z���T����������^�C�}�lread�T�u�ďo�p�~�B���W�X�^�𒼐ڏ����B
 * 2021.05.11   �r�f�I�V���NLED (TMR3)-> LAN2pin:VE_CONTROL1 
 * 
 * V7
 * 2021.08.22   ���ԑ�����@��timer1����SMT�ɕύX
 * 2021.09.20   SMT����OK�ɂȂ����̂ŃZ���T��������LEN12=61.8mm,LEN34=76.6mm
 * 2021.09.21   TMR1,3,5�p�~�B�r�f�I�V���NTMR3->PWM3_16BIT 6Hz,duty7%(60fps)  240fps�̂Ƃ���24Hz duty7%
 * 2021.09.22   ���[�V�����\���֌W�͑S��motion.c�ֈړ����AVmeasure�Ɗ��S�ɕ�����
 * 2021.09.25   ���[�V�����f�[�^SD�����o����SDcard.c�ֈړ�
 * 2021.09.28   �V���b�g�C���W�P�[�^���t�H���g����{�b�N�X�`���
 * 2021.10.02   �Z���T3-4�����C��76.6->76.2mm
 * 2021.10.02   �Z���T3-4�����C��76.2->76.4mm
 * 2021.10.13   �Z���T���͂ɐ��K�̏����ȊO�̓��͂����������̓^�C���A�E�g
 * 
 * V8
 * 2022.04.24   V7 Vmeasure2.c���ڐA
 *              �I���v����d�q�^�[�Q�b�g�ɕύX�̑Ή��@PT3�Ȃ��@�ʐM�Œ��e�ʒu��PT4�␳���Ԃ��擾
 * V8_2
 * 2022.04.25   �����̑����SMT1�̃^�C���I�t�t���C�g���[�h�̃��C�Y�G�b�W�Ԃő���B
 *              ���e���Ԃ�TMR1GATE��CAP1�ő���B(�d�q�^�[�Q�b�g���[�h)
 * 2022.04.28   ���e���Ԃ�SMT1�ŁB������TMR1��CCP1�Œ�����TMR3��CCP2�ŁBPT1-4��CLC1-4�ւ���Ĕ��]�Ɠ��͎������B
 * 2022.04.30   CCP���W���[���̓��͂�CLC���g���Ɠ��삵�Ȃ��݂���????
 *              Timer Gate Single Pulse & Toggle Combined Mode. PT1-2:TMR1+CLC5,PT3-4:TMR3+CLC6
 *              ���x1000m/sec�ȏ�̎�OVER�\��
 * 2022.05.03   �����҂��΍�TMR0�^�C���A�E�g�ǉ�
 * 2022.08.20   v0�`���[�g�@�f�[�^��40�i�o�b�t�@���j�ȏ�̂Ƃ��\���X�N���[�����Ȃ��̃o�O�t�B�N�X
 * 2022.10.15   v0�����Z���T��XCORTECH�ƍ����X�g���{�̑I������SW3�Ő؂�ւ�
 * 2022.10.19   �X�g���{�B�e���̃��O���ڒǉ�
 * 
 * 
 * 
 * 
*/

#include "header.h"
#include "Vmeasure4.h"


//constant value
#define     XCORTECH            0       //�����v
#define     STROBE              1       //�����X�g���{�Z���T

#define     LEN12_mm_XCORTECH   62.0    //�Z���T1�`2�ԋ���[mm]        �@�B���@ 62.0mm XCORTECH�����v
#define     LEN12_mm_STROBE     12.0    //�����X�g���{�̏����Z���T[mm]  �@�B���@ 12.0mm
#define     LEN34_mm            77.1    //�Z���T3�`4�ԋ���[mm]        �@�B���@ 76.2mm�Z���T���S�ԋ����A�a���@77.0mm

//display  �h�b�g�ʒu x:COL[��] y:ROW[�s]
//V8����^�C�g���o�[�����̐��@
#define     COL_SHOT        12      //2�{�p�t�H���g12x16
#define     ROW_SHOT        2 
#define     COL_INDI        3       //�W���t�H���g����6x8
#define     ROW_INDI        6
#define     COL_MODE        170
#define     ROW_MODE        4
#define     COL_V0SENS12    232
#define     ROW_V0SENS12    4
//
#define     COL_V0          0       //2�{�p�t�H���g12x16
#define     ROW_V0          20
#define     COL_V0_CHART    0
#define     ROW_V0_CHART    50
#define     COL_ERROR       156
#define     ROW_ERROR       20
#define     COL_VE          156     //�W���t�H���g6x8
#define     ROW_VE          30
#define     COL_TIME        132
#define     ROW_TIME        40
#define     COL_TARGET      2
#define     ROW_TARGET      40

//
#define     NUM_SHOTS       20
//�Z���T�[�X�e�[�^�X
#define     SENSOR1_PORT    PORTAbits.RA0
#define     SENSOR2_PORT    PORTAbits.RA1
#define     SENSOR3_PORT    PORTAbits.RA2
#define     SENSOR4_PORT    PORTAbits.RA3



//global
device_connect_t    target_mode;    ////////////local��������
measure_status_t    measure_status; ////////////local��������
shot_data_t         shot_data[NUM_SHOTS];
uint8_t             sensor_type;


//local
const shot_data_t  clear_shot_data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 999.9, 999.9, 0, VMEASURE_READY};
//�N���A�l: ���e���W(x,y)=(0,0)�́A�ǐ^�񒆂̈Ӗ��Ȃ̂ŁA�N���A�l�̓}�g�̊O�̒l(999.9,999.9)�Ƃ���B
uint16_t    shot = 0;               //�V���b�g��
uint8_t     shot_buf_pointer = 0;   //�V���b�g�������o�b�t�@�p�|�C���^
uint8_t     len12_mm;               //�����Z���T1-2�̋���[mm]
char        v0device[15];           //�������u�̎��
    
    
//����&����X�e�[�^�X �[ ����V�[�P���X�̃`�F�b�N�p
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

//�G���[���b�Z�[�W
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
    NUM_SENSOR_ERROR,  //�錾��  =�@���b�Z�[�W�z��Y����
} vmeasure_error_t;

#define     ERR_LEN     (14 + 1)    //�G���h�}�[�N��+1
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
    //���x���菉���ݒ�
    //SMT1 timer�l
#define     TIME_IMPACT     0x00f423ff      //16000000 = 1sec ���e����1-4��
    
    uint8_t i;
    
    //overflow
    TMR0_SetInterruptHandler(timer0_timeout);
    TMR1_SetInterruptHandler(timer1_timeout);
    TMR3_SetInterruptHandler(timer3_timeout);
    //timer gate
    TMR1_SetGateInterruptHandler(v0_timer_gate);
    TMR3_SetGateInterruptHandler(ve_timer_gate);
    //CLC1-4������MCC�̃R�[�h���ɋL�q
    //SMT1������MCC�̃R�[�h���ɋL�q
    
    //�S�V���b�g�f�[�^�o�b�t�@���N���A
    for (i = 0; i < NUM_SHOTS; i++){
        shot_data[i] = clear_shot_data;
    }
    
    SMT1_SetPeriod((uint32_t)TIME_IMPACT);  //SMT timer overflow�l
}


void set_v0sensor(bool change){
    //�Z���T1-2�̋������Z�b�g
    //change:0 = print only�@�E��
    //change:1 = change & print
    char sen12_str[] = "X";
    
    if (change == 1){
        //�Z���T�ݒ���g�O���؂�ւ�
        switch(sensor_type){
            //�Z���T3��ȏ�ɂ��Ή�
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
            sprintf(v0device, "STROBE  ");      //���O�o�͗p
            break;
        default:
            sensor_type = XCORTECH;     //�������݌�̍ŏ���EEP ROM��0xFF�ɂȂ��Ă���͗l
            DATAEE_WriteByte(V0SENS12_ADDRESS, sensor_type);
        case XCORTECH:
            len12_mm = LEN12_mm_XCORTECH;
            sprintf(sen12_str, "X");
            sprintf(v0device, "XCORTECH");
            break;
    }
    LCD_Printf(COL_V0SENS12, ROW_V0SENS12 , sen12_str, 1, YELLOW, 1); 
    while(SW3_PORT == SW_ON){
        //�L�[���������̂�҂�
    }
    
}


void vmeasure_ready(void){
    //���胊�Z�b�g(����O�N���A)
    
    //Timer stop
    TMR0_StopTimer();
    TMR1_StopTimer();
    TMR3_StopTimer();    
    SMT1_DataAcquisitionDisable();
    PWM1_16BIT_Disable();           //VideoSyncLED off
    //�����N���A
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
    print_indicator(INDI_OFF);      //����C���W�P�[�^�I�t
    //clear
    sleep_count_reset();
    shot_data[shot_buf_pointer] = clear_shot_data;  //���̃f�[�^���������N���A �f�[�^�������̓����O�o�b�t�@
    int_status = clear_status;                      //�V�[�P���X�p�����t���O�N���A
    rx_buffer_clear_rs485();                        //�x���^�C�}�[����i�}�g����̃f�[�^�ǂݎ̂āj///////////
    rx_buffer_clear_esp32();                        //�x���^�C�}�[����i�}�g����̃f�[�^�ǂݎ̂āj///////////
    motion_clear();                                 //�Ō�ɃN���A(motion_gate=1�̊֌W��)

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
    SMT1_DataAcquisitionEnable();           //SMT1 GO (SMT1->0�@ready)
    TMR1_StartSinglePulseAcquisition();     //TMR1 Gate GO (Ready))
}


uint8_t vmeasure(void){
    //�ʑ��x����
    static vmeasure_error_t  err = SENSOR_OK;   //static���Ȃ��ƒl���ۊǂ���Ă��Ȃ�
    uint8_t answ;
    uint8_t i;
    float   rx_data_f[3];   //�d�q�^�[�Q�b�g���e�f�[�^���p
    uint16_t ctc_num;       //�߂�l
    uint8_t c2 = 0;
    uint8_t tmp2;
    
    motion_data_read();     //���[�V�����f�[�^�̓ǂݏo��
    
    switch(shot_data[shot_buf_pointer].status){
        case VMEASURE_READY:
            //����J�n�҂�
            angle_level_disp();            //�X�Ε\��
            //
            if ((int_status.sensor1on == 1) &&
               ((V0_MODE        == target_mode) ||
                (V0_MOTION_MODE == target_mode) ||
                (V0_VE_MODE     == target_mode) ||
                (V0_TARGET_MODE == target_mode))){
                
                TMR0_StartTimer();                      //�^�C���A�E�g���o�p
                TMR3_StartSinglePulseAcquisition();     //TMR3 Gate GO (Ready)�I���p
                LED_LEFT_SetHigh();                     //LED���_�� 
                LED_RIGHT_SetHigh();
                shot_data[shot_buf_pointer].status = SENSOR1_ON_START;
            }
            if ((int_status.sensor4on == 1) &&
                (TARGET_ONLY_MODE == target_mode)){
                
                TMR0_StartTimer();                                  //�^�C���A�E�g���o�p
                shot_data[shot_buf_pointer].status = TARGET4_ON_START;
            }
            break;
            
    //Sensor1-2////////////////////////
        case SENSOR1_ON_START:
            //����X�^�[�g
            print_indicator(INDI_ON);
            sleep_count_reset();
            shot_data[shot_buf_pointer].shot = shot;
            //TMR1 Gate start (hardware CLC5)
            //SMT1 time of Flight start (hardware CLC1)
            shot_data[shot_buf_pointer].status = SMT1_START;
            break;
            
        case SMT1_START:    //���o�͂��Ă��Ȃ�
            //printf("\nSen1ON_START,");
            shot_data[shot_buf_pointer].status = SHOT_DISP;
            break;
            
        case SHOT_DISP:
            //�V���b�g�ԍ��\��
            print_shot(WHITE);
            shot_data[shot_buf_pointer].status = TIMER1_START;
            break;
            
        case TIMER1_START:   //���o�͂��Ă��Ȃ�
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
                //�\���̃^�C���A�E�g���o
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = V0_TIMEOUT;
            }
            break;
            
        case V0_TIMER_12_GET:
            //�Q�[�g�^�C�}�X�g�b�v�l���擾
            shot_data[shot_buf_pointer].v0_timer = TMR1_ReadTimer();
            if (shot_data[shot_buf_pointer].v0_timer != 0){
                shot_data[shot_buf_pointer].t0_sec = shot_data[shot_buf_pointer].v0_timer * (float)TIME_CONV_TMR1;
                shot_data[shot_buf_pointer].v0_mps = (float)(len12_mm) / 1000 / shot_data[shot_buf_pointer].t0_sec;  
#if DEBUG_STROBE_V0     //�X�g���{�̑���l�ƍ���Ȃ�??????�f�o�b�O�K�v  
                printf("V0_Timer: 0x%04lx  ", shot_data[shot_buf_pointer].v0_timer);
                printf("t0:%8.3fusec  ", shot_data[shot_buf_pointer].t0_sec * 1000000);
                printf("v0:%8.2fm/sec\n", shot_data[shot_buf_pointer].v0_mps);
#endif
            }
            //�����\��
            print_v0(WHITE);

            switch(target_mode){
                case V0_MODE:
                case V0_MOTION_MODE:    
                    //���O�f�[�^���
                    print_ve(BLACK);
                    print_impact_time(BLACK);
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
                case V0_VE_MODE:
                    shot_data[shot_buf_pointer].status = WAIT_FLIGHT23;
                    break;
                case V0_TARGET_MODE:
                    //2�s�ڏ����ƃf�[�^���
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
        case TIMER3_START:      //���o�͂��Ă��Ȃ�
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
                //�\���̃^�C���A�E�g���o
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = VE_TIMEOUT;
            }
            break;
        
         case VE_TIMER_34_GET:
            //�Q�[�g�^�C�}�X�g�b�v�l���擾
            shot_data[shot_buf_pointer].ve_timer = TMR3_ReadTimer();
            //printf("Ve_Timer:%4lxh\n", shot_data[shot_buf_pointer].ve_timer);
            if (shot_data[shot_buf_pointer].ve_timer != 0){
                shot_data[shot_buf_pointer].te_sec = shot_data[shot_buf_pointer].ve_timer * (float)TIME_CONV_TMR3;
                shot_data[shot_buf_pointer].ve_mps = (float)(LEN34_mm) / 1000 / shot_data[shot_buf_pointer].te_sec;
            }
            //�����\��
            print_ve(WHITE);
            shot_data[shot_buf_pointer].status = WAIT_IMPACT_TIMER;
            break;
                

    //Impact///////////////////////////
        case WAIT_IMPACT_TIMER:
            //���e�҂�    
            if (int_status.impacttimer == 1){
                shot_data[shot_buf_pointer].status = TIME_IMPACT_GET;
            }
            if (int_status.smt1ovf == 1){
                printf("Smt1_OVF\n");
                shot_data[shot_buf_pointer].status = IMPACT_TIMEOUT;
            }
            if (int_status.tmr0ovf == 1){
                //�\���̃^�C���A�E�g���o
                printf("Tmr0_OVF\n");
                shot_data[shot_buf_pointer].status = IMPACT_TIMEOUT;
            }            
            break;
            
        case TIME_IMPACT_GET:
            //Time-of-Flight Measurement Mode�ɂăZ���T14���C�Y�G�b�W�Ԃ̎��Ԃ��擾
            shot_data[shot_buf_pointer].impact_timer = SMT1CPR;
            shot_data[shot_buf_pointer].t_imp_msec = shot_data[shot_buf_pointer].impact_timer * (float)TIME_CONV_SMT1 * 1000;
    
            //printf("Imp_Timer:%4lxh\n", shot_data[shot_buf_pointer].impact_timer);
            //���e���ԕ\��
            print_impact_time(WHITE);           //�d�q�^�[�Q�b�g�̎��␳�K�v
            shot_data[shot_buf_pointer].status = MEASURE_DONE;
            break;
        
    
    //Target ONLY mode///////////////////////// 
        case TARGET4_ON_START:
            //�����v�����^�[�Q�b�g�I�����[���[�h
            print_indicator(INDI_ON);
            sleep_count_reset();
            shot_data[shot_buf_pointer].shot = shot;
            TMR0_StartTimer();                                  //�^�C���A�E�g���o�p
            print_shot(WHITE);
            //printf("\nTaget4_ON_START,\n");
            //2~4�s�ڏ���&���
            print_v0(BLACK);
            print_ve(BLACK);
            print_impact_time(BLACK);
            shot_data[shot_buf_pointer].status = MOTION_DONE;
            break;
            
    //timeout
        case V0_TIMEOUT:
            //�Z���T2�^�C���A�E�g
            print_shot(RED);
            print_v0(RED);
            err = SENSOR2_ERROR;
            printf("s#%d Sen2 TIMEOUT\n", shot + 1);
            
            switch(target_mode){
                case V0_MODE:
                    //2�`3�s�ڏ���
                    print_ve(BLACK);
                    print_impact_time(BLACK);
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
                case V0_VE_MODE:
                    shot_data[shot_buf_pointer].status = WAIT_FLIGHT23;
                    break;
                case V0_TARGET_MODE:
                    //2�s�ڏ���
                    print_ve(BLACK);
                    shot_data[shot_buf_pointer].status = WAIT_IMPACT_TIMER;
                    break;
                default:
                    shot_data[shot_buf_pointer].status = MEASURE_DONE;
                    break;
            }
            break;
            
        case VE_TIMEOUT:
            //�Z���T4�^�C���A�E�g
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
            print_ve(WHITE);    //V0_VE�QMODE�ȊO�ł͖����ɂȂ�悤�ɃT�u���[�`�����ɂď���
            print_impact_time(RED);
            err = IMPACT_ERROR;
            printf("s#%d Imp TIMEOUT\n", shot + 1);
            shot_data[shot_buf_pointer].status = MEASURE_DONE;
            break;
            
    //**************************//////////////////
        case MEASURE_DONE:
            //�e�����芮��
#ifdef  TIMING_LOG                  //debug
            DEBUG_timing_log(8);    //Stage 8            
#endif
            shot_data[shot_buf_pointer].status = WAIT_MOTION;
            break;
            
        case WAIT_MOTION:
            //��������̃��[�V�������芮���܂ő҂�
            if ((motion_gate == 0) && (motion_message_remain == 0)){
                //���[�V�������b�Z�[�W�Ǎ�������
                int_status.motion = 1;
                shot_data[shot_buf_pointer].status = MOTION_DONE;
                break;                                                  
            }
            if (int_status.tmr0ovf == 1){
                //�\���̃^�C���A�E�g���o
                printf("s#%d Motion TIMEOUT\n", shot + 1);
                err = MOTION_ERROR;
                shot_data[shot_buf_pointer].status = MOTION_DONE;
            }            
            break;
            
        case MOTION_DONE:
            //�S����I��
            //Timer stop
            TMR1_StopTimer();
            TMR3_StopTimer();
            SMT1_DataAcquisitionDisable();
            PWM1_16BIT_Disable();           //VideoSyncLED off
            
#ifdef  TIMING_LOG              //debug
            DEBUG_timing_log(21);       //complete
#endif            
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //�d�q�^�[�Q�b�g�̎�
                shot_data[shot_buf_pointer].status = RECEIVE_DATA;
                break;
            }
            //�d�q�^�[�Q�b�g�ł͂Ȃ���
            if (V0_MODE == target_mode){
                //�����W�v���[�h�̎�
                shot_data[shot_buf_pointer].status = V0_CHART;
                break;
            }
            //���[�V�����O���t�\����
            shot_data[shot_buf_pointer].status = MOTION_GRAPH;
            break;
        
        //TARGET_GRAPH    
        case RECEIVE_DATA:
            //�d�q�^�[�Q�b�g����̃f�[�^��M����
            if (V0_TARGET_MODE == target_mode){////////////////////////////////////RS485/ESP32WiFi////////////////////////
                answ = data_uart_receive_rs485(rx_data_f);    //�m���u���b�L���O����
            }else{
                answ = data_uart_receive_esp32(rx_data_f);    //�m���u���b�L���O����
            }
            if (RX_OK == answ){
                //�f�[�^������Ɏ�M���ꂽ��
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = rx_data_f[0];
                shot_data[shot_buf_pointer].impact_y = rx_data_f[1];
                shot_data[shot_buf_pointer].impact_offset_usec = rx_data_f[2]; 
                shot_data[shot_buf_pointer].status = DATA_RECIEVED;
                break;
            }
            if (CALC_ERROR == answ){
                //�f�[�^���v�Z�ł��Ȃ������Ƃ�
                err = TARGET_CALC_ERROR;
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                __delay_ms(300);                                //target����̃f�[�^�ɏd�Ȃ�Ȃ��悤��
                printf("s#%d target calc err\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
                break;
            }
            if (RX_ERROR == answ){
                //�^�[�Q�b�g�f�[�^���G���[��������
                err = TARGET_ERROR;
                int_status.uart = 1;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                __delay_ms(300);                                //target����̃f�[�^�ɏd�Ȃ�Ȃ��悤��
                printf("s#%d tamamoni rx err\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
                break;
            }
            
            //answ = RX_READING�̎��@�f�[�^��M��
            
            if (int_status.tmr0ovf == 1){
                //�^�C���A�E�g���o
                err = RECEIVE_ERROR;
                shot_data[shot_buf_pointer].impact_x = 999.9;
                shot_data[shot_buf_pointer].impact_y = 999.9;
                shot_data[shot_buf_pointer].ctc_max = shot_data[shot_buf_pointer - 1].ctc_max;
                printf("s#%d tamamoni rx tout\n", shot + 1);
                shot_data[shot_buf_pointer].status = TARGET_DISP;
 
                while (UART2_is_rx_ready()){
                    //�̂ēǂ�
                    tmp2 = UART2_Read();
                    c2++;
                    if (c2 > 64){
                        //�������[�v�΍�
                        break;
                    }
                }
            }
            break;

        case DATA_RECIEVED:
            if(V0_TARGET_MODE == target_mode){
                //�␳�v�Z����impact time�ĕ\��
                shot_data[shot_buf_pointer].t_imp_msec += shot_data[shot_buf_pointer].impact_offset_usec / 1000;    //�I�t�Z�b�g�l�̓}�C�i�X�l�ł���
                print_impact_time(WHITE);
            }        
            shot_data[shot_buf_pointer].status = GRAPH_DRAW;
            break;
            
        case GRAPH_DRAW:
            //���e�ʒu�\��
#define ONE_POINT_DRAW  0
#define RESET_NONE      0
            
            shot_data[shot_buf_pointer].ctc_max = 
            impact_plot_graph(shot, shot_data[shot_buf_pointer].impact_x, shot_data[shot_buf_pointer].impact_y, ONE_POINT_DRAW, RESET_NONE, &ctc_num);


            
            
/////////////////////////////
            
            //��ʊO�̂��Ƃ���///////////////
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
           
        //v0 MODE   
        case V0_CHART:
            //�����W�v�`���[�g&���όv�Z
            print_v0_chart(0);  //0:�ʏ�\��
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
            
        //MOTION_GRAPH
        case MOTION_GRAPH:
            //���[�V�����O���t�\��
            motion_graph_initialize();      //�O���t������

            //���[�V�����̌v�Z//////////////////////////////////���̂ւ�SDcard.c���番������//////////
            //���[�V�����̃O���t
            
            shot_data[shot_buf_pointer].status = TARGET_DISP;
            break;
            
            
        //************************////////////////////////////////////////    
        case TARGET_DISP:
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //�d�q�^�[�Q�b�g�̎�
                print_target_xy(AQUA);  //���e���W
            }else {
                print_target_xy(BLACK); //����
            }
            shot_data[shot_buf_pointer].status = CTOC_PRINT;
            break;
            
        case CTOC_PRINT:
            //�ő咅�e�Z���^to�Z���^��\��
            if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
                //�d�q�^�[�Q�b�g�̎�
                print_target_ctc(shot_data[shot_buf_pointer].ctc_max, ctc_num, ctc_color);       ///////ctc_color��target_graph.c���Ō��肳���@local�ɂ�����
////////////////////////////

            }else {
                //�^�[�Q�b�g�\���ł͖�����
                print_target_ctc(0, 0, BLACK); //���O�f�[�^����̂݁B��ʏ������Ȃ��B�@(��ʏ�������������ctc���[���ȊO�ɂ��ČĂ�)
            }
            shot_data[shot_buf_pointer].status = ERROR_DISP;
            break;
            
        case ERROR_DISP:
            //�G���[�̗L���Ɋւ�炸����
            print_error(err);
            if (SENSOR_OK == err){
#if DEBUG
                printf("shot#%d OK!\n", shot + 1);
#endif
            }
            shot_data[shot_buf_pointer].status = ANGLE_LEVEL;
            break;
            
        case ANGLE_LEVEL:
            //�����o���O�Î~��(ringbuf_count�̈ʒu)�̌X�Ίp�x�f�[�^(shot_log�p)���v�Z��������邽�� =�t���\��
            if (TARGET_ONLY_MODE == target_mode){
                //�����v�Ȃ��̂Ƃ��A���[�V�����̃^�C�~���O���s��
                sprintf( &bullet_CSVdata[TILT]     [0], "         ");
                sprintf( &bullet_CSVdata[ELEVATION][0], "         ");
            }else {
                angle_level_tilt_print(ringbuf_count, 1);
            }
            shot_data[shot_buf_pointer].status = SD_SAVE_SHOT;
            break;
            
        case SD_SAVE_SHOT:
            //�V���b�g���O��SD�J�[�h�֏�������
            answ = shot_log_sd_write();
            if (answ != 0){
                //SD�������݃G���[
                //�Ȃ񂩖ڗ����@�Ȃ���?????????????????????????????????
            }
            shot_data[shot_buf_pointer].status = SD_SAVE_MOTION;
            break;
            
        case SD_SAVE_MOTION:
            //���[�V�������O��SD�J�[�h�֏�������
#ifdef  MOTION_DISP_OFF_V8
            if (TARGET_ONLY_MODE != target_mode){///////////////////////////////
                motion_save();/////////////////////////////////
            }
#endif
            shot_data[shot_buf_pointer].status = COMPLETE;
            break;
            
        case COMPLETE:
            //���̃V���b�g�̂��߂̏I������
#ifdef  TIMING_LOG                  //debug
    DEBUG_time_stamp_save();        //�f�o�O�p�^�C���X�^���v��SD�J�[�h�֏�������
#endif
            //����&�\����������
            shot++;
            shot_buf_pointer++;                         //�V���b�g��+1                 
            if (shot_buf_pointer >= NUM_SHOTS){
                shot_buf_pointer = 0;                   //0�̓N���A�p�̃f�[�^��ۊǂ��Ă���
            }
            LED_RIGHT_SetLow();
            LED_LEFT_SetLow();
            err = SENSOR_OK;
            vmeasure_ready();               //���胊�Z�b�g
            break;
            
        //*** FAULT ********************    
        default:
            //�s���ȃG���[
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
            //�V���b�g���͐i�߂Ȃ�
            vmeasure_ready();               //���胊�Z�b�g
            break;
            
    }
    return shot_data[shot_buf_pointer].status;
}


//�T�u���[�`��

void sensor_connect_check(void){
    //�^�[�Q�b�g�ڑ��`�F�b�N�Ƒ��胂�[�h�̕\��
    static uint8_t  temp = 0xff;
    
    target_mode = (uint8_t)(SENSOR4_PORT << 3) | (uint8_t)(SENSOR3_PORT << 2)
                | (uint8_t)(SENSOR2_PORT << 1) | (uint8_t)SENSOR1_PORT;
    
    if (target_mode != temp){
        print_target_mode(INDIGO);
        temp = target_mode;
    }
}
    
    
void print_indicator(indicator_status_t stat){
    //���蒆�C���W�P�[�^�\��
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
    //�V���b�g���̕\���Ƒ��
    sprintf(tmp_string, "%-3d", shot + 1);     //���l��
    LCD_Printf(COL_SHOT + 5 * 12, ROW_SHOT, tmp_string, 2, color, 1);
    sprintf(&bullet_CSVdata[SHOT][0], "%5d", shot + 1);
}


void print_v0(uint8_t color){
    //�摤�Z���T�[�@���� m/sec�@�\���Ƒ��
    
    sprintf(&bullet_CSVdata[V0_DEVICE][0], "%s", v0device);
    sprintf(&bullet_CSVdata[V0_TIME][0], "      ");    //�o�b�t�@���N���A
    sprintf(&bullet_CSVdata[V0][0], "         ");
    
    
    if (color == BLACK){
        sprintf(tmp_string, "  --- ");  //v0�\�������� = �G���[�̎�(�Z���T2�^�C���A�E�g��)
        
    }else if(shot_data[shot_buf_pointer].v0_timer == 0){
        sprintf(tmp_string, " xxxx ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].v0_mps > 999){
        sprintf(tmp_string, " OVER ");
        color = RED;
    }else {
        //����l�̎�
        sprintf(&bullet_CSVdata[V0_TIME][0], "%6.2f", shot_data[shot_buf_pointer].t0_sec * 1000000);    //usec
        
        sprintf(tmp_string, "%6.2f", shot_data[shot_buf_pointer].v0_mps);
        sprintf(&bullet_CSVdata[V0][0], "%9.4f", shot_data[shot_buf_pointer].v0_mps);
        
    }
    LCD_Printf(COL_V0 + 3 * 12, ROW_V0, tmp_string, 2, color, 1);
}


void print_v0_chart(bool reset){
    //�������[�h m/sec�@�̎��A�ꗗ�\���Ɠ��v�v�Z
    //reset=0:�ʏ�, 1:�f�[�^���N���A���ă��Z�b�g
    typedef struct {
        uint16_t    shot;                   //�V���b�g#
        float       v0_mps;                 //����[m/sec]
    } v0_data_t;
    
#define     BUF_NUM     40      //�����L����    40
#define     ROW_NUM     20      //�����\���s��   20

    static v0_data_t    buf[BUF_NUM];
    static uint8_t      p = 0;          //�|�C���^(�����O�o�b�t�@))
    static float        sum_v0 = 0;     //���ς̃T��
    static uint16_t     n = 0;          //���όv�Z�̂��߂̗L���f�[�^��
    uint8_t             i;
    int8_t              i_min, i_max;
    float               v0, joule;
    
    if (reset == 1){
        //�f�[�^���Z�b�g
        p = 0;
        sum_v0 = 0;
        n = 0;
        return;
    }
    
    //�o�b�t�@�֑��
    buf[p].shot = shot_data[shot_buf_pointer].shot;
    buf[p].v0_mps = shot_data[shot_buf_pointer].v0_mps;
    if ((buf[p].v0_mps != 0) && (buf[p].v0_mps < 999)){
        //����l�̎��͕��όv�Z�ɉ�����
        sum_v0 += buf[p].v0_mps;
        n++;
    }
    
    
    //�ꗗ�\��
    i_max = ROW_NUM;
    if ((p < ROW_NUM - 1) && (n <= (p + 1))){
        //�f�[�^�������Ȃ���
        i_max = (int8_t)p + 1;
    }
    
    //BB����
    sprintf(tmp_string, "BB %5.3fg", (float)bbmass_g / 1000);
    LCD_Printf(180, ROW_V0_CHART + 16, tmp_string, 1, WHITE, 1);
    //����
    if (n == 0){
        //�f�[�^��0�̎�
        sprintf(tmp_string, "ave  ----m/s(n=0)");
    }else {
        sprintf(tmp_string, "ave%6.2fm/s(n=%d)", (sum_v0 / n), n);
    }
    LCD_Printf(COL_V0_CHART, ROW_V0_CHART, tmp_string, 2, CYAN, 1);
    //�`���[�g
    for (i = 0; i < i_max; i++){
        //0~19�s��\��
        if ((p < ROW_NUM - 1) && (n <= (p + 1))){
            //�f�[�^�������Ȃ���
            i_min = (int8_t)i;
        }else{
            i_min = (p + 1) - ROW_NUM + (int8_t)i;
            if (i_min < 0){
                i_min += BUF_NUM; 
                if (i_min >= BUF_NUM){    //�Y����������
                    i_min--;              //�̂�1�}�C�i�X�łȂ����������Ă͂���///////////////////
                }
            }
        }
        
        if(buf[i_min].v0_mps == 0){
            sprintf(tmp_string, "#%03d   xxxx              ", buf[i_min].shot + 1);
        }else if (buf[i_min].v0_mps > 999){
            sprintf(tmp_string, "#%03d   OVER              ", buf[i_min].shot + 1);
            }else {
            //����l�̎�
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
    //v0�f�[�^�����Z�b�g����B�@���ρ�0
    //SW2������
    uint8_t     i;
    
    if (target_mode != V0_MODE){
        return;
    }
    
    sprintf(tmp_string, "v0 average RESET? ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1); 
    //1�b������
    for (i = 0; i < 15; i++){
        __delay_ms(100);
        if (SW2_PORT == SW_OFF){
            sprintf(tmp_string, "                  ");
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
            return;
        }
    }
    
    //�����f�[�^���N���A
    print_v0_chart(1);
    motion_clear_screen();
    sprintf(tmp_string, "DONE!             ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
    __delay_ms(1000);
    sprintf(tmp_string, "                  ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
    
    while(SW2_PORT == SW_ON){
        //�L�[���������̂�҂�
    }
    
}



void print_ve(uint8_t color){
    //��둤�Z���T�[�@�I�� m/sec�@�\���Ƒ�� 
    sprintf(&bullet_CSVdata[VE][0], "         ");  //�o�b�t�@���N���A
    
    if (V0_VE_MODE != target_mode){
        //V0_VE_MODE�ȊO�ł͖���
        color = BLACK;
    }
    if (color == BLACK){
        sprintf(tmp_string, "  --- ");  //ve�\�������� = �G���[�̎�(�Z���T3,4�^�C���A�E�g��)
        
    }else if(shot_data[shot_buf_pointer].ve_timer == 0){
        sprintf(tmp_string, " xxxx ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].ve_mps > 999){
        sprintf(tmp_string, " OVER ");
        color = RED;
    }else {
        //����l�̎� 
        sprintf(tmp_string, "%6.2f", shot_data[shot_buf_pointer].ve_mps);
        sprintf(&bullet_CSVdata[VE][0], "%9.4f", shot_data[shot_buf_pointer].ve_mps);
    }
    LCD_Printf(COL_VE + 3 * 6, ROW_VE, tmp_string, 1, color, 1);
}


void print_impact_time(uint8_t color){
    //���e���� msec�@�\���Ƒ��
    sprintf(&bullet_CSVdata[IMPACT_TIME][0], "            ");  //�o�b�t�@���N���A

    if (color == BLACK){
        sprintf(tmp_string, "   ---- ");    //time�\������������
        
    }else if(shot_data[shot_buf_pointer].t_imp_msec == 0){
        sprintf(tmp_string, "   ---- ");
        color = RED;
    }else if (shot_data[shot_buf_pointer].t_imp_msec > 9999){
        sprintf(tmp_string, "   OVER ");
        color = RED;
    }else {
        //����l�̎�
        sprintf(tmp_string, "%8.3f", shot_data[shot_buf_pointer].t_imp_msec);
        sprintf(&bullet_CSVdata[IMPACT_TIME][0], "%12.6f", shot_data[shot_buf_pointer].t_imp_msec);
    }
    LCD_Printf(COL_TIME + 5 * 6, ROW_TIME, tmp_string, 1, color, 1);
}


void print_target_xy(uint8_t color){
    //���e���W mm,mm �\���Ƒ��
    sprintf(&bullet_CSVdata[TARGET_X][0], "        ");     //�o�b�t�@���N���A
    sprintf(&bullet_CSVdata[TARGET_Y][0], "        ");
        
    if (color == BLACK){
        sprintf(tmp_string, "                  ");  //�\������������
        
    }else if ((shot_data[shot_buf_pointer].impact_x >= 399.9) || (shot_data[shot_buf_pointer].impact_y >= 399.9)){
        //���e�ʒu�G���[�̎�
        sprintf(tmp_string, "x: ---   y: ---   ");

    }else {
        //���eOK�̎�
        sprintf(tmp_string, "x:%6.1f y:%6.1f", shot_data[shot_buf_pointer].impact_x, shot_data[shot_buf_pointer].impact_y);
        sprintf(&bullet_CSVdata[TARGET_X][0], "%8.3f", shot_data[shot_buf_pointer].impact_x);
        sprintf(&bullet_CSVdata[TARGET_Y][0], "%8.3f", shot_data[shot_buf_pointer].impact_y);
    }
    LCD_Printf(COL_TARGET, ROW_TARGET, tmp_string, 1, color, 1);
}


void    print_target_ctc(float ctc, uint16_t num, uint8_t color){
    //�ő咅�e�_�Ԋu mm�ƃT���v�����@��\��
    sprintf(&bullet_CSVdata[TARGET_CTC][0], "        ");  //�o�b�t�@���N���A
    if (color == BLACK){
        if (ctc == 0) {
            //�\�����������Ȃ��B
        }else {
            //ctc��0�łȂ����͕\������������B
            sprintf(tmp_string, "                  ");  //�\������������
            LCD_Printf(COL_TARGET, ROW_TARGET + 10, tmp_string, 2, color, 1);
        }
    }else {
        sprintf(tmp_string, "CtoC%6.1fmm(n=%2d) ", ctc, num);
        sprintf(&bullet_CSVdata[TARGET_CTC][0], "%8.3f", shot_data[shot_buf_pointer].ctc_max);
        LCD_Printf(COL_TARGET, ROW_TARGET + 10, tmp_string, 2, color, 1);
    }
}


void print_target_mode(uint8_t color){
    //���[�h�ƃp�����[�^�̏����\���Ƒ��
    sprintf(tmp_string, "SHOT#");
    LCD_Printf(COL_SHOT, ROW_SHOT, tmp_string, 2, WHITE, 1);
    
    switch(target_mode){
        case NO_DEVICE:     //0000* �ڑ�����   ���薳�� none
        case V0_ERROR1:     //0001e �����v�G���[
        case V0_ERROR2:     //0010e �����v�G���[
        case UNKNOWN:       //0100  ����
        case V0_ERROR3:     //0101e �����v�G���[
        case V0_ERROR4:     //0110e �����v�G���[
        case V0_ERROR5:     //1001e �����v�G���[
        case V0_ERROR6:     //1010e �����v�G���[
        case VE_ONLY_ERROR: //1100  �����v�����G���[
        case V0_ERROR7:     //1101e �����v�G���[
        case V0_ERROR8:     //1110e �����v�G���[
            color = PINK;   //�G���[�ȃ��[�h�͌x���F�\��
            LCD_Printf(COL_MODE, ROW_MODE, (char*)&target_mode_mes[target_mode][0], 1, color, 1);
            break;
            
        //motion
        case V0_MODE:           //0011* �����v�̂݁@�������胂�[�h v0
        case V0_MOTION_MODE:    //0111* �����v�̂݁@���[�V�����\�����[�h v0
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
            
        case V0_VE_MODE:        //1111* �����A�����A���e���ԑ��胂�[�h  v0,ve,time
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
        case V0_TARGET_MODE:    //1011* �d�q�^�[�Q�b�g�@�����A���e���ԑ���A���e�ʒu�\�����[�h�@v0,x,y,time
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
            
        case TARGET_ONLY_MODE:  //1000* ���������d�q�^�[�Q�b�g�̂݁@���e�ʒu�\�����[�h ���ex,y
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
    
    set_v0sensor(0);    //�����Z���T�[�̕\��

}


void print_error(uint8_t error){
    //�G���[�̑���ƕ\��
    sprintf(&bullet_CSVdata[ERR_STATUS][0], "%s", &vmeasure_err_mes[error][0]);
    LCD_Printf(COL_ERROR, ROW_ERROR, (char*)&vmeasure_err_mes[error][0], 1, RED, 1);
}


//����
//Sensor Interrupt
void detect_sensor1(void){
    //�Z���T1�I������ CLC1 = ����X�^�[�g
    if (shot_data[shot_buf_pointer].status != VMEASURE_READY){
        return;
    }
    //VideoSync LED PWM start
    PWM1CONbits.EN = 1; //PWM1_16BIT_Enable();
    
    motion_gate = AFTER_SHOT_COUNT + 1; //�X�^�[�g��̃J�E���g�����Z�b�g
    int_status.sensor1on = 1;

#ifdef  TIMING_LOG          //debug        
    time_log = 0;           //log start(���܂ɁA���łɃJ�E���g�������Ă��邱�Ƃ����邽�߁@�m�C�Y��)
    DEBUG_timing_log(1);    //PT1 return
#endif
}


void detect_sensor2(void){
    //�Z���T2�I������ CLC2
    int_status.sensor2on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(2);    //PT2 return
#endif
}


void detect_sensor3(void){
    //�Z���T3�I������ CLC3
    int_status.sensor3on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(3);    //PT3 return
#endif
}


void detect_sensor4(void){
    //�Z���T4�I������ CLC4
    int_status.sensor4on = 1;
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(4);    //PT4 return
#endif
}


//Timer Interrupt
void v0_timer_gate(void){
    //TMR1 GATE���� ���C�Y�G�b�W2��ڂŃ^�C�}�[�X�g�b�v=����l
    int_status.v0timer = 1;
}


void ve_timer_gate(void){
    //TMR3 GATE���� ���C�Y�G�b�W2��ڂŃ^�C�}�[�X�g�b�v=����l
    int_status.vetimer = 1;
}


void impact_timer_capture(void){
    //SMT1����
    int_status.impacttimer = 1;
}


//timer overflow
void timer0_timeout(void){
    //TMR0�I�[�o�[�t���[
    int_status.tmr0ovf = 1;
}


void timer1_timeout(void){
    //TMR1�I�[�o�[�t���[
    int_status.tmr1ovf = 1;
}


void timer3_timeout(void){
    //TMR3�I�[�o�[�t���[
    int_status.tmr3ovf = 1;
}


void smt_timeout(void){
    //SMT1�^�C�}�[�I�[�o�[�t���[
    int_status.smt1ovf = 1;
}



/*
 End of File
*/