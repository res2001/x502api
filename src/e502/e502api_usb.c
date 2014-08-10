#include "e502api.h"
#include "x502api_private.h"
#include "e502_cm4_defs.h"
#include "lboot_req.h"
#include "libusb-1.0/libusb.h"
#include "e502_fpga_regs.h"
#include "osspec.h"
#include "timer.h"

#include <stdlib.h>
#include <string.h>


#define E502_USB_VID        0x2A52
#define E502_USB_PID        0xE502
#define E502_USB_REQ_TOUT   1000
#define USB_TRANSFER_TOUT   -1


#define MUTEX_STREAM_LOCK_TOUT  2000


#define STREAM_STOP_TOUT      1000

#define USB_IN_STREAM_BUF_MIN   64

#define E502_DEVICE_NAME "E502"


#define USB_BULK_MAX_TRANSF_CNT          20
#define USB_BULK_MAX_TRANSF_SIZE         (32*1024)


static int f_lusb_init_done = 0;


struct st_transf_info;

typedef struct {
    uint32_t *addr;
    struct st_transf_info *info;
    uint32_t size;
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
    t_mutex mutex;
    t_thread thread;
    t_event  data_rdy_evt;

    volatile int stop_req;
    volatile int usb_rdy;
    uint32_t *data; /* промежуточный буфер, используемый во всех запросах */
    unsigned buf_size;

    union {
        struct {
            t_rx_cpl_part *cpls;
            uint32_t cpl_cnt;
            uint32_t cpl_get_pos;
            uint32_t transf_size;
            volatile uint32_t transf_busy;
        } rx;
        struct {
            uint32_t buf_pos_transf_start;
            uint32_t buf_pos_transf_done;
            uint32_t buf_pos_cpy_done;
            int err;
        } tx;
    };
} t_transf_info;


typedef struct {
    libusb_device_handle *devhnd;
    t_transf_info streams[X502_STREAM_CH_CNT];
} t_usb_iface_data;


static int32_t f_ioreq(libusb_device_handle *handle, uint32_t cmd_code, uint32_t param,
                   const void* snd_data, uint32_t snd_size,
                   void* rcv_data, uint32_t recv_size, uint32_t* recvd_size);

static int32_t f_iface_free_devinfo_data(void *devinfo_data);
static int32_t f_iface_open(t_x502_hnd hnd, const t_lpcie_devinfo *devinfo);
static int32_t f_iface_close(t_x502_hnd hnd);
static int32_t f_iface_fpga_read(t_x502_hnd hnd, uint16_t addr, uint32_t *val);
static int32_t f_iface_fpga_write(t_x502_hnd hnd, uint16_t addr, uint32_t val);
static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params);
static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t signle);
static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch);
static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch);
static int32_t f_iface_stream_running(t_x502_hnd hnd, uint32_t ch, int32_t* running);
static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout);
static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) ;






static const t_x502_dev_iface f_usb_iface = {
    E502_REGS_ARM_HARD_ID,
    USB_IN_STREAM_BUF_MIN,
    f_iface_free_devinfo_data,
    f_iface_open,
    f_iface_close,
    f_iface_fpga_read,
    f_iface_fpga_write,
    f_iface_stream_cfg,
    f_iface_stream_start,
    f_iface_stream_stop,
    f_iface_stream_free,
    f_iface_stream_running,
    f_iface_stream_read,
    f_iface_stream_write
};






