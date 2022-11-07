/*
 * spi_dma.c
 * 
 * SPI TX only mode
 * SPI DMA
 * 
 * 2019.09.15
 * 
 * 
 * 
 * 
 * SPI Tx ONLY mode
 * �f�[�^��SPI1TXB�o�b�t�@�[�ɏ������ނ����ő��M�����B
 * ����ǁALCD�̏ꍇ��D/C �R�}���h�A�f�[�^�̐؂�ւ�������A
 * �؂�ւ��^�C�~���O�̓f�[�^�̑��M�ɍ��킹�Ȃ��Ƃ����Ȃ��̂ŁA
 * SRMTIF���M�����̃t���O�����đ҂K�v������B
 * DMA�]���ő����̃f�[�^��A���ő��鎞�ȊO�͂��܂�L�v�ł͂Ȃ������B
 * 
 * 2019.10.2 �g�����X�t�@�[�J�E���g��SS(slave select)�s���̐��䂪�ł���
 * 
 * 
 * *****************************************************************************
 * 2020.08.17    �v���Ԃ�Ɏg�����Ƃ����瓮���Ȃ������B
 * �ŏd�v�@PPS���b�N���Ȃ���SFR�ɂ͏������߂Ȃ�*****************
 * dma1.c
 * void DMA1_SetDMAPriority(uint8_t priority)
 * �ɂ�PPS���b�N
 * MCC�Ő��������񂾂���ǁAMAC����DMA1�̐ݒ肵�悤�Ƃ���ƃt���[�Y���Ăł��Ȃ��B
 * 
 * �ŉ����B
 * 
 * 2020.09.08   PIC18F27Q43�ֈڐA�B�@�@DMA1*** -> DMASELECT = 0x00; DAMn*** ....  DMA��2ch����6ch�ɑ���������
 * 2021.05.02   PIC18F57Q43�ֈڐA�B�@�@SPI1 -> SPI2
 * 2022.10.23   �J���[���[�h18bit�̂�
 * 
*/

#include "header.h"
#include "spiDMA.h"


void LCD_spi_Txonly_open(void){
    //SPI TX only mode�̃I�[�v��
    LCD_CS_SetLow();                //LCD�`�b�v��I���I��
    SPI2_Open(LCDDMA_CONFIG);
    //MCC��open����Tx only�ɂł��Ȃ��̂Ł��Đݒ�
    SPI2CON0bits.EN = 0;                    // duplex -> TX ONLY mode
    SPI2CON0bits.BMODE = 1;                 ///�]���J�E���^�@BMODE:1�ϓ]���T�C�YWIDTH:0��1�o�C�g��(BMODE:0�͓]�����r�b�g�����w��)
    SPI2CON2bits.TXR = 1;                   //TxData Transfer
    SPI2CON2bits.RXR = 0;                   //RxData is not stored
    
    //SS�̏���
    //SPI1CON2bits.SSET = 0;                //SS Slave Select  cnt!=0�̎�active
    SPI2CON0bits.EN = 1;                    //SPI enable
    SPI2INTFbits.SRMTIF = 1;                //�t���O�Z�b�g(�f�[�^���M������Ԃɂ��Ă���)////////
}


void LCD_spi_Txonly_close(void){
    //SPI TX only mode�̃N���[�Y
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI���̑��M���������o
    LCD_DC_SetHigh();                       //�f�[�^��M�ɂ��Ă����΁@��ǂݍ��݂ƂȂ�
    SPI2_Close();
    LCD_CS_SetHigh();                       //LCD�`�b�v�I�����I�t
}


void LCD_spi_Txonly_sendCommand(uint8_t c){
    //TX only mode �ł̃R�}���h���M
    while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N���M������1�ɂȂ�
    LCD_DC_SetLow();                        //Command
    SPI2INTFbits.SRMTIF = 0;                //���M�����t���O�����Z�b�g->�K�v
    SPI2TXB = c;
}


void LCD_spi_Txonly_sendByte(uint8_t d){
    //TX only mode �ł̃f�[�^���M
    while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
    LCD_DC_SetHigh();                       //Data
    SPI2INTFbits.SRMTIF = 0;                //���M�����t���O�����Z�b�g->�K�v
    SPI2TXB = d;

}


void LCD_spi_Txonly_sendWord(uint16_t dd){
    LCD_spi_Txonly_sendByte((uint8_t)((dd >> 8) & 0x00FF));
    LCD_spi_Txonly_sendByte((uint8_t)(dd & 0x00FF));
}


void LCD_spi_Txonly_sendBlock(uint8_t *data, uint16_t n){
     //n�o�C�g���M
    uint16_t i;
    while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
    LCD_DC_SetHigh();                       //Data
    for (i = 0; i < n; i++){
        while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
        SPI2INTFbits.SRMTIF = 0;                //���M�����t���O�����Z�b�g->�K�v
        SPI2TXB = data[i];
    } 
}


