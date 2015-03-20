#include "l502api_compat.h"
#include "x502api_private.h"

LPCIE_EXPORT(t_l502_hnd) L502_Create(void) {
    return X502_Create();
}

LPCIE_EXPORT(int32_t) L502_Free(t_l502_hnd hnd) {
    return X502_Free(hnd);
}

LPCIE_EXPORT(int32_t) L502_Close(t_l502_hnd hnd) {
    return X502_Close(hnd);
}

LPCIE_EXPORT(int32_t) L502_GetDevInfo(t_l502_hnd hnd, t_l502_info* info) {
    return X502_GetDevInfo(hnd, (t_x502_info*)info);
}

LPCIE_EXPORT(int32_t) L502_Configure(t_l502_hnd hnd, uint32_t flags) {
    return X502_Configure(hnd, flags);
}

LPCIE_EXPORT(int32_t) L502_SetLChannel(t_l502_hnd hnd, uint32_t lch, uint32_t phy_ch,
                      uint32_t mode, uint32_t range, uint32_t avg) {
    return X502_SetLChannel(hnd, lch, phy_ch, mode, range, avg);
}

LPCIE_EXPORT(int32_t) L502_SetLChannelCount(t_l502_hnd hnd, uint32_t lch_cnt) {
    return X502_SetLChannelCount(hnd, lch_cnt);
}

LPCIE_EXPORT(int32_t) L502_GetLChannelCount(t_l502_hnd hnd, uint32_t* lch_cnt) {
    return X502_GetLChannelCount(hnd, lch_cnt);
}

LPCIE_EXPORT(int32_t) L502_SetAdcFreqDivider(t_l502_hnd hnd, uint32_t adc_freq_div) {
    return X502_SetAdcFreqDivider(hnd, adc_freq_div);
}

LPCIE_EXPORT(int32_t) L502_SetAdcInterframeDelay(t_l502_hnd hnd, uint32_t delay) {
    return X502_SetAdcInterframeDelay(hnd, delay);
}

LPCIE_EXPORT(int32_t) L502_SetDinFreqDivider(t_l502_hnd hnd, uint32_t din_freq_div) {
    return X502_SetDinFreqDivider(hnd, din_freq_div);
}

LPCIE_EXPORT(int32_t) L502_SetAdcFreq(t_l502_hnd hnd, double *f_acq, double *f_frame) {
    return X502_SetAdcFreq(hnd, f_acq, f_frame);
}

LPCIE_EXPORT(int32_t) L502_SetDinFreq(t_l502_hnd hnd, double *f_din) {
    return X502_SetDinFreq(hnd, f_din);
}


LPCIE_EXPORT(int32_t) L502_GetAdcFreq(t_l502_hnd hnd, double *f_acq, double *f_frame) {
    return X502_GetAdcFreq(hnd, f_acq, f_frame);
}

LPCIE_EXPORT(int32_t) L502_SetRefFreq(t_l502_hnd hnd, uint32_t freq) {
    return X502_SetRefFreq(hnd, freq);
}

LPCIE_EXPORT(int32_t) L502_SetSyncMode(t_l502_hnd hnd, uint32_t sync_mode) {
    return X502_SetSyncMode(hnd, sync_mode);
}

LPCIE_EXPORT(int32_t) L502_SetSyncStartMode(t_l502_hnd hnd, uint32_t sync_start_mode) {
    return X502_SetSyncStartMode(hnd, sync_start_mode);
}

LPCIE_EXPORT(int32_t) L502_SetMode(t_l502_hnd hnd, uint32_t mode) {
    return X502_SetMode(hnd, mode);
}

LPCIE_EXPORT(int32_t) L502_GetMode(t_l502_hnd hnd, uint32_t* mode) {
    return X502_GetMode(hnd, mode);
}

LPCIE_EXPORT(int32_t) L502_SetAdcCoef(t_l502_hnd hnd, uint32_t range, double k, double offs) {
    return X502_SetAdcCoef(hnd, range, k, offs);
}

LPCIE_EXPORT(int32_t) L502_GetAdcCoef(t_l502_hnd hnd, uint32_t range, double* k, double* offs) {
    return X502_GetAdcCoef(hnd, range, k, offs);
}

LPCIE_EXPORT(int32_t) L502_SetDacCoef(t_l502_hnd hnd, uint32_t ch, double k, double offs) {
    return X502_SetDacCoef(hnd, ch, k, offs);
}

LPCIE_EXPORT(int32_t) L502_GetDacCoef(t_l502_hnd hnd, uint32_t ch, double* k, double* offs) {
    return X502_GetDacCoef(hnd, ch, k, offs);
}

