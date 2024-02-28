/*
 * touch_2046.c
 * 
 * タッチパネル
 * 感圧式
 * TSC2046
 * 
 * 
 * 2019.09.15
 * 
 * 2020.07.28   縦長方向に使う時の設定
 * 2022.10.22   ボタン処理追加 ←LC-LCD28_2 PIC32MZより移植
 *              TOUCH_INTはポートで見る。変化割込だと一度離さないと入力しなくなる。
 *              タッチの瞬間はZT=500～600くらいあるようで、判定値が違ってくる????
 * 2022.10.26   初期化でPDモード00として、検出待機中にする。->PENIRQを有効にする
 * 2022.10.27   LCD基板で外部Vrefが3.3Vに接続されていて内部リファレンス2.50V使用不可。
 * 2023.02.01   SPI2->SPI1 SDcardと共用
 * 
 */

#include "header.h"
#include "touch_2046.h"


#define PORTRAIT_DISPLAY
#define LANDSCAPE_DISPLAY_no
#define DEBUG_no                        //debug UART -> printf

//位置補正を考えて変数にしている
#ifdef  LANDSCAPE_DISPLAY
//横長
uint16_t disp_touch_x_start = 3850;     //実測値　個々の差がある
uint16_t disp_touch_x_end   = 350;
uint16_t disp_touch_y_start = 130;
uint16_t disp_touch_y_end   = 3850;
#else
//縦長
uint16_t disp_touch_x_start = 3830;     //実測値　個々の差がある
uint16_t disp_touch_x_end   = 280;
uint16_t disp_touch_y_start = 3870;
uint16_t disp_touch_y_end   = 400;
#endif

float disp_touch_x_gain;
float disp_touch_y_gain;


void touch_init(void){
    //タッチパネル初期設定
    
#define     VREF            3.3     //ピン9-外部Vref
#define     TEMP_OFFSET     10      //温度補正値
    
    uint8_t     dummy, ans1, ans2;;
    uint16_t    temp0, temp1;
    float       vt0, vt1, temp;
    
    printf("Touch INIT...");
            
    disp_touch_x_gain = (float)(DISPLAY_COLUMN_e - DISPLAY_COLUMN_s + 1) / (int16_t)(disp_touch_x_end - disp_touch_x_start);
    disp_touch_y_gain = (float)(DISPLAY_ROW_e - DISPLAY_ROW_s + 1) / (int16_t)(disp_touch_y_end - disp_touch_y_start);
    
    TOUCH_CS_SetLow();                  //TOUCHチップを選択オン
    SPI1_Open(TOUCH_CONFIG);
    //温度計測　　　うまくいかない
    //PDモードのリファレンスは内部。外部refがつながっているのでONにできない。
    dummy = SPI1_ExchangeByte(0b10000101);      //0x85  Temp0　dummy read
    dummy = SPI1_ExchangeByte(0x00);
    dummy = SPI1_ExchangeByte(0x00);
    __delay_ms(3);
    dummy = SPI1_ExchangeByte(0b10000101);      //0x85  Temp0
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);
    temp0 = (uint16_t)((ans1 << 8) + ans2) >> 3;
    vt0 = (float)(VREF * temp0 / 4096 * 1000);
    
    dummy = SPI1_ExchangeByte(0b11110101);      //0xf5  Temp1
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);
    temp1 = (uint16_t)((ans1 << 8) + ans2) >> 3;
    vt1 = (float)(VREF * temp1 / 4096 * 1000);
    //temp = 2.573 * (vt1 - vt0) - 273;           //degC
    temp = (float)(0.3141 * VREF * (vt1 - vt0) - 273);     //degC　だめ全然違う
    
    //パワーダウンモードを00:検出待機状態にする
    dummy = SPI1_ExchangeByte(0b10000100);      //0x84 
    dummy = SPI1_ExchangeByte(0x00);
    dummy = SPI1_ExchangeByte(0x00);
    
    SPI1_Close();
    TOUCH_CS_SetHigh();                 //TOUCHチップを選択オフ
    
    if (vt0 == 0){
        printf("error?\n");
    }else{
        printf("OK!\n");
        //debug
        //printf("temp0 = 0x%03X\n", temp0);
        //printf("temp1 = 0x%03X\n", temp1);
        //printf("Vt0 =%7.2fmV\n", vt0);
        //printf("Vt1 =%7.2fmV\n", vt1);
        //temp0からの温度
        //printf("temp0 =%6.1fdegC\n", (600 - vt0) / 2.1 + 25 + TEMP_OFFSET);
        //temp01からの温度
        //printf("temp01=%6.1fdegC\n", temp);   ///////////合わない??????????????
    }
    
}
    

