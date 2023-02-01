/*
 * colorLCD.c
 * 
 * 
 * 
 * 2019.08.10
 * 2019.08.22       カラーモード　12、16、18ビット3種類に対応
 * 2019.09.16       読み込みテストを追加
 * 2020.08.29       倍角フォント対応
 * 2020.09.09       DMA使用して高速化(spiDMA.c必要)
 * 2021.05.02       PIC18F57Q43 48pin SPI1 -> SPI2
 * 2021.05.05       塗りつぶし円デバグ完了
 * 2021.10.12       測定中mesure_stage = 1~9の間文字描写できず。途中でもリターンするため。
 * 2022.05.07       ↑測定中リターン無し。V8バージョン　タイトルバー表示なし。ターゲット画面を広く使うため
 * 2022.10.23       メモリ対策 dotRGB 960->580(2倍角)へ。カラー18ビットのみに。
 * 2022.10.23       SDのLFN廃止でメモリ確保。dotRGB[1280]
 * 2023.02.01       SPI2 -> SPI1 SDカードと共用化

 * 
 */

#include "header.h"
#include "colorLCD.h"

#define LCD_READ_NONE


//global
uint8_t     RGB_18bit[3];                 // RGB値からST7735の色情報に変換したテーブル


//LCD SPI関連
void LCD_spi_open(void) {
    //コマンド操作するとき必要
    SPI1_Open(LCD_CONFIG);
    LCD_CS_SetLow(); //LCDチップを選択オン
}

void LCD_spi_close(void) {
    //CSが無いLCDでの誤作動防止対策付き
    LCD_spi_sendCommand(LCD_CMD_NOP); //空コマンドを打って、メモリー書き込みを終える
    LCD_DC_SetHigh(); //データ受信にしておけば　空読み込みとなる
    SPI1_Close();
    LCD_CS_SetHigh(); //LCDチップ選択をオフ
}

void LCD_spi_sendCommand(uint8_t c) {
    //コマンドを送る
    LCD_DC_SetLow(); //コマンド
    SPI1_ExchangeByte(c);
}

void LCD_spi_sendByte(uint8_t d) {
    //1バイトデータを書き出し
    LCD_DC_SetHigh(); //データ
    SPI1_ExchangeByte(d);
}

void LCD_spi_sendWord(uint16_t dd) {
    //2バイトデータを書き出し
    LCD_DC_SetHigh(); //データ
    SPI1_ExchangeByte((uint8_t)(dd >> 8) & 0x00ff);
    SPI1_ExchangeByte((uint8_t)(dd & 0x00ff));
}

uint8_t LCD_spi_readByte(uint8_t c) {
    //コマンドを送り1バイトデータを読み込み
    uint8_t d;
    //LCD_spi_open();
    LCD_DC_SetLow(); //コマンド
    SPI1_ExchangeByte(c);
    LCD_DC_SetHigh(); //データ
    d = SPI1_ExchangeByte(0x00);
    //LCD_spi_close();
    return d;
}

void LCD_spi_readLongbytes(uint8_t c, uint8_t *data, uint16_t n) {
    //コマンドを送り　nバイトのデータを読み出し  ID,Statusの3～4バイト用
    uint16_t i;
    //LCD_spi_open();
    SPI1CON0bits.EN = 0; //SPI1:disableしてCON0を変更可に
    SPI1CON0bits.BMODE = 1; //バリアブルビットモードに変更
    SPI1CON0bits.EN = 1; //SPI1:enable
    LCD_DC_SetLow(); //コマンド
    SPI1_ExchangeByte(c);
    LCD_DC_SetHigh(); //データ
    SPI1TWIDTH = 1; //データ幅1ビット
    //1ビットのダミークロックを捨てる
    SPI1_ExchangeByte(0x01);
    SPI1TWIDTH = 0; //データ幅8ビットに戻す
    for (i = 0; i < n; i++) {
        data[i] = SPI1_ExchangeByte(0x00);
    }
    //LCD_spi_close();
}

void LCD_spi_readVramData(uint8_t *data, uint16_t n) {
    //nバイトのVRAMデータを読み出し
    uint16_t i;
    //LCD_spi_open();
    LCD_DC_SetHigh(); //データ   
    for (i = 0; i < n; i++) {
        data[i] = SPI1_ExchangeByte(0x00);
    }
    //LCD_spi_close();
}


