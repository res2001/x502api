/***************************************************************************//**
 @file lcspec.h

 Файл содержит константы и типы, которые использует как загрузчик lboot,
    так и приложение:
      - константы и типы для запроса на перепрошивку со стороны application
      - константы и типы для определения информации о прошивке, которая хранится
          внутри прошивки и проверяется bootloader'ом

 !!Файл должен быть один (или одинаковый) для обоих проектов!!

 @date 16.11.2010
 @author: Borisov Alexey <borisov@lcard.ru>
 ******************************************************************************/

#ifndef LBOOT_REQ_H_
#define LBOOT_REQ_H_


/** адрес, по которому должен быть сформирован запрос на перепрошивку */
#define LBOOT_REQ_ADDR                0x10000000
/** Адрес, начиная с кторого хранится информация о загрузчике */
#define LBOOT_INFO_ADDR               0x200

/******************* размеры полей ***************************************/
#define LBOOT_DEVNAME_SIZE            32
#define LBOOT_SERIAL_SIZE             32
#define LBOOT_SOFTVER_SIZE            32
#define LBOOT_REVISION_SIZE           16
#define LBOOT_IMPLEMENTATION_SIZE     16
#define LBOOT_SPECINFO_SIZE           64

#define LBOOT_MAC_ADDR_SIZE            6

#define LBOOT_REQ_MAX_FILENAME_SIZE   256
#define LBOOT_REQ_MAX_SIZE            4096


/** различные коды режимов загрузки */
#define LBOOT_BOOTMODE_MODBUS_RTU    0x100
#define LBOOT_BOOTMODE_TFTP_CLIENT   0x200
#define LBOOT_BOOTMODE_TFTP_SERVER   0x201
#define LBOOT_BOOTMODE_USB           0x300
#define LBOOT_BOOTMODE_CAN_LSS       0x400

#define LBOOT_BOOTMODE_TFTP          LBOOT_BOOTMODE_TFTP_CLIENT

//режим работы устройства (application/bootloader) - в modbus сразу за device info
#define LBOOT_MODE_BOOT                0x1
#define LBOOT_MODE_APPL                0x2

#define LBOOT_TFTP_FLAGS_PACKED        0x2


#define LBOOT_SEED_CODE_NOSIGN   0xFFAA5500


/** признак, находящийся в информации о прошивке, по которому можно узнать
    наличие этой информации. Признак означает, что поддерживается защита этой
    информации CRC */
#define LBOOT_INFO_SIGNATURE 0x4C42A55A




/** общие для всех интерфейсов флаги, определяющие поведение загрузчика */
typedef enum
{
    /** - при активном запросе прошивки (tftp) - признак,
          что перезависывется резервная копия, а не основная прошивка
        - при пассивном приеме (mbrtu, usb, can_lss) - признак,
          что разрешено перезаписывать резервную копию */
    LBOOT_REQ_FLAGS_RECOVERY_WR       = 0x01,
    /** признак, что не нужно сверять имя устройства в прошивке */
    LBOOT_REQ_FLAGS_DONT_CHECK_NAME   = 0x02,
    /** не проверять, что записывается стабильная прошивка (установлен флаг
    #LBOOT_APP_FLAGS_STABLE в информации о прошивке)*/
    LBOOT_REQ_FLAGS_DONT_CHECK_STABLE = 0x04,
    /** разрешение записывать прошивку для разработчика (с установленным флагом
    #LBOOT_APP_FLAGS_DEVELOP в информации о прошивке)*/
    LBOOT_REQ_FLAGS_ENABLE_DEVELOP    = 0x08,
    /** разрешение записи прошивки без подписи (если поддерживается загрузчиком) */
    LBOOT_REQ_FLAGS_ENABLE_NO_SIGN    = 0x10
} t_lboot_req_flags;

/** флаги из #t_app_info */
typedef enum
{
    /** признак, что прошивка стабильная */
    LBOOT_APP_FLAGS_STABLE     = 0x1,
    /** признак, что это прошивка для разработчиков */
    LBOOT_APP_FLAGS_DEVELOP    = 0x2
} t_lboot_app_flags;




/** Информация о программе, содержащаяся в прошивке */
struct st_app_info
{
    uint32_t size; /**< размер информции о прошивке */
    uint32_t flags; /**< флаги */
    /** название устройства, для которого предназначена прошивка*/
    char devname[LBOOT_DEVNAME_SIZE];
} __attribute__ ((packed));
typedef struct st_app_info t_app_info;




/** Информация о загрузчике, находящаяся непосредственно
    в коде самого загрузчика */
struct lboot_info_st
{
    /** версия загрузчика (младший байт - минорная, старший - мажорная) */
    uint16_t ver;
    uint16_t flags; /**< флаги - резерв */
    /** серийный номер устройства */
    char serial[LBOOT_SERIAL_SIZE];
    uint8_t mac[6];  /**< mac-адрес устройства */
    /** ревизия платы (если вшита в загрузчик)*/
    char brd_revision[LBOOT_REVISION_SIZE];
    /** модификация (если вшита в загрузчик)*/
    char brd_impl[LBOOT_IMPLEMENTATION_SIZE];
    char creation_date[18]; /**< дата создания загрузчика */
    /** признак, что эта структура действительна (только с v1.3) */
    uint32_t sign;
    /** размер структуры (включая два байта crc)
        - для определения положения crc */
    uint32_t size;
    char devname[LBOOT_DEVNAME_SIZE];
    /** поддерживаемые возможности загрузчика */
    uint32_t features;
    uint32_t reserv;
    uint8_t ip_addr[4]; /**< ip-адрес устройства по-умолчанию */
    uint8_t ip_mask[4]; /**< маска устройства по-умолчанию */
    uint8_t gate[4];    /**< адрес шлюза по-умолчанию */
    /** crc (но при проверке следует брать ее не отсюда,
               а по адресу = адрес начала структуры + size - 2) */
    uint16_t crc;
} __attribute__ ((packed));
typedef struct lboot_info_st t_lboot_info;

