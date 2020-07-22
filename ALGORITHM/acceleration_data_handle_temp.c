#include <stdint.h>
#include <math.h>
#include "acceleration_data_handle_temp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "sleep_monitor.h"

uint32_t smooth_data1;

int16_t ls_p2v_time = 0;
int8_t v2p_time = 0;
int8_t ud_v2p_time_ok = 0;
int8_t wave_time = 0;
int32_t ls_valley = 0;
int32_t ls_peak = 0;
int32_t ctinu_wave = 0;
int8_t wave_raise_trend = 0;
int32_t ls_value = 0;
int16_t valley_update_time = 0;
int32_t ls_amp = 0;
uint32_t thr = 2500;
int32_t front_amp = 0;
int32_t rear_amp = 0;
uint32_t sum_amp = 0;
uint32_t steps = 0;
int8_t steps_flag = 0;
int8_t vut = 40;
uint32_t ave_peak_intv = 0;
uint16_t tmp_peak_intv = 0;
uint32_t sum_peak_intv = 0;
uint32_t data_max = 0;
uint32_t SMOOTH_WINDOWS_step1[2] = {0};
uint32_t SMOOTH_WINDOWS_step2[2] = {0};
uint32_t SMOOTH_WINDOWS_step3[2] = {0};
uint32_t SMOOTH_WINDOWS_step4[2] = {0};
uint32_t SMOOTH_WINDOWS_step5[2] = {0};

int16_t w_last_data = 0;
int16_t w_current_data = 0;
uint8_t w_continue_point = 0;
uint8_t w_turn_flag = 0;
int32_t w_max_data = 0;
uint32_t w_lift_the_wrist_flag = 0;
uint8_t z_Upward = 0;
uint32_t z_Upward_point = 0;

uint32_t step_index = 1;

uint32_t xcount = 0;
uint32_t ycount = 0;
uint32_t zcount = 0;

//睡眠算法
uint32_t u32TotalBodyMovement;

uint32_t s_BodyMovementTimes = 0;//十分钟内体动次数
uint8_t s_Deep_sleep = 0;//深睡
uint8_t s_Light_sleep = 0;//浅睡
uint8_t s_Awake = 0;//醒
uint32_t s_time = 0;//系统时间
uint8_t s_ctinu_time = 0;//半小时内手臂连续摆臂的时间
uint8_t s_all_time = 0;//半小时内手臂活动的总时间
uint8_t s_Number_of_arm_swing = 0;//半小时内手臂从静止到摆臂的次数
uint8_t s_ctinu_max_time = 0;//半小时内手臂连续摆臂的最大时间
uint8_t s_max_ctinu_wave = 0;//半小时内最大的连续步数

void vBodyMovementCheck(uint8_t *pu8WristStatus, uint32_t *pu32TenMinutesTotalMovement)
{
	if(u32TotalBodyMovement <= 5)
	{
		*pu8WristStatus = eWRIST_STATUS_DEEP_SLEEP;
	}
	else if(u32TotalBodyMovement <= 30 && u32TotalBodyMovement > 5)
	{
		*pu8WristStatus = eWRIST_STATUS_LIGHT_SLEEP;
	}
	else
	{
		*pu8WristStatus = eWRIST_STATUS_STEP;
	}

	*pu32TenMinutesTotalMovement = u32TotalBodyMovement;

	u32TotalBodyMovement = 0;	
}

void smooth(uint32_t index, uint32_t *windows, uint32_t data)
{
    uint8_t j = 0;
    uint32_t sum = 0;

    for (j = 0; j < 1; j++)
    {
        windows[j] = windows[j + 1];
    }
    windows[1] = data;
    for (j = 0; j < 2; j++)
    {
        sum = sum + windows[j];
    }

    if (index >= 2)
    {
        smooth_data1 = sum / 2;
    }
    else
    {
        smooth_data1 = sum / (index);
    }
}

