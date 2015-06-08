#include <Windows.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <stddef.h>
#include <tchar.h>

#include "../l502api_private.h"

#include <winioctl.h>



/* GUID интерфейса устройств lpcie для связи приложения с драйвером */
DEFINE_GUID (GUID_LPCIE_INTERFACE,
   0x53869b9a, 0x7875, 0x4fd3, 0x9e, 0x04, 0xbe, 0xc8, 0x1a, 0x92, 0xf9, 0xa9);

#define L502_IOCTL_TIMEOUT  500


typedef struct {
    int32_t size;
    SP_DEVICE_INTERFACE_DETAIL_DATA *intf_detail;
} t_lpcie_devlst_intptr;


static int32_t f_ioctl(HANDLE hDevice,
                        uint32_t dwIoControlCode,        // control code of operation to perform
                        void* lpInBuffer,            // pointer to buffer to supply input data
                        uint32_t nInBufferSize,            // size of input buffer in bytes
                        void* lpOutBuffer,            // pointer to buffer to receive output data
                        uint32_t nOutBufferSize,        // size of output buffer in bytes
                        uint32_t* rx_size,
                        uint32_t TimeOut) {             // таймаут в мс
    uint32_t RealBytesTransferred;
    uint32_t BytesReturned;
    OVERLAPPED Ov;
    int32_t err = X502_ERR_OK;
    uint32_t syserr = 0;


    // инициализируем OVERLAPPED структуру
    memset(&Ov, 0x0, sizeof(OVERLAPPED));
    // создаём событие для асинхронного запроса
    Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
    if(!Ov.hEvent) {
        err = X502_ERR_MEMORY_ALLOC;
    } else {
        // посылаем требуемый запрос
        if(!DeviceIoControl(    hDevice, dwIoControlCode,
                                lpInBuffer, nInBufferSize,
                                lpOutBuffer, nOutBufferSize,
                                &BytesReturned, &Ov)) {
            syserr = GetLastError();
            if(syserr != ERROR_IO_PENDING) {
                CloseHandle(Ov.hEvent);
                err =  X502_ERR_IOCTL_FAILD;
                GetOverlappedResult(hDevice, &Ov, &RealBytesTransferred, TRUE);
            }
        }
    }

    if (err == X502_ERR_OK) {
        // ждём окончания выполнения запроса
        if (WaitForSingleObject(Ov.hEvent, TimeOut) == WAIT_TIMEOUT) {
            CancelIo(hDevice);
            CloseHandle(Ov.hEvent);
            err = X502_ERR_IOCTL_TIMEOUT;
            GetOverlappedResult(hDevice, &Ov, &RealBytesTransferred, TRUE);
        }
    }
    // попробуем получить кол-во реально переданных байт данных
    if (err == X502_ERR_OK) {
        if(!GetOverlappedResult(hDevice, &Ov, &RealBytesTransferred, TRUE)) {
            syserr = GetLastError();
            CancelIo(hDevice);
            CloseHandle(Ov.hEvent);
            err = X502_ERR_IOCTL_FAILD;
        } else if(nOutBufferSize != RealBytesTransferred) {
            CancelIo(hDevice);
            CloseHandle(Ov.hEvent);
        } else {
            CloseHandle(Ov.hEvent);
        }
    }

    if ((err == X502_ERR_OK) && (rx_size != NULL)) {
        *rx_size = RealBytesTransferred;
    }

    if (err==X502_ERR_IOCTL_FAILD) {
        if (syserr == ERROR_NO_SYSTEM_RESOURCES)
            err = X502_ERR_INSUFFICIENT_SYSTEM_RESOURCES;
    }
    return err;
}


int32_t l502_port_fpga_reg_write(t_x502_hnd hnd, uint32_t reg, uint32_t val) {
    t_lpcie_mem_rw mem_wr = {reg,val};
    return f_ioctl (L502_PCI_IFACE_FILE(hnd),
                    LPCIE_IOCTL_MEMFPGA_WR, &mem_wr, sizeof(mem_wr),
                    NULL, 0, NULL, L502_IOCTL_TIMEOUT) ?
                    X502_ERR_FPGA_REG_READ : 0;
}


int32_t l502_port_fpga_reg_read(t_x502_hnd hnd, uint32_t reg, uint32_t *val) {
    uint32_t rd_val;
    int32_t err = f_ioctl (L502_PCI_IFACE_FILE(hnd),
                           LPCIE_IOCTL_MEMFPGA_RD, &reg, sizeof(reg),
                           &rd_val, sizeof(rd_val), NULL, L502_IOCTL_TIMEOUT);
    if (err != X502_ERR_OK) {
        err = X502_ERR_FPGA_REG_READ;
    } else if (val != NULL) {
        *val = rd_val;
    }
    return err;
}

