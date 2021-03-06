#ifdef ENABLE_USB
#include "libusb-1.0/libusb.h"
#include "e502api_private.h"
#include "lboot_req.h"
#include "e502_fpga_regs.h"
#include "osspec.h"
#include "ltimer.h"

#include <stdlib.h>
#include <string.h>

#define E502_USB_VID        0x2A52
#define E502_USB_PID        0xE502
#define E502_USB_REQ_TOUT   3000

#define USB_CTL_REQ_MAX_SIZE 512


#define MUTEX_STREAM_LOCK_TOUT  2000


#define STREAM_STOP_TOUT      1000

#define USB_IN_STREAM_BUF_MIN   64


#define USB_BULK_RX_MAX_TRANSF_CNT          20
#define USB_BULK_RX_MAX_TRANSF_SIZE         (32*1024)
#define USB_BULK_RX_TRANSFER_TOUT           -1

#define USB_BULK_TX_MAX_TRANSF_CNT          20
#define USB_BULK_TX_MAX_TRANSF_SIZE         (32*1024)
#define USB_BULK_TX_TRANSFER_TOUT           -1

#ifndef LIBUSB_CALL
    #define LIBUSB_CALL
#endif

static int f_lusb_init_done = 0;


struct st_transf_info;


typedef enum {
    TRANSF_CPL_BUSY = 0,
    TRANSF_CPL_COMPLETED  = 1,
    TRANSF_CPL_IDLE       = 2
} t_transf_state;

typedef struct {
    uint32_t *addr;
    uint32_t size;
    uint32_t transf_pos;
    volatile enum {
        RX_STATE_IDLE,
        RX_STATE_BUSY,
        RX_STATE_RDY,
        RX_STATE_ERR
    } state;
} t_rx_cpl_part; /* результат выполнения запросов */

typedef struct st_transf_info {
    t_x502_hnd dev;
    uint8_t addr; /* адрес конечной точки */
    uint16_t pkt_size;
    t_mutex mutex;
    t_thread thread;
    t_event  user_wake_evt;
    t_event  usb_wake_evt;

    volatile int stop_req;
    int usb_rdy;
    uint32_t *data; /* промежуточный буфер, используемый во всех запросах */
    unsigned buf_size;


    int err;

    union {
        struct {
            t_rx_cpl_part *cpls;
            uint32_t cpl_cnt;
            uint32_t cpl_get_pos;
            uint32_t transf_size;
            uint32_t buf_get_rdy;
        } rx;
        struct {
            uint32_t buf_pos_put;
            uint32_t buf_put_rdy;
            uint32_t busy_size;
        } tx;
    };
} t_transf_info;


typedef struct {
    libusb_device_handle *devhnd;
    t_transf_info streams[X502_STREAM_CH_CNT];
} t_usb_iface_data;


static int32_t f_ioreq(libusb_device_handle *handle, uint32_t cmd_code, uint32_t param,
                   const void* snd_data, uint32_t snd_size,
                   void* rcv_data, uint32_t recv_size, uint32_t* recvd_size, uint32_t tout);

static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr);
static int32_t f_iface_open(t_x502_hnd hnd, const t_x502_devrec *devrec);
static int32_t f_iface_close(t_x502_hnd hnd);
static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params);
static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t flags);
static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch, uint32_t flags);
static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch, uint32_t flags);
static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout);
static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) ;
static int32_t f_iface_stream_get_rdy_cnt(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_cnt);

static int32_t f_iface_gen_ioctl(t_x502_hnd hnd, uint32_t cmd_code, uint32_t param,
                                 const void* snd_data, uint32_t snd_size,
                                 void* rcv_data, uint32_t recv_size, uint32_t* recvd_size,
                                 uint32_t tout);

static const t_x502_dev_iface f_usb_iface = {
    E502_REGS_ARM_HARD_ID,
    USB_IN_STREAM_BUF_MIN,
    USB_CTL_REQ_MAX_SIZE,
    USB_CTL_REQ_MAX_SIZE/4,
    USB_CTL_REQ_MAX_SIZE, //flash rd size
    USB_CTL_REQ_MAX_SIZE, //flash wr size
    f_iface_free_devinfo_ptr,
    f_iface_open,
    f_iface_close,
    e502_iface_fpga_read,
    e502_iface_fpga_write,
    f_iface_stream_cfg,
    f_iface_stream_start,
    f_iface_stream_stop,
    f_iface_stream_free,
    e502_iface_stream_running,
    f_iface_stream_read,
    f_iface_stream_write,
    f_iface_stream_get_rdy_cnt,
    e502_iface_bf_mem_block_rd,
    e502_iface_bf_mem_block_wr,
    e502_iface_bf_firm_load,
    e502_iface_flash_rd,
    e502_iface_flash_wr,
    e502_iface_flash_erase,
    e502_iface_flash_set_prot,
    e502_iface_reload_dev_info,
    e502_iface_cycle_load_start,
    e502_iface_cycle_setup,
    e502_iface_cycle_stop,
    e502_iface_cycle_check_setup,
    e502_iface_fpga_mode_init,
    f_iface_gen_ioctl,
    e502_iface_check_feature
};






