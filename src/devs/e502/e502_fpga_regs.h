#ifndef E502_FPGA_REGS_H
#define E502_FPGA_REGS_H

#ifndef L5XX_REGS_H
#define L5XX_REGS_H

#define E502_MAX_PAGES_CNT  252

#define E502_BF_SDRAM_SIZE (32UL*1024*1024)

#define E502_BF_MEMADDR_CMD      0xFF800800


#define E502_BF_CMD_READ          0x0001
#define E502_BF_CMD_WRITE         0x0002
#define E502_BF_CMD_HIRQ          0x0004
#define E502_BF_CMD_HDMA_RST      0x0008



/********************* Адреса регистров блока ARM_INTERFACE *******************/

#define E502_REGS_ARM_BLOCK                       0x0100
#define E502_REGS_ARM_DMA                         (E502_REGS_ARM_BLOCK+0)
#define E502_REGS_ARM_HARD_ID                     (E502_REGS_ARM_BLOCK+0xA)


/********************* Адреса регистров блока IOHARD **************************/
#define E502_REGS_IOHARD_BLOCK                    0x0200
//Адрес Control Table
#define E502_REGS_IOHARD_LTABLE                   (E502_REGS_IOHARD_BLOCK+0)
#define E502_REGS_IOHARD_LTABLE_MAX_SIZE          0x100 // Максимальный размер Control Table

#define E502_REGS_IOHARD_LCH_CNT                  (E502_REGS_IOHARD_BLOCK+0x100)
#define E502_REGS_IOHARD_ADC_FREQ_DIV             (E502_REGS_IOHARD_BLOCK+0x102)
#define E502_REGS_IOHARD_ADC_FRAME_DELAY          (E502_REGS_IOHARD_BLOCK+0x104)
#define E502_REGS_IOHARD_DIGIN_FREQ_DIV           (E502_REGS_IOHARD_BLOCK+0x106)
#define E502_REGS_IOHARD_IO_MODE                  (E502_REGS_IOHARD_BLOCK+0x108)
#define E502_REGS_IOHARD_GO_SYNC_IO               (E502_REGS_IOHARD_BLOCK+0x10A)
#define E502_REGS_IOHARD_PRELOAD_ADC              (E502_REGS_IOHARD_BLOCK+0x10C)
#define E502_REGS_IOHARD_ASYNC_OUT                (E502_REGS_IOHARD_BLOCK+0x112)
#define E502_REGS_IOHARD_LED                      (E502_REGS_IOHARD_BLOCK+0x114)
#define E502_REGS_IOHARD_DIGIN_PULLUP             (E502_REGS_IOHARD_BLOCK+0x116)
#define E502_REGS_IOHARD_OUTSWAP_BFCTL            (E502_REGS_IOHARD_BLOCK+0x118)
#define E502_REGS_IOHARD_OUTSWAP_ERROR            (E502_REGS_IOHARD_BLOCK+0x120)



/********************* Адреса регистров блока IOARITH **************************/
#define E502_REGS_IOARITH_BLOCK                    0x0400
#define E502_REGS_IOARITH_B10                      E502_REGS_IOARITH_BLOCK
#define E502_REGS_IOARITH_B5                       (E502_REGS_IOARITH_BLOCK+0x01)
#define E502_REGS_IOARITH_B2                       (E502_REGS_IOARITH_BLOCK+0x02)
#define E502_REGS_IOARITH_B1                       (E502_REGS_IOARITH_BLOCK+0x03)
#define E502_REGS_IOARITH_B05                      (E502_REGS_IOARITH_BLOCK+0x04)
#define E502_REGS_IOARITH_B02                      (E502_REGS_IOARITH_BLOCK+0x05)
#define E502_REGS_IOARITH_K10                      (E502_REGS_IOARITH_BLOCK+0x08)
#define E502_REGS_IOARITH_K5                       (E502_REGS_IOARITH_BLOCK+0x09)
#define E502_REGS_IOARITH_K2                       (E502_REGS_IOARITH_BLOCK+0x0A)
#define E502_REGS_IOARITH_K1                       (E502_REGS_IOARITH_BLOCK+0x0B)
#define E502_REGS_IOARITH_K05                      (E502_REGS_IOARITH_BLOCK+0x0C)
#define E502_REGS_IOARITH_K02                      (E502_REGS_IOARITH_BLOCK+0x0D)
#define E502_REGS_IOARITH_ADC_FREQ_DIV             (E502_REGS_IOARITH_BLOCK+0x12)
#define E502_REGS_IOARITH_IN_STREAM_ENABLE         (E502_REGS_IOARITH_BLOCK+0x19)
#define E502_REGS_IOARITH_DIN_ASYNC                (E502_REGS_IOARITH_BLOCK+0x1A)


