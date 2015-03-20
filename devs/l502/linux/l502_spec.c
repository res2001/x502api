#include "l502_private.h"

#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "lpcie_ioctls.h"


#define LPCIE_CLASS_DIR "/sys/class/lpcie"


typedef struct
{
    int32_t size;
    char* devname;
} t_lpcie_devlst_intptr;


static int32_t f_ioctl(int fd, unsigned long int req, void* val)
{
    return ioctl(fd, req, val) ? L502_ERR_IOCTL_FAILD : 0;
}




int32_t _fpga_reg_write(t_l502_hnd hnd, uint32_t reg, uint32_t val)
{
    t_lpcie_mem_rw mem_wr = {reg,val};
    return f_ioctl(hnd->file, LPCIE_IOCTL_MEMFPGA_WR, &mem_wr);
}


int32_t _fpga_reg_read(t_l502_hnd hnd, uint32_t reg, uint32_t *val)
{
    t_lpcie_mem_rw mem_wr = {reg,0};
    int32_t err = f_ioctl (hnd->file, LPCIE_IOCTL_MEMFPGA_RD, &mem_wr);
    if (!err && val)
        *val = mem_wr.val;
    return err;
}

int32_t _stream_set_params(t_l502_hnd hnd, t_lpcie_stream_ch_params *par)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_STREAM_SET_PARAMS, par);
}

int32_t _stream_start(t_l502_hnd hnd, uint32_t ch, int single)
{
    return f_ioctl(hnd->file, single ? LPCIE_IOCTL_STREAM_START_SINGLE :
                                       LPCIE_IOCTL_STREAM_START, &ch);
}

int32_t _stream_stop(t_l502_hnd hnd, uint32_t ch)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_STREAM_STOP, &ch);
}

int32_t _stream_free(t_l502_hnd hnd, uint32_t ch)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_STREAM_FREE, &ch);
}


int32_t _stream_rdy_size(t_l502_hnd hnd, uint32_t ch, uint32_t *rdy_size)
{
    t_lpcie_get_rdy_par rdy_par = {ch,0};
    int32_t err = f_ioctl (hnd->file, LPCIE_IOCTL_STREAM_GET_RDY_SIZE, &rdy_par);
    if (!err && rdy_size)
        *rdy_size = rdy_par.rdy_size;
    return err;
}

int32_t _cycle_load_start(t_l502_hnd hnd, uint32_t ch, uint32_t size)
{
    t_lpcie_cycle_set_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.size = size;
    return f_ioctl (hnd->file, LPCIE_IOCTL_CYCLE_LOAD, &par);
}

int32_t _cycle_setup(t_l502_hnd hnd, uint32_t ch, uint32_t evt)
{
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (hnd->file, LPCIE_IOCTL_CYCLE_SWITCH, &par);
}

int32_t _cycle_stop(t_l502_hnd hnd, uint32_t ch, uint32_t evt)
{
    t_lpcie_cycle_evt_par par;
    memset(&par, 0, sizeof(par));
    par.ch = ch;
    par.evt = evt;
    return f_ioctl (hnd->file, LPCIE_IOCTL_CYCLE_STOP, &par);
}

int32_t _renew_info(t_l502_hnd hnd)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_RELOAD_DEVINFO, NULL);
}

int32_t _get_drv_ver(t_l502_hnd hnd, uint32_t *ver)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_GET_DRV_VERSION, ver);
}



int32_t _stream_in_read(t_l502_hnd hnd, uint32_t* buff, int32_t size, uint32_t timeout)
{
    struct timeval tmval = {timeout/1000, (timeout%1000)*1000};
    int32_t rcv_size = 0;
    int out = 0;
    int err = L502_ERR_OK;
    while (!out && !err && (rcv_size < size))
    {
        fd_set rd_set;
        int sel = 0;

        FD_ZERO(&rd_set);
        FD_SET(hnd->file, &rd_set);
        sel = select(hnd->file+1, &rd_set, NULL, NULL, &tmval);
        if ((sel > 0) && (FD_ISSET(hnd->file, &rd_set)))
        {
            ssize_t rd = read(hnd->file, &buff[rcv_size], (size-rcv_size)*4);
            if (rd>0)
            {
                rcv_size += rd/4;
            }
            else if (rd == 0)
            {
                out = 1;
            }
            else
            {
                err = L502_ERR_RECV;
            }
        }
        else if (sel==0)
        {
            out = 1;
        }
        else
        {
            err = L502_ERR_RECV;
        }
    }
    return err ? err : rcv_size;
}



