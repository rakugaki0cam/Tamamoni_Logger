/* 
 * File:   RTClock_8900.c
 * Author: IsobeR
 * Comments: PIC18F26K83 MCC
 *           PIC18F57Q43 MCC
 * 
 * ���A���^�C���N���b�N
 * I2C
 * �@RX-8900
 * 
 * Revision history: 
 * 2020.08.09   I2C�̋L�q���ύX
 * 2020.08.14   ���v���킹��MPLABX�C�����C���f�o�b�O���g�p
 * 2021.02.22   �d���f���o�r�b�g�v�Z�ԈႢ�C��
 * 2021.09.20   RTC_timing TMR5->SMT1 uint16_t->uint32_t
 * 2021.09.25   RTC_disp()�X�V�ӏ������\���̃��[�h��ǉ�
 * 2021.12.19   RTC_disp()���̍X�V�\������Ȃ��o�O�t�B�N�X
 * 2022.10.21   �����Z�b�g��32MZ���ڐA
 * 2022.10.23   �����Z�b�g�����j���[�ɂ��g����悤�ɐ���
 * 
 */

#include "header.h"
#include "RTClock_8900.h"

#define     RTC_TIMING_DEBUG_no
#define     ROW_RTCLOCK     310
#define     COL_RTCLOCK     110
//BCD-decimal
#define decimaltobcd(x)     (((x / 10) << 4) + ((x - ((x / 10) * 10))))
#define bcdtodecimal(x)     ((x & 0xF0) >> 4) * 10 + (x & 0x0F)


//global
uint32_t    RTC_timing;         //�Z���T�[1�I�����璼��̑���l�X�V�܂ł̎���
bool        RTC_int_flag = 0;    //���荞�݃t���O�@�����X�V�@1�b��                
uint16_t    sleep_count;        //�X���[�v�p�^�C�}�[�J�E���g
int8_t      low_battery_count;  //��d�����o��̃J�E���^

//�j���p������ϐ�
char week[4];


uint8_t RTC_initialize(void){
    uint8_t     dd[7];
    
    __delay_ms(100);
    printf("RTCC INIT...");
    
    //////���M����M�m�F�����ăG���[�����o
    
    if ((I2C1_Read1ByteRegister(RX8900_ADDRESS, 0x1e) & 0b00000010) != 0){
    //if ((i2c_master_read1byte(RX8900_ADDRESS, 0x1e) & 0b00000010) != 0){
    
        //bit1 VLF=1�̎��͓d���f�����������̂Ǝv����̂Ł@�S�Ă̏����ݒ�����{
        printf("RESET\n");
        //I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x08);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-1Hz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1e, 0x00);    //reset all flag
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1f, 0x61);    //reset bit7-6:CSEL-2sec, bit5:UIE-UpdateINT ON,�@bit0:RESET
        
        I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
  
        __delay_ms(1000);
        RTC8900_time_set();          //���t�����Z�b�g��
        return 1;
        
    }else {
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        //i2c_master_write1byte(RX8900_ADDRESS, 0x1d, 0x00);    //set bit6:WADA-WEEK ALARM, bit5:USEL-UpdateINT 1sec, bit3-2:FSEL-32.768kHz to FOUT
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1e, 0x00);    //reset all flag
        I2C1_Write1ByteRegister(RX8900_ADDRESS, 0x1f, 0x60);    //reset bit7-6:CSEL-2sec, bit5:UIE-UpdateINT ON
    }
    IOCCF2_SetInterruptHandler(RTC_interrupt);
    printf("OK!\n");
    return 0;
}


void RTC_interrupt(void){
    //�����X�V���荞��
#if RTC_TIMING_DEBUG
    RTC_timing = SMT1_GetTimerValue();  //�Z���T�[1�I�����璼��̑���l�X�V�܂ł̎���=debug
#endif
    sleep_count ++;                     //�X���[�v�܂ł̃^�C�}�[�J�E���g
    low_battery_count--;                //��d�����o���Ă���̃J�E���g�_�E���^�C�}�[
    RTC_int_flag = 1;
}


