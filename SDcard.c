/*
 * SDcard.c
 * 
 * V8edition
 * 
 * 
 * 2020.08.30
 *     
 * 2020.09.12   �w�b�_�ɓ��t������ǉ�
 * 2021.01.11   �t�@�C���l�[������2������t��
 * 2021.05.12   �����O�t�@�C���l�[��
 * 2021.09.21   �t�@�C�����̌���a��ǉ�
 * 2021.09.25   �t�@�C�����̌���a�����Bmotion�������݂�motion.c����ړ�
 * 2022.05.05   V8 �d�q�^�[�Q�b�g���e�ʒu,�}�g���u���[�h�̃f�[�^�L�^��ǉ�
 * 2022.10.19   �z�b�v��]�����胍�O���ڂ�ǉ�
 * 2022.10.23   �V���[�g�t�@�C���l�[���B�f�[�^�������[�s���̂��߁�2.6k�قǋ󂢂��B
 * 2022.11.01   �t�@�C�����g���q.txt����.csv�ɕύX
 * 
 */

#include "header.h"
#include "SDcard.h"


//GLOBAL
char    bullet_CSVdata[BULLET_ITEM_NUM][LEN_BLL];   //SD�J�[�h�֏o���f�[�^�̎��[�p[���ڐ�][�f�[�^�̕�����]
char    motion_CSVdata[MOTION_ITEM_NUM][LEN_MOT];   //���[�V�����f�[�^
bool    ShotHeader_write_flag = 0;                  //SD�t�@�C���Ƀw�b�_�[���������񂾂��̃t���O



//LOCAL
char    filename_bullet [] = "BU000000A.CSV ";      //�t�@�C���l�[�� Bullet LOG (�啶����������ʖ���)
char    filename_motion [] = "MO000000A.CSV ";      //�t�@�C���l�[�� Motion LOG
char    filename_timelog[] = "TI000000A.CSV ";      //�t�@�C���l�[�� Time LOG
bool    data_saved_flag;                            //SD�t�@�C���Ƀf�[�^���������񂾂�:1�A�܂��Ȃɂ���������ł��Ȃ���:0(�t�@�C���l�[���̐ڔ������X�V���邩���Ȃ����̔��f)

//�G���[��
char    SDerror_strings1[20];               //�G���[���@18����
char    SDerror_strings2[20];               //�G���[���@18����


