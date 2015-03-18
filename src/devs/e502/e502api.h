#ifndef E502API_H
#define E502API_H

#include "x502api.h"


#ifdef __cplusplus
extern "C" {
#endif


X502_EXPORT(int32_t) E502_OpenUsb(t_x502_hnd hnd, const char* serial);


X502_EXPORT(int32_t) E502_OpenByIpAddr(t_x502_hnd hnd, uint32_t ip_addr,
                                        uint32_t flags, uint32_t tout);

X502_EXPORT(int32_t) E502_MakeDevRecordByIpAddr(t_x502_devrec *devrec, uint32_t ip_addr,
                                                uint32_t flags, uint32_t tout);













/***************************************************************************//**
    @brief Получение списка серийных номеров модулей E502, подключенных по USB.

    Функция возвращает список номеров всех найденных модулей E502, независимо от
    того, открыты они сейчас или нет.

    Если нужен список только тех модулей, которые не открыты (то есть
    только тех, с которыми можно установить соединение), то для этого можно
    передать в функцию флаг #X502_GETDEVS_FLAGS_ONLY_NOT_OPENED.

 @param[in] serials   Массив размером size*#X502_SERIAL_SIZE байт, в который
                      будут сохранены серийные номера найденных модулей.
                      Может быть NULL, если size=0, а devcnt!=NULL, в случае,
                      если нужно только получить количество модулей в системе.
 @param[in] size      Определяет, сколько максимально серийных номеров может
                      быть сохранено в массив serial. Будут сохранены только
                      первые size серийных номеров.
                      Может быть 0, если serials=NULL
 @param[in] flags     Флаги из #t_x502_getdevs_flags, определяющие поведение
                      функции.
 @param[out] devcnt   Если devcnt!=NULL, то в данную переменную сохраняется
                      общее число найденных модулей E502
                      (может быть больше size).
 @return              Если <0 - код ошибки, иначе количество сохраненных
                      серийных номеров в массиве serials (всегда <= size)
*******************************************************************************/
X502_EXPORT(int32_t) E502_UsbGetSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                                           uint32_t flags, uint32_t *devcnt);


X502_EXPORT(int32_t) E502_UsbGetDevRecordsList(t_x502_devrec* list, uint32_t size,
                                               uint32_t flags, uint32_t* devcnt) ;










typedef struct st_e502_eth_config_state* t_e502_eth_config_hnd;




X502_EXPORT(t_e502_eth_config_hnd) E502_EthConfigCreate(void);
X502_EXPORT(int32_t) E502_EthConfigFree(t_e502_eth_config_hnd cfg);
X502_EXPORT(int32_t) E502_GetIpAddr(t_x502_hnd hnd, uint32_t *ip_addr);


X502_EXPORT(int32_t) E502_EthConfigRead(t_x502_hnd hnd, t_e502_eth_config_hnd cfg_hnd);
X502_EXPORT(int32_t) E502_EthConfigWrite(t_x502_hnd hnd, t_e502_eth_config_hnd cfg, const char *passwd);



X502_EXPORT(int32_t) E502_EthConfigGetEnabled(t_e502_eth_config_hnd cfg, int32_t *en);
X502_EXPORT(int32_t) E502_EthConfigSetEnabled(t_e502_eth_config_hnd cfg, int32_t en);
X502_EXPORT(int32_t) E502_EthConfigGetAutoIPEnabled(t_e502_eth_config_hnd cfg, int32_t *en);
X502_EXPORT(int32_t) E502_EthConfigSetAutoIPEnabled(t_e502_eth_config_hnd cfg, int32_t en);
X502_EXPORT(int32_t) E502_EthConfigGetUserMACEnabled(t_e502_eth_config_hnd cfg, int32_t *en);
X502_EXPORT(int32_t) E502_EthConfigSetUserMACEnabled(t_e502_eth_config_hnd cfg, int32_t en);
X502_EXPORT(int32_t) E502_EthConfigGetIPv4Addr(t_e502_eth_config_hnd cfg_hnd, uint32_t *addr);
X502_EXPORT(int32_t) E502_EthConfigSetIPv4Addr(t_e502_eth_config_hnd cfg_hnd, uint32_t addr);
X502_EXPORT(int32_t) E502_EthConfigGetIPv4Mask(t_e502_eth_config_hnd cfg_hnd, uint32_t *mask);
X502_EXPORT(int32_t) E502_EthConfigSetIPv4Mask(t_e502_eth_config_hnd cfg_hnd, uint32_t mask);
X502_EXPORT(int32_t) E502_EthConfigGetIPv4Gate(t_e502_eth_config_hnd cfg_hnd, uint32_t *gate);
X502_EXPORT(int32_t) E502_EthConfigSetIPv4Gate(t_e502_eth_config_hnd cfg_hnd, uint32_t gate);
X502_EXPORT(int32_t) E502_EthConfigGetUserMac(t_e502_eth_config_hnd cfg, uint8_t *mac);
X502_EXPORT(int32_t) E502_EthConfigSetUserMac(t_e502_eth_config_hnd cfg, const uint8_t *mac);
X502_EXPORT(int32_t) E502_EthConfigGetFactoryMac(t_e502_eth_config_hnd cfg, uint8_t *mac);
X502_EXPORT(int32_t) E502_EthConfigGetInstanceName(t_e502_eth_config_hnd cfg, char *name);
X502_EXPORT(int32_t) E502_EthConfigSetInstanceName(t_e502_eth_config_hnd cfg, const char *name);

X502_EXPORT(int32_t) E502_EthConfigSetNewPassword(t_e502_eth_config_hnd cfg, const char *new_passwd);


X502_EXPORT(int32_t) E502_SwitchToBootloader(t_x502_hnd hnd);


#ifdef __cplusplus
}
#endif

#endif // E502API_H