int32_t _stream_in_write(t_l502_hnd hnd, const uint32_t* buff, int32_t size,
                         uint32_t timeout)
{
    struct timeval tmval = {timeout/1000, (timeout%1000)*1000};
    int32_t snd_size = 0;
    int32_t err = L502_ERR_OK;
    int out = 0;

    while (!out && !err && (snd_size < size))
    {
        fd_set wr_set;
        int sel = 0;

        FD_ZERO(&wr_set);
        FD_SET(hnd->file, &wr_set);
        sel = select(hnd->file+1, NULL, &wr_set, NULL, &tmval);
        if ((sel > 0) && (FD_ISSET(hnd->file, &wr_set)))
        {
            ssize_t wr = write(hnd->file, &buff[snd_size], (size-snd_size)*4);
            if (wr > 0)
            {
                snd_size += wr/4;
            }
            else if (wr==0)
            {
                out = 1;
            }
            else
            {
                 err = L502_ERR_SEND;
            }
        }
        else if (sel==0)
        {
            out = 1;
        }
        else
        {
            err = L502_ERR_SEND;
        }
    }
    return err ? err : snd_size;
}



static int32_t f_get_file_par(const char *path, char* filebuf,
                              const char* file, char* res, uint32_t req_size,
                              uint32_t* read_size)
{
    int f;
    int32_t err = 0;
    strcpy(filebuf, path);
    strcat(filebuf, file);

    f = open (filebuf, O_RDONLY);
    if (f)
    {
        ssize_t rd = read(f, res, req_size);
        if (rd < 0)
            err = L502_ERR_GET_INFO;
        else
        {
            if (read_size!=NULL)
                *read_size = rd;
        }
        close(f);
    }
    else
    {
        err = L502_ERR_GET_INFO;
    }
    return err;
}


static void f_del_eol(char* str)
{
    for ( ; *str; str++)
    {
        if ((*str=='\n') || (*str=='\r'))
            *str = '\0';
    }
}

static int f_fill_devlist(const char* devname,
                          t_lpcie_devinfo* info)
{
    int err = 0;

    t_lpcie_devlst_intptr* intptr = malloc(sizeof(t_lpcie_devlst_intptr));
    int path_len = strlen(devname) + strlen(LPCIE_CLASS_DIR) + 2;
    char* filename = malloc(path_len + 21);
    char* path = malloc(path_len);

    if (intptr && filename && path)
    {
        sprintf(path, LPCIE_CLASS_DIR "/%s", devname);

        err = f_get_file_par(path, filename, "/name", info->devname,
                       sizeof(info->devname), NULL);
        if (!err)
            f_del_eol(info->devname);

        if (!err)
        {
            /* получаем серийный номер устройства */
            err = f_get_file_par(path, filename, "/sn", info->serial,
                           sizeof(info->serial), NULL);
            if (!err)
                f_del_eol(info->serial);
        }

        /* получаем информацию, открыто ли устройство */
        if (!err)
        {
            char val = '0';
            f_get_file_par(path, filename, "/opened", &val, 1, NULL);
            if (!err && (val!='0'))
                info->flags |= LPCIE_DEVINFO_FLAGS_DEV_OPENED;
        }

        /* получаем информацию, присутствует ли BlackFin */
        if (!err)
        {
            char val = '0';
            if ((f_get_file_par(path, filename, "/bf", &val, 1, NULL)==0)
                    && (val != '0'))
            {
                info->flags |= LPCIE_DEVINFO_FLAGS_BF_PRESENT;
            }

            if ((f_get_file_par(path, filename, "/dac", &val, 1, NULL)==0)
                    && (val != '0'))
            {
                info->flags |= LPCIE_DEVINFO_FLAGS_DAC_PRESENT;
            }

            if ((f_get_file_par(path, filename, "/gal", &val, 1, NULL)==0)
                    && (val != '0'))
            {
                info->flags |= LPCIE_DEVINFO_FLAGS_GAL_PRESENT;
            }
        }

        if (!err)
        {
            char* devname_cpy = malloc(strlen(devname)+1);
            if (devname_cpy!=NULL)
            {
                strcpy(devname_cpy, devname);
                intptr->size = sizeof(t_lpcie_devlst_intptr);
                strcpy(filename, devname);
                intptr->devname = devname_cpy;
                info->intptr = intptr;
            }
            else
            {
                err = L502_ERR_MEMORY_ALLOC;
            }
        }
    }
    else
    {               
        err = L502_ERR_MEMORY_ALLOC;
    }

    if (err)
    {
        free(intptr);
    }

    free(filename);
    free(path);
    return err;
}


