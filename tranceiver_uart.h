/*  tranceiver_uart.h
 * 
 * �d�q�^�[�Q�b�g�Ƃ̒ʐM
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TRANCEIVER_UART_H
#define TRANCEIVER_UART_H


//data_uart_receive�@�߂�l
typedef enum {
    RX_OK,          //���튮��
    RX_READING,     //��M��
    CALC_ERROR,     //�v�Z���ʂ��G���[        
    RX_ERROR,       //�G���[
} data_rx_status_t;


data_rx_status_t data_uart_receive(float*);
uint8_t uart_rx(uint8_t);
void    rx_buffer_clear(void);
void    command_uart_send(uint8_t*);

#endif //TRANCEIVER_UART_H
