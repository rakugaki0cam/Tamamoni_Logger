/*
 * colorLCD.c
 * 
 * 
 * 
 * 2019.08.10
 * 2019.08.22       �J���[���[�h�@12�A16�A18�r�b�g3��ނɑΉ�
 * 2019.09.16       �ǂݍ��݃e�X�g��ǉ�
 * 2020.08.29       �{�p�t�H���g�Ή�
 * 2020.09.09       DMA�g�p���č�����(spiDMA.c�K�v)
 * 2021.05.02       PIC18F57Q43 48pin SPI1 -> SPI2
 * 2021.05.05       �h��Ԃ��~�f�o�O����
 * 2021.10.12       ���蒆mesure_stage = 1~9�̊ԕ����`�ʂł����B�r���ł����^�[�����邽�߁B
 * 2022.05.07       �����蒆���^�[�������BV8�o�[�W�����@�^�C�g���o�[�\���Ȃ��B�^�[�Q�b�g��ʂ��L���g������
 * 2022.10.23       �������΍� dotRGB 960->580(2�{�p)�ցB�J���[18�r�b�g�݂̂ɁB
 * 2022.10.23       SD��LFN�p�~�Ń������m�ہBdotRGB[1280]
 * 2023.02.01       SPI2 -> SPI1 SD�J�[�h�Ƌ��p��

 * 
 */

#include "header.h"
#include "colorLCD.h"

#define LCD_READ_NONE


//global
uint8_t     RGB_18bit[3];                 // RGB�l����ST7735�̐F���ɕϊ������e�[�u��


//LCD SPI�֘A
void LCD_spi_open(void) {
    //�R�}���h���삷��Ƃ��K�v
    SPI1_Open(LCD_CONFIG);
    LCD_CS_SetLow(); //LCD�`�b�v��I���I��
}

void LCD_spi_close(void) {
    //CS������LCD�ł̌�쓮�h�~�΍��t��
    LCD_spi_sendCommand(LCD_CMD_NOP); //��R�}���h��ł��āA�������[�������݂��I����
    LCD_DC_SetHigh(); //�f�[�^��M�ɂ��Ă����΁@��ǂݍ��݂ƂȂ�
    SPI1_Close();
    LCD_CS_SetHigh(); //LCD�`�b�v�I�����I�t
}

void LCD_spi_sendCommand(uint8_t c) {
    //�R�}���h�𑗂�
    LCD_DC_SetLow(); //�R�}���h
    SPI1_ExchangeByte(c);
}

void LCD_spi_sendByte(uint8_t d) {
    //1�o�C�g�f�[�^�������o��
    LCD_DC_SetHigh(); //�f�[�^
    SPI1_ExchangeByte(d);
}

void LCD_spi_sendWord(uint16_t dd) {
    //2�o�C�g�f�[�^�������o��
    LCD_DC_SetHigh(); //�f�[�^
    SPI1_ExchangeByte((uint8_t)(dd >> 8) & 0x00ff);
    SPI1_ExchangeByte((uint8_t)(dd & 0x00ff));
}

uint8_t LCD_spi_readByte(uint8_t c) {
    //�R�}���h�𑗂�1�o�C�g�f�[�^��ǂݍ���
    uint8_t d;
    //LCD_spi_open();
    LCD_DC_SetLow(); //�R�}���h
    SPI1_ExchangeByte(c);
    LCD_DC_SetHigh(); //�f�[�^
    d = SPI1_ExchangeByte(0x00);
    //LCD_spi_close();
    return d;
}

void LCD_spi_readLongbytes(uint8_t c, uint8_t *data, uint16_t n) {
    //�R�}���h�𑗂�@n�o�C�g�̃f�[�^��ǂݏo��  ID,Status��3�`4�o�C�g�p
    uint16_t i;
    //LCD_spi_open();
    SPI1CON0bits.EN = 0; //SPI1:disable����CON0��ύX��
    SPI1CON0bits.BMODE = 1; //�o���A�u���r�b�g���[�h�ɕύX
    SPI1CON0bits.EN = 1; //SPI1:enable
    LCD_DC_SetLow(); //�R�}���h
    SPI1_ExchangeByte(c);
    LCD_DC_SetHigh(); //�f�[�^
    SPI1TWIDTH = 1; //�f�[�^��1�r�b�g
    //1�r�b�g�̃_�~�[�N���b�N���̂Ă�
    SPI1_ExchangeByte(0x01);
    SPI1TWIDTH = 0; //�f�[�^��8�r�b�g�ɖ߂�
    for (i = 0; i < n; i++) {
        data[i] = SPI1_ExchangeByte(0x00);
    }
    //LCD_spi_close();
}