static int32_t f_iface_open(t_x502_hnd hnd, const t_x502_devrec *devrec) {
    int32_t err, usberr;
    libusb_device *dev = (libusb_device *)devrec->internal->iface_data;
    libusb_device_handle *devhnd = NULL;


    usberr = libusb_open(dev, &devhnd);
    if (!usberr) {
        usberr = libusb_claim_interface(devhnd, 0);
    }

    err = usberr == LIBUSB_SUCCESS ? X502_ERR_OK :
          usberr == LIBUSB_ERROR_BUSY ? X502_ERR_ALREADY_OPENED :
          usberr == LIBUSB_ERROR_ACCESS ? X502_ERR_DEVICE_ACCESS_DENIED :
          usberr == LIBUSB_ERROR_NO_DEVICE ? X502_ERR_DEVICE_DISCONNECTED :
                     X502_ERR_DEVICE_OPEN;

    if (err == X502_ERR_OK) {
        t_lboot_devinfo lboot_info;

        err = f_ioreq(devhnd, E502_CM4_CMD_GET_MODULE_INFO, 0, NULL, 0, &lboot_info,
                              sizeof(lboot_info), NULL, 0);
        if (err == X502_ERR_OK) {
            if (strcmp(lboot_info.devname, devrec->devname)) {
                err = X502_ERR_INVALID_DEVICE;
            } else {
                e502_devinfo_init(&hnd->info, &lboot_info);
            }
        }
    }

    if (err == X502_ERR_OK) {
        t_usb_iface_data *usb_data = calloc(1, sizeof(t_usb_iface_data));
        unsigned stream;
        struct libusb_config_descriptor* cfg;

        usb_data->devhnd = devhnd;

        /* Чтобы определить номера используемых конечных точек, получаем
         * конфигурацию устройства и находим по одной конечной точки типа
         * bulk на каждое направление (in/out) */
        usberr = libusb_get_config_descriptor(dev, 0, &cfg);
        if (!usberr) {
            int intf_idx, s, fnd_in=0, fnd_out=0;
            for (intf_idx = 0; intf_idx < cfg->bNumInterfaces; intf_idx++) {
                for (s=0; s < cfg->interface[intf_idx].num_altsetting; s++) {
                    int e;
                    for (e=0; e < cfg->interface[intf_idx].altsetting[s].bNumEndpoints; e++) {
                        const struct libusb_endpoint_descriptor *ep_descr =
                                &cfg->interface[intf_idx].altsetting[s].endpoint[e];
                        if ((ep_descr->bmAttributes & 0x3)==
                                LIBUSB_TRANSFER_TYPE_BULK ) {
                            if ((ep_descr->bEndpointAddress & 0x80)
                                    == LIBUSB_ENDPOINT_IN) {
                                if (!fnd_in) {
                                     usb_data->streams[X502_STREAM_CH_IN].addr = ep_descr->bEndpointAddress;
                                     usb_data->streams[X502_STREAM_CH_IN].pkt_size = ep_descr->wMaxPacketSize;
                                     fnd_in = 1;
                                }
                            } else {
                                if (!fnd_out) {
                                    usb_data->streams[X502_STREAM_CH_OUT].addr = ep_descr->bEndpointAddress;
                                    usb_data->streams[X502_STREAM_CH_OUT].pkt_size = ep_descr->wMaxPacketSize;
                                    fnd_out = 1;
                                }
                            }
                        }
                    }
                }
            }
            libusb_free_config_descriptor(cfg);
            if (!fnd_in || !fnd_out)
                err = X502_ERR_INVALID_USB_CONFIGURATION;
        } else {
            err = X502_ERR_INVALID_USB_CONFIGURATION;
        }

        for (stream=0; stream < X502_STREAM_CH_CNT; stream++) {
            usb_data->streams[stream].mutex = osspec_mutex_create();
            usb_data->streams[stream].user_wake_evt = osspec_event_create(0);
            usb_data->streams[stream].usb_wake_evt = osspec_event_create(0);
            usb_data->streams[stream].dev = hnd;
            usb_data->streams[stream].thread = OSSPEC_INVALID_THREAD;

            if ((usb_data->streams[stream].mutex == OSSPEC_INVALID_MUTEX) ||
                    (usb_data->streams[stream].user_wake_evt == OSSPEC_INVALID_EVENT) ||
                    (usb_data->streams[stream].usb_wake_evt == OSSPEC_INVALID_EVENT))  {
                err = X502_ERR_MUTEX_CREATE;
            }
        }

        if (err == X502_ERR_OK) {
            hnd->iface_data = usb_data;
        } else {
            for (stream=0; stream < X502_STREAM_CH_CNT; stream++) {
                if (usb_data->streams[stream].mutex != OSSPEC_INVALID_MUTEX)
                    osspec_mutex_destroy(usb_data->streams[stream].mutex);
                if (usb_data->streams[stream].user_wake_evt != OSSPEC_INVALID_EVENT)
                    osspec_event_destroy(usb_data->streams[stream].user_wake_evt);
                if (usb_data->streams[stream].usb_wake_evt != OSSPEC_INVALID_EVENT)
                    osspec_event_destroy(usb_data->streams[stream].usb_wake_evt);
            }
            free(usb_data);
        }
    }

    if (err != X502_ERR_OK) {
        if (devhnd != NULL) {
            libusb_release_interface(devhnd, 0);
            libusb_close(devhnd);
        }
    }

    return err;
}

