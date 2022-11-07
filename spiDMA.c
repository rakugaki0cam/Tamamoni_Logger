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
 * データをSPI1TXBバッファーに書き込むだけで送信される。
 * けれど、LCDの場合はD/C コマンド、データの切り替えがあり、
 * 切り替えタイミングはデータの送信に合わせないといけないので、
 * SRMTIF送信完了のフラグを見て待つ必要がある。
 * DMA転送で多くのデータを連続で送る時以外はあまり有益ではないかも。
 * 
 * 2019.10.2 トランスファーカウントでSS(slave select)ピンの制御ができる
 * 
 * 
 * *****************************************************************************
 * 2020.08.17    久しぶりに使おうとしたら動かなかった。
 * 最重要　PPSロックしないとSFRには書き込めない*****************
 * dma1.c
 * void DMA1_SetDMAPriority(uint8_t priority)
 * にてPPSロック
 * MCCで生成されるんだけれど、MACだとDMA1の設定しようとするとフリーズしてできない。
 * 
 * で解決。
 * 
 * 2020.09.08   PIC18F27Q43へ移植。　　DMA1*** -> DMASELECT = 0x00; DAMn*** ....  DMAが2chから6chに増えたため
 * 2021.05.02   PIC18F57Q43へ移植。　　SPI1 -> SPI2
 * 2022.10.23   カラーモード18bitのみ
 * 
*/

#include "header.h"
#include "spiDMA.h"


void LCD_spi_Txonly_open(void){
    //SPI TX only modeのオープン
    LCD_CS_SetLow();                //LCDチップを選択オン
    SPI2_Open(LCDDMA_CONFIG);
    //MCCのopenだとTx onlyにできないので↓再設定
    SPI2CON0bits.EN = 0;                    // duplex -> TX ONLY mode
    SPI2CON0bits.BMODE = 1;                 ///転送カウンタ　BMODE:1可変転送サイズWIDTH:0で1バイト幅(BMODE:0は転送総ビット数を指定)
    SPI2CON2bits.TXR = 1;                   //TxData Transfer
    SPI2CON2bits.RXR = 0;                   //RxData is not stored
    
    //SSの処理
    //SPI1CON2bits.SSET = 0;                //SS Slave Select  cnt!=0の時active
    SPI2CON0bits.EN = 1;                    //SPI enable
    SPI2INTFbits.SRMTIF = 1;                //フラグセット(データ送信完了状態にしておく)////////
}


void LCD_spi_Txonly_close(void){
    //SPI TX only modeのクローズ
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI側の送信完了を検出
    LCD_DC_SetHigh();                       //データ受信にしておけば　空読み込みとなる
    SPI2_Close();
    LCD_CS_SetHigh();                       //LCDチップ選択をオフ
}


void LCD_spi_Txonly_sendCommand(uint8_t c){
    //TX only mode でのコマンド送信
    while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック送信完了で1になる
    LCD_DC_SetLow();                        //Command
    SPI2INTFbits.SRMTIF = 0;                //送信完了フラグをリセット->必要
    SPI2TXB = c;
}


void LCD_spi_Txonly_sendByte(uint8_t d){
    //TX only mode でのデータ送信
    while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
    LCD_DC_SetHigh();                       //Data
    SPI2INTFbits.SRMTIF = 0;                //送信完了フラグをリセット->必要
    SPI2TXB = d;

}


void LCD_spi_Txonly_sendWord(uint16_t dd){
    LCD_spi_Txonly_sendByte((uint8_t)((dd >> 8) & 0x00FF));
    LCD_spi_Txonly_sendByte((uint8_t)(dd & 0x00FF));
}


void LCD_spi_Txonly_sendBlock(uint8_t *data, uint16_t n){
     //nバイト送信
    uint16_t i;
    while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
    LCD_DC_SetHigh();                       //Data
    for (i = 0; i < n; i++){
        while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
        SPI2INTFbits.SRMTIF = 0;                //送信完了フラグをリセット->必要
        SPI2TXB = data[i];
    } 
}