void RTC8900_time_get(uint8_t *time_data){
    //�^�C���擾
    //�f�[�^��BCD2���@�@�@��. 21�b��������0x21
    //time  0:sec, 1:min, 2:hour, 3:week (sun=1, mon=2, tue=4, wed=8, thu=16, fri=32, sat=64)
    //date  4:day, 5:month, 6:year(-2000)
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, time_data, 7);
}


void RTC_disp(bool full){
    //���t�A�����f�[�^��ǂݍ��݁A�\��
    uint8_t         dd[7];
    static uint8_t  dd_before[7] = {0,0,0,0,0,0,0};
    uint8_t         i;
    
    
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
    if (full == 1){
        //�S���\�����[�h
        sprintf(tmp_string, "20%02x/%02x/%02x  %02x:%02x:%02x", dd[6], dd[5], dd[4], dd[2], dd[1], dd[0]);    //time_str��global
        LCD_Printf(COL_RTCLOCK, ROW_RTCLOCK, tmp_string, 1, WHITE, 1); 
    }else{
        //�ω������Ƃ��낾���������[�h
        //time
        for (i = 0; i <= 2; i++){
            if (dd_before[i] != dd[i]){
                sprintf(tmp_string, "%02x", dd[i]);
                LCD_Printf((COL_RTCLOCK + 108 - 18 * i), ROW_RTCLOCK, tmp_string, 1, WHITE, 1);
            }
        }
        //date
        for (i = 4; i <= 6; i++){
            if (dd_before[i] != dd[i]){
                sprintf(tmp_string, "%02x", dd[i]);
                LCD_Printf((COL_RTCLOCK + 120 - 18 * i), ROW_RTCLOCK, tmp_string, 1, WHITE, 1);
            }
        }
        for (i = 0; i < 7; i++){
            dd_before[i] = dd[i];
        }
    }
#if DEBUGGER_1
    //UART Tx
    printf("%s\n", tmp_string);
#endif
    //csv
    sprintf(&bullet_CSVdata[DATE][0], "20%02x/%02x/%02x", dd[6], dd[5], dd[4]);
    sprintf(&bullet_CSVdata[TIME][0], "%02x:%02x:%02x", dd[2], dd[1], dd[0]);
#if RTC_TIMING_DEBUG
    //�������u�Ԃ��玟�ɍX�V�������܂ł̎���(���蒆�Ɋ��荞�݂����������ǂ����̊m�F����)=debug
    sprintf(&bullet_CSVdata[RTCC_LAG][0], "%10.3fmsec", (float)RTC_timing * TIME_CONV1 / 1000);
#endif
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(16);       //RTCC return
#endif
}


void RTC_get_date(uint8_t * dd){
    //�t�@�C���l�[���p�N�����̎擾
    //�z��ŕԂ�
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10, dd, 7);
}


/*
void RTC_set(void){
    //���v���킹
    uint8_t dd[7], i;
    I2C1_ReadDataBlock(RX8900_ADDRESS, 0x10,dd,7);
    sprintf(tmp_string, "20%02x/%02x/%02x  %02x:%02x:%02x", dd[6], dd[5], dd[4], dd[2], dd[1], dd[0]);
    LCD_Printf(50, 100, tmp_string, 1, WHITE, 1); 
    sprintf(tmp_string, "set time to use MPLABX");
    LCD_Printf(50, 150, tmp_string, 1, RED, 1); 

    __delay_ms(1000);
    dd[0] = 0x00;//sec BCD
    dd[1] = 0x25;//min
    dd[2] = 0x22;//hour
    dd[3] = 0x06;//week bit6:sat 5:fri 4:tue 3:wed 2:tue 1:mon 0:sun
    dd[4] = 0x23;//day
    dd[5] = 0x04;//month
    dd[6] = 0x22;//year
    // �� �����Ƀu���[�N�|�C���g��ݒ肵�Ď��v�����Ȃ��獇�킹��
    for (i = 0; i < 7; i++){                                            //���v�����킹�鎞�͂����Ƀu���[�N�|�C���g��ݒ肷��B
        I2C1_Write1ByteRegister(RX8900_ADDRESS, (0x10 + i) , dd[i]);
    }
}
*/


