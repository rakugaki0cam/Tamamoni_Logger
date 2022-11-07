/* 
 * File:   motion.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 * Motion DATA process
 * 
 * 
 * Revision history: 
 * 2020.08.09   MPU6050Q10.c���ڐA�@I2C�̋L�q���ύX
 * 2020.08.12   �Z���t�e�X�g��ǉ�
 * 2020.09.05   �\���̕ύX�A�f�[�^�u���b�N��csv�o�͒ǉ�
 * 2020.09.12   �T���v�����O���[�g�������� 125->250Hz
 * 2020.09.15   2���X�Ίp�v�Z
 * 2020.09.16   �I�t�Z�b�g�A�Q�C���A��t�I�t�Z�b�g�C��
 * 2020.09.16   3���X�Ίp�v�Z
 * 2020.09.28   �����O�o�b�t�@128�ɂ���0.5�b�قǂ̃��[�V�����f�[�^�𑪒�(���ςƕ\���͐�������ꂽ)
 * 2021.01.12   �������[�V�������O�̎��ƒʏ펞�̐؂�ւ����}�N���ɂ����@=> LONG_TIME_RECORD
 * 2021.01.16   ���[�V�����f�[�^�V���b�g��̐���8��
 * 2021.01.17   ���[�V�����f�[�^�V���b�g��̐���7��
 * 2021.01.20   ���[�V�����������ƃZ���T�[���𕪗�
 * 2021.01.22   �u���b�N�\���A�W���C�����A���^�C���\���Ȃ��A�O���t�\���ɏW��
 * 2021.05.04   �W���C�������m�F�̂��߂̃��A���^�C���\��  #define GYRO_MONITOR_TEST
 * 2021.05.05   ���t���p�x�␳�l���킹
 * 2021.05.06   ���t���p�x�␳�͂����ł̓_���B�X��LED�̓���p�x������Ȃ��Ȃ�B-> MPU6050.c��accel_offset�ł��
 * 2021.07.23   �X��LED�_�� 0.42�� �� 0.25��
 * 2021.07.24   �X��LED�_�� 0.25�� �� 0.20��
 * 2021.09.20   motion_timer_offset TMR5->SMT1 uint16_t->uint32_t
 * 2021.09.25   MPU6050�����ƃf�[�^�̓ǂݍ��ݎ���̃`�F�b�N�A�C��
 * 2021.09.28   tiltLED���ϒl�̊��Z���V�t�g�ɕύX���������ԍ팸�@/16 -> >>4 32bit�ł�OK�݂���
 * 2021.10.02   �Z���T�[�f�[�^�ǂݏo����FIFO�ɕύX
 *              �X��LED�_�� 0.20�� �� 0.30��
 * 2021.10.02   motion_data_count��motion_gate���܂Ƃ߂�_count���폜
 * 2021.10.08   �u���̌v�Z�A�p���x�͕ω�����̂őO��l�Ƃ̕��ςŌv�Z(��`)
 * 2021.10.09   �u���̌v�Z�A�ϐ��̌^�̎w���Ⴂ��5%�قǏ������Ȃ��Ă����B(���������_������) (float)��t����ʒu�̊ԈႢ�B
 *              �v�Z��̂Ƃ���ɂ��Ă��_���B�v�Z�O�̕ϐ��̓��ɂ���B
 * 2021.10.10   �O���t�\������motion_graph.c�֕���
 * 
 * 
*/

#include "math.h"
#include "header.h"
#include "motion.h"


//display
#define     COL_TILT        0       //�X�Ίp�x
#define     ROW_TILT        280
#define     COL_ERROR       (MOTION_X0 - 16)
#define     ROW_ERROR       (MOTION_Y0 - 64)

//�W���C���l�̃e�X�g�\��
//status
#define     GYRO_DISP_EN    1       //�W���C���̕\��������
#define     GYRO_DISP_DIS   0       //�W���C���̕\�������Ȃ�
#define     GYRO_MONITOR_TEST_no    //�W���C���̌����m�F�̂��߁B�n�[�h�ύX�̎��K�v�B



