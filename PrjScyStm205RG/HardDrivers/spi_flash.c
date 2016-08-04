/*----------------------------------------Copyright (c)---------------------------------------------------                              
*                                        http://www.39board.com
*
*      39board�������ṩ������ST��TI��Freescale��Altera��ƽ̨�����̡��ĵ���������������Ϣּ��Ϊѧϰ���ṩ
*  ѧϰ�ο�����ʹ������ʹ�ù��������κ�ԭ�����ֱ�ӻ��ӵ���ʧ���������Ҳ��е��κ����Ρ�
*  
*
*
*                                                                             ����������39board������
*
*----------------------------------------------------------------------------------------------------------
* File name:          spi_flash.c
* Author:             alvan wang
* Version:            V1.0
* Date:               2014-01-03
* Description:		
* Function List:		
* History:    		
*-----------------------------------------------------------------------------------------------------------
*/

#include "stm32f2xx.h"
#include "spi_flash.h"


/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_init
* Description:      �ú�����spi_flash �õ���SPI1���г�ʼ����spi_flash_cs--GPIOA.4,spi1_sck--GPIOA.5,
                    spi1_miso--GPIOA.6,spi1_mosi--GPIOA.7
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-02
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void  spi_flash_init (void)
{	
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /*!< Enable the SPI clock */
    sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK, ENABLE);

    /*!< Enable GPIO clocks */
    RCC_AHB1PeriphClockCmd(sFLASH_SPI_SCK_GPIO_CLK  | sFLASH_SPI_MISO_GPIO_CLK | 
                           sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_CS_GPIO_CLK, ENABLE);

    /*!< SPI pins configuration *************************************************/

    /*!< Connect SPI pins to AF5 */  
    GPIO_PinAFConfig(sFLASH_SPI_SCK_GPIO_PORT, sFLASH_SPI_SCK_SOURCE, sFLASH_SPI_SCK_AF);
    GPIO_PinAFConfig(sFLASH_SPI_MISO_GPIO_PORT, sFLASH_SPI_MISO_SOURCE, sFLASH_SPI_MISO_AF);
    GPIO_PinAFConfig(sFLASH_SPI_MOSI_GPIO_PORT, sFLASH_SPI_MOSI_SOURCE, sFLASH_SPI_MOSI_AF);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        
    /*!< SPI SCK pin configuration */
    GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
    GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< SPI MOSI pin configuration */
    GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MOSI_PIN;
    GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< SPI MISO pin configuration */
    GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MISO_PIN;
    GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
    GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);    

    /*!< SPI configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(sFLASH_SPI, &SPI_InitStructure);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(sFLASH_SPI, ENABLE);
}   

/*
*--------------------------------------------------------------------------------------------------------
* Function:     	spi_flash_send_byte()
* Description:      SPI1����һ���ֽڵ�����
* Input:            Ҫ���͵�����
* Output:           none
* Return:           ���յ�������
* Created by:       alvan wang
* Created date:     2014-01-02
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
u8 spi_flash_send_byte (u8 data)
{					
    while((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET);   /* �ȴ����ͻ�������                             */ 
    SPI1->DR = data;                                 /* ����һ���ֽ�����                             */ 
    while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);  /* �ȴ�������һ���ֽ�����                       */	
    return(SPI1->DR);                                /* ���ؽ��յ�������                             */ 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_cs_low
