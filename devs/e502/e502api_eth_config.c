#include "e502api_private.h"
#include "e502_eth_config.h"
#include <string.h>
#include <stdlib.h>

#if E502_ETHCONFIG_MAC_ADDR_SIZE != X502_MAC_ADDR_SIZE
    #error "inconsistent E502_ETHCONFIG_MAC_ADDR_SIZE"
#endif
#if E502_ETHCONFIG_INSTANCE_NAME_SIZE != X502_INSTANCE_NAME_SIZE
    #error "inconsistent E502_ETHCONFIG_INSTANCE_NAME_SIZE"
#endif
#if E502_ETHCONFIG_PASSWD_SIZE != X502_PASSWORD_SIZE
    #error "inconsistent E502_ETHCONFIG_PASSWD_SIZE"
#endif


#define E502_ETH_CFG_SIGN  0xE502CFA5

#define E502_ETH_CHECK_CFG(cfg) ((cfg != NULL) ? (cfg)->sign == E502_ETH_CFG_SIGN ? X502_ERR_OK \
    : X502_ERR_INVALID_CONFIG_HANDLE : X502_ERR_INVALID_CONFIG_HANDLE)



#define IP_ADDR_TO_UINT32(addr, dword) ((dword) = ((uint32_t)addr[0] << 24) | \
                                                 ((uint32_t)addr[1] << 16) | \
                                                 ((uint32_t)addr[2] << 8) | \
                                                  addr[3]);

#define IP_UINT32_TO_ADDR(dword, addr) do { \
                                        addr[0] = (dword >> 24) & 0xFF; \
                                        addr[1] = (dword >> 16) & 0xFF; \
                                        addr[2] = (dword >> 8) & 0xFF; \
                                        addr[3] = (dword) & 0xFF; \
                                      } while(0)


typedef struct st_e502_eth_config_state {
    uint32_t sign;
    uint32_t flags;
    uint8_t  factory_mac[E502_ETHCONFIG_MAC_ADDR_SIZE];
    t_e502_eth_set_config_params params;
} t_e502_eth_config_state;




X502_EXPORT(t_e502_eth_config_hnd) E502_EthConfigCreate(void) {
    t_e502_eth_config_hnd cfg = calloc(1, sizeof(t_e502_eth_config_state));
    if (cfg != NULL) {
        cfg->sign = E502_ETH_CFG_SIGN;
    }
    return cfg;
}

X502_EXPORT(int32_t) E502_EthConfigCopy(t_e502_eth_config_hnd src_cfg, t_e502_eth_config_hnd dst_cfg) {
    int32_t err = E502_ETH_CHECK_CFG(src_cfg);
    if (err == X502_ERR_OK)
        err = E502_ETH_CHECK_CFG(dst_cfg);
    if (err == X502_ERR_OK) {
        memcpy(dst_cfg, src_cfg, sizeof(t_e502_eth_config_state));
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigFree(t_e502_eth_config_hnd cfg) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        memset(cfg, 0, sizeof(t_e502_eth_config_state));
        free(cfg);
    }
    return err;
}


X502_EXPORT(int32_t) E502_EthConfigRead(t_x502_hnd hnd, t_e502_eth_config_hnd cfg) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        err = X502_CHECK_HND_OPENED(hnd);
    }
    if (err == X502_ERR_OK) {
        uint32_t recvd_size;

        err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_ETH_CFG_GET, 0,
                                    NULL, 0, &cfg->params.cfg, sizeof(t_e502_eth_config), &recvd_size, 0);
        if (err == X502_ERR_OK) {
            cfg->flags = 0;
            memcpy(cfg->factory_mac, hnd->info.factory_mac, sizeof(cfg->factory_mac));
        }
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigWrite(t_x502_hnd hnd, t_e502_eth_config_hnd cfg, const char *passwd) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        err = X502_CHECK_HND_OPENED(hnd);
    }
    if (err == X502_ERR_OK) {
        strncpy(cfg->params.passwd, passwd, E502_ETHCONFIG_PASSWD_SIZE);
        cfg->params.passwd[E502_ETHCONFIG_PASSWD_SIZE-1] = '\0';
        err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_ETH_CFG_SET, cfg->flags,
                                    &cfg->params, sizeof(cfg->params),
                                    NULL, 0, NULL, 0);
        memset(cfg->params.passwd, 0, E502_ETHCONFIG_PASSWD_SIZE);
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetNewPassword(t_e502_eth_config_hnd cfg, const char *new_passwd) {
    int32_t err =  E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        cfg->flags |= E502_ETH_CONFIG_FLAGS_SET_NEW_PASSWD;
        strncpy(cfg->params.new_passwd, new_passwd, E502_ETHCONFIG_PASSWD_SIZE);
        cfg->params.new_passwd[E502_ETHCONFIG_PASSWD_SIZE-1] = '\0';
    }
    return err;
}


