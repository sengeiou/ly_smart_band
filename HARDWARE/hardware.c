#include "hardware.h"

void hardware_init(hardware_t *p_hardware)
{
	drv_lcd_init(&p_hardware->drv_lcd);
	drv_uart_init(&p_hardware->drv_uart);
}