LPCIE_EXPORT(int32_t) L502_AsyncOutDac(t_l502_hnd hnd, uint32_t ch, double data, uint32_t flags) {
    return X502_AsyncOutDac(hnd, ch, data, flags);
}

LPCIE_EXPORT(int32_t) L502_AsyncOutDig(t_l502_hnd hnd, uint32_t val, uint32_t msk) {
    return X502_AsyncOutDig(hnd, val, msk);
}


LPCIE_EXPORT(int32_t) L502_AsyncInDig(t_l502_hnd hnd, uint32_t* din) {
    return X502_AsyncInDig(hnd, din);
}


LPCIE_EXPORT(int32_t) L502_AsyncGetAdcFrame(t_l502_hnd hnd, uint32_t flags,
                                            uint32_t tout, double* data) {
    return X502_AsyncGetAdcFrame(hnd, flags, tout, data);
}

LPCIE_EXPORT(int32_t) L502_StreamsEnable(t_l502_hnd hnd, uint32_t streams) {
    return X502_StreamsEnable(hnd, streams);
}

LPCIE_EXPORT(int32_t) L502_StreamsDisable(t_l502_hnd hnd, uint32_t streams) {
    return X502_StreamsDisable(hnd, streams);
}

LPCIE_EXPORT(int32_t) L502_StreamsStart(t_l502_hnd hnd) {
    return X502_StreamsStart(hnd);
}

LPCIE_EXPORT(int32_t) L502_StreamsStop(t_l502_hnd hnd) {
    return X502_StreamsStop(hnd);
}

LPCIE_EXPORT(int32_t) L502_IsRunning(t_l502_hnd hnd) {
    return X502_IsRunning(hnd);
}

LPCIE_EXPORT(int32_t) L502_Recv(t_l502_hnd hnd, uint32_t* buf, uint32_t size, uint32_t tout) {
    return X502_Recv(hnd, buf, size, tout);
}

LPCIE_EXPORT(int32_t) L502_Send(t_l502_hnd hnd, const uint32_t* buf, uint32_t size, uint32_t tout) {
    return X502_Send(hnd, buf, size, tout);
}

LPCIE_EXPORT(int32_t) L502_ProcessAdcData(t_l502_hnd hnd, const uint32_t* src,  double *dest,
                                        uint32_t *size, uint32_t flags) {
    return X502_ProcessAdcData(hnd, src, dest, size, flags);
}

LPCIE_EXPORT(int32_t) L502_ProcessData(t_l502_hnd hnd, const uint32_t* src, uint32_t size,
                     uint32_t flags, double *adc_data, uint32_t *adc_data_size,
                     uint32_t *din_data, uint32_t *din_data_size) {
    return X502_ProcessData(hnd, src, size, flags, adc_data, adc_data_size,
                            din_data, din_data_size);
}


LPCIE_EXPORT(int32_t) L502_ProcessDataWithUserExt(t_l502_hnd hnd, const uint32_t* src, uint32_t size,
                                   uint32_t flags, double *adc_data,
                                   uint32_t *adc_data_size, uint32_t *din_data,
                                   uint32_t *din_data_size,
                                   uint32_t *usr_data, uint32_t *usr_data_size) {
    return X502_ProcessDataWithUserExt(hnd, src, size, flags, adc_data, adc_data_size,
                                       din_data, din_data_size, usr_data, usr_data_size);
}

LPCIE_EXPORT(int32_t) L502_PrepareData(t_l502_hnd hnd, const double* dac1, const double* dac2,
                            const uint32_t* digout, uint32_t size, int32_t flags,
                            uint32_t* out_buf) {
    return X502_PrepareData(hnd, dac1, dac2, digout, size, flags, out_buf);
}

LPCIE_EXPORT(int32_t) L502_GetRecvReadyCount(t_l502_hnd hnd, uint32_t *rdy_cnt) {
    return X502_GetRecvReadyCount(hnd, rdy_cnt);
}

LPCIE_EXPORT(int32_t) L502_GetSendReadyCount(t_l502_hnd hnd, uint32_t *rdy_cnt) {
    return X502_GetSendReadyCount(hnd, rdy_cnt);
}

LPCIE_EXPORT(int32_t) L502_GetNextExpectedLchNum(t_l502_hnd hnd, uint32_t *lch) {
    return X502_GetNextExpectedLchNum(hnd, lch);
}

LPCIE_EXPORT(int32_t) L502_PreloadStart(t_l502_hnd hnd) {
    return X502_PreloadStart(hnd);
}

