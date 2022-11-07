/**
  SPI2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    spi2.c

  @Summary
    This is the generated driver implementation file for the SPI2 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides implementations for driver APIs for SPI2.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F57Q43
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above or later
        MPLAB             :  MPLAB X 5.45
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "spi2.h"
#include <xc.h>

typedef struct { 
    uint8_t con0; 
    uint8_t con1; 
    uint8_t con2; 
    uint8_t baud; 
    uint8_t operation;
} spi2_configuration_t;


//con0 == SPIxCON0, con1 == SPIxCON1, con2 == SPIxCON2, baud == SPIxBAUD, operation == Master/Slave
static const spi2_configuration_t spi2_configuration[] = {   
    { 0x2, 0x40, 0x0, 0xf, 0 },     //TOUCH
    //{ 0x2, 0x40, 0x0, 0x1, 0 },   //LCD DMA
    { 0x3, 0x40, 0x2, 0x1, 0 },     //LCD DMA ***BMODE=1
    { 0x2, 0x40, 0x0, 0x1, 0 },     //LCD
    { 0x2, 0x40, 0x0, 0x0, 0 }      //SPI2_DEFAULT
};

void SPI2_Initialize(void)
{
    //EN disabled; LSBF MSb first; MST bus slave; BMODE last byte; 
    SPI2CON0 = 0x02;
    //SMP Middle; CKE Active to idle; CKP Idle:Low, Active:High; FST disabled; SSP active high; SDIP active high; SDOP active high; 
    SPI2CON1 = 0x40;
    //SSET disabled; TXR not required for a transfer; RXR data is not stored in the FIFO; 
    SPI2CON2 = 0x00;
    //CLKSEL FOSC; 
    SPI2CLK = 0x00;
    //BAUD 0; 
    SPI2BAUD = 0x00;
    TRISDbits.TRISD6 = 0;
}

bool SPI2_Open(spi2_modes_t spi2UniqueConfiguration)
{
    if(!SPI1CON0bits.EN)
    {
        SPI2CON0 = spi2_configuration[spi2UniqueConfiguration].con0;
        SPI2CON1 = spi2_configuration[spi2UniqueConfiguration].con1;
        SPI2CON2 = spi2_configuration[spi2UniqueConfiguration].con2 | (_SPI1CON2_SPI1RXR_MASK | _SPI1CON2_SPI1TXR_MASK);
        SPI2CLK  = 0x00;
        SPI2BAUD = spi2_configuration[spi2UniqueConfiguration].baud;        
        TRISDbits.TRISD6 = spi2_configuration[spi2UniqueConfiguration].operation;
        SPI2CON0bits.EN = 1;
        return true;
    }
    return false;
}

void SPI2_Close(void)
{
    SPI2CON0bits.EN = 0;
}

uint8_t SPI2_ExchangeByte(uint8_t data)
{
    SPI2TCNTL = 1;
    SPI2TXB = data;
    while(!PIR5bits.SPI2RXIF);
    return SPI2RXB;
}

void SPI2_ExchangeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        SPI2TCNTL = 1;
        SPI2TXB = *data;
        while(!PIR5bits.SPI2RXIF);
        *data++ = SPI2RXB;
    }
}

// Half Duplex SPI Functions
void SPI2_WriteBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        SPI2_ExchangeByte(*data++);
    }
}

void SPI2_ReadBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data++ = SPI2_ExchangeByte(0);
    }
}

void SPI2_WriteByte(uint8_t byte)
{
    SPI2TXB = byte;
}

uint8_t SPI2_ReadByte(void)
{
    return SPI2RXB;
}