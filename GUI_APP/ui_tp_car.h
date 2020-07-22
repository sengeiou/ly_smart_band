#ifndef __UI_TP_CAR_H__
#define __UI_TP_CAR_H__

enum
{
	UITPCAR_DISPLAY_AREA_CAR_BODY = 0,
	UITPCAR_DISPLAY_AREA_TIRE_01,
	UITPCAR_DISPLAY_AREA_TIRE_01_P,//pressure
	UITPCAR_DISPLAY_AREA_TIRE_01_T,//temperature
	UITPCAR_DISPLAY_AREA_TIRE_02,
	UITPCAR_DISPLAY_AREA_TIRE_02_P,
	UITPCAR_DISPLAY_AREA_TIRE_02_T,
	UITPCAR_DISPLAY_AREA_TIRE_03,
	UITPCAR_DISPLAY_AREA_TIRE_03_P,
	UITPCAR_DISPLAY_AREA_TIRE_03_T,
	UITPCAR_DISPLAY_AREA_TIRE_04,
	UITPCAR_DISPLAY_AREA_TIRE_04_P,
	UITPCAR_DISPLAY_AREA_TIRE_04_T,
	UITPCAR_DISPLAY_AREA_MAX
};

extern void ui_tp_car_enter(void);
extern void ui_tp_car_exit(void);
extern void ui_tp_car_show(void);
extern void ui_tp_car_init(void);

#endif