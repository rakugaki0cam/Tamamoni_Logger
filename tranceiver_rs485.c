/*  tranceiver_rs485.h
 * 
 * �d�q�^�[�Q�b�g�Ƃ̒ʐM
 * 
 * V8
 * 
 * 
 *  2022.04.24
 *
 * 
*/
#include "header.h"
#include "tranceiver_rs485.h"


#define LEN_TMP     60
#define LEN_MES     41      //��M���b�Z�[�W��

char    tmp_str[LEN_TMP];


data_rx_status_t data_uart_receive(float *data){
    //�d�q�^�[�Q�b�g���璅�e�f�[�^����M
    
    //�f�[�^�����X�e�[�^�X
    typedef enum {
        START_UART_B,
        START_UART_I,
        START_UART_N,
        READ_UART,
        DATA_SCANF,
        COMPLETE_UART,
    } rx_status_t;
    
    static rx_status_t  status = START_UART_B;
    static uint8_t  n = 0;
    
    data_rx_status_t    ans;        //�߂�l
    int16_t             num_scan;   //��M�Z�b�g�l�̐�
    

    //�m���u���b�L���O�Ή�
    switch(status){
        case START_UART_B:
            //�ŏ���B
            if (UART2_is_rx_ready()){
                tmp_str[n] = UART2_Read();
                if ('B' == tmp_str[n]){
                    n++;
                    status = START_UART_I;
                    break;
                }
            }
            ans = RX_READING;
            break;
            
        case START_UART_I:
            //�񕶎��ڂ�I
            if (UART2_is_rx_ready()){
                tmp_str[n] = UART2_Read();
                if ('I' == tmp_str[n]){
                    n++;
                    status = START_UART_N;
                    break;
                }else {
                    n = 0;
                    status = START_UART_B;
                }
            }
            ans = RX_READING;
            break;
            
        case START_UART_N:
            //�O�����ڂ�N
            if (UART2_is_rx_ready()){
                tmp_str[n] = UART2_Read();
                if ('N' == tmp_str[n]){
                    n++;
                    status = READ_UART;
                    break;
                }else {
                    n = 0;
                    status = START_UART_B;
                }
            }
            ans = RX_READING;
            break;
            
        case READ_UART:
            //�f�[�^�̓ǂݍ���
            if (UART2_is_rx_ready()){
                tmp_str[n] = UART2_Read();
                n++;
                if (n > LEN_MES){
                    //�ǂݍ��݊���
                    n = 0;
                    status = DATA_SCANF;
                }
            }
            ans = RX_READING;
            break;
            
        case DATA_SCANF:
            //�f�[�^�̑��
            num_scan = sscanf(tmp_str, "BINX0Y0dT %f %f %f END", &data[0], &data[1], &data[2]);
            //�w�b�_�A�t�b�^����v���Ȃ��ꍇ�A�l����������������Ȃ��Ȃ�
            if (num_scan != 3){
                //���e�f�[�^���G���[�̎�
                ans = RX_ERROR;
            }else if(data[2] == 0){
                ans = CALC_ERROR;
            }else{
                //�f�[�^���OK
                ans = RX_OK;
            }
            status = COMPLETE_UART;
            //break;    //�����Ńu���[�N����ƃf�o�b�K�ւ̕\�����o�Ȃ�
            
        case COMPLETE_UART:
            //�I������
#define DEBUG_RECEIVE_DATA
#ifndef DEBUG_RECEIVE_DATA
            printf("\n");
            printf("rx_data=%d\n", num_scan);
            printf("x: %f\n", data[0]);
            __delay_ms(30);
            printf("y: %f\n", data[1]);
            printf("dt:%f\n", data[2]);
            printf("\n");
#endif 
            status = START_UART_B;     //�ŏ��֖߂�
            break;
    }
    
    return ans;
}


void    rx_buffer_clear(void){
    //��M�o�b�t�@���N���A
    uint8_t     i;
    
    for (i = 0; i < LEN_TMP; i++){
        tmp_str[i] = 0;
    }
    
    __delay_ms(100);/////////�}�g�����f�o�b�O�p�f�[�^���o���I��鍠�܂Œx��
    
    while(UART2_is_rx_ready()){
        UART2_Read();               //�̂ēǂ�
    }
}

