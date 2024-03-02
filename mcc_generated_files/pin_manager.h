/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F57Q43
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA0 procedures
#define RA0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define RA0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define RA0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define RA0_GetValue()              PORTAbits.RA0
#define RA0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define RA0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define RA0_SetPullup()             do { WPUAbits.WPUA0 = 1; } while(0)
#define RA0_ResetPullup()           do { WPUAbits.WPUA0 = 0; } while(0)
#define RA0_SetAnalogMode()         do { ANSELAbits.ANSELA0 = 1; } while(0)
#define RA0_SetDigitalMode()        do { ANSELAbits.ANSELA0 = 0; } while(0)

// get/set RA1 procedures
#define RA1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define RA1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define RA1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define RA1_GetValue()              PORTAbits.RA1
#define RA1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define RA1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define RA1_SetPullup()             do { WPUAbits.WPUA1 = 1; } while(0)
#define RA1_ResetPullup()           do { WPUAbits.WPUA1 = 0; } while(0)
#define RA1_SetAnalogMode()         do { ANSELAbits.ANSELA1 = 1; } while(0)
#define RA1_SetDigitalMode()        do { ANSELAbits.ANSELA1 = 0; } while(0)

// get/set RA2 procedures
#define RA2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RA2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RA2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RA2_GetValue()              PORTAbits.RA2
#define RA2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RA2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RA2_SetPullup()             do { WPUAbits.WPUA2 = 1; } while(0)
#define RA2_ResetPullup()           do { WPUAbits.WPUA2 = 0; } while(0)
#define RA2_SetAnalogMode()         do { ANSELAbits.ANSELA2 = 1; } while(0)
#define RA2_SetDigitalMode()        do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set RA3 procedures
#define RA3_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define RA3_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define RA3_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define RA3_GetValue()              PORTAbits.RA3
#define RA3_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define RA3_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define RA3_SetPullup()             do { WPUAbits.WPUA3 = 1; } while(0)
#define RA3_ResetPullup()           do { WPUAbits.WPUA3 = 0; } while(0)
#define RA3_SetAnalogMode()         do { ANSELAbits.ANSELA3 = 1; } while(0)
#define RA3_SetDigitalMode()        do { ANSELAbits.ANSELA3 = 0; } while(0)

// get/set SW1 aliases
#define SW1_TRIS                 TRISAbits.TRISA4
#define SW1_LAT                  LATAbits.LATA4
#define SW1_PORT                 PORTAbits.RA4
#define SW1_WPU                  WPUAbits.WPUA4
#define SW1_OD                   ODCONAbits.ODCA4
#define SW1_ANS                  ANSELAbits.ANSELA4
#define SW1_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define SW1_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define SW1_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define SW1_GetValue()           PORTAbits.RA4
#define SW1_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define SW1_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define SW1_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define SW1_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define SW1_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define SW1_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define SW1_SetAnalogMode()      do { ANSELAbits.ANSELA4 = 1; } while(0)
#define SW1_SetDigitalMode()     do { ANSELAbits.ANSELA4 = 0; } while(0)

// get/set MOTION_INT aliases
#define MOTION_INT_TRIS                 TRISAbits.TRISA5
#define MOTION_INT_LAT                  LATAbits.LATA5
#define MOTION_INT_PORT                 PORTAbits.RA5
#define MOTION_INT_WPU                  WPUAbits.WPUA5
#define MOTION_INT_OD                   ODCONAbits.ODCA5
#define MOTION_INT_ANS                  ANSELAbits.ANSELA5
#define MOTION_INT_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define MOTION_INT_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define MOTION_INT_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define MOTION_INT_GetValue()           PORTAbits.RA5
#define MOTION_INT_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define MOTION_INT_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define MOTION_INT_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define MOTION_INT_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define MOTION_INT_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define MOTION_INT_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define MOTION_INT_SetAnalogMode()      do { ANSELAbits.ANSELA5 = 1; } while(0)
#define MOTION_INT_SetDigitalMode()     do { ANSELAbits.ANSELA5 = 0; } while(0)

// get/set PT1_TO_ESP aliases
#define PT1_TO_ESP_TRIS                 TRISBbits.TRISB0
#define PT1_TO_ESP_LAT                  LATBbits.LATB0
#define PT1_TO_ESP_PORT                 PORTBbits.RB0
#define PT1_TO_ESP_WPU                  WPUBbits.WPUB0
#define PT1_TO_ESP_OD                   ODCONBbits.ODCB0
#define PT1_TO_ESP_ANS                  ANSELBbits.ANSELB0
#define PT1_TO_ESP_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define PT1_TO_ESP_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define PT1_TO_ESP_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define PT1_TO_ESP_GetValue()           PORTBbits.RB0
#define PT1_TO_ESP_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define PT1_TO_ESP_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define PT1_TO_ESP_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define PT1_TO_ESP_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define PT1_TO_ESP_SetPushPull()        do { ODCONBbits.ODCB0 = 0; } while(0)
#define PT1_TO_ESP_SetOpenDrain()       do { ODCONBbits.ODCB0 = 1; } while(0)
#define PT1_TO_ESP_SetAnalogMode()      do { ANSELBbits.ANSELB0 = 1; } while(0)
#define PT1_TO_ESP_SetDigitalMode()     do { ANSELBbits.ANSELB0 = 0; } while(0)

