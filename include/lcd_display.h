#pragma once

#ifdef CON_DISPLAY

// Display LCD 1602 por I2C (PIN_LCD_SDA / PIN_LCD_SCL, dirección LCD_I2C_ADDR).
// Si el display no responde en el arranque, el módulo queda deshabilitado.
void lcd_display_setup();   // inicializa I2C y el LCD (si está presente)
void lcd_display_poll();    // actualiza la pantalla según el estado; llamar desde loop()

#endif // CON_DISPLAY