/********************* Адреса регистров блока управления BlackFin'ом **********/
#define E502_REGS_BF_CTL_BLOCK               0
#define E502_REGS_BF_CTL                     (E502_REGS_BF_CTL_BLOCK+0)
#define E502_REGS_BF_CMD                     (E502_REGS_BF_CTL_BLOCK+1)
#define E502_REGS_BF_STATUS                  (E502_REGS_BF_CTL_BLOCK+2)
#define E502_REGS_BF_IRQ                     (E502_REGS_BF_CTL_BLOCK+3)
#define E502_REGS_BF_IRQ_EN                  (E502_REGS_BF_CTL_BLOCK+4)
#define E502_REGS_BF_REQ_ADDR                (E502_REGS_BF_CTL_BLOCK+5)
#define E502_REGS_BF_REQ_SIZE                (E502_REGS_BF_CTL_BLOCK+6)
#define E502_REGS_BF_REQ_DATA                (E502_REGS_BF_CTL_BLOCK+128)

#define E502_BF_REQ_DATA_SIZE_MAX            128
#define E502_BF_REQ_DATA_SIZE_MIN              8




/* описание отдельных битов регистров */

#define E502_REGBIT_ARM_DMA_ADC_BUF_CLR_Pos   0
#define E502_REGBIT_ARM_DMA_ADC_BUF_CLR_Msk   (1UL << E502_REGBIT_ARM_DMA_ADC_BUF_CLR_Pos)

#define E502_REGBIT_ARM_DMA_DAC_BUF_CLR_Pos   1
#define E502_REGBIT_ARM_DMA_DAC_BUF_CLR_Msk   (1UL << E502_REGBIT_ARM_DMA_DAC_BUF_CLR_Pos)

#define E502_REGBIT_ARM_DMA_RING_MODE_Pos     2
#define E502_REGBIT_ARM_DMA_RING_MODE_Msk     (1UL << E502_REGBIT_ARM_DMA_RING_MODE_Pos)


#define E502_REGBIT_BF_STATUS_HWAIT_Pos      0
#define E502_REGBIT_BF_STATUS_HWAIT_Msk      (1UL << E502_REGBIT_BF_STATUS_HWAIT_Pos)

#define E502_REGBIT_BF_STATUS_BUSY_Pos       1
#define E502_REGBIT_BF_STATUS_BUSY_Msk       (1UL << E502_REGBIT_BF_STATUS_BUSY_Pos)



#define E502_REGBIT_BF_CTL_BF_RESET_Pos       1
#define E502_REGBIT_BF_CTL_BF_RESET_Msk       (0x1UL << E502_REGBIT_BF_CTL_BF_RESET_Pos)


#define E502_REGBIT_BF_CTL_HOST_WAIT_Pos      3
#define E502_REGBIT_BF_CTL_HOST_WAIT_Msk      (0x1UL << E502_REGBIT_BF_CTL_HOST_WAIT_Pos)

#define E502_REGBIT_BF_CTL_DSP_MODE_Pos       4
#define E502_REGBIT_BF_CTL_DSP_MODE_Msk       (0x1UL << E502_REGBIT_BF_CTL_DSP_MODE_Pos)

#define E502_REGBIT_BF_CTL_DBG_MODE_Pos       5
#define E502_REGBIT_BF_CTL_DBG_MODE_Msk       (0x1UL << E502_REGBIT_BF_CTL_DBG_MODE_Pos)

#define E502_REGBIT_BF_CTL_CLK_DIV_Pos        8
#define E502_REGBIT_BF_CTL_CLK_DIV_Msk        (0xFUL << E502_REGBIT_BF_CTL_CLK_DIV_Pos)

#define E502_REGBIT_ADC_SLV_CLK_LOCK_Pos      31
#define E502_REGBIT_ADC_SLV_CLK_LOCK_Msk      (0x1UL << E502_REGBIT_ADC_SLV_CLK_LOCK_Pos)

#define E502_REGBIT_IOHARD_OUT_SWAP_Pos     0
#define E502_REGBIT_IOHARD_OUT_SWAP_Msk     (0x1UL << E502_REGBIT_IOHARD_OUT_SWAP_Pos)

#define E502_REGBIT_IOHARD_OUT_TFS_EN_Pos   1
#define E502_REGBIT_IOHARD_OUT_TFS_EN_Msk   (0x1UL << E502_REGBIT_IOHARD_OUT_TFS_EN_Pos)

#define E502_REGBIT_IOHARD_OUT_RING_Pos     2
#define E502_REGBIT_IOHARD_OUT_RING_Msk     (0x1UL << E502_REGBIT_IOHARD_OUT_RING_Pos)

#define E502_REGBIT_IOHARD_OUT_RFS_EN_Pos   3
#define E502_REGBIT_IOHARD_OUT_RFS_EN_Msk   (0x1UL << E502_REGBIT_IOHARD_OUT_RFS_EN_Pos)









#define E502_REGBIT_DMA_IRQ_STEP_Msk(ch)     (1UL << ch)
#define E502_REGBIT_DMA_IRQ_PAGE_Msk(ch)     (1UL << (ch+8))
#define E502_REGBIT_DMA_IRQ_FLUSH_Msk(ch)    (1UL << (ch+16))



#endif // L5XX_REGS_H



#endif // E502_FPGA_REGS_H
