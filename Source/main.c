#include "config.h"

#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_emac.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_clkpwr.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/tcpip.h"

/* lwIP netif includes */
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip/dns.h"

#include "gpio.h"
#include "i2c.h"
#include "uart.h"
#include "cli.h"
#include "spi.h"
#include "xr77192.h"

extern int32_t write_PHY(uint16_t bPhyAddr, uint32_t PhyReg, uint16_t Value);
extern int32_t read_PHY (uint16_t bPhyAddr, uint32_t PhyReg);

#define _EMAC
#define DEBUGSTR(...) taskENTER_CRITICAL(); \
					  printf(__VA_ARGS__); \
					  taskEXIT_CRITICAL()

extern err_t ethernetif_init(struct netif *xNetIf);
extern int32_t EMAC_CheckPHYStatus(uint32_t ulPHYState);
extern uint16_t EMAC_Read_PHY (uint8_t PhyReg);
extern void http_server_netconn_init(void);

static struct netif net_iface;
int init_done=0;
int CLOCK_init_done=0;

static void iface_callback(struct netif *iface)
{
	char ip[16] = {0}, gw[16] = {0}, mask[16] = {0};
	strcpy(ip, inet_ntoa(iface->ip_addr));
	strcpy(gw, inet_ntoa(iface->gw));
	strcpy(mask, inet_ntoa(iface->netmask));

	printf("Network interface is up, ip = %s, gw = %s, mask = %s\n", ip, gw, mask);
}

static const uint8_t EMAC_clkdiv[] =  { 4, 6, 8, 10, 14, 20, 28, 36, 40, 44, 48, 52, 56, 60, 64 };

void phy_init(void)
{
	EMAC_CFG_Type Emac_Config;
	/* Initialize the EMAC Ethernet controller. */
	int32_t regv,tout, tmp;

	// init pins
	PINSEL_ConfigPin(1, 0, 1);
	PINSEL_ConfigPin(1, 1, 1);
	PINSEL_ConfigPin(1, 4, 1);
	PINSEL_ConfigPin(1, 8, 1);
	PINSEL_ConfigPin(1, 9, 1);
	PINSEL_ConfigPin(1, 10, 1);
	PINSEL_ConfigPin(1, 14, 1);
	PINSEL_ConfigPin(1, 15, 1);
	PINSEL_ConfigPin(1, 16, 1);
	PINSEL_ConfigPin(1, 17, 1);

//	PINSEL_ConfigPin(1, 0, 1); // txd0
//	PINSEL_ConfigPin(1, 1, 1); // rxd1
////	PINSEL_ConfigPin(1, 2, 1); // txd2
////	PINSEL_ConfigPin(1, 3, 1); // txd3
//
//	PINSEL_ConfigPin(1, 4, 1); // tx_en
//	PINSEL_ConfigPin(1, 6, 1); // tx_clk
//	PINSEL_ConfigPin(1, 7, 1); // col
//	PINSEL_ConfigPin(1, 8, 1); // crs
//
//	PINSEL_ConfigPin(1, 9, 1); // rxd0
//	PINSEL_ConfigPin(1, 10, 1); // rxd1
////	PINSEL_ConfigPin(1, 11, 1); // rxd2
////	PINSEL_ConfigPin(1, 12, 1); // rxd3
////	PINSEL_ConfigPin(1, 13, 1); // rx_dv
//
////	PINSEL_ConfigPin(1, 14, 1); // rx_err
////	PINSEL_ConfigPin(1, 15, 1); // rx_clk
//	PINSEL_ConfigPin(1, 16, 1); // mdc
//	PINSEL_ConfigPin(1, 17, 1); // mdio

	Emac_Config.PhyCfg.Mode = EMAC_MODE_AUTO;
	EMAC_Init( &Emac_Config );
}

//int32_t read_PHY (uint16_t bPhyAddr, uint32_t PhyReg)
//{
//	/* Read a PHY register 'PhyReg'. */
//	uint32_t tout;
//
//	LPC_EMAC->MADR = ((bPhyAddr & 0x1F) << 8) | (PhyReg & 0x1F);
//	LPC_EMAC->MCMD = EMAC_MCMD_READ;
//
//	/* Wait until operation completed */
//	tout = 0;
//	for (tout = 0; tout < EMAC_MII_RD_TOUT; tout++) {
//		if ((LPC_EMAC->MIND & EMAC_MIND_BUSY) == 0) {
//			LPC_EMAC->MCMD = 0;
//			return (LPC_EMAC->MRDD);
//		}
//	}
//	// Time out!
//	return (-1);
//}
//
//int32_t write_PHY(uint16_t bPhyAddr, uint32_t PhyReg, uint16_t Value)
//{
//	/* Write a data 'Value' to PHY register 'PhyReg'. */
//	uint32_t tout;
//
//	LPC_EMAC->MADR = ((bPhyAddr & 0x1F) << 8 ) | (PhyReg & 0x1F);
//	LPC_EMAC->MWTD = Value;
//
//	/* Wait until operation completed */
//	tout = 0;
//	for (tout = 0; tout < EMAC_MII_WR_TOUT; tout++) {
//		if ((LPC_EMAC->MIND & EMAC_MIND_BUSY) == 0) {
//			return (0);
//		}
//	}
//	// Time out!
//	return (-1);
//}

