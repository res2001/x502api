#include "../l502api_private.h"
#include "../lpcie_ioctls.h"
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>



#define LPCIE_CLASS_DIR "/sys/class/lpcie"


static int32_t f_ioctl(int fd, unsigned long int req, void* val) {
    return ioctl(fd, req, val) ? X502_ERR_IOCTL_FAILD : X502_ERR_OK;
}




int32_t l502_port_fpga_reg_write(t_x502_hnd hnd, uint32_t reg, uint32_t val) {
    t_lpcie_mem_rw mem_wr = {reg,val};
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_MEMFPGA_WR, &mem_wr);
}


int32_t l502_port_fpga_reg_read(t_x502_hnd hnd, uint32_t reg, uint32_t *val) {
    t_lpcie_mem_rw mem_wr = {reg,0};
    int32_t err = f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_MEMFPGA_RD, &mem_wr);
    if ((err == X502_ERR_OK) && (val != NULL))
        *val = mem_wr.val;
    return err;
}

int32_t l502_port_stream_set_params(t_x502_hnd hnd, t_lpcie_stream_ch_params *par) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_SET_PARAMS, par);
}

int32_t l502_port_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t single) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), single ? LPCIE_IOCTL_STREAM_START_SINGLE :
                                             LPCIE_IOCTL_STREAM_START, &ch);
}

int32_t l502_port_stream_stop(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_STOP, &ch);
}

int32_t l502_port_stream_free(t_x502_hnd hnd, uint32_t ch, uint32_t flags) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_FREE, &ch);
}


int32_t l502_port_stream_rdy_size(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_size) {
    t_lpcie_get_rdy_par rdy_par = {ch,0};
    int32_t err = f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_STREAM_GET_RDY_SIZE, &rdy_par);
    if ((err == X502_ERR_OK) && (rdy_size != NULL))
        *rdy_size = rdy_par.rdy_size;
    return err;
}

int32_t l502_port_cycle_load_start(t_x502_hnd hnd, uint32_t ch, uint32_t size) {
    t_lpcie_cycle_set_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.size = size;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_LOAD, &par);
}

int32_t l502_port_cycle_setup(t_x502_hnd hnd, uint32_t ch, uint32_t evt) {
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_SWITCH, &par);
}

int32_t l502_port_cycle_stop(t_x502_hnd hnd, uint32_t ch, uint32_t evt) {
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_CYCLE_STOP, &par);
}

int32_t l502_port_renew_info(t_x502_hnd hnd) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_RELOAD_DEVINFO, NULL);
}

int32_t l502_port_get_drv_ver(t_x502_hnd hnd, uint32_t *ver) {
    return f_ioctl(L502_PCI_IFACE_FILE(hnd), LPCIE_IOCTL_GET_DRV_VERSION, ver);
}


int32_t l502_port_stream_read(t_x502_hnd hnd, uint32_t *buff, uint32_t size, uint32_t timeout) {
    struct timeval tmval = {timeout/1000, (timeout%1000)*1000};
    uint32_t rcv_size = 0;
    int out = 0;
    int32_t err = X502_ERR_OK;
    while (!out && (err == X502_ERR_OK) && (rcv_size < size)) {
        fd_set rd_set;
        int sel = 0;

        FD_ZERO(&rd_set);
        FD_SET(L502_PCI_IFACE_FILE(hnd), &rd_set);
        sel = select(L502_PCI_IFACE_FILE(hnd)+1, &rd_set, NULL, NULL, &tmval);
        if ((sel > 0) && (FD_ISSET(L502_PCI_IFACE_FILE(hnd), &rd_set))) {
            ssize_t rd = read(L502_PCI_IFACE_FILE(hnd), &buff[rcv_size], (size-rcv_size)*4);
            if (rd>0) {
                rcv_size += rd/4;
            } else if (rd == 0) {
                out = 1;
            } else {
                err = X502_ERR_RECV;
            }
        } else if (sel==0) {
            out = 1;
        } else {
            err = X502_ERR_RECV;
        }
    }
    return err != X502_ERR_OK ? err : (int32_t)rcv_size;
}


int32_t l502_port_stream_write(t_x502_hnd hnd, const uint32_t *buff, uint32_t size,
                                    uint32_t timeout) {
    struct timeval tmval = {timeout/1000, (timeout%1000)*1000};
    uint32_t snd_size = 0;
    int32_t err = X502_ERR_OK;
    int out = 0;

    while (!out && (err == X502_ERR_OK) && (snd_size < size)) {
        fd_set wr_set;
        int sel = 0;

        FD_ZERO(&wr_set);
        FD_SET(L502_PCI_IFACE_FILE(hnd), &wr_set);
        sel = select(L502_PCI_IFACE_FILE(hnd)+1, NULL, &wr_set, NULL, &tmval);
        if ((sel > 0) && (FD_ISSET(L502_PCI_IFACE_FILE(hnd), &wr_set))) {
            ssize_t wr = write(L502_PCI_IFACE_FILE(hnd), &buff[snd_size], (size-snd_size)*4);
            if (wr > 0) {
                snd_size += wr/4;
            } else if (wr==0) {
                out = 1;
            } else {
                err = X502_ERR_SEND;
            }
        } else if (sel==0) {
            out = 1;
        } else {
            err = X502_ERR_SEND;
        }
    }
    return err != X502_ERR_OK ? err : (int32_t)snd_size;
}