void pedometer(uint8_t hour, uint8_t minute, int index, uint32_t data)
{
	//NRF_LOG_RAW_INFO("%d\r\n", hour);	
	uint32_t cur_data = 0;

	cur_data = data;
	if (data_max < cur_data)
     data_max = cur_data;
	if(z_Upward>0)
	{
		ctinu_wave = 0;
		//w_lift_the_wrist_flag = 0;
		z_Upward = 0;
	}
	//NRF_LOG_RAW_INFO("%d\r\n", hour);	
	/*****************睡眠分析*******************/
	
	 s_time = s_time + 1;
	
	//NRF_LOG_RAW_INFO("%d\r\n", s_time);	
	if(s_time>0&&s_time%15000 == 0)
	{
		//NRF_LOG_RAW_INFO("%d\r\n", s_all_time);	
		//NRF_LOG_RAW_INFO("%d\r\n", s_Number_of_arm_swing);	
		
		//睡眠模式
		 s_Awake = 0;
     s_Deep_sleep = 0;
     s_Light_sleep = 0;		
		 if (s_BodyMovementTimes <= 10)	//1 深睡模式 ||s_all_time<100
		 {
        s_Deep_sleep = 1;
			 // NRF_LOG_RAW_INFO("%d\r\n", 1);	
		 }
      
     if (s_BodyMovementTimes <= 30 && s_BodyMovementTimes > 10) //2 浅睡模式
		 {
        s_Light_sleep = 1;
			 // NRF_LOG_RAW_INFO("%d\r\n", 2);	
		 }
        
     if (s_BodyMovementTimes > 30) //0 觉醒||s_max_ctinu_wave>10
		 {
        s_Awake = 1;
			  //NRF_LOG_RAW_INFO("%d\r\n", 3);	
		 }
    //sleep_data_handle_result_get();    
		s_time = 0 ;// 计时清零
		s_ctinu_time = 0;//半小时内手臂连续摆臂的最大时间清零
		s_all_time = 0;//半小时内手臂活动总时间情清零
		s_Number_of_arm_swing = 0;//半小时内手臂从静止到摆臂的次数	
		s_ctinu_max_time = 0;//半小时内手臂连续摆臂的最大时间
		s_max_ctinu_wave = 0;//半小时内最大的连续步数
		s_BodyMovementTimes = 0;
		
		
	}
		
	if (ls_value > cur_data) 
	{
		if (wave_raise_trend == 1)
		{
			if (ls_value > ls_peak || ud_v2p_time_ok == 0)
			{
				ls_peak = ls_value;
				ud_v2p_time_ok = 1;
				v2p_time = v2p_time + wave_time;
				wave_time = 0;
				valley_update_time = 0;
			}
		}
		wave_raise_trend = 0;
		if(valley_update_time > vut)
		{
			ls_peak = ls_value;
			wave_time = 0;
			ls_p2v_time = 0;
			valley_update_time = 0;
		}
	}
	else if(ls_value < cur_data)
	{
		if (wave_raise_trend == 0)
		{			
			front_amp = data_max - ls_valley;
			rear_amp = data_max - ls_value;
			tmp_peak_intv = v2p_time + wave_time;
			//log_print("%s\n","CCCCCCCCCCC");
			//log_print("%8d%8d\n",index,tmp_peak_intv);
			//log_print("%s\n","CCCCCCCCCCC");
			if (tmp_peak_intv >= 35)
			{
				thr = 2500;
				sum_amp = 0;	
        data_max = 0;				
				//log_print("%s\n","AAAAAAAAAAAAAAA");
				//log_print("%8d%8d\n",ctinu_wave,tmp_peak_intv);
				//log_print("%s\n","BBBBBBBBBBBBBBB");
				s_ctinu_time = 0;				
				ctinu_wave = 0;
				//sum_peak_intv = 0;
        //ave_peak_intv = 0;
				ud_v2p_time_ok = 0;
				v2p_time = 0;
				ls_p2v_time = wave_time;
				wave_time = 0;
				ls_valley = ls_value;
			}
			if ((rear_amp > thr )||(front_amp > thr ))
			{
				if (tmp_peak_intv > 7)
				{
					if (ctinu_wave > 0)
			    {
				    thr = (0.4*sum_amp)/ctinu_wave;
				    if(thr < 2500)
				    {
					    thr = 2500;
				    }
			    }
			  else
			  {
				  thr = 2500;
			  }
				
					if (ctinu_wave>0)
            ave_peak_intv = sum_peak_intv/ctinu_wave;
               
					if (front_amp > rear_amp)
					{
						sum_amp = sum_amp + front_amp;
					}
					else
					{
						sum_amp = sum_amp + rear_amp;
					}
					//number = number + 1;
					data_max = 0;
					ctinu_wave = ctinu_wave + 1;
					s_BodyMovementTimes = s_BodyMovementTimes + 1;
					u32TotalBodyMovement++;
					
					/***********记录每半个小时内连续摆臂的最大时间和摆臂的总时间*************/
					if(ctinu_wave == 2)
					{
						s_Number_of_arm_swing = s_Number_of_arm_swing + 1;
						//NRF_LOG_RAW_INFO("%d\r\n", s_Number_of_arm_swing);		
					}
					s_all_time = s_all_time + tmp_peak_intv;
					s_ctinu_time = s_ctinu_time + tmp_peak_intv;
					if(s_ctinu_max_time<s_ctinu_time)
						s_ctinu_max_time = s_ctinu_time;
					if(s_max_ctinu_wave<ctinu_wave)
						s_max_ctinu_wave = ctinu_wave;
					/***********记录每半个小时内连续摆臂的最大时间和摆臂的总时间*************/
					//log_print("%8d%8d\n",ctinu_wave,index);
					sum_peak_intv = sum_peak_intv + tmp_peak_intv;
					if (ctinu_wave == 10)
					{
						steps = steps + 9;
					}
					if (ctinu_wave > 10  )
					{
						steps = steps + 1;
					}
					if (tmp_peak_intv>=1.5*ave_peak_intv&&ave_peak_intv>0)
					{
						 if (ctinu_wave >=10)
									steps = steps + 1;
						 else
						 {
								 //ctinu_wave = ctinu_wave + 1;
								 if (ctinu_wave == 10)
										 steps = steps + 9;
						}
					}
					//log_print("%8d%8d%8d\n",index,ctinu_wave,steps);
					ud_v2p_time_ok = 0;
					v2p_time = 0;
					ls_p2v_time = wave_time;
					wave_time = 0;
					ls_valley = ls_value;
				}
				valley_update_time = 0;
			}
			if (ls_value < ls_valley)
			{
				ls_valley = ls_value;
				v2p_time = 0;
				//wave_time = 0;
        //ls_p2v_time = 0;
				valley_update_time = 0;
				if (front_amp <= thr)
				{
					ud_v2p_time_ok = 0;
				}
			}
	  }
		wave_raise_trend = 1;
		if (valley_update_time > vut)
		{
			ls_valley = ls_value;
			v2p_time = 0;
			//thr = 2200;
			valley_update_time = 0;
		}
	}
	else
	{
		if (valley_update_time > vut)
		{
			ls_valley = ls_value;
			ls_peak = ls_value;
			wave_time = 0;
			v2p_time = 0;
			valley_update_time = 0;
			//thr = 2200;
		}
	}
	valley_update_time = valley_update_time + 1;
	ls_value = cur_data;
	wave_time = wave_time + 1;
}