void configure_phy(void)
{
//	uint32_t val = read_PHY(21, 0x00);
//	printf("Link [%d] = %d\r\n", 21, (val >> 11) & 0x01);
//	printf("CMode [%d] = %d\r\n", 21, val & 0b1111);
//	printf("==============\r\n");

	printf("Configuring PHY\r\n");

//	force link down
	uint16_t ret = read_PHY(21, 0x01);
	printf("Link forced value %d\r\n", ret);
	ret &= ~(1 << 5); // bit 4 = 1 forced link down
	ret |= (1 << 4); // bit 5 = 1 enable @up setting
	write_PHY(21, 0x01, ret);

	delay_ms(10);

	ret = read_PHY(21, 0x01);

	// enable 100mbs
	ret |= (1 << 0);
	ret &= ~(1 << 1);

//	flow control value
	ret |= (1 << 7);

//	flow control enable
	ret |= (1 << 6);

	// duplex = half
//	ret &= ~(1 << 3);

	// duplex full
	ret |= (1 << 3);

	// force duplex enable
	ret |= (1 << 2);

//	enable rgmii timing
//	ret |= (1 << 15);
//	ret |= (1 << 14);

	// force 10mbs
	ret &= ~(1 << 0);
	ret &= ~(1 << 1);

	// force 100mbs
//	ret &= ~(1 << 0);
//	ret |= (1 << 1);

	write_PHY(21, 0x01, ret);
	delay_ms(10);

	ret = read_PHY(21, 0x01);
	delay_ms(10);

	ret |= (1 << 5); // force link up
	write_PHY(21, 0x01, ret);

	delay_ms(10);
	ret = read_PHY(21, 0x01);

	uint32_t val = read_PHY(21, 0x00);
	printf("PauseEn [15] = %d\r\n", (val >> 15) & 0x01);
	printf("MyPause [14] = %d\r\n", (val >> 14) & 0x01);
	printf("PhyDetect [12] = %d\r\n", (val >> 12) & 0x01);
	printf("Link [11] = %d\r\n", (val >> 11) & 0x01);
	printf("Duplex [10] = %d\r\n", (val >> 10) & 0x01);
	printf("Speed [9:8] = %d\r\n", (val & 0b110000000) >> 7);
	printf("TxPaused [5] = %d\r\n", (val >> 5) & 0x01);
	printf("FlowCtrl [4] = %d\r\n", (val >> 4) & 0x01);
	printf("CMode [3:0] = %d\r\n", val & 0b1111);
	printf("==============\r\n");

	uint32_t gpio = read_PHY(21, 0x1F);
	printf("PORT ENABLE %d\r\n", gpio >> 12);

//	ret = read_PHY(21, 0x04);
//	ret |= 0x03;
//	write_PHY(21, 0x04, ret);
//
//	delay_ms(10);
//	ret = read_PHY(21, 0x04);
//	printf("Port Control %d\r\n", ret);
}


void phy_scan(void)
{
	uint16_t recv = 0x0;
	for (int i = 0; i < 32; i++)
	{
//		write_PHY(i, 31, 0x10);
		recv = read_PHY(i, 0x00);
		if (recv != 0xFFFF) printf("PHY Scan [0x%02X GMIICR] = 0x%02X\n", i, recv);
	}
	printf("Scan done\n");
}

void psu_init (void) {
	//power supplies
	 printf("\PSU & IO init\n");

	LPC_GPIO1->DIR |= (1 << 26);//EN RTM 12V
	LPC_GPIO1->DIR |= (1 << 28);//EN VCCINT
	LPC_GPIO1->DIR |= (1 << 29);//EN P5V0
	LPC_GPIO1->DIR |= (1 << 30);//EN RTM MP
	LPC_GPIO1->DIR |= (1 << 31);//EN PSU CH
	LPC_GPIO1->SET |= (1 << 28); //EN VCCINT
	LPC_GPIO1->SET |= (1 << 29); //EN P5V0
	LPC_GPIO1->SET |= (1 << 31); //EN PSU CH
	LPC_GPIO1->DIR |= (1 << 26); //RTM 12V PWR pin
	LPC_GPIO1->DIR |= (1 << 30); //RTM 3.3MP pin
	delay_ms(100); //wait for power supplies to boot

	//I2C switch ADDR1=0
	LPC_GPIO2->DIR |= (1 << 29);
	LPC_GPIO2->CLR |= (1 << 29);

	//I2C switch ADDR2=0
	LPC_GPIO2->DIR |= (1 << 28);
	LPC_GPIO2->CLR |= (1 << 28);
	// i2c mux 0x70 address
}