//global
uint8_t     motion_message_remain = 0;              //���[�V�������b�Z�[�W�Ǎ��c��
uint8_t     motion_gate = 1;                        //���[�V�����Z���T�@���b�Z�[�W�X�V�̉ہ@1:�X�V�� 0:�s��
uint32_t    motion_timer_offset;                    //�Z���T1�I���ƃ��[�V�����f�[�^�ǂݍ��݃^�C�~���O�̃Y��(�^�C�}�[�l)
int32_t     sum_accel_v2[3], sum_gyro_v2[3];        //����v2�p
uint8_t     sum_count_v2;

//local
uint8_t     motion_disp_count = 0;          //�����x�\���Ԉ����p�J�E���^
uint8_t     motion_disp_countup;            //�\���Ԋu�J�E���g�A�b�v�l
uint8_t     led_sample_count;               //�X��LED�̕��σT���v����
uint16_t    fifo_data_at_shot;              //�������u�Ԃł̃Z���TFIFO�f�[�^��
float       Gx, Gy, Gz;                     //�����x�v�Z�l


void motion_initialize(void){
    //�����ݒ�@
    //�Z���T�[��������ɐݒ�
    motion_disp_countup = (uint8_t)(200000 / sample_period);    //�X���\������200,000usec�@�@�e������J�n���Ă��Ȃ����̂ݕ\�����X�V
    
    led_sample_count = SAMPLE;
    if (led_sample_count > RINGBUF_MAX){
        led_sample_count = RINGBUF_MAX;
    }
}

void motion_clear(void){
    //���胊�Z�b�g�@���[�V�����֌W
    uint8_t i;
    
    motion_timer_offset = 0;    //��������̍ŏ��̃f�[�^�̓Ǎ��^�C�~���O
    motion_disp_count = 0;      //��ʕ\���Ԉ����p�J�E���^

    for (i = 0; i <= RINGBUF_MAX; i++){
        motion_log_remaindata[i] = 0;
    }
    //sigmay = 0;                 //�u���ʂ͐ώZ�l�Ȃ̂ŃN���A���Ă���
    //sigmaz = 0;
    
    MPU6050_fifo_reset();       //�����Z�b�g�̌�A�[���ɂ���B
    motion_gate = 1;            //MPU6050�f�[�^�Ǎ���

}


void motion_data_interrupt(void){
    //���[�V�����f�[�^�X�V�������
    if (motion_gate > AFTER_SHOT_COUNT){
        //PT1�Z���T�I����AFTER_SHOT_COUNT+1�ɂ���Ă���
        //����X�^�[�g��̍ŏ��̃f�[�^�X�V�̎������擾
        motion_timer_offset = SMT1TMR;  //SMT1_GetTimerValue()���e�܂ł̋������Z�����^�C�}�[���~�܂��Ă��邩���B
        motion_gate = AFTER_SHOT_COUNT;
        
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(11);       //MPU-time offset
#endif
    }
    
    if (motion_gate != 0){
        motion_message_remain++;           //�ǂݍ��݃��b�Z�[�W�c�����C���N�������g
        if (measure_status != VMEASURE_READY){
            //���蒆�̎������J�E���g�_�E��
            motion_gate--;
        }
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(10);       //MPU6050interrupt
        if (motion_gate == 0){
            DEBUG_timing_log(12);   //MotionGate close
        }
#endif
 
    }
    motion_disp_count++;        //�\���Ԉ����p�J�E���^
}


