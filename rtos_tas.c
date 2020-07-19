#include "rtos_tas.h"
#include "adc/adc.h"
#include "keypad/keypad.h"
#include "uart/uart.h"
#include "lcd/lcd.h"

//states definitions
typedef enum
{
	TAS_MAIN,
	TAS_ALARM,
	TAS_THRESHOLD_CONFIG_KEY,
	TAS_THRESHOLD_CONFIG_TERM,
	TAS_ALARM_COUNT,
	TAS_NONE
}TAS_States;

TAS_States TAS_CURRENT = TAS_MAIN;
TAS_States TAS_PREV = TAS_NONE;

//display strings
uint8 MAIN_DISP_STR_1[] = "C:    T:    AA: ";
uint8 MAIN_DISP_STR_2[] = "K:15  T:G  AM:13";

uint8 ALRM_DISP_STR_1[] = "    ALARMING!   ";
uint8 ALRM_DISP_STR_2[] = "  KAC:12 TAC:S  ";

uint8 KCON_DISP_STR_1[] = "      T:        ";
uint8 KCON_DISP_STR_2[] = "   OK:15 CN:12  ";

uint8 TCON_DISP_STR_1[] = "      T:        ";
uint8 TCON_DISP_STR_2[] = "   OK:O  CN:C   ";


//initialize variables
uint8 c_temp = 0;
uint8 th_temp = 120;
uint8 alarm_mode = 1;
uint8 uart_choice = 0;
uint8 key_choice = 0;
uint8 ind = 0;

void rtos_tas_run()
{
	vTaskStartScheduler();
}

void rtos_tas_init()
{
	//enable global interrupt
	setBit(SREG, 7);
	//buzzer pin init
	setBit(DDRE, 7);
	//initialize adc
	adc_init();
	//initialize keypad
	keypad_init();
	//initialize uart
	uart_init(0);
	//initialize lcd
	lcd_init();
	//create tasks
	xTaskCreate(t_tas_alarm, "t_tas_alarm", 100, NULL, 6, NULL);
	xTaskCreate(t_tas_check, "t_tas_check", 100, NULL, 5, NULL);
	xTaskCreate(t_tas_key_config, "t_tas_key_config", 100, NULL, 4, NULL);
	xTaskCreate(t_tas_uart_config, "t_tas_uart_config", 100, NULL, 3, NULL);
	xTaskCreate(t_tas_temp_update, "t_tas_temp_update", 100, NULL, 2, NULL);
	xTaskCreate(t_tas_disp, "t_tas_disp", 100, NULL, 1, NULL);
	//create event group
	eg_tas = xEventGroupCreate();
}

void t_tas_alarm(void* ptr)
{
	while(1)
	{
		eb_vals = xEventGroupWaitBits(eg_tas,					
									  E_ALRM_ACTV,
									  1,
									  0,
									  portMAX_DELAY);
		//check for alarm event
		if(eb_vals & E_ALRM_ACTV)
		{
			if(TAS_PREV != TAS_ALARM)
			{
				TAS_PREV = TAS_ALARM;
			}
			//alarm sequence
			setBit(PORTE, 7);
			vTaskDelay(250);
			clearBit(PORTE, 7);
			vTaskDelay(250);
		}
		
		if(c_temp < th_temp)
		{
			TAS_CURRENT = TAS_MAIN;
		}
		vTaskDelay(100);
	}
}

void t_tas_temp_update(void* ptr)
{
	uint8 c_temp_prev = 255;
	while(1)
	{
		c_temp = (adc_get_data(0) / 1024.0) * 200;
		if(c_temp != c_temp_prev)
		{
			xEventGroupSetBits(eg_tas, E_TEMP_DISP);
		}
		c_temp_prev = c_temp;
		vTaskDelay(100);
	}
}

void t_tas_check(void* ptr)
{
	while(1)
	{
		if(c_temp >= th_temp && alarm_mode == 1) //alarm state
		{
			//system state change
			TAS_CURRENT = TAS_ALARM;
			//raise alarm event
			xEventGroupSetBits(eg_tas, E_ALRM_ACTV);
			//raise alarm disp event
			xEventGroupSetBits(eg_tas, E_ALRM_DISP);
		}
		else                       //main state
		{
			//change system state
			if(TAS_CURRENT == TAS_MAIN)
			{
				if(TAS_PREV != TAS_MAIN)
				{
					TAS_PREV = TAS_MAIN;
					//raise main disp event
					xEventGroupSetBits(eg_tas, E_MAIN_DISP);
				}
			}
			
		}
		vTaskDelay(50);
	}
}