// get/set RB1 procedures
#define RB1_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define RB1_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define RB1_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define RB1_GetValue()              PORTBbits.RB1
#define RB1_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define RB1_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define RB1_SetPullup()             do { WPUBbits.WPUB1 = 1; } while(0)
#define RB1_ResetPullup()           do { WPUBbits.WPUB1 = 0; } while(0)
#define RB1_SetAnalogMode()         do { ANSELBbits.ANSELB1 = 1; } while(0)
#define RB1_SetDigitalMode()        do { ANSELBbits.ANSELB1 = 0; } while(0)

// get/set RB2 procedures
#define RB2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define RB2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define RB2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define RB2_GetValue()              PORTBbits.RB2
#define RB2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define RB2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define RB2_SetPullup()             do { WPUBbits.WPUB2 = 1; } while(0)
#define RB2_ResetPullup()           do { WPUBbits.WPUB2 = 0; } while(0)
#define RB2_SetAnalogMode()         do { ANSELBbits.ANSELB2 = 1; } while(0)
#define RB2_SetDigitalMode()        do { ANSELBbits.ANSELB2 = 0; } while(0)

// get/set RB3 procedures
#define RB3_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define RB3_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define RB3_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define RB3_GetValue()              PORTBbits.RB3
#define RB3_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define RB3_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define RB3_SetPullup()             do { WPUBbits.WPUB3 = 1; } while(0)
#define RB3_ResetPullup()           do { WPUBbits.WPUB3 = 0; } while(0)
#define RB3_SetAnalogMode()         do { ANSELBbits.ANSELB3 = 1; } while(0)
#define RB3_SetDigitalMode()        do { ANSELBbits.ANSELB3 = 0; } while(0)

// get/set POWER_CHRG aliases
#define POWER_CHRG_TRIS                 TRISBbits.TRISB5
#define POWER_CHRG_LAT                  LATBbits.LATB5
#define POWER_CHRG_PORT                 PORTBbits.RB5
#define POWER_CHRG_WPU                  WPUBbits.WPUB5
#define POWER_CHRG_OD                   ODCONBbits.ODCB5
#define POWER_CHRG_ANS                  ANSELBbits.ANSELB5
#define POWER_CHRG_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define POWER_CHRG_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define POWER_CHRG_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define POWER_CHRG_GetValue()           PORTBbits.RB5
#define POWER_CHRG_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define POWER_CHRG_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define POWER_CHRG_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define POWER_CHRG_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define POWER_CHRG_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define POWER_CHRG_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define POWER_CHRG_SetAnalogMode()      do { ANSELBbits.ANSELB5 = 1; } while(0)
#define POWER_CHRG_SetDigitalMode()     do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set RTCC_FOE aliases
#define RTCC_FOE_TRIS                 TRISCbits.TRISC0
#define RTCC_FOE_LAT                  LATCbits.LATC0
#define RTCC_FOE_PORT                 PORTCbits.RC0
#define RTCC_FOE_WPU                  WPUCbits.WPUC0
#define RTCC_FOE_OD                   ODCONCbits.ODCC0
#define RTCC_FOE_ANS                  ANSELCbits.ANSELC0
#define RTCC_FOE_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RTCC_FOE_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RTCC_FOE_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RTCC_FOE_GetValue()           PORTCbits.RC0
#define RTCC_FOE_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RTCC_FOE_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RTCC_FOE_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define RTCC_FOE_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define RTCC_FOE_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define RTCC_FOE_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define RTCC_FOE_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define RTCC_FOE_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set RTCC_INT aliases
#define RTCC_INT_TRIS                 TRISCbits.TRISC2
#define RTCC_INT_LAT                  LATCbits.LATC2
#define RTCC_INT_PORT                 PORTCbits.RC2
#define RTCC_INT_WPU                  WPUCbits.WPUC2
#define RTCC_INT_OD                   ODCONCbits.ODCC2
#define RTCC_INT_ANS                  ANSELCbits.ANSELC2
#define RTCC_INT_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RTCC_INT_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RTCC_INT_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RTCC_INT_GetValue()           PORTCbits.RC2
#define RTCC_INT_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RTCC_INT_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RTCC_INT_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define RTCC_INT_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define RTCC_INT_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define RTCC_INT_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define RTCC_INT_SetAnalogMode()      do { ANSELCbits.ANSELC2 = 1; } while(0)
#define RTCC_INT_SetDigitalMode()     do { ANSELCbits.ANSELC2 = 0; } while(0)

// get/set RC3 procedures
#define RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()              PORTCbits.RC3
#define RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define RC3_SetPullup()             do { WPUCbits.WPUC3 = 1; } while(0)
#define RC3_ResetPullup()           do { WPUCbits.WPUC3 = 0; } while(0)
#define RC3_SetAnalogMode()         do { ANSELCbits.ANSELC3 = 1; } while(0)
#define RC3_SetDigitalMode()        do { ANSELCbits.ANSELC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSELC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSELC4 = 0; } while(0)

