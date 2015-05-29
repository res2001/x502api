#ifndef E502API_TCP_PRIVATE_H
#define E502API_TCP_PRIVATE_H

#include "e502api_private.h"

typedef struct {
    uint16_t cmd_port;
    uint32_t ip_addr;
    uint32_t open_tout;
    uint32_t flags;
    t_e502_eth_svc_record_hnd svc_rec;
} t_tcp_devinfo_data;

int32_t e502_make_tcp_rec(t_x502_devrec *devrec, uint32_t flags, uint32_t tout);
int32_t e502_svc_fill_devinfo(t_tcp_devinfo_data *data);

#endif // E502API_TCP_PRIVATE_H