void t_tas_key_config(void* ptr)
{
	uint8 key_config_arr[3] = {0};
	uint8 i = 0;
	uint8 th_temp_check = 0;
	while(1)
	{	
		key_choice = keypad_getKey();
		if(key_choice == 13 && TAS_CURRENT == TAS_MAIN)		//alarm toggling 
		{
			rtos_tas_toggle_am();
		}
		else if(key_choice == 12 && TAS_CURRENT == TAS_ALARM) //shutting alarm
		{
			alarm_mode = 0;
			TAS_CURRENT = TAS_MAIN;
		}
		else if(key_choice == 15 && TAS_CURRENT == TAS_MAIN)
		{
			TAS_CURRENT = TAS_THRESHOLD_CONFIG_KEY;
			if(TAS_PREV != TAS_THRESHOLD_CONFIG_KEY)
			{
				TAS_PREV = TAS_THRESHOLD_CONFIG_KEY;
				//raise event for edit display
				xEventGroupSetBits(eg_tas, E_KCON_DISP);
				ind = 0;
			}
		}
		else if(key_choice == 12 && TAS_CURRENT == TAS_THRESHOLD_CONFIG_KEY)
		{
			//cancel editing and return to main state
			TAS_CURRENT = TAS_MAIN;
		}
		else if(key_choice == 15 && TAS_CURRENT == TAS_THRESHOLD_CONFIG_KEY)
		{
			//confirm data in global variable
			th_temp = th_temp_check;
			//return to main state
			TAS_CURRENT = TAS_MAIN;
		}
		else if(key_choice >= 1 && key_choice <= 10 && ind < 3 && TAS_CURRENT == TAS_THRESHOLD_CONFIG_KEY)
		{
			if(key_choice == 10)
			{
				key_choice = 0;
			}
			key_config_arr[ind] = key_choice;
			ind++;
			//raise event for threshold edit
			xEventGroupSetBits(eg_tas, E_THRE_DISP);
			//get data from array
			th_temp_check = 0;
			for(i = 0; i < ind; i++)
			{
				th_temp_check = (th_temp_check * 10) + key_config_arr[i];
			}
		}
		vTaskDelay(50);
	}
}

void t_tas_uart_config(void* ptr)
{
	uint8 uart_config_arr[3] = {0};
	uint8 i = 0;
	uint8 th_temp_check = 0;
	while(1)
	{	
			uart_choice = uart_rece_byte(0);
			
			if(uart_choice == 'S' && TAS_CURRENT == TAS_ALARM)
			{
				alarm_mode = 0;
				TAS_CURRENT = TAS_MAIN;
			}
			else if(uart_choice == 'G' && TAS_CURRENT == TAS_MAIN)
			{
				TAS_CURRENT = TAS_THRESHOLD_CONFIG_TERM;
				if(TAS_PREV != TAS_THRESHOLD_CONFIG_TERM)
				{
					TAS_PREV = TAS_THRESHOLD_CONFIG_TERM;
					//raise event for threshold terminal display
					xEventGroupSetBits(eg_tas, E_TCON_DISP);
					ind = 0;
				}
			}
			else if(uart_choice == 'C' && TAS_CURRENT == TAS_THRESHOLD_CONFIG_TERM)
			{
				//cancel editing and return to main
				TAS_CURRENT = TAS_MAIN;
			}
			else if(uart_choice == 'O' && TAS_CURRENT == TAS_THRESHOLD_CONFIG_TERM)
			{
				//confirm threshold into global variable
				th_temp = th_temp_check;
				//return to main state
				TAS_CURRENT = TAS_MAIN;
			}
			else if(uart_choice >= '0' && uart_choice <= '9' && TAS_CURRENT == TAS_THRESHOLD_CONFIG_TERM)
			{
				uart_config_arr[ind] = uart_choice - '0';
				ind++;
				//raise event for threshold edit
				xEventGroupSetBits(eg_tas, E_THRE_DISP);
				//get data from array
				th_temp_check = 0;
				for(i = 0; i < ind; i++)
				{
					th_temp_check = (th_temp_check * 10) + uart_config_arr[i];
				}
			}
			vTaskDelay(50);
	}
}