// get/set TOUCH_INT aliases
#define TOUCH_INT_TRIS                 TRISCbits.TRISC5
#define TOUCH_INT_LAT                  LATCbits.LATC5
#define TOUCH_INT_PORT                 PORTCbits.RC5
#define TOUCH_INT_WPU                  WPUCbits.WPUC5
#define TOUCH_INT_OD                   ODCONCbits.ODCC5
#define TOUCH_INT_ANS                  ANSELCbits.ANSELC5
#define TOUCH_INT_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define TOUCH_INT_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define TOUCH_INT_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define TOUCH_INT_GetValue()           PORTCbits.RC5
#define TOUCH_INT_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define TOUCH_INT_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define TOUCH_INT_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define TOUCH_INT_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define TOUCH_INT_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define TOUCH_INT_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define TOUCH_INT_SetAnalogMode()      do { ANSELCbits.ANSELC5 = 1; } while(0)
#define TOUCH_INT_SetDigitalMode()     do { ANSELCbits.ANSELC5 = 0; } while(0)

// get/set LCD_CS aliases
#define LCD_CS_TRIS                 TRISCbits.TRISC6
#define LCD_CS_LAT                  LATCbits.LATC6
#define LCD_CS_PORT                 PORTCbits.RC6
#define LCD_CS_WPU                  WPUCbits.WPUC6
#define LCD_CS_OD                   ODCONCbits.ODCC6
#define LCD_CS_ANS                  ANSELCbits.ANSELC6
#define LCD_CS_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define LCD_CS_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define LCD_CS_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define LCD_CS_GetValue()           PORTCbits.RC6
#define LCD_CS_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define LCD_CS_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define LCD_CS_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define LCD_CS_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define LCD_CS_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define LCD_CS_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define LCD_CS_SetAnalogMode()      do { ANSELCbits.ANSELC6 = 1; } while(0)
#define LCD_CS_SetDigitalMode()     do { ANSELCbits.ANSELC6 = 0; } while(0)

// get/set LCD_RESET aliases
#define LCD_RESET_TRIS                 TRISCbits.TRISC7
#define LCD_RESET_LAT                  LATCbits.LATC7
#define LCD_RESET_PORT                 PORTCbits.RC7
#define LCD_RESET_WPU                  WPUCbits.WPUC7
#define LCD_RESET_OD                   ODCONCbits.ODCC7
#define LCD_RESET_ANS                  ANSELCbits.ANSELC7
#define LCD_RESET_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define LCD_RESET_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define LCD_RESET_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define LCD_RESET_GetValue()           PORTCbits.RC7
#define LCD_RESET_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define LCD_RESET_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define LCD_RESET_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define LCD_RESET_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define LCD_RESET_SetPushPull()        do { ODCONCbits.ODCC7 = 0; } while(0)
#define LCD_RESET_SetOpenDrain()       do { ODCONCbits.ODCC7 = 1; } while(0)
#define LCD_RESET_SetAnalogMode()      do { ANSELCbits.ANSELC7 = 1; } while(0)
#define LCD_RESET_SetDigitalMode()     do { ANSELCbits.ANSELC7 = 0; } while(0)

// get/set RD0 procedures
#define RD0_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define RD0_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define RD0_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define RD0_GetValue()              PORTDbits.RD0
#define RD0_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define RD0_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)
#define RD0_SetPullup()             do { WPUDbits.WPUD0 = 1; } while(0)
#define RD0_ResetPullup()           do { WPUDbits.WPUD0 = 0; } while(0)
#define RD0_SetAnalogMode()         do { ANSELDbits.ANSELD0 = 1; } while(0)
#define RD0_SetDigitalMode()        do { ANSELDbits.ANSELD0 = 0; } while(0)

// get/set RD1 procedures
#define RD1_SetHigh()            do { LATDbits.LATD1 = 1; } while(0)
#define RD1_SetLow()             do { LATDbits.LATD1 = 0; } while(0)
#define RD1_Toggle()             do { LATDbits.LATD1 = ~LATDbits.LATD1; } while(0)
#define RD1_GetValue()              PORTDbits.RD1
#define RD1_SetDigitalInput()    do { TRISDbits.TRISD1 = 1; } while(0)
#define RD1_SetDigitalOutput()   do { TRISDbits.TRISD1 = 0; } while(0)
#define RD1_SetPullup()             do { WPUDbits.WPUD1 = 1; } while(0)
#define RD1_ResetPullup()           do { WPUDbits.WPUD1 = 0; } while(0)
#define RD1_SetAnalogMode()         do { ANSELDbits.ANSELD1 = 1; } while(0)
#define RD1_SetDigitalMode()        do { ANSELDbits.ANSELD1 = 0; } while(0)

// get/set RD2 procedures
#define RD2_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define RD2_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define RD2_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define RD2_GetValue()              PORTDbits.RD2
#define RD2_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define RD2_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define RD2_SetPullup()             do { WPUDbits.WPUD2 = 1; } while(0)
#define RD2_ResetPullup()           do { WPUDbits.WPUD2 = 0; } while(0)
#define RD2_SetAnalogMode()         do { ANSELDbits.ANSELD2 = 1; } while(0)
#define RD2_SetDigitalMode()        do { ANSELDbits.ANSELD2 = 0; } while(0)