bool touch_xyz(uint16_t *t_data){
    //タッチ座標をAD変換値から求める
    //返り値:   0:タッチ検出、1:タッチ無し
    //         t_data 座標x,座標y,接触圧z
     
#define     SUM_A       3               //2^3　= 8回
#define     SUM_NUM     1 << SUM_A      //平均サンプル数
//touch data
enum{
    POINT_X,
    POINT_Y,
    TOUCH_ZT,
};
//adc_data
enum{
    X,
    Y,
    Z1,
    Z2,
    NUM_OF_ADC,
};
    uint16_t    adc_data[NUM_OF_ADC];    //測定値
    uint8_t     n, i;
    uint16_t    sum_data[NUM_OF_ADC] = {0, 0, 0, 0};    //平均計算用
    uint16_t    data[NUM_OF_ADC];       //平均値

    //測定値の平均を取る 
    for(n = 0; n < SUM_NUM; n++){
        touch_adc(adc_data);
        for(i = 0; i < 4; i++){
            sum_data[i] += adc_data[i];
        }
    }
    for(i = 0; i < 4; i++){
            data[i] = sum_data[i] >> SUM_A;
        }
    
#ifdef  LANDSCAPE_DISPLAY
    t_data[TOUCH_ZT] = data[Y] /(float)4096 * 100 * ((float)data[Z2] / data[Z1] - 1);
#else
    t_data[TOUCH_ZT] = (uint16_t)(data[X] /(float)4096 * 100 * ((float)data[Z2] / data[Z1] - 1));
#endif 
    
#if  DEBUG_AD_VALUE == 1
    printf("zt:%3d\n", t_data[TOUCH_ZT]);
#endif 
    
    if ((t_data[TOUCH_ZT] > 50) && (t_data[TOUCH_ZT] < 700)){              //50~190(~2022/10/22) 
        //押し圧が適正の時だけタッチポイント有効(タッチの瞬間は500～700くらいのよう)
        t_data[POINT_X] = (uint16_t)((int16_t)(data[X] - disp_touch_x_start) * disp_touch_x_gain);
        t_data[POINT_Y] = (uint16_t)((int16_t)(data[Y] - disp_touch_y_start) * disp_touch_y_gain);

#if DEBUG_SET_PIXEL == 1
        LCD_SetColor(0x00, 0x00, 0xff);     //BLUE
        LCD_SetPixel(t_data[POINT_X], t_data[POINT_Y]);
        //DEBUGger
        printf("t. x:%3d y:%3d z:%3d\n", t_data[POINT_X], t_data[POINT_Y], t_data[TOUCH_ZT]);
#endif
    }else{
        return 1;
    }
    return 0;
}
    

void touch_adc(uint16_t * data){
    //タッチ測定値
    //data  x,y,z1,z2
    enum{
        X,
        Y,
        Z1,
        Z2,
    };
    
    uint8_t     dummy, ans1, ans2;
    
    TOUCH_CS_SetLow();                  //TOUCHチップを選択オン
    SPI1_Open(TOUCH_CONFIG);

    dummy = SPI1_ExchangeByte(0x91);    //計測コマンド+PD:01変換ー変換
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);    //計測値はタイミングの都合で3ビット右シフトで12bitデータとなる
    
#ifdef  LANDSCAPE_DISPLAY
    data[X] = ((ans1 << 8) + ans2) >> 3;   //横長
#else
    data[Y] = (uint16_t)(((ans1 << 8) + ans2) >> 3);   //縦長
