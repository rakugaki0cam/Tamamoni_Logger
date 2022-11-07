/* 
 * File:   MPU6050.c
 * Author: IsobeR
 * Comments: PIC18F27Q43 MCC
 *
 *      Accelerometer
 *      Gyroscope
 *
 *      6-axis sensor     
 *      I2C
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
 * 2021.01.20   �Z���T�[���ƃ��[�V���������ɕ�����
 * 2021.01.26   �e�X�g�̂Ƃ��뒼��
 * 2021.05.04   6�p���̎����l���Q�C���A�I�t�Z�b�g��2���@�̒l�ɍ��킹��
 * 2021.05.06   ���t���p�x�␳�͂�����ōs���Bmotion.c�̕��ł͌X��LED�̕\��������Ȃ��Ȃ邽�߁B
 * 2021.05.22   ���t���p�x�̊ȈՕ␳(�x�Ŏw��)���ǉ�
 * 2021.08.16   �G���x�[�V�����\����0.34���ɂȂ����̂ŃI�t�Z�b�g�ďC��
 * 2021.09.20   motion_timer_offset TMR5��SMT1
 * 2021.10.02   ���荞�݂̏������[�`����motion.c��
 * 2021.10.10   FIFO�ǂݍ��݃o�O�t�B�N�X�B
 * 
 * 
*/

#include "math.h"
#include "header.h"
#include "MPU6050.h"

#define     ANGLE_MEASURE_OFFSET        //�Y���p�x���C��

#define     MPU6050_ADDRESS 0x68        //I2C�ڑ� MPU6050 6���@�����W���C���Z���T�[���W���[��
                                        //ADO =+Vcc:ID 0x69  =GND:ID 0x68
//initialize
#define     SAMPLING_RATE_250HZ
#define     TEMP_ENABLE_no
#define     TEST_YES_no                 //TEST_YES�ŏ�������Ƀe�X�g���s���A�ʏ퓮��ց@6�p���̎����Ɏg�p

//global
int16_t     rawdata_accel[3][RINGBUF_MAX + 1];      //G�f�[�^�̃X�g�A �ŏI�֕��ϒl����
int16_t     rawdata_gyro[3][RINGBUF_MAX + 1];       //�W���C���f�[�^�̃X�g�A
uint16_t    motion_log_remaindata[RINGBUF_MAX + 1]; //�Z���T�[FIFO�f�[�^�c���@16bit 0�`1024�o�C�g
uint16_t    sample_period;              //MPU6050 �T���v�����O�Ԋu usec
int16_t     accel_offset[3];            //G�I�t�Z�b�g�l
int16_t     accel_conv[3];              //G�Q�C��
uint16_t    accel_DLPF_delay;           //G DigitalLowPassFilter�̃f�B���C
int16_t     gyro_offset[3];             //�W���C���I�t�Z�b�g�l
int16_t     gyro_conv[3];               //�W���C���Q�C��
uint16_t    gyro_DLPF_delay;            //�W���C���@DLPF�̃f�B���C
uint8_t     ringbuf_count;              //�����x�f�[�^�ۑ������O�o�b�t�@�J�E���g

//local
int16_t     mpu_temp;                   //�Z���T���x


bool MPU6050_initialize(void){
    // InvenSense 6axis Motion accel gyro
    
    if (I2C1_Read1ByteRegister(MPU6050_ADDRESS, 117) != 0x68){           //ID return value
        printf("MPU6050 ERROR\n");
        //�G���[��Ԃ�
        LED_RIGHT_SetHigh();
        LED_LEFT_SetHigh();
        __delay_ms(1000);
        return 1;
    }
   
    printf("MPU6050 INIT...");
    //reset
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 107, 0x80);            //Device RESET
    __delay_ms(100);
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 104, 0x07);            //Signal Path reset gyro accel temp
    __delay_ms(100);
    //config
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 107, 0x00);            //�X���[�v����
    
