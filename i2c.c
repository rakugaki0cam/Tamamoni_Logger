/*
 *  I2C
 *      7ビットアドレス　マスターモード
 * 
 *  2021.09.24
 * 
 *  MCCのI2Cは割込ルーチンから使えないため
 * 
 *  シンプルルーチン
 *  PIC18F26K83のMCCよりコピー
 * 
 * 
 * 2021.10.18   ところが速度が出ない。250kHz (OSCFRQ=0;1MHz, FME=1;x1/4)
 * 
 */
#include "header.h"
#include "i2c.h"

static i2c1_error lastError = I2C1_GOOD;


void i2c_master_initialize(void){
    if (!I2C1CON0bits.EN || lastError != I2C1_GOOD){
        
		lastError = I2C1_GOOD;        
        // I2C Clock = HFINTOSC
        I2C1CLK = 0x02;//////////////1MHz(OSCILLATOR_Initialize()@mcc.c)
        
        //MFINTOSC
        //I2C1CLK = 0x03;///////////////
        
        //bit7:NACKs to end a Read (ACKCNT = 1)
        //bit6:ACK for every valid byte (ACKDT = 0)
        I2C1CON1 = 0x80;
        
        //bit7:Auto-count disabled (ACNT = 0)
        //bit6:General Call disabled (GCEN = 0)
        //bit5:Fast mode enabled
        // (FME = 1; SCL = I2CCLK/4)
        //bit4:ADB1 address buffer used (ADB = 0)
        //bit3-2:SDA Hold time of 300 ns (SDAHT = 0)
        //bit1-0:Bus free time of 8 I2C Clock pulses
        // (BFRET = 0)
        I2C1CON2 = 0x20;
        //I2C1CON2 = 0x00;  //FME=0 1/5
        
        //bit7:I2C Enable
        //bit2-0:7bit Master Mode (MODE = 4)
        I2C1CON0 = 0x84;
        I2C1PIR = 0;        //Clear all the error flags
        I2C1ERR = 0;
    }
}

static inline void wait4Start(void)
{
    uint8_t waitCount = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--waitCount)
        {
            if(!I2C1CON0bits.S)
            {
                return;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
}

static inline void wait4BusFree(void)
{
    uint8_t waitCount = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--waitCount)
        {
            if(I2C1STAT0bits.BFRE)
            {
                return;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
}

static inline void sendByte(uint8_t data)
{
    uint8_t delayCounter = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--delayCounter)
        {
            if(I2C1STAT1bits.TXBE)
            {
                I2C1TXB = data;
                return;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
}

static inline uint8_t receiveByte(void)
{
    uint8_t delayCounter = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--delayCounter)
        {
            if(I2C1STAT1bits.RXBF)
            {
                return I2C1RXB;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
	return 0;
}

static inline void wait4Stop(void)
{
    uint8_t waitCount = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--waitCount)
        {
            if(I2C1PIRbits.PCIF)
            {
                return;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
}

static inline void wait4MDRSetcount(uint8_t count)
{
    uint8_t waitCount = 255;
    if(lastError == I2C1_GOOD)
    {
        while(--waitCount)
        {
            if(I2C1CON0bits.MDR)
            {
                I2C1CNT = count;
                return;
            }
            else
            {
                __delay_us(1);
            }
        }
        lastError = I2C1_FAIL_TIMEOUT;
    }
}


void i2c_master_write1byte(uint8_t address, uint8_t reg, uint8_t data){
    i2c_master_initialize();
    I2C1ADB1 = (uint8_t)(address<<1);
    wait4BusFree();
    I2C1CNT = 2;
    I2C1CON0bits.S = 1;
    wait4Start();
    sendByte(reg);
    sendByte(data);
    wait4Stop();
}


uint8_t i2c_master_read1byte(uint8_t address, uint8_t reg){
    uint8_t result;

    i2c_master_initialize();
    I2C1ADB1 = (uint8_t)(address<<1);
    wait4BusFree();
    I2C1CNT = 1;
    I2C1CON0bits.RSEN = 1;
    I2C1CON0bits.S = 1; //Start
    wait4Start();
    sendByte(reg);
    wait4MDRSetcount(1);
    address = (uint8_t)(address<<1);
    I2C1ADB1 = (uint8_t)(address| 0x01); //Change the R/W bit for read
    I2C1CON0bits.S = 1; //Start
    I2C1CON0bits.RSEN = 0;
    result = receiveByte();
    wait4Stop();
    return result;
}


void i2c_master_readblock(uint8_t address, uint8_t reg, unsigned char *data, uint8_t len){
    i2c_master_initialize();
    I2C1ADB1 = (uint8_t)(address<<1);
    wait4BusFree();
    I2C1CNT = 1;
    I2C1CON0bits.RSEN = 1;
    I2C1CON0bits.S = 1; //Start
    wait4Start();
    sendByte(reg);
    wait4MDRSetcount(len);
    address = (uint8_t)(address<<1);
    I2C1ADB1 = (uint8_t)(address| 0x01); //Change the R/W bit for read
    I2C1CON0bits.S = 1; //Start
    I2C1CON0bits.RSEN = 0;
    wait4Start();
    while(len--)
    {
        *data++ = receiveByte();
    }
    wait4Stop();
}


/**
 End of File
*/