void t_tas_disp(void* ptr)
{
	while(1)
	{
		eb_vals = xEventGroupWaitBits(eg_tas,
		(E_MAIN_DISP | E_KCON_DISP | E_TCON_DISP | E_ALRM_DISP | E_TEMP_DISP | E_THRE_DISP | E_AMOD_DISP),
		1,
		0,
		portMAX_DELAY);
		
		//check which display event has happened
		if(eb_vals & E_MAIN_DISP)
		{
			//display main state
			lcd_send_cmd(LCD_CLR);
			vTaskDelay(2);	
			lcd_send_str( MAIN_DISP_STR_1, sizeof(MAIN_DISP_STR_1) );
			lcd_goto_xy(0,1);
			lcd_send_str( MAIN_DISP_STR_2, sizeof(MAIN_DISP_STR_2) );
			//display initial temp on main state
			lcd_goto_xy(TAS_TEMP_EDIT, TAS_LINE_EDIT);
			lcd_send_itoa(c_temp);
			//display initial threshold on main state
			lcd_goto_xy(TAS_THRE_EDIT, TAS_LINE_EDIT);
			lcd_send_itoa(th_temp);
			//display initial alarm mode on main state
			lcd_goto_xy(TAS_AM_EDIT, TAS_LINE_EDIT);
			if(alarm_mode == 1)
			{
				lcd_send_char('Y');
			}
			else if(alarm_mode == 0)
			{
				lcd_send_char('N');
			}
		}
		else if(eb_vals & E_ALRM_DISP)
		{
			//display alarm state
			lcd_send_cmd(LCD_CLR);
			_delay_ms(2);
			lcd_send_str( ALRM_DISP_STR_1, sizeof(ALRM_DISP_STR_1) );
			lcd_goto_xy(0,1);
			lcd_send_str( ALRM_DISP_STR_2, sizeof(ALRM_DISP_STR_2) );
		}
		else if( (eb_vals & E_TEMP_DISP) && TAS_CURRENT == TAS_MAIN )
		{
			//update temp on main state
			lcd_goto_xy(TAS_TEMP_EDIT, TAS_LINE_EDIT);
			lcd_send_itoa( (uint8)c_temp );
		}
		else if( (eb_vals & E_AMOD_DISP) && TAS_CURRENT == TAS_MAIN )
		{
			//update alarm mode on main state
			lcd_goto_xy(TAS_AM_EDIT, TAS_LINE_EDIT);
			if(alarm_mode == 1)
			{
				lcd_send_char('Y');
			}
			else if(alarm_mode == 0)
			{
				lcd_send_char('N');
			}
		}
		else if( (eb_vals & E_KCON_DISP) && (TAS_CURRENT == TAS_THRESHOLD_CONFIG_KEY) )
		{
			//display key config state
			lcd_send_cmd(LCD_CLR);
			vTaskDelay(2);
			lcd_send_str( KCON_DISP_STR_1, sizeof(KCON_DISP_STR_1) );
			lcd_goto_xy(0,1);
			lcd_send_str( KCON_DISP_STR_2, sizeof(KCON_DISP_STR_2) );
		}
		else if( (eb_vals & E_TCON_DISP) && (TAS_CURRENT == TAS_THRESHOLD_CONFIG_TERM) )
		{
			//display uart config state
			lcd_send_cmd(LCD_CLR);
			vTaskDelay(2);
			lcd_send_str( TCON_DISP_STR_1, sizeof(TCON_DISP_STR_1) );
			lcd_goto_xy(0,1);
			lcd_send_str( TCON_DISP_STR_2, sizeof(TCON_DISP_STR_2) );
		}
		else if(eb_vals & E_THRE_DISP)
		{
			lcd_goto_xy(TAS_THRE_EDIT + ind, TAS_LINE_EDIT);
			if(TAS_CURRENT == TAS_THRESHOLD_CONFIG_KEY)
				lcd_send_char(key_choice + '0');
			else if(TAS_CURRENT == TAS_THRESHOLD_CONFIG_TERM)
				lcd_send_char(uart_choice);
		}
	}
}

void rtos_tas_toggle_am()
{
	alarm_mode = !alarm_mode;
	xEventGroupSetBits(eg_tas, E_AMOD_DISP);
}