static int32_t f_iface_close(t_x502_hnd hnd) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)hnd->iface_data;
    int32_t err = 0;
    if (usb_data != NULL) {
        unsigned ch;

        for (ch=0; ch < sizeof(usb_data->streams)/sizeof(usb_data->streams[0]); ch++) {
            if (usb_data->streams[ch].mutex != OSSPEC_INVALID_MUTEX) {
                osspec_mutex_destroy(usb_data->streams[ch].mutex);
                usb_data->streams[ch].mutex = OSSPEC_INVALID_MUTEX;
            }
            if (usb_data->streams[ch].user_wake_evt != OSSPEC_INVALID_EVENT) {
                osspec_event_destroy(usb_data->streams[ch].user_wake_evt);
                usb_data->streams[ch].user_wake_evt = OSSPEC_INVALID_EVENT;
            }
            if (usb_data->streams[ch].usb_wake_evt != OSSPEC_INVALID_EVENT) {
                osspec_event_destroy(usb_data->streams[ch].usb_wake_evt);
                usb_data->streams[ch].usb_wake_evt = OSSPEC_INVALID_EVENT;
            }
        }

        libusb_release_interface(usb_data->devhnd, 0);
        libusb_close(usb_data->devhnd);


        free(hnd->iface_data);
        hnd->iface_data = NULL;
    }
    return err;
}


static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr) {
    libusb_device *dev = (libusb_device*)devinfo_ptr->iface_data;
    libusb_unref_device(dev);
    free(devinfo_ptr);
    return X502_ERR_OK;
}



static void LIBUSB_CALL f_usb_transf_cb(struct libusb_transfer *transfer) {
    *((int*)transfer->user_data) = TRANSF_CPL_COMPLETED;
}


static OSSPEC_THREAD_FUNC_RET OSSPEC_THREAD_FUNC_CALL f_usb_rx_thread_func(void *arg) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)arg;
    struct libusb_transfer *transfers[USB_BULK_RX_MAX_TRANSF_CNT]; /* запросы usb */
    volatile int transf_completed[USB_BULK_RX_MAX_TRANSF_CNT];
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_IN];
    unsigned transf_pos=0;
    unsigned cpl_pos = 0;
    unsigned transf_check_pos=0;
    unsigned cpl_check_pos=0;

    unsigned idx;
    int err = 0;
    info->err = 0;

    for (idx = 0; idx < USB_BULK_RX_MAX_TRANSF_CNT; idx++) {
        transfers[idx] = libusb_alloc_transfer(0);
        transf_completed[idx] = TRANSF_CPL_IDLE;
        if (transfers[idx]==NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        }
    }

    while (!err && !info->stop_req) {
        /* обрабатываем завершенные передачи в порядке добавления */
        while (transf_completed[transf_check_pos] == TRANSF_CPL_COMPLETED) {
            t_rx_cpl_part *cpl = &info->rx.cpls[cpl_check_pos];
            struct libusb_transfer *transfer = transfers[transf_check_pos];

            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
            if ((transfer->status == LIBUSB_TRANSFER_COMPLETED)
                    || (transfer->status == LIBUSB_TRANSFER_TIMED_OUT)
                    || (transfer->status == LIBUSB_TRANSFER_CANCELLED)) {
                cpl->size = transfer->actual_length/sizeof(uint32_t);
                info->rx.buf_get_rdy += cpl->size;
                cpl->state = RX_STATE_RDY;
            } else {
                cpl->state = RX_STATE_ERR;
            }
            transf_completed[transf_check_pos] = TRANSF_CPL_IDLE;
            osspec_event_set(info->user_wake_evt);

            if (++transf_check_pos == USB_BULK_RX_MAX_TRANSF_CNT)
                transf_check_pos = 0;
            if (++cpl_check_pos == info->rx.cpl_cnt)
                cpl_check_pos = 0;

            osspec_mutex_release(info->mutex);
        }


        /* пробуем запустить новые трансферы на передачу */
        while ((transf_completed[transf_pos] == TRANSF_CPL_IDLE) &&
               (info->rx.cpls[cpl_pos].state == RX_STATE_IDLE) && !err) {

            info->rx.cpls[cpl_pos].addr = &info->data[info->rx.transf_size*cpl_pos];
            libusb_fill_bulk_transfer(transfers[transf_pos], usb_data->devhnd, info->addr,
                                     (unsigned char*)info->rx.cpls[cpl_pos].addr,
                                     info->rx.transf_size*sizeof(info->data[0]),
                                     f_usb_transf_cb, (void*)&transf_completed[transf_pos],
                                     USB_BULK_RX_TRANSFER_TOUT);
            info->rx.cpls[cpl_pos].state = RX_STATE_BUSY;
            transf_completed[transf_pos] = TRANSF_CPL_BUSY;

            if (libusb_submit_transfer(transfers[transf_pos])==0) {
                osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
                osspec_mutex_release(info->mutex);
                if (++transf_pos == USB_BULK_RX_MAX_TRANSF_CNT)
                    transf_pos = 0;
            } else {
                err = X502_ERR_RECV;
                transf_completed[transf_pos] = TRANSF_CPL_IDLE;
                info->rx.cpls[cpl_pos].state = RX_STATE_ERR;
            }

            if (++cpl_pos == info->rx.cpl_cnt)
                cpl_pos = 0;
        }

        if (!err) {
            int wt_buf_rdy = 0;


            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);           
            if (info->rx.cpls[cpl_pos].state != RX_STATE_IDLE) {
                wt_buf_rdy = 1;
                osspec_event_clear(info->usb_wake_evt);
            }
            osspec_mutex_release(info->mutex);


            if (transf_completed[transf_check_pos] != TRANSF_CPL_IDLE) {
                int lusberr;
                /* иначе ждем событий от USB */
                struct timeval tv;
                tv.tv_sec =0;
                tv.tv_usec = 200 * 1000;
                lusberr = libusb_handle_events_timeout_completed(NULL, &tv, (int*)&transf_completed[transf_check_pos]);

                if ((lusberr != LIBUSB_SUCCESS) && (lusberr != LIBUSB_ERROR_INTERRUPTED)) {
                    err = X502_ERR_RECV;
                }

            } else if (wt_buf_rdy) {
                /* если буфер занят - ждем события от потока чтения */
                osspec_event_wait(info->usb_wake_evt, OSSPEC_TIMEOUT_INFINITY);
            }
        }
    }

    /* отменяем все поставленные запросы */
    for (idx = 0; idx < USB_BULK_RX_MAX_TRANSF_CNT; idx++) {
       unsigned pos = (transf_check_pos + idx) % USB_BULK_RX_MAX_TRANSF_CNT;
       if (transf_completed[pos] == TRANSF_CPL_BUSY) {
           if (libusb_cancel_transfer(transfers[pos]) !=LIBUSB_SUCCESS)
               transf_completed[pos] = TRANSF_CPL_COMPLETED;
       }
    }

    /* ждем завершения всех запросов */
    while (transf_completed[transf_check_pos] != TRANSF_CPL_IDLE) {
        if (transf_completed[transf_check_pos] == TRANSF_CPL_BUSY) {
            struct timeval ztv = {0,0};
            libusb_handle_events_timeout(NULL, &ztv);
        } else {
            transf_completed[transf_check_pos] = TRANSF_CPL_IDLE;
            if (++transf_check_pos == USB_BULK_RX_MAX_TRANSF_CNT)
                transf_check_pos = 0;
        }
    }

    /* очищаем ресурсы */
    for (idx = 0; idx < USB_BULK_RX_MAX_TRANSF_CNT; idx++) {
        if (transfers[idx] != NULL) {
            libusb_free_transfer(transfers[idx]);
        }
    }

    if (info->err == X502_ERR_OK)
       info->err = err;

    return 0;
}