#ifdef      SAMPLING_RATE_125HZ
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 25, 7);                //�T���v�����[�g1kHz/(1+x) 125Hz=8msec   LPF�����������Ƃ��������Ȃ�??
    sample_period = 7959;                                           //MPU6050 �T���v�����O�Ԋu usec �����l
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 26, 0x01);             //�f�W�^��LPF 1:accel 184Hz(delay2.0msec)Fs 1kHz, gyro 188Hz(delay1.9msec)Fs 1kHz�@0�͂��������Ȃ�? 
    accel_DLPF_delay = 2000;
    gyro_DLPF_delay = 1900;
#endif
    
#ifdef      SAMPLING_RATE_250HZ 
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 25, 31);               //�T���v�����[�g Fs8kHz/(1+x) 250Hz=4msec
    //sample_period = 3976;                                         //MPU6050 �T���v�����O�Ԋu usec
    sample_period = 3955;                                           //MPU6050 �T���v�����O�Ԋu usec �����l2021/10/01
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 26, 0x00);             //�f�W�^��LPF 0:accel 260Hz(delay0msec)Fs 1KHz, gyro 256Hz(delay0.98msec)Fs 8kHz
    accel_DLPF_delay = 0;
    gyro_DLPF_delay = 980;
#endif
    
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 27, 0x00);             //Gyro config +-250deg/sec
#define     GYRO_CONV_X      131                                    //16bit => 65536 �� (-250 ~ +250) = 65536 / 500 = 131.072 
#define     GYRO_CONV_Y      131
#define     GYRO_CONV_Z      131
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 28, 0x00);             //0x00:Accel config  +-2G  ////////0x08 +-4G, 0x10 +-8G, 0x18 +-16G
#define     ACCEL_CONV_X     16416                                  //16bit => 65536 �� (-2 ~ +2) = 65536 / 4 = 16384 typical
#define     ACCEL_CONV_Y     16324                                  //6�p���̎����l��苁�߂�
#define     ACCEL_CONV_Z     16930
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 55, 0xc0);             //interrupt bit7:level 1:activeL, bit6:open 1:opendrain, bit5:latch 0:50usec, bit4:clear 0:reg58clear, bit3-0:000 

#ifdef FIFO_DISABLE
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 56, 0x01);             //bit0:DATA ready interrupt
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 106, 0x00);            //FIFO disable
#else
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 56, 0x11);             //bit4:FIFO overflow, bit0:DATA ready interrupt
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 35, 0x78);             //FIFO Gyro x,y,z + Accelxyz 
    #define     MESSAGE_LENGTH  12                                  //fifo1�g�̃��b�Z�[�W�̓Ǐo���f�[�^�o�C�g���@accel 2x3 + gyro 2x3
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 106, 0x04);            //FIFO disable & reset   
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 106, 0x40);            //FIFO enable   
#endif
    //IOCAF5_SetInterruptHandler(MPU6050_interrupt);                //���荞�ݏ�������Z�b�g
    IOCAF5_SetInterruptHandler(motion_data_interrupt);              //���荞�ݏ�������Z�b�g
    ringbuf_count = 0;                                              //�����x�f�[�^�����O�o�b�t�@�J�E���g

    //�I�t�Z�b�g�l�̃Z�b�g (�����l���)
    gyro_offset[0] = -940;  //sensor x  = Z���܂�� ���[�C���O
    gyro_offset[1] = +360;  //sensor y  = Y���܂�� �s�b�`���O
    gyro_offset[2] = -224;  //sensor z  = X���܂�� ���[�����O
    
#if OFFSET_BY_CALC
    //�O����ł̒l (6�p�������l����̌v�Z���)
    accel_offset[0] = +653;     //sensor x
    accel_offset[1] = -150;     //sensor y
    accel_offset[2] = -2342;    //sensor z