//LCD操作 サブルーチン
void LCD_initialize(void) {
    // ディスプレイ初期化
#if LCD_RESET_NO == 0
    //ハードウエアリセット
    LCD_RESET_SetLow();
    __delay_us(10);
    LCD_RESET_SetHigh();
    __delay_ms(120);
#endif
    
    LCD_spi_open();
    LCD_spi_sendCommand(LCD_CMD_SWREST);            //ソフトウェアリセット
    __delay_ms(120);
    LCD_spi_sendCommand(LCD_CMD_SLPOUT);            //スリープから復帰
    __delay_ms(120);
    LCD_spi_sendCommand(LCD_CMD_COLMOD);            //カラーモード
    LCD_spi_sendByte((COLOR_18BIT << 4) | COLOR_18BIT);
    LCD_spi_sendCommand(LCD_CMD_MADCTL);            //メモリーデータアクセスコントロール
    //LCD_spi_sendByte(CMD_MADCTL1 | CMD_MADCTL2);
    LCD_spi_sendByte(CMD_MADCTL2);
    LCD_spi_sendCommand(CMD_INV);                   //反転表示
    __delay_ms(120);
    // ディスプレイの表示方向を指定する処理//////////////////////////////まだ
    //LCD_DisplayDirection(4) ;
    
    LCD_spi_sendCommand(LCD_CMD_DISPON);            // ディスプレイオン
    LCD_spi_close();
    LCD_ClearScreen(0);                             // 画面を0=黒、1=白で消去する
    __delay_ms(50);                                 //クリアし終わるまで通信時間がすこしかかる。
}

void LCD_ClearScreen(uint8_t white) {
    //画面クリア
    //DMAで高速化
    uint8_t color;
    uint16_t i, cnt;
    
    //カラーコードの計算
    if (white == 0) {
        color = 0x00;   //BLACK
    } else {
        color = 0xff;   //WHITE
    }
    
    //横1ライン分のカラーデータを作る 240x3=720バイト
    cnt = DISPLAY_WIDTH * 3;
    for (i = 0; i < cnt; i++ ){
        dotRGB[i] = color;
    }
    
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(0, DISPLAY_WIDTH - 1, 0, DISPLAY_HEIGHT - 1);    //画面全域
    LCD_spi_dma(dotRGB, cnt);    //DMA転送1ライン目
    for (i = 1; i < DISPLAY_HEIGHT; i++) {
        LCD_spi_dma_onceagain();    //同条件でDMA転送繰り返し
    }
    LCD_spi_Txonly_close();
}

void LCD_SetColor(uint8_t red, uint8_t green, uint8_t blue) {
    //24ビットカラー値を18bitカラーデータに変換
        //1ドット3バイト　R6+欠2, G6+欠2, B6+欠2 下位2ビットがゼロ
        //global        /////////////////////////////////////////////////////////////////////ローカルにしたい
        RGB_18bit[0] = red & 0xfc;
        RGB_18bit[1] = green & 0xfc;
        RGB_18bit[2] = blue & 0xfc;
}

void LCD_SetPosition(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1) {
    //座標をセット
    /*
    uint16_t swap;
    if (x0 > x1) {
        swap = x1;
        x1 = x0;
        x0 = swap;
    }
    if (y0 > y1) {
        swap = y1;
        y1 = y0;
        y0 = swap;
    }
    */
    
    if (x1 > DISPLAY_WIDTH - 1) {
        x1 = DISPLAY_WIDTH - 1;
    }
    if (y1 > DISPLAY_HEIGHT - 1) {
        y1 = DISPLAY_HEIGHT - 1;
    }
    LCD_spi_open();
    // 書き込むフレームメモリの列側範囲を指定する
    LCD_spi_sendCommand(LCD_CMD_CASET);
    LCD_spi_sendWord(X_START + x0); // 開始列
    LCD_spi_sendWord(X_START + x1); // 終了列
    // 書き込むフレームメモリの行側範囲を指定する
    LCD_spi_sendCommand(LCD_CMD_RASET);
    LCD_spi_sendWord(Y_START + y0); // 開始行
    LCD_spi_sendWord(Y_START + y1); // 終了行
    LCD_spi_close();
}


