/*
 *  esp32_i2c_slave.h
 * 
 *  2024.05.13
 */

#ifndef ESP32SLAVE_H
#define ESP32SLAVE_H


#include <xc.h> // include processor files - each processor file is guarded.  


bool    ESP32slave_Init(void);
//command
void    ESP32slave_ClearCommand(void);
void    ESP32slave_ResetCommand(void);
void    ESP32slave_DefaultSetCommand(void); 
bool    ESP32slave_OffSetCommand(float);
bool    ESP32slave_AimpointCommand(float);
bool    ESP32slave_BrightnessCommand(float);
//send data
void    ESP32slave_SendImpactData(uint8_t*, uint8_t);
void    ESP32slave_SendPT1Status(uint8_t);
void    ESP32slave_SendMessage(uint8_t*);

//sleep
void    ESP32slave_SleepCommand(void);


#endif	/* ESP32SLAVE_H */

