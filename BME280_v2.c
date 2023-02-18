/* 
 * File:   BME280_v2.c
 * Author: IsobeR
 * Comments: PIC18F26K83 MCC
 * 
 * BOSCH BME280 温度　湿度　気圧センサー　モジュール
 * 
 * 　I2C
 * 
 * Revision history: 
 * 2020.08.09   BME280Q10.cより移植　I2Cの記述が変更
 * 2020.09.03   BME280_read廃止、BME280_dispで読み込みと表示
 * 2021.05.05   [(条件)? a : b]演算子をif文に展開
 * _v2
 * 2021.09.26   表示を小分けに
 * 
 */

#include "header.h"
#include "BME280_v2.h"


#define     BME280_ADDRESS  0x76    //I2C接続 BOSCH BME280 温湿度気圧センサーモジュール
//display
#define     ROW_BME280      300
#define     COL_BME280      110
//ascii code
#define     DEG             0xdf    // deg = 'ﾟ'  度の半角カナ  
#define     DEG_C           0x8b    // degC = '℃'  の半角カナ  

//local
unsigned long   Temp_OUT_raw, Pres_OUT_raw, Humi_OUT_raw;
signed long     Data_Temp_32;
unsigned long   Data_Pres_32,Data_Humi_32;
float           Pr,Hu,Te;
    
signed long     t_fine;         //精密気温　気圧補正にも使用
unsigned int    dig_T1;        //センサー補正値
  signed int    dig_T2;
  signed int    dig_T3;
 
unsigned int    dig_P1;
  signed int    dig_P2;
  signed int    dig_P3;
  signed int    dig_P4;
  signed int    dig_P5;
  signed int    dig_P6;
  signed int    dig_P7;
  signed int    dig_P8;
  signed int    dig_P9;
 
unsigned char   dig_H1;
  signed int    dig_H2;
unsigned char   dig_H3;
  signed int    dig_H4;   //12bit
  signed int    dig_H5;   //12bit
  signed char   dig_H6;

  
unsigned char BME280_initialize(void){
    //initialize
    printf("BME280 INIT...");
    if (I2C1_Read1ByteRegister(BME280_ADDRESS, 0xd0) == 0x60){         //ID return value 0x60
    //if (i2c_master_read1byte(BME280_ADDRESS, 0xd0) == 0x60){         //ID return value 0x60
        __delay_ms(100);
    }else{
        printf("ERROR\n");
        //エラーを返す
        LED_RIGHT_SetHigh();
        LED_LEFT_SetHigh();
        __delay_ms(1000);
        return 1;
    }
    //Config
    //oversampling
    unsigned char osrs_h = 0b010;   //000:skip, 001:x1, 010:x2, 011:x4, 100:x8, 101:x16
    unsigned char osrs_t = 0b010;
    unsigned char osrs_p = 0b011; 
    unsigned char mode = 0b11;      //mode 00:Sleep, 01:ForceMode, 11:NormalMode
    unsigned char t_sb = 0b101;     //Tstandby 000:0.5ms, 001:62.5ms 010:125ms, 011:250ms, 100:500ms, 101:1sec, 110:10ms, 111:20ms
    unsigned char filter = 0b010;   //IIRfilter 000:off, 001:2, 010:4, 011:8, 100:16
    
    I2C1_Write1ByteRegister(BME280_ADDRESS, 0xf2, (uint8_t)osrs_h);                                  //ctrl-hum
    I2C1_Write1ByteRegister(BME280_ADDRESS, 0xf4, (uint8_t)((osrs_t << 5) + (osrs_p << 2) + mode));    //ctrl-meas
    I2C1_Write1ByteRegister(BME280_ADDRESS, 0xf5, (uint8_t)((t_sb << 5) + (filter << 2)));             //config
    
    Trim_Read();
    printf("OK!\n");
    return 0;
}