static int32_t f_iface_open(t_x502_hnd hnd, const t_lpcie_devinfo *devinfo) {
    int32_t err, usberr;
    libusb_device *dev = (libusb_device *)(((t_lpcie_devinfo_inptr*)devinfo->internal)->iface_data);
    libusb_device_handle *devhnd;


    usberr = libusb_open(dev, &devhnd);
    if (!usberr) {
        err = libusb_claim_interface(devhnd, 0);        
    }

    err = usberr == LIBUSB_SUCCESS ? X502_ERR_OK :
          usberr == LIBUSB_ERROR_BUSY ? X502_ERR_ALREADY_OPENED :
          usberr == LIBUSB_ERROR_ACCESS ? X502_ERR_DEVICE_ACCESS_DENIED :
          X502_ERR_DEVICE_OPEN;

    if (err == X502_ERR_OK) {
        t_usb_iface_data *usb_data = calloc(1, sizeof(t_usb_iface_data));
        unsigned stream;

        usb_data->devhnd = devhnd;

        /** @todo нахождение адресов кт по дускриптору конфигурации */
        usb_data->streams[X502_STREAM_CH_IN].addr  = 0x81;
        usb_data->streams[X502_STREAM_CH_OUT].addr = 0x01;

        hnd->iface_data = usb_data;


        for (stream=0; stream < X502_STREAM_CH_CNT; stream++) {
            usb_data->streams[stream].mutex = osspec_mutex_create();
            usb_data->streams[stream].data_rdy_evt = osspec_event_create(0);
            usb_data->streams[stream].dev = hnd;
            usb_data->streams[stream].thread = OSSPEC_INVALID_THREAD;

            if ((usb_data->streams[stream].mutex == OSSPEC_INVALID_MUTEX) ||
                    (usb_data->streams[stream].data_rdy_evt == OSSPEC_INVALID_EVENT))  {
                err = X502_ERR_MUTEX_CREATE;
            }
        }
    }

    return err;
}

static int32_t f_iface_close(t_x502_hnd hnd) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)hnd->iface_data;
    int32_t err = 0;
    unsigned ch;

    for (ch=0; ch < sizeof(usb_data->streams)/sizeof(usb_data->streams[0]); ch++) {
        if (usb_data->streams[ch].mutex != OSSPEC_INVALID_MUTEX) {
            osspec_mutex_destroy(usb_data->streams[ch].mutex);
            usb_data->streams[ch].mutex = OSSPEC_INVALID_MUTEX;
        }
        if (usb_data->streams[ch].data_rdy_evt != OSSPEC_INVALID_EVENT) {
            osspec_event_destroy(usb_data->streams[ch].data_rdy_evt);
            usb_data->streams[ch].data_rdy_evt = OSSPEC_INVALID_EVENT;
        }
    }

    if (libusb_release_interface(usb_data->devhnd, 0)!=LIBUSB_SUCCESS) {
        err = X502_ERR_INTERFACE_RELEASE;
    }
    if (!err) {
        libusb_close(usb_data->devhnd);
    }
    return err;
}


static int32_t f_iface_free_devinfo_data(void *devinfo_data) {
    libusb_device *dev = (libusb_device*)devinfo_data;
    libusb_unref_device(dev);
    return 0;
}

static int32_t f_iface_fpga_read(t_x502_hnd hnd, uint16_t addr, uint32_t *val) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    return f_ioreq(usb_data->devhnd, E502_CM4_CMD_FPGA_REG_READ, addr, NULL, 0,
                   val, sizeof(*val), NULL);
}

static int32_t f_iface_fpga_write(t_x502_hnd hnd, uint16_t addr, uint32_t val) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    return f_ioreq(usb_data->devhnd, E502_CM4_CMD_FPGA_REG_WRITE, addr, &val, sizeof(val),
                   NULL, 0, NULL);
}




#if 0
static void f_usb_transf_tx_cb(struct libusb_transfer *transfer) {
    t_transf_info *info = (t_transf_info*)transfer->user_data;
    if ((transfer->status != LIBUSB_TRANSFER_COMPLETED)
            && (transfer->status != LIBUSB_TRANSFER_CANCELLED)) {
        info->tx.err = X502_ERR_SEND;
    }

    osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
    info->trans_busy--;
    info->tx.rdy_size += transfer->length/sizeof(info->data[0]);
    if (info->tx.rdy_size)
        info->tx.rdy = 1;
    if (++info->trans_get_pos == info->transf_cnt)
        info->trans_get_pos = 0;
    osspec_mutex_release(info->mutex);
}
#endif