void generate_filename_date(void){
    //�t�@�C���l�[���ɓ��t��g�ݍ���
#define     START_SUFFIX    0x41    //"A"
#define     END_SUFFIX      0x5a    //"Z"

    uint8_t date[7];
    uint8_t year, month, day, suffix;
    
    //EEPROM����ǂݏo��
    year     = DATAEE_ReadByte(FILENAME_YEAR);
    month    = DATAEE_ReadByte(FILENAME_MONTH);
    day      = DATAEE_ReadByte(FILENAME_DAY);
    suffix   = DATAEE_ReadByte(FILENAME_SUFFIX);        //�O��r���h�����t�@�C����
    data_saved_flag = DATAEE_ReadByte(DATA_OUTPUT);     //�������݂����ꂽ���A�w�b�_�݂̂��̎��ʃt���O

    RTC_get_date(date);
    
    if ((year == date[6]) && (month == date[5]) && (day == date[4])){
        if (data_saved_flag == 1){ 
            //�f�[�^�����łɏ����o����Ă��鎞
            suffix += 1;                //�ڔ�����1�i�߂�
            if (suffix > END_SUFFIX){
                suffix = END_SUFFIX;    //�s���߂��Ȃ��悤�ɋK������B
            }
        //}else{
            //�f�[�^�͏o����Ă��Ȃ���
            //suffix�͂��̂܂�
        }
    }else{
        //�{���̏��N���̎�
        suffix = START_SUFFIX;
    }
    
    sprintf(filename_bullet , "B%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);    //6+3����
    sprintf(filename_motion , "M%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);
    sprintf(filename_timelog, "T%02x%02x%02x%c.CSV", date[6], date[5], date[4], suffix);
    data_saved_flag = 0; 

    //EEPROM�֏�������
    DATAEE_WriteByte(FILENAME_YEAR,  date[6]);
    DATAEE_WriteByte(FILENAME_MONTH, date[5]);
    DATAEE_WriteByte(FILENAME_DAY,   date[4]);
    DATAEE_WriteByte(FILENAME_SUFFIX, suffix);
    DATAEE_WriteByte(DATA_OUTPUT, data_saved_flag);
    
}
  

//shot log
uint8_t shotheader_sd_write(void){
    //�w�b�_�[�������o���@�V���b�g���O
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
    //�V���b�g�f�[�^�������o�� 
    uint8_t ans;
    
    if (ShotHeader_write_flag == 0){
        //�w�b�_�[���܂��������܂�Ă��Ȃ��Ƃ�        
        if(shotheader_sd_write()){
            return 4;
        }else{
            ShotHeader_write_flag = 1;                  //�w�b�_�[�������ݍς�
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
        data_saved_flag = 1;            //�f�[�^���Z�[�u����(�������ɋN���������ɂ̓t�@�C���l�[���̐ڔ�����1�i�߂邽�߂̎��ʃt���O)
        DATAEE_WriteByte(DATA_OUTPUT, data_saved_flag);
    }
    
    return ans;
}


//motion log
uint8_t  motion_log_sd_write(void){
       //���[�V�����f�[�^�������o��
#define         OK          0               //SD�J�[�h��ԁ@-> sd_card
    
    uint8_t     i;
    uint8_t     data_point;                 //�f�[�^�ǂݏo���|�C���^
    int32_t     output_time;                //�Z���T�o�͎����o�� (usec)
    float       lag_time;                   //�Z���T1�I������ŏ��̑���l�܂ł̃Y������ (usec)
    float       sampling_time;              //�T���v�����O���� (usec)
    uint8_t     shot_index;                 //�����Ă����ڂ̃f�[�^�̃C���f�b�N�X�ԍ�
    float       time_at_shot;               //�����Ă����ڂ̃f�[�^�̎��ԃI�t�Z�b�g����
    uint8_t     ans;                        //SD�J�[�h�̏��  0:OK 1:�����ݏ����Ȃ�
    
    ans = SDcard_open(filename_motion);
    if (ans == OK){
        //Motion header��SD�ɏ�������
        sprintf(tmp_string, "\n%s,%s,%s\n", bullet_CSVdata[SHOT], bullet_CSVdata[DATE], bullet_CSVdata[TIME]);
        SDcard_write();
        sprintf(tmp_string, "target xy,%s,%s\n", bullet_CSVdata[TARGET_X], bullet_CSVdata[TARGET_Y]);     //���e�ʒu
        SDcard_write();                 
        sprintf(tmp_string, "TimeAccel,Gx       ,Gy       ,Gz       ,TimeGyro ,Roll     ,Pitch    ,Yaw      ,BlurRL   ,BlurUpDow,FIFO data \n");
        SDcard_write();
        sprintf(tmp_string, "msec     ,m/sec2   ,m/sec2   ,m/sec2   ,msec     ,deg/sec  ,deg/sec  ,deg/sec  ,MIL      ,MIL      , @read    \n");
        SDcard_write();
    }
    
    lag_time = motion_timer_offset * (float)TIME_CONV_SMT1 * 1000000;   //�Z���T1�I������ŏ��̑���l�܂ł̎��� (usec)
    shot_index = (RINGBUF_MAX - AFTER_SHOT_COUNT);                      //���������ڂ̃��[�V�����f�[�^�̃C���f�b�N�X�ԍ�
    
    //Motion data 
    for (i = 0; i < RINGBUF_MAX; i ++){
        data_point = ringbuf_count + i;         //ringbuf_count�̈ʒu����ԌÂ��f�[�^�Bringbuf_count-1�̈ʒu���ŐV�̃f�[�^�B
        if (data_point >= RINGBUF_MAX){
            data_point -= RINGBUF_MAX;
        }
        //time
        output_time = (i - RINGBUF_MAX + AFTER_SHOT_COUNT) * (int32_t)sample_period;    //usec
        
        //accel
        sampling_time = ((float)output_time + lag_time - (float)accel_DLPF_delay);      //usec
        sprintf(&motion_CSVdata[TIMEACCEL][0], "%9.3f", sampling_time / (float)1000);   //msec
        motion_print_accel(data_point, 1);      //csv=1:���O�f�[�^�������

        //gyro
        sampling_time = ((float)output_time + lag_time - (float)gyro_DLPF_delay);       //usec
        //DLPF�ł̒x��̂��ߑ��莞���͏o�͎������x�ꕪ�����O�ɂȂ�
        sprintf(&motion_CSVdata[TIMEGYRO][0], "%9.3f", sampling_time / (float)1000);    //msec
        
        //��������̍ŏ��̃f�[�^���ǂ����@= �V���b�g�^�C�~���O�}�[�N
        if (i == shot_index){
            if (sampling_time >= 0){
                time_at_shot = sampling_time / (float)sample_period;    //���O�^�C���ʒu=����(0�`1 ��@0.5�̎�����)
            }else{
                //�^�C�����}�C�i�X�̎��͎��̃f�[�^���V���b�g���̃f�[�^�ƂȂ�
                shot_index++;
                time_at_shot = -1;  //�V���b�g���ł͂Ȃ��̂ŕ�����
            }
        }else {
            time_at_shot = -1;      //�V���b�g���ł͂Ȃ����͕���
        }
        motion_print_gyro (data_point, time_at_shot, 1, (i == 0));    //full_disp, start

        //error
        motion_print_error(data_point);
        //check
        //�Ō�̃f�[�^�ǂݏo���̂Ƃ���FIFO�J�E���g���[���łȂ���,��ʏ�ɃG���[��\��
        if ((i == (RINGBUF_MAX - 1)) & (motion_log_remaindata[data_point] != 0)){
            motion_data_remain_check(data_point);                                   ////////////
        }
        //fifo_count_@zero
        //motion_data_at_zero();////////////////////////////���܂���

        
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
    //�^�C���X�^���v�������o��
    uint8_t     i;
    float       time_usec;
    
    static char time_point_name [25][17] = {             //16������+�G���h�}�[�N0x00�Ȃ̂Ő錾��17��
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
    //header.h�Ő錾���Amain.c�Œ�`���Ȃ��ƃX�y�[�X�m�ۂł��Ȃ�

    if( SD_SPI_IsMediaPresent() == false){
        //�X�C�b�`�ݒ肵�Ă��Ȃ��B�B�B
        sprintf(SDerror_strings1, "NO SDcard!!       ");     //18����
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
    if (f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK){           //�����̃t�@�C���ɒǋL���� 
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
    //strings(������)��SD�J�[�h�֏�������
    f_write(&file, tmp_string, strlen(tmp_string), &actualLength );     //strlen():�������Bstring.h���K�v
    //SD�J�[�h�̃t�@�C���֏�������
}


void SDcard_close(void){
    f_close(&file);
    f_mount(0,"0:",0);
}


void SDcard_false(void){
    //�G���[���e��\��
    LCD_Printf(COL_WARNING, ROW_WARNING1, SDerror_strings1, 1, PINK, 1);        //�G���[���̕\��
    //UART Tx
    //printf("%s\n", SDerror_strings);
    ShotHeader_write_flag = 0;                 //�G���[�̌�̓w�b�_�[���o�����Ƃɂ���B
}


void SDcard_false_erase(void){
    //�G���[�\��������
    sprintf(SDerror_strings1, "                  ");                       //18������
    LCD_Printf(COL_WARNING, ROW_WARNING1, SDerror_strings1, 1, BLACK, 1);       //�G���[���̏���
}