/*  tranceiver_rs485.h
 * 
 * �d�q�^�[�Q�b�g�Ƃ̒ʐM
 * 
 * 
 * 
 *  2022.04.24
 *
 * 
*/

#ifndef TRANCEIVER_RS485_H
#define TRANCEIVER_RS485_H


//data_uart_receive�@�߂�l
typedef enum {
    RX_OK,          //���튮��
    RX_READING,     //��M��
    RX_ERROR,       //�G���[
} data_rx_status_t;


data_rx_status_t data_uart_receive(float*);
void    rx_buffer_clear(void);


#endif //TRANCEIVER_RS485_H