static void f_usb_transf_rx_cb(struct libusb_transfer *transfer) {
    t_rx_cpl_part *cpl = (t_rx_cpl_part*)transfer->user_data;


    osspec_mutex_lock(cpl->info->mutex, MUTEX_STREAM_LOCK_TOUT);
    if ((transfer->status == LIBUSB_TRANSFER_COMPLETED)
            || (transfer->status == LIBUSB_TRANSFER_TIMED_OUT)
            || (transfer->status == LIBUSB_TRANSFER_CANCELLED)) {
        cpl->size = transfer->actual_length/sizeof(uint32_t);
        cpl->state = RX_STATE_RDY;
    } else {
        cpl->state = RX_STATE_ERR;
    }
    cpl->info->rx.transf_busy--;
    osspec_event_set(cpl->info->data_rdy_evt);
    cpl->info->usb_rdy = 1;
    osspec_mutex_release(cpl->info->mutex);
}


static OSSPEC_THREAD_FUNC_RET f_usb_rx_thread_func(void *arg) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)arg;
    struct libusb_transfer *transfers[USB_BULK_MAX_TRANSF_CNT]; /* запросы usb */
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_IN];
    unsigned transf_pos=0;
    unsigned cpl_pos = 0;

    unsigned idx;
    int err = 0;

    info->rx.transf_busy = 0;






    for (idx = 0; (idx < USB_BULK_MAX_TRANSF_CNT) && (err == X502_ERR_OK); idx++) {
        transfers[idx] = libusb_alloc_transfer(0);
        if (transfers[idx]==NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        }
    }

    while (!err && !info->stop_req) {        
        /* пробуем запустить новые трансферы на передачу */
        while ((info->rx.transf_busy != USB_BULK_MAX_TRANSF_CNT) &&
               (info->rx.cpls[cpl_pos].state == RX_STATE_IDLE) && !err) {

            info->rx.cpls[cpl_pos].addr = &info->data[info->rx.transf_size*cpl_pos];
            libusb_fill_bulk_transfer(transfers[transf_pos], usb_data->devhnd, info->addr,
                                     (unsigned char*)info->rx.cpls[cpl_pos].addr,
                                     info->rx.transf_size*sizeof(info->data[0]),
                                     f_usb_transf_rx_cb, &info->rx.cpls[cpl_pos], USB_TRANSFER_TOUT);
            info->rx.cpls[cpl_pos].state = RX_STATE_BUSY;

            if (libusb_submit_transfer(transfers[transf_pos])==0) {                
                osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
                info->rx.transf_busy++;                
                osspec_mutex_release(info->mutex);
                if (++transf_pos == USB_BULK_MAX_TRANSF_CNT)
                    transf_pos = 0;
            } else {
                err = X502_ERR_RECV;
                info->rx.cpls[cpl_pos].state = RX_STATE_ERR;
            }

            if (++cpl_pos == info->rx.cpl_cnt)
                cpl_pos = 0;
        }

        if (!err) {
            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
            info->usb_rdy = (info->rx.transf_busy != USB_BULK_MAX_TRANSF_CNT) &&
                    (info->rx.cpls[cpl_pos].state == RX_STATE_IDLE);
            osspec_mutex_release(info->mutex);
            libusb_handle_events_completed(NULL, (int*)&info->usb_rdy);
        }
   }


   if (osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT)==0) {
       unsigned busy_cnt = info->rx.transf_busy;
       if (busy_cnt==0) {
           osspec_mutex_release(info->mutex);
       } else {
           unsigned pos = transf_pos > busy_cnt ? transf_pos - busy_cnt :
                                                  USB_BULK_MAX_TRANSF_CNT - busy_cnt + transf_pos;

           osspec_mutex_release(info->mutex);
           for (idx = 0; idx < busy_cnt; idx++) {
               libusb_cancel_transfer(transfers[pos]);
               if (++pos == USB_BULK_MAX_TRANSF_CNT)
                   pos = 0;
           }
       }


        while (info->rx.transf_busy!=0) {
            struct timeval ztv = {0,0};
            libusb_handle_events_timeout(NULL, &ztv);
        }

        for (idx = 0; idx < USB_BULK_MAX_TRANSF_CNT; idx++) {
            if (transfers[idx]) {
                libusb_free_transfer(transfers[idx]);
            }
        }
    }
    return 0;
}