int32_t l502_port_stream_set_params(t_x502_hnd hnd, t_lpcie_stream_ch_params *par) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_SET_PARAMS, par,
                   sizeof(t_lpcie_stream_ch_params), NULL, 0, NULL,
                   L502_IOCTL_TIMEOUT);   
}

int32_t l502_port_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t single) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), single ? LPCIE_IOCTL_STREAM_START_SINGLE :
                                       LPCIE_IOCTL_STREAM_START,
                   &ch, sizeof(ch), NULL, 0, NULL,
                   L502_IOCTL_TIMEOUT);
}

int32_t l502_port_stream_stop(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_STOP, &ch, sizeof(ch), NULL, 0, NULL,
                   L502_IOCTL_TIMEOUT);
}

int32_t l502_port_stream_free(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_FREE, &ch, sizeof(ch), NULL, 0, NULL,
                   L502_IOCTL_TIMEOUT);
}


int32_t l502_port_stream_rdy_size(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_size) {
    uint32_t rd_val;
    int32_t err = f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_GET_RDY_SIZE, &ch, sizeof(ch),
                          &rd_val, sizeof(rd_val), NULL, L502_IOCTL_TIMEOUT);
    if ((err == X502_ERR_OK) && (rdy_size != NULL))
        *rdy_size = rd_val;
    return err;
}



int32_t l502_port_cycle_load_start(t_x502_hnd hnd, uint32_t ch, uint32_t size) {
    t_lpcie_cycle_set_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.size = size;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_LOAD, &par, sizeof(par), NULL, 0,
                     NULL, L502_IOCTL_TIMEOUT);
}

int32_t l502_port_cycle_setup(t_x502_hnd hnd, uint32_t ch, uint32_t evt) {
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_SWITCH, &par, sizeof(par), NULL, 0,
                    NULL, L502_IOCTL_TIMEOUT);
}

int32_t l502_port_cycle_stop(t_x502_hnd hnd, uint32_t ch, uint32_t evt) {
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_STOP, &par, sizeof(par), NULL, 0,
                    NULL, L502_IOCTL_TIMEOUT);
}

int32_t l502_port_renew_info(t_x502_hnd hnd) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_RELOAD_DEVINFO, NULL, 0, NULL, 0, NULL,
                   L502_IOCTL_TIMEOUT);
}

int32_t l502_port_get_drv_ver(t_x502_hnd hnd, uint32_t *ver) {
    uint32_t rd_val;
    int32_t err = f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_GET_DRV_VERSION, NULL, 0,
                          &rd_val, sizeof(rd_val), NULL, L502_IOCTL_TIMEOUT);
    if ((err == X502_ERR_OK) && (ver != NULL))
        *ver = rd_val;
    return err;
}


int32_t l502_port_stream_read(t_x502_hnd hnd, uint32_t* buff, uint32_t size, uint32_t timeout) {
    int send_size = 4*size;
    uint32_t NumberOfBytesRead = 0;
    int32_t err = X502_ERR_OK;
    OVERLAPPED Ov;

    // инициализируем OVERLAPPED структуру
    memset(&Ov, 0, sizeof(OVERLAPPED));
    Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
    if(!Ov.hEvent) {
        err = X502_ERR_MEMORY_ALLOC;
    }

    if (err == X502_ERR_OK) {
        // посылаем асинхронный запрос на сбор необходимого кол-ва данных
        if(!ReadFile(L502_PCI_IFACE_FILE(hnd), buff, send_size, NULL, &Ov)) {
            if(GetLastError() != ERROR_IO_PENDING) {
                CancelIo(L502_PCI_IFACE_FILE(hnd));
                CloseHandle(Ov.hEvent);
                err = X502_ERR_RECV;
                GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE);
            }
        }
    }
    // ждём окончания выполнения запроса
    if (err == X502_ERR_OK) {
        if (WaitForSingleObject(Ov.hEvent, timeout) == WAIT_TIMEOUT) {
            CancelIo(L502_PCI_IFACE_FILE(hnd));
            CloseHandle(Ov.hEvent);
            GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE);
        } else if(!GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE)) {
            CancelIo(L502_PCI_IFACE_FILE(hnd));
            CloseHandle(Ov.hEvent);
            err = X502_ERR_RECV;
        } else {
            CloseHandle(Ov.hEvent);
        }
    }
    return err != X502_ERR_OK ? err : NumberOfBytesRead/4;
}



