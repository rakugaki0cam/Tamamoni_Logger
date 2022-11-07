//
//  RTClock_8900.h
//
//  2020.07.30
//

#ifndef RTCLOCK_RX8900_H
#define RTCLOCK_RX8900_H


#define     RX8900_ADDRESS     0x32     //I2C接続 DTCXO内蔵　RTCモジュール

//グローバル変数
extern uint32_t     RTC_timing;         //センサー1オンから直後の測定値更新までの時間
extern bool         RTC_int_flag;       //時刻更新割込フラグ       
extern uint16_t     sleep_count;        //スリープ用タイマーカウント
extern int8_t       low_battery_count;  //低電圧検出  @hlvd.c


uint8_t     RTC_initialize(void);
void        RTC_interrupt(void);
void        RTC8900_time_get(uint8_t *);
void        RTC_disp(bool);
void        RTC_get_date(uint8_t *);
void        RTC_set(void);
void        RTC8900_time_set(void);
uint8_t     youbi(uint8_t, uint8_t, uint8_t);

#endif // RTCLOCK_RX8900_H //