LPCIE_EXPORT(int32_t) L502_OutCycleLoadStart(t_l502_hnd hnd, uint32_t size) {
    return X502_OutCycleLoadStart(hnd, size);
}


LPCIE_EXPORT(int32_t) L502_OutCycleSetup(t_l502_hnd hnd, uint32_t flags) {
    return X502_OutCycleSetup(hnd, flags);
}

LPCIE_EXPORT(int32_t) L502_OutCycleStop(t_l502_hnd hnd, uint32_t flags) {
    return X502_OutCycleStop(hnd, flags);
}

LPCIE_EXPORT(int32_t) L502_SetDmaBufSize(t_l502_hnd hnd, uint32_t dma_ch, uint32_t size) {
    return X502_SetStreamBufSize(hnd, dma_ch, size);
}

LPCIE_EXPORT(int32_t) L502_SetDmaIrqStep(t_l502_hnd hnd, uint32_t dma_ch, uint32_t step) {
    return X502_SetStreamStep(hnd, dma_ch, step);
}

LPCIE_EXPORT(int32_t) L502_BfLoadFirmware(t_l502_hnd hnd, const char* filename) {
    return X502_BfLoadFirmware(hnd, filename);
}

LPCIE_EXPORT(int32_t) L502_BfCheckFirmwareIsLoaded(t_l502_hnd hnd, uint32_t *version) {
    return X502_BfCheckFirmwareIsLoaded(hnd, version);
}

LPCIE_EXPORT(int32_t) L502_BfMemRead(t_l502_hnd hnd, uint32_t addr, uint32_t* regs,
                                uint32_t size) {
    return X502_BfMemRead(hnd, addr, regs, size);
}

LPCIE_EXPORT(int32_t) L502_BfMemWrite(t_l502_hnd hnd, uint32_t addr,
                                        const uint32_t* regs, uint32_t size) {
    return X502_BfMemWrite(hnd, addr, regs, size);
}

LPCIE_EXPORT(int32_t) L502_BfExecCmd(t_l502_hnd hnd, uint16_t cmd_code, uint32_t par,
                const uint32_t* snd_data, uint32_t snd_size,
                uint32_t* rcv_data, uint32_t rcv_size, uint32_t tout, uint32_t* recvd_size) {
    return X502_BfExecCmd(hnd, cmd_code, par, snd_data, snd_size, rcv_data, rcv_size,
                          tout, recvd_size);
}

LPCIE_EXPORT(int32_t) L502_FlashRead(t_l502_hnd hnd, uint32_t addr, uint8_t* data,
                                uint32_t size) {
    return X502_FlashRead(hnd, addr, data, size);
}

LPCIE_EXPORT(int32_t) L502_FlashWrite(t_l502_hnd hnd, uint32_t addr,
                                 const uint8_t* data, uint32_t size) {
    return X502_FlashWrite(hnd, addr, data, size);
}

LPCIE_EXPORT(int32_t) L502_FlashErase(t_l502_hnd hnd, uint32_t addr, uint32_t size) {
    return X502_FlashErase(hnd, addr, size);
}

LPCIE_EXPORT(int32_t) L502_FlashWriteEnable(t_l502_hnd hnd) {
    return X502_FlashWriteEnable(hnd);
}

LPCIE_EXPORT(int32_t) L502_FlashWriteDisable(t_l502_hnd hnd) {
    return X502_FlashWriteDisable(hnd);
}

LPCIE_EXPORT(uint32_t) L502_GetDllVersion(void) {
    return X502_GetLibraryVersion();
}


LPCIE_EXPORT(const char*) L502_GetErrorString(int32_t err) {
    return X502_GetErrorString(err);
}

LPCIE_EXPORT(int32_t) L502_LedBlink(t_l502_hnd hnd) {
    return X502_LedBlink(hnd);
}

LPCIE_EXPORT(int32_t) L502_SetDigInPullup(t_l502_hnd hnd, uint32_t pullups) {
    return X502_SetDigInPullup(hnd, pullups);
}


LPCIE_EXPORT(int32_t) L502_FpgaRegWrite(t_l502_hnd hnd, uint32_t reg, uint32_t val) {
    return X502_FpgaRegWrite(hnd, reg, val);
}

LPCIE_EXPORT(int32_t) L502_FpgaRegRead(t_l502_hnd hnd, uint32_t reg, uint32_t *val) {
    return X502_FpgaRegRead(hnd, reg, val);
}

LPCIE_EXPORT(int32_t) L502_ReloadDevInfo(t_l502_hnd hnd) {
    return X502_ReloadDevInfo(hnd);
}