static OSSPEC_THREAD_FUNC_RET OSSPEC_THREAD_FUNC_CALL f_usb_tx_thread_func(void *arg) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)arg;
    struct libusb_transfer *transfers[USB_BULK_TX_MAX_TRANSF_CNT]; /* запросы usb */
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_OUT];
    volatile int transf_completed[USB_BULK_RX_MAX_TRANSF_CNT];
    unsigned transf_pos=0;
    unsigned transf_check_pos=0;
    unsigned idx;
    int err = 0;
    int last_full_packet = 0; /* признак, что последний пакет был полный, чтобы
                                 послять нулевой пакет */
    unsigned snd_pos = 0;


    info->err = 0;
    info->tx.busy_size = 0;

    for (idx = 0; (idx < USB_BULK_TX_MAX_TRANSF_CNT) && (err == X502_ERR_OK); idx++) {
        transfers[idx] = libusb_alloc_transfer(0);
        transf_completed[idx] = TRANSF_CPL_IDLE;
        if (transfers[idx]==NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        }
    }

    while (!err && !info->stop_req) {
        uint32_t snd_rdy_size;
        osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
        snd_rdy_size = info->buf_size - info->tx.buf_put_rdy - info->tx.busy_size;

        if (snd_rdy_size == 0)
            osspec_event_clear(info->usb_wake_evt);
        osspec_mutex_release(info->mutex);

        /* обрабатываем завершенные передачи */
        while (transf_completed[transf_check_pos] == TRANSF_CPL_COMPLETED) {
            struct libusb_transfer *transfer = transfers[transf_check_pos];
            if ((transfer->status != LIBUSB_TRANSFER_COMPLETED)
                    && (transfer->status != LIBUSB_TRANSFER_CANCELLED)) {
                err = X502_ERR_SEND;
            }


            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
            info->tx.buf_put_rdy += transfer->length/sizeof(info->data[0]);
            info->tx.busy_size -= transfer->length/sizeof(info->data[0]);
            osspec_event_set(info->user_wake_evt);
            osspec_mutex_release(info->mutex);

            transf_completed[transf_check_pos] = TRANSF_CPL_IDLE;

            if (++transf_check_pos == USB_BULK_RX_MAX_TRANSF_CNT)
                transf_check_pos = 0;
        }


        while (((snd_rdy_size != 0) || (last_full_packet && (info->tx.busy_size==0)))
               && (transf_completed[transf_pos] == TRANSF_CPL_IDLE)
                && (err == X502_ERR_OK)) {
            uint32_t snd_size = snd_rdy_size;
            if (snd_size > USB_BULK_TX_MAX_TRANSF_SIZE)
                snd_size = USB_BULK_TX_MAX_TRANSF_SIZE;
            if (snd_size > (info->buf_size - snd_pos))
                snd_size = info->buf_size - snd_pos;

            libusb_fill_bulk_transfer(transfers[transf_pos], usb_data->devhnd, info->addr,
                                        (unsigned char*)&info->data[snd_pos],
                                        snd_size*sizeof(info->data[0]),
                                        f_usb_transf_cb,
                                        (void*)&transf_completed[transf_pos],
                                        USB_BULK_TX_TRANSFER_TOUT);



            transf_completed[transf_pos] = TRANSF_CPL_BUSY;
            if (libusb_submit_transfer(transfers[transf_pos])==0) {
                osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);                
                info->tx.busy_size+=snd_size;
                osspec_mutex_release(info->mutex);
                if (++transf_pos == USB_BULK_TX_MAX_TRANSF_CNT)
                    transf_pos = 0;
                snd_pos+=snd_size;
                if (snd_pos==info->buf_size)
                    snd_pos = 0;
                snd_rdy_size-=snd_size;

                last_full_packet = (snd_size != 0) && ((snd_size % (info->pkt_size/sizeof(info->data[0]))) == 0);
            } else {
                transf_completed[transf_pos] = TRANSF_CPL_IDLE;
                err = X502_ERR_SEND;
            }
        }


        if (err == X502_ERR_OK) {
            if (info->tx.busy_size!=0) {
                /* иначе ждем событий от USB */
                struct timeval tv;
                tv.tv_sec =0;
                tv.tv_usec = 200 * 1000;
                libusb_handle_events_timeout_completed(NULL, &tv,
                                                       (int*)&transf_completed[transf_check_pos]);
            } else if ((snd_rdy_size==0) && !last_full_packet) {
                /* если буфер занят - ждем события от потока чтения */
                osspec_event_wait(info->usb_wake_evt, OSSPEC_TIMEOUT_INFINITY);
            }
        }
    }

    /* отменяем все поставленные запросы */
    for (idx = 0; idx < USB_BULK_RX_MAX_TRANSF_CNT; idx++) {
       unsigned pos = (transf_check_pos + idx) % USB_BULK_RX_MAX_TRANSF_CNT;
       if (transf_completed[pos] == TRANSF_CPL_BUSY) {
           if (libusb_cancel_transfer(transfers[pos]) !=LIBUSB_SUCCESS)
               transf_completed[pos] = TRANSF_CPL_COMPLETED;
       }
    }

    /* ждем завершения всех запросов */
    while (transf_completed[transf_check_pos] != TRANSF_CPL_IDLE) {
        if (transf_completed[transf_check_pos] == TRANSF_CPL_BUSY) {
            struct timeval ztv = {0,0};
            libusb_handle_events_timeout(NULL, &ztv);
        } else {
            transf_completed[transf_check_pos] = TRANSF_CPL_IDLE;
            if (++transf_check_pos == USB_BULK_RX_MAX_TRANSF_CNT)
                transf_check_pos = 0;
        }
    }

    /* очищаем ресурсы */
    for (idx = 0; idx < USB_BULK_RX_MAX_TRANSF_CNT; idx++) {
        if (transfers[idx] != NULL) {
            libusb_free_transfer(transfers[idx]);
        }
    }

    if (info->err == X502_ERR_OK)
       info->err = err;


    return 0;
}


