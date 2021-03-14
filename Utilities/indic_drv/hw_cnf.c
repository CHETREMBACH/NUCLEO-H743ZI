/**
  ******************************************************************************
  * @file    hw_cnf.c
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    28-02-2021
  * @brief   Файл содержит функции аппаратных ресурсов индикации 
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "hw_cnf.h"
#include "led_7seg.h"

//  DP G F E D C B A
//  7  6 5 4 3 2 1 0
#define SEG_A (0x01)
#define SEG_B (0x02)
#define SEG_C (0x04)
#define SEG_D (0x08)
#define SEG_E (0x10)
#define SEG_F (0x20)
#define SEG_G (0x40)
#define SEG_P (0x80)	

/**
  * @brief  Write the charater Indic.
  * @param  dgt the character that has to be displayed 
  * @retval None
  */
void Hw_Write_Sym(uint8_t dgt)
{
	if ((dgt & SEG_A) > 0) {  A_HI; } else {  A_LO;};
	if ((dgt & SEG_B) > 0) {  B_HI; } else {  B_LO;};
	if ((dgt & SEG_C) > 0) {  C_HI; } else {  C_LO;};	
	if ((dgt & SEG_D) > 0) {  D_HI; } else {  D_LO;};	
	if ((dgt & SEG_E) > 0) {  E_HI; } else {  E_LO;};	
	if ((dgt & SEG_F) > 0) {  F_HI; } else {  F_LO;};	
	if ((dgt & SEG_G) > 0) {  G_HI; } else {  G_LO;};	
	if ((dgt & SEG_P) > 0) { DP_HI; } else { DP_LO;}
	;		
}	

/**
  * @brief  Configures hardware Indic.
  * @param  None 
  * @retval None
  */
void Hw_Indic_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;	
		
	I1_CLK_ENABLE();
	GPIO_InitStruct.Pin       = I1_PIN;
	HAL_GPIO_Init(I1_PORT, &GPIO_InitStruct);
	I1_HI;
	
	I2_CLK_ENABLE();
	GPIO_InitStruct.Pin       = I2_PIN;
	HAL_GPIO_Init(I2_PORT, &GPIO_InitStruct);
	I2_HI;
	
	I3_CLK_ENABLE();
	GPIO_InitStruct.Pin       = I3_PIN;
	HAL_GPIO_Init(I3_PORT, &GPIO_InitStruct);
	I3_HI;
	
	I4_CLK_ENABLE();
	GPIO_InitStruct.Pin       = I4_PIN;
	HAL_GPIO_Init(I4_PORT, &GPIO_InitStruct);
	I4_HI;
	
	A_CLK_ENABLE();
	GPIO_InitStruct.Pin       = A_PIN;
	HAL_GPIO_Init(A_PORT, &GPIO_InitStruct);
	A_HI;
	
	B_CLK_ENABLE();
	GPIO_InitStruct.Pin       = B_PIN;
	HAL_GPIO_Init(B_PORT, &GPIO_InitStruct);
	B_HI;	
	
	C_CLK_ENABLE();
	GPIO_InitStruct.Pin       = C_PIN;
	HAL_GPIO_Init(C_PORT, &GPIO_InitStruct);
	C_HI;	
	
	D_CLK_ENABLE();
	GPIO_InitStruct.Pin       = D_PIN;
	HAL_GPIO_Init(D_PORT, &GPIO_InitStruct);
	D_HI;	

	E_CLK_ENABLE();
	GPIO_InitStruct.Pin       = E_PIN;
	HAL_GPIO_Init(E_PORT, &GPIO_InitStruct);
	E_HI;	
	
	F_CLK_ENABLE();
	GPIO_InitStruct.Pin       = F_PIN;
	HAL_GPIO_Init(F_PORT, &GPIO_InitStruct);
	F_HI;	
	
	G_CLK_ENABLE();
	GPIO_InitStruct.Pin       = G_PIN;
	HAL_GPIO_Init(G_PORT, &GPIO_InitStruct);
	G_HI;	
	
	DP_CLK_ENABLE();
	GPIO_InitStruct.Pin       = DP_PIN;
	HAL_GPIO_Init(DP_PORT, &GPIO_InitStruct);
	DP_HI;	
}


/*
 *  HW Functions
 */

/**
 * \brief Clean the display
 *
 * This is the procedure that clean the display in HW mode.
 * you have to write it according with your hardware and micro.
 */
void sseg_off(void)
{
	/* Set the display OFF */
#ifdef CONFIG_LED_7SEG_CCAT
	/* Common Cathode */
	I1_LO; I2_LO; I3_LO; I4_LO;
#else
	/* Common Anode */
	I1_HI; I2_HI; I3_HI; I4_HI;	
#endif
}

/**
 * \brief writes the character to the single digit of the display
 *
 * This is the procedure that writes the character to the single digit
 * of the display, you have to write it according with your hardware and micro.
 *
 * 	\param dgt the character that has to be displayed
 * 	\param n_dgt the digit where to disply the character of the display's digits.
 */
void sseg_on(uint8_t dgt, uint8_t n_dgt)
{
	switch (n_dgt)
	{		
# ifdef CONFIG_LED_7SEG_CCAT
		//Common Cathode	
		case 0 :
			I2_LO; I3_LO; I4_LO;
		I1_HI; 		
		break;
	case 1 :
		I1_LO; I3_LO; I4_LO;
		I2_HI; 	
		break;
	case 2 :
		I1_LO; I2_LO; I4_LO;
		I3_HI; 	
		break;
	case 3 :
		I1_LO; I2_LO; I3_LO;
		I4_HI; 	
		break;
# else
		//Common Anode		
		case 0 :
			I2_HI; I3_HI; I4_HI;
		I1_LO; 		
		break;
	case 1 :
		I1_HI; I3_HI; I4_HI;
		I2_LO; 	
		break;
	case 2 :
		I1_HI; I2_HI; I4_HI;
		I3_LO; 	
		break;
	case 3 :
		I1_HI; I2_HI; I3_HI;
		I4_LO; 	
		break;
			
# endif
	}
	//Write the charater
	Hw_Write_Sym(dgt);
}

/**
 * \brief initalize the HW regsiters
 *
 * This is the procedure that initalize the HW regsiters.
 * you have to write it according with your hardware and micro.
 */
void sseg_init(void)
{
	/* Initialize PIN Direction to OUTPUT*/
	Hw_Indic_Init();
	/* Set the display OFF */
	sseg_off();
}

/************************ (C) COPYRIGHT ONE TO ONE *****END OF FILE****/