int32_t l502_port_stream_write(t_x502_hnd hnd, const uint32_t* buff,
                               uint32_t size, uint32_t timeout) {
    int send_size = 4*size;
    uint32_t NumberOfBytesRead = 0;
    int32_t err = X502_ERR_OK;
    OVERLAPPED Ov;

    // инициализируем OVERLAPPED структуру
    memset(&Ov, 0, sizeof(OVERLAPPED));
    Ov.hEvent = CreateEvent(NULL, FALSE , FALSE, NULL);
    if(!Ov.hEvent) {
        err = X502_ERR_MEMORY_ALLOC;
    }
    if (err == X502_ERR_OK) {
        // посылаем асинхронный запрос на сбор необходимого кол-ва данных
        if(!WriteFile(L502_PCI_IFACE_FILE(hnd), buff, send_size, NULL, &Ov)) {
            if(GetLastError() != ERROR_IO_PENDING) {
                CancelIo(L502_PCI_IFACE_FILE(hnd));
                CloseHandle(Ov.hEvent);
                err = X502_ERR_SEND;
                GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE);
            }
        }
    }
    // ждём окончания выполнения запроса
    if (err == X502_ERR_OK) {
        if (WaitForSingleObject(Ov.hEvent, timeout) == WAIT_TIMEOUT) {
            CancelIo(L502_PCI_IFACE_FILE(hnd));
            CloseHandle(Ov.hEvent);
            GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE);
        } else if(!GetOverlappedResult(L502_PCI_IFACE_FILE(hnd), &Ov, &NumberOfBytesRead, TRUE)) {
            CancelIo(L502_PCI_IFACE_FILE(hnd));
            CloseHandle(Ov.hEvent);
            err = X502_ERR_SEND;
        } else {
            CloseHandle(Ov.hEvent);
        }
    }
    return err != X502_ERR_OK ? err : NumberOfBytesRead/4;
}



static int32_t f_get_file_par(SP_DEVICE_INTERFACE_DETAIL_DATA *detail, TCHAR* filebuf,
                              const TCHAR* file, char* res, uint32_t req_size,
                              uint32_t* read_size) {
    HANDLE ifile;
    int32_t err = 0;
    _tcscpy(filebuf, TEXT(detail->DevicePath));
    _tcscat(filebuf, file);

    ifile = CreateFile(filebuf, GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL, OPEN_EXISTING,
                       0, NULL);
    if (ifile != INVALID_HANDLE_VALUE) {
        DWORD read_cnt=0;
        if (!ReadFile(ifile, res, req_size, &read_cnt, NULL)) {
            err = X502_ERR_GET_INFO;
        } else {
            if (read_size!=NULL)
                *read_size = read_cnt;
        }
        CloseHandle(ifile);
    } else {
        err = X502_ERR_GET_INFO;
    }
    return err;
}




static int f_fill_devlist(SP_DEVICE_INTERFACE_DETAIL_DATA *detail,
                          t_x502_devrec *info) {
    int32_t err = X502_ERR_OK;
    TCHAR *filename = malloc(sizeof(TCHAR)*21 + _tcslen(detail->DevicePath));

    if (filename == NULL) {
        err = X502_ERR_MEMORY_ALLOC;
    } else {
        err = f_get_file_par(detail, filename, TEXT("\\name"), info->devname,
                       sizeof(info->devname), NULL);
        if (err == X502_ERR_OK) {
            /* получаем серийный номер устройства */
            err = f_get_file_par(detail, filename, TEXT("\\sn"), info->serial,
                           sizeof(info->serial), NULL);
        }

        /* получаем информацию, открыто ли устройство */
        if (err == X502_ERR_OK) {
            char val = '0';
            f_get_file_par(detail, filename, TEXT("\\opened"), &val, 1, NULL);
            if (!err && (val!='0'))
                info->flags |= X502_DEVFLAGS_DEVREC_OPENED;
        }

        /* получаем информацию, присутствует ли BlackFin */
        if (err == X502_ERR_OK) {
            char val = '0';
            if ((f_get_file_par(detail, filename, TEXT("\\bf"), &val, 1, NULL)==0)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_BF_PRESENT;
            }

            if ((f_get_file_par(detail, filename, TEXT("\\dac"), &val, 1, NULL)==0)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_DAC_PRESENT;
            }

            if ((f_get_file_par(detail, filename, TEXT("\\gal"), &val, 1, NULL)==0)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_GAL_PRESENT;
            }
        }

        if (err == X502_ERR_OK) {
            err = l502_devlist_gen(info, detail);
        }
        free(filename);
    }
    return err;
}