void LCD_spi_Txonly_address_set(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2){
    //TX only mode でのLCDアドレスセット
    LCD_spi_Txonly_sendCommand(LCD_CMD_CASET);
    LCD_spi_Txonly_sendWord(X_START + x1);
    LCD_spi_Txonly_sendWord(X_START + x2);
    LCD_spi_Txonly_sendCommand(LCD_CMD_RASET);
    LCD_spi_Txonly_sendWord(Y_START + y1);
    LCD_spi_Txonly_sendWord(Y_START + y2);
    LCD_spi_Txonly_sendCommand(LCD_CMD_RAMWR);
    //ラムライトコマンドまで実行しておく
}


void LCD_spi_dma(uint8_t *data, uint16_t n){
    //nバイトDMA転送 条件を再設定したいとき  
    //重要　System ArbitrationロックしておかないとDMAは動かない
    while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
    LCD_DC_SetHigh();                       //Data
    
    DMASELECT = 0x00;
    DMAnCON0bits.EN = 0;                    //DMA1 reset
                                            //DMAnSCNTにはカウントアップ時にDMAnSSZがリロードされているので
                                            //一度EN=0リセットしないと条件変更は活かされない
    DMA1_SetSourceAddress((uint24_t)data);
    DMA1_SetSourceSize(n);
    
    //EN enabled; SIRQEN enabled; DGO not in progress; AIRQEN disabled; 
    DMASELECT = 0x00;
    DMAnCON0 = 0xC0;                        //転送スタート

    while(PIR2bits.DMA1SCNTIF == 0){}       //DMA転送元側カウントがゼロ///////////DMA転送が終わるまで待ってるので、もったいない
    //この時点でlcdは最後の1バイトの受信が完了していないよう
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI側の送信完了を検出
    
    //割り込みをクリア
    PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
     //PIR2bits.DMA1AIF    = 0;                //clear abort Interrupt Flag bit              (bit7)
     //PIR2bits.DMA1ORIF   = 0;                //clear overrun Interrupt Flag bit            (bit6)
     //PIR2bits.DMA1DCNTIF = 0;                //clear Destination Count Interrupt Flag bit  (bit5)
     //PIR2bits.DMA1SCNTIF = 0;                //clear Source Count Interrupt Flag bit       (bit4)
}


void LCD_spi_dma_onceagain(void){
    //同条件でもう一度転送する
    while(SPI2INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
    LCD_DC_SetHigh();                       //Data
    DMA1_StartTransferWithTrigger();        //転送スタート
    
    while(PIR2bits.DMA1SCNTIF == 0){}       //DMA転送元側カウントがゼロ///////////DMA転送が終わるまで待ってるので、もったいない
    while(SPI2INTFbits.SRMTIF == 0){}       //SPI側の送信完了を検出   
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
    
    // DMA 1回目　転送 
    //5x7エリアにイエロー7ドット書き込み
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
    //LCD_spi_Txonly_sendBlock(dotRGB, j);                    //spi送信
    LCD_spi_dma(dotRGB, j);                                 //DMA転送
    
    // DMA 2回目　転送
    xp += 12;
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //右横へ
    //同条件でもう一回
    LCD_spi_dma_onceagain();

    // DMA 3回目　転送
    xp = 0;
    yp += 8;
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //下の行頭へ
    LCD_spi_dma(dotRGB, j); 
    LCD_spi_Txonly_close();  
    
    // DMA 4回目転送 ///////////////////////////
    //5x7エリアにシアン23ドット書き込み　5+5+5+5+3
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
    LCD_spi_Txonly_address_set(xp, xp + 4, yp, yp + 6);     //右横へ
    //LCD_spi_Txonly_sendBlock(dotRGB, j);                    //spi送信
    LCD_spi_dma(dotRGB, j);                                 //DMA転送
    LCD_spi_Txonly_close();

}
*/