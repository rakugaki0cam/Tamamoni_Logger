/*
 * SDcard.c
 * 
 * V8edition
 * 
 * 
 * 2020.08.30
 *     
 * 2020.09.12   ヘッダに日付時刻を追加
 * 2021.01.11   ファイルネーム末尾2桁を日付に
 * 2021.05.12   ロングファイルネーム
 * 2021.09.21   ファイル名の後ろにaを追加
 * 2021.09.25   ファイル名の後ろにa無し。motion書き込みをmotion.cから移動
 * 2022.05.05   V8 電子ターゲット着弾位置,マト装置モードのデータ記録を追加
 * 2022.10.19   ホップ回転数測定ログ項目を追加
 * 2022.10.23   ショートファイルネーム。データメモリー不足のため→2.6kほど空いた。
 * 2022.11.01   ファイル名拡張子.txtから.csvに変更
 * 
 */

#include "header.h"
#include "SDcard.h"


//GLOBAL
char    bullet_CSVdata[BULLET_ITEM_NUM][LEN_BLL];   //SDカードへ出すデータの収納用[項目数][データの文字長]
char    motion_CSVdata[MOTION_ITEM_NUM][LEN_MOT];   //モーションデータ
bool    ShotHeader_write_flag = 0;                  //SDファイルにヘッダーを書き込んだかのフラグ



//LOCAL
char    filename_bullet [] = "BU000000A.CSV ";      //ファイルネーム Bullet LOG (大文字小文字区別無し)
char    filename_motion [] = "MO000000A.CSV ";      //ファイルネーム Motion LOG
char    filename_timelog[] = "TI000000A.CSV ";      //ファイルネーム Time LOG
bool    data_saved_flag;                            //SDファイルにデータを書き込んだか:1、まだなにも書き込んでいないか:0(ファイルネームの接尾辞を更新するかしないかの判断)

//エラー文
char    SDerror_strings1[20];               //エラー文　18文字
char    SDerror_strings2[20];               //エラー文　18文字