static int32_t f_get_file_par(const char *path, char *filebuf,
                              const char *file, char *res, uint32_t req_size,
                              uint32_t *read_size) {
    int f;
    int32_t err = X502_ERR_OK;
    strcpy(filebuf, path);
    strcat(filebuf, file);

    f = open (filebuf, O_RDONLY);
    if (f == -1) {
        err = X502_ERR_GET_INFO;
    } else {
        ssize_t rd = read(f, res, req_size);
        if (rd < 0) {
            err = X502_ERR_GET_INFO;
        } else {
            if (read_size!=NULL)
                *read_size = rd;
        }
        close(f);
    }
    return err;
}


static void f_del_eol(char *str) {
    for ( ; *str; str++) {
        if ((*str=='\n') || (*str=='\r'))
            *str = '\0';
    }
}

static int32_t f_fill_devlist(const char *devname, t_x502_devrec *info) {
    int32_t err = 0;

    int path_len = strlen(devname) + strlen(LPCIE_CLASS_DIR) + 2;
    char *filename = malloc(path_len + 21);
    char *path = malloc(path_len);

    if ((filename != NULL) && (path != NULL)) {
        sprintf(path, LPCIE_CLASS_DIR "/%s", devname);

        err = f_get_file_par(path, filename, "/name", info->devname,
                       sizeof(info->devname), NULL);
        if (err == X502_ERR_OK)
            f_del_eol(info->devname);

        if (err == X502_ERR_OK) {
            /* получаем серийный номер устройства */
            err = f_get_file_par(path, filename, "/sn", info->serial,
                                 sizeof(info->serial), NULL);
            if (err == X502_ERR_OK)
                f_del_eol(info->serial);
        }

        /* получаем информацию, открыто ли устройство */
        if (err == X502_ERR_OK) {
            char val = '0';
            err = f_get_file_par(path, filename, "/opened", &val, 1, NULL);
            if ((err == X502_ERR_OK) && (val!='0'))
                info->flags |= X502_DEVFLAGS_DEVREC_OPENED;
        }

        /* получаем информацию, присутствует ли BlackFin */
        if (err == X502_ERR_OK) {
            char val = '0';
            if ((f_get_file_par(path, filename, "/bf", &val, 1, NULL) == X502_ERR_OK)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_BF_PRESENT;
            }

            if ((f_get_file_par(path, filename, "/dac", &val, 1, NULL) == X502_ERR_OK)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_DAC_PRESENT;
            }

            if ((f_get_file_par(path, filename, "/gal", &val, 1, NULL) == X502_ERR_OK)
                    && (val != '0')) {
                info->flags |= X502_DEVFLAGS_GAL_PRESENT;
            }
        }

        if (err == X502_ERR_OK) {
            char *devname_cpy = malloc(strlen(devname)+1);
            if (devname_cpy!=NULL) {
                strcpy(devname_cpy, devname);
                err = l502_devlist_gen(info, devname_cpy);
                if (err != X502_ERR_OK) {
                    free(devname_cpy);
                }
            } else {
                err = X502_ERR_MEMORY_ALLOC;
            }
        }
    } else {
        err = X502_ERR_MEMORY_ALLOC;
    }

    free(filename);
    free(path);
    return err;
}


int32_t l502_port_free_iface_data(void *intptr) {
    free(intptr);
    return X502_ERR_OK;
}

int32_t l502_port_open(t_x502_hnd hnd, const t_x502_devrec *devinfo) {
    const char *devname = (const char *)devinfo->internal->iface_data;
    int32_t err = X502_ERR_OK;
    int path_len = strlen(devname)+6;
    char *path = malloc(path_len);

    if (path != NULL) {
        int file;
        snprintf(path, path_len, "/dev/%s", devname);
        file = open(path, O_RDWR);
        if (file != -1) {
            hnd->iface_data = malloc(sizeof(t_pci_iface_data));
            if (hnd->iface_data == NULL) {
                err = X502_ERR_MEMORY_ALLOC;
                close(file);
            } else {
                L502_PCI_IFACE_FILE(hnd) = file;
            }
        } else {
            /** @todo Разобрать коды ошибок */
            err = X502_ERR_DEVICE_OPEN;
        }
        free(path);
    } else {
        err = X502_ERR_MEMORY_ALLOC;
    }
    return err;
}

int32_t l502_port_close(t_x502_hnd hnd) {
    if (hnd->iface_data !=NULL) {
        close(L502_PCI_IFACE_FILE(hnd));
    }
    return X502_ERR_OK;
}



X502_EXPORT(int32_t) L502_GetDevRecordsList(t_x502_devrec *list, uint32_t size,
                                            uint32_t flags, uint32_t *devcnt) {
    uint32_t curcnt = 0;
    int32_t err = X502_ERR_OK;
    /* все устройства, поддерживаемые драйвером LPCIE, создают папку
       в директории класса устройств lpcie*/
    DIR *class_dir = opendir(LPCIE_CLASS_DIR);
    if (class_dir!=NULL) {
        struct dirent *dev_ent;
        /* читаем все записи в директории класса */
        while ((dev_ent=readdir(class_dir))!=NULL) {
            /* проверяем, что имя начинается с lpcie */
            if (!memcmp(dev_ent->d_name, "lpcie", sizeof("lpcie")-1)) {
                t_x502_devrec info;
                int info_used = 0;
                X502_DevRecordInit(&info);

                /* получаем информацию о устройстве из служебных файлов,
                 * предоставляемых драйвером */
                if (f_fill_devlist(dev_ent->d_name, &info) == X502_ERR_OK) {
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
        }
        closedir(class_dir);
    }

    if (devcnt != NULL)
        *devcnt = curcnt;

    return err != X502_ERR_OK ? err  : curcnt > size ? (int32_t)size : (int32_t)curcnt ;
}