void LCD_SetPixel(uint16_t x0, uint16_t y0) {
    // 1ドットうつ
    //DMAで高速化
    dotRGB[0] = RGB_18bit[0];
    dotRGB[1] = RGB_18bit[1];
    dotRGB[2] = RGB_18bit[2];
    
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(x0, x0, y0, y0);
    //LCD_spi_dma(dotRGB, 3);               //DMA転送/////////////////////////////////RGB直でよいのでは??????
    LCD_spi_dma(RGB_18bit, 3);
    LCD_spi_Txonly_close();
}


void LCD_DrawBox(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint16_t     i;
    for (i = x0; i <= x1; i++){
        LCD_SetPixel(i, y0);       //上側横線
        LCD_SetPixel(i, y1);       //下側横線
    }
    for (i = y0 + 1; i <= y1; i++){
        LCD_SetPixel(x0, i);       //左側縦線
        LCD_SetPixel(x1, i);       //右側縦線
    }
}


void LCD_DrawFillBox(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // (x1,y1)-(x2,y2)を対角線とした塗りつぶしの四角形を描く
    //DMAで高速化
    uint16_t i, cnt;
    cnt = 0;
    //横1ライン分のカラーデータを作る
    for (i = x0; i <= x1; i++ ){
        dotRGB[cnt] = RGB_18bit[0];
        cnt++;
        dotRGB[cnt] = RGB_18bit[1];
        cnt++;
        dotRGB[cnt] = RGB_18bit[2];
        cnt++;
    }
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(x0, x1, y0, y1);
    LCD_spi_dma(dotRGB, cnt);               //DMA転送1ライン目
    for (i = y0 + 1; i <= y1; i++) {
        LCD_spi_dma_onceagain();            //同条件でDMA転送繰り返し
    }
    LCD_spi_Txonly_close();
}

//***********コピペ　　　　みづらい

void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r){
    //円を描く
    //ちょっと大きいような???
    int16_t     f     = 1 - r ;
    int16_t     ddF_x = 1 ;
    int16_t     ddF_y = -2 * r ;
    int16_t     x     = 0 ;
    int16_t     y     = r ;

    LCD_SetPixel(x0, y0 + r) ;
    LCD_SetPixel(x0, y0 - r) ;
    LCD_SetPixel(x0 + r, y0) ;
    LCD_SetPixel(x0 - r, y0) ;

    while (x < y) {
        if (f >= 0){
            y-- ;
            ddF_y += 2 ;
            f += ddF_y ;
        }
        x++ ;
        ddF_x += 2 ;
        f += ddF_x ;
        LCD_SetPixel(x0 + x, y0 + y) ;
        LCD_SetPixel(x0 - x, y0 + y) ;
        LCD_SetPixel(x0 + x, y0 - y) ;
        LCD_SetPixel(x0 - x, y0 - y) ;
        LCD_SetPixel(x0 + y, y0 + x) ;
        LCD_SetPixel(x0 - y, y0 + x) ;
        LCD_SetPixel(x0 + y, y0 - x) ;
        LCD_SetPixel(x0 - y, y0 - x) ;
    }
    
}


void LCD_DrawFillCircle(uint16_t x0, uint16_t y0, uint16_t r){
    //塗りつぶし円を描写
    int16_t     x, y;
    int32_t     xx, yy, rr;
    
    // 四角形エリア内(x1,y1)(x2,y2)について半径rの円内を塗りつぶす
    rr = r * r;
    for(y = y0 - r; y < y0 + r; y++) {
        yy = (y - y0) * (y - y0);         //中心との距離の2乗(dy^2)
        for(x = x0 - r; x < x0 + r; x++) {
            xx = (x - x0) * (x - x0);    //中心との距離の2乗(dx^2)
            if ((xx + yy) <= rr){        //円の公式 x^2 + y^2 = r^2
                LCD_SetPixel(x, y);     //円内なのでドット描写
            }
        }
    }
}