X502_EXPORT(int32_t) E502_EthConfigGetEnabled(t_e502_eth_config_hnd cfg, uint32_t *en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        *en = cfg->params.cfg.flags & E502_ETH_FLAGS_IFACE_ENABLED;
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetEnabled(t_e502_eth_config_hnd cfg, uint32_t en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        if (en) {
            cfg->params.cfg.flags |= E502_ETH_FLAGS_IFACE_ENABLED;
        } else {
            cfg->params.cfg.flags &= ~E502_ETH_FLAGS_IFACE_ENABLED;
        }
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetAutoIPEnabled(t_e502_eth_config_hnd cfg, uint32_t *en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        *en = cfg->params.cfg.flags & E502_ETH_FLAGS_AUTO_IP;
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetAutoIPEnabled(t_e502_eth_config_hnd cfg, uint32_t en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        if (en) {
            cfg->params.cfg.flags |= E502_ETH_FLAGS_AUTO_IP;
        } else {
            cfg->params.cfg.flags &= ~E502_ETH_FLAGS_AUTO_IP;
        }
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetUserMACEnabled(t_e502_eth_config_hnd cfg, uint32_t *en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        *en = cfg->params.cfg.flags & E502_ETH_FLAGS_USER_MAC;
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetUserMACEnabled(t_e502_eth_config_hnd cfg, uint32_t en) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        if (en) {
            cfg->params.cfg.flags |= E502_ETH_FLAGS_USER_MAC;
        } else {
            cfg->params.cfg.flags &= ~E502_ETH_FLAGS_USER_MAC;
        }
    }
    return err;
}


X502_EXPORT(int32_t) E502_EthConfigGetIPv4Addr(t_e502_eth_config_hnd cfg, uint32_t *addr) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_ADDR_TO_UINT32(cfg->params.cfg.ipv4.addr, *addr);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetIPv4Addr(t_e502_eth_config_hnd cfg, uint32_t addr) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_UINT32_TO_ADDR(addr, cfg->params.cfg.ipv4.addr);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetIPv4Mask(t_e502_eth_config_hnd cfg, uint32_t *mask) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_ADDR_TO_UINT32(cfg->params.cfg.ipv4.mask, *mask);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetIPv4Mask(t_e502_eth_config_hnd cfg, uint32_t mask) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_UINT32_TO_ADDR(mask, cfg->params.cfg.ipv4.mask);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetIPv4Gate(t_e502_eth_config_hnd cfg, uint32_t *gate) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_ADDR_TO_UINT32(cfg->params.cfg.ipv4.gate, *gate);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetIPv4Gate(t_e502_eth_config_hnd cfg, uint32_t gate) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        IP_UINT32_TO_ADDR(gate, cfg->params.cfg.ipv4.gate);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetUserMac(t_e502_eth_config_hnd cfg, uint8_t *mac) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        memcpy(mac, cfg->params.cfg.mac, E502_ETHCONFIG_MAC_ADDR_SIZE);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetUserMac(t_e502_eth_config_hnd cfg, const uint8_t *mac) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        memcpy(cfg->params.cfg.mac, mac, E502_ETHCONFIG_MAC_ADDR_SIZE);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetFactoryMac(t_e502_eth_config_hnd cfg, uint8_t *mac) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK)
        memcpy(mac, cfg->factory_mac, E502_ETHCONFIG_MAC_ADDR_SIZE);
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigGetInstanceName(t_e502_eth_config_hnd cfg, char *name) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        strncpy(name, cfg->params.cfg.inst_name, E502_ETHCONFIG_INSTANCE_NAME_SIZE);
        name[E502_ETHCONFIG_INSTANCE_NAME_SIZE-1] = '\0';
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthConfigSetInstanceName(t_e502_eth_config_hnd cfg, const char *name) {
    int32_t err = E502_ETH_CHECK_CFG(cfg);
    if (err == X502_ERR_OK) {
        strncpy(cfg->params.cfg.inst_name, name, E502_ETHCONFIG_INSTANCE_NAME_SIZE);
        cfg->params.cfg.inst_name[E502_ETHCONFIG_INSTANCE_NAME_SIZE-1] = '\0';
    }
    return err;
}