/** информация о устройстве и прошивке  */
struct lboot_devinfo_st
{
    char devname[LBOOT_DEVNAME_SIZE]; /**< название устройства */
    char serial[LBOOT_SERIAL_SIZE];   /**< серийный номер */
    char soft_ver[LBOOT_SOFTVER_SIZE]; /**< версия прошивки */
    char brd_revision[LBOOT_REVISION_SIZE]; /**< ревизия платы */
    char brd_impl[LBOOT_IMPLEMENTATION_SIZE]; /**< опции платы */
    char spec_info[LBOOT_SPECINFO_SIZE]; /**< резерв */
} __attribute__ ((packed));
typedef struct lboot_devinfo_st t_lboot_devinfo;


/** стандартный заголовок запроса на перепрошивку */
struct lboot_params_hdr_st
{
    uint32_t size;   /**< размер структуры запроса (включая
                         специфические для интерфейса данные и crc) */
    uint32_t bootmode;  /**< режим загрузки - определяет интерфейс */
    uint32_t flags;     /**< общие флаги */
    uint32_t timeout;  /**< таймаут в мс - сколько загрузчик будет ожидать
                            запроса на перепрошивку (0 - бесконечно) */
    uint32_t reserv[2]; /**< резерв */
    t_lboot_devinfo devinfo; /**< информация о устройстве и прошивке */
} __attribute__ ((packed));
typedef struct lboot_params_hdr_st t_lboot_params_hdr;


//специфические для tftp параметры
struct st_lboot_specpar_tftp
{
    uint16_t flags;  //спец флаги (резерв)
    uint16_t server_port;
    uint8_t mac[6]; //mac-адрес
    uint8_t l_ip[4]; //свой ip-адрес
    uint8_t mask[4]; //mask - маска сети
    uint8_t gate[4]; //адрес шлюза
    uint8_t r_ip[4]; //адрес сервера tftp
    char filename[LBOOT_REQ_MAX_FILENAME_SIZE]; //имя файла с прошивкой
    uint16_t crc;
} __attribute__ ((packed));
typedef struct st_lboot_specpar_tftp t_lboot_specpar_tftp;

//параметры для запроса прошивки по tftp
struct st_lboot_params_tftp
{
    t_lboot_params_hdr hdr;
    uint16_t tftp_flags;  //спец флаги (резерв)
    uint16_t server_port;
    uint8_t mac[6]; //mac-адрес
    uint8_t l_ip[4]; //свой ip-адрес
    uint8_t mask[4]; //mask - маска сети
    uint8_t gate[4]; //адрес шлюза
    uint8_t r_ip[4]; //адрес сервера tftp
    char filename[LBOOT_REQ_MAX_FILENAME_SIZE]; //имя файла с прошивкой
    uint16_t crc;
} __attribute__ ((packed));
typedef struct st_lboot_params_tftp t_lboot_params_tftp;



struct st_lboot_specpar_modbus_rtu
{
   uint16_t flags ; //спец флаги
   uint8_t addr ; //адрес устройства в modbus сети
   uint8_t parity ;
   uint16_t res ;
   uint32_t baud_rate; //скорость передачи по rs-485/232
   uint16_t crc;
}  __attribute__ ((packed));
typedef struct st_lboot_specpar_modbus_rtu t_lboot_specpar_modbus_rtu;

//параметры для перепрошивки по Modbus RTU
struct st_lboot_params_modbus_rtu
{
    t_lboot_params_hdr hdr ;
    uint16_t flags ; //спец флаги
    uint8_t addr ; //адрес устройства в modbus сети
    uint8_t parity ;
    uint16_t res ;
    uint32_t baud_rate; //скорость передачи по rs-485/232
    uint16_t crc;
} __attribute__ ((packed));
typedef struct st_lboot_params_modbus_rtu t_lboot_params_modbus_rtu;



struct st_lboot_specpar_usb
{
    uint16_t flags ; //спец флаги
    uint16_t crc;
} __attribute__ ((packed));
typedef struct st_lboot_specpar_usb t_lboot_specpar_usb;

//параметры для перепрошивки по USB
struct st_lboot_params_usb
{
    t_lboot_params_hdr hdr ;
    uint16_t flags ; //спец флаги
    uint16_t crc;
} __attribute__ ((packed));
typedef struct st_lboot_params_usb t_lboot_params_usb;

struct st_lboot_specpar_can_lss
{
    uint16_t flags;
    uint8_t br_index; /* BaudRate Index */
    uint8_t res;
    uint32_t vid;
    uint32_t pid;
    uint32_t rev;
    uint16_t crc;
}__attribute__ ((packed));
typedef struct st_lboot_specpar_can_lss t_lboot_specpar_can_lss;

struct st_lboot_params_can_lss
{
    t_lboot_params_hdr hdr;
    t_lboot_specpar_can_lss can;
} __attribute__ ((packed));
typedef struct st_lboot_params_can_lss t_lboot_params_can_lss;



struct st_lboot_params
{
    t_lboot_params_hdr hdr ;
    union
    {
        t_lboot_specpar_tftp tftp;
        t_lboot_specpar_modbus_rtu mbrtu;
        t_lboot_specpar_usb usb;
        t_lboot_specpar_can_lss can;
    };
} __attribute__ ((packed));

typedef struct st_lboot_params t_lboot_params;




#endif /* LBOOT_REQ_H_ */