static void f_stream_init(uint32_t ch, t_transf_info *info) {
    info->stop_req = 0;
    info->err = 0;
    if (ch==X502_STREAM_CH_IN) {
        unsigned i;
        info->rx.cpl_get_pos = 0;
        info->rx.buf_get_rdy = 0;


        for (i=0; i < info->rx.cpl_cnt; i++) {
            info->rx.cpls[i].state = RX_STATE_IDLE;
        }
    }  else {
        info->tx.buf_pos_put = 0;
        info->tx.buf_put_rdy = info->buf_size;
    }
}

static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[ch];
    int err = 0;

    /** @todo вариант изменения размеров буфера в работе */
    if (info->data == NULL) {
        info->buf_size = params->buf_size;


        if (ch==X502_STREAM_CH_IN) {
            info->rx.transf_size = ((params->step+127)/128)*128;
            if (info->rx.transf_size > USB_BULK_RX_MAX_TRANSF_SIZE)
                info->rx.transf_size = USB_BULK_RX_MAX_TRANSF_SIZE;

            info->rx.cpl_cnt = info->buf_size/info->rx.transf_size;
            /* буфер разбиваем на столько частей, чтобы можно было запустить
             * в параллель все USB_BULK_RX_MAX_TRANSF_CNT трансферов */
            if (info->rx.cpl_cnt < USB_BULK_RX_MAX_TRANSF_CNT)
                info->rx.cpl_cnt = USB_BULK_RX_MAX_TRANSF_CNT;
            info->buf_size = info->rx.cpl_cnt*info->rx.transf_size;

            info->rx.cpls = malloc(info->rx.cpl_cnt * sizeof(info->rx.cpls[0]));
            if (info->rx.cpls == NULL) {
                err = X502_ERR_MEMORY_ALLOC;
            }
        }

        if (err == X502_ERR_OK) {
            info->data = malloc(info->buf_size*sizeof(info->data[0]));
            if (info->data == NULL) {
                err = X502_ERR_MEMORY_ALLOC;
            }
        }

        if (err == X502_ERR_OK) {
            f_stream_init(ch, info);
        }

        if (err == X502_ERR_OK) {
            uint16_t send_step = params->step > 0xFFFF ? 0xFFFF : params->step;
            err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_SET_STEP,
                          (ch<<16) | send_step, NULL, 0, NULL, 0, NULL, 0);
        }


        if (err != X502_ERR_OK) {
            f_iface_stream_free(hnd, ch, 0);
        }
    }


    return err;
}