void LCD_spi_readVramData(uint8_t *data, uint16_t n) {
    //n�o�C�g��VRAM�f�[�^��ǂݏo��
    uint16_t i;
    //LCD_spi_open();
    LCD_DC_SetHigh(); //�f�[�^   
    for (i = 0; i < n; i++) {
        data[i] = SPI1_ExchangeByte(0x00);
    }
    //LCD_spi_close();
}


//LCD���� �T�u���[�`��
void LCD_initialize(void) {
    // �f�B�X�v���C������
#if LCD_RESET_NO == 0
    //�n�[�h�E�G�A���Z�b�g
    LCD_RESET_SetLow();
    __delay_us(10);
    LCD_RESET_SetHigh();
    __delay_ms(120);
#endif
    
    LCD_spi_open();
    LCD_spi_sendCommand(LCD_CMD_SWREST);            //�\�t�g�E�F�A���Z�b�g
    __delay_ms(120);
    LCD_spi_sendCommand(LCD_CMD_SLPOUT);            //�X���[�v���畜�A
    __delay_ms(120);
    LCD_spi_sendCommand(LCD_CMD_COLMOD);            //�J���[���[�h
    LCD_spi_sendByte((COLOR_18BIT << 4) | COLOR_18BIT);
    LCD_spi_sendCommand(LCD_CMD_MADCTL);            //�������[�f�[�^�A�N�Z�X�R���g���[��
    //LCD_spi_sendByte(CMD_MADCTL1 | CMD_MADCTL2);
    LCD_spi_sendByte(CMD_MADCTL2);
    LCD_spi_sendCommand(CMD_INV);                   //���]�\��
    __delay_ms(120);
    // �f�B�X�v���C�̕\���������w�肷�鏈��//////////////////////////////�܂�
    //LCD_DisplayDirection(4) ;
    
    LCD_spi_sendCommand(LCD_CMD_DISPON);            // �f�B�X�v���C�I��
    LCD_spi_close();
    LCD_ClearScreen(0);                             // ��ʂ�0=���A1=���ŏ�������
    __delay_ms(50);                                 //�N���A���I���܂ŒʐM���Ԃ�������������B
}

void LCD_ClearScreen(uint8_t white) {
    //��ʃN���A
    //DMA�ō�����
    uint8_t color;
    uint16_t i, cnt;
    
    //�J���[�R�[�h�̌v�Z
    if (white == 0) {
        color = 0x00;   //BLACK
    } else {
        color = 0xff;   //WHITE
    }
    
    //��1���C�����̃J���[�f�[�^����� 240x3=720�o�C�g
    cnt = DISPLAY_WIDTH * 3;
    for (i = 0; i < cnt; i++ ){
        dotRGB[i] = color;
    }
    
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(0, DISPLAY_WIDTH - 1, 0, DISPLAY_HEIGHT - 1);    //��ʑS��
    LCD_spi_dma(dotRGB, cnt);    //DMA�]��1���C����
    for (i = 1; i < DISPLAY_HEIGHT; i++) {
        LCD_spi_dma_onceagain();    //��������DMA�]���J��Ԃ�
    }
    LCD_spi_Txonly_close();
}

void LCD_SetColor(uint8_t red, uint8_t green, uint8_t blue) {
    //24�r�b�g�J���[�l��18bit�J���[�f�[�^�ɕϊ�
        //1�h�b�g3�o�C�g�@R6+��2, G6+��2, B6+��2 ����2�r�b�g���[��
        //global        /////////////////////////////////////////////////////////////////////���[�J���ɂ�����
        RGB_18bit[0] = red & 0xfc;
        RGB_18bit[1] = green & 0xfc;
        RGB_18bit[2] = blue & 0xfc;
}

void LCD_SetPosition(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1) {
    //���W���Z�b�g
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
    // �������ރt���[���������̗񑤔͈͂��w�肷��
    LCD_spi_sendCommand(LCD_CMD_CASET);
    LCD_spi_sendWord(X_START + x0); // �J�n��
    LCD_spi_sendWord(X_START + x1); // �I����
    // �������ރt���[���������̍s���͈͂��w�肷��
    LCD_spi_sendCommand(LCD_CMD_RASET);
    LCD_spi_sendWord(Y_START + y0); // �J�n�s
    LCD_spi_sendWord(Y_START + y1); // �I���s
    LCD_spi_close();
}