int32_t l502_port_free_iface_data(void *intptr) {
    free(intptr);
    return X502_ERR_OK;
}


int32_t l502_port_open(t_x502_hnd hnd, const t_x502_devrec *devinfo) {
    SP_DEVICE_INTERFACE_DETAIL_DATA *detail = (SP_DEVICE_INTERFACE_DETAIL_DATA *)devinfo->internal->iface_data;
    int32_t err = X502_ERR_OK;
    HANDLE file = CreateFile(detail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL, OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        hnd->iface_data = malloc(sizeof(t_pci_iface_data));
        if (hnd->iface_data == NULL) {
            err = X502_ERR_MEMORY_ALLOC;
            CloseHandle(file);
        } else {
            L502_PCI_IFACE_FILE(hnd) = file;
        }
    } else {
        DWORD syserr = GetLastError();
        if (syserr == ERROR_ACCESS_DENIED) {
            err = X502_ERR_DEVICE_ACCESS_DENIED;
        } else if (syserr == ERROR_FILE_NOT_FOUND) {
            err = X502_ERR_DEVICE_NOT_FOUND;
        } else {
            err = X502_ERR_DEVICE_OPEN;
        }
    }
    return err;
}

int32_t l502_port_close(t_x502_hnd hnd) {
    if (hnd->iface_data !=NULL) {
        CloseHandle(L502_PCI_IFACE_FILE(hnd));        
        free(hnd->iface_data);
        hnd->iface_data = NULL;
    }
    return X502_ERR_OK;
}



X502_EXPORT(int32_t) L502_GetDevRecordsList(t_x502_devrec *list, uint32_t size,
                                            uint32_t flags, uint32_t *devcnt) {
    HDEVINFO infoSet;
    SP_DEVINFO_DATA  infoData;
    DWORD index=0;
    uint32_t curcnt=0;
    int32_t err = X502_ERR_OK;


    infoData.cbSize = sizeof(SP_DEVINFO_DATA);

    /* получаем список устройств с подержкой интерфейса lpcie */
    infoSet = SetupDiGetClassDevs(&GUID_LPCIE_INTERFACE,NULL,
                                  NULL,
                                  DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    /* проходимся по всем устройствам из списка */
    while (SetupDiEnumDeviceInfo(infoSet, index, &infoData)) {
        SP_DEVICE_INTERFACE_DATA intfData;
        intfData.cbSize = sizeof(intfData);
        /* получаем информацию о интерфейсе */
        if (SetupDiEnumDeviceInterfaces(infoSet, &infoData, &GUID_LPCIE_INTERFACE,
                                        0, &intfData)) {
            DWORD req_size;
            /* узнаем резмер детальной информации о интерфейсе (нужна для
                получения имени устройства) */
            if (!SetupDiGetDeviceInterfaceDetail(infoSet, &intfData, NULL,
                                                 0, &req_size, NULL)
                    && (GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
                SP_DEVICE_INTERFACE_DETAIL_DATA* detail =
                        (SP_DEVICE_INTERFACE_DETAIL_DATA*) malloc(req_size);

                if (detail != NULL) {
                    int detail_used = 0;

                    /* пытаемся получить всю информацию */
                    detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

                    if (SetupDiGetDeviceInterfaceDetail(infoSet, &intfData,
                                                              detail, req_size,
                                                              NULL, NULL)) {
                        t_x502_devrec info;
                        int info_used = 0;
                        detail_used = 1;

                        X502_DevRecordInit(&info);

                        /* получаем информацию о устройстве из служебных файлов,
                         * предоставляемых драйвером */
                        if (f_fill_devlist(detail, &info) == X502_ERR_OK) {
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
                    }

                    if (!detail_used) {
                        free(detail);
                    }
                } else {
                    err = X502_ERR_MEMORY_ALLOC;
                }
            }
        }
        index++;
    }


    if (infoSet != NULL) {
        SetupDiDestroyDeviceInfoList(infoSet);
    }

    if (devcnt != NULL)
        *devcnt = curcnt;

    return err != X502_ERR_OK ? err  : curcnt > size ? size : curcnt ;
}