#endif
    
    //�}�E���g��ł̏C���I�t�Z�b�g�l(���t�����ő����X������)    
    //�}�E���g������Ԃ�ACCEL_OFFSET�𑪒�(#define TEST_YES)���ĕ��όv�Z�l����
    accel_offset[0] = 17081 - ACCEL_CONV_X;     //sensor x = Z��(+1G)= �d�͕��� vertical
    accel_offset[1] = -72;                      //sensor y = Y��     = ���E���� lateral
    accel_offset[2] = -2387;                    //sensor z = X��     = �O����� longitudinal   
    
#ifdef ANGLE_MEASURE_OFFSET
    //�����}�E���g���̎����ǂݎ��p�x���␳
    //��̌����ɂ��B�@Xaxis:sensorz, Yaxis:sensory, Zaxis:sensorx

#define     YAW_ANGLE_OFFSET       0            //����p�x�@���������
#define     ROLL_ANGLE_OFFSET      -0.9         //���E�̌X���p�x�̕\���Y��(��) TILT_ANGLE_OFFSET  21/07/05 0 -> -0.9
#define     PITCH_ANGLE_OFFSET     (-0.85+0.34) //�㉺�̂������p�x�̕\���Y��(��) ELEVATION_ANGLE_OFFSET�@21/07/11 -0.45 -> -0.85�@�� 8/16:0.34���ƂȂ����̂ōďC��
    //�@��𐅕��ɃZ�b�g�������̕\���l��ݒ�@��
    
    accel_offset[0] += (int16_t)(sin((float)YAW_ANGLE_OFFSET   * 3.1416 / 180) * ACCEL_CONV_X);
    accel_offset[1] += (int16_t)(sin((float)ROLL_ANGLE_OFFSET  * 3.1416 / 180) * ACCEL_CONV_Y);
    accel_offset[2] -= (int16_t)(sin((float)PITCH_ANGLE_OFFSET * 3.1416 / 180) * ACCEL_CONV_Z); //�����̓Z���T�̌����ɂ��̂ł����̓}�C�i�X
    //motion.c�ŕ\���̎��݂̂̎��t���p�x�␳�ōs�� -> �X��LED�̌v�Z�ɓ����Ă��Ȃ��̂Ń{�c
    //accel_offset�ł��ƌv�Z�ɓ����Ă���??
#endif
    
    //�W���C�����Z�W��
    gyro_conv[0] = GYRO_CONV_X;
    gyro_conv[1] = GYRO_CONV_Y;
    gyro_conv[2] = GYRO_CONV_Z;
    //�����x���Z�W��
    accel_conv[0] = ACCEL_CONV_X;
    accel_conv[1] = ACCEL_CONV_Y;
    accel_conv[2] = ACCEL_CONV_Z;
    
    
#ifdef TEST_YES
    //�ŏ������̐ݒ�l�����߂�
    //�Z���t�e�X�g�l
    //MPU6050_SelfTest_accel();
    //MPU6050_SelfTest_gyro();
    
    //�I�t�Z�b�g�A�Q�C�������߂�
    //���x���ɍڂ��āA6�p����G�l�𑪒肵�A�d��-1G�A+1G����Q�C�����v�Z�B
    //���S�̃Y������I�t�Z�b�g�����߂�B(�I�t�Z�b�g�̓}�C�i�X�̒l���傫�����̓}�C�i�X�B)
    
    while(1){
        MPU6050_offset_accel();
    }
    
    //�W���C���͐Î~�����Ă���Ƃ��̒l���I�t�Z�b�g�l�B
    //MPU6050_offset_gyro();
    
    LCD_ClearScreen(0);         // ��ʂ����ŏ���

#endif //TEST_YES
    printf("OK!\n");
    return 0;
}


void MPU6050_interrupt(void){
    //�f�[�^�X�V����
    //motion_data_interrupt();
    //����IOCAF5_SetInterruptHandler��motion_data_interrupt()��ݒ�
}
    

