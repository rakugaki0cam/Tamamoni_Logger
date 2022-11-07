/*  MPU6050.h
 * 
 *      Accelerometer
 *      Gyroscope
 *
 *      6-axis sensor     
 * 
 *  2021.01.20 
 * ���[�V���������𕪗�
 * 
*/

#ifndef MPU6050_H
#define MPU6050_H

//�Z���T�[�̎��ƌ����̐ݒ�(IC�̎�t�����ɂ��)
#define     WORLD_X             2   //X��:sensorz  
#define     WORLD_Y             1   //Y��:sensory
#define     WORLD_Z             0   //Z��:sensorx
#define     DIRECTION_AX        -1
#define     DIRECTION_AY        1
#define     DIRECTION_AZ        1
#define     DIRECTION_GX        -1
#define     DIRECTION_GY        -1
#define     DIRECTION_GZ        -1


//�O���[�o���ϐ�
extern int16_t     rawdata_accel[3][RINGBUF_MAX + 1];   //G�f�[�^�̃X�g�A�@�@�@�@�@@motion.c
extern int16_t     rawdata_gyro[3][RINGBUF_MAX + 1];    //�W���C���f�[�^�̃X�g�A�@�@@motion.c
extern uint16_t    motion_log_remaindata[];             //�Z���T�[FIFO�f�[�^�c���@16bit 0�`1024�o�C�g
extern uint16_t    sample_period;          //MPU6050 �T���v�����O�Ԋu usec   @measure.c�@@motion.c
extern int16_t     accel_offset[];         //G�I�t�Z�b�g�l                  @motion.c
extern int16_t     accel_conv[];           //G�Q�C��                       @motion.c
extern uint16_t    accel_DLPF_delay;       //G DigitalLowPassFilter�̃f�B���C  @motion.c
extern int16_t     gyro_offset[];          //�W���C���I�t�Z�b�g�l              @motion.c
extern int16_t     gyro_conv[];            //�W���C���Q�C��                   @motion.c
extern uint16_t    gyro_DLPF_delay;        //�W���C���@DLPF�̃f�B���C          @motion.c
extern uint8_t     ringbuf_count;          //�����x�f�[�^�����O�o�b�t�@�J�E���g @measure.c�@@motion.c


bool        MPU6050_initialize(void);
void        MPU6050_interrupt(void);
void        MPU6050_read_rawdata(void);
uint16_t    MPU6050_read_fifo(void);
uint16_t    MPU6050_read_fifo_count(void);
bool        MPU6050_read_fifo_overflow(void);
bool        MPU6050_check_fifo_remain(void);
void        MPU6050_fifo_reset(void);
void        MPU6050_print_temp(void);

// test
void        wait_for(uint8_t);
void        MPU6050_SelfTest_accel(void);
void        MPU6050_SelfTest_gyro(void);
void        MPU6050_offset_accel(void);
void        MPU6050_offset_gyro(void);

#endif //MPU6050_H