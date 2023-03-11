/*  tranceiver_uart.c
 * 
 * 電子ターゲットとの通信
 * 
 * 2022.04.24   V8
 * 2023.02.02   V9.0 UART2 - ESP32 UART ESP-NOW WiFi
 *                   UART4 - RS485 UART LAN
 * 2023.02.18   コマンド送信を追加
 * 2023.02.13   RS485とESP-NOW 共用化
 *                     
 */

#include "header.h"
#include "tranceiver_uart.h"


#define LEN_TMP     80
#define LEN_MES     41      //受信メッセージ長

char    tmp_str[LEN_TMP + 1];



data_rx_status_t data_uart_receive(float *data){
    //電子ターゲットから着弾データを受信
    
    //データ処理ステータス
    typedef enum {
        START_UART_B,
        START_UART_I,
        START_UART_N,
        READ_UART,
        DATA_SCANF,
    } rx_status_t;
    
    static rx_status_t  status = START_UART_B;
    static uint8_t      n = 0;
    data_rx_status_t    ans;        //戻り値
    int16_t             num_scan;   //受信セット値の数
    

    //ノンブロッキング対応
    switch(status){
        case START_UART_B:
            //最初のB
            if (uart_rx(n)){
#define WIFI_ONLY_DEBUG     //*************************** DEBUG中　　LANつなぐとシリアルデバッグ不可     
#ifdef  WIFI_ONLY_DEBUG
                if (ESP_NOW == target_com_path){
                    printf("1st chr:'%c'\n", tmp_str[n]);  //LANケーブルの時にはターゲット側のESP32までコマンド扱いで渡ってしまいループする -> 絶対ダメ!!!
                }                               //でもWiFiのみのときはデバッグには使える
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
            //二文字目のI
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
            //三文字目のN
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
            //データの読み込み
            if (uart_rx(n)){
                n++;
                if ((',' == tmp_str[n - 1]) || (n > LEN_MES)){
                    //読み込み完了
                    tmp_str[n] = 0;     //文字列のエンドマークを追加
#ifdef  WIFI_ONLY_DEBUG
                    if (ESP_NOW == target_com_path){
                        printf("UART2 read data:'%s'\n", tmp_str);  //LANケーブルの時にはターゲット側のESP32までコマンド扱いで渡ってしまいループする -> 絶対ダメ!!!
                    }                                   //でもWiFiのみのときはデバッグには使える
#endif                    
                    status = DATA_SCANF;
                }
            }
            ans = RX_READING;
            break;
            
        case DATA_SCANF:
            //データの代入
            num_scan = sscanf(tmp_str, "BINX0Y0dT %f %f %f END ,", &data[0], &data[1], &data[2]);
            //ヘッダ、フッタが一致しない場合、値を代入した数が合わなくなる
            if (num_scan != 3){
                //着弾データがエラーの時
                ans = RX_ERROR;
            }else if(data[2] == 0){
                //計算がエラーだった時
                ans = CALC_ERROR;
            }else{
                //データ代入OK
                ans = RX_OK;
            }
            //終了処理
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
            status = START_UART_B;     //最初へ戻る
            break;
    }
    
    return ans;
}


uint8_t uart_rx(uint8_t n){
    // GLOBAL - target_com_path
    //tmp_str(n)に代入
    
    if (RS485 == target_com_path){
        //LANケーブルRS485から受信
        if (UART4_is_rx_ready()){
            tmp_str[n] = UART4_Read();
            return 1;
        }
    }else if (ESP_NOW == target_com_path){
        //ESP32のESP-NOWから受信
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
    //受信バッファをクリア
    uint24_t    i;
    uint8_t     skip_mes;
    
    //esp32 115200bps
#ifdef  WIFI_ONLY_DEBUG
    if(UART2_is_rx_ready()){
        printf("\n--Rx buffer clear---\n");
        i = 0;
        while(UART2_is_rx_ready()){
            skip_mes = UART2_Read();    //捨て読み
            if (ESP_NOW == target_com_path){
                printf("%c", skip_mes);    //LANケーブルの時にはターゲット側のESP32までコマンド扱いで渡ってしまいループする -> 絶対ダメ!!!
            }                               //でもWiFiのみのときはデバッグには使える
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
        skip_mes = UART2_Read();    //捨て読み
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
        UART4_Read();           //捨て読み
        i++;
        if (i > 250){
            //timeout;
            break;
        }
    }
    
}


void    command_uart_send(uint8_t* command){
    //コマンドをターゲットへ送る
    uint8_t* str;
    sprintf(tmp_str, "TARGET_%s END ,", command); //","が読み込みエンドマーク
    str = (uint8_t*)tmp_str;
    
    while(*str){
        //データ送信
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