/* Called from the TCP/IP thread */
void network_init(void *arg)
{
	ip_addr_t ipaddr, netmask, gw;

	/* Tell main thread TCP/IP init is done */
	*(s32_t *) arg = 1;

#if LWIP_DHCP
	IP4_ADDR(&gw, 0, 0, 0, 0);
	IP4_ADDR(&ipaddr, 0, 0, 0, 0);
	IP4_ADDR(&netmask, 0, 0, 0, 0);
#else
	IP4_ADDR(&gw, 192, 168, 95, 254);
	IP4_ADDR(&ipaddr, 192, 168, 95, 99);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
#endif

	netif_set_default(netif_add(&net_iface, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
	netif_set_status_callback(&net_iface, iface_callback);

#if LWIP_DHCP
	dhcp_start(&net_iface);
#else
	netif_set_up(&net_iface);
#endif
}

xTaskHandle xBroadcastTask = NULL;
#include "lwip/udp.h"

static void bcast_task(void *pvParameters)
{
	struct netconn *conn;
	char msg[] = "testing";
	struct netbuf *buf;
	char * data;

//	conn = netconn_new(NETCONN_UDP);
//	netconn_bind(conn, IP_ADDR_ANY, 7776);
//	netconn_connect(conn, IP_ADDR_BROADCAST, 7777);

	printf("[log] starting udp broadcaster task\n");

    struct pbuf  *p;
    char data1[8] = { 0x00 };
    u16_t  Port;
    Port = 69;
    int count = 0;
    int n = 0;
    int buflen = 8;
    struct udp_pcb *udp_1;
    struct ip_addr ipaddr, ipaddr_remote, netmask, gw;

    IP4_ADDR(&ipaddr_remote, 192, 168, 95, 146);

    udp_1 = udp_new();
    udp_connect(udp_1, &ipaddr_remote, Port);

	for (;;)
	{
		p = pbuf_alloc(PBUF_TRANSPORT, buflen, PBUF_POOL);

		if (!p) {
			printf("error allocating pbuf\r\n");
			return ERR_MEM;
		}

		memcpy(p->payload, data1, buflen);
		udp_send(udp_1, p);
		printf("SEND\r\n");
		count = 0;
		pbuf_free(p);

//		buf = netbuf_new();
//		data = netbuf_alloc(buf, sizeof(msg));
//		memcpy (data, msg, sizeof (msg));
//		netconn_send(conn, buf);
//		netbuf_delete(buf);
//
//		printf("Sending\r\n");

		vTaskDelay( 500 );
	}
}

void bcast_task_start(void)
{
	printf("[log] starting udp broadcaster\n");
	xTaskCreate(bcast_task , "UDP Daemon", configMINIMAL_STACK_SIZE, ( void * ) NULL, tskIDLE_PRIORITY + 1, &xBroadcastTask);
}

void bcast_task_stop(void)
{
	printf("[log] stopping udp broadcaster\n");
	if (xBroadcastTask != NULL)
		vTaskSuspend(xBroadcastTask);
}

/* LWIP kickoff and PHY link monitor thread */
static void vSetupIFTask(void *pvParameters) {
	volatile s32_t tcpipdone = 0;
	int32_t physts, phystsprev;

//	DEBUGSTR("LWIP HTTP Web Server FreeRTOS Demo...\r\n");

	/* Wait until the TCP/IP thread is finished before
	   continuing or wierd things may happen */
	DEBUGSTR("Waiting for TCPIP thread to initialize...\r\n");
	tcpip_init(network_init, (void *) &tcpipdone);

	while (!tcpipdone)
	{
		vTaskDelay(configTICK_RATE_HZ / 1000);
	}

	DEBUGSTR("Starting LWIP HTTP server...\r\n");

	/* Initialize and start application */
//	http_server_netconn_init();
//	bcast_task_start();

	/* This loop monitors the PHY link and will handle cable events
	   via the PHY driver. */
	phystsprev = 0;
	while (1)
	{
		physts = EMAC_CheckPHYStatus(EMAC_PHY_STAT_LINK);

		if (physts != phystsprev)
		{
			phystsprev = physts;
			if (physts == 1)
			{
				printf("Net up\n");
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up,
														  (void *) &net_iface, 1);
			} else {
				printf("Net down\n");
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_down,
														  (void *) &net_iface, 1);
			}
		}

		vTaskDelay(configTICK_RATE_HZ / 4);
	}
}

//
//uint16_t readphy(uint16_t bPhyAddr, uint8_t PhyReg)
//{
//	unsigned int tout;
//
//	LPC_EMAC->MADR = ((bPhyAddr & 0x1F) << 8 )| (PhyReg & 0x1F);
//	LPC_EMAC->MCMD = EMAC_MCMD_READ;
//
//	/* Wait until operation completed */
//	tout = 0;
//	for (tout = 0; tout < EMAC_MII_RD_TOUT; tout++)
//	{
//		if ((LPC_EMAC->MIND & EMAC_MIND_BUSY) == 0)
//		{
//			break;
//		}
//	}
//
//	LPC_EMAC->MCMD = 0;
//	return (LPC_EMAC->MRDD);
//}
//
//void writephy (uint16_t bPhyAddr,uint8_t PhyReg, uint16_t Value)
//{
//	unsigned int tout;
//
//	LPC_EMAC->MADR = ((bPhyAddr & 0x1F) << 8 )| (PhyReg & 0x1F);
//	LPC_EMAC->MWTD = Value;
//
//	/* Wait utill operation completed */
//	tout = 0;
//
//	for (tout = 0; tout < EMAC_MII_WR_TOUT; tout++)
//	{
//		if ((LPC_EMAC->MIND & EMAC_MIND_BUSY) == 0)
//		{
//			break;
//		}
//	}
//}