void LCD_SetPixel(uint16_t x0, uint16_t y0) {
    // 1�h�b�g����
    //DMA�ō�����
    dotRGB[0] = RGB_18bit[0];
    dotRGB[1] = RGB_18bit[1];
    dotRGB[2] = RGB_18bit[2];
    
    LCD_spi_Txonly_open();
    LCD_spi_Txonly_address_set(x0, x0, y0, y0);
    //LCD_spi_dma(dotRGB, 3);               //DMA�]��/////////////////////////////////RGB���ł悢�̂ł�??????
    LCD_spi_dma(RGB_18bit, 3);
    LCD_spi_Txonly_close();
}


void LCD_DrawBox(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint16_t     i;
    for (i = x0; i <= x1; i++){
        LCD_SetPixel(i, y0);       //�㑤����
        LCD_SetPixel(i, y1);       //��������
    }
    for (i = y0 + 1; i <= y1; i++){
        LCD_SetPixel(x0, i);       //�����c��
        LCD_SetPixel(x1, i);       //�E���c��
    }
}


void LCD_DrawFillBox(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // (x1,y1)-(x2,y2)��Ίp���Ƃ����h��Ԃ��̎l�p�`��`��
    //DMA�ō�����
    uint16_t i, cnt;
    cnt = 0;
    //��1���C�����̃J���[�f�[�^�����
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
    LCD_spi_dma(dotRGB, cnt);               //DMA�]��1���C����
    for (i = y0 + 1; i <= y1; i++) {
        LCD_spi_dma_onceagain();            //��������DMA�]���J��Ԃ�
    }
    LCD_spi_Txonly_close();
}

//***********�R�s�y�@�@�@�@�݂Â炢

void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r){
    //�~��`��
    //������Ƒ傫���悤��???
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
    //�h��Ԃ��~��`��
    int16_t     x, y;
    int32_t     xx, yy, rr;
    
    // �l�p�`�G���A��(x1,y1)(x2,y2)�ɂ��Ĕ��ar�̉~����h��Ԃ�
    rr = r * r;
    for(y = y0 - r; y < y0 + r; y++) {
        yy = (y - y0) * (y - y0);         //���S�Ƃ̋�����2��(dy^2)
        for(x = x0 - r; x < x0 + r; x++) {
            xx = (x - x0) * (x - x0);    //���S�Ƃ̋�����2��(dx^2)
            if ((xx + yy) <= rr){        //�~�̌��� x^2 + y^2 = r^2
                LCD_SetPixel(x, y);     //�~���Ȃ̂Ńh�b�g�`��
            }
        }
    }
}


void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    //����`�悷��@DDA�@
    bool        incline;
    int16_t     x, y;
    int16_t     tmp;
    int16_t     dx, dy;     //����
    int16_t     e, ys;      //e:y��
	
    //�����̔���
    incline = (abs(y1 - y0) > abs(x1 - x0));
    //�������傫������x�ɂ���(y�̑������傫���Ɠ_���ɂȂ��Ă��܂�)
    if (incline){
        tmp = x0;
        x0 = y0;
        y0 = tmp;
        
        tmp = x1;
        x1 = y1;
        y1 = tmp;
    }
    //�n�_ x0 < x1 �I�_�Ƃ���(������E�֕`��)
    if (x0 > x1) {
        tmp = x0;
        x0 = x1;
        x1 = tmp;
        
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }
    
    //�����̌v�Z 
    dx = x1 - x0;
    dy = abs(y1 - y0);
    e = 0;
    y  = y0;
    
    //�X���ŃX�e�b�v�̐�����؂�ւ�
    if (y0 < y1){
        ys = 1; 
    }else {
        ys = -1;
    }
    
    //�_�ŕ`��
    for(x = x0; x <= x1; x++) {
        if (incline){
            LCD_SetPixel(y, x); 
        }else{
            LCD_SetPixel(x, y);
        }
        e += dy;
        if((e << 1) >= dx){
            //y�̑�����x�̑�����菬�����̂ŉ��񂩓���y�ʒu������
            y += ys;
            e -= dx;
        }
    }
}

//***����

