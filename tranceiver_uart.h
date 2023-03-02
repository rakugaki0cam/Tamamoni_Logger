/*  tranceiver_uart.h
 * 
 * 電子ターゲットとの通信
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TRANCEIVER_UART_H
#define TRANCEIVER_UART_H


//data_uart_receive　戻り値
typedef enum {
    RX_OK,          //正常完了
    RX_READING,     //受信中
    CALC_ERROR,     //計算結果がエラー        
    RX_ERROR,       //エラー
} data_rx_status_t;


data_rx_status_t data_uart_receive(float*);
uint8_t uart_rx(uint8_t);
void    rx_buffer_clear(void);
void    command_uart_send(uint8_t*);

#endif //TRANCEIVER_UART_H