//
//void PHY_dump (void) {
//
//							//SET RGMII = 1 -> MII mode
//								LPC_GPIO2->CLR |= (1 << 2);
//
//								// EMAC_Configs.bPhyAddr   = 0x4;
//								printf("------------ PHY Dump ----------\n");
//				   		         printf("Register, ADDR DATA\n");
//
//				   		      writephy(0x04,  31, 0x10); //page 0 //(uint16_t bPhyAddr,uint8_t PhyReg, uint16_t Value)
//				   		         printf("BMCR %d 0x%x\n", 0, readphy(0x4, 0));
//				   		         printf("BMSR %d 0x%x\n", 1, readphy(0x4, 1));
//				   		         printf("ID1 %d 0x%x\n", 2, readphy(0x4, 2));
//				   		         printf("ID2 %d 0x%x\n", 3, readphy(0x4, 3));
//				   		         printf("AN_ADV %d 0x%x\n", 4, readphy(0x4, 4));
//				   		         printf("AN_RX %d 0x%x\n", 5, readphy(0x4, 5));
//				   		         printf("AN_EXP %d 0x%x\n", 6, readphy(0x4, 6));
//				   		         printf("EXT_STAT %d 0x%x\n", 15, readphy(0x4, 15));
//				   		      printf("page0\n");
//				   		         printf("JIT_DIAG %d 0x%x\n", 16, readphy(0x4, 16));
//				   		         printf("PCSCR %d 0x%x\n", 17, readphy(0x4, 17));
//				   		         printf("GMIICR %d 0x%x\n", 18, readphy(0x4, 18));
//				   		         printf("CR %d 0x%x\n", 19, readphy(0x4, 19));
//				   		         printf("IR %d 0x%x\n", 20, readphy(0x4, 20));
//				   		      printf("page1\n");
//				   		      writephy(0x04,  31, 0x11); //page 1
//				   		         printf("ID %d 0x%x\n", 16, readphy(0x4, 16));
//				   		         printf("GPIOCR1 %d 0x%x\n", 17, readphy(0x4, 17));
//				   		         printf("GPIOCR2 %d 0x%x\n", 18, readphy(0x4, 18));
//				   		         printf("GPIOSR %d 0x%x\n", 19, readphy(0x4, 19));
//				   		         printf("PTPCR1 %d 0x%x\n", 20, readphy(0x4, 20));
//				   		      printf("page2\n");
//					   		      writephy(0x04,  31, 0x12); //page 1
//					   		      	 printf("PTPCR1 %d 0x%x\n", 16, readphy(0x4, 16));
//					   		     	 printf("?? %d 0x%x\n", 17, readphy(0x4, 17));
//					   		         printf("?? %d 0x%x\n", 18, readphy(0x4, 18));
//					   		         printf("?? %d 0x%x\n", 19, readphy(0x4, 19));
//					   		         printf("?? %d 0x%x\n", 20, readphy(0x4, 20));
//					   		      printf("page3\n");
//						   		      writephy(0x04,  31, 0x13); //page 1
//						   		         printf("?? %d 0x%x\n", 16, readphy(0x4, 16));
//						   		         printf("?? %d 0x%x\n", 17, readphy(0x4, 17));
//						   		         printf("?? %d 0x%x\n", 18, readphy(0x4, 18));
//						   		         printf("?? %d 0x%x\n", 19, readphy(0x4, 19));
//						   		         printf("?? %d 0x%x\n", 20, readphy(0x4, 20));
//
//				   		         //SET RGMII = 1 -> RMII mode
//				   		      	LPC_GPIO2->SET |= (1 << 2);
//}
//
//void init_PHY (void) {
//
//
//	//		// init ETH PHY in MII mode
////					//RESET PHY
////
////						  	LPC_GPIO0->CLR |= (1 << 23);
////					//SET RGMII = 0 -> MMC MUX mode
////
////						  	LPC_GPIO2->CLR |= (1 << 2);
////					//PHY SPD 0 = 1
////							LPC_GPIO0->DIR |= (1 << 25);
////							LPC_GPIO0->SET |= (1 << 25);
////					//PHY SPD 1 = 0
////							LPC_GPIO0->DIR |= (1 << 24);
////							LPC_GPIO0->CLR |= (1 << 24);
////					//CFG DDR = 0
////							LPC_GPIO0->DIR |= (1 << 26);
////							LPC_GPIO0->CLR |= (1 << 26);
////					//un-RESET PHY
////							LPC_GPIO0->DIR |= (1 << 23);
////							LPC_GPIO0->SET |= (1 << 23);
////					// MII LED ON
////							LPC_GPIO0->DIR |= (1 << 31);
////							LPC_GPIO0->SET |= (1 << 31);
//					//MDIO
////					PINSEL_ConfigPin(1, 16, 1);    //( uint8_t portnum, uint8_t pinnum, uint8_t funcnum)
////					PINSEL_ConfigPin(1, 17, 1);
//
//			    	//// init ETH PHY in RGMII mode
//							//RESET PHY
//									LPC_GPIO0->DIR |= (1 << 23);
//								  	LPC_GPIO0->CLR |= (1 << 23);
//							//SET RGMII = 1 -> RGMII mode
//								  	LPC_GPIO2->DIR |= (1 << 2);
//								  	LPC_GPIO2->CLR |= (1 << 2); // give access to MMC
//							//PHY SPD 0 = 0
//									LPC_GPIO0->DIR |= (1 << 25);
//									LPC_GPIO0->CLR |= (1 << 25);
//							//PHY SPD 1 = 1
//									LPC_GPIO0->DIR |= (1 << 24);
//									LPC_GPIO0->SET |= (1 << 24);
//							//CFG DDR = 1
//									LPC_GPIO0->DIR |= (1 << 26);
//									LPC_GPIO0->SET |= (1 << 26);
//							//un-RESET PHY
//									LPC_GPIO0->DIR |= (1 << 23);
//									LPC_GPIO0->SET |= (1 << 23);
//							// MII LED OFF
//									LPC_GPIO0->DIR |= (1 << 31);
//									LPC_GPIO0->CLR |= (1 << 31);
//									//MDIO
//									PINSEL_ConfigPin(1, 16, 1);    //( uint8_t portnum, uint8_t pinnum, uint8_t funcnum)
//									PINSEL_ConfigPin(1, 17, 1);
//
//
//	//un-RESET PHY
//			   					LPC_GPIO0->DIR |= (1 << 23);
//			   					LPC_GPIO0->CLR |= (1 << 23); //PHY RST=0
//			   					//vTaskDelay(configTICK_RATE_HZ / 1000);
//			   					delay_ms(10);
//			   					LPC_GPIO0->SET |= (1 << 23); // PHY RST=1
//			   			    // ETH LED ON
//			   			   		LPC_GPIO0->DIR |= (1 << 31);
//			   			   		LPC_GPIO0->SET |= (1 << 31);
//
//								writephy(0x04,  31, 0x12); // select page 2
//								writephy(0x04,  16, 0x4004); // power down Rx CDR
//								//vTaskDelay(configTICK_RATE_HZ / 1000);
//								delay_ms(10);
//								writephy(0x04,  16, 0x4000); // power up Rx CDR
//								writephy(0x04,  0, 0x8000); // reset the data path BMCR.DP_RST
//
//								writephy(0x04,  31, 0x10); // select page 0
//								writephy(0x04,  18, 0b1000100010000000 ); // force RGMII, TXCLKEN=0
//								printf("GMIICR mod %d 0x%x\n", 18, readphy(0x4, 18));
//								printf("PHY init done\n");
//
//								//SET RGMII = 1 -> FPGA mode
//									  	LPC_GPIO2->DIR |= (1 << 2);
//									  	LPC_GPIO2->SET |= (1 << 2);
//
//
//};