// get/set RD3 procedures
#define RD3_SetHigh()            do { LATDbits.LATD3 = 1; } while(0)
#define RD3_SetLow()             do { LATDbits.LATD3 = 0; } while(0)
#define RD3_Toggle()             do { LATDbits.LATD3 = ~LATDbits.LATD3; } while(0)
#define RD3_GetValue()              PORTDbits.RD3
#define RD3_SetDigitalInput()    do { TRISDbits.TRISD3 = 1; } while(0)
#define RD3_SetDigitalOutput()   do { TRISDbits.TRISD3 = 0; } while(0)
#define RD3_SetPullup()             do { WPUDbits.WPUD3 = 1; } while(0)
#define RD3_ResetPullup()           do { WPUDbits.WPUD3 = 0; } while(0)
#define RD3_SetAnalogMode()         do { ANSELDbits.ANSELD3 = 1; } while(0)
#define RD3_SetDigitalMode()        do { ANSELDbits.ANSELD3 = 0; } while(0)

// get/set LCD_DC aliases
#define LCD_DC_TRIS                 TRISDbits.TRISD4
#define LCD_DC_LAT                  LATDbits.LATD4
#define LCD_DC_PORT                 PORTDbits.RD4
#define LCD_DC_WPU                  WPUDbits.WPUD4
#define LCD_DC_OD                   ODCONDbits.ODCD4
#define LCD_DC_ANS                  ANSELDbits.ANSELD4
#define LCD_DC_SetHigh()            do { LATDbits.LATD4 = 1; } while(0)
#define LCD_DC_SetLow()             do { LATDbits.LATD4 = 0; } while(0)
#define LCD_DC_Toggle()             do { LATDbits.LATD4 = ~LATDbits.LATD4; } while(0)
#define LCD_DC_GetValue()           PORTDbits.RD4
#define LCD_DC_SetDigitalInput()    do { TRISDbits.TRISD4 = 1; } while(0)
#define LCD_DC_SetDigitalOutput()   do { TRISDbits.TRISD4 = 0; } while(0)
#define LCD_DC_SetPullup()          do { WPUDbits.WPUD4 = 1; } while(0)
#define LCD_DC_ResetPullup()        do { WPUDbits.WPUD4 = 0; } while(0)
#define LCD_DC_SetPushPull()        do { ODCONDbits.ODCD4 = 0; } while(0)
#define LCD_DC_SetOpenDrain()       do { ODCONDbits.ODCD4 = 1; } while(0)
#define LCD_DC_SetAnalogMode()      do { ANSELDbits.ANSELD4 = 1; } while(0)
#define LCD_DC_SetDigitalMode()     do { ANSELDbits.ANSELD4 = 0; } while(0)

// get/set RD5 procedures
#define RD5_SetHigh()            do { LATDbits.LATD5 = 1; } while(0)
#define RD5_SetLow()             do { LATDbits.LATD5 = 0; } while(0)
#define RD5_Toggle()             do { LATDbits.LATD5 = ~LATDbits.LATD5; } while(0)
#define RD5_GetValue()              PORTDbits.RD5
#define RD5_SetDigitalInput()    do { TRISDbits.TRISD5 = 1; } while(0)
#define RD5_SetDigitalOutput()   do { TRISDbits.TRISD5 = 0; } while(0)
#define RD5_SetPullup()             do { WPUDbits.WPUD5 = 1; } while(0)
#define RD5_ResetPullup()           do { WPUDbits.WPUD5 = 0; } while(0)
#define RD5_SetAnalogMode()         do { ANSELDbits.ANSELD5 = 1; } while(0)
#define RD5_SetDigitalMode()        do { ANSELDbits.ANSELD5 = 0; } while(0)

// get/set RD6 procedures
#define RD6_SetHigh()            do { LATDbits.LATD6 = 1; } while(0)
#define RD6_SetLow()             do { LATDbits.LATD6 = 0; } while(0)
#define RD6_Toggle()             do { LATDbits.LATD6 = ~LATDbits.LATD6; } while(0)
#define RD6_GetValue()              PORTDbits.RD6
#define RD6_SetDigitalInput()    do { TRISDbits.TRISD6 = 1; } while(0)
#define RD6_SetDigitalOutput()   do { TRISDbits.TRISD6 = 0; } while(0)
#define RD6_SetPullup()             do { WPUDbits.WPUD6 = 1; } while(0)
#define RD6_ResetPullup()           do { WPUDbits.WPUD6 = 0; } while(0)
#define RD6_SetAnalogMode()         do { ANSELDbits.ANSELD6 = 1; } while(0)
#define RD6_SetDigitalMode()        do { ANSELDbits.ANSELD6 = 0; } while(0)