static OSSPEC_THREAD_FUNC_RET f_usb_tx_thread_func(void *arg) {
    return 0;
}



static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[ch];
    int err = 0;

    /** @todo вариант изменения размеров буфера в работе */
    if (info->data == NULL) {
        info->buf_size = params->buf_size;

        info->stop_req = 0;


        if (ch==X502_STREAM_CH_IN) {
            info->rx.transf_size = ((params->step+127)/128)*128;
            if (info->rx.transf_size > USB_BULK_MAX_TRANSF_SIZE)
                info->rx.transf_size = USB_BULK_MAX_TRANSF_SIZE;

            info->rx.cpl_cnt = info->buf_size/info->rx.transf_size;
            info->buf_size = info->rx.cpl_cnt*info->rx.transf_size;

            info->rx.cpl_get_pos = 0;
            info->rx.cpls = malloc(info->rx.cpl_cnt * sizeof(info->rx.cpls[0]));
            if (info->rx.cpls == NULL) {
                err = X502_ERR_MEMORY_ALLOC;
            } else {
                unsigned i;
                for (i=0; i < info->rx.cpl_cnt; i++) {                    
                    info->rx.cpls[i].state = RX_STATE_IDLE;
                    info->rx.cpls[i].info = info;
                }
            }
        }

        if (err == X502_ERR_OK) {
            info->data = malloc(info->buf_size*sizeof(info->data[0]));
            if (info->data == NULL) {
                err = X502_ERR_MEMORY_ALLOC;
            }
        }


        if (!err) {
            uint16_t send_step = params->step > 0xFFFF ? 0xFFFF : params->step;
            err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_SET_STEP,
                          (ch<<16) | send_step, NULL, 0, NULL, 0, NULL);
        }


        if (err) {
            f_iface_stream_free(hnd, ch);
        }
    }


    return err;
}






static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t signle) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    int err = 0;

    if (!err) {
        t_transf_info *info = &usb_data->streams[ch];
        info->thread = osspec_thread_create(ch == X502_STREAM_CH_IN ?
                                                f_usb_rx_thread_func : f_usb_tx_thread_func,
                                            usb_data, 0);
        if (info->thread == OSSPEC_INVALID_THREAD) {
            err = X502_ERR_THREAD_START;
        }
    }

    if (!err) {
        err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_START, (ch<<16),
                      NULL, 0, NULL, 0, NULL);
    }

    return err;
}

static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[ch];
    int err = 0;
    int ioctl_err;

    ioctl_err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_STOP, (ch << 16), NULL, 0, NULL, 0, NULL);

    if (info->thread != OSSPEC_INVALID_THREAD) {
        info->stop_req = 1;
        info->usb_rdy = 1;
        err = osspec_thread_wait(info->thread, STREAM_STOP_TOUT);
    }

    return err != X502_ERR_OK ? err : ioctl_err;
}


static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch) {
    int32_t err;
    int32_t running;


    err = hnd->iface->stream_running(hnd, ch, &running);
    if (!err && running)
        err = hnd->iface->stream_stop(hnd, ch);

    if (!err) {
        t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
        t_transf_info *info = &usb_data->streams[ch];


        free(info->data);
        info->data = NULL;

        if (ch==X502_STREAM_CH_IN) {
            free(info->rx.cpls);
            info->rx.cpls = NULL;
            info->rx.cpl_cnt = 0;
            info->rx.transf_size = 0;
        }


    }
    return err;
}

static int32_t f_iface_stream_running(t_x502_hnd hnd, uint32_t ch, int32_t* running) {
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    int32_t err;
    uint8_t l_run;
    err = f_ioreq(usb_data->devhnd, E502_CM4_CMD_STREAM_IS_RUNNING, (ch << 16), NULL, 0, &l_run, 1, NULL);
    if (!err && (running!=NULL)) {
        *running = l_run;
    }
    return err;
}


