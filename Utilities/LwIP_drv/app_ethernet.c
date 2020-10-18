/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/app_ethernet.c 
  * @author  MCD Application Team
  * @brief   Ethernet specefic module
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
#include "lwip/opt.h"
#include "main.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "app_ethernet.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "http_cgi_ssi.h"

struct netif gnetif; /* network interface structure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if LWIP_DHCP
#define MAX_DHCP_TRIES  4
__IO uint8_t DHCP_state = DHCP_OFF;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Notify the User about the nework interface config status 
  * @param  netif: the network interface
  * @retval None
  */
void ethernet_link_status_updated(struct netif *netif) 
{
  if (netif_is_up(netif))
 {
#if LWIP_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_START;
#else

#endif /* LWIP_DHCP */
  }
  else
  {  
#if LWIP_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_LINK_DOWN;
#else  
    
#endif /* LWIP_DHCP */
  } 
}

#if LWIP_DHCP
/**
  * @brief  DHCP Process
  * @param  argument: network interface
  * @retval None
  */
void DHCP_Thread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;
  
  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);    
        DHCP_state = DHCP_WAIT_ADDRESS;

        dhcp_start(netif);
      }
      break;    
    case DHCP_WAIT_ADDRESS:
      {                
        if (dhcp_supplied_address(netif)) 
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;	
        }
        else
        {
          dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    
          /* DHCP timeout */
          if (dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;
            
            /* Static address used */
            IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));

          }
        }
      }
      break;
  case DHCP_LINK_DOWN:
    {
      DHCP_state = DHCP_OFF;
    }
    break;
    default: break;
    }
    
    /* wait 500 ms */
    vTaskDelay(500);
  }
}
#endif  /* LWIP_DHCP */

  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{ 
#if LWIP_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* LWIP_DHCP */
  
  /* add the network interface */    
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  
  /*  Registers the default network interface. */
  netif_set_default(&gnetif);
  
  ethernet_link_status_updated(&gnetif); 
  
#if LWIP_NETIF_LINK_CALLBACK 
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);
  //osThreadDef(EthLink, ethernet_link_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE *2);
  //osThreadCreate (osThread(EthLink), &gnetif);
  
  xTaskCreate(ethernet_link_thread, (const char*)"EthLink", configMINIMAL_STACK_SIZE * 2, &gnetif, TCPIP_THREAD_PRIO, NULL);
  
#endif   
 
#if LWIP_DHCP
  /* Start DHCPClient */
  //osThreadDef(DHCP, DHCP_Thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  //osThreadCreate (osThread(DHCP), &gnetif);
  xTaskCreate(DHCP_Thread, (const char*)"DHCP", configMINIMAL_STACK_SIZE * 2, &gnetif, TCPIP_THREAD_PRIO, NULL);  
  
#endif 
}


void InitLwIP(void)
{
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);
  
  /* Initialize the LwIP stack */
  Netif_Config();
  
  /* Http webserver Init */
  http_server_init();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