void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    //線を描画する　DDA法
    bool        incline;
    int16_t     x, y;
    int16_t     tmp;
    int16_t     dx, dy;     //増分
    int16_t     e, ys;      //e:yの
	
    //増分の判定
    incline = (abs(y1 - y0) > abs(x1 - x0));
    //増分が大きい方をxにする(yの増分が大きいと点線になってしまう)
    if (incline){
        tmp = x0;
        x0 = y0;
        y0 = tmp;
        
        tmp = x1;
        x1 = y1;
        y1 = tmp;
    }
    //始点 x0 < x1 終点とする(左から右へ描写)
    if (x0 > x1) {
        tmp = x0;
        x0 = x1;
        x1 = tmp;
        
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    
    //増分の計算 
    dx = x1 - x0;
    dy = abs(y1 - y0);
    e = 0;
    y  = y0;
    
    //傾きでステップの正負を切り替え
    if (y0 < y1){
        ys = 1; 
    }else {
        ys = -1;
    }
    
    //点で描画
    for(x = x0; x <= x1; x++) {
        if (incline){
            LCD_SetPixel(y, x); 
        }else{
            LCD_SetPixel(x, y);
        }
        e += dy;
        if((e << 1) >= dx){
            //yの増分はxの増分より小さいので何回か同じy位置が続く
            y += ys;
            e -= dx;
        }
    }
}

//***文字

void LCD_PutCHR(uint16_t x0, uint16_t y0, uint8_t asc_code, uint8_t font_size, uint8_t background) {
    //1文字を表示 5x7font
    //DMA一部使用
    //サイズ　拡大倍率　1: 5x7, 2: 10x14, 3: 15x21 ...
    //背景に重ねて表示 background=0;--->DMAでは無理
    //背景色黒　background=1
    uint16_t    x1, y1;
    uint16_t    cnt;
    uint8_t     fontx, fonty, dotxy, fontdata;

    if (background == 1){
        //背景色黒　background = 1のときDMAで高速描画
        cnt = (6 * font_size) * (8 * font_size) * 3;    //18bitRGB = 3byte使う
        LCD_FontData_Generate(asc_code, font_size, 0);
      
        x1 = x0 + 6 * font_size - 1;
        y1 = y0 + 8 * font_size - 1;

        LCD_spi_Txonly_open();
        LCD_spi_Txonly_address_set(x0, x1, y0, y1);
        LCD_spi_dma(dotRGB, cnt);               //DMA転送
        LCD_spi_Txonly_close();
    }else{
        //背景に重ねて描画 background　=　0　のとき
        //Txonlyを使用
        LCD_spi_Txonly_open();
        for (fontx = 0; fontx < 6; fontx++) {
            if (fontx < 5){
                fontdata = font5x7[asc_code - 0x20][fontx];
            }else{
                fontdata = 0x00;
            }
            for (fonty = 0; fonty < 8; fonty++) {
                x1 = x0 + fontx * font_size;
                y1 = y0 + fonty * font_size;
                LCD_spi_Txonly_address_set(x1, x1 + font_size - 1, y1, y1 + font_size - 1);     //右横へ
                for (dotxy = 0; dotxy < (font_size * font_size); dotxy++){
                    //点を打つ
                    if (((fontdata >> fonty) & 0x01) == 1) {        //ドットあり
                        LCD_spi_Txonly_sendByte(RGB_18bit[0]);
                        LCD_spi_Txonly_sendByte(RGB_18bit[1]);
                        LCD_spi_Txonly_sendByte(RGB_18bit[2]);     
                    }else if (background == 1){                 //ドットなし
                        //背景色＝黒
                        LCD_spi_Txonly_sendByte(0);
                        LCD_spi_Txonly_sendByte(0);
                        LCD_spi_Txonly_sendByte(0);     
                    }
                }
            }
        }
        LCD_spi_Txonly_close();
    }
}