static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout) {
    int32_t recvd = 0;
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_IN];
    t_timer tmr;
    int err = 0;

    timer_set(&tmr, tout*CLOCK_CONF_SECOND/1000);


    do {
        int check_next = 1;
        int processed = 0;
#if 1
        while (!err && check_next) {
            t_rx_cpl_part *cpl = &info->rx.cpls[info->rx.cpl_get_pos];
            int cur_state = cpl->state;
            check_next = 0;

            if (cur_state == RX_STATE_ERR) {
                err = X502_ERR_RECV;
            } else if (cur_state == RX_STATE_RDY){
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
                    }
                }

                if (cpl->size==0) {
                    cpl->state = RX_STATE_IDLE;
                    processed = 1;
                    check_next = 1;
                }
            }

            if (check_next) {
                if (++info->rx.cpl_get_pos==info->rx.cpl_cnt)
                    info->rx.cpl_get_pos = 0;
            }
        }

        if (processed) {
            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
            info->usb_rdy = 1;

            if ((info->rx.cpls[info->rx.cpl_get_pos].state == RX_STATE_IDLE)
                    || ((info->rx.cpls[info->rx.cpl_get_pos].state == RX_STATE_BUSY))) {
                osspec_event_clear(info->data_rdy_evt);
            }

            osspec_mutex_release(info->mutex);
        }

        if (!err && (size!=0)) {
            osspec_event_wait(info->data_rdy_evt, timer_expiration(&tmr)
                                    *1000/CLOCK_CONF_SECOND);
        }
#endif
    } while (!err && (size!=0) && !timer_expired(&tmr));

    return err == X502_ERR_OK ? recvd : err;
}



static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) {
    int32_t sent = 0;
    t_usb_iface_data *usb_data = (t_usb_iface_data *)hnd->iface_data;
    t_transf_info *info = &usb_data->streams[X502_STREAM_CH_OUT];
    t_timer tmr;
    int err = 0;

#if 0

    timer_set(&tmr, tout*CLOCK_CONF_SECOND/1000);

    while (size!=0 && !timer_expired(&tmr)) {
        if (info->tx.rdy) {


            uint32_t put_transf;
            uint32_t tx_size = size;

            osspec_mutex_lock(info->mutex, MUTEX_STREAM_LOCK_TOUT);
            put_transf = (info->trans_get_pos + info->trans_busy) % info->transf_cnt;

            if (tx_size > info->tx.rdy_size)
                tx_size = info->tx.rdy_size;
            if (tx_size > (info->buf_size - info->tx.start_pos))
                tx_size = info->buf_size - info->tx.start_pos;

            memcpy(&info->data[info->tx.start_pos], &buf[sent], tx_size*sizeof(buf[0]));

            libusb_fill_bulk_transfer(info->transfers[put_transf], usb_data->devhnd, info->addr,
                                     (unsigned char*)&info->data[info->tx.start_pos],
                                     tx_size*sizeof(info->data[0]),
                                     f_usb_transf_tx_cb, info, USB_TRANSFER_TOUT);

            err = libusb_submit_transfer(info->transfers[put_transf]);
            if (!err) {
                info->trans_busy++;
                info->tx.rdy_size -= tx_size;
                size -= tx_size;
                info->tx.rdy = (info->trans_busy != info->transf_cnt) && (info->tx.rdy_size!=0);

                info->tx.start_pos += tx_size;
                if (info->tx.start_pos==info->buf_size)
                    info->tx.start_pos = 0;
                sent += tx_size;

            } else {
                err = X502_ERR_SEND;
            }
            osspec_mutex_release(info->mutex);

            if ((size!=0) && !info->tx.rdy) {
                struct timeval tv;
                tv.tv_sec = timer_expiration(&tmr)/CLOCK_CONF_SECOND;
                tv.tv_usec = (timer_expiration(&tmr)%CLOCK_CONF_SECOND) * 1000000/CLOCK_CONF_SECOND;
                libusb_handle_events_timeout_completed(NULL, &tv, &info->tx.rdy);
            }
        }
    }
#endif

    return err == X502_ERR_OK ? sent : err;
}