void LCD_spi_Txonly_address_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2){
    //TX only mode �ł�LCD�A�h���X�Z�b�g
    LCD_spi_Txonly_sendCommand(LCD_CMD_CASET);
    LCD_spi_Txonly_sendWord(X_START + x1);
    LCD_spi_Txonly_sendWord(X_START + x2);
    LCD_spi_Txonly_sendCommand(LCD_CMD_RASET);
    LCD_spi_Txonly_sendWord(Y_START + y1);
    LCD_spi_Txonly_sendWord(Y_START + y2);
    LCD_spi_Txonly_sendCommand(LCD_CMD_RAMWR);
    //�������C�g�R�}���h�܂Ŏ��s���Ă���
}


void LCD_spi_dma(uint8_t *data, uint16_t n){
    //n�o�C�gDMA�]�� �������Đݒ肵�����Ƃ�  
    //�d�v�@System Arbitration���b�N���Ă����Ȃ���DMA�͓����Ȃ�
    while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
    LCD_DC_SetHigh();                       //Data
    
    DMASELECT = 0x00;
    DMAnCON0bits.EN = 0;                    //DMA1 reset
                                            //DMAnSCNT�ɂ̓J�E���g�A�b�v����DMAnSSZ�������[�h����Ă���̂�
                                            //��xEN=0���Z�b�g���Ȃ��Ə����ύX�͊�������Ȃ�
    DMA1_SetSourceAddress((uint24_t)data);
    DMA1_SetSourceSize(n);
    
    //EN enabled; SIRQEN enabled; DGO not in progress; AIRQEN disabled; 
    DMASELECT = 0x00;
    DMAnCON0 = 0xC0;                        //�]���X�^�[�g

    while(PIR2bits.DMA1SCNTIF == 0){}       //DMA�]�������J�E���g���[��///////////DMA�]�����I���܂ő҂��Ă�̂ŁA���������Ȃ�
    //���̎��_��lcd�͍Ō��1�o�C�g�̎�M���������Ă��Ȃ��悤
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI���̑��M���������o
    
    //���荞�݂��N���A
    PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
     //PIR2bits.DMA1AIF    = 0;                //clear abort Interrupt Flag bit              (bit7)
     //PIR2bits.DMA1ORIF   = 0;                //clear overrun Interrupt Flag bit            (bit6)
     //PIR2bits.DMA1DCNTIF = 0;                //clear Destination Count Interrupt Flag bit  (bit5)
     //PIR2bits.DMA1SCNTIF = 0;                //clear Source Count Interrupt Flag bit       (bit4)
}


void LCD_spi_dma_onceagain(void){
    //�������ł�����x�]������
    while(SPI2INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
    LCD_DC_SetHigh();                       //Data
    DMA1_StartTransferWithTrigger();        //�]���X�^�[�g
    
    while(PIR2bits.DMA1SCNTIF == 0){}       //DMA�]�������J�E���g���[��///////////DMA�]�����I���܂ő҂��Ă�̂ŁA���������Ȃ�
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI���̑��M���������o   
    PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
}


/*
//test
void LCD_spi_dma_test(void){
    uint8_t Rr = 0xff, Gg = 0xff, Bb = 0xff;
    uint8_t sq, i;
    uint16_t j;
    uint16_t xp,yp;
    
    if (DISPLAY_WIDTH > DISPLAY_HEIGHT){
        sq = DISPLAY_HEIGHT >> 3;
    }else {
        sq = DISPLAY_WIDTH >> 3;
    }
    
    // DMA 1��ځ@�]�� 
    //5x7�G���A�ɃC�G���[7�h�b�g��������
    xp = 0;
    yp = sq * 7 + 1;
    LCD_SetColor(Rr, Gg , 0);            //yellow
    j = 0;
    for (i = 0; i < 7;i++ ){
        dotRGB[j] = RGB[0];
        j++;
        dotRGB[j] = RGB[1];
        j++;
        dotRGB[j] = RGB[2];
        j++;
    }
    
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);
    //LCD_spi_Txonly_sendBlock(dotRGB, j);                    //spi���M
    LCD_spi_dma(dotRGB, j);                                 //DMA�]��
    
    // DMA 2��ځ@�]��
    xp += 12;
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //�E����
    //�������ł������
    LCD_spi_dma_onceagain();

    // DMA 3��ځ@�]��
    xp = 0;
    yp += 8;
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //���̍s����
    LCD_spi_dma(dotRGB, j); 
    LCD_spi_Txonly_close();  
    
    // DMA 4��ړ]�� ///////////////////////////
    //5x7�G���A�ɃV�A��23�h�b�g�������݁@5+5+5+5+3
    xp += 12;
    LCD_SetColor(0, Gg , Bb);           //cyan

    j = 0;
    for (i = 0; i < 23;i++ ){
        dotRGB[j] = RGB[0];
        j++;
        dotRGB[j] = RGB[1];
        j++;
        dotRGB[j] = RGB[2];
        j++;
    }
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //�E����
    //LCD_spi_Txonly_sendBlock(dotRGB, j);                    //spi���M
    LCD_spi_dma(dotRGB, j);                                 //DMA�]��
    LCD_spi_Txonly_close();

}
*/