uint8_t BME280_disp(bool full_disp){
    //環境センサー　読み出しと表示
    //full_dispの時は分割処理しない
    uint8_t         BME[8];
    static uint8_t  disp_stage = 0;
    
    do{
        switch (disp_stage){
            case 0:
            case 1:
                //データ読み込み
                I2C1_ReadDataBlock(BME280_ADDRESS,0xF7,BME,8);
                //気圧　気温　湿度　の順
                Pres_OUT_raw = ((unsigned long)BME[0] << 12) | ((unsigned int)BME[1] << 4) | (BME[2] >> 4);
                Temp_OUT_raw = ((unsigned long)BME[3] << 12) | ((unsigned int)BME[4] << 4) | (BME[5] >> 4);
                Humi_OUT_raw = ((unsigned int) BME[6] << 8 ) | BME[7];
                //型をつけておかないと、ビットシフトした際にちょんぎれている
                disp_stage = 2;
                break;
            case 2:
                //温度の計算
                Data_Temp_32 = compensate_T((signed long)Temp_OUT_raw);
                // x0.01deg -40~85deg (-4000~0~8500 = 0xf060~0xffff,0x0~0x2134)
                disp_stage = 3;
                break;
            case 3:
                //湿度の計算
                Data_Humi_32 = compensate_H((signed long)Humi_OUT_raw);             ///user
                // /1024 %RH  0~100%RH  (0~102400 = 0x0~0x19000)
                disp_stage = 4;
                break;
            case 4:
                //気圧の計算
                Data_Pres_32 = compensate_P((signed long)Pres_OUT_raw);             ///user
                // x0.01hPa 300~1100hPa (0~110000 = 0x00~0x1adb0)
                disp_stage = 5;
                break;
            case 5:
                //フル表示のときのみ
                //LCD
                if(full_disp == 1){
                    BME280_frame_disp();
                }
                disp_stage = 6;
            case 6:
                //温度の表示
                //Temp = Data_Temp_32;
                Te = (float)((float)Data_Temp_32 / (float)100);
                sprintf(tmp_string, "%4.1f", Te);
                LCD_Printf(COL_BME280, ROW_BME280, tmp_string, 1, WHITE, 1); 
                disp_stage = 7;
                break;
            case 7:
                //湿度の表示
                //Humi = (((unsigned long)(Data_Humi_32) * 100)) >> 10;
                Hu = (float)((float)Data_Humi_32 / (float)1024);
                sprintf(tmp_string, "%3.0f", Hu);
                LCD_Printf((COL_BME280 + 7 * 6), ROW_BME280, tmp_string, 1, WHITE, 1); 
                disp_stage = 8;
                break;
            case 8:
                //気圧の表示
                //Pres = ((unsigned long)Data_Pres_32 / 10);
                Pr = (float)((float)Data_Pres_32 / (float)100);
                sprintf(tmp_string, "%6.1f", Pr);
                LCD_Printf((COL_BME280 + 12 * 6), ROW_BME280, tmp_string, 1, WHITE, 1); 
                disp_stage = 9;
                break;
            case 9: 
                //LOG data
                sprintf( &bullet_CSVdata[TEMP][0]      , "%7.2f", Te);
                sprintf( &bullet_CSVdata[HUMIDITY][0]  , "%7.2f", Hu);
                sprintf( &bullet_CSVdata[BARO_PRESS][0], "%8.2f", Pr);
                //UART Tx
                //printf("%4.1fC %3.0f%% ", Te, Hu);
                //printf("%6.1fhPa\n", Pr);
                //csv
                disp_stage = 0;
                break;
        }
    } while((full_disp == 1) & (disp_stage != 0));

#ifdef  TIMING_LOG              //debug
        DEBUG_timing_log(17);   //BME280 return
#endif
        
    return disp_stage;
}

