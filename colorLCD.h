/*
 * colorLCD.h
 * 
 *  2019.08.11
 *  2020.08.29
 *  2021.01.03
 *  2022.10.23 18bit Color ONLY
 * 
 * 
*/

#ifndef COLORLCD_H
#define	COLORLCD_H

//チップの仕様
//ST7735  MAX 132RGB(H) x 162(V) 18bit_color
//ST7789  MAX 240RGB(H) x 320(V) 18bit_color
//ILI9341 MAX 240RGB(H) x 320(V) 18bit_color

//ディスプレーの設定値
#define DISPLAY_TYPE_7A

#ifdef  DISPLAY_TYPE_1
    #define     LCD_TYPE            "0.96in 80x160IPS ST7735s 8pin"
    #define     DISPLAY_COLUMN_s    26
    #define     DISPLAY_COLUMN_e    105     
    #define     DISPLAY_ROW_s       1 
    #define     DISPLAY_ROW_e       160
    #define     CMD_INV             0x21    //反転表示オン
    #define     CMD_MADCTL2         0x08    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=1:BGR,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode3_CONFIG
#endif

#ifdef  DISPLAY_TYPE_2
    #define     LCD_TYPE            "1.3in 240x240IPS ST7789 7pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    239     
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       239 
    #define     CMD_INV             0x21    // 反転表示オン
    #define     CMD_MADCTL2         0x00    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=0:RGB,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode3_CONFIG     //3 or 2
#endif

#ifdef  DISPLAY_TYPE_3
    #define     LCD_TYPE            "1.54in 240x240IPS ST7789 8pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    239     
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       239 
    #define     CMD_INV             0x21    // 反転表示オン
    #define     CMD_MADCTL2         0x00    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=0:RGB,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode3_CONFIG     //3 or 2 (0はokなこともある)
#endif

#ifdef  DISPLAY_TYPE_4
    #define     LCD_TYPE            "1.8in 128x160 ST7735 RED +uSD 11pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    127     
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       159 
    #define     CMD_INV             0x20    // 反転表示オフ
    #define     CMD_MADCTL2         0x00    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=0:RGB,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode0_CONFIG
#endif

#ifdef  DISPLAY_TYPE_4B
    #define     LCD_TYPE            "1.8in 128x160 ST7735 Blue 8pin"
    #define     DISPLAY_COLUMN_s    2
    #define     DISPLAY_COLUMN_e    129     
    #define     DISPLAY_ROW_s       1 
    #define     DISPLAY_ROW_e       160 
    #define     CMD_INV             0x20    // 反転表示オフ
  //#define     CMD_MADCTL2         0x00    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=0:RGB,-,-,- ピンが下
    #define     CMD_MADCTL2         0xc0    //メモリアクセス MY=1,MX=1,MV=0,-,RGB=0:RGB,-,-,-   さかさま　ピンが上
    #define     LCD_OPEN_TYPE       LCD_SPImode0_CONFIG
#endif

#ifdef  DISPLAY_TYPE_5
    #define     LCD_TYPE            "2.0in 240x320IPS ST7789 8pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    239     
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       319 
    #define     CMD_INV             0x21    // 反転表示オン
    #define     CMD_MADCTL2         0x00    //メモリアクセス MY=0,MX=0,MV=0,-,RGB=0:RGB,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode3_CONFIG     //3 or 2 (0はokなこともある)
#endif

#ifdef  DISPLAY_TYPE_6
    #define     LCD_TYPE            "2.2in 240x320 QVGA ILI9341+SD 9pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    319  
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       239
    #define     CMD_INV             0x20    // 反転表示オフ
    #define     CMD_MADCTL2         0x28    //メモリアクセス MY=0,MX=0,MV=1,-,RGB=1:BGR,-,-,-
    #define     LCD_OPEN_TYPE       LCD_SPImode0_CONFIG
#endif

#ifdef  DISPLAY_TYPE_7A
    #define     LCD_TYPE            "2.4in 240x320 QVGA ILI9341+SD+Touch Portrate 14pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    239  
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       319
    #define     CMD_INV             0x20    // 反転表示オフ
    #define     CMD_MADCTL2         0x48    //メモリアクセス MY=0,MX=1,MV=0,-,RGB=1:BGR,-,-,- 縦長
    #define     LCD_OPEN_TYPE       LCD_SPImode0_CONFIG     //0 or 3
#endif

#ifdef  DISPLAY_TYPE_7B
    #define     LCD_TYPE            "2.4in 240x320 QVGA ILI9341+SD+Touch Landscape 14pin"
    #define     DISPLAY_COLUMN_s    0 
    #define     DISPLAY_COLUMN_e    319
    #define     DISPLAY_ROW_s       0 
    #define     DISPLAY_ROW_e       239
    #define     CMD_INV             0x20    // 反転表示オフ
    #define     CMD_MADCTL2         0x28    //メモリアクセス MY=0,MX=0,MV=1,-,RGB=1:BGR,-,-,- 横長
    #define     LCD_OPEN_TYPE       LCD_SPImode0_CONFIG
#endif