// get/set RD7 procedures
#define RD7_SetHigh()            do { LATDbits.LATD7 = 1; } while(0)
#define RD7_SetLow()             do { LATDbits.LATD7 = 0; } while(0)
#define RD7_Toggle()             do { LATDbits.LATD7 = ~LATDbits.LATD7; } while(0)
#define RD7_GetValue()              PORTDbits.RD7
#define RD7_SetDigitalInput()    do { TRISDbits.TRISD7 = 1; } while(0)
#define RD7_SetDigitalOutput()   do { TRISDbits.TRISD7 = 0; } while(0)
#define RD7_SetPullup()             do { WPUDbits.WPUD7 = 1; } while(0)
#define RD7_ResetPullup()           do { WPUDbits.WPUD7 = 0; } while(0)
#define RD7_SetAnalogMode()         do { ANSELDbits.ANSELD7 = 1; } while(0)
#define RD7_SetDigitalMode()        do { ANSELDbits.ANSELD7 = 0; } while(0)

// get/set DIVIDER_ON aliases
#define DIVIDER_ON_TRIS                 TRISEbits.TRISE0
#define DIVIDER_ON_LAT                  LATEbits.LATE0
#define DIVIDER_ON_PORT                 PORTEbits.RE0
#define DIVIDER_ON_WPU                  WPUEbits.WPUE0
#define DIVIDER_ON_OD                   ODCONEbits.ODCE0
#define DIVIDER_ON_ANS                  ANSELEbits.ANSELE0
#define DIVIDER_ON_SetHigh()            do { LATEbits.LATE0 = 1; } while(0)
#define DIVIDER_ON_SetLow()             do { LATEbits.LATE0 = 0; } while(0)
#define DIVIDER_ON_Toggle()             do { LATEbits.LATE0 = ~LATEbits.LATE0; } while(0)
#define DIVIDER_ON_GetValue()           PORTEbits.RE0
#define DIVIDER_ON_SetDigitalInput()    do { TRISEbits.TRISE0 = 1; } while(0)
#define DIVIDER_ON_SetDigitalOutput()   do { TRISEbits.TRISE0 = 0; } while(0)
#define DIVIDER_ON_SetPullup()          do { WPUEbits.WPUE0 = 1; } while(0)
#define DIVIDER_ON_ResetPullup()        do { WPUEbits.WPUE0 = 0; } while(0)
#define DIVIDER_ON_SetPushPull()        do { ODCONEbits.ODCE0 = 0; } while(0)
#define DIVIDER_ON_SetOpenDrain()       do { ODCONEbits.ODCE0 = 1; } while(0)
#define DIVIDER_ON_SetAnalogMode()      do { ANSELEbits.ANSELE0 = 1; } while(0)
#define DIVIDER_ON_SetDigitalMode()     do { ANSELEbits.ANSELE0 = 0; } while(0)

// get/set BAT_V_AN aliases
#define BAT_V_AN_TRIS                 TRISEbits.TRISE1
#define BAT_V_AN_LAT                  LATEbits.LATE1
#define BAT_V_AN_PORT                 PORTEbits.RE1
#define BAT_V_AN_WPU                  WPUEbits.WPUE1
#define BAT_V_AN_OD                   ODCONEbits.ODCE1
#define BAT_V_AN_ANS                  ANSELEbits.ANSELE1
#define BAT_V_AN_SetHigh()            do { LATEbits.LATE1 = 1; } while(0)
#define BAT_V_AN_SetLow()             do { LATEbits.LATE1 = 0; } while(0)
#define BAT_V_AN_Toggle()             do { LATEbits.LATE1 = ~LATEbits.LATE1; } while(0)
#define BAT_V_AN_GetValue()           PORTEbits.RE1
#define BAT_V_AN_SetDigitalInput()    do { TRISEbits.TRISE1 = 1; } while(0)
#define BAT_V_AN_SetDigitalOutput()   do { TRISEbits.TRISE1 = 0; } while(0)
#define BAT_V_AN_SetPullup()          do { WPUEbits.WPUE1 = 1; } while(0)
#define BAT_V_AN_ResetPullup()        do { WPUEbits.WPUE1 = 0; } while(0)
#define BAT_V_AN_SetPushPull()        do { ODCONEbits.ODCE1 = 0; } while(0)
#define BAT_V_AN_SetOpenDrain()       do { ODCONEbits.ODCE1 = 1; } while(0)
#define BAT_V_AN_SetAnalogMode()      do { ANSELEbits.ANSELE1 = 1; } while(0)
#define BAT_V_AN_SetDigitalMode()     do { ANSELEbits.ANSELE1 = 0; } while(0)