void RTC8900_time_set(void) {
    //���v���킹�@�^�b�`�{�^��
    
    //��ʕ\��
#define     B2X     12      //2�{�p�����@12x16pixel
#define     B2Y     16
    
#define     COL_BUTTON_SEL  0
#define     ROW_BUTTON_SEL  (8*33)
    
    //�{�^������
    typedef enum {
        IDLE,
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        SECOND,
        PLUS,
        MINUS,
        NEXT,
        BACK,
        SET,
        CANCEL,
        NUM_BUTTON_TIME, //�錾��  =�@���b�Z�[�W�z��Y����     
    } button_time_t;
    
    
#define     BUTTON_TIME_LEN    13
    //���v���킹�̃{�^���ʒu�̐ݒ�
    const uint16_t button_time[BUTTON_TIME_LEN][4] = {
      //{ x0,  y0, ����, ����}, 
        {   240,   320,     0,   0},    //dummy
        { 1*B2X, 2*B2Y, 4*B2X, B2Y},    
        { 8*B2X, 2*B2Y, 2*B2X, B2Y},    
        {13*B2X, 2*B2Y, 2*B2X, B2Y},    
        { 3*B2X, 4*B2Y, 2*B2X, B2Y},    
        { 8*B2X, 4*B2Y, 2*B2X, B2Y},    
        {13*B2X, 4*B2Y, 2*B2X, B2Y},    
        { 3*B2X, 7*B2Y, 3*B2X, B2Y},    
        { 7*B2X, 7*B2Y, 3*B2X, B2Y},    
        {11*B2X, 7*B2Y, 3*B2X, B2Y},    
        {15*B2X, 7*B2Y, 3*B2X, B2Y},    
        { 3*B2X, 9*B2Y, 5*B2X, B2Y},    
        {11*B2X, 9*B2Y, 6*B2X, B2Y}     
    };

    
    uint8_t tx_data[8];
    uint8_t time_data[7];
    //tmp_string�̓O���[�o���Œ�`����Ă���

    uint8_t i;
    button_time_t   bnum;
    uint8_t bsel = MINUTE;  //�I�𒆂̃{�^��
    
    int8_t  year, month, day, wnum;
    int8_t  hour, minute, second;
    //�����̓��� end day of mounth
    uint8_t edom[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};     //�Y���ƌ�����v�����Ă�B0����0
    

    RTC8900_time_get(time_data);
    year =  bcdtodecimal(time_data[6]);
    month = bcdtodecimal(time_data[5]);
    day =   bcdtodecimal(time_data[4]);
    wnum =  time_data[3];
    hour =   bcdtodecimal(time_data[2]);
    minute = bcdtodecimal(time_data[1]);
    //second = bcdtodecimal(time_data[0]);
    second = 0;
    wnum = youbi(year, month, day);

    //�������
    menu_clear_screen();
    sprintf(tmp_string, "*** set a clock ****");
    LCD_Printf(0*B2X, 0*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "20%02d / %02d / %02d %s", year, month, day, week);
    LCD_Printf( 1*B2X, 2*B2Y, tmp_string, 2, WHITE, 1);
#define     COL_WEEK    16*B2X
#define     ROW_WEEK     2*B2Y
    sprintf(tmp_string, "%02d : %02d : %02d", hour, minute, second);
    LCD_Printf( 3*B2X, 4*B2Y, tmp_string, 2, WHITE, 1);
    sprintf(tmp_string, "+   -   %c   %c", 0x7e,0x7f);
    //�� ��  �� ��  0x7e~0x81�@���p���
    LCD_Printf(4*B2X, 7*B2Y, tmp_string, 2, YELLOW, 1);
    sprintf(tmp_string, "SET    CANCEL");
    LCD_Printf(4*B2X, 9*B2Y, tmp_string, 2, YELLOW, 1);
    
    button_init(button_time, NUM_BUTTON_TIME);
    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);   //�ŏ��̕\��

    
    //main loop
    while(1){
        
        if (sw1_int_flag == 1){
            power_switch();        //����񉟂��Ń��^�[���A�������Ńp���[�I�t
            sw1_int_flag = 0;
            return;             //set_time�𔲂���  
        }
        
        if (TOUCH_INT_PORT == 0){
            //�^�b�`����
            bnum = button_number(button_time, NUM_BUTTON_TIME);
            
            switch(bnum){
                case IDLE:
                    //idle
                    break;
                case YEAR:
                case MONTH:
                case DAY:
                case HOUR:
                case MINUTE:
                case SECOND:
                    //�O��̈ʒu������(�g�O���C�\��)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel = bnum;
                    //�{�^���I��\��(���F)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case PLUS:
                    switch(bsel){
                        case YEAR:
                            year++;
                            if (year > 50){
                                year = 20;
                            }
                            if ((year % 4) == 0){
                                //���邤�N��2���̓����ύX
                                edom[2] = 29;
                            }else{
                                edom[2] = 28;
                            }
                            sprintf(tmp_string, "20%02d", year);
                            break;
                        case MONTH:
                            month++;
                            if (month > 12){
                                month = 1;
                            }
                            sprintf(tmp_string, "%02d", month);
                            break;
                        case DAY:
                            day++;
                            if (day > edom[month]){
                                day = 1;
                            }
                            sprintf(tmp_string, "%02d", day);
                            break;
                        case HOUR:
                            hour++;
                            if (hour > 23){
                                hour = 0;
                            }
                            sprintf(tmp_string, "%02d", hour);
                            break;
                        case MINUTE:
                            minute++;
                            if (minute > 59){
                                minute = 0;
                            }
                            sprintf(tmp_string, "%02d", minute);
                            break;
                        case SECOND:
                            if (second == 0){
                                second = 30;
                            }else{
                                second = 0;
                            }
                            sprintf(tmp_string, "%02d", second);
                            __delay_ms(500);
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                        }
                    //�{�^�����e�L�X�g
                    LCD_Printf(button_time[bsel][0], button_time[bsel][1], tmp_string, 2, WHITE, 1);
                    //�j���̍Čv�Z
                    wnum = youbi(year, month, day);
                    sprintf(tmp_string, "%s", week);
                    LCD_Printf(COL_WEEK, ROW_WEEK, tmp_string, 2, WHITE, 1);   
                    break;
                case MINUS:
                    switch(bsel){
                        case YEAR:
                            year--;
                            if (year < 20){
                                year = 50;
                            }
                            if ((year % 4) == 0){
                                //���邤�N��2���̓����ύX
                                edom[2] = 29;
                            }else{
                                edom[2] = 28;
                            }
                            sprintf(tmp_string, "20%02d", year);
                            break;
                        case MONTH:
                            month--;
                            if (month < 1){
                                month = 12;
                            }
                            sprintf(tmp_string, "%02d", month);
                            break;
                        case DAY:
                            day--;
                            if (day < 1){
                                day = edom[month];
                            }
                            sprintf(tmp_string, "%02d", day);
                            break;
                        case HOUR:
                            hour--;
                            if (hour < 0){
                                hour = 23;
                            }
                            sprintf(tmp_string, "%02d", hour);
                            break;
                        case MINUTE:
                            minute--;
                            if (minute < 0 ){
                                minute = 59;
                            }
                            sprintf(tmp_string, "%02d", minute);
                            break;
                        case SECOND:
                            if (second == 0){
                                second = 30;
                            }else{
                                second = 0;
                            }
                            sprintf(tmp_string, "%02d", second);
                            __delay_ms(500);
                            break;
                        default:
                            sprintf(tmp_string, "");
                            break;
                    }
                    //�{�^�����e�L�X�g�ĕ`��
                    LCD_Printf(button_time[bsel][0], button_time[bsel][1], tmp_string, 2, WHITE, 1);
                    //�j���̍Čv�Z
                    wnum = youbi(year, month, day);
                    sprintf(tmp_string, "%s", week);
                    LCD_Printf(COL_WEEK, ROW_WEEK, tmp_string, 2, WHITE, 1);   
                    break;
                case NEXT:
                    //�O��̈ʒu������(�g�O���C�\��)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel++;
                    if(bsel > MINUTE){
                        bsel = MINUTE;
                    }
                    //�{�^���I��\��(���F)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case BACK:
                    //�O��̈ʒu������(�g�O���C�\��)
                    button_select_disp(bsel, 0, button_time, NUM_BUTTON_TIME);
                    bsel--;
                    if(bsel < YEAR){
                        bsel = YEAR;
                    }
                    //�{�^���I��\��(���F)
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    break;
                case SET:
                    //�^�C�����Z�b�g
                    tx_data[0] = decimaltobcd(second + 1);  //sec BCD �^�C�~���O���킹�̂���+1
                    tx_data[1] = decimaltobcd(minute);      //min
                    tx_data[2] = decimaltobcd(hour);        //hour
                    tx_data[3] = 1 << wnum;                 //week bit6(0x40):sat bit5(0x20):fri bit4(0x10):tue bit3(0x8):wed bit2(0x4):tue bit1(0x2):mon bit0(0x1):sun
                    tx_data[4] = decimaltobcd(day);         //day
                    tx_data[5] = decimaltobcd(month);       //month
                    tx_data[6] = decimaltobcd(year);        //year
                    __delay_ms(500);                        //�^�C�~���O���킹
                    for (i = 0; i < 7; i++){
                        I2C1_Write1ByteRegister(RX8900_ADDRESS, (0x10 + i) , tx_data[i]);
                    }

                    sprintf(tmp_string, "DATE&TIME SET!");
                    LCD_Printf(2*B2X, 11*B2Y, tmp_string, 2, PINK, 1);
                    __delay_ms(1500);
                    menu_clear_screen();
                    return;
                case CANCEL:
                    button_select_disp(bsel, 1, button_time, NUM_BUTTON_TIME);
                    __delay_ms(300);
                    menu_clear_screen();
                    return;
                default:
                    break;
            
            }   //switch
            
            //button select  ///debug
            sprintf(tmp_string, "bu.sel = %2d", bsel);
            LCD_Printf(COL_BUTTON_SEL, ROW_BUTTON_SEL, tmp_string, 1, RED, 1);
            
        }   //if
            
    }  //while
    
    
}


uint8_t youbi(uint8_t y, uint8_t m, uint8_t d){
    //���t����j�������߂�@�c�F���[(Zeller)�̌���
    uint8_t w;
    
    if (m < 3) {
    //�[�N�����邽�߁A1�N�̋N�_��3���Ƃ��Čv�Z
         y--;
         m += 12;
     }
     w = (y + y / 4 - y / 100 + y / 400 + (13 * m + 8) / 5 + d) % 7;
     
     switch(w){
        case 0:
            sprintf(week, "sun");
            break;
        case 1:
            sprintf(week, "mon");
            break;
        case 2:
            sprintf(week, "tue");
            break;
        case 3:
            sprintf(week, "wed");
            break;
        case 4:
            sprintf(week, "thu");
            break;
        case 5:
            sprintf(week, "fri");
            break;
        case 6:
            sprintf(week, "sat");
    }
     
    return w;

}