static void vblinkIFTask(void *pvParameters)
{
   while(1)
	{
	   	   	   	   	   LPC_GPIO3->CLR |= (1 << 5);    // Make  the Port pins as high
	   		        	LPC_GPIO3->SET |= (1 << 4);
	   		        	vTaskDelay(configTICK_RATE_HZ / 5);

	   		            LPC_GPIO3->SET |= (1 << 5);     // Make  the Port pins as low
	   		            LPC_GPIO3->CLR |= (1 << 4);
	   		            vTaskDelay(configTICK_RATE_HZ / 5);
	   		           // printf("test \n");

	   		            //check if FPGA is programmed
	   		            //DONE line is high - FPGA not ready

	   		            if (LPC_GPIO0->PIN &(1 << 5))
	   						{

//	   		            	if (init_done==0)
//	   		            	{
//	   		            		init_PHY();
//	   		            		init_done=1;
//	   		            	}


	   						}
	   		            else
	   		            {

	   				      	//RESET PHY
//	   					   						LPC_GPIO0->DIR |= (1 << 23);
//	   					   						LPC_GPIO0->CLR |= (1 << 23);
//	   					   						// ETH LED OFF
//	   					   						LPC_GPIO0->DIR |= (1 << 31);
//	   					   						LPC_GPIO0->CLR |= (1 << 31);
//	   					   					init_done=0;
	   		            }


	   		         if (LPC_GPIO0->PIN &(1 << 30))
	   		         	   						{

	   		         	   		            	if (CLOCK_init_done==0)
	   		         	   		            	{
	   		         	   		            	 printf("clocking init \n");
												i2c_setSI5324();
												i2c_setSI5341();
												i2c_set8V54816();
	   		         	   		            		CLOCK_init_done=1;
	   		         	   		            	}


	   		         	   						}
	   		         	   		            else
	   		         	   		            {


	   		         	   					   	CLOCK_init_done=0;
	   		         	   		            }


	   		            //check if RTM is connected
		   		            //RTM_PSn is low

		   		            if (LPC_GPIO0->PIN &(1 << 29))
		   						{
		   						//3.3MP power off
		   						LPC_GPIO1->CLR |= (1 << 30);
		   						// 12V power off
		   						LPC_GPIO1->CLR |= (1 << 26);

		   						}
		   		            else
		   		            {
		   						//3.3MP power on
		   						LPC_GPIO1->SET |= (1 << 30);
		   						// 12V power 0n
		   						LPC_GPIO1->SET |= (1 << 26);

		   		            }

			   		     	LPC_GPIO4->DIR &= ~(1 << 12); // switch port IN
	// PRINT phy AFTER BUTTON IS PRESSED



							 if (!(LPC_GPIO4->PIN & (1 << 12)))
							 {


					//	PHY_dump();
			   		     i2c_Exar_dump();

							 }

							 uint8_t Rxbuf[2];
		if( UART_Receive(LPC_UART0, Rxbuf,1, NONE_BLOCKING))
		{
			if (Rxbuf[0] == 'P')
			{
					i2c_Exar_dump();
			}
			else if (Rxbuf[0] == 'J')
			{
				LPC_GPIO2->CLR |= (1 << 16); // BOOT0
				LPC_GPIO2->CLR |= (1 << 17); // BOOT1
				LPC_GPIO2->CLR |= (1 << 18); // BOOT2
				LPC_GPIO2->CLR |= (1 << 19); // BOOT3
				printf("jtag\r\n");
			}
			else if (Rxbuf[0] == 'Q')
			{
				LPC_GPIO2->CLR |= (1 << 16); // BOOT0
				LPC_GPIO2->SET |= (1 << 17); // BOOT1
				LPC_GPIO2->CLR |= (1 << 18); // BOOT2
				LPC_GPIO2->CLR |= (1 << 19); // BOOT3
				printf("qspi\r\n");
			}
			else if (Rxbuf[0] == 'L')
			{
				LPC_GPIO2->CLR |= (1 << 16); // BOOT0
				LPC_GPIO2->SET |= (1 << 17); // BOOT1
				LPC_GPIO2->SET |= (1 << 18); // BOOT2
				LPC_GPIO2->SET |= (1 << 19); // BOOT3
				printf("SD with Level Shifter\r\n");
			}
			else if (Rxbuf[0] == 'U')
			{
				LPC_GPIO2->SET |= (1 << 16); // BOOT0
				LPC_GPIO2->SET |= (1 << 17); // BOOT1
				LPC_GPIO2->SET |= (1 << 18); // BOOT2
				LPC_GPIO2->CLR |= (1 << 19); // BOOT3
				printf("USB\r\n");
			}
			else if (Rxbuf[0] == 'S')
			{
				LPC_GPIO2->SET |= (1 << 16); // BOOT0
				LPC_GPIO2->CLR |= (1 << 17); // BOOT1
				LPC_GPIO2->SET |= (1 << 18); // BOOT2
				LPC_GPIO2->CLR |= (1 << 19); // BOOT3
				printf("sdcard\r\n");
			}
			else if (Rxbuf[0] == 'R')
			{
				NVIC_SystemReset();
//				LPC_GPIO4->SET |= (1 << 9);
//				vTaskDelay(100);
//				LPC_GPIO4->CLR |= (1 << 9);
//				vTaskDelay(100);
//				LPC_GPIO4->SET |= (1 << 9);
//				printf("reset\r\n");
			}
		}
	}
   //vTaskDelay(configTICK_RATE_HZ / 4);
}