void MPU6050_read_rawdata(void){
    //���W�X�^����̓ǂݏo��
    //���[�V�����Z���T�f�[�^�X�V -> �����O�o�b�t�@�ɕۊ�
    uint8_t     data[14];
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(13);       //MPU6050DataRead
#endif
        
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 59, data, 14);
    rawdata_accel[0][ringbuf_count] = (data[0] << 8) + data[1];             //accelx
    rawdata_accel[1][ringbuf_count] = (data[2] << 8) + data[3];             //accely
    rawdata_accel[2][ringbuf_count] = (data[4] << 8) + data[5];             //accelz
#ifdef TEMP_ENABLE
    mpu_temp = (data[6] << 8) + data[7];                                    //temp
#endif
    rawdata_gyro [0][ringbuf_count] = (data[8] << 8) + data[9];             //gyrox
    rawdata_gyro [1][ringbuf_count] = (data[10] << 8) + data[11];           //gyroy
    rawdata_gyro [2][ringbuf_count] = (data[12] << 8) + data[13];           //gyroz
    
    ringbuf_count++;
    if (ringbuf_count >= RINGBUF_MAX){
        ringbuf_count = 0;     //�J�E���g�������O�����Ă���@���l:0~max-1
    }
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(14);       //MPU6050DataComp
#endif
}


uint16_t MPU6050_read_fifo(void){
    //FIFO����̃f�[�^�ǂݍ���
    //FIFO�o�b�t�@��1024�o�C�g�@12�o�C�g x 85�� + �]��4   �\���ȗʂ�����
    
    uint8_t     data[MESSAGE_LENGTH];
    uint8_t     axis;
    uint16_t    fifo_count;    //���4�r�b�g�͎��ʗp
    
    //FIFO Count
    fifo_count = MPU6050_read_fifo_count();
    if (fifo_count < MESSAGE_LENGTH){
        //�ǂݏo���f�[�^���s���̎�
        //�ǂݏo����fifo_count�������̂悤�ɂȂ�悤(-> 1012�Ƃ��̐���)�ł��Ȃ��݂���???
        fifo_count = 0x1000 | fifo_count;   //1:no data
#ifdef  TIMING_LOG              //debug////////////////////////////////////////////////
    DEBUG_timing_log(23);       //MPU nodata return
#endif 
        return fifo_count;
    }
    
    //FIFO overflow
    if (MPU6050_read_fifo_overflow() != 0){
        fifo_count = 0x3000 | fifo_count;   //3:overflow
#ifdef  TIMING_LOG                  //debug////////////////////////////////////////////////
        DEBUG_timing_log(24);       //MPU overflow return  
#endif 
        return fifo_count;
    }
    
#ifdef  TIMING_LOG              //debug
    DEBUG_timing_log(13);       //MPU6050DataRead
#endif 
    //read data from FIFO buffer
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 116, data, MESSAGE_LENGTH); //FIFO data read
    rawdata_accel[0][ringbuf_count] = (data[0] << 8) + data[1];     //accelx
    rawdata_accel[1][ringbuf_count] = (data[2] << 8) + data[3];     //accely
    rawdata_accel[2][ringbuf_count] = (data[4] << 8) + data[5];     //accelz
    rawdata_gyro [0][ringbuf_count] = (data[6] << 8) + data[7];     //gyrox
    rawdata_gyro [1][ringbuf_count] = (data[8] << 8) + data[9];     //gyroy
    rawdata_gyro [2][ringbuf_count] = (data[10] << 8) + data[11];   //gyroz

    //�X�Ίp�x����v2�p
    for (axis = 0; axis < 3; axis++){
        //���ϒl�p�ώZ
        sum_accel_v2[axis] += rawdata_accel[axis][ringbuf_count]; 
        sum_gyro_v2[axis] += rawdata_gyro[axis][ringbuf_count];
    }
    sum_count_v2++;          //���σT���v�����J�E���^�C���N�������g
    
    fifo_count -= MESSAGE_LENGTH;
    motion_log_remaindata[ringbuf_count] = fifo_count;
    motion_message_remain--;       //�Ǎ����b�Z�[�W�c�����f�N�������g 
    
    if (fifo_count >= 512){
        //�o�b�t�@�[�͔����ȏ㗭�܂��Ă���
        fifo_count = 0x2000 | fifo_count;   //2:buffer half fill
    //}else {
        //fifo_count = 0x0000 | fifo_count; //0:OK
    }

    ringbuf_count++;
    if (ringbuf_count >= RINGBUF_MAX){
        ringbuf_count = 0;                  //�J�E���g�������O�����Ă���@���l:0~max-1
    }
    
