/*  tranceiver_esp32.h
 * 
 * 電子ターゲットとの通信
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TRANCEIVER_ESP32_H
#define TRANCEIVER_ESP32_H



data_rx_status_t data_uart_receive_esp32(float*);
void    rx_buffer_clear_esp32(void);
void    command_uart_send_esp32(uint8_t*);

#endif //TRANCEIVER_ESP325_H