static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	gpio_pre_pwr_init();

	uart_init();
//	cli_init();
	i2c_init();
	spi_init();
}

static void vblinkIFTask2(void *pvParameters)
{
   while(1)
	{
	   printf("========================\r\n");
	   printf("TSV0 %d TSV1 %d\r\n", LPC_EMAC->TSV0, LPC_EMAC->TSV1);
	   printf("EMAC STATUS %d\r\n", LPC_EMAC->Status);
	   uint32_t ret = LPC_EMAC->TSV0;
	   if (ret & 0x01) printf("CRCERR\r\n");
	   if (ret & 0x02) printf("LCE\r\n");
	   if (ret & 0x04) printf("LOR\r\n");
	   if (ret & 0x08) printf("DONE\r\n");
	   if (ret & 0x10) printf("MULTICAST\r\n");
	   if (ret & 0x20) printf("BROADCAST\r\n");
	   if (ret & 0x40) printf("PACKETDEFER\r\n");
	   if (ret & 0x80) printf("EXDF\r\n");
	   if (ret & 0x100) printf("EXCOL\r\n");
	   if (ret & 0x200) printf("LCOL\r\n");
	   if (ret & 0x400) printf("GIANT\r\n");
	   if (ret & 0x800) printf("UNDERRUN\r\n");
	   if (ret & 0x10000000) printf("CONTROLFRAME\r\n");
	   if (ret & 0x20000000) printf("PAUSE\r\n");
	   if (ret & 0x40000000) printf("BACKPRESSURE\r\n");
	   if (ret & 0x80000000) printf("VLAN\r\n");
	   printf("========================\r\n");
	   vTaskDelay(1000);
	}
}

