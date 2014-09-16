#ifndef E502_TCP_PROTOCOL_H
#define E502_TCP_PROTOCOL_H

//значения сигнатуры, которое является признаком действительной команды или ответа
#define E502_TCP_CMD_SIGNATURE  0x314C5443
//заголово команды
typedef struct {
    uint32_t sign;
    uint32_t cmd; //код команды (реально используются 0-255)
    uint32_t par; //параметр (соответствует value и index в usb)
    uint32_t data_len; //кол-во данных на передачу в байтах
    uint32_t resp_len; //кол-во ожидаем данных на прием в байтах
} t_e502_tcp_cmd_hdr;

//заголовок ответа
typedef struct {
    uint32_t sign;
    int32_t res;  //код возврата (0 - ОК, <0 - ошибка)
    uint32_t len; //кол-во данных в ответе
} t_e502_tcp_resp_hdr;

#define E502_TCP_CMD_HDR_SIZE sizeof(t_e502_tcp_cmd_hdr)
#define E502_TCP_CMD_RESP_SIZE sizeof(t_e502_tcp_resp_hdr)


#define E502_TCP_CMD_RX_DATA_SIZE_MAX   512
#define E502_TCP_CMD_TX_DATA_SIZE_MAX   512


#define E502_TCP_DEFAULT_CMD_PORT   11113
#define E502_TCP_DEFAULT_DATA_PORT  11110


#endif // E502_TCP_PROTOCOL_H