// get/set SW2 aliases
#define SW2_TRIS                 TRISEbits.TRISE2
#define SW2_LAT                  LATEbits.LATE2
#define SW2_PORT                 PORTEbits.RE2
#define SW2_WPU                  WPUEbits.WPUE2
#define SW2_OD                   ODCONEbits.ODCE2
#define SW2_ANS                  ANSELEbits.ANSELE2
#define SW2_SetHigh()            do { LATEbits.LATE2 = 1; } while(0)
#define SW2_SetLow()             do { LATEbits.LATE2 = 0; } while(0)
#define SW2_Toggle()             do { LATEbits.LATE2 = ~LATEbits.LATE2; } while(0)
#define SW2_GetValue()           PORTEbits.RE2
#define SW2_SetDigitalInput()    do { TRISEbits.TRISE2 = 1; } while(0)
#define SW2_SetDigitalOutput()   do { TRISEbits.TRISE2 = 0; } while(0)
#define SW2_SetPullup()          do { WPUEbits.WPUE2 = 1; } while(0)
#define SW2_ResetPullup()        do { WPUEbits.WPUE2 = 0; } while(0)
#define SW2_SetPushPull()        do { ODCONEbits.ODCE2 = 0; } while(0)
#define SW2_SetOpenDrain()       do { ODCONEbits.ODCE2 = 1; } while(0)
#define SW2_SetAnalogMode()      do { ANSELEbits.ANSELE2 = 1; } while(0)
#define SW2_SetDigitalMode()     do { ANSELEbits.ANSELE2 = 0; } while(0)

// get/set SW3 aliases
#define SW3_TRIS                 TRISFbits.TRISF0
#define SW3_LAT                  LATFbits.LATF0
#define SW3_PORT                 PORTFbits.RF0
#define SW3_WPU                  WPUFbits.WPUF0
#define SW3_OD                   ODCONFbits.ODCF0
#define SW3_ANS                  ANSELFbits.ANSELF0
#define SW3_SetHigh()            do { LATFbits.LATF0 = 1; } while(0)
#define SW3_SetLow()             do { LATFbits.LATF0 = 0; } while(0)
#define SW3_Toggle()             do { LATFbits.LATF0 = ~LATFbits.LATF0; } while(0)
#define SW3_GetValue()           PORTFbits.RF0
#define SW3_SetDigitalInput()    do { TRISFbits.TRISF0 = 1; } while(0)
#define SW3_SetDigitalOutput()   do { TRISFbits.TRISF0 = 0; } while(0)
#define SW3_SetPullup()          do { WPUFbits.WPUF0 = 1; } while(0)
#define SW3_ResetPullup()        do { WPUFbits.WPUF0 = 0; } while(0)
#define SW3_SetPushPull()        do { ODCONFbits.ODCF0 = 0; } while(0)
#define SW3_SetOpenDrain()       do { ODCONFbits.ODCF0 = 1; } while(0)
#define SW3_SetAnalogMode()      do { ANSELFbits.ANSELF0 = 1; } while(0)
#define SW3_SetDigitalMode()     do { ANSELFbits.ANSELF0 = 0; } while(0)

// get/set LED_RIGHT aliases
#define LED_RIGHT_TRIS                 TRISFbits.TRISF1
#define LED_RIGHT_LAT                  LATFbits.LATF1
#define LED_RIGHT_PORT                 PORTFbits.RF1
#define LED_RIGHT_WPU                  WPUFbits.WPUF1
#define LED_RIGHT_OD                   ODCONFbits.ODCF1
#define LED_RIGHT_ANS                  ANSELFbits.ANSELF1
#define LED_RIGHT_SetHigh()            do { LATFbits.LATF1 = 1; } while(0)
#define LED_RIGHT_SetLow()             do { LATFbits.LATF1 = 0; } while(0)
#define LED_RIGHT_Toggle()             do { LATFbits.LATF1 = ~LATFbits.LATF1; } while(0)
#define LED_RIGHT_GetValue()           PORTFbits.RF1
#define LED_RIGHT_SetDigitalInput()    do { TRISFbits.TRISF1 = 1; } while(0)
#define LED_RIGHT_SetDigitalOutput()   do { TRISFbits.TRISF1 = 0; } while(0)
#define LED_RIGHT_SetPullup()          do { WPUFbits.WPUF1 = 1; } while(0)
#define LED_RIGHT_ResetPullup()        do { WPUFbits.WPUF1 = 0; } while(0)
#define LED_RIGHT_SetPushPull()        do { ODCONFbits.ODCF1 = 0; } while(0)
#define LED_RIGHT_SetOpenDrain()       do { ODCONFbits.ODCF1 = 1; } while(0)
#define LED_RIGHT_SetAnalogMode()      do { ANSELFbits.ANSELF1 = 1; } while(0)
#define LED_RIGHT_SetDigitalMode()     do { ANSELFbits.ANSELF1 = 0; } while(0)

