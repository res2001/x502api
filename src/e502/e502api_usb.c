#include "e502api.h"
#include "x502api_private.h"
#include "e502_cm4_defs.h"
#include "lboot_req.h"
#include "libusb-1.0/libusb.h"
#include "e502_fpga_regs.h"


#include <stdlib.h>
#include <string.h>

#define E502_USB_VID 0x0471
#define E502_USB_PID 0xE502
#define E502_USB_REQ_TOUT 1000





static int f_lusb_init_done = 0;


typedef struct {
    libusb_device_handle *devhnd;

} t_usb_iface_data;


static int32_t f_ioreq(libusb_device_handle *handle, uint32_t cmd_code, uint32_t param,
                   const void* snd_data, uint32_t snd_size,
                   void* rcv_data, uint32_t recv_size, uint32_t* recvd_size);

static int32_t f_iface_open(t_x502_hnd hnd, const t_lpcie_devinfo *devinfo);
static int32_t f_iface_close(t_x502_hnd hnd);
static int32_t f_iface_fpga_read(t_x502_hnd hnd, uint16_t addr, uint32_t *val);
static int32_t f_iface_fpga_write(t_x502_hnd hnd, uint16_t addr, uint32_t val);
static int32_t f_iface_free_devinfo_data(void *devinfo_data);


static const t_x502_dev_iface f_usb_iface = {
    E502_REGS_ARM_HARD_ID,
    f_iface_open,
    f_iface_close,
    f_iface_fpga_read,
    f_iface_fpga_write,
    f_iface_free_devinfo_data
};



static int32_t f_iface_open(t_x502_hnd hnd, const t_lpcie_devinfo *devinfo) {
    int32_t err;
    libusb_device *dev = (libusb_device *)(((t_lpcie_devinfo_inptr*)devinfo->internal)->iface_data);
    libusb_device_handle *devhnd;


    err = libusb_open(dev, &devhnd);
    if (!err) {
        err = libusb_claim_interface(devhnd, 0);
        /** @todo перевести код ошибки в ошибки библиотеки */
    }

    if (!err) {
        t_usb_iface_data *usb_data = calloc(1, sizeof(t_usb_iface_data));
        usb_data->devhnd = devhnd;

        hnd->iface_data = usb_data;
    }

    return err;
}

static int32_t f_iface_close(t_x502_hnd hnd) {
    t_usb_iface_data *usb_data = (t_usb_iface_data*)hnd->iface_data;
    int32_t err = 0;

    err = libusb_release_interface(usb_data->devhnd, 0);
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
    return X502_Open(hnd, serial, "E502", E502_GetUsbDevInfoList);
}