int32_t _free_intptr(void* intptr)
{
    t_lpcie_devlst_intptr* ptr = (t_lpcie_devlst_intptr*)intptr;
    if (ptr)
    {
        free(ptr->devname);
        ptr->devname = NULL;
    }
    return 0;
}

int32_t _open_by_list_intptr(t_l502_hnd hnd, void* devlst_intptr)
{
    t_lpcie_devlst_intptr* intptr = (t_lpcie_devlst_intptr*)devlst_intptr;
    int32_t err = 0;
    int path_len = strlen(intptr->devname)+6;

    char *path = malloc(path_len);
    if (path!=NULL)
    {
        int file;
        snprintf(path, path_len, "/dev/%s", intptr->devname);
        file = open(path, O_RDWR);
        if (file > 0)
        {
            hnd->file = file;
        }
        else
        {
            /** @todo Разобрать коды ошибок */
            err = L502_ERR_DEVICE_OPEN;
        }
        free(path);
    }
    else
    {
        err = L502_ERR_MEMORY_ALLOC;
    }
    return err;
}

int32_t _dev_close(t_l502_hnd hnd)
{
    close(hnd->file);
    return 0;
}



LPCIE_EXPORT(int32_t) LPCIE_GetDevInfoList(t_lpcie_devinfo* list, uint32_t size, uint32_t flags, uint32_t* devcnt)
{
    uint32_t curcnt=0;
    int32_t err =0;
    /* все устройства, поддерживаемые драйвером LPCIE, создают папку
       в директории класса устройств lpcie*/
    DIR* class_dir = opendir(LPCIE_CLASS_DIR);
    if (class_dir!=NULL)
    {
        struct dirent *dev_ent;
        /* читаем все записи в директории класса */
        while ((dev_ent=readdir(class_dir))!=NULL)
        {
            /* проверяем, что имя начинается с lpcie */
            if (!memcmp(dev_ent->d_name, "lpcie", sizeof("lpcie")-1))
            {
                t_lpcie_devinfo info;
                int detail_used = 0;
                memset(&info,0, sizeof(info));

                /* получаем информацию о устройстве из служебных файлов,
                 * предоставляемых драйвером */
                if (f_fill_devlist(dev_ent->d_name, &info)==0)
                {
                    /* если нужны только не открытые, то уже открытое
                     * устройство пропускаем */
                    if (!(flags & LPCIE_GETDEVS_FLAGS_ONLY_NOT_OPENED) ||
                            !(info.flags & LPCIE_DEVINFO_FLAGS_DEV_OPENED))
                    {
                        /* если есть место в списке - то сохраняем
                         * полученную информацию */
                        if ((list!=NULL) && (curcnt < size))
                        {
                            list[curcnt] = info;
                            detail_used = 1;
                        }
                        curcnt++;
                    }
                }

                if (!detail_used)
                    LPCIE_FreeDevInfoList(&info,1);
            }
        }
        closedir(class_dir);
    }

    if (devcnt)
        *devcnt = curcnt;

    return err ? err  : curcnt > size ? (int32_t)size : (int32_t)curcnt ;
}


LPCIE_EXPORT(int32_t) L502_Shutdown(t_l502_hnd hnd)
{
    return f_ioctl(hnd->file, LPCIE_IOCTL_POWER_DONW, NULL);
}