static int32_t f_ioreq(libusb_device_handle *handle, uint32_t cmd_code, uint32_t param,
                   const void* snd_data, uint32_t snd_size,
                   void* rcv_data, uint32_t recv_size, uint32_t* recvd_size) {
    int32_t err = 0;
#ifdef _WIN32
    uint16_t usbreq[4];
    if (req->RxLength > 0)
        usbreq[0] = 1;
    else
        usbreq[0] = 0;

    if (!err)
    {
        //заполняем последующие 3 слова запроса к драйверу
        usbreq[1] = req->CmdCode & 0xFF;
        usbreq[2] = req->Param & 0xFFFF;
        usbreq[3] = (req->Param >> 16) & 0xFFFF;

        //посылаем запрос
        err = f_usb_ioctl(handle, DIOC_SEND_COMMAND,
                            usbreq, sizeof(usbreq),
                            usbreq[0] ? rcv_data : snd_data,
                            usbreq[0] ? req->RxLength : req->TxLength,
                            rx_size,
                            f_ioctl_tout);

        //при неудаче - пробуем получить код ошибки устройства
        //послав ему запрос LBOOT_USBREQ_GET_LAST_ERROR
        if (err)
        {
            int32_t eres;
            int32_t devres;
            uint32_t errs_rx_size =0;

            usbreq[0] = 1;
            usbreq[1] = E502_LPC_CMD_GET_LAST_ERROR;
            usbreq[2] = usbreq[3] = 0;

            eres = f_usb_ioctl(handle, DIOC_SEND_COMMAND,
                               usbreq, sizeof(usbreq),
                               &devres, sizeof(devres),
                               &errs_rx_size, f_ioctl_tout);
            //если успешно получили код ошибки устройства - то возвращаем его в качестве результата
            if (!eres
                && (errs_rx_size == sizeof(devres))
                && (devres !=0))
            {
                err = devres;
            }
        }
    }
#else
    uint8_t req_type = LIBUSB_REQUEST_TYPE_VENDOR;
    uint16_t len;
    uint8_t* iobuf;
    int usbres = 0;

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
                iobuf, len, E502_USB_REQ_TOUT);
        if (usbres < 0) {
            err = X502_ERR_IOCTL_FAILD;
        } else {
            if (recvd_size!=NULL) {
                *recvd_size = usbres;
            } else if (usbres != len) {
                err = X502_ERR_IOCTL_FAILD;
            }
        }

        //если управляющий запрос не выполнен успешно, то пытаемся получить
        //код ошибки из устройства
        if (usbres < 0) {
            uint32_t devres;
            usbres = libusb_control_transfer(handle,
                    LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR,
                    E502_CM4_CMD_GET_LAST_ERROR,
                    0, 0,
                    (uint8_t*)&devres, sizeof(devres),
                    E502_USB_REQ_TOUT);

            //если успешно получили код ошибки устройства - то возвращаем его в качестве результата
            if ((usbres == sizeof(devres))
                && (devres !=0)) {
                err = devres;
            }
        }
    }
#endif
    return err;
}

static int f_fill_devlist(libusb_device_handle *hnd, t_lpcie_devinfo* info) {
    int32_t err = 0;
    t_lpcie_devinfo_inptr *devinfo_ptr = calloc(1, sizeof(t_lpcie_devinfo_inptr));
    if (devinfo_ptr==NULL)  {
        err = X502_ERR_MEMORY_ALLOC;
    }

    if (!err) {
        t_lboot_devinfo lboot_info;        
        uint32_t id;

        //получаем информацию о устройстве
        err = f_ioreq(hnd, E502_CM4_CMD_GET_MODULE_INFO, 0, NULL, 0, &lboot_info,
                      sizeof(lboot_info), NULL);

        if (!err) {
            err = f_ioreq(hnd, E502_CM4_CMD_FPGA_REG_READ, E502_REGS_ARM_HARD_ID,
                          NULL, 0, &id, sizeof(id), NULL);
        }

        if (!err) {
            strcpy(info->devname, lboot_info.devname);
            strcpy(info->serial, lboot_info.serial);

            devinfo_ptr->iface = &f_usb_iface;
            devinfo_ptr->iface_data = libusb_ref_device(libusb_get_device(hnd));
            info->internal = devinfo_ptr;
        }
    }

    if (err) {
        free(devinfo_ptr);
    }
    return err;
}