#ifdef  TIMING_LOG          //debug
    DEBUG_timing_log(14);   //MPU6050DataComp
#endif
    
    return fifo_count;
}

uint16_t MPU6050_read_fifo_count(void){
    //FIFO�o�b�t�@�f�[�^��
    uint8_t data[2];
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 114, data, 2);
    return (uint16_t)((data[0] << 8) + data[1]);
}


bool MPU6050_read_fifo_overflow(void){
    //�I�[�o�[�t���[�t���O�`�F�b�N
    //0:no, 1:overflow
    return (I2C1_Read1ByteRegister(MPU6050_ADDRESS, 58) >> 4) & 0x01;
}


bool MPU6050_check_fifo_remain(void){
    //FIFO�o�b�t�@�f�[�^���Ǝc�J�E���^�̃`�F�b�N
    if (motion_message_remain != 0){
        return 0;
    }
    if (MPU6050_read_fifo_count() == MESSAGE_LENGTH){
        motion_message_remain = 1;
        return 1;
    }
    return 0;
}


void MPU6050_fifo_reset(void){
    //FIFO�f�[�^���N���A
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 106, 0x04);    //FIFO disable & reset
    motion_message_remain = 0;                              //�ǂݍ��݃��b�Z�[�W�c���N���A
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 106, 0x40);    //FIFO enable
    
    I2C1_Read1ByteRegister(MPU6050_ADDRESS, 58);            //FIFO_overflow read & reset
    //�I�[�o�[�t���[���荞�݃t���O��FIFOreset�ł̓N���A����Ȃ��̂ŁA���W�X�^�Ǎ��ɂăN���A����K�v������??
    
}


#ifdef  TEMP_ENABLE
void MPU6050_print_temp(void){
    //UART Tx
    
    printf("Temp %6.2f \n", mpu_temp  / (float)(340.0) + 36.53);
}
#endif  //TEMP_ENABLE


#ifdef TEST_YES
void wait_for(uint8_t sec){
    //�h�ꂪ���܂�܂ő҂�
    uint8_t i;
    for (i = sec; i > 0 ; i--){
        sprintf(tmp_string, " Wait for %d sec    ", i);
        LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
        __delay_ms(1000);
    }
    sprintf(tmp_string, "                    ");
    LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1); 
}