void motion_data_read(void){
    //���[�V�����f�[�^���Z���T�[����ǂݏo��   <- vmeasure()@vmeasure.c
    uint16_t fifo_count;

    if (motion_message_remain == 0){
        //�ǂݍ��݃f�[�^����
        //�����Ŏ~�߂Ȃ��ƃf�[�^���X�V���Ă��܂��ă_���Bmotion_gate�Ŏ~�߂��Ӗ����Ȃ��Ȃ��Ă��܂��B
//#ifdef  TIMING_LOG              //debug///////////////////////////////////////////////���肷����
        //DEBUG_timing_log(22);       //motion return
//#endif 
        return;
    }
     
    fifo_count = MPU6050_read_fifo();  //�Z���T�[FIFO����ǂݍ���
    
    switch ((fifo_count >> 12) & 0x000f){   //���4�r�b�g�̓X�e�[�^�X
        case 0:
            //read OK
            break;
        case 1:
            //no data
            break;
        case 2:
            //�o�b�t�@����512�o�C�g�ȏオ�g�p����Ă���
            if (measure_status == VMEASURE_READY){
                //���蒆�łȂ����͕\���x��
                //sprintf(tmp_string, "%4d", (fifo_count & 0x0fff));
                //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, YELLOW, 1);   //�\���Ɏ��Ԃ������肷���A�I�[�o�[�t���[����
            }else {
                //���镪�����c���āA���Ƃ͓ǂݏo���Ď̂ĂĂ��������B
            }
            break;
        case 3:
            //FIFO�I�[�o�[�t���[
            //FIFO�̓����O�o�b�t�@�ɂȂ��Ă���̂ŃI�[�o�[�t���[����ƌÂ����̃f�[�^��������
#ifdef  TIMING_LOG                  //debug
            DEBUG_timing_log(20);   //FIFO overflow
#endif            
            if (measure_status == VMEASURE_READY){
                //���蒆�łȂ����̓��Z�b�g
                MPU6050_fifo_reset();
                //sprintf(tmp_string, "FIFO reset");
                //LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, RED, 1);    //�\���������Ȃ�
            }else {
                //�I�[�o�[�t���[�G���[
            }
            break;
    }
}


void angle_level_disp(void){
    //�X���p�x��LED�̕\��
    //����T�C�N���X�^�[�g���Ă��Ȃ���if(measure_stage == 0)�̏���
    angle_level_disp_average_v2(GYRO_DISP_DIS);      //���[�V�����f�[�^��1�s�\���@�@�@���ϐ��������o�[�W����
    
    angle_level_tiltLED();                           //�X��led��_��
#if 0
    if(MPU6050_check_fifo_remain()){
        //fifo�Ǝc�f�[�^��������Ȃ��Ȃ邱�Ƃ�����
        sprintf(tmp_string, "Rem+1");
        LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, CYAN, 1);
    }
#endif
    
}
    

void motion_save(void){
    //���[�V�����O���t�̕\����SD�J�[�h�֏����o��
    //motion_graph_initialize();      //�O���t������
    motion_log_sd_write();
}


void angle_level_tiltLED(void){
    //���E�̌X����LED�Ōx��    
//#define   TILT_WARNING_THR    120     //�_����臒l �}0.42�x (1�x = accely_gain(16384�݌v�l) x 3.1416 / 180 = 286)
//#define   TILT_WARNING_THR    57      //  �}0.20�x
#define     TILT_WARNING_THR    86      //  �}0.30�x

    uint8_t     i;
    int32_t     tilt_angle = 0;
    
    for (i = 0; i < led_sample_count; i++){                     //���v
        tilt_angle += rawdata_accel[WORLD_Y][i];   //accel y(16bit)
    }
    tilt_angle = (tilt_angle >> SHIFT) - accel_offset[WORLD_Y];     //���ϒl
    
    if (tilt_angle + TILT_WARNING_THR <= 0){
        //�����Ⴂ
        LED_RIGHT_SetLow();
        LED_LEFT_SetHigh();
    }else if (tilt_angle - TILT_WARNING_THR >= 0){
        //�E���Ⴂ
        LED_RIGHT_SetHigh();
        LED_LEFT_SetLow();
    }else{
        //����
        LED_RIGHT_SetLow();
        LED_LEFT_SetLow();
    }
    
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(5);        //Tilt LED return
#endif
}


