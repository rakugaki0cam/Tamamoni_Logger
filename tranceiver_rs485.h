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


//data_uart_receive　戻り値
typedef enum {
    RX_OK,          //正常完了
    RX_READING,     //受信中
    CALC_ERROR,     //計算結果がエラー        
    RX_ERROR,       //エラー
} data_rx_status_t;


data_rx_status_t data_uart_receive(float*);
void    rx_buffer_clear(void);


#endif //TRANCEIVER_RS485_H
