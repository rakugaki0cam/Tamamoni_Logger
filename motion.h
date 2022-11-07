/*  motion.h
 * 
 *  motion data process
 * 
 *  2021.01.20
 * MPU6050��胂�[�V�����������𕪗�
 * 
*/

#ifndef MOTION_H
#define MOTION_H


//motion log �L�^�f�[�^���Ɋւ������
#define     SHIFT               4       //����Z�̕ς��Ƀr�b�g�V�t�g���邽�߂̒l
#define     SAMPLE              16      //2�O4
#define     RINGBUF_MAX         SAMPLE  //�����O�o�b�t�@=�f�[�^�ۑ��o�C�g�� �@�Î~��Ԃ̓Z���T1�I����20msec�ȏ�O���̗p
#define     AFTER_SHOT_COUNT    7       //��������ɋL�^����f�[�^��(���O�̃f�[�^����16-7=9�ɂȂ�)
//#define     FIFO_OVERFLOW       0x1000  //�I�[�o�[�t���[�̎�

#define     LONG_TIME_RECORD_no
#ifdef      LONG_TIME_RECORD            //�u���[�o�b�N���쓙�̒����Ԃ��L�^�������ꍇ
    #define RINGBUF_MAX         128
    #define AFTER_SHOT_COUNT    (RINGBUF_MAX - 8)
#endif


//�O���[�o���ϐ�
extern uint8_t     motion_message_remain;               //���[�V�������b�Z�[�W�ǂݍ��ݎc��
extern uint8_t     motion_gate;                         //���[�V�����Z���T�@�f�[�^�X�V�c��  Vmeasure2.c
extern uint32_t    motion_timer_offset;                 //�Z���T1�I���ƃ��[�V�����f�[�^�ǂݍ��݃^�C�~���O�̃Y��(�^�C�}�[�l) @measure.c @motion.c
extern int32_t     sum_accel_v2[], sum_gyro_v2[];       //����v2�p
extern uint8_t     sum_count_v2;                     //���ς��邽�߂̃T���v����


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