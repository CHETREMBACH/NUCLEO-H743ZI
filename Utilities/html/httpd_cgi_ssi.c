/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Raw/Src/httpd_cg_ssi.c
  * @author  MCD Application Team
  * @brief   Webserver SSI and CGI handlers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/debug.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "http_cgi_ssi.h"

#include "printf_dbg.h"

#include <string.h>
#include <stdlib.h>

/* we will use character "t" as tag for CGI */
char const* TAGCHAR="t";
char const** TAGS=&TAGCHAR;

u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen);

/* CGI handler for LED control */
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
void httpd_ssi_init(void);
void httpd_cgi_init(void);

/* Html request for "/leds.cgi" will start LEDS_CGI_Handler */
const tCGI LEDS_CGI={"/leds.cgi", LEDS_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[1];

/**
  * @brief  ADC_Handler : SSI handler for ADC page
  */
u16_t Time_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
	/* We have only one SSI handler iIndex = 0 */
	if (iIndex == 0)
	{
		char Digit1 = 0, Digit2 = 0, Digit3 = 0, Digit4 = 0;
		uint32_t TimeVal = 0;


		TimeVal = SysWorkTime.all_second; 
		/* get digits to display */
		Digit1 = TimeVal / 1000;
		Digit2 = (TimeVal - (Digit1 * 1000)) / 100;
		Digit3 = (TimeVal - ((Digit1 * 1000) + (Digit2 * 100))) / 10;
		Digit4 = TimeVal - ((Digit1 * 1000) + (Digit2 * 100) + (Digit3 * 10));

		/* prepare data to be inserted in html */
		*pcInsert       = (char)(Digit1 + 0x30);
		*(pcInsert + 1) = (char)(Digit2 + 0x30);
		*(pcInsert + 2) = (char)(Digit3 + 0x30);
		*(pcInsert + 3) = (char)(Digit4 + 0x30);

		/* 4 characters need to be inserted in html*/
		return 4;
	}
	return 0;
}

/**
  * @brief  CGI handler for LEDs control
  */
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	uint32_t i = 0;

	/* We have only one SSI handler iIndex = 0 */
	if (iIndex == 0)
	{
		/* Check cgi parameter : application GET /leds.cgi?led=2&led=4 */
		for (i = 0; i < iNumParams; i++)
		{
			/* check parameter "led" */
			if (strcmp(pcParam[i], "led") == 0)
			{
				/* Switch LED1 ON if 1 */
				if (strcmp(pcValue[i], "1") == 0)
				{
					printf("LED1_ON\n");
				}
				else
				{
					printf("LED1_OFF\n");
				}

				/* Switch LED2 ON if 2 */
				if (strcmp(pcValue[i], "2") == 0)
				{
					printf("LED2_ON\n");
				}
				else
				{
					printf("LED2_OFF\n");
				}
				/* Switch LED3 ON if 3 */
				if (strcmp(pcValue[i], "3") == 0)
				{
					printf("LED3_ON\n");
				}
				else
				{
					printf("LED3_OFF\n");
				}

				/* Switch LED4 ON if 4 */
				if (strcmp(pcValue[i], "4") == 0)
				{
					printf("LED4_ON\n");
				}
				else
				{
					printf("LED4_OFF\n");
				}
			}
		}
	}
	/* uri to send after cgi call*/
	return "/STM32H7xxLED.html";
}

/**
  * @brief  Http webserver Init
  */
void http_server_init(void)
{
	/* Httpd Init */
	httpd_init();

	/* configure SSI handlers (ADC page SSI) */
	http_set_ssi_handler(Time_Handler, (char const **)TAGS, 1);

	/* configure CGI handlers (LEDs control CGI) */
	CGI_TAB[0] = LEDS_CGI;
	http_set_cgi_handlers(CGI_TAB, 1);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