void generate_filename_date(void){
    //ファイルネームに日付を組み込む
#define     START_SUFFIX    0x41    //"A"
#define     END_SUFFIX      0x5a    //"Z"

    uint8_t date[7];
    uint8_t year, month, day, suffix;
    
    //EEPROMから読み出し
    year     = DATAEE_ReadByte(FILENAME_YEAR);
    month    = DATAEE_ReadByte(FILENAME_MONTH);
    day      = DATAEE_ReadByte(FILENAME_DAY);
    suffix   = DATAEE_ReadByte(FILENAME_SUFFIX);        //前回ビルドしたファイル名
    data_saved_flag = DATAEE_ReadByte(DATA_OUTPUT);     //書き込みがされたか、ヘッダのみかの識別フラグ

    RTC_get_date(date);
    
    if ((year == date[6]) && (month == date[5]) && (day == date[4])){
        if (data_saved_flag == 1){ 
            //データがすでに書き出されている時
            suffix += 1;                //接尾辞を1つ進める
            if (suffix > END_SUFFIX){
                suffix = END_SUFFIX;    //行き過ぎないように規制する。
            }
        //}else{
            //データは出されていない時
            //suffixはそのまま
        }
    }else{
        //本日の初起動の時
        suffix = START_SUFFIX;
    }
    
    sprintf(filename_bullet , "B%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);    //6+3文字
    sprintf(filename_motion , "M%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);
    sprintf(filename_timelog, "T%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);
    data_saved_flag = 0; 

    //EEPROMへ書き込み
    DATAEE_WriteByte(FILENAME_YEAR,  date[6]);
    DATAEE_WriteByte(FILENAME_MONTH, date[5]);
    DATAEE_WriteByte(FILENAME_DAY,   date[4]);
    DATAEE_WriteByte(FILENAME_SUFFIX, suffix);
    DATAEE_WriteByte(DATA_OUTPUT, data_saved_flag);
    
}
  

//shot log
uint8_t shotheader_sd_write(void){
    //ヘッダーを書き出し　ショットログ
    uint8_t ans;
    ans = SDcard_open(filename_bullet);
    if (ans){
        return ans;
    }
    sprintf(tmp_string, "\n%s,%s\n", bullet_CSVdata[DATE], bullet_CSVdata[TIME]);
    SDcard_write();
    
    sprintf(tmp_string, "shot#,date      ,time    ,temp   ,humi   ,baropres,distance,gun type    ,v0device,BB    ,BBtype      ,extract,v0time ,v0       ,ve       ,imp time    ,target x,target y,C to C  ,tilt     ,elevation,mode      ,status          \n");
    SDcard_write();
    sprintf(tmp_string, "     ,          ,        ,degC   ,%%RH    ,hPa     ,m       ,            ,        ,g     ,            ,gf     ,usec   ,m/sec    ,m/sec    ,msec        ,mm      ,mm      ,mm      ,deg      ,deg      ,          ,                \n");
    SDcard_write();
    SDcard_close();
    return ans;
}


uint8_t shot_log_sd_write(void){
    //ショットデータを書き出し 
    uint8_t ans;
    
    if (ShotHeader_write_flag == 0){
        //ヘッダーがまだ書き込まれていないとき        
        if(shotheader_sd_write()){
            return 4;
        }else{
            ShotHeader_write_flag = 1;                  //ヘッダー書き込み済み
        }
    }
    
    ans = SDcard_open(filename_bullet);
    if (ans){
        return ans;
    }
    sprintf(tmp_string, 
        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
        bullet_CSVdata[SHOT],
        bullet_CSVdata[DATE],
        bullet_CSVdata[TIME],
        bullet_CSVdata[TEMP],
        bullet_CSVdata[HUMIDITY],
        bullet_CSVdata[BARO_PRESS],
        bullet_CSVdata[DISTANCE],
        bullet_CSVdata[AIRSOFT_TYPE],
        bullet_CSVdata[V0_DEVICE],
        bullet_CSVdata[BB_MASS],
        bullet_CSVdata[BB_TYPE],
        bullet_CSVdata[F_EXTRACT],
        bullet_CSVdata[V0_TIME],
        bullet_CSVdata[V0],
        bullet_CSVdata[VE],
        bullet_CSVdata[IMPACT_TIME],
        bullet_CSVdata[TARGET_X],
        bullet_CSVdata[TARGET_Y],            
        bullet_CSVdata[TARGET_CTC],            
        bullet_CSVdata[TILT],
        bullet_CSVdata[ELEVATION],
        bullet_CSVdata[DEVICE_MODE],            
        bullet_CSVdata[ERR_STATUS]
    );
    SDcard_write();
    SDcard_close();
    
    if (data_saved_flag == 0){
        data_saved_flag = 1;            //データをセーブした(当日中に起動した時にはファイルネームの接尾辞を1つ進めるための識別フラグ)
        DATAEE_WriteByte(DATA_OUTPUT, data_saved_flag);
    }
    
    return ans;
}


//motion log
uint8_t  motion_log_sd_write(void){
       //モーションデータを書き出し
#define         OK          0               //SDカード状態　-> sd_card
    
    uint8_t     i;
    uint8_t     data_point;                 //データ読み出しポインタ
    int32_t     output_time;                //センサ出力時刻経過 (usec)
    float       lag_time;                   //センサ1オンから最初の測定値までのズレ時間 (usec)
    float       sampling_time;              //サンプリング時刻 (usec)
    uint8_t     shot_index;                 //撃ってから一つ目のデータのインデックス番号
    float       time_at_shot;               //撃ってから一つ目のデータの時間オフセット割合
    uint8_t     ans;                        //SDカードの状態  0:OK 1:書込み処理なし
    
    ans = SDcard_open(filename_motion);
    if (ans == OK){
        //Motion headerをSDに書き込み
        sprintf(tmp_string, "\n%s,%s,%s\n", bullet_CSVdata[SHOT], bullet_CSVdata[DATE], bullet_CSVdata[TIME]);
        SDcard_write();
        sprintf(tmp_string, "target xy,%s,%s\n", bullet_CSVdata[TARGET_X], bullet_CSVdata[TARGET_Y]);     //着弾位置
        SDcard_write();                 
        sprintf(tmp_string, "TimeAccel,Gx       ,Gy       ,Gz       ,TimeGyro ,Roll     ,Pitch    ,Yaw      ,BlurRL   ,BlurUpDow,FIFO data \n");
        SDcard_write();
        sprintf(tmp_string, "msec     ,m/sec2   ,m/sec2   ,m/sec2   ,msec     ,deg/sec  ,deg/sec  ,deg/sec  ,MIL      ,MIL      , @read    \n");
        SDcard_write();
    }
    
    lag_time = motion_timer_offset * (float)TIME_CONV_SMT1 * 1000000;   //センサ1オンから最初の測定値までの時間 (usec)
    shot_index = (RINGBUF_MAX - AFTER_SHOT_COUNT);                      //撃った後一つ目のモーションデータのインデックス番号
    
    //Motion data 
    for (i = 0; i < RINGBUF_MAX; i ++){
        data_point = ringbuf_count + i;         //ringbuf_countの位置が一番古いデータ。ringbuf_count-1の位置が最新のデータ。
        if (data_point >= RINGBUF_MAX){
            data_point -= RINGBUF_MAX;
        }
        //time
        output_time = (i - RINGBUF_MAX + AFTER_SHOT_COUNT) * (int32_t)sample_period;    //usec
        
        //accel
        sampling_time = ((float)output_time + lag_time - (float)accel_DLPF_delay);      //usec
        sprintf(&motion_CSVdata[TIMEACCEL][0], "%9.3f", sampling_time / (float)1000);   //msec
        motion_print_accel(data_point, 1);      //csv=1:ログデータ代入あり

        //gyro
        sampling_time = ((float)output_time + lag_time - (float)gyro_DLPF_delay);       //usec
        //DLPFでの遅れのため測定時刻は出力時刻より遅れ分だけ前になる
        sprintf(&motion_CSVdata[TIMEGYRO][0], "%9.3f", sampling_time / (float)1000);    //msec
        
        //撃った後の最初のデータかどうか　= ショットタイミングマーク
        if (i == shot_index){
            if (sampling_time >= 0){
                time_at_shot = sampling_time / (float)sample_period;    //ラグタイム位置=割合(0～1 例　0.5の時中間)
            }else{
                //タイムがマイナスの時は次のデータがショット時のデータとなる
                shot_index++;
                time_at_shot = -1;  //ショット時ではないので負数に
            }
        }else {
            time_at_shot = -1;      //ショット時ではない時は負数
        }
        motion_print_gyro (data_point, time_at_shot, 1, (i == 0));    //full_disp, start

        //error
        motion_print_error(data_point);
        //check
        //最後のデータ読み出しのときにFIFOカウントがゼロでない時,画面上にエラーを表示
        if ((i == (RINGBUF_MAX - 1)) & (motion_log_remaindata[data_point] != 0)){
            motion_data_remain_check(data_point);                                   ////////////
        }
        //fifo_count_@zero
        //motion_data_at_zero();////////////////////////////いまいち

        
        if (ans == OK){
            sprintf(tmp_string,
                "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                motion_CSVdata[TIMEACCEL],
                motion_CSVdata[GX],
                motion_CSVdata[GY],
                motion_CSVdata[GZ],
                motion_CSVdata[TIMEGYRO],
                motion_CSVdata[ROLL],
                motion_CSVdata[PITCH],
                motion_CSVdata[YAW],
                motion_CSVdata[BLUR_RL],
                motion_CSVdata[BLUR_UD],
                motion_CSVdata[MPU_ERROR]
            );
            SDcard_write();  
        }
    }
    SDcard_close();
    
    return ans;
}

#ifdef  TIMING_LOG
void DEBUG_time_stamp_save(void){
    //タイムスタンプを書き出し
    uint8_t     i;
    float       time_usec;
    
    static char time_point_name [25][17] = {             //16文字長+エンドマーク0x00なので宣言は17で
        "                ",     //0
        "PT1 return      ",     //1
        "PT2 return      ",     //2
        "PT3 return      ",     //3
        "PT4 return      ",     //4
        "Tilt LED return ",     //5
        "Disp ave return ",     //6
        "Disp tilt return",     //7
        "Stage 8         ",     //8
        "Disp gyro return",     //9
        "MPU6050interrupt",     //10
        "MPU-time offset ",     //11
        "MotionGate close",     //12
        "MPU6050DataRead ",     //13
        "MPU6050DataComp ",     //14
        "Shot indi return",     //15
        "RTCC return     ",     //16
        "BME280 return   ",     //17
        "BatV return     ",     //18
        "Low Bat return  ",     //19
        "FIFO overflow   ",     //20
        "complete        ",     //21           
        "motion_return   ",     //22
        "MPUnodata return",     //23           
        "MPU o/f return  "      //24           
    };
    
    SDcard_open(filename_timelog);
    sprintf(tmp_string, "\n%s,%s,%s\n", bullet_CSVdata[SHOT], bullet_CSVdata[DATE], bullet_CSVdata[TIME]);
    SDcard_write();
    sprintf(tmp_string, "  #, point          , time stamp(usec), data remain \n");
    SDcard_write();
        
    for (i = 0; i < time_log; i++){
        time_usec = (float)time_stamp[i] * TIME_CONV_SMT1; 
        sprintf(tmp_string, "%3d,%s, %12.3f, rem%3d\n", time_point[i], &time_point_name[time_point[i]][0], time_usec, time_remain[i]);
        SDcard_write();
    }
    SDcard_close();
}

#endif //TIMING_LOG


//SDcard sub
uint8_t SDcard_open(char *filename){
    //FATFS   drive;
    //FIL     file;
    //UINT    actualLength;
    //header.hで宣言し、main.cで定義しないとスペース確保できない

    if( SD_SPI_IsMediaPresent() == false){
        //スイッチ設定していない。。。
        sprintf(SDerror_strings1, "NO SDcard!!       ");     //18文字
        //sprintf(SDerror_strings2, "                  ");
        SDcard_false();
        return 1;
    }
    if (f_mount(&drive,"0:",1) != FR_OK){
        sprintf(SDerror_strings1, "SDcard MOUNT ERROR");
        //sprintf(SDerror_strings2, "                  ");
        SDcard_false();
        SDcard_close();
        return 2;  
    }
    if (f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK){           //既存のファイルに追記する 
        //sprintf(SDerror_strings, "SDcard file open error  :%s", filename);
        sprintf(SDerror_strings1, "File open error:  ");
        //sprintf(SDerror_strings2, " %s", filename);
        SDcard_false();
        SDcard_close();
        return 3;
    }
    SDcard_false_erase();
    return 0;
}


void SDcard_write(void){
    //strings(文字列)をSDカードへ書き込み
    f_write(&file, tmp_string, strlen(tmp_string), &actualLength );     //strlen():文字数。string.hが必要
    //SDカードのファイルへ書き込み
}


void SDcard_close(void){
    f_close(&file);
    f_mount(0,"0:",0);
}


void SDcard_false(void){
    //エラー内容を表示
    LCD_Printf(COL_WARNING, ROW_WARNING1, SDerror_strings1, 1, PINK, 1);        //エラー文の表示
    //UART Tx
    //printf("%s\n", SDerror_strings);
    ShotHeader_write_flag = 0;                 //エラーの後はヘッダーを出すことにする。
}


void SDcard_false_erase(void){
    //エラー表示を消す
    sprintf(SDerror_strings1, "                  ");                       //18文字分
    LCD_Printf(COL_WARNING, ROW_WARNING1, SDerror_strings1, 1, BLACK, 1);       //エラー文の消去
}