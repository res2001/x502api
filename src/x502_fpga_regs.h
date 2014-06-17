#ifndef X502_FPGA_REGS_H
#define X502_FPGA_REGS_H

#define X502_BF_SDRAM_SIZE (32UL*1024*1024)

#define X502_BF_MEMADDR_CMD      0xFF800800


#define X502_BF_CMD_READ          0x0001
#define X502_BF_CMD_WRITE         0x0002
#define X502_BF_CMD_HIRQ          0x0004
#define X502_BF_CMD_HDMA_RST      0x0008




/********************* Адреса регистров блока IOHARD **************************/
#define X502_REGS_IOHARD_BLOCK                    0x0200
//Адрес Control Table
#define X502_REGS_IOHARD_LTABLE                   (X502_REGS_IOHARD_BLOCK+0)
#define X502_REGS_IOHARD_LTABLE_MAX_SIZE          0x100 // Максимальный размер Control Table

#define X502_REGS_IOHARD_LCH_CNT                  (X502_REGS_IOHARD_BLOCK+0x100)
#define X502_REGS_IOHARD_ADC_FREQ_DIV             (X502_REGS_IOHARD_BLOCK+0x102)
#define X502_REGS_IOHARD_ADC_FRAME_DELAY          (X502_REGS_IOHARD_BLOCK+0x104)
#define X502_REGS_IOHARD_DIGIN_FREQ_DIV           (X502_REGS_IOHARD_BLOCK+0x106)
#define X502_REGS_IOHARD_IO_MODE                  (X502_REGS_IOHARD_BLOCK+0x108)
#define X502_REGS_IOHARD_GO_SYNC_IO               (X502_REGS_IOHARD_BLOCK+0x10A)
#define X502_REGS_IOHARD_PRELOAD_ADC              (X502_REGS_IOHARD_BLOCK+0x10C)
#define X502_REGS_IOHARD_ASYNC_OUT                (X502_REGS_IOHARD_BLOCK+0x112)
#define X502_REGS_IOHARD_LED                      (X502_REGS_IOHARD_BLOCK+0x114)
#define X502_REGS_IOHARD_DIGIN_PULLUP             (X502_REGS_IOHARD_BLOCK+0x116)
#define X502_REGS_IOHARD_OUTSWAP_BFCTL            (X502_REGS_IOHARD_BLOCK+0x118)
#define X502_REGS_IOHARD_OUTSWAP_ERROR            (X502_REGS_IOHARD_BLOCK+0x120)



/********************* Адреса регистров блока IOARITH **************************/
#define X502_REGS_IOARITH_BLOCK                    0x0400
#define X502_REGS_IOARITH_B10                      X502_REGS_IOARITH_BLOCK
#define X502_REGS_IOARITH_B5                       (X502_REGS_IOARITH_BLOCK+0x01)
#define X502_REGS_IOARITH_B2                       (X502_REGS_IOARITH_BLOCK+0x02)
#define X502_REGS_IOARITH_B1                       (X502_REGS_IOARITH_BLOCK+0x03)
#define X502_REGS_IOARITH_B05                      (X502_REGS_IOARITH_BLOCK+0x04)
#define X502_REGS_IOARITH_B02                      (X502_REGS_IOARITH_BLOCK+0x05)
#define X502_REGS_IOARITH_K10                      (X502_REGS_IOARITH_BLOCK+0x08)
#define X502_REGS_IOARITH_K5                       (X502_REGS_IOARITH_BLOCK+0x09)
#define X502_REGS_IOARITH_K2                       (X502_REGS_IOARITH_BLOCK+0x0A)
#define X502_REGS_IOARITH_K1                       (X502_REGS_IOARITH_BLOCK+0x0B)
#define X502_REGS_IOARITH_K05                      (X502_REGS_IOARITH_BLOCK+0x0C)
#define X502_REGS_IOARITH_K02                      (X502_REGS_IOARITH_BLOCK+0x0D)
#define X502_REGS_IOARITH_ADC_FREQ_DIV             (X502_REGS_IOARITH_BLOCK+0x12)
#define X502_REGS_IOARITH_IN_STREAM_ENABLE         (X502_REGS_IOARITH_BLOCK+0x19)
#define X502_REGS_IOARITH_DIN_ASYNC                (X502_REGS_IOARITH_BLOCK+0x1A)