#endif
    
    dummy = SPI1_ExchangeByte(0xb1);
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);
    data[Z1] = (uint16_t)(((ans1 << 8) + ans2) >> 7);   //8bitにしておく

    dummy = SPI1_ExchangeByte(0xc1);
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);
    data[Z2] = (uint16_t)(((ans1 << 8) + ans2) >> 7);   //8bitにしておく

    dummy = SPI1_ExchangeByte(0xd0);    //計測コマンド+PD:00待機モード　PENIRQを有効に
    ans1  = SPI1_ExchangeByte(0x00);
    ans2  = SPI1_ExchangeByte(0x00);
#ifdef  LANDSCAPE_DISPLAY
    data[Y] = (uint16_t)(((ans1 << 8) + ans2) >> 3);    //横長
#else
    data[X] = (uint16_t)(((ans1 << 8) + ans2) >> 3);    //縦長
#endif 
    SPI1_Close();
    TOUCH_CS_SetHigh();                 //TOUCHチップを選択オフ
    
#if  DEBUG_AD_VALUE == 1
    printf("x:%4d y:%4d \n", data[X], data[Y]);
    printf("z1:%4d z2:%4d\n", data[Z1], data[Z2]);
#endif 

}


//touch button

void button_init(const uint16_t button[][4], uint8_t button_len){
    //ボタンの枠を初期表示
    //button[][]:ボタンのデータ
    //button_len:ボタンデータの数
    uint8_t i;
    
    //ボタンナンバー0はダミーなので表示しない
    for(i = 1; i < button_len; i++){
        LCD_SetColor(0x40, 0x40, 0x40);
        button_select_disp(i, 0, button, button_len);       //ボタンの枠グレイ表示
    }
}


uint8_t button_detect(uint16_t xt, uint16_t yt, const uint16_t button[][4], uint8_t button_len){
    //タッチポイントがボタン上かの判定
    //ボタン番号を返す
    //button[][]:ボタンのデータ
    //button_len:ボタンデータの数
    
//ボタン番号モニタ位置  
#define     BUTTON_COL  (6*24)
#define     BUTTON_ROW  (8*36)
    
    uint8_t     bo = 0;
    uint8_t     i;
    
    for (i = 0; i < button_len; i++){
        if ((xt >= button[i][0]) && (xt <= button[i][0] + button[i][2])
         && (yt >= button[i][1]) && (yt <= button[i][1] + button[i][3])){
            bo = i;
            //ボタン選択表示(黄色)
            button_select_disp(bo, 1, button, button_len);
            __delay_ms(100);
            //枠グレイ表示で消去
            button_select_disp(bo, 0, button, button_len);
            break;
        }
    }
#if DEBUG_BUTTON == 1
    //デバッグ用ボタン番号表示
    sprintf(tmp_string, "button = %2d", bo);
    LCD_Printf(BUTTON_COL, BUTTON_ROW, tmp_string, 1, GREEN, 1);
#endif
    return bo;
}


uint8_t button_number(const uint16_t button[][4], uint8_t button_len){
    //ボタンナンバーを返す
    //button[][]:ボタンのデータ
    //button_len:ボタンデータの数
    uint16_t touch[3];
    uint8_t a;
    
    if (touch_xyz(touch) != 0){     //タッチ検出
        return 0;
    }
    a = button_detect(touch[0], touch[1], button, button_len);

    return a;  
}


void button_select_disp(uint8_t num, bool stat, const uint16_t button[][4], uint8_t button_len){
    //番号numのボタン枠をstat=1:表示、0:消去
    //button[][]:ボタンのデータ
    //button_len:ボタンデータの数
    if ((num <= 0) || (num >= button_len)){
        //ボタン番号エラー
        return;
    }
    if (stat == 1){
        //ボタン選択表示(黄色)
        LCD_SetColor(0xff, 0xff, 0x00);
    }else{
        //消去(枠グレイ表示)
        LCD_SetColor(0x60, 0x60, 0x60);
    }
    
    LCD_DrawBox(button[num][0] - 4,
                button[num][1] - 4,
                button[num][0] + button[num][2] + 1,
                button[num][1] + button[num][3] + 1);
}