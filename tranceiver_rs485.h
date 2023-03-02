/*  tranceiver_rs485.h
 * 
 * 電子ターゲットとの通信
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TRANCEIVER_RS485_H
#define TRANCEIVER_RS485_H



data_rx_status_t data_uart_receive_rs485(float*);
void    rx_buffer_clear_rs485(void);
void    command_uart_send_rs485(uint8_t*);

#endif //TRANCEIVER_RS485_H
