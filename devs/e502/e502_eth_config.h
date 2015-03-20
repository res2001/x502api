#ifndef E502_ETH_CONFIG_H
#define E502_ETH_CONFIG_H

#define E502_ETHCONFIG_MAC_ADDR_SIZE        6
#define E502_ETHCONFIG_INSTANCE_NAME_SIZE  64
#define E502_ETHCONFIG_PASSWD_SIZE         32



#define E502_IPV4_ADDR_SIZE 4

typedef enum {
    E502_ETH_FLAGS_IFACE_ENABLED   = 0x0001,
    E502_ETH_FLAGS_AUTO_IP  = 0x0002,
    E502_ETH_FLAGS_USER_MAC = 0x0004
} t_e502_eth_flags;


typedef struct {
    uint8_t addr[E502_IPV4_ADDR_SIZE];
    uint8_t mask[E502_IPV4_ADDR_SIZE];
    uint8_t gate[E502_IPV4_ADDR_SIZE];
} t_e502_ipv4_config;

typedef struct {
    uint32_t format;
    uint32_t flags;
    char     inst_name[E502_ETHCONFIG_INSTANCE_NAME_SIZE];
    uint8_t  mac[E502_ETHCONFIG_MAC_ADDR_SIZE];
    t_e502_ipv4_config ipv4;
    uint16_t tcp_cmd_port;
    uint16_t tcp_data_port;
} t_e502_eth_config;

typedef enum {
    E502_ETH_CONFIG_FLAGS_SET_NEW_PASSWD = 0x0001
} t_e502_eth_config_flags;

typedef struct {
    char     passwd[E502_ETHCONFIG_PASSWD_SIZE];
    char     new_passwd[E502_ETHCONFIG_PASSWD_SIZE];
    t_e502_eth_config cfg;
} t_e502_eth_set_config_params;

#define E502_ETHCONFIG_SET_HDR_SIZE  offsetof(t_e502_eth_set_config_params, cfg)


#endif // E502_ETH_CONFIG_H

