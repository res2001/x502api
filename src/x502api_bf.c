#include "x502api_private.h"

LPCIE_EXPORT(int32_t) X502_BfExecCmd(t_x502_hnd hnd, uint16_t cmd_code, uint32_t par,
                                    const uint32_t* snd_data, uint32_t snd_size,
                                    uint32_t* rcv_data, uint32_t rcv_size, uint32_t tout,
                                    uint32_t* recvd_size) {
    /** @todo */
    return X502_ERR_NOT_IMPLEMENTED;
}