/********************* Адреса регистров блока управления BlackFin'ом **********/
#define X502_REGS_BF_CTL_BLOCK               0
#define X502_REGS_BF_CTL                     (X502_REGS_BF_CTL_BLOCK+0)
#define X502_REGS_BF_CMD                     (X502_REGS_BF_CTL_BLOCK+1)
#define X502_REGS_BF_STATUS                  (X502_REGS_BF_CTL_BLOCK+2)
#define X502_REGS_BF_IRQ                     (X502_REGS_BF_CTL_BLOCK+3)
#define X502_REGS_BF_IRQ_EN                  (X502_REGS_BF_CTL_BLOCK+4)
#define X502_REGS_BF_REQ_ADDR                (X502_REGS_BF_CTL_BLOCK+5)
#define X502_REGS_BF_REQ_SIZE                (X502_REGS_BF_CTL_BLOCK+6)
#define X502_REGS_BF_REQ_DATA                (X502_REGS_BF_CTL_BLOCK+128)

#define X502_BF_REQ_DATA_SIZE_MAX            128
#define X502_BF_REQ_DATA_SIZE_MIN              8




/* описание отдельных битов регистров */

#define X502_REGBIT_BF_STATUS_HWAIT_Pos      0
#define X502_REGBIT_BF_STATUS_HWAIT_Msk      (1UL << X502_REGBIT_BF_STATUS_HWAIT_Pos)

#define X502_REGBIT_BF_STATUS_BUSY_Pos       1
#define X502_REGBIT_BF_STATUS_BUSY_Msk       (1UL << X502_REGBIT_BF_STATUS_BUSY_Pos)



#define X502_REGBIT_BF_CTL_BF_RESET_Pos       1
#define X502_REGBIT_BF_CTL_BF_RESET_Msk       (0x1UL << X502_REGBIT_BF_CTL_BF_RESET_Pos)


#define X502_REGBIT_BF_CTL_HOST_WAIT_Pos      3
#define X502_REGBIT_BF_CTL_HOST_WAIT_Msk      (0x1UL << X502_REGBIT_BF_CTL_HOST_WAIT_Pos)

#define X502_REGBIT_BF_CTL_DSP_MODE_Pos       4
#define X502_REGBIT_BF_CTL_DSP_MODE_Msk       (0x1UL << X502_REGBIT_BF_CTL_DSP_MODE_Pos)

#define X502_REGBIT_BF_CTL_DBG_MODE_Pos       5
#define X502_REGBIT_BF_CTL_DBG_MODE_Msk       (0x1UL << X502_REGBIT_BF_CTL_DBG_MODE_Pos)

#define X502_REGBIT_BF_CTL_CLK_DIV_Pos        8
#define X502_REGBIT_BF_CTL_CLK_DIV_Msk        (0xFUL << X502_REGBIT_BF_CTL_CLK_DIV_Pos)

#define X502_REGBIT_ADC_SLV_CLK_LOCK_Pos      31
#define X502_REGBIT_ADC_SLV_CLK_LOCK_Msk      (0x1UL << X502_REGBIT_ADC_SLV_CLK_LOCK_Pos)

#define X502_REGBIT_IOHARD_OUT_SWAP_Pos     0
#define X502_REGBIT_IOHARD_OUT_SWAP_Msk     (0x1UL << X502_REGBIT_IOHARD_OUT_SWAP_Pos)

#define X502_REGBIT_IOHARD_OUT_TFS_EN_Pos   1
#define X502_REGBIT_IOHARD_OUT_TFS_EN_Msk   (0x1UL << X502_REGBIT_IOHARD_OUT_TFS_EN_Pos)

#define X502_REGBIT_IOHARD_OUT_RING_Pos     2
#define X502_REGBIT_IOHARD_OUT_RING_Msk     (0x1UL << X502_REGBIT_IOHARD_OUT_RING_Pos)

#define X502_REGBIT_IOHARD_OUT_RFS_EN_Pos   3
#define X502_REGBIT_IOHARD_OUT_RFS_EN_Msk   (0x1UL << X502_REGBIT_IOHARD_OUT_RFS_EN_Pos)

#endif // E502_FPGA_REGS_H
