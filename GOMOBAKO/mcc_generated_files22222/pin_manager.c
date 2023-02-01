/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F57Q43
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB             :  MPLAB X 6.00

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
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

#include "pin_manager.h"




void (*IOCAF4_InterruptHandler)(void);
void (*IOCAF5_InterruptHandler)(void);
void (*IOCCF2_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATE = 0x00;
    LATD = 0x11;
    LATA = 0x00;
    LATF = 0x10;
    LATB = 0x00;
    LATC = 0xC0;

    /**
    TRISx registers
    */
    TRISE = 0x06;
    TRISF = 0x01;
    TRISA = 0xFF;
    TRISB = 0xF3;
    TRISC = 0x26;
    TRISD = 0x0A;

    /**
    ANSELx registers
    */
    ANSELD = 0x00;
    ANSELC = 0x02;
    ANSELB = 0xD0;
    ANSELE = 0x02;
    ANSELF = 0x00;
    ANSELA = 0xC0;

    /**
    WPUx registers
    */
    WPUD = 0x00;
    WPUF = 0x01;
    WPUE = 0x04;
    WPUB = 0x30;
    WPUA = 0x30;
    WPUC = 0x3C;

    /**
    RxyI2C registers
    */
    RB1I2C = 0x00;
    RB2I2C = 0x00;
    RC3I2C = 0x61;
    RC4I2C = 0x61;

    /**
    ODx registers
    */
    ODCONE = 0x00;
    ODCONF = 0x00;
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x18;
    ODCOND = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0xFF;
    SLRCONB = 0x30;
    SLRCONC = 0xBF;
    SLRCOND = 0x8F;
    SLRCONE = 0x07;
    SLRCONF = 0xEF;

    /**
    INLVLx registers
    */
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;
    INLVLD = 0xFF;
    INLVLE = 0x0F;
    INLVLF = 0xFF;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF4 = 0;
    //interrupt on change for group IOCAF - flag
    IOCAFbits.IOCAF5 = 0;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN4 = 1;
    //interrupt on change for group IOCAN - negative
    IOCANbits.IOCAN5 = 1;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP4 = 0;
    //interrupt on change for group IOCAP - positive
    IOCAPbits.IOCAP5 = 0;
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF2 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN2 = 1;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP2 = 0;



    // register default IOC callback functions at runtime; use these methods to register a custom function
    IOCAF4_SetInterruptHandler(IOCAF4_DefaultInterruptHandler);
    IOCAF5_SetInterruptHandler(IOCAF5_DefaultInterruptHandler);
    IOCCF2_SetInterruptHandler(IOCCF2_DefaultInterruptHandler);
   
    // Enable IOCI interrupt 
    PIE0bits.IOCIE = 1; 
    
	
    U2RXPPS = 0x19;   //RD1->UART2:RX2;    
    SPI2SDIPPS = 0x08;   //RB0->SPI2:SDI2;    
    RC3PPS = 0x37;   //RC3->I2C1:SCL1;    
    RC4PPS = 0x38;   //RC4->I2C1:SDA1;    
    CLCIN4PPS = 0x02;   //RA2->CLC4:CLCIN4;    
    CLCIN5PPS = 0x03;   //RA3->CLC4:CLCIN5;    
    RD6PPS = 0x34;   //RD6->SPI2:SCK2;    
    I2C1SCLPPS = 0x13;   //RC3->I2C1:SCL1;    
    SPI2SCKPPS = 0x1E;   //RD6->SPI2:SCK2;    
    RD7PPS = 0x17;   //RD7->CCP3:CCP3;    
    RD2PPS = 0x24;   //RD2->UART2:TXDE2;    
    SPI1SCKPPS = 0x0A;   //RB2->SPI1:SCK1;    
    I2C1SDAPPS = 0x14;   //RC4->I2C1:SDA1;    
    RD5PPS = 0x35;   //RD5->SPI2:SDO2;    
    RF3PPS = 0x18;   //RF3->PWM1_16BIT:PWM11;    
    CLCIN0PPS = 0x00;   //RA0->CLC3:CLCIN0;    
    RB2PPS = 0x31;   //RB2->SPI1:SCK1;    
    RB3PPS = 0x32;   //RB3->SPI1:SDO1;    
    RD0PPS = 0x23;   //RD0->UART2:TX2;    
    CLCIN2PPS = 0x1B;   //RD3->CLC4:CLCIN2;    
    CLCIN1PPS = 0x01;   //RA1->CLC6:CLCIN1;    
    SPI1SDIPPS = 0x09;   //RB1->SPI1:SDI1;    
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCAF4
    if(IOCAFbits.IOCAF4 == 1)
    {
        IOCAF4_ISR();  
    }	
	// interrupt on change for pin IOCAF5
    if(IOCAFbits.IOCAF5 == 1)
    {
        IOCAF5_ISR();  
    }	
	// interrupt on change for pin IOCCF2
    if(IOCCFbits.IOCCF2 == 1)
    {
        IOCCF2_ISR();  
    }	
}

/**
   IOCAF4 Interrupt Service Routine
*/
void IOCAF4_ISR(void) {

    // Add custom IOCAF4 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCAF4_InterruptHandler)
    {
        IOCAF4_InterruptHandler();
    }
    IOCAFbits.IOCAF4 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF4 at application runtime
*/
void IOCAF4_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCAF4_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF4
*/
void IOCAF4_DefaultInterruptHandler(void){
    // add your IOCAF4 interrupt custom code
    // or set custom function using IOCAF4_SetInterruptHandler()
}

/**
   IOCAF5 Interrupt Service Routine
*/
void IOCAF5_ISR(void) {

    // Add custom IOCAF5 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCAF5_InterruptHandler)
    {
        IOCAF5_InterruptHandler();
    }
    IOCAFbits.IOCAF5 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF5 at application runtime
*/
void IOCAF5_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCAF5_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF5
*/
void IOCAF5_DefaultInterruptHandler(void){
    // add your IOCAF5 interrupt custom code
    // or set custom function using IOCAF5_SetInterruptHandler()
}

/**
   IOCCF2 Interrupt Service Routine
*/
void IOCCF2_ISR(void) {

    // Add custom IOCCF2 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF2_InterruptHandler)
    {
        IOCCF2_InterruptHandler();
    }
    IOCCFbits.IOCCF2 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF2 at application runtime
*/
void IOCCF2_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF2_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF2
*/
void IOCCF2_DefaultInterruptHandler(void){
    // add your IOCCF2 interrupt custom code
    // or set custom function using IOCCF2_SetInterruptHandler()
}

/**
 End of File
*/