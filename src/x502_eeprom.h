/***************************************************************************//**
  @file x502_eeprom.h
  @author Borisov Alexey <borisov@lcard.ru>
  @date   14.03.2011

   Файл содержет описание констант и типов, для описание формата информации,
   записаной в EEPROM. Эти константы используются только внутри библиотеки и
   в специальном софте (для калибровки и обновлениия прошивки ПЛИС) и не
   доступны пользователю по-умолчанию
 ******************************************************************************/
#ifndef X502_EEPROM_H
#define X502_EEPROM_H


#define X502_EEPROM_CBR_SIGN   0x4C434352 //LCBR
#define X502_EEPROM_CBR_FROMAT 2

#define X502_EEPROM_CBR_SRC_ADC  1
#define X502_EEPROM_CBR_SRC_DAC  2


#define X502_EEPROM_SIZE        0x200000UL

#define X502_EEPROM_ADDR_DESCR  0x1F0000UL
#define X502_DESCR_MAX_SIZE     0x010000UL





#define X502_EEPROM_FORMAT      1
#define X502_DESCR_MIN_SIZE   (sizeof(t_x502_eeprom_hdr)+4)
#define X502_EEPROM_SIGN      0x4C524F4D


typedef struct {
    uint32_t cbr_sign;
    uint32_t cbr_size;
    uint32_t format;
    uint32_t src;
    uint32_t flags;
    uint32_t reserv[3];
    uint64_t time;
    uint32_t channel_cnt;
    uint32_t range_cnt;        
} t_x502_eeprom_cbr_hdr;

typedef struct {
    t_x502_eeprom_cbr_hdr hdr;
    t_x502_cbr_coef coefs[X502_ADC_RANGE_CNT];
} t_x502_eeprom_cbr_adc;

typedef struct {
    t_x502_eeprom_cbr_hdr hdr;
    t_x502_cbr_coef coefs[X502_DAC_CH_CNT];
} t_x502_eeprom_cbr_dac;

typedef struct {
    uint32_t sign;
    uint32_t size;
    uint32_t format;
    char name[X502_DEVNAME_SIZE];
    char serial[X502_SERIAL_SIZE];
    uint8_t mac[6];
    char res[64-18];
} t_x502_eeprom_hdr;


typedef struct {
    t_x502_eeprom_hdr hdr;
    t_x502_eeprom_cbr_adc cbr_adc;
    t_x502_eeprom_cbr_dac cbr_dac;
    uint32_t crc;
} t_x502_descr;


typedef enum {
    X502_EEPROM_PROT_ALL          = 0, /**< Защищена вся область памяти */
    X502_EEPROM_PROT_WR_USER      = 1, /**< Разрешено изменение только пользовательской части */
    X502_EEPROM_PROT_WR_SETTINGS  = 2, /**< Кроме пользовательской части, разрешено изменение
                                           области настроек, флага загрузки и второй копии прошивки ПЛИС */
    X502_EEPROM_PROT_WR_FPGA_FIRM = 3, /**< Разрешено изменение основной прошивки ПЛИС */
    X502_EEPROM_PROT_WR_ALL       = 4, /**< Разрешена запись в любую область памяти */
} t_x502_eeprom_prot_state;
#endif // X502_EEPROM_H
