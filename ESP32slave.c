/*
 * File:   ESP32slave.c
 * Comments: PIC18Q
 * 
 * ESP32 TAMAMONI　　I2C slave
 * 
 * 3.3V I2C
 * 
 * Revision history: 
 * 2024.05.13
 * 
 */

#include "header.h"
#include "ESP32slave.h"

//DEBUGger printf (Global...header.h)
//#define DEBUG_ESP_SLAVE_0_no    //Debug用printf(エラー関連)
//#define DEBUG_ESP_SLAVE_2_no    //Debug用printf(バッテリ電圧)
//#define DEBUG_ESP_SLAVE_3_no    //Debug用printf(送信データ)

//return value
#define OK 0
#define ERROR 1


#define ESP_SLAVE_ID      0x26  //I2C ID ESP32 TAMAMONI
//register address
typedef enum  {
    REG_ID_CHECK        = 0x01, //ID確認　返答"ESP"
    REG_TARGET_CLEAR    = 0x20, //ターゲットクリアコマンド
    REG_TARGET_RESET    = 0x21, //ターゲットリセットコマンド
    REG_TARGET_DEFAULT  = 0x22, //ターゲットデフォルトセットコマンド
    REG_TARGET_OFFSET   = 0x23, //ターゲットYオフセットコマンド
    REG_TARGET_AIMPOINT = 0x24, //ターゲット狙点セットコマンド
    REG_TARGET_BRIGHT   = 0x25, //ターゲット輝度セットコマンド
    REG_DATA_IMPACT     = 0x30, //着弾データを送る
    REG_DATA_PT1CON     = 0x40, //PT1有線、無線が切り替わった時に送る
    REG_DATA_MESSAGE    = 0x80, //テキストデータを送る
    REG_TARGET_SLEEP    = 0x90, //ターゲットスリープコマンド
    DATA_SLEEP_KEY      = 0x99, //スリープのkey
} esp_register_t;


//#define i2c1_Write1byteRegister i2c_master_write1byte
//#define i2c1_WriteDataBlock i2c_master_readblock


bool ESP32slave_Init(void){
    //ESP32 I2C slave ID check   
#define DEBUG32_1//_no
    
    uint8_t i2cRxData [4] = {0,0,0,0};  //エンドマークを含む
    uint8_t reply[] = "ESP";
    
    printf("ESP32 init ");
    if(i2c1_ESP32ReadDataBlock(ESP_SLAVE_ID, 0x01, i2cRxData, 3)){
        printf("error!\n");
        return ERROR;
    }
    if (strcmp((char*)i2cRxData, (char*)reply) != 0){
        //返答不一致
        printf("error!\n");
        return ERROR;
    }
#ifdef  DEBUG32_1
    printf("\n(detect '%s').", i2cRxData);   
#endif
    printf("OK\n"); 
    return OK;
}



//**** TARGET command **************************************************

void ESP32slave_ClearCommand(void){  
    //ESP32へターゲットクリア
    printf("> target clear\n");
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_CLEAR, 0);
}


void ESP32slave_ResetCommand(void){  
    //ESP32へターゲットリセット
    printf("** RESET ESP32 in 3 second **\n");
    __delay_ms(3000);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_RESET, 0);
}


void ESP32slave_DefaultSetCommand(void){  
    //ESP32へターゲットデフォルトセッティング
    printf("> target default set \n");
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_DEFAULT, 0);
}


bool ESP32slave_OffSetCommand(float offSet){  
    //ESP32へオフセットコマンド
    uint8_t val;
    
    if ((offSet < -40) || (offSet > 35)){
        printf("error (-40~35) \n");
        return ERROR;
    }
    //1バイト数値に変換
    val = (uint8_t)(offSet + 128);
    printf("> target Y offset %4d \n", val - 128);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_OFFSET, val);
    return OK;
}


bool ESP32slave_AimpointCommand(float aimPoint){  
    //ESP32へ狙点セットコマンド
    if ((aimPoint < 30) || (aimPoint > 120)){
        printf("error (30~120) \n");
        return ERROR;
    }
    //1バイト数値に変換
    printf("> aimpoint set %3.0f \n", aimPoint);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_AIMPOINT, (uint8_t)aimPoint);
    return OK;
}


bool ESP32slave_BrightnessCommand(float brightness){  
    //ESP32へ輝度セットコマンド
    if ((brightness < 0) || (brightness > 250)){
        printf("error (0~250) \n");
        return ERROR;
    }
    printf("> LCD backlight brightness set %3.0f \n", brightness);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_BRIGHT, (uint8_t)brightness);
    return OK;
}


//**** DATA send **************************************************

void ESP32slave_SendImpactData(uint8_t* dataToEsp, uint8_t len){
    //着弾データを送信
    //len:バイト数
    //送る前の変数の型は呼び出しの際(uint8_t*)でキャストすれば　なんでもOK。

    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_IMPACT, &dataToEsp[0], len);
}
    
/*  ポインタ変数を立てなくてもできるので、このやり方は無駄だけどポインタを理解するのには良いかも。
bool ESP32slave_SendImpactData(float* impactData, uint8_t len){
    //着弾データを送信
    //float 4バイト変数を1バイトごとの配列に変換
    uint8_t     dataToEsp[len];
    uint8_t*    pointerByte;    //ポインタ変数 - uint8_tなので1バイトずつ進む
    uint8_t     i;
    
    pointerByte = (uint8_t*)&impactData[0];
    for (i = 0; i < len; i++){
        dataToEsp[i] = pointerByte[i];
    }

    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_IMPACT, &dataToEsp[0], len)){
        printf("ESPslave error!\n");
        return ERROR;
    }
    return OK;
}
*/


void ESP32slave_SendPT1Status(uint8_t pt1Status){
    //PT1接続ステータスをESP32へ送る
    //pt1Status..0:有線LAN, 1:無線WIFI (pt1con_sor_t)
    uint8_t     dataToEsp[1];

    dataToEsp[0] = pt1Status;

    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_PT1CON, &dataToEsp[0], 1);
}



void ESP32slave_SendMessage(uint8_t* textStr) {
    //テキストメッセージをESP32へ送る。ESP側でLCDに表示    
    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_MESSAGE, textStr, (uint8_t)strlen((char*)textStr));   
}



//**** sleep *******************************************************************

void ESP32slave_SleepCommand(void){  
    //ESP32へスリープ要請

    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_SLEEP, DATA_SLEEP_KEY);
}


