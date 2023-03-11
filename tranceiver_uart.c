/*  tranceiver_uart.c
 * 
 * �d�q�^�[�Q�b�g�Ƃ̒ʐM
 * 
 * 2022.04.24   V8
 * 2023.02.02   V9.0 UART2 - ESP32 UART ESP-NOW WiFi
 *                   UART4 - RS485 UART LAN
 * 2023.02.18   �R�}���h���M��ǉ�
 * 2023.02.13   RS485��ESP-NOW ���p��
 *                     
 */

#include "header.h"
#include "tranceiver_uart.h"


#define LEN_TMP     80
#define LEN_MES     41      //��M���b�Z�[�W��

char    tmp_str[LEN_TMP + 1];



data_rx_status_t data_uart_receive(float *data){
    //�d�q�^�[�Q�b�g���璅�e�f�[�^����M
    
    //�f�[�^�����X�e�[�^�X
    typedef enum {
        START_UART_B,
        START_UART_I,
        START_UART_N,
        READ_UART,
        DATA_SCANF,
    } rx_status_t;
    
    static rx_status_t  status = START_UART_B;
    static uint8_t      n = 0;
    data_rx_status_t    ans;        //�߂�l
    int16_t             num_scan;   //��M�Z�b�g�l�̐�
    

    //�m���u���b�L���O�Ή�
    switch(status){
        case START_UART_B:
            //�ŏ���B
            if (uart_rx(n)){
#define WIFI_ONLY_DEBUG     //*************************** DEBUG���@�@LAN�Ȃ��ƃV���A���f�o�b�O�s��     
#ifdef  WIFI_ONLY_DEBUG
                if (ESP_NOW == target_com_path){
                    printf("1st chr:'%c'\n", tmp_str[n]);  //LAN�P�[�u���̎��ɂ̓^�[�Q�b�g����ESP32�܂ŃR�}���h�����œn���Ă��܂����[�v���� -> ��΃_��!!!
                }                               //�ł�WiFi�݂̂̂Ƃ��̓f�o�b�O�ɂ͎g����
#endif     
                if ('B' == tmp_str[n]){
                    n++;
                    status = START_UART_I;
                    break;
                }else {
                    n = 0;
                    status = START_UART_B;
                }
            }
            ans = RX_READING;
            break;
            
        case START_UART_I:
            //�񕶎��ڂ�I
            if (uart_rx(n)){
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
            if (uart_rx(n)){
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
            if (uart_rx(n)){
                n++;
                if ((',' == tmp_str[n - 1]) || (n > LEN_MES)){
                    //�ǂݍ��݊���
                    tmp_str[n] = 0;     //������̃G���h�}�[�N��ǉ�
#ifdef  WIFI_ONLY_DEBUG
                    if (ESP_NOW == target_com_path){
                        printf("UART2 read data:'%s'\n", tmp_str);  //LAN�P�[�u���̎��ɂ̓^�[�Q�b�g����ESP32�܂ŃR�}���h�����œn���Ă��܂����[�v���� -> ��΃_��!!!
                    }                                   //�ł�WiFi�݂̂̂Ƃ��̓f�o�b�O�ɂ͎g����
#endif                    
                    status = DATA_SCANF;
                }
            }
            ans = RX_READING;
            break;
            
        case DATA_SCANF:
            //�f�[�^�̑��
            num_scan = sscanf(tmp_str, "BINX0Y0dT %f %f %f END ,", &data[0], &data[1], &data[2]);
            //�w�b�_�A�t�b�^����v���Ȃ��ꍇ�A�l����������������Ȃ��Ȃ�
            if (num_scan != 3){
                //���e�f�[�^���G���[�̎�
                ans = RX_ERROR;
            }else if(data[2] == 0){
                //�v�Z���G���[��������
                ans = CALC_ERROR;
            }else{
                //�f�[�^���OK
                ans = RX_OK;
            }
            //�I������
#define DEBUG_RECEIVE_DATA_no
#ifdef  DEBUG_RECEIVE_DATA
            //__delay_ms(3);
            printf("\n");
            printf("rx_data=%d\n", num_scan);
            printf("x: %f\n", data[0]);
            __delay_ms(30);
            printf("y: %f\n", data[1]);
            printf("dt:%f\n", data[2]);
            printf("\n");
#endif 
            n = 0;
            status = START_UART_B;     //�ŏ��֖߂�
            break;
    }
    
    return ans;
}


uint8_t uart_rx(uint8_t n){
    // GLOBAL - target_com_path
    //tmp_str(n)�ɑ��
    
    if (RS485 == target_com_path){
        //LAN�P�[�u��RS485�����M
        if (UART4_is_rx_ready()){
            tmp_str[n] = UART4_Read();
            return 1;
        }
    }else if (ESP_NOW == target_com_path){
        //ESP32��ESP-NOW�����M
        if (UART2_is_rx_ready()){
            tmp_str[n] = UART2_Read();
            return 1;
        }
    }else{
        //printf("TARGET path error!\n");
    }
    return 0;
}


void    rx_buffer_clear(void){
    //��M�o�b�t�@���N���A
    uint24_t    i;
    uint8_t     skip_mes;
    
    //esp32 115200bps
#ifdef  WIFI_ONLY_DEBUG
    if(UART2_is_rx_ready()){
        printf("\n--Rx buffer clear---\n");
        i = 0;
        while(UART2_is_rx_ready()){
            skip_mes = UART2_Read();    //�̂ēǂ�
            if (ESP_NOW == target_com_path){
                printf("%c", skip_mes);    //LAN�P�[�u���̎��ɂ̓^�[�Q�b�g����ESP32�܂ŃR�}���h�����œn���Ă��܂����[�v���� -> ��΃_��!!!
            }                               //�ł�WiFi�݂̂̂Ƃ��̓f�o�b�O�ɂ͎g����
            i++;
            if (i > 250){
                //timeout;
                break;
            }
        }
        if (ESP_NOW == target_com_path){
            printf("\n--------------------\n");
        }
    }
#else
    i = 0;
    while(UART2_is_rx_ready()){
        skip_mes = UART2_Read();    //�̂ēǂ�
        i++;
        if (i > 64){
            //timeout;
            break;
        }
    }
#endif   
    
    
    //rs485 9600bps
    if (UART4_is_rx_ready()){
        printf("--LAN buffer clear--\n\n");
    }
    i = 0;
    while(UART4_is_rx_ready()){
        UART4_Read();           //�̂ēǂ�
        i++;
        if (i > 250){
            //timeout;
            break;
        }
    }
    
}


void    command_uart_send(uint8_t* command){
    //�R�}���h���^�[�Q�b�g�֑���
    uint8_t* str;
    sprintf(tmp_str, "TARGET_%s END ,", command); //","���ǂݍ��݃G���h�}�[�N
    str = (uint8_t*)tmp_str;
    
    while(*str){
        //�f�[�^���M
        if (RS485 == target_com_path){
            while(!UART4_is_tx_ready());
            UART4_Write(*str);
        }else if (ESP_NOW == target_com_path){
            while(!UART2_is_tx_ready());
            UART2_Write(*str);
        }else{
        //printf("TARGET path error!\n");
        }
        
        str++;
    }
}