void MPU6050_SelfTest_accel(void){
    //�����x�Z���t�e�X�g�l�̓ǂݏo���@(�ŏ����̂�)
    //�����x�f�[�^��100��ǂݏo�����ς���
    //�\���̓Z���T�[��xyz��(���t�����ɂ�莲����������K�v������)
    
    uint8_t     data[6];
    uint8_t     i;
    int16_t     x, y, z;
    int32_t     sumx, sumy, sumz;
    int16_t     avex_on, avey_on, avez_on;
    int16_t     avex_off, avey_off, avez_off;
    int8_t      XA_TEST, YA_TEST, ZA_TEST;
    int16_t     XA_BIAS, YA_BIAS, ZA_BIAS;
    
    LCD_ClearScreen(0);
    sprintf(tmp_string, "*****  SELF TEST ACCEL  ***************");
    LCD_Printf(0, 16, tmp_string, 1, CYAN, 1);
    
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 28, 0x10);     // +-8G

    //Read Self Test Register
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 13, data, 4);       //read Self Test Register
    XA_TEST = ((data[0] >> 3) & 0b00011100) | ((data[3] >> 4) & 0b00000011);
    YA_TEST = ((data[1] >> 3) & 0b00011100) | ((data[3] >> 2) & 0b00000011);
    ZA_TEST = ((data[2] >> 3) & 0b00011100) | (data[3]  & 0b00000011);
    sprintf(tmp_string, "Factory SelfTest Value (+-8G)");
    LCD_Printf(0, 140, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "XA_TEST %2d ", XA_TEST);
    LCD_Printf(0, 150, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "YA_TEST %2d ", YA_TEST);
    LCD_Printf(0, 158, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "ZA_TEST %2d ", ZA_TEST);
    LCD_Printf(0, 166, tmp_string, 1, CYAN, 1);
    
    //Read Accel Bias Register (accel offset cancellations register  +-8G  Factory trim value)
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 6, data, 6);       //read Self Test Register
    XA_BIAS = (data[0] << 8) | data[1];
    YA_BIAS = (data[2] << 8) | data[3];
    ZA_BIAS = (data[4] << 8) | data[5];
    sprintf(tmp_string, "Factory AccelOffset Value (+-8G)");
    LCD_Printf(0, 180, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "XA_BIAS %04x ", XA_BIAS);
    LCD_Printf(0, 190, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "YA_BIAS %04x ", YA_BIAS);
    LCD_Printf(0, 198, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "ZA_BIAS %04x ", ZA_BIAS);
    LCD_Printf(0, 206, tmp_string, 1, CYAN, 1);
    
    wait_for(5);    //�h�ꂪ���܂�܂ő҂�

    //SelfTest Enable
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 28, 0xf0);     //Self Test ACCEL XYZ +-8G
    sprintf(tmp_string, "SelfTest Enable(+-8G)");
    LCD_Printf(0, 30, tmp_string, 1, YELLOW, 1);
    sumx = 0;
    sumy = 0;
    sumz = 0;
    __delay_ms(50);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 59, data, 6);       //read accel 59~64 xh xl yh yl zh zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 40, tmp_string, 1, YELLOW, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex_on = sumx / (int32_t)(i + 1);
        avey_on = sumy / (int32_t)(i + 1);
        avez_on = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex_on, avey_on, avez_on);
        LCD_Printf(0, 50, tmp_string, 1, YELLOW, 1);
    }
    //SelfTest Disable
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 28, 0x10);     //+-8G
    sprintf(tmp_string, "SelfTest Disable(+-8G)");
    LCD_Printf(0, 70, tmp_string, 1, GREEN, 1);
    sumx = 0;
    sumy = 0;
    sumz = 0;
    __delay_ms(50);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 59, data, 6);       //read accel 59~64 xh xl yh yl zh zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 80, tmp_string, 1, GREEN, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex_off = sumx / (int32_t)(i + 1);
        avey_off = sumy / (int32_t)(i + 1);
        avez_off = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex_off, avey_off, avez_off);
        LCD_Printf(0, 90, tmp_string, 1, GREEN, 1);
    }
    //SelfTestResponse
    x = (int32_t)(avex_on) - avex_off;
    y = (int32_t)(avey_on) - avey_off;
    z = (int32_t)(avez_on) - avez_off;
    sprintf(tmp_string, "SelfTestResponse(+-8G)");
    LCD_Printf(0, 110, tmp_string, 1, WHITE, 1);
    sprintf(tmp_string, "            x %04x  y %04x  z %04x", x, y, z);
    LCD_Printf(0, 120, tmp_string, 1, WHITE, 1);
    Stop_until_SW1();                   //SW1���͑҂�
}


