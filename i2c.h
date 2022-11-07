/*
 *  I2C
 *  
 * 
 * 2021.09.24
 * 
 */
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_H
#define	I2C_H

typedef enum { I2C1_GOOD, I2C1_FAIL_TIMEOUT } i2c1_error;


void i2c_master_initialize(void);
void i2c_master_readblock(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
uint8_t i2c_master_read1byte(uint8_t address, uint8_t reg);
void i2c_master_write1byte(uint8_t address, uint8_t reg, uint8_t data);

#endif	/* I2C_H */

