#include "stm32f0xx_conf.h"
#include "stm32f0xx_spi.h"

#include "ssp.h"

/*****************************************************************************
** Function name:		spi1_init
**
** Descriptions:		SPI1(SSP) port initialization routine
**				
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**				can't be installed correctly, return false.
** 
*****************************************************************************/
uint32_t spi1_init(void)
{
  GPIO_InitTypeDef GPIO_init_structure;
  SPI_InitTypeDef SPI_init_structure;
  
  // Enable SPI clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  // SPI pins configuration
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_init_structure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_init_structure.GPIO_Mode = GPIO_Mode_AF;  
  GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOB, &GPIO_init_structure);
  
  // Setup SS pin as output pin
  GPIO_init_structure.GPIO_Pin = GPIO_Pin_11;
  GPIO_init_structure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_init_structure.GPIO_OType = GPIO_OTYPER_OT_11;
  GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_init_structure);
  
  SPI_init_structure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_init_structure.SPI_CPOL = SPI_CPOL_Low;
  SPI_init_structure.SPI_DataSize = SPI_DataSize_8b;
  SPI_init_structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_init_structure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_init_structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_init_structure.SPI_NSS = SPI_NSS_Soft;
  SPI_init_structure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI1, &SPI_init_structure);
  
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(SPI1, ENABLE);
  
  GPIOB->BSRR = (1 << 11);
  return 1;
}

/*****************************************************************************
** Function name:		spi1_send
**
** Descriptions:		Send a block of data to the SPI1(SSP) port, the 
**				first parameter is the buffer pointer, the 2nd 
**				parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/
void spi1_send(uint8_t *buf, uint32_t length)
{
  uint32_t i;
  
  for ( i = 0; i < length; i++ )
  {
    /* wait as long as the transimission register is not empty */
    while (SPI_GetTransmissionFIFOStatus(SPI1) != SPI_TransmissionFIFOStatus_Empty);
    SPI_SendData8(SPI1, *buf);
    buf++;
    while (SPI_GetReceptionFIFOStatus(SPI1) != SPI_ReceptionFIFOStatus_1QuarterFull);        
  }
  return; 
}

/*****************************************************************************
** Function name:		spi1_receive
** Descriptions:		the module will receive a block of data from 
**				the SPI1(SSP), the 2nd parameter is the block 
**				length.
** parameters:			buffer pointer, and block length
** Returned value:		None
** 
*****************************************************************************/
void spi1_receive(uint8_t *buf, uint32_t length)
{
  uint32_t i;
  
  for ( i = 0; i < length; i++ )
  {    
    /* wait as long as the transimission register is not empty */
    while (SPI_GetTransmissionFIFOStatus(SPI1) != SPI_TransmissionFIFOStatus_Empty);
    SPI_SendData8(SPI1, 0xAA);
    while (SPI_GetReceptionFIFOStatus(SPI1) != SPI_ReceptionFIFOStatus_1QuarterFull);
    *buf = SPI_ReceiveData8(SPI1);
    buf++;
  }
  
  return; 
}

uint8_t spiSendRcv(uint8_t outgoing)
{
  uint8_t incoming=0;
  
  /* wait as long as the transimission register is not empty */
  while (SPI_GetTransmissionFIFOStatus(SPI1) != SPI_TransmissionFIFOStatus_Empty);
  SPI_SendData8(SPI1, outgoing);
  while (SPI_GetReceptionFIFOStatus(SPI1) != SPI_ReceptionFIFOStatus_1QuarterFull);
  incoming = SPI_ReceiveData8(SPI1);
  
  return incoming;
}



/******************************************************************************
**                            End Of File
******************************************************************************/



