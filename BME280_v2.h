/* 
 * File:   BME280_v2.h
 * Author: IsobeR
 * Comments: PIC18F25K80 MCC
 * 
 * BOSCH BME280 温度　湿度　気圧センサー　モジュール　I2C
 * 
 * 3.3V
 * 
 * Revision history: 
 * 2019.02.11
 * _v2
 * 2021.09.26
 * 
 */

#ifndef BME280_V2_H
#define	BME280_V2_H


unsigned char   BME280_initialize(void);
uint8_t         BME280_disp(bool);
void            BME280_frame_disp(void);
void            Trim_Read(void);
signed long     compensate_T(signed long);
unsigned long   compensate_P(signed long);
unsigned long   compensate_H(signed long);


#endif //BME280__V2H