void LCD_Printf(uint16_t x0, uint16_t y0, char *string, uint8_t font_size, uint8_t color, uint8_t bgb) {
    //文字列をlprint風に表示 文字間ピッチ6ドット　1行8ドット x 倍率
    //カラーコードは3^3 = 27色  00,01,11  10は無し
    uint8_t     rgb2[3], i;
    uint16_t    x1, y1;
    uint16_t    cnt;
    uint8_t     mem_page = 0;
    
    if (font_size > 2){
        //フォントサイズ3倍角以上はDMA不可のため背景重ねモードで書き込み
        bgb = 0;
    }
    
    //色のセット 27色
    rgb2[0]=(color >> 4) & 0x03;
    rgb2[1]=(color >> 2) & 0x03;
    rgb2[2]=color & 0x03;
    for (i = 0; i < 3; i++){
        switch  (rgb2[i]){
            case 0:
                rgb2[i] = 0;
                break;
            case 1:
                rgb2[i] = 0x7f;
                break;
            case 2:
                //ほんとは無しなんだけど
                rgb2[i] = 0x7f;
                break;
            case 3:
                rgb2[i] = 0xff;
                break;    
        }
    }
    LCD_SetColor(rgb2[0], rgb2[1], rgb2[2]);
    
    //文字列表示
    if (bgb == 0){
        //背景に重ね書き
        while (*string) {
            LCD_PutCHR(x0, y0, *string++, font_size, bgb);
            x0 += (6 * font_size); //横を1ドット空ける為に5+1=6とする
        }   
    }else{
        //背景黒
        //DMAを使って高速に文字列を表示
        cnt = (6 * font_size) * (8 * font_size) * 3;

        //DMAの設定
        DMASELECT = 0x00;
        DMAnCON0bits.EN = 0;                    //DMA1 reset
                                                //DMAnSCNTにはカウントアップ時にDMAnSSZがリロードされているので
                                                //一度EN=0リセットしないと条件変更は活かされない
        //DMA1_SetSourceAddress((uint24_t)dotRGB);
        //DMA1_SetSourceSize(cnt);
        DMASELECT = 0x00;
        DMAnSSA = (uint24_t)dotRGB;
        DMAnSSZ = cnt;
        
        PIR2bits.DMA1SCNTIF = 1;               //DMA転送完了状態にしておく

        LCD_spi_Txonly_open();
        while (*string) {
            //DMA転送中に次のデータを変更できる
            LCD_FontData_Generate(*string++, font_size, mem_page);
            
            x1 = x0 + 6 * font_size - 1;
            y1 = y0 + 8 * font_size - 1;    //改行は無し
            
            while(PIR2bits.DMA1SCNTIF == 0){
                //DMA転送完了待ち
            }
            DMASELECT = 0x00;
            DMAnCON0bits.EN = 0;                    //DMA1 reset
            DMAnSSA = (uint24_t)dotRGB;
            if (mem_page == 1){
                DMAnSSA += 640;
            }
            PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
            
            LCD_spi_Txonly_address_set(x0, x1, y0, y1);
            while(SPI1INTFbits.SRMTIF == 0){}       //送信中でないかのチェック
            LCD_DC_SetHigh();                       //Data
    
            //EN enabled; SIRQEN enabled; DGO not in progress; AIRQEN disabled; 
            DMASELECT = 0x00;
            DMAnCON0 = 0xC0;                        //DMA転送
            
            x0 += (6 * font_size);                  //横を1ドット空ける為に5+1=6とする
            mem_page ^= 0x01;                       //ページを入れ替え

        }      
        while(PIR2bits.DMA1SCNTIF == 0){}       //DMA転送元側カウントがゼロ
        while(SPI1INTFbits.SRMTIF == 0){}       //SPI側の送信完了を検出
        PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
        DMASELECT = 0x00;
        DMAnCON0bits.EN = 0;                    //DMA1 reset
        
        LCD_spi_Txonly_close();
    }
}


