/*  tranceiver_rs485.c
 * 
 * 電子ターゲットとの通信
 * 
 * 2022.04.24   V8
 * 2023.02.01   V9.0からUART2->UART4
 * 
 * 
 * 2023.02.18 コマンド送信を追加
 *  
 */
#include "header.h"
#include "tranceiver_rs485.h"


#define LEN_TMP     60
#define LEN_MES     41      //受信メッセージ長

char    tmp_str[LEN_TMP];


data_rx_status_t data_uart_receive_rs485(float *data){
    //電子ターゲットから着弾データを受信
    
    //データ処理ステータス
    typedef enum {
        START_UART_B,
        START_UART_I,
        START_UART_N,
        READ_UART,
        DATA_SCANF,
        COMPLETE_UART,
    } rx_status_t;
    
    static rx_status_t  status = START_UART_B;
    static uint8_t      n = 0;
    data_rx_status_t    ans;        //戻り値
    int16_t             num_scan;   //受信セット値の数
    

    //ノンブロッキング対応
    switch(status){
        case START_UART_B:
            //最初のB
            if (UART4_is_rx_ready()){
                tmp_str[n] = UART4_Read();
                if ('B' == tmp_str[n]){
                    n++;
                    status = START_UART_I;
                    break;
                }
            }
            ans = RX_READING;
            break;
            
        case START_UART_I:
            //二文字目のI
            if (UART4_is_rx_ready()){
                tmp_str[n] = UART4_Read();
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
            if (UART4_is_rx_ready()){
                tmp_str[n] = UART4_Read();
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
            if (UART4_is_rx_ready()){
                tmp_str[n] = UART4_Read();
                n++;
                if (n > LEN_MES){
                    //読み込み完了
                    n = 0;
                    status = DATA_SCANF;
                }
            }
            ans = RX_READING;
            break;
            
        case DATA_SCANF:
            //データの代入
            num_scan = sscanf(tmp_str, "BINX0Y0dT %f %f %f END", &data[0], &data[1], &data[2]);
            //ヘッダ、フッタが一致しない場合、値を代入した数が合わなくなる
            if (num_scan != 3){
                //着弾データがエラーの時
                ans = RX_ERROR;
            }else if(data[2] == 0){
                ans = CALC_ERROR;
            }else{
                //データ代入OK
                ans = RX_OK;
            }
            status = COMPLETE_UART;
            //break;    //ここでブレークするとデバッガへの表示が出ない
            
        case COMPLETE_UART:
            //終了処理
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
            status = START_UART_B;     //最初へ戻る
            break;
    }
    
    return ans;
}


void    rx_buffer_clear_rs485(void){
    //受信バッファをクリア
    uint8_t     i;
    
    for (i = 0; i < LEN_TMP; i++){
        tmp_str[i] = 0;
    }
    
    __delay_ms(100);/////////マト側がデバッグ用データを出し終わる頃まで遅延
    
    while(UART4_is_rx_ready()){
        UART4_Read();               //捨て読み
    }
}


void    command_uart_send_rs485(uint8_t* command){
    //コマンドをLANケーブルでターゲットに送る
    uint8_t* str;
    sprintf(tmp_str, "TARGET_%s END\n", command);
    str = (uint8_t*)tmp_str;
    while(*str){
        while(!UART4_is_tx_ready());
        UART4_Write(*str);  //データ送信
        str++;
    }
}