void MPU6050_SelfTest_gyro(void){
    //�W���C���Z���t�e�X�g�l�̓ǂݏo���@(�ŏ����̂�)
    //�W���C���f�[�^��100��ǂݏo�����ς���
    //�\���̓Z���T�[��xyz��(���t�����ɂ�莲����������K�v������)
    uint8_t     data[6];
    uint8_t     i;
    int16_t     x, y, z;
    int32_t     sumx, sumy, sumz;
    int16_t     avex_on, avey_on, avez_on;
    int16_t     avex_off, avey_off, avez_off;
    int8_t      XG_TEST, YG_TEST, ZG_TEST;
    int16_t     XG_BIAS, YG_BIAS, ZG_BIAS;
    
    LCD_ClearScreen(0);
    sprintf(tmp_string, "*****  SELF TEST GYRO  ***************");
    LCD_Printf(0, 16, tmp_string, 1, CYAN, 1);
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 27, 0x00);     // +-1000deg/sec

    //Read Self Test Register
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 13, data, 3);       //read Self Test Register
    XG_TEST = (data[0] & 0b00011111);
    YG_TEST = (data[1] & 0b00011111);
    ZG_TEST = (data[2] & 0b00011111);
    sprintf(tmp_string, "Factory SelfTest Value(+-250dps)");
    LCD_Printf(0, 140, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "XG_TEST %2d ", XG_TEST);
    LCD_Printf(0, 150, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "YG_TEST %2d ", YG_TEST);
    LCD_Printf(0, 158, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "ZG_TEST %2d ", ZG_TEST);
    LCD_Printf(0, 166, tmp_string, 1, CYAN, 1);
    
    //Read Accel Bias Register (accel offset cancellations register  +-8G  Factory trim value)
    I2C1_ReadDataBlock(MPU6050_ADDRESS, 19, data, 6);       //read Self Test Register
    XG_BIAS = (data[0] << 8) | data[1];
    YG_BIAS = (data[2] << 8) | data[3];
    ZG_BIAS = (data[4] << 8) | data[5];
    sprintf(tmp_string, "Factory AccelOffset Value(+-1000dps)");
    LCD_Printf(0, 180, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "XA_BIAS %04x ", XG_BIAS);
    LCD_Printf(0, 190, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "YA_BIAS %04x ", YG_BIAS);
    LCD_Printf(0, 198, tmp_string, 1, CYAN, 1);
    sprintf(tmp_string, "ZA_BIAS %04x ", ZG_BIAS);
    LCD_Printf(0, 206, tmp_string, 1, CYAN, 1);
    
    wait_for(5);        //�h�ꂪ���܂�܂ő҂�

    //SelfTest Enable
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 27, 0xe0);     //Self Test GYRO XYZ +-250dps
    sprintf(tmp_string, "SelfTest Enable(+-250dps)");
    LCD_Printf(0, 30, tmp_string, 1, YELLOW, 1);
    sumx = 0;
    sumy = 0;
    sumz = 0;
    __delay_ms(50);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 67, data, 6);       //read gyro 67-72 xh,xl,yh,yl,zh,zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 40, tmp_string, 1, YELLOW, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex_on = sumx / (int32_t)(i + 1);
        avey_on = sumy / (int32_t)(i + 1);
        avez_on = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex_on, avey_on, avez_on);
        LCD_Printf(0, 50, tmp_string, 1, YELLOW, 1);
    }
    //SelfTest Disable
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 27, 0x00);     //+-250dps
    sprintf(tmp_string, "SelfTest Disable(+-250dps)");
    LCD_Printf(0, 70, tmp_string, 1, GREEN, 1);
    sumx = 0;
    sumy = 0;
    sumz = 0;
    __delay_ms(50);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 67, data, 6);       //read gyro 67-72 xh,xl,yh,yl,zh,zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 80, tmp_string, 1, GREEN, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex_off = sumx / (int32_t)(i + 1);
        avey_off = sumy / (int32_t)(i + 1);
        avez_off = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex_off, avey_off, avez_off);
        LCD_Printf(0, 90, tmp_string, 1, GREEN, 1);
    }
    //SelfTestResponse
    x = (int32_t)(avex_on) - avex_off;
    y = (int32_t)(avey_on) - avey_off;
    z = (int32_t)(avez_on) - avez_off;
    sprintf(tmp_string, "SelfTestResponse(+-250dps)");
    LCD_Printf(0, 110, tmp_string, 1, WHITE, 1);
    sprintf(tmp_string, "            x %04x  y %04x  z %04x", x, y, z);
    LCD_Printf(0, 120, tmp_string, 1, WHITE, 1);
    Stop_until_SW1();                           //SW1���͑҂�
}