static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    int32_t err = X502_ERR_OK;

    if (err == X502_ERR_OK) {
        t_transf_info *info = &usb_data->streams[ch];
        f_stream_init(ch, info);
        info->thread = osspec_thread_create(ch == X502_STREAM_CH_IN ?
                                                f_usb_rx_thread_func : f_usb_tx_thread_func,
                                            usb_data, 0);
        if (info->thread == OSSPEC_INVALID_THREAD) {
            err = X502_ERR_THREAD_START;
        }
    }

    if (!err && !(flags & X502_STREAM_FLAG_NO_REQUEST)) {
        err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_START, (ch<<16),
                      NULL, 0, NULL, 0, NULL, 0);
    }

    return err;
}

static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[ch];
    int err = X502_ERR_OK;
    int ioctl_err = X502_ERR_OK;
    int32_t running;

    if (!(flags & X502_STREAM_FLAG_NO_REQUEST)) {
        ioctl_err = hnd->iface_hnd->stream_running(hnd, ch, &running);
        if (!ioctl_err && running) {
            ioctl_err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_STOP, (ch << 16),
                                NULL, 0, NULL, 0, NULL, 0);
        }
    }


    if (info->thread != OSSPEC_INVALID_THREAD) {
        info->stop_req = 1;
        osspec_event_set(info->usb_wake_evt);
        err = osspec_thread_wait(info->thread, STREAM_STOP_TOUT);
        info->thread  = OSSPEC_INVALID_THREAD;
    }

    return err != X502_ERR_OK ? err : ioctl_err;
}


static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    int32_t err;
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[ch];

    err = hnd->iface_hnd->stream_stop(hnd, ch, flags);

    free(info->data);
    info->data = NULL;

    if (ch==X502_STREAM_CH_IN) {
        free(info->rx.cpls);
        info->rx.cpls = NULL;
        info->rx.cpl_cnt = 0;
        info->rx.transf_size = 0;
    }

    return err;
}




static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout) {
    int32_t recvd = 0;
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_IN];
    t_ltimer tmr;
    int32_t err = info->data == NULL ? X502_ERR_STREAM_IS_NOT_RUNNING : X502_ERR_OK;

    if (err == X502_ERR_OK) {
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        do {
            int check_next = 1;
            err = info->err;

            while (!err && check_next) {
                t_rx_cpl_part *cpl = &info->rx.cpls[info->rx.cpl_get_pos];
                int cur_state = cpl->state;
                check_next = 0;

                if (cur_state == RX_STATE_ERR) {
                    err = X502_ERR_RECV;
                } else if (cur_state == RX_STATE_RDY) {
                    osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);

                    if (cpl->size) {
                        uint32_t cpy_size = cpl->size;
                        if (cpy_size > size)
                            cpy_size = size;
                        if (cpy_size) {
                            memcpy(&buf[recvd], cpl->addr, cpy_size*sizeof(buf[0]));
                            recvd+=cpy_size;
                            size-=cpy_size;
                            cpl->size -= cpy_size;
                            cpl->addr += cpy_size;
                            info->rx.buf_get_rdy -= cpy_size;
                        }
                    }

                    if (cpl->size==0) {
                        cpl->state = RX_STATE_IDLE;
                        osspec_event_set(info->usb_wake_evt);
                        check_next = 1;
                    }
                    osspec_mutex_release(info->mutex);
                }

                if (check_next) {
                    if (++info->rx.cpl_get_pos==info->rx.cpl_cnt)
                        info->rx.cpl_get_pos = 0;
                }
            }



            if (!err && (size!=0)) {
                osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
                if ((info->rx.cpls[info->rx.cpl_get_pos].state == RX_STATE_IDLE)
                        || ((info->rx.cpls[info->rx.cpl_get_pos].state == RX_STATE_BUSY))) {
                    osspec_event_clear(info->user_wake_evt);
                }
                osspec_mutex_release(info->mutex);

                osspec_event_wait(info->user_wake_evt, LTIMER_CLOCK_TICKS_TO_MS(ltimer_expiration(&tmr)));
            }
        } while (!err && (size!=0) && !ltimer_expired(&tmr));
    }

    return err == X502_ERR_OK ? recvd : err;
}