/**
 * Program entry point 
 */
int main(void)
{
	prvSetupHardware();

	psu_init();
	delay_ms(200);
//	printf("xr start\r\n");
	xr77129_load_runtimes();

//	i2c_probe_slaves2(); // probe i2cmux slaves at every channel
	printf("XR END\r\n");
	delay_ms(100);

	LPC_GPIO0->DIR |= (1 << 23); 		// Switch RESETn
	LPC_GPIO4->SET |= (1 << 18); 		//initialize SCANSTA
	LPC_GPIO4->DIR |= (1 << 18);		// scansta  reset
	LPC_GPIO4->DIR |= (1 << 25);		//lpsel0 PROG master
	LPC_GPIO4->DIR |= (1 << 26);		//lpsel1 MMC JTAG
	LPC_GPIO4->DIR |= (1 << 27);		//lpsel2 FPGA JTAG
	LPC_GPIO4->DIR |= (1 << 28);		//lpsel3 FMC1JTAG
	LPC_GPIO4->DIR |= (1 << 29);		//lpsel4 FMC2 JTAG
  	LPC_GPIO4->DIR |= (1 << 30);		//lpsel5 RTM JTAG
  	LPC_GPIO4->DIR |= (1 << 31);		//lpsel6 PS JTAG
  	LPC_GPIO4->DIR |= (1 << 19);		//stitcher mode
  	LPC_GPIO0->DIR &= ~(1 << 29);     // Make  the Port pin input - RTM present
  	LPC_GPIO0->DIR &= ~(1 << 5);     // Make  the Port pins inputs - DONE pin

  	LPC_GPIO4->DIR |= (1 << 9); //FPGA resetn output
  	LPC_GPIO0->DIR &= ~(1 << 13); //FPGA INIT_B as input
  	LPC_GPIO0->DIR &= ~(1 << 4); //FPGA PROG_B as input
 	LPC_GPIO2->DIR |= (1 << 16); //BOOT0
	LPC_GPIO2->DIR |= (1 << 17); //BOOT1
 	LPC_GPIO2->DIR |= (1 << 18); //BOOT2
	LPC_GPIO2->DIR |= (1 << 19); //BOOT3
	LPC_GPIO0->DIR |= (1 << 12); //EN FLASH UPDATE
	LPC_GPIO0->DIR &= ~(1 << 30); //P12V OK input

		////					//un-RESET PHY
		////
////							LPC_GPIO0->SET |= (1 << 23);

  	LPC_GPIO4->CLR |= (1 << 9); //keep FPGA in reset state

 	LPC_GPIO2->CLR |= (1 << 16);//BOOT0 boot mode = JTAG
	LPC_GPIO2->CLR |= (1 << 17);//BOOT1 boot mode =
 	LPC_GPIO2->CLR |= (1 << 18);//BOOT2 boot mode =
	LPC_GPIO2->CLR |= (1 << 19);//BOOT3	boot mode =

	LPC_GPIO0->SET |= (1 << 12);//EN FLASH UPDATE = 1

	delay_ms(200); //wait for power supplies to boot

	// RESET PHY
	LPC_GPIO0->DIR |= (1 << 23);
	LPC_GPIO0->CLR |= (1 << 23); // keep Switch in reset state

	//SET RGMII = 0 -> MMC MUX mode
	LPC_GPIO2->DIR |= (1 << 2);

	//LEDs
	LPC_GPIO3->DIR |= (1 << 5);
	LPC_GPIO3->DIR |= (1 << 4);

	//SCANSTA config
  	LPC_GPIO4->CLR |= (1 << 18); //keep scansta in reset
  	delay_ms(10);
  	LPC_GPIO4->CLR |= (1 << 25);//lpsel0 keep unseleted - inactive master port becomes LSP0
  	LPC_GPIO4->CLR |= (1 << 26);//lpsel1 MMC JTAG
  	LPC_GPIO4->SET |= (1 << 27);//lpsel2 FPGA JTAG
  	LPC_GPIO4->CLR |= (1 << 28);//lpsel3 FMC1 JTAG
  	LPC_GPIO4->CLR |= (1 << 29);//lpsel4 FMC2 JTAG
//  	LPC_GPIO4->CLR |= (1 << 30);//lpsel5 RTM JTAG
  	LPC_GPIO4->CLR |= (1 << 31);//lpsel6 PS JTAG
  	LPC_GPIO4->CLR |= (1 << 19);//stitcher mode

    //check if RTM is connected  RTM_PSn is low
	if (LPC_GPIO0->PIN &(1 << 29))
	{
		LPC_GPIO4->CLR |= (1 << 30);//lpsel5 RTM JTAG
	}
	else
	{
		LPC_GPIO4->SET |= (1 << 30);//lpsel5 RTM JTAG
	}


 	delay_ms(10);
 	printf("\SCANSTA init\n");

	LPC_GPIO0->CLR |= (1 << 23);
	delay_ms(10);
	LPC_GPIO0->SET |= (1 << 23);

	LPC_GPIO0->SET |= (1 << 23); // release Ethernet switch RESETn
	LPC_GPIO0->DIR |= (1 << 31); // MDIO enable line
	LPC_GPIO0->CLR |= (1 << 31); // connect Ethernet switch to ZYNQ


//			  	LPC_GPIO4->SET |= (1 << 18); //initialize SCANSTA
//			  	LPC_GPIO0->SET |= (1 << 23); // release Ethernet switch RESETn
//				LPC_GPIO0->DIR |= (1 << 31); // MDIO enable line
//				LPC_GPIO0->SET |= (1 << 31); // connect Ethernet switch to ZYNQ

	LPC_GPIO1->CLR |= (1 << 30); //RTM 3.3MP power off
	LPC_GPIO1->CLR |= (1 << 26);// 12V power off
	//init_PHY();

  // set correct levels on clock management i2c extender
//	i2c_TCA9539(CLK_SI57X_OE2 | CLK_CLK_SW_RSTn | CLK_SI57X_OE1 | CLK_SI53xx_RST); // setup the clock circuit mux
//	i2c_TCA9539( CLK_CLK_SW_RSTn  | CLK_SI53xx_RST); // set all ones on clock circuit mux

	//write SI5324 register values
	//i2c_setSI5324();
	//i2c_setSI5341();
	//i2c_set8V54816();
	LPC_GPIO4->SET |= (1 << 9); //release FPGA reset
	//LPC_GPIO4->DIR &= ~(1 << 9);     // place FPGA_reset in Hi-Z mode



       //check if RTM is connected
           //RTM_PSn is low

           if (LPC_GPIO0->PIN &(1 << 29))
				{
				//3.3MP power off
				LPC_GPIO1->CLR |= (1 << 30);
				// 12V power off
				LPC_GPIO1->CLR |= (1 << 26);

				}
           else
           {
				//3.3MP power on
				LPC_GPIO1->SET |= (1 << 30);
				// 12V power 0n
				LPC_GPIO1->SET |= (1 << 26);
				delay_ms(200);
				// enable DC/DC
			//	i2c_RTM_PWRON();
			 //   delay_ms(1000);
			//	i2c_setSI5324_RTM();
           }


           LPC_GPIO0->CLR |= (1 << 23);
           delay_ms(200);
           LPC_GPIO0->SET |= (1 << 23);
//
//
//       for (int j = 0x10; j < 0x17; j++)
//       {
//    	   delay_ms(5);
//    	   uint32_t val = read_PHY(21, 0x00);
//    	   printf("Link [%d] = %d\r\n", 21, (val >> 11) & 0x01);
//    	   printf("CMode [%d] = %d\r\n", 21, val & 0b1111);
//    	   printf("==============\r\n");
//       }
////
////       uint16_t val = read_PHY(21, 0x01);
////       printf("PHY [0x%02X] Reg %X = %d\r\n", 21, 0x01, val);
////       val |= (1 << 14);
//////       val |= (1 << 15);
////
////       write_PHY(21, 0x01, val);
////       val = 0x00;
////       val = read_PHY(21, 0x01);
////	   printf("PHY [0x%02X] Reg %X = %d\r\n", 21, 0x01, val);
//
//


//       // enable RSVD2CPU
//	   uint8_t addr_mask = read_PHY(0x1C, 0x02);
//	   printf("Register map %d\r\n", addr_mask);
//       write_PHY(0x1C, 0x02, 0xFFFF);
//
//       addr_mask = read_PHY(0x1C, 0x02);
//       printf("Register map %d\r\n", addr_mask);

//       // enable RSVD2CPU bit
//       uint8_t mgmt_mask = read_PHY(0x1C, 0x05);
//       printf("mgmt mask %d\r\n", addr_mask);
//       mgmt_mask |= (1 << 3);
//       write_PHY(0x1C, 0x05, mgmt_mask);

//       mgmt_mask = read_PHY(0x1C, 0x05);
//       printf("mgmt mask %d\r\n", addr_mask);

//       for (int i = 0; i < 33; i++)
//       {
////	   write_PHY(i, 22, 0x00);
//	   for (int j = 0; j < 10; j++) {
//		   delay_ms(5);
//		   uint32_t val = read_PHY(i, j);
//		   printf("PHY [0x%02X] Reg %X = %d\r\n", i, j, val);
//	   }
//       }
	   delay_ms(1);

	/* First record in log also initialize it */
 	printf("Started. CPU clock = %lu MHz, SDRAM clock = %lu Mhz\n", SystemCoreClock / 1000000u, EMCClock / 1000000u);

	/* Add another thread for initializing physical interface. This
	   is delayed from the main LWIP initialization. */
	xTaskCreate(vSetupIFTask, (signed char *) "SetupIFx", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
	//blink LED task
	xTaskCreate(vblinkIFTask, "Blink", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
//	xTaskCreate(vblinkIFTask2, "Blink", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
//	xTaskCreate(vCommandConsoleTask, "CLI", configMINIMAL_STACK_SIZE + 256UL, NULL, tskIDLE_PRIORITY + 1, NULL );

	vTaskStartScheduler();
	return 1;
}
