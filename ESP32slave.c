/*
 * File:   ESP32slave.c
 * Comments: PIC18Q
 * 
 * ESP32 TAMAMONI�@�@I2C slave
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
//#define DEBUG_ESP_SLAVE_0_no    //Debug�pprintf(�G���[�֘A)
//#define DEBUG_ESP_SLAVE_2_no    //Debug�pprintf(�o�b�e���d��)
//#define DEBUG_ESP_SLAVE_3_no    //Debug�pprintf(���M�f�[�^)

//return value
#define OK 0
#define ERROR 1


#define ESP_SLAVE_ID      0x26  //I2C ID ESP32 TAMAMONI
//register address
typedef enum  {
    REG_ID_CHECK        = 0x01, //ID�m�F�@�ԓ�"ESP"
    REG_TARGET_CLEAR    = 0x20, //�^�[�Q�b�g�N���A�R�}���h
    REG_TARGET_RESET    = 0x21, //�^�[�Q�b�g���Z�b�g�R�}���h
    REG_TARGET_DEFAULT  = 0x22, //�^�[�Q�b�g�f�t�H���g�Z�b�g�R�}���h
    REG_TARGET_OFFSET   = 0x23, //�^�[�Q�b�gY�I�t�Z�b�g�R�}���h
    REG_TARGET_AIMPOINT = 0x24, //�^�[�Q�b�g�_�_�Z�b�g�R�}���h
    REG_TARGET_BRIGHT   = 0x25, //�^�[�Q�b�g�P�x�Z�b�g�R�}���h
    REG_DATA_IMPACT     = 0x30, //���e�f�[�^�𑗂�
    REG_DATA_PT1CON     = 0x40, //PT1�L���A�������؂�ւ�������ɑ���
    REG_DATA_MESSAGE    = 0x80, //�e�L�X�g�f�[�^�𑗂�
    REG_TARGET_SLEEP    = 0x90, //�^�[�Q�b�g�X���[�v�R�}���h
    DATA_SLEEP_KEY      = 0x99, //�X���[�v��key
} esp_register_t;


//#define i2c1_Write1byteRegister i2c_master_write1byte
//#define i2c1_WriteDataBlock i2c_master_readblock


bool ESP32slave_Init(void){
    //ESP32 I2C slave ID check   
#define DEBUG32_1//_no
    
    uint8_t i2cRxData [4] = {0,0,0,0};  //�G���h�}�[�N���܂�
    uint8_t reply[] = "ESP";
    
    printf("ESP32 init ");
    if(i2c1_ESP32ReadDataBlock(ESP_SLAVE_ID, 0x01, i2cRxData, 3)){
        printf("error!\n");
        return ERROR;
    }
    if (strcmp((char*)i2cRxData, (char*)reply) != 0){
        //�ԓ��s��v
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
    //ESP32�փ^�[�Q�b�g�N���A
    printf("> target clear\n");
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_CLEAR, 0);
}


void ESP32slave_ResetCommand(void){  
    //ESP32�փ^�[�Q�b�g���Z�b�g
    printf("** RESET ESP32 in 3 second **\n");
    __delay_ms(3000);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_RESET, 0);
}


void ESP32slave_DefaultSetCommand(void){  
    //ESP32�փ^�[�Q�b�g�f�t�H���g�Z�b�e�B���O
    printf("> target default set \n");
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_DEFAULT, 0);
}


bool ESP32slave_OffSetCommand(float offSet){  
    //ESP32�փI�t�Z�b�g�R�}���h
    uint8_t val;
    
    if ((offSet < -40) || (offSet > 35)){
        printf("error (-40~35) \n");
        return ERROR;
    }
    //1�o�C�g���l�ɕϊ�
    val = (uint8_t)(offSet + 128);
    printf("> target Y offset %4d \n", val - 128);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_OFFSET, val);
    return OK;
}


bool ESP32slave_AimpointCommand(float aimPoint){  
    //ESP32�֑_�_�Z�b�g�R�}���h
    if ((aimPoint < 30) || (aimPoint > 120)){
        printf("error (30~120) \n");
        return ERROR;
    }
    //1�o�C�g���l�ɕϊ�
    printf("> aimpoint set %3.0f \n", aimPoint);
    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_AIMPOINT, (uint8_t)aimPoint);
    return OK;
}


bool ESP32slave_BrightnessCommand(float brightness){  
    //ESP32�֋P�x�Z�b�g�R�}���h
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
    //���e�f�[�^�𑗐M
    //len:�o�C�g��
    //����O�̕ϐ��̌^�͌Ăяo���̍�(uint8_t*)�ŃL���X�g����΁@�Ȃ�ł�OK�B

    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_IMPACT, &dataToEsp[0], len);
}
    
/*  �|�C���^�ϐ��𗧂ĂȂ��Ă��ł���̂ŁA���̂����͖��ʂ����ǃ|�C���^�𗝉�����̂ɂ͗ǂ������B
bool ESP32slave_SendImpactData(float* impactData, uint8_t len){
    //���e�f�[�^�𑗐M
    //float 4�o�C�g�ϐ���1�o�C�g���Ƃ̔z��ɕϊ�
    uint8_t     dataToEsp[len];
    uint8_t*    pointerByte;    //�|�C���^�ϐ� - uint8_t�Ȃ̂�1�o�C�g���i��
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
    //PT1�ڑ��X�e�[�^�X��ESP32�֑���
    //pt1Status..0:�L��LAN, 1:����WIFI (pt1con_sor_t)
    uint8_t     dataToEsp[1];

    dataToEsp[0] = pt1Status;

    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_PT1CON, &dataToEsp[0], 1);
}



void ESP32slave_SendMessage(uint8_t* textStr) {
    //�e�L�X�g���b�Z�[�W��ESP32�֑���BESP����LCD�ɕ\��    
    i2c_master_readblock(ESP_SLAVE_ID, REG_DATA_MESSAGE, textStr, (uint8_t)strlen((char*)textStr));   
}



//**** sleep *******************************************************************

void ESP32slave_SleepCommand(void){  
    //ESP32�փX���[�v�v��

    i2c_master_write1byte(ESP_SLAVE_ID, REG_TARGET_SLEEP, DATA_SLEEP_KEY);
}