void MPU6050_offset_accel(void){
    //�����x�����I�t�Z�b�g�␳�l�����߂�@(�ŏ����̂�)
    uint8_t     data[6];
    uint8_t     i;
    int16_t     x, y, z;
    int32_t     sumx = 0, sumy = 0, sumz = 0;
    int16_t     avex, avey, avez;
    
    LCD_ClearScreen(0);
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 28, 0x00);             //0x00:Accel config  +-2G  ////////0x08 +-4G, 0x10 +-8G, 0x18 +-16G
    sprintf(tmp_string, "ACCEL OFFSET +-2G");
    LCD_Printf(0, 30, tmp_string, 1, YELLOW, 1);
    sprintf(tmp_string, "sensor x+, y+, z+ -> axis  +Z, +Y, -X"  );
    LCD_Printf(0, 40, tmp_string, 1, MAGENTA, 1);
    Stop_until_SW1();                                         //SW1���͑҂�
    
    wait_for(6);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 59, data, 6);       //read accel 59~64 xh xl yh yl zh zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 60, tmp_string, 1, GREEN, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex = sumx / (int32_t)(i + 1);
        avey = sumy / (int32_t)(i + 1);
        avez = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex, avey, avez);
        LCD_Printf(0, 70, tmp_string, 1, GREEN, 1);
    }
    sprintf(tmp_string, "            x %4d  y %4d  z %4d", avex, avey, avez);
    LCD_Printf(0, 90, tmp_string, 1, WHITE, 1);
    Stop_until_SW1();                                         //SW1���͑҂�
}

    
void MPU6050_offset_gyro(void){
    //�W���C�������␳�l(�N����)
    uint8_t     data[6];
    uint8_t     i;
    int16_t     x, y, z;
    int32_t     sumx = 0, sumy = 0, sumz = 0;
    int16_t     avex, avey, avez;
    
    //LCD_ClearScreen(0);
    I2C1_Write1ByteRegister(MPU6050_ADDRESS, 27, 0x00);             //Gyro config +-250deg/sec
    sprintf(tmp_string, "GYRO OFFSET +-250deg/sec");
    LCD_Printf(0, 130, tmp_string, 1, YELLOW, 1);
    sprintf(tmp_string, "sensor x+, y+, z+ -> axis  -Z, -Y, -X"  );
    LCD_Printf(0, 140, tmp_string, 1, MAGENTA, 1);

    wait_for(6);
    for (i = 0; i < 100; i++){
        I2C1_ReadDataBlock(MPU6050_ADDRESS, 67, data, 6);       //read gyro 67~72 xh xl yh yl zh zl
        x =  (data[0] << 8) + data[1];
        y =  (data[2] << 8) + data[3];
        z =  (data[4] << 8) + data[5];
        sprintf(tmp_string, " read %03d   x %04x  y %04x  z %04x", (i + 1), x, y, z);
        LCD_Printf(0, 160, tmp_string, 1, GREEN, 1);
        sumx = sumx + (int32_t)x;
        sumy = sumy + (int32_t)y;
        sumz = sumz + (int32_t)z;
        avex = sumx / (int32_t)(i + 1);
        avey = sumy / (int32_t)(i + 1);
        avez = sumz / (int32_t)(i + 1);
        sprintf(tmp_string, " average    x %04x  y %04x  z %04x", avex, avey, avez);
        LCD_Printf(0, 170, tmp_string, 1, GREEN, 1);
    }
    sprintf(tmp_string, "            x %4d  y %4d  z %4d", avex, avey, avez);
    LCD_Printf(0, 190, tmp_string, 1, WHITE, 1);
    Stop_until_SW1();                                         //SW1���͑҂�
}

#endif  //TEST_YES