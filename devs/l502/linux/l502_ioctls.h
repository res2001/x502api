#ifndef LPCIE_IOCTLS_H
#define LPCIE_IOCTLS_H

#include "linux/ioctl.h"
#include "linux/types.h"


#define L502_BF_IO_SIZE  8

#define LPCIE_DEVNAME_SIZE      32
#define LPCIE_SERIAL_SIZE       32
#define LPCIE_SOFTVER_SIZE      32
#define LPCIE_REVISION_SIZE     16
#define LPCIE_MODIFICATION_SIZE 16
#define LPCIE_SPECINFO_SIZE     64

/** Информация о модуле */
typedef struct
{
    char Name[LPCIE_DEVNAME_SIZE]; /**< название модуля ("L-502") */
    char Serial[LPCIE_SERIAL_SIZE];   /**< серийный номер изделия */
    char SoftVer[LPCIE_SOFTVER_SIZE]; /**< версия ПО контроллера */
    char Revision[LPCIE_REVISION_SIZE]; /**< ревизия платы */
    char Modification[LPCIE_MODIFICATION_SIZE]; /**< опции */
    char SpecInfo[LPCIE_SPECINFO_SIZE];       /**< резервная информация */
} t_lpcie_devinfo;

typedef struct
{
    __u32 addr;
    __u32 val;
} t_lpcie_mem_rw;

typedef struct
{
    __u32 page_size; /** размер каждой страницы */
    __u32 irq_step; /** количество отсчетов, после которого генерится прерывание */
    __u16 pages_cnt; /** количество страниц */
    __u16 packet_size; /** размер пакета по PCI-Express */
} t_lpcie_dma_params;

typedef struct
{
    uint32_t addr;
    uint32_t reserv;
    uint32_t buf[L502_BF_IO_SIZE];
} t_l502_bf_rw;

#define LPCIE_IO_MAGIC 'L'

#define LPCIE_IOCTL_GET_DEVINFO _IOR(LPCIE_IO_MAGIC, 0x80, t_lpcie_devinfo)
#define LPCIE_IOCTL_TIMER_START _IOW(LPCIE_IO_MAGIC, 0x81, __u32)
#define LPCIE_IOCTL_TIMER_STOP  _IO(LPCIE_IO_MAGIC, 0x82)
#define LPCIE_IOCTL_READ_CFG    _IOWR(LPCIE_IO_MAGIC, 0x83, __u32)
#define LPCIE_IOCTL_MEM_FPGA_RD _IOWR(LPCIE_IO_MAGIC, 0x86, t_lpcie_mem_rw)
#define LPCIE_IOCTL_MEM_FPGA_WR _IOW(LPCIE_IO_MAGIC, 0x87, t_lpcie_mem_rw)
#define LPCIE_IOCTL_MEM_BLOCK_RD _IOWR(LPCIE_IO_MAGIC, 0x86, t_lpcie_mem_rw)

#define LPCIE_IOCTL_ADC_START       _IO(LPCIE_IO_MAGIC, 0x90)
#define LPCIE_IOCTL_ADC_STOP        _IO(LPCIE_IO_MAGIC, 0x91)
#define LPCIE_IOCTL_DAC_START       _IO(LPCIE_IO_MAGIC, 0x92)
#define LPCIE_IOCTL_DAC_STOP        _IO(LPCIE_IO_MAGIC, 0x93)
#define LPCIE_IOCTL_SET_ADC_DMA_PAR _IOW(LPCIE_IO_MAGIC, 0x94, t_lpcie_dma_params)
#define LPCIE_IOCTL_SET_DAC_DMA_PAR _IOW(LPCIE_IO_MAGIC, 0x95, t_lpcie_dma_params)
#define LPCIE_IOCTL_DAC_RST_BUFS    _IO(LPCIE_IO_MAGIC, 0x96)

#define LPCIE_IOCTL_ADC_GET_RDY     _IOR(LPCIE_IO_MAGIC, 0x97, __u32)
#define LPCIE_IOCTL_DAC_GET_FREE    _IOR(LPCIE_IO_MAGIC, 0x98, __u32)

#define LPCIE_IOCTL_DAC_SET_CYCLE_BUF _IOW(LPCIE_IO_MAGIC, 0x99, __u32)
#define LPCIE_IOCTL_DAC_SET_CYCLE_DATA _IOW(LPCIE_IO_MAGIC, 0x9A, __u32)


#define LPCIE_IOCTL_BF_RD _IOR(LPCIE_IO_MAGIC, 0xA0, t_l502_bf_rw)
#define LPCIE_IOCTL_BF_WR _IOW(LPCIE_IO_MAGIC, 0xA0, t_l502_bf_rw)
#define LPCIE_IOCTL_BF_HOST_IRQ _IO(LPCIE_IO_MAGIC, 0xA1)

#endif // LPCIE_IOCTLS_H
