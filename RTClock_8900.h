//
//  RTClock_8900.h
//
//  2020.07.30
//

#ifndef RTCLOCK_RX8900_H
#define RTCLOCK_RX8900_H


#define     RX8900_ADDRESS     0x32     //I2C�ڑ� DTCXO�����@RTC���W���[��

//�O���[�o���ϐ�
extern uint32_t     RTC_timing;         //�Z���T�[1�I�����璼��̑���l�X�V�܂ł̎���
extern bool         RTC_int_flag;       //�����X�V�����t���O       
extern uint16_t     sleep_count;        //�X���[�v�p�^�C�}�[�J�E���g
extern int8_t       low_battery_count;  //��d�����o  @hlvd.c


uint8_t     RTC_initialize(void);
void        RTC_interrupt(void);
void        RTC8900_time_get(uint8_t *);
void        RTC_disp(bool);
void        RTC_get_date(uint8_t *);
void        RTC_set(void);
void        RTC8900_time_set(void);
uint8_t     youbi(uint8_t, uint8_t, uint8_t);

#endif // RTCLOCK_RX8900_H //