static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_OUT];
    t_ltimer tmr;
    int32_t sent = 0;
    int32_t err = info->data == NULL ? X502_ERR_STREAM_IS_NOT_RUNNING : X502_ERR_OK;

    if (err == X502_ERR_OK) {
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        do {
            uint32_t rdy_size, snd_size;
            snd_size = rdy_size = info->tx.buf_put_rdy;
            err = info->err;

            if (!err) {
                if (rdy_size == 0) {
                    osspec_event_wait(info->user_wake_evt,
                                      LTIMER_CLOCK_TICKS_TO_MS(ltimer_expiration(&tmr)));
                } else {
                    uint32_t end_size;
                    uint32_t put_pos;
                    if (snd_size > size)
                        snd_size = size;
                    end_size = info->buf_size - info->tx.buf_pos_put;
                    if (snd_size >= end_size) {
                        memcpy(&info->data[info->tx.buf_pos_put], &buf[sent], end_size*sizeof(buf[0]));
                        if (snd_size!=end_size) {
                            memcpy(&info->data[0], &buf[sent+end_size], (snd_size-end_size)*sizeof(buf[0]));
                        }
                        put_pos = snd_size-end_size;
                    } else {
                        memcpy(&info->data[info->tx.buf_pos_put], &buf[sent], snd_size*sizeof(buf[0]));
                        put_pos = info->tx.buf_pos_put + snd_size;
                    }

                    osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
                    info->tx.buf_pos_put  = put_pos;
                    info->tx.buf_put_rdy -= snd_size;

                    if (info->tx.buf_put_rdy==0) {
                        osspec_event_clear(info->user_wake_evt);
                    }

                    osspec_event_set(info->usb_wake_evt);

                    osspec_mutex_release(info->mutex);


                    sent+=snd_size;
                    size-=snd_size;
                }
            }
        } while (!err && (size!=0) && !ltimer_expired(&tmr));
    }
    return err == X502_ERR_OK ? sent : err;
}



static int32_t f_iface_stream_get_rdy_cnt(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_cnt) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    int32_t err=0;
    if (ch == X502_STREAM_CH_IN) {
        *rdy_cnt = usb_data->streams[X502_STREAM_CH_IN].rx.buf_get_rdy;
    } else if (ch == X502_STREAM_CH_OUT) {
        *rdy_cnt = usb_data->streams[X502_STREAM_CH_OUT].tx.buf_put_rdy;
    } else {
        err = X502_ERR_INVALID_STREAM_CH;

    }
    return err;
}






static int32_t f_ioreq(libusb_device_handle *handle, uint32_t cmd_code, uint32_t param,
                   const void* snd_data, uint32_t snd_size,
                   void* rcv_data, uint32_t recv_size, uint32_t* recvd_size, uint32_t tout) {
    int32_t err = 0;
    uint8_t req_type = LIBUSB_REQUEST_TYPE_VENDOR;
    uint16_t len;
    uint8_t* iobuf;
    int usbres = 0;
    if (tout == 0)
        tout = E502_USB_REQ_TOUT;

    if (recv_size > 0) {
        req_type |= LIBUSB_ENDPOINT_IN;
        len = recv_size;
        iobuf = rcv_data;
    } else {
        req_type |= LIBUSB_ENDPOINT_OUT;
        len = snd_size;
        iobuf = (uint8_t*)snd_data;
    }

    if (!err) {
        usbres = libusb_control_transfer(handle,
                req_type, cmd_code & 0xFF,
                param & 0xFFFF,
                (param >> 16) & 0xFFFF,
                iobuf, len, tout);
        if (usbres < 0) {
            err = (usbres == LIBUSB_ERROR_NO_DEVICE ? X502_ERR_DEVICE_DISCONNECTED :
                                                     X502_ERR_IOCTL_FAILD);
        } else {
            if (recvd_size!=NULL) {
                *recvd_size = usbres;
            } else if (usbres != len) {
                err = X502_ERR_IOCTL_INVALID_RESP_SIZE;
            }
        }

        //если управляющий запрос не выполнен успешно, то пытаемся получить
        //код ошибки из устройства
        if (err == X502_ERR_IOCTL_FAILD) {
            uint32_t devres;
            usbres = libusb_control_transfer(handle,
                    LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR,
                    E502_CM4_CMD_GET_LAST_ERROR,
                    0, 0,
                    (uint8_t*)&devres, sizeof(devres),
                    E502_USB_REQ_TOUT);

            //если успешно получили код ошибки устройства - то возвращаем его в качестве результата
            if ((usbres == sizeof(devres)) && (devres !=0)) {
                err = devres;
            }
        }
    }
    return err;
}


static int32_t f_iface_gen_ioctl(t_x502_hnd hnd, uint32_t cmd_code, uint32_t param,
                                 const void* snd_data, uint32_t snd_size,
                                 void* rcv_data, uint32_t recv_size, uint32_t* recvd_size,
                                 uint32_t tout) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    return f_ioreq(usb_data->devhnd, cmd_code, param, snd_data, snd_size,
                   rcv_data, recv_size, recvd_size, tout);
}

static int32_t f_fill_devlist(libusb_device_handle *hnd, t_x502_devrec *info) {
    int32_t err = X502_ERR_OK;
    t_x502_devrec_inptr *devinfo_ptr = calloc(1, sizeof(t_x502_devrec_inptr));
    if (devinfo_ptr==NULL)  {
        err = X502_ERR_MEMORY_ALLOC;
    }

    if (err == X502_ERR_OK) {
        t_lboot_devinfo lboot_info;


        //получаем информацию о устройстве
        err = f_ioreq(hnd, E502_CM4_CMD_GET_MODULE_INFO, 0, NULL, 0, &lboot_info,
                      sizeof(lboot_info), NULL, 0);

        if (err == X502_ERR_OK) {
            uint32_t devflags;


            strcpy(info->devname, lboot_info.devname);
            strcpy(info->serial, lboot_info.serial);

            devinfo_ptr->iface = &f_usb_iface;
            devinfo_ptr->iface_data = libusb_ref_device(libusb_get_device(hnd));
            info->internal = devinfo_ptr;
            info->iface = X502_IFACE_USB;
            info->flags = X502_DEVFLAGS_IFACE_SUPPORT_USB;

            if (f_ioreq(hnd, E502_CM4_CMD_GET_DEVFLAGS, 0,
                        NULL, 0, &devflags, sizeof(devflags), NULL, 0) == X502_ERR_OK) {
                info->flags &= ~E502_CM4_DEVFLAGS;
                info->flags |= devflags;
            }
        }
    }

    if (err != X502_ERR_OK) {
        free(devinfo_ptr);
    }
    return err;
}




X502_EXPORT(int32_t) E502_UsbGetDevRecordsList(t_x502_devrec *list, uint32_t size,
                                             uint32_t flags, uint32_t *devcnt) {
    uint32_t curcnt=0;
    int32_t err = X502_ERR_OK;
    libusb_device **usb_devlist;
    ssize_t usb_devlist_size, i;

    if (!f_lusb_init_done) {
        libusb_init(NULL);
#ifdef LIBUSB_DEBUG
        libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);
#endif
        f_lusb_init_done = 1;
    }


    //получаем список устройств
    usb_devlist_size = libusb_get_device_list(NULL, &usb_devlist);
    for (i=0; i < usb_devlist_size; i++) {
        //получаем дескриптор устройства для проверки vid, pid
        struct libusb_device_descriptor devdescr;
        libusb_device_handle* usbhnd = NULL;

        if (libusb_get_device_descriptor (usb_devlist[i], &devdescr) == 0) {
            //сравниваем VID, PID с индентификаторами загрузочного устройства
            if ((devdescr.idVendor == E502_USB_VID)
                    && (devdescr.idProduct == E502_USB_PID)) {
                int32_t cur_err;
                cur_err = libusb_open(usb_devlist[i], &usbhnd);
                if (cur_err == X502_ERR_OK) {
                    t_x502_devrec info;
                    int info_used = 0;
                    X502_DevRecordInit(&info);


                   if (f_fill_devlist(usbhnd, &info) == X502_ERR_OK) {
                       /* если нужны только не открытые, то уже открытое
                        * устройство пропускаем */
                       if (!(flags & X502_GETDEVS_FLAGS_ONLY_NOT_OPENED) ||
                               !(info.flags & X502_DEVFLAGS_DEVREC_OPENED)) {
                           /* если есть место в списке - то сохраняем
                            * полученную информацию */
                           if ((list!=NULL) && (curcnt < size)) {
                               list[curcnt] = info;
                               info_used = 1;
                           }
                           curcnt++;
                       }
                   }

                   if (!info_used)
                       X502_FreeDevRecordList(&info,1);

                   libusb_close(usbhnd);
                }
            }
        }
    }
    libusb_free_device_list(usb_devlist, 1);


    if (devcnt != NULL)
        *devcnt = curcnt;

    return err != X502_ERR_OK ? err  : curcnt > (uint32_t)size ? (int32_t)size : (int32_t)curcnt ;
}

X502_EXPORT(int32_t) E502_OpenUsb(t_x502_hnd hnd, const char* serial) {
    return X502_Open(hnd, serial, E502_DEVICE_NAME, E502_UsbGetDevRecordsList);
}

X502_EXPORT(int32_t) E502_UsbGetSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                           uint32_t flags, uint32_t *devcnt) {
   return X502_GetSerialList(serials, size, flags, devcnt, E502_DEVICE_NAME, E502_UsbGetDevRecordsList);
}
#else
#include "e502api.h"
X502_EXPORT(int32_t) E502_OpenUsb(t_x502_hnd hnd, const char* serial) {
    return X502_ERR_NOT_IMPLEMENTED;
}

X502_EXPORT(int32_t) E502_UsbGetSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                           uint32_t flags, uint32_t *devcnt) {
   return X502_ERR_NOT_IMPLEMENTED;
}
#endif