uint8_t angle_level_disp_average_v2(uint8_t gyroOn){
    //�X�Ίp�x�𕽋ω����ĕ\��
    //�\���������̑S�f�[�^�ŕ��ς��v�Z�@0.2sec�̎��ɖ�50�l�̕���
    
    //���ϒl�͓Y���Ō�ringbuf_max�̈ʒu�Ɏ��[�B(�����O�o�b�t�@�͓Y��0~max-1�ŉ��)
    uint8_t         axis;    
    static uint8_t   disp_stage = 0;
    
    //�W���C���l�̃e�X�g�\��
#ifdef GYRO_MONITOR_TEST
    gyroOn = 1;
#endif
            
    switch(disp_stage){
        case 0:
        case 1:
            //�\���̃^�C�~���O�҂�
            //���̊Ԃ͐ώZ��
            if (motion_disp_count >= motion_disp_countup){
                disp_stage = 2;
            }
            break;
        case 2:
            //���ς̌v�Z
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
            //�p�x�̕\��
            if (angle_level_tilt_print(RINGBUF_MAX, 0) == 0){    //�l�����\��
                //�\����������܂Ń��[�v
                disp_stage = 4;
            }
            break;
        case 4:
            //�W���C���̕\���̗L��
            if (gyroOn == 1){
                disp_stage = 5;
            }else {
                disp_stage = 6;
            }
            break;
        case 5:
            //�W���C���̕\��
            if (motion_print_gyro(RINGBUF_MAX, 0, 0, 0) == 0){    //gyro����, not_full_disp, not_start
                //�\����������܂Ń��[�v
                disp_stage = 6;
            }
            break;
        case 6:
            //�\��������̏���
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
    //�����x�v�Z�ƃ��O�l����@Gx:�O�� Gy:���E Gz:�㉺
    //�\���͖���
    Gx = (rawdata_accel[WORLD_X][point] - accel_offset[WORLD_X]) / (float)accel_conv[WORLD_X] * DIRECTION_AX;  
    Gy = (rawdata_accel[WORLD_Y][point] - accel_offset[WORLD_Y]) / (float)accel_conv[WORLD_Y] * DIRECTION_AY;
    Gz = (rawdata_accel[WORLD_Z][point] - accel_offset[WORLD_Z]) / (float)accel_conv[WORLD_Z] * DIRECTION_AZ;
    
    //csv���
    if (csv == 1){
        sprintf( &motion_CSVdata[GX][0],"%9.4f", Gx * 9.8);        //�P�ʁ@m/sec2
        sprintf( &motion_CSVdata[GY][0],"%9.4f", Gy * 9.8);
        sprintf( &motion_CSVdata[GZ][0],"%9.4f", Gz * 9.8);
    }
}


uint8_t angle_level_tilt_print(uint8_t point, bool full_disp){
    //�X�Ίp�x�̌v�Z�ƕ\���ƃ��O�l���
    //full_disp�̎��͕����������Ȃ�
#define     TRIPLE_AXIS_TILT_CAL      //SINGLE, DUAL, TRIPLE
    
    static uint8_t  disp_stage = 0;
    static float    anglex, angley, anglez; //�X�Ίp�x�v�Z�l

    
    do{
        switch(disp_stage){
            case 0:
            case 1:
                //�����x�v�Z�l(Gx, Gy, Gz)���K�v�@
                motion_print_accel(point, 0);   //csv�������
                disp_stage = 2;
                break;
            case 2:
                //3���ł̌X�Όv�Z
                //anglex = (float)(atan((float)Gx / sqrt((float)Gy * Gy + (float)Gz * Gz)) * 180 / 3.1416); 
                //angley = (float)(atan((float)Gy / sqrt((float)Gx * Gx + (float)Gz * Gz)) * 180 / 3.1416);
                //anglez = (float)(atan(sqrt((float)Gx * Gx + (float)Gy * Gy) / (float)Gz) * 180 / 3.1416);
                anglex = atan((float)Gx / sqrt((float)Gy * Gy + (float)Gz * Gz)) * 180 / 3.1416; 
                angley = atan((float)Gy / sqrt((float)Gx * Gx + (float)Gz * Gz)) * 180 / 3.1416;
                anglez = atan(sqrt((float)Gx * Gx + (float)Gy * Gy) / (float)Gz) * 180 / 3.1416;
                //anglez < 0�̎� ///////////////???

                disp_stage = 3;
                break;
            case 3:
                //�t���\���̂Ƃ��̂�
                //LCD
                if(full_disp == 1){
                    angle_level_tilt_disp_init();
                }
                disp_stage = 4;
                break;
            case 4:
                //�\��
                //LCD
                sprintf(tmp_string, "%6.1f", angley);               ////����𔭎˂̂Ƃ��ɌŒ�\���ɂł��Ȃ���?/////////
                //�� ��  0x7e~0x7f�@���p���
                LCD_Printf((COL_TILT + 2 * 12), ROW_TILT, tmp_string, 2, WHITE, 1); //�{�p��2�����E����
                disp_stage = 5;
                break;
            case 5:
                //�\��
                //LCD
                sprintf(tmp_string, "%6.1f", anglex);
                //�� ��  0x80~0x81�@���p���
                LCD_Printf((COL_TILT + 13 * 12), ROW_TILT, tmp_string, 2, WHITE, 1); //�{�p��13�����E����
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
    //�� ��  �� ��  0x7e~0x81�@���p���
    LCD_Printf(COL_TILT, ROW_TILT, tmp_string, 2, WHITE, 1);
}


uint8_t motion_print_gyro(uint8_t po, float time_at_shot, bool full_disp, bool start){
    //�W���C���\���ƃ��O�l���
    // x������x�����[���Ay������y���s�b�`�Az����肪��z�����[
    //time_at_shot�́A�ʂ����˃^�C�~���O�̎��̃I�t�Z�b�g�����A���˂̎��łȂ����͕���
    //full_disp�̎��͕����������Ȃ�
    //start�̎��͑O��l�A�ώZ�l���[��

    static uint8_t  disp_stage = 0;
    static float    sigmay, sigmaz;     //�u���� = �p���x x ���� �̑��a
    static float    omegax, omegay, omegaz;         //�p���x�v�Z�l
    static float    omegay_before, omegaz_before;   //�p���x�O��l

    if (start == 1){
        omegay_before = 0;          //�p���x�O��l�N���A
        omegaz_before = 0;
        sigmay = 0;                 //�u����:�ώZ�l�N���A
        sigmaz = 0;
    }
    
    do{
        switch(disp_stage){
            case 0:
            case 1:
                //�W���C���̌v�Z
                omegax = (rawdata_gyro[WORLD_X][po] - gyro_offset[WORLD_X]) / (float)gyro_conv[WORLD_X] * DIRECTION_GX;
                omegay = (rawdata_gyro[WORLD_Y][po] - gyro_offset[WORLD_Y]) / (float)gyro_conv[WORLD_Y] * DIRECTION_GY;
                omegaz = (rawdata_gyro[WORLD_Z][po] - gyro_offset[WORLD_Z]) / (float)gyro_conv[WORLD_Z] * DIRECTION_GZ;
                sprintf( &motion_CSVdata[ROLL] [0],"%9.4f", omegax);
                sprintf( &motion_CSVdata[PITCH][0],"%9.4f", omegay);
                sprintf( &motion_CSVdata[YAW]  [0],"%9.4f", omegaz);
                disp_stage = 2;
                break;
            case 2:
                //�u���ʂ̌v�Z
                //�u���� = �p���x x ���ԁ@�@�P��:MIL
                //��`�̒藝���g���@�p���x�͕ω����Ă���̂ŁA���ϊp���x=(��1+��2)/2
                //1MIL:360�x/6400 = 0.05625��
                //sample_period:usec => 1/1000000sec
                //sigmay = sigmay + (omegay + omegay_before) * (float)sample_period * ((float)6400 / 360 / 1000000 / 2);
                //6400��360�̂Ƃ���@(float)(6400 / 360)�ł̓_���B17.78������17�ɂȂ��Ă��܂�5%�قǏ������l�ɂȂ��Ă��܂��B
                sigmay = sigmay + (omegay + omegay_before) * (float)sample_period * (float)0.000008888889;     //�s�b�`�̑��v���㉺�̃u����
                sigmaz = sigmaz + (omegaz + omegaz_before) * (float)sample_period * (float)0.000008888889;     //���[�̑��v�����E�̃u����
                sprintf(&motion_CSVdata[BLUR_RL][0],"%9.4f", sigmaz);
                sprintf(&motion_CSVdata[BLUR_UD][0],"%9.4f", sigmay);
                disp_stage = 3;
                break;
            case 3:
                //�O���t�Ƀv���b�g
                if ((V0_TARGET_MODE != target_mode) && (TARGET_ONLY_MODE != target_mode) && (V0_MODE != target_mode)){
                    //�d�q�^�[�Q�b�g�ł͂Ȃ���
                    motion_plot_graph(sigmay, sigmaz, time_at_shot);
                    //�����o�����̃��[�V�����͂��̕ӂł����v�Z�ł��Ȃ�����/////////time_at_shot���O�f�[�^�Ƃ̔䗦�ʒu//////////////
                }
                disp_stage = 4;
                break;
            case 4:
                //�W���C���̃e�X�g�\��
#ifdef  GYRO_MONITOR_TEST
                //LCD
                sprintf(tmp_string, "GYRO ro%6.1f pi%6.1f ya%6.1f", omegax, omegay, omegaz);    //�@�x/�b
                LCD_Printf(COL_TILT, ROW_TILT - 16, tmp_string, 1, GREEN, 1);
#endif  //GYRO_MONITOR_TEST
                
                omegay_before = omegay;     //�O��l�Ƃ��ĕۑ�
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
    //���[�V�����f�[�^�Ǎ��c�� 
    //csv
    if (motion_log_remaindata[data_point] == 0){
        sprintf(&motion_CSVdata[MPU_ERROR][0], "OK       ");
    } else{
        //�ǂݍ��ݎc��
        sprintf(&motion_CSVdata[MPU_ERROR][0], "%4dbytes", motion_log_remaindata[data_point]);
        
    }       
}


void motion_data_remain_check(uint8_t data_point){
    //�Ō�̃f�[�^�ǂݍ��݌��FIFO�J�E���g���[���łȂ���
    //��ʂɌx���\��
#ifdef  MOTION_DISP_OFF_V8
    if ((V0_TARGET_MODE == target_mode) || 
        (TARGET_ONLY_MODE == target_mode) || 
        (V0_MODE == target_mode)){////////////////////////
        return;
    }
#endif
    
    sprintf(tmp_string, "motion error %4dbytes", motion_log_remaindata[data_point]);/////////////���܂����@�@�f�o�b�O���������������������
    LCD_Printf(COL_ERROR, ROW_ERROR, tmp_string, 1, MAGENTA, 1);
}


void motion_data_at_zero(void){
    //�V���b�g����MPU�Z���TFIFO�f�[�^��
#ifdef  MOTION_DISP_OFF_V8
    if ((V0_TARGET_MODE == target_mode) || (TARGET_ONLY_MODE == target_mode)){
        return;
    }
#endif
    
    sprintf(tmp_string, "fifo@zero %4dbytes", fifo_data_at_shot);
    LCD_Printf(COL_ERROR, ROW_ERROR + 8, tmp_string, 1, YELLOW, 1);
}