void LCD_FontData_Generate(uint8_t asc_code, uint8_t font_size, uint8_t mem_page){
    //1文字分のカラーデータを作る -> dotRGB[]　6x8フォントの時 6pixel x 8pixel x 3byteRBG = 144byte
    //DMA転送用
    uint8_t     fontx, fonty, dotx, doty , fontdata;
    uint16_t    index_d;
    
    if(font_size > 2){
        //DMAは2倍角まで 2倍角の時12pixelx16pixelx3byteRGB = 576bytes必要
        font_size = 2;
    }
    
    for (fontx = 0; fontx < 6; fontx++) {
        if (fontx < 5){
            fontdata = font5x7[asc_code - 0x20][fontx];
        }else{
            fontdata = 0x00;
        }
        for (fonty = 0; fonty < 8; fonty++) {
            //フォントデータは最下位ビットが上
            for (doty = 0; doty < font_size; doty++){
                for (dotx = 0; dotx < font_size; dotx++){
                    index_d = ((fontx * (uint16_t)font_size + dotx) + ((fonty * (uint16_t)font_size + doty) * (6 * (uint16_t)font_size))) * 3;
                    if (mem_page == 1){
                        index_d += 640;
                    }
                    //printf("i%2d j%2d a%3d\n", i, j, ab);
                    if (((fontdata >> fonty) & 0x01) == 1) {    //ドットあり
                        dotRGB[index_d] = RGB_18bit[0];
                        dotRGB[index_d + 1] = RGB_18bit[1];
                        dotRGB[index_d + 2] = RGB_18bit[2];
                    }else {                                     //ドットなし
                        //背景色=黒
                        dotRGB[index_d] = 0;
                        dotRGB[index_d + 1] = 0;
                        dotRGB[index_d + 2] = 0;
                    }
                }
            }
        }
    }
}


void LCD_Title(void) {
    //Bullet Logger V8 title
    __delay_ms(2);
    LCD_SetColor(0xb0, 0x80, 0x30);                       //カラシ色
    LCD_DrawFillBox(5, 60, DISPLAY_WIDTH - 6, 110);
    sprintf(tmp_string, "%s", title);
    LCD_Printf(10, 70, tmp_string, 2, BLACK, 0);
    sprintf(tmp_string, "ver.%s", version);
    LCD_Printf(130, 95, tmp_string, 1, BLACK, 0);
}


void LCD_Title_Clear(void){
    //タイトル消去
    __delay_ms(2);
    LCD_SetColor(0x0, 0x0, 0x0);                        //黒
    LCD_DrawFillBox(0, 50, DISPLAY_WIDTH - 1, 150);
}



/*// test
void LCD_ColorSample(void){
    //色見本
    uint8_t     i, yl = 20;
    __delay_ms(3000);
    LCD_SetColor(0, 0, 0); //black
    LCD_DrawFillBox(0, yl, DISPLAY_WIDTH - 1, 239);
    yl += 4;
    LCD_Printf(6, yl, "27 Color  Sample  3 x 3", 2, WHITE, 1);
    yl += 20;
    for (i = 0; i < 0b00111111; i += 4){
        if ((((i >> 2) & 0x03) != 2) & (((i >> 4) & 0x03) != 2)){
            sprintf(tmp_string, "%02d ABCD", i    );
            LCD_Printf(                        5, yl, tmp_string, 2, i    , 1); 
            sprintf(tmp_string, "%02d ABCD", i + 1);
            LCD_Printf(DISPLAY_WIDTH / 3 + 5    , yl, tmp_string, 2, i + 1, 1);
            sprintf(tmp_string, "%02d ABCD", i + 3);
            LCD_Printf(DISPLAY_WIDTH / 3 * 2 + 5, yl, tmp_string, 2, i + 3, 1); 
            yl += 20;
        }
    }
    Stop_until_SW1();
    __delay_ms(100);
    LCD_ClearScreen(0);                                 // 画面を0=黒、1=白で消去する
}
*/

