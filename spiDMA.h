//
//  spi_dma.h
//
//  2019.09.15
//

#ifndef SPIDMA_H
#define SPIDMA_H

//SPI Tx only
void LCD_spi_Txonly_open(void);
void LCD_spi_Txonly_close(void);
void LCD_spi_Txonly_sendCommand(uint8_t);
void LCD_spi_Txonly_sendByte(uint8_t );
void LCD_spi_Txonly_sendWord(uint16_t);
void LCD_spi_Txonly_sendBlock(uint8_t *, uint16_t);
void LCD_spi_Txonly_address_set(uint16_t, uint16_t, uint16_t, uint16_t);
//SPI DMA
void LCD_spi_dma(uint8_t *, uint16_t);
void LCD_spi_dma_onceagain(void);

//test
void LCD_spi_dma_test(void);


#endif //SPIDMA_H