void LCD_PutCHR(uint16_t x0, uint16_t y0, uint8_t asc_code, uint8_t font_size, uint8_t background) {
    //1������\�� 5x7font
    //DMA�ꕔ�g�p
    //�T�C�Y�@�g��{���@1: 5x7, 2: 10x14, 3: 15x21 ...
    //�w�i�ɏd�˂ĕ\�� background=0;--->DMA�ł͖���
    //�w�i�F���@background=1
    uint16_t    x1, y1;
    uint16_t    cnt;
    uint8_t     fontx, fonty, dotxy, fontdata;

    if (background == 1){
        //�w�i�F���@background = 1�̂Ƃ�DMA�ō����`��
        cnt = (6 * font_size) * (8 * font_size) * 3;    //18bitRGB = 3byte�g��
        LCD_FontData_Generate(asc_code, font_size, 0);
      
        x1 = x0 + 6 * font_size - 1;
        y1 = y0 + 8 * font_size - 1;

        LCD_spi_Txonly_open();
        LCD_spi_Txonly_address_set(x0, x1, y0, y1);
        LCD_spi_dma(dotRGB, cnt);               //DMA�]��
        LCD_spi_Txonly_close();
    }else{
        //�w�i�ɏd�˂ĕ`�� background�@=�@0�@�̂Ƃ�
        //Txonly���g�p
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
                LCD_spi_Txonly_address_set(x1, x1 + font_size - 1, y1, y1 + font_size - 1);     //�E����
                for (dotxy = 0; dotxy < (font_size * font_size); dotxy++){
                    //�_��ł�
                    if (((fontdata >> fonty) & 0x01) == 1) {        //�h�b�g����
                        LCD_spi_Txonly_sendByte(RGB_18bit[0]);
                        LCD_spi_Txonly_sendByte(RGB_18bit[1]);
                        LCD_spi_Txonly_sendByte(RGB_18bit[2]);     
                    }else if (background == 1){                 //�h�b�g�Ȃ�
                        //�w�i�F����
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
    //�������lprint���ɕ\�� �����ԃs�b�`6�h�b�g�@1�s8�h�b�g x �{��
    //�J���[�R�[�h��3^3 = 27�F  00,01,11  10�͖���
    uint8_t     rgb2[3], i;
    uint16_t    x1, y1;
    uint16_t    cnt;
    uint8_t     mem_page = 0;
    
    if (font_size > 2){
        //�t�H���g�T�C�Y3�{�p�ȏ��DMA�s�̂��ߔw�i�d�˃��[�h�ŏ�������
        bgb = 0;
    }
    
    //�F�̃Z�b�g 27�F
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
                //�ق�Ƃ͖����Ȃ񂾂���
                rgb2[i] = 0x7f;
                break;
            case 3:
                rgb2[i] = 0xff;
                break;    
        }
    }
    LCD_SetColor(rgb2[0], rgb2[1], rgb2[2]);
    
    //������\��
    if (bgb == 0){
        //�w�i�ɏd�ˏ���
        while (*string) {
            LCD_PutCHR(x0, y0, *string++, font_size, bgb);
            x0 += (6 * font_size); //����1�h�b�g�󂯂�ׂ�5+1=6�Ƃ���
        }   
    }else{
        //�w�i��
        //DMA���g���č����ɕ������\��
        cnt = (6 * font_size) * (8 * font_size) * 3;

        //DMA�̐ݒ�
        DMASELECT = 0x00;
        DMAnCON0bits.EN = 0;                    //DMA1 reset
                                                //DMAnSCNT�ɂ̓J�E���g�A�b�v����DMAnSSZ�������[�h����Ă���̂�
                                                //��xEN=0���Z�b�g���Ȃ��Ə����ύX�͊�������Ȃ�
        //DMA1_SetSourceAddress((uint24_t)dotRGB);
        //DMA1_SetSourceSize(cnt);
        DMASELECT = 0x00;
        DMAnSSA = (uint24_t)dotRGB;
        DMAnSSZ = cnt;
        
        PIR2bits.DMA1SCNTIF = 1;               //DMA�]��������Ԃɂ��Ă���

        LCD_spi_Txonly_open();
        while (*string) {
            //DMA�]�����Ɏ��̃f�[�^��ύX�ł���
            LCD_FontData_Generate(*string++, font_size, mem_page);
            
            x1 = x0 + 6 * font_size - 1;
            y1 = y0 + 8 * font_size - 1;    //���s�͖���
            
            while(PIR2bits.DMA1SCNTIF == 0){
                //DMA�]�������҂�
            }
            DMASELECT = 0x00;
            DMAnCON0bits.EN = 0;                    //DMA1 reset
            DMAnSSA = (uint24_t)dotRGB;
            if (mem_page == 1){
                DMAnSSA += 640;
            }
            PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
            
            LCD_spi_Txonly_address_set(x0, x1, y0, y1);
            while(SPI1INTFbits.SRMTIF == 0){}       //���M���łȂ����̃`�F�b�N
            LCD_DC_SetHigh();                       //Data
    
            //EN enabled; SIRQEN enabled; DGO not in progress; AIRQEN disabled; 
            DMASELECT = 0x00;
            DMAnCON0 = 0xC0;                        //DMA�]��
            
            x0 += (6 * font_size);                  //����1�h�b�g�󂯂�ׂ�5+1=6�Ƃ���
            mem_page ^= 0x01;                       //�y�[�W�����ւ�

        }      
        while(PIR2bits.DMA1SCNTIF == 0){}       //DMA�]�������J�E���g���[��
        while(SPI1INTFbits.SRMTIF == 0){}       //SPI���̑��M���������o
        PIR2 &= 0x0f;                           //bit7~5:DMA1  -> clear
        DMASELECT = 0x00;
        DMAnCON0bits.EN = 0;                    //DMA1 reset
        
        LCD_spi_Txonly_close();
    }
}


void LCD_FontData_Generate(uint8_t asc_code, uint8_t font_size, uint8_t mem_page){
    //1�������̃J���[�f�[�^����� -> dotRGB[]�@6x8�t�H���g�̎� 6pixel x 8pixel x 3byteRBG = 144byte
    //DMA�]���p
    uint8_t     fontx, fonty, dotx, doty , fontdata;
    uint16_t    index_d;
    
    if(font_size > 2){
        //DMA��2�{�p�܂� 2�{�p�̎�12pixelx16pixelx3byteRGB = 576bytes�K�v
        font_size = 2;
    }
    
    for (fontx = 0; fontx < 6; fontx++) {
        if (fontx < 5){
            fontdata = font5x7[asc_code - 0x20][fontx];
        }else{
            fontdata = 0x00;
        }
        for (fonty = 0; fonty < 8; fonty++) {
            //�t�H���g�f�[�^�͍ŉ��ʃr�b�g����
            for (doty = 0; doty < font_size; doty++){
                for (dotx = 0; dotx < font_size; dotx++){
                    index_d = ((fontx * (uint16_t)font_size + dotx) + ((fonty * (uint16_t)font_size + doty) * (6 * (uint16_t)font_size))) * 3;
                    if (mem_page == 1){
                        index_d += 640;
                    }
                    //printf("i%2d j%2d a%3d\n", i, j, ab);
                    if (((fontdata >> fonty) & 0x01) == 1) {    //�h�b�g����
                        dotRGB[index_d] = RGB_18bit[0];
                        dotRGB[index_d + 1] = RGB_18bit[1];
                        dotRGB[index_d + 2] = RGB_18bit[2];
                    }else {                                     //�h�b�g�Ȃ�
                        //�w�i�F=��
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
    LCD_SetColor(0xb0, 0x80, 0x30);                       //�J���V�F
    LCD_DrawFillBox(5, 60, DISPLAY_WIDTH - 6, 110);
    sprintf(tmp_string, "%s", title);
    LCD_Printf(10, 70, tmp_string, 2, BLACK, 0);
    sprintf(tmp_string, "ver.%s", version);
    LCD_Printf(130, 95, tmp_string, 1, BLACK, 0);
}


void LCD_Title_Clear(void){
    //�^�C�g������
    __delay_ms(2);
    LCD_SetColor(0x0, 0x0, 0x0);                        //��
    LCD_DrawFillBox(0, 50, DISPLAY_WIDTH - 1, 150);
}



/*// test
void LCD_ColorSample(void){
    //�F���{
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
    LCD_ClearScreen(0);                                 // ��ʂ�0=���A1=���ŏ�������
}
*/

#ifdef LCD_READ
void LCD_ReadTest(void) {
    //LCD�f�[�^�̓ǂݏo���e�X�g
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

    LCD_SetPosition(0, 319, 0, 239);          //�A�h���X�Z�b�g�L��
    for (y = 0;y < 2;y++){
        //1���C�����ǂݍ���
        LCD_spi_open();
        command = LCD_CMD_RAMRD;
        LCD_spi_sendCommand(command);
        SPI1_ExchangeByte(0x00);                //�_�~�[1�o�C�g�̂ēǂ�
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

    LCD_SetPosition(0, 5, 152, 159);          //�A�h���X�Z�b�g�L��
    LCD_spi_open();
    command = LCD_CMD_RAMRD;
    LCD_spi_sendCommand(command);
    SPI1_ExchangeByte(0x00);                //�_�~�[1�o�C�g�̂ēǂ�
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