#ifdef LCD_READ
void LCD_ReadTest(void) {
    //LCDデータの読み出しテスト
    uint8_t command;
    uint8_t readdata;
    uint8_t readblock[4];
    
    LCD_spi_open();
    command = LCD_CMD_RDDID;
    LCD_spi_readLongbytes(command, readblock, 3);
    printf("%02X ID     %02x%02x%02x\n", command, readblock[0], readblock[1], readblock[2]);
    __delay_ms(50);

    command = LCD_CMD_RDDST;
    LCD_spi_readLongbytes(command, readblock, 4);
    printf("%02X status %02x%02x%02x%02x\n", command, readblock[0], readblock[1], readblock[2], readblock[3]);
    __delay_ms(50);

    command = LCD_CMD_RDDPM;
    readdata = LCD_spi_readByte(command);
    printf("%02X power  %02x \n", command, readdata);
    __delay_ms(50);

    command = LCD_CMD_RDDMADCTL;
    readdata = LCD_spi_readByte(command);
    printf("%02X MADCTL %02x \n", command, readdata);
    __delay_ms(50);

    command = LCD_CMD_RDDCOLMOD;
    readdata = LCD_spi_readByte(command);
    printf("%02X pixel  %02x \n", command, readdata);
    __delay_ms(50);
    
    /*
    command = 0x0d;
    readdata = LCD_spi_readByte(command);
    printf("%02X image  %02x \n", command, readdata);
    __delay_ms(50);

    command = 0x0e;
    readdata = LCD_spi_readByte(command);
    printf("%02X signal %02x \n", command, readdata);
    __delay_ms(50);

    command = 0x0f;
    readdata = LCD_spi_readByte(command);
    printf("%02X diag   %02x \n", command, readdata);
    __delay_ms(50);

    command = 0x52;
    readdata = LCD_spi_readByte(command);
    printf("%02X DISBV  %02x \n", command, readdata);
    __delay_ms(50);
    
    command = 0x54;
    readdata = LCD_spi_readByte(command);
    printf("%02X CTRLD  %02x \n", command, readdata);
    __delay_ms(50);
    
    command = 0x56;
    readdata = LCD_spi_readByte(command);
    printf("%02X CABC   %02x \n", command, readdata);
    __delay_ms(50);
    
    command = 0x5f;
    readdata = LCD_spi_readByte(command);
    printf("%02X CABCM  %02x \n", command, readdata);
    __delay_ms(50);
    
    command = 0x68;
    readdata = LCD_spi_readByte(command);
    printf("%02X ABCSDR %02x \n", command, readdata);
    __delay_ms(50);
     */
    
    command = 0xd3;
    LCD_spi_readLongbytes(command, readblock, 3);
    printf("%02X ID4    %02x%02x%02x\n", command, readblock[0], readblock[1], readblock[2]);
    __delay_ms(50);

    command = 0xda;
    readdata = LCD_spi_readByte(command);
    printf("%02X ID1    %02x \n", command, readdata);
    __delay_ms(50);

    command = 0xdb;
    readdata = LCD_spi_readByte(command);
    printf("%02X ID2    %02x \n", command, readdata);
    __delay_ms(50);

    command = 0xdc;
    readdata = LCD_spi_readByte(command);
    printf("%02X ID3    %02x \n", command, readdata);
    __delay_ms(50);
    LCD_spi_close();
}

void LCD_ReadTest2(void) {
    uint8_t command;
    uint16_t x, y;

    LCD_SetPosition(0, 319, 0, 239);          //アドレスセット有効
    for (y = 0;y < 2;y++){
        //1ラインずつ読み込み
        LCD_spi_open();
        command = LCD_CMD_RAMRD;
        LCD_spi_sendCommand(command);
        SPI1_ExchangeByte(0x00);                //ダミー1バイト捨て読み
        LCD_spi_readVramData(dotRGB, 320 * 3);
        LCD_spi_close();

        printf("%02X VRAM y%03dline\n", command,y);
        for (x = 0; x < 320; x += 2) {
            printf("%03d ", x);
            printf("%02x%02x%02x ", dotRGB[x * 3], dotRGB[x * 3 + 1], dotRGB[x * 3 + 2]);
            __delay_ms(50);
            printf("%02x%02x%02x\n", dotRGB[x * 3 + 3], dotRGB[x * 3 + 4], dotRGB[x * 3 + 5]);
            __delay_ms(100);
        }
    }
}

void LCD_ReadTest3(void) {
    uint8_t command;
    uint16_t x, y;

    LCD_SetPosition(0, 5, 152, 159);          //アドレスセット有効
    LCD_spi_open();
    command = LCD_CMD_RAMRD;
    LCD_spi_sendCommand(command);
    SPI1_ExchangeByte(0x00);                //ダミー1バイト捨て読み
    LCD_spi_readVramData(dotRGB, 48 * 3);
    LCD_spi_close();

    printf("H\n");
    for (y = 0; y < 8; y++){
        printf("%02d\n", y);
        for (x = 0; x < (6*3); x+=3){
            printf("%02x%02x%02x ", dotRGB[x+y*18], dotRGB[x+y*18 + 1], dotRGB[x +y*18 + 2]);
            __delay_ms(100);
        }
    }
}
#endif //LCD_READ
