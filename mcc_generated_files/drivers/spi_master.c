/**
\file
\addtogroup doc_driver_spi_code
\brief This file contains the functions that implement the SPI master driver functionalities.

\copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
\page License
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
 */

#include "spi_master.h"

bool TOUCH_open(void);
bool LCDDMA_open(void);
bool SDFAST_open(void);
bool LCD_open(void);
bool SDSLOW_open(void);

const spi_master_functions_t spiMaster[] = {   
    { SPI2_Close, TOUCH_open, SPI2_ExchangeByte, SPI2_ExchangeBlock, SPI2_WriteBlock, SPI2_ReadBlock, SPI2_WriteByte, SPI2_ReadByte, NULL, NULL },
    { SPI2_Close, LCDDMA_open, SPI2_ExchangeByte, SPI2_ExchangeBlock, SPI2_WriteBlock, SPI2_ReadBlock, SPI2_WriteByte, SPI2_ReadByte, NULL, NULL },
    { SPI1_Close, SDFAST_open, SPI1_ExchangeByte, SPI1_ExchangeBlock, SPI1_WriteBlock, SPI1_ReadBlock, SPI1_WriteByte, SPI1_ReadByte, NULL, NULL },
    { SPI2_Close, LCD_open, SPI2_ExchangeByte, SPI2_ExchangeBlock, SPI2_WriteBlock, SPI2_ReadBlock, SPI2_WriteByte, SPI2_ReadByte, NULL, NULL },
    { SPI1_Close, SDSLOW_open, SPI1_ExchangeByte, SPI1_ExchangeBlock, SPI1_WriteBlock, SPI1_ReadBlock, SPI1_WriteByte, SPI1_ReadByte, NULL, NULL }
};

bool TOUCH_open(void){
    return SPI2_Open(TOUCH_CONFIG);
}

bool LCDDMA_open(void){
    return SPI2_Open(LCDDMA_CONFIG);
}

bool SDFAST_open(void){
    return SPI1_Open(SDFAST_CONFIG);
}

bool LCD_open(void){
    return SPI2_Open(LCD_CONFIG);
}

bool SDSLOW_open(void){
    return SPI1_Open(SDSLOW_CONFIG);
}

/**
 *  \ingroup doc_driver_spi_code
 *  \brief Open the SPI interface.
 *
 *  This function is to keep the backward compatibility with older API users
 *  \param[in] configuration The configuration to use in the transfer
 *
 *  \return Initialization status.
 *  \retval false The SPI open was unsuccessful
 *  \retval true  The SPI open was successful
 */
bool spi_master_open(spi_master_configurations_t config){
    switch(config){
        case TOUCH:
            return TOUCH_open();
        case LCDDMA:
            return LCDDMA_open();
        case SDFAST:
            return SDFAST_open();
        case LCD:
            return LCD_open();
        case SDSLOW:
            return SDSLOW_open();
        default:
            return 0;
    }
}

/**
 End of File
 */