void wrist_lift_up_or_lift_down(int16_t z_data)
{
    //watch_evt_t watch_evt;
    w_last_data = w_current_data;
    w_current_data = -z_data;
    w_lift_the_wrist_flag = 0;

    if(w_current_data > 14000)
        z_Upward_point = z_Upward_point + 1;
    else
    {
        z_Upward_point = 0;
        z_Upward = 0;
    }
    if(z_Upward_point > 5)
    {
        z_Upward = 1;
        z_Upward_point = 0;
    }


    if (w_turn_flag == 1 && w_current_data > 1000)
    {
        w_continue_point = w_continue_point + 1;
        if (w_max_data < w_current_data)
        {
            w_max_data = w_current_data;
        }
    }
    else
    {
        w_turn_flag = 0;
        w_continue_point = 0;
        w_max_data = 0;
    }

    if (w_last_data < 1000 && w_current_data > 1000)
    {
        w_turn_flag = 1;
        w_max_data = 0;
    }

    if (w_continue_point > 20 && w_max_data > 8000)
    {
        w_lift_the_wrist_flag = 1;
        w_turn_flag = 0;
        w_continue_point = 0;
        w_max_data = 0;
    }
}

void acceleartion_data_handle(uint8_t hour, uint8_t minute, int16_t x_axis, int16_t y_axis, int16_t z_axis)
{
    int32_t resultant_acceleration;
    resultant_acceleration = sqrt(pow(x_axis, 2) + pow(y_axis, 2) + pow(z_axis, 2));
    wrist_lift_up_or_lift_down(z_axis);
    smooth(step_index, SMOOTH_WINDOWS_step1, resultant_acceleration);
    smooth(step_index, SMOOTH_WINDOWS_step2, smooth_data1);
    smooth(step_index, SMOOTH_WINDOWS_step3, smooth_data1);
    smooth(step_index, SMOOTH_WINDOWS_step4, smooth_data1);
    smooth(step_index, SMOOTH_WINDOWS_step5, smooth_data1);
    pedometer(hour, minute, step_index, smooth_data1);
    step_index++;
	
	if(x_axis>14000 || x_axis <-14000)
        xcount = xcount + 1;
    else
        xcount = 0;
  
    if(y_axis>14000 ||y_axis <-14000)
        ycount = ycount + 1;
    else
        ycount = 0;
    
    if(z_axis>14000 || z_axis <-14000)
        zcount = zcount + 1;
    else
        zcount = 0;
		//NRF_LOG_RAW_INFO("xcount = %d,ycount = %d, zcount = %d\r\n", xcount, ycount, zcount);
	
#ifdef ENABLE_WRIST_LIFT_CHECK
    //wrist_lift_up_or_lift_down(z_axis);
#endif
}

uint32_t acceleartion_data_handle_result_get(void)
{
    return steps;
}

void acceleration_data_handle_clear(void)
{
    steps = 0;
}

uint8_t sleep_data_handle_result_get(void)
{
	if(s_Deep_sleep == 1)
	{
		return eWRIST_STATUS_DEEP_SLEEP;
	}

	if(s_Light_sleep == 1)
	{
		return eWRIST_STATUS_LIGHT_SLEEP;
	}

	return eWRIST_STATUS_STEP;
}

uint8_t lift_the_wrist_result_get(void)
{
    return w_lift_the_wrist_flag;
}