// get/set LED_LEFT aliases
#define LED_LEFT_TRIS                 TRISFbits.TRISF2
#define LED_LEFT_LAT                  LATFbits.LATF2
#define LED_LEFT_PORT                 PORTFbits.RF2
#define LED_LEFT_WPU                  WPUFbits.WPUF2
#define LED_LEFT_OD                   ODCONFbits.ODCF2
#define LED_LEFT_ANS                  ANSELFbits.ANSELF2
#define LED_LEFT_SetHigh()            do { LATFbits.LATF2 = 1; } while(0)
#define LED_LEFT_SetLow()             do { LATFbits.LATF2 = 0; } while(0)
#define LED_LEFT_Toggle()             do { LATFbits.LATF2 = ~LATFbits.LATF2; } while(0)
#define LED_LEFT_GetValue()           PORTFbits.RF2
#define LED_LEFT_SetDigitalInput()    do { TRISFbits.TRISF2 = 1; } while(0)
#define LED_LEFT_SetDigitalOutput()   do { TRISFbits.TRISF2 = 0; } while(0)
#define LED_LEFT_SetPullup()          do { WPUFbits.WPUF2 = 1; } while(0)
#define LED_LEFT_ResetPullup()        do { WPUFbits.WPUF2 = 0; } while(0)
#define LED_LEFT_SetPushPull()        do { ODCONFbits.ODCF2 = 0; } while(0)
#define LED_LEFT_SetOpenDrain()       do { ODCONFbits.ODCF2 = 1; } while(0)
#define LED_LEFT_SetAnalogMode()      do { ANSELFbits.ANSELF2 = 1; } while(0)
#define LED_LEFT_SetDigitalMode()     do { ANSELFbits.ANSELF2 = 0; } while(0)

// get/set RF3 procedures
#define RF3_SetHigh()            do { LATFbits.LATF3 = 1; } while(0)
#define RF3_SetLow()             do { LATFbits.LATF3 = 0; } while(0)
#define RF3_Toggle()             do { LATFbits.LATF3 = ~LATFbits.LATF3; } while(0)
#define RF3_GetValue()              PORTFbits.RF3
#define RF3_SetDigitalInput()    do { TRISFbits.TRISF3 = 1; } while(0)
#define RF3_SetDigitalOutput()   do { TRISFbits.TRISF3 = 0; } while(0)
#define RF3_SetPullup()             do { WPUFbits.WPUF3 = 1; } while(0)
#define RF3_ResetPullup()           do { WPUFbits.WPUF3 = 0; } while(0)
#define RF3_SetAnalogMode()         do { ANSELFbits.ANSELF3 = 1; } while(0)
#define RF3_SetDigitalMode()        do { ANSELFbits.ANSELF3 = 0; } while(0)

// get/set SDCard_CS aliases
#define SDCard_CS_TRIS                 TRISFbits.TRISF4
#define SDCard_CS_LAT                  LATFbits.LATF4
#define SDCard_CS_PORT                 PORTFbits.RF4
#define SDCard_CS_WPU                  WPUFbits.WPUF4
#define SDCard_CS_OD                   ODCONFbits.ODCF4
#define SDCard_CS_ANS                  ANSELFbits.ANSELF4
#define SDCard_CS_SetHigh()            do { LATFbits.LATF4 = 1; } while(0)
#define SDCard_CS_SetLow()             do { LATFbits.LATF4 = 0; } while(0)
#define SDCard_CS_Toggle()             do { LATFbits.LATF4 = ~LATFbits.LATF4; } while(0)
#define SDCard_CS_GetValue()           PORTFbits.RF4
#define SDCard_CS_SetDigitalInput()    do { TRISFbits.TRISF4 = 1; } while(0)
#define SDCard_CS_SetDigitalOutput()   do { TRISFbits.TRISF4 = 0; } while(0)
#define SDCard_CS_SetPullup()          do { WPUFbits.WPUF4 = 1; } while(0)
#define SDCard_CS_ResetPullup()        do { WPUFbits.WPUF4 = 0; } while(0)
#define SDCard_CS_SetPushPull()        do { ODCONFbits.ODCF4 = 0; } while(0)
#define SDCard_CS_SetOpenDrain()       do { ODCONFbits.ODCF4 = 1; } while(0)
#define SDCard_CS_SetAnalogMode()      do { ANSELFbits.ANSELF4 = 1; } while(0)
#define SDCard_CS_SetDigitalMode()     do { ANSELFbits.ANSELF4 = 0; } while(0)

// get/set TOUCH_CS aliases
#define TOUCH_CS_TRIS                 TRISFbits.TRISF5
#define TOUCH_CS_LAT                  LATFbits.LATF5
#define TOUCH_CS_PORT                 PORTFbits.RF5
#define TOUCH_CS_WPU                  WPUFbits.WPUF5
#define TOUCH_CS_OD                   ODCONFbits.ODCF5
#define TOUCH_CS_ANS                  ANSELFbits.ANSELF5
#define TOUCH_CS_SetHigh()            do { LATFbits.LATF5 = 1; } while(0)
#define TOUCH_CS_SetLow()             do { LATFbits.LATF5 = 0; } while(0)
#define TOUCH_CS_Toggle()             do { LATFbits.LATF5 = ~LATFbits.LATF5; } while(0)
#define TOUCH_CS_GetValue()           PORTFbits.RF5
#define TOUCH_CS_SetDigitalInput()    do { TRISFbits.TRISF5 = 1; } while(0)
#define TOUCH_CS_SetDigitalOutput()   do { TRISFbits.TRISF5 = 0; } while(0)
#define TOUCH_CS_SetPullup()          do { WPUFbits.WPUF5 = 1; } while(0)
#define TOUCH_CS_ResetPullup()        do { WPUFbits.WPUF5 = 0; } while(0)
#define TOUCH_CS_SetPushPull()        do { ODCONFbits.ODCF5 = 0; } while(0)
#define TOUCH_CS_SetOpenDrain()       do { ODCONFbits.ODCF5 = 1; } while(0)
#define TOUCH_CS_SetAnalogMode()      do { ANSELFbits.ANSELF5 = 1; } while(0)
#define TOUCH_CS_SetDigitalMode()     do { ANSELFbits.ANSELF5 = 0; } while(0)