LPCIE_EXPORT(int32_t) E502_GetUsbDevInfoList(t_lpcie_devinfo* list, uint32_t size,
                                             uint32_t flags, uint32_t* devcnt) {
    uint32_t curcnt=0;
    int32_t err =0;
    libusb_device **usb_devlist;
    ssize_t usb_devlist_size, i;

    if (!f_lusb_init_done) {
        libusb_init(NULL);
            libusb_set_debug(NULL,  LIBUSB_LOG_LEVEL_WARNING);
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
                int32_t cur_err = libusb_open(usb_devlist[i], &usbhnd);
                if (!cur_err) {
                    t_lpcie_devinfo info;
                    int info_used = 0;
                    memset(&info,0, sizeof(info));


                   if (f_fill_devlist(usbhnd, &info)==0) {
                       /* если нужны только не открытые, то уже открытое
                        * устройство пропускаем */
                       if (!(flags & LPCIE_GETDEVS_FLAGS_ONLY_NOT_OPENED) ||
                               !(info.flags & LPCIE_DEVINFO_FLAGS_DEV_OPENED)) {
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
                       X502_FreeDevInfoList(&info,1);

                   libusb_close(usbhnd);


                }
            }
        }
    }
    libusb_free_device_list(usb_devlist, 1);


    if (devcnt)
        *devcnt = curcnt;

    return err ? err  : curcnt > usb_devlist_size ? (int32_t)usb_devlist_size : (int32_t)curcnt ;
}

LPCIE_EXPORT(int32_t) E502_OpenUsb(t_x502_hnd hnd, const char* serial) {
    return X502_Open(hnd, serial, E502_DEVICE_NAME, E502_GetUsbDevInfoList);
}

LPCIE_EXPORT(int32_t) E502_GetUsbSerialList(char serials[][X502_SERIAL_SIZE], uint32_t size,
                           uint32_t flags, uint32_t *devcnt) {
    uint32_t fnd_cnt=0, e502_cnt=0;
    /* получаем количество устройств, поддерживаемых драйвером lpcie */
    int32_t res = E502_GetUsbDevInfoList(NULL, 0, flags, &fnd_cnt);
    if ((res>=0) && fnd_cnt) {
        t_lpcie_devinfo *info_list = malloc(sizeof(t_lpcie_devinfo)*fnd_cnt);
        if (info_list==NULL) {
            res = X502_ERR_MEMORY_ALLOC;
        } else {
            /* получаем информацию по всем устройствам драйвера lpcie */
            res = E502_GetUsbDevInfoList(info_list, fnd_cnt, flags, NULL);
            if (res>0) {
                int32_t i;
                for (i=0; i < res; i++) {
                    /* проверяем, что это устройство - E502 */
                    if (!strcmp(info_list[i].devname, E502_DEVICE_NAME)) {
                        /* если есть место в списке, то сохраняем серийный номер
                           устройства */
                        if (e502_cnt < size) {
                            memcpy(serials[e502_cnt], info_list[i].serial,
                                   X502_SERIAL_SIZE);
                        }
                        e502_cnt++;
                    }
                }
            }

            X502_FreeDevInfoList(info_list, fnd_cnt);
            free(info_list);
        }
    }

    if (devcnt!=NULL)
        *devcnt = e502_cnt;

    return res < 0 ? res : e502_cnt > size ? (int32_t)size : (int32_t)e502_cnt;
}