//ディスプレイサイズ
#define     X_START         DISPLAY_COLUMN_s        // フレームメモリの列開始位置
#define     X_END           DISPLAY_COLUMN_e        // フレームメモリの列終了位置
#define     Y_START         DISPLAY_ROW_s           // フレームメモリの行開始位置
#define     Y_END           DISPLAY_ROW_e           // フレームメモリの行終了位置
#define     DISPLAY_WIDTH   (X_END - X_START + 1)   //ディスプレイ幅ドット数
#define     DISPLAY_HEIGHT  (Y_END - Y_START + 1)   //ディスプレイ高さドット数

// コマンドとパラメータの情報
#define     LCD_COMMAND         0       // SOIのデータはコマンドを指示
#define     LCD_DATA            1       // SOIのデータはパラメータを指示
#define     LCD_CMD_NOP         0x00    //ノップ
#define     LCD_CMD_SWREST      0x01    //ソフトウェアリセット
#define     LCD_CMD_RDDID       0x04    //ID読み出し
#define     LCD_CMD_RDDST       0x09    //ステータス読み出し
#define     LCD_CMD_RDDPM       0x0A    //パワーモード読み出し
#define     LCD_CMD_RDDMADCTL   0x0B    //メモリアドレスコントロール読み出し
#define     LCD_CMD_RDDCOLMOD   0x0C    //カラーモード読み出し
#define     LCD_CMD_SLPIN       0x10    //スリープ
#define     LCD_CMD_SLPOUT      0x11    //スリープから復帰
#define     LCD_CMD_INVOFF      0x20    //反転表示オフ
#define     LCD_CMD_INVON       0x21    //反転表示オン
#define     LCD_CMD_DISPOFF     0x28    //ディスプレイをオフ
#define     LCD_CMD_DISPON      0x29    //ディスプレイオン
#define     LCD_CMD_CASET       0x2A    //列アドレスセット
#define     LCD_CMD_RASET       0x2B    //行アドレスセット
#define     LCD_CMD_RAMWR       0x2C    //メモリに書込
#define     LCD_CMD_RAMRD       0x2E    //メモリ読み出し
#define     LCD_CMD_MADCTL      0x36    //メモリコントロール
#define     LCD_CMD_COLMOD      0x3A    //カラーモード

#define     BLACK_SCREEN        0
#define     WHITE_SCREEN        1
#define     COLOR_18BIT         6       //R:6 G:6 B:6


//テキスト文字色 3^3  R G B  00,01,11
#define     WHITE           0b00111111  //63
#define     LEMON           0b00111101  //61
#define     YELLOW          0b00111100  //60
#define     VIOLET          0b00110111  //55
#define     ROSE            0b00110101  //53
#define     ORANGE          0b00110100  //52
#define     MAGENTA         0b00110011  //51
#define     PINK            0b00110001  //49
#define     RED             0b00110000  //48
#define     AQUA            0b00011111  //31
#define     PAREGREEN       0b00011101  //29
#define     CHARTREUSE      0b00011100  //28
#define     SKY             0b00010111  //23
#define     GREY            0b00010101  //21
#define     OLIVE           0b00010100  //20
#define     SAPPHIRE        0b00010011  //19
#define     PURPLE          0b00010001  //17
#define     MAROON          0b00010000  //16
#define     CYAN            0b00001111  //15
#define     LIME            0b00001101  //13
#define     GREEN           0b00001100  //12
#define     INDIGO          0b00000111  //07
#define     TEAL            0b00000101  //05
#define     EMELARD         0b00000100  //04
#define     BLUE            0b00000011  //03
#define     NAVY            0b00000001  //01
#define     BLACK           0b00000000  //00


//グローバル変数
extern uint8_t      RGB_18bit[];          // RGB値からST7735の色情報に変換したテーブル



//SPIバスへの操作
void    LCD_spi_open(void);
void    LCD_spi_close(void);   
void    LCD_spi_sendCommand(uint8_t);
void    LCD_spi_sendByte(uint8_t);
void    LCD_spi_sendWord(uint16_t);
uint8_t LCD_spi_readByte(uint8_t);
void    LCD_spi_readLongbytes(uint8_t, uint8_t *, uint16_t);
void    LCD_spi_readVramData(uint8_t *, uint16_t);

//LCD表示のサブルーチン
void    LCD_initialize(void);
void    LCD_ClearScreen(uint8_t);
void    LCD_SetColor(uint8_t, uint8_t, uint8_t);
void    LCD_SetPosition(uint16_t, uint16_t, uint16_t, uint16_t);

void    LCD_SetPixel(uint16_t, uint16_t);
void    LCD_DrawBox(uint16_t, uint16_t, uint16_t, uint16_t);
void    LCD_DrawFillBox(uint16_t, uint16_t, uint16_t, uint16_t);
void    LCD_DrawCircle(uint16_t, uint16_t, uint16_t);
void    LCD_DrawFillCircle(uint16_t, uint16_t, uint16_t);
void    LCD_DrawLine(uint16_t, uint16_t, uint16_t, uint16_t);

void    LCD_PutCHR(uint16_t, uint16_t, uint8_t, uint8_t, uint8_t);
void    LCD_Printf(uint16_t, uint16_t, char *, uint8_t, uint8_t, uint8_t);
void    LCD_FontData_Generate(uint8_t, uint8_t, uint8_t);

//テスト
void    LCD_Title(void);
void    LCD_Title_Clear(void);
void    LCD_ColorSample(void);
void    LCD_ReadTest(void);
void    LCD_ReadTest2(void);
void    LCD_ReadTest3(void);


#endif	//COLORLCD_H

/**
 End of File
*/