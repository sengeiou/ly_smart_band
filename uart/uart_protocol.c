#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "crc16.h"
#include "sys_info.h"
#include "sys_malloc.h"
#include "uart_protocol.h"
#include "uart_protocol_ble_central_handler.h"
#include "uart_protocol_ble_peripheral_handler.h"

static uint8_t sequence;

uint8_t uart_protocol_tx_working_buffer[UART_PROTOCOL_TX_WORKING_BUFFER_SIZE];

void uart_protocol_assemble_command_and_send(uint8_t group_id, uint8_t command_id, uint8_t *p_payload, uint16_t payload_length)
{
     uint16_t index = 0;
     uint16_t crc16;
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((DEVICE_TO_PC_HEADER >> 0) & 0xFF);
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((DEVICE_TO_PC_HEADER >> 8) & 0xFF);
     uart_protocol_tx_working_buffer[index++] = sequence++;
     uart_protocol_tx_working_buffer[index++] = group_id;
     uart_protocol_tx_working_buffer[index++] = command_id;
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((payload_length >> 0) & 0xFF);
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((payload_length >> 8) & 0xFF);
     memcpy(&uart_protocol_tx_working_buffer[index], p_payload, payload_length);
     index += payload_length;
     crc16 = crc16_compute(&uart_protocol_tx_working_buffer[2], index - 2, NULL);
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((crc16 >> 0) & 0xFF);
     uart_protocol_tx_working_buffer[index++] = (uint8_t)((crc16 >> 8) & 0xFF);
	      
     uint8_t *p_start_addr;
     //NRF_LOG_INFO("Ready allocate %04x space\r\n", index);

	 sys_info.hardware.drv_uart.drv_uart_tx_command_handler(uart_protocol_tx_working_buffer, index);
	 #if 0
	 p_start_addr = sys_malloc_apply(index, MEMORY_USAGE_UART_TX, __LINE__);
    
	 if(p_start_addr != NULL)
	 {
        NRF_LOG_INFO("store data into queue\r\n");
        memcpy(p_start_addr, uart_protocol_tx_working_buffer, index);
		sys_queue_put(&sys_info.tx_queue, ELEMENT_TYPE_UART, p_start_addr, index);
	 }
     else
     {
        NRF_LOG_INFO("p_start_addr == NULL\r\n");
     }
	 #endif
}

static void uart_protocol_group_id_hardware_handler(uint8_t *p_data, uint16_t length)
{
    switch(p_data[UART_COMMAND_COMMAND_ID_OFFSET])
    {
        case GID_HARDWARE_CID_PTD_LCD_FILL:
            NRF_LOG_INFO("LCD_FILL\r\n");
            uint16_t x1, y1, x2, y2, color;
            x1 = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 0]);
            y1 = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 2]);
            x2 = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 4]);
            y2 = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 6]);
            color = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 8]);
            NRF_LOG_INFO("x1:%d, y1:%d, x2:%d, y2:%d, color:%04x\r\n", x1, y1, x2, y2, color);
            sys_info.hardware.drv_lcd.drv_lcd_fill(x1, y1, x2, y2, color);
			break;
		case GID_HARDWARE_CID_PTD_LCD_DRAW_POINT:
			break;
		case GID_HARDWARE_CID_PTD_LCD_DRAW_LINE:
			break;
		case GID_HARDWARE_CID_PTD_LCD_DRAW_RECTANGLE:
			break;
		case GID_HARDWARE_CID_PTD_LCD_DRAW_PICTURE:
			NRF_LOG_INFO("LCD_DRAW_PICTURE\r\n");
			uint16_t x, y, length, width;
			x = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 0]);
			y = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 2]);
			length = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 4]);
			width = uint16_decode(&p_data[UART_COMMAND_PAYLOAD_OFFSET + 6]);
            NRF_LOG_INFO("x = %d, y = %d, length = %d, width = %d\r\n", x, y, length, width);
            sys_info.hardware.drv_lcd.drv_lcd_draw_picture(x, y, length, width, &p_data[UART_COMMAND_PAYLOAD_OFFSET + 8]);
			//sanity to check the picture size
			//p_sys_info->p_hardware->p_drv_lcd->drv_lcd_draw_picture(x, y, length, width, &p_data[COMMAND_PAYLOAD_OFFSET + 7]);
			break;
        default:
            break;
    }
}

static void uart_protocol_group_id_ble_handler(uint8_t *p_data, uint16_t length)
{
    switch(p_data[UART_COMMAND_COMMAND_ID_OFFSET])
    {
        case 0x01:
            break;
        default:
            break;
    }
}

static void uart_protocol_group_id_ack_handler(uint8_t *p_data, uint16_t length)
{
    switch(p_data[UART_COMMAND_COMMAND_ID_OFFSET])
    {
        case 0x01:
            break;
        default:
            break;
    }
}

static void uart_protocol_send_ack(uint8_t group_id, uint8_t command_id)
{
	uint8_t buffer[2];
	buffer[0] = group_id;
	buffer[1] = command_id;
	
	uart_protocol_assemble_command_and_send(GROUP_ID_ACK, 0x00, buffer, 2);
}

static void uart_protocol_rx_handler(uint8_t *p_data, uint16_t length)
{
	//First do crc16 checksum
    uint16_t crc16_01, crc16_02;
    crc16_01 = (p_data[length - 2] << 0) + (p_data[length - 1] << 8);
    crc16_02 = crc16_compute(&p_data[2], length - 2 - 2, NULL);
    NRF_LOG_INFO("crc16_01 = %x, crc16_02 = %x\r\n", crc16_01, crc16_02);
    if(crc16_01 != crc16_02)
    {
        NRF_LOG_INFO("crc16 checksum error!");
        return;
    }

    switch(p_data[UART_COMMAND_GROUP_ID_OFFSET])
    {
        case GROUP_ID_HARDWARE:
            uart_protocol_group_id_hardware_handler(p_data, length);
            break;
        case GROUP_ID_BLE:
            uart_protocol_group_id_ble_handler(p_data, length);
            break;
        case GROUP_ID_BLE_P:   //BLE Peripheral
        	uart_protocol_group_id_ble_peripheral_handler(p_data, length);
            break;
        case GROUP_ID_BLE_C:   //BLE Central
        	uart_protocol_group_id_ble_central_handler(p_data, length);
            break;
        case GROUP_ID_BLE_MESH:
            break;
        case GROUP_ID_BLE_AUDIO:
            break;
        case GROUP_ID_ACK:
            uart_protocol_group_id_ack_handler(p_data, length);
            break;
        default:
            break;
    }
}

void uart_protocol_init(uart_protocol_t *p_uart_protocol)
{
	p_uart_protocol->uart_protocol_send_ack    = uart_protocol_send_ack;
	p_uart_protocol->uart_protocol_rx_handler  = uart_protocol_rx_handler;
    p_uart_protocol->uart_protocol_assemble_command_and_send = uart_protocol_assemble_command_and_send;
}