// get/set POWER_EN2 aliases
#define POWER_EN2_TRIS                 TRISFbits.TRISF6
#define POWER_EN2_LAT                  LATFbits.LATF6
#define POWER_EN2_PORT                 PORTFbits.RF6
#define POWER_EN2_WPU                  WPUFbits.WPUF6
#define POWER_EN2_OD                   ODCONFbits.ODCF6
#define POWER_EN2_ANS                  ANSELFbits.ANSELF6
#define POWER_EN2_SetHigh()            do { LATFbits.LATF6 = 1; } while(0)
#define POWER_EN2_SetLow()             do { LATFbits.LATF6 = 0; } while(0)
#define POWER_EN2_Toggle()             do { LATFbits.LATF6 = ~LATFbits.LATF6; } while(0)
#define POWER_EN2_GetValue()           PORTFbits.RF6
#define POWER_EN2_SetDigitalInput()    do { TRISFbits.TRISF6 = 1; } while(0)
#define POWER_EN2_SetDigitalOutput()   do { TRISFbits.TRISF6 = 0; } while(0)
#define POWER_EN2_SetPullup()          do { WPUFbits.WPUF6 = 1; } while(0)
#define POWER_EN2_ResetPullup()        do { WPUFbits.WPUF6 = 0; } while(0)
#define POWER_EN2_SetPushPull()        do { ODCONFbits.ODCF6 = 0; } while(0)
#define POWER_EN2_SetOpenDrain()       do { ODCONFbits.ODCF6 = 1; } while(0)
#define POWER_EN2_SetAnalogMode()      do { ANSELFbits.ANSELF6 = 1; } while(0)
#define POWER_EN2_SetDigitalMode()     do { ANSELFbits.ANSELF6 = 0; } while(0)

// get/set POWER_MODE aliases
#define POWER_MODE_TRIS                 TRISFbits.TRISF7
#define POWER_MODE_LAT                  LATFbits.LATF7
#define POWER_MODE_PORT                 PORTFbits.RF7
#define POWER_MODE_WPU                  WPUFbits.WPUF7
#define POWER_MODE_OD                   ODCONFbits.ODCF7
#define POWER_MODE_ANS                  ANSELFbits.ANSELF7
#define POWER_MODE_SetHigh()            do { LATFbits.LATF7 = 1; } while(0)
#define POWER_MODE_SetLow()             do { LATFbits.LATF7 = 0; } while(0)
#define POWER_MODE_Toggle()             do { LATFbits.LATF7 = ~LATFbits.LATF7; } while(0)
#define POWER_MODE_GetValue()           PORTFbits.RF7
#define POWER_MODE_SetDigitalInput()    do { TRISFbits.TRISF7 = 1; } while(0)
#define POWER_MODE_SetDigitalOutput()   do { TRISFbits.TRISF7 = 0; } while(0)
#define POWER_MODE_SetPullup()          do { WPUFbits.WPUF7 = 1; } while(0)
#define POWER_MODE_ResetPullup()        do { WPUFbits.WPUF7 = 0; } while(0)
#define POWER_MODE_SetPushPull()        do { ODCONFbits.ODCF7 = 0; } while(0)
#define POWER_MODE_SetOpenDrain()       do { ODCONFbits.ODCF7 = 1; } while(0)
#define POWER_MODE_SetAnalogMode()      do { ANSELFbits.ANSELF7 = 1; } while(0)
#define POWER_MODE_SetDigitalMode()     do { ANSELFbits.ANSELF7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF4 pin functionality
 * @Example
    IOCAF4_ISR();
 */
void IOCAF4_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF4 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF4 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF4_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF4 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF4_SetInterruptHandler() method.
    This handler is called every time the IOCAF4 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(IOCAF4_InterruptHandler);

*/
extern void (*IOCAF4_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF4 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF4_SetInterruptHandler() method.
    This handler is called every time the IOCAF4 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF4_SetInterruptHandler(IOCAF4_DefaultInterruptHandler);

*/
void IOCAF4_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF5 pin functionality
 * @Example
    IOCAF5_ISR();
 */
void IOCAF5_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF5 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF5 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF5_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF5 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_InterruptHandler);

*/
extern void (*IOCAF5_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF5 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_DefaultInterruptHandler);

*/
void IOCAF5_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCCF2 pin functionality
 * @Example
    IOCCF2_ISR();
 */
void IOCCF2_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCCF2 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCCF2 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(MyInterruptHandler);

*/
void IOCCF2_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCCF2 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_InterruptHandler);

*/
extern void (*IOCCF2_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCCF2 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCCF2_SetInterruptHandler() method.
    This handler is called every time the IOCCF2 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCCF2_SetInterruptHandler(IOCCF2_DefaultInterruptHandler);

*/
void IOCCF2_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/