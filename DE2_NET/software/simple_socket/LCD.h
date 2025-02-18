/*
 * lcd.h
 *
 *  Created on: Oct 29, 2020
 *      Author: mathe
 */

#ifndef LCD_H_
#define LCD_H_

#define LOGIC_1 0x01
#define LOGIC_0 0x00

#define CLEAR_CMD 0x001
#define RETURN_HOME_CMD 0x002

#define SET_DDRAM_ADDR_PREFIX 0x080

#define WRITE_PREFIX 0x200
#define READ_PREFIX 0x300

void lcdInit();
void updateValue(int value);


#endif /* LCD_H_ */