* Description:      spi_flash_cs�����
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_cs_low (void)
{
    GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN);
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_cs_high
* Description:      spi_flash_cs�����
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_cs_high (void)
{
    GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN);
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_write_enable
* Description:      оƬдʹ��
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_write_enable(void)
{
    spi_flash_cs_low(); 
    spi_flash_send_byte(SPI_FLASH_WRITE_ENABLE); 
    spi_flash_cs_high(); 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_wait_for_write_end
* Description:      �ȴ�д����
* Input:            none
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_wait_for_write_end (void)
{
    u8 flash_status = 0;

    spi_flash_cs_low(); 
    spi_flash_send_byte(SPI_FLASH_READ_STATUS_REG); 
    do
    {
        flash_status = spi_flash_send_byte(0x00);
    }while ((flash_status & 0x01) == 0x01); 

    spi_flash_cs_high();
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:       spi_flash_read_device_ID
* Description:    ��ȡоƬID
* Input:          none
* Output:         оƬID
* Return:         none
* Created by:     alvan wang
* Created date:   2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
u8 spi_flash_read_device_ID (void)
{
    u8 data_buf;

    spi_flash_cs_low(); 
    spi_flash_send_byte(SPI_FLASH_DEVICE_ID); 
    spi_flash_send_byte(0x00); 
    spi_flash_send_byte(0x00); 
    spi_flash_send_byte(0x00);
    data_buf = spi_flash_send_byte(0x00);
    spi_flash_cs_high(); 

    return 	data_buf;
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_sector_erase
* Description:      ������������
* Input:            Ҫ������������ַ
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_sector_erase (u32 sector_addr)
{
    spi_flash_write_enable();

    spi_flash_cs_low();
    spi_flash_send_byte(SPI_FLASH_SECTOR_ERASE);
    spi_flash_send_byte((sector_addr & 0xFFFFFF) >> 16);
    spi_flash_send_byte((sector_addr & 0xFFFF) >> 8);
    spi_flash_send_byte(sector_addr & 0xFF);
    spi_flash_cs_high();

    spi_flash_wait_for_write_end(); 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_flash_block_erase
* Description:      ���������
* Input:            Ҫ�����Ŀ��ַ
* Output:           none
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_block_erase (u32 block_addr)
{
    spi_flash_write_enable();

    spi_flash_cs_low();
    spi_flash_send_byte(SPI_FLASH_BLOCK_ERASE);
    spi_flash_send_byte((block_addr & 0xFFFFFF) >> 16);
    spi_flash_send_byte((block_addr & 0xFFFF) >> 8);
    spi_flash_send_byte(block_addr & 0xFF);
    spi_flash_cs_high();

    spi_flash_wait_for_write_end(); 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:           spi_flash_chip_erase
* Description:        ȫƬ��������
* Input:              none
* Output:             none
* Return:             none
* Created by:         alvan wang
* Created date:       2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_chip_erase (void)
{
    spi_flash_write_enable();
    
    spi_flash_cs_low();
    spi_flash_send_byte(SPI_FLASH_CHIP_ERASE);
    spi_flash_cs_high();
    
    spi_flash_wait_for_write_end(); 
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:           spi_flash_page_write
* Description:        д���ݺ���
* Input:              write_addr��д������ݵ�ַ 
                      p_buffer��Ҫд�������ָ��
                      num_byte_to_write��д�����������(���Ϊ256�ֽ�)
* Output:             none
* Return:             none
* Created by:         alvan wang
* Created date:       2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_page_write (u32 write_addr, u8* p_buffer, u16 num_byte_to_write)
{
    spi_flash_write_enable();

    spi_flash_cs_low();
    spi_flash_send_byte(SPI_FLASH_PAGE_PROGRAM);
    spi_flash_send_byte((write_addr & 0xFFFFFF) >> 16);
    spi_flash_send_byte((write_addr & 0xFFFF) >> 8);
    spi_flash_send_byte(write_addr & 0xFF);	
    while (num_byte_to_write--)
    {
        spi_flash_send_byte(*p_buffer);
        p_buffer++;
    }
    spi_flash_cs_high();
    spi_flash_wait_for_write_end();
}


/*
*--------------------------------------------------------------------------------------------------------
* Function:           spi_flash_Write
* Description:        д���ݺ���
* Input:              write_addr��д������ݵ�ַ 
                      p_buffer��Ҫд�������ָ��
                      num_byte_to_write��д�����������
* Output:             none
* Return:             none
* Created by:         alvan wang
* Created date:       2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_write (u32 write_addr, u8* p_buffer, u16 num_byte_to_write)
{
    unsigned int num_of_page = 0, num_of_sigle = 0, page_addr = 0, 
                 count = 0, temp = 0, byte_addr = write_addr;

    page_addr = byte_addr  % SPI_FLASH_PAGE_SIZE;
    count = SPI_FLASH_PAGE_SIZE - page_addr;
    num_of_page =  num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_sigle = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

    if (page_addr == 0) 
    {
        if (num_of_page == 0) /* num_byte_to_write < SPI_FLASH_PageSize */
        {
            if((byte_addr % 4096) == 0)
            {
                spi_flash_sector_erase(byte_addr);
            }
            spi_flash_page_write (byte_addr, p_buffer, num_byte_to_write);
        }
        else /* num_byte_to_write > SPI_FLASH_PageSize */
        {
            while (num_of_page--)
            {
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                
                spi_flash_page_write (byte_addr, p_buffer, SPI_FLASH_PAGE_SIZE);
                byte_addr +=  SPI_FLASH_PAGE_SIZE;
                p_buffer += SPI_FLASH_PAGE_SIZE;
            }

            if((byte_addr % 4096) == 0)
            {
                spi_flash_sector_erase(byte_addr);
            }

            spi_flash_page_write (byte_addr, p_buffer, num_of_sigle);
        }
    }
    else 
    {
        if (num_of_page == 0) /* num_byte_to_write < SPI_FLASH_PageSize */
        {
            if (num_of_sigle > count) /* (num_byte_to_write + write_addr) > SPI_FLASH_PageSize */
            {
                temp = num_of_sigle - count;
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                spi_flash_page_write (byte_addr, p_buffer, count);
                byte_addr +=  count;
                p_buffer += count;
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                spi_flash_page_write (byte_addr, p_buffer, temp);
            }
            else
            {
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                spi_flash_page_write (byte_addr, p_buffer, num_byte_to_write);
            }
        }
        else /* num_byte_to_write > SPI_FLASH_PageSize */
        {
            num_byte_to_write -= count;
            num_of_page =  num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_sigle = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            if((byte_addr % 4096) == 0)
            {
                spi_flash_sector_erase(byte_addr);
            }
            spi_flash_page_write (byte_addr, p_buffer, count);
            byte_addr +=  count;
            p_buffer += count;

            while (num_of_page--)
            {
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                spi_flash_page_write (byte_addr, p_buffer, SPI_FLASH_PAGE_SIZE);
                byte_addr +=  SPI_FLASH_PAGE_SIZE;
                p_buffer += SPI_FLASH_PAGE_SIZE;
            }

            if (num_of_sigle != 0)
            {
                if((byte_addr % 4096) == 0)
                {
                    spi_flash_sector_erase(byte_addr);
                }
                spi_flash_page_write (byte_addr, p_buffer, num_of_sigle);
            }
        }
    }
}

/*
*--------------------------------------------------------------------------------------------------------
* Function:         spi_falsh_read
* Description:      �����ݺ���
* Input:            read_addr����ȡ�����ݵ�ַ                
                    num_byte_to_write����ȡ���ݵ�����
* Output:           p_buffer����ȡ���ݵĻ�����ָ��
* Return:           none
* Created by:       alvan wang
* Created date:     2014-01-03
* Others:        	
*---------------------------------------------------------------------------------------------------------
*/
void spi_flash_read (u32 read_addr, u8* p_buffer, u16 num_byte_to_read)
{
    spi_flash_cs_low();
    spi_flash_send_byte(SPI_FLASH_FAST_READ_DATA);
    spi_flash_send_byte((read_addr & 0xFFFFFF) >> 16);
    spi_flash_send_byte((read_addr & 0xFFFF) >> 8);
    spi_flash_send_byte(read_addr & 0xFF);	
    spi_flash_send_byte(0x00);

    while (num_byte_to_read--)
    {
        *p_buffer = spi_flash_send_byte(0xff);
        p_buffer++;
    }
    spi_flash_cs_high();
}


/************************ (C) COPYRIGHT ZOEHOO *****************************END OF FILE********************************/


