#ifndef __DRAW_ICON_H__
#define __DRAW_ICON_H__

#include "sdk_common.h"
#include "display_area.h"

typedef enum
{
	//ui_main
	ICON_ID_BLUETOOTH,
	ICON_ID_BATTERY_00,
	ICON_ID_BATTERY_01,
	ICON_ID_BATTERY_02,
	ICON_ID_BATTERY_03,
	ICON_ID_BATTERY_04,
	ICON_ID_HEART_RATE,
	ICON_ID_BLOOD_PRESSURE,
	ICON_ID_OVAL_FRAME_01,
	ICON_ID_OVAL_FRAME_02,
	//ui_analog_clock
	ICON_ID_ANALOG_CLOCK_DIAL,
	ICON_ID_ANALOG_CLOCK_HAND_HOUR,
	ICON_ID_ANALOG_CLOCK_HAND_MINUTE,
	//ui_heart_rate_measure
	ICON_ID_HEART_RATE_ICON,
	ICON_ID_HERAT_RATE_UNIT,
	//ui_blood_pressure_measure
	ICON_ID_BLOOD_PRESSURE_ICON,
	ICON_ID_BLOOD_PRESSURE_UNIT,
	//ui_notification
	ICON_ID_QQ,
	ICON_ID_WECHAT,
	ICON_ID_TEXT,
	ICON_ID_INCOMING_CALL,
	ICON_ID_OVAL_FRAME_03,
	ICON_ID_CIRCLE_01_BLUE,
	ICON_ID_CIRCLE_01_GRAY,
	//ui_charging
	ICON_ID_CHARGING_00,
	ICON_ID_CHARGING_01,
	ICON_ID_CHARGING_02,
	ICON_ID_CHARGING_03,
	ICON_ID_CHARGING_04,
	//ui_tire_setting
	ICON_ID_TIRE_SETTING_ICON,
	ICON_ID_TIRE_SETTING_NAME,
	//ui_ts_type_select
	ICON_ID_CIRCLE_02_BLUE,
	ICON_ID_CIRCLE_02_WHITE,
	/*tire select*/
	ICON_ID_TS_CIRCLE_BLUE,
	ICON_ID_TS_CIRCLE_DARK_GRAY,
	ICON_ID_TS_BACK_SELECT,
	ICON_ID_TS_BACK_DESELECT,
	ICON_ID_TS_OK_SELECT,
	ICON_ID_TS_OK_DESELECT,
	ICON_ID_TS_TIRE_BINDING_NAME,
	//ui_tp_motor
	ICON_ID_TP_MOTOR_BODY,
	ICON_ID_TP_MOTOR_TIRE_RED,
	ICON_ID_TP_MOTOR_TIRE_BLUE,
	ICON_ID_TP_MOTOR_TIRE_GRAY,
	//ui_tp_car ui_tp_suv
	ICON_ID_TP_CAR_BODY,
	ICON_ID_TP_SUV_BODY,
    ICON_ID_TP_CAR_SUV_H_TIRE_RED,
    ICON_ID_TP_CAR_SUV_H_TIRE_BLUE,
    ICON_ID_TP_CAR_SUV_H_TIRE_GRAY,
    ICON_ID_TP_CAR_SUV_V_TIRE_RED,
    ICON_ID_TP_CAR_SUV_V_TIRE_BLUE,
    ICON_ID_TP_CAR_SUV_V_TIRE_GRAY,
	ICON_ID_MAX,
}icon_id_t;

extern void draw_icon(const display_area_t *p_display_area, uint8_t icon_id);

#endif