void BME280_frame_disp(void){
    //フレームのみ表示
    sprintf(tmp_string, "    %cC    %%       hPa", DEG);
    LCD_Printf(COL_BME280, ROW_BME280, tmp_string, 1, WHITE, 1); 
}

            
void Trim_Read(void){
    //補正値の読み出し
    unsigned char da[24];
    I2C1_ReadDataBlock(BME280_ADDRESS, 0x88, da, 24);  
    dig_T1 = (unsigned int)((da[1] << 8) | da[0]);      //0x88/0x89 -> dig_T1 [7:0]/[15:8]      //user
    dig_T2 = (da[3] << 8) | da[2];                      //0x8A/0x8B -> dig_T2 [7:0]/[15:8]
    dig_T3 = (da[5] << 8) | da[4];                      //0x8C/0x8D -> dig_T3 [7:0]/[15:8]
    
    dig_P1 = (unsigned int)((da[7] << 8) | da[6]);      //0x8E/0x8F -> dig_P1 [7:0]/[15:8]      //user
    dig_P2 = (da[9] << 8) | da[8];                      //0x90/0x91 -> dig_P2 [7:0]/[15:8]
    dig_P3 = (da[11]<< 8) | da[10];                     //0x92/0x93 -> dig_P3 [7:0]/[15:8]
    dig_P4 = (da[13]<< 8) | da[12];                     //0x94/0x95 -> dig_P3 [7:0]/[15:8]
    dig_P5 = (da[15]<< 8) | da[14];                     //0x96/0x97 -> dig_P3 [7:0]/[15:8]
    dig_P6 = (da[17]<< 8) | da[16];                     //0x98/0x99 -> dig_P3 [7:0]/[15:8]
    dig_P7 = (da[19]<< 8) | da[18];                     //0x9A/0x9B -> dig_P3 [7:0]/[15:8]
    dig_P8 = (da[21]<< 8) | da[20];                     //0x9C/0x9D -> dig_P3 [7:0]/[15:8]
    dig_P9 = (da[23]<< 8) | da[22];                     //0x9E/0x9F -> dig_P3 [7:0]/[15:8]
    
    
    dig_H1 = I2C1_Read1ByteRegister(BME280_ADDRESS, 0xa1);  //0xA1       -> dig_H1 [7:0]
    
    I2C1_ReadDataBlock(BME280_ADDRESS, 0xe1, da, 7);
    dig_H2 = (da[1] << 8) | da[0];                      //0xE1/0xE2      -> dig_H2 [7:0]/[15:8]
    dig_H3 =  da[2];                                    //0xE3           -> dig_H3 [7:0]
    dig_H4 = (da[3] << 4) | (da[4] & 0x0F);             //0xE4/0xE5[3:0] -> dig_H4 [11:4]/[3:0] 12bit
    dig_H5 = (da[5] << 4) | ((da[4] & 0xF0) >> 4);      //0xE5[7:4]/0xE6 -> dig_H5 [3:0]/[11:4] 12bit
    dig_H6 = (signed char)da[6];                        //0xE7           -> dig_H6 [7:0]                //user
   
}

signed long compensate_T(signed long adc_T){
    //温度補正計算
    //"5123" = 51.23 deg C
    signed long var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((signed long)dig_T1 << 1))) * ((signed long)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long)dig_T1)) * ((adc_T >> 4) - ((signed long)dig_T1))) >> 12) * ((signed long)dig_T3)) >> 14;
    t_fine = var1 + var2;               //Fine Temperature (Global value)
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}

unsigned long compensate_P(signed long adc_P){
    //気圧の補正計算 (t_fineが必要)
    //"96386" = 963.86hPa
    signed long var1, var2;
    unsigned long p;
    var1 = (((signed long)t_fine) >> 1) - (signed long)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long)dig_P6);
    var2 = var2 + ((var1 * ((signed long)dig_P5)) << 1);
    var2 = (var2 >> 2) + (((signed long)dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((signed long)dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((signed long)dig_P1)) >> 15);
    if (var1 == 0){
        //0で割ることになった場合
        return 0;
    }    
    p = (((unsigned long)(((signed long)1048576) - adc_P) - (unsigned long)(var2 >> 12))) * 3125;       //user
    if (p < 0x80000000){
        p = (p << 1) / ((unsigned long) var1);   
    } else {
        p = (p / (unsigned long)var1) * 2;    
    }
    var1 = (((signed long)dig_P9) * ((signed long)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((signed long)(p >> 2)) * ((signed long)dig_P8)) >> 13;
    p = (unsigned long)((signed long)p + ((var1 + var2 + dig_P7) >> 4));
    return p;
}

unsigned long compensate_H(signed long adc_H){
    //湿度の補正計算 (t_fineが必要)
    //"47445" = 47445/1024 = 46.333%RH
    signed long v_x1;
    
    v_x1 = (t_fine - ((signed long)76800));
    v_x1 = (((((adc_H << 14) -(((signed long)dig_H4) << 20) - (((signed long)dig_H5) * v_x1)) + 
              ((signed long) 16384)) >> 15) * (((((((v_x1 * ((signed long)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long)dig_H3)) >> 11) + ((signed long) 32768))) >> 10) + ((signed long) 2097152)) * 
              ((signed long) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long)dig_H1)) >> 4));
   
   // [(条件)? a : b]演算子はコンパイルされないようなのでif文に展開
   //v_x1 = (v_x1 < 0 ? 0 : v_x1);
   if (v_x1 < 0){
        v_x1 = 0;
   }
   //v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   if (v_x1 > 419430400){
        v_x1 = 419430400;
   }
   
   return (unsigned long)(v_x1 >> 12);   
}



