#include "e502api_private.h"
#include "e502_tcp_protocol.h"
#include "ltimer.h"
#include "lboot_req.h"
#include "e502_fpga_regs.h"

#include <stdlib.h>
#include <string.h>

#if defined _WIN32
    #include <winsock2.h>
    typedef int socklen_t;
    typedef SOCKET t_socket;

    #define SOCK_ERR_SIGBREAK() 0

    #define L_SOCK_LAST_ERR_BLOCK() (WSAEWOULDBLOCK == WSAGetLastError())
    #define L_SOCK_LAST_ERR_RESET() (WSAECONNRESET  == WSAGetLastError())
#else
    #include <errno.h>

    #include <stdlib.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <arpa/inet.h>

    #include <sys/ioctl.h>
    //#include <stropts.h>

    #include <linux/sockios.h>
    #include <asm-generic/ioctls.h>



    typedef int t_socket;

    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SOCK_ERR_SIGBREAK() (EINTR == errno)

    #define L_SOCK_LAST_ERR_BLOCK() ((EAGAIN==errno) || (EWOULDBLOCK==errno))
#ifdef ECONNRESET
    #define L_SOCK_LAST_ERR_RESET() (ECONNRESET==errno)
#endif

    #define closesocket(sock) close(sock)
#endif

#ifndef MSG_NOSIGNAL
    #define MSG_NOSIGNAL 0
#endif

#define E502_TCP_REQ_TOUT           5000
#define E502_TCP_STOP_WAIT_TOUT     5000



#define TCP_CTL_REQ_MAX_SIZE   512
#define TCP_IN_STREAM_BUF_MIN  128


typedef struct {
    uint16_t cmd_port;
    uint32_t ip_addr;
    uint32_t open_tout;
    uint32_t flags;
} t_tcp_devinfo_data;


typedef struct {
    t_socket cmd_sock;
    t_socket data_sock;
    uint32_t ip_addr;
    uint32_t open_tout;

    uint32_t recv_part_wrd; /**< принятое неполностью слово */
    uint32_t send_part_wrd; /**< переданное неполностью слово */
    uint8_t  recv_part_size;  /**< кол-во принятых байт в последнем неполном слове */
    uint8_t  send_part_size;  /**< кол-во неотправленных байт в последнем переданном не полностью слове */
} t_tcp_iface_data;

static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr);
static int32_t f_iface_open(t_x502_hnd hnd, const t_x502_devrec *devinfo);
static int32_t f_iface_close(t_x502_hnd hnd);
static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params);
static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t signle);
static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch);
static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch);
static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout);
static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) ;
static int32_t f_iface_stream_get_rdy_cnt(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_cnt);

static int32_t f_iface_gen_ioctl(t_x502_hnd hnd, uint32_t cmd_code, uint32_t param,
                                 const void* snd_data, uint32_t snd_size,
                                 void* rcv_data, uint32_t recv_size, uint32_t* recvd_size,
                                 uint32_t tout);

static const t_x502_dev_iface f_tcp_iface = {
    E502_REGS_ARM_HARD_ID,
    TCP_IN_STREAM_BUF_MIN,
    TCP_CTL_REQ_MAX_SIZE,
    TCP_CTL_REQ_MAX_SIZE/4,
    TCP_CTL_REQ_MAX_SIZE, //flash rd size
    TCP_CTL_REQ_MAX_SIZE, //flash wr size
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
    f_iface_gen_ioctl
};


static void f_set_timeval_left(t_ltimer* tmr, struct timeval* tval) {
    t_lclock_ticks left = ltimer_expiration(tmr);
    tval->tv_sec = left / LCLOCK_TICKS_PER_SECOND;
    tval->tv_usec = (left % LCLOCK_TICKS_PER_SECOND) * 1000000/LCLOCK_TICKS_PER_SECOND;
}

static int32_t f_con_sock(t_socket *psock, uint32_t ip_addr, uint16_t port, uint32_t tout) {
    int32_t err = X502_ERR_OK;
    struct sockaddr_in peer;
    int connected = 0;

    t_socket s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
        err = X502_ERR_SOCKET_OPEN;

    /* Переводим сокет в неблокирующий режим работы */
    if (err == X502_ERR_OK) {
#ifdef _WIN32
        ULONG nonblocking = 1;
        if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &nonblocking))
            err = X502_ERR_SOCKET_OPEN;
#else
        int n = fcntl(s, F_GETFL, 0);
        if (fcntl(s, F_SETFL, n|O_NONBLOCK)==-1) {
            err = X502_ERR_SOCKET_OPEN;
        }
#endif
    }

    if (err == X502_ERR_OK) {
        /* заполняем структуру с адресом LTR-сервера */
        memset(&peer, 0, sizeof(peer));
        peer.sin_family = AF_INET;
        peer.sin_port = htons(port);
        peer.sin_addr.s_addr = htonl(ip_addr);
    }

    while (!connected && (err==X502_ERR_OK)) {
        t_ltimer tmr;
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        if (SOCKET_ERROR == connect(s, (struct sockaddr*)&peer, sizeof(peer))) {
            int sockerr = 0;
            fd_set fd_w, fd_e;
#ifdef _WIN32
            if (WSAEWOULDBLOCK != WSAGetLastError()) {
#else
            if (errno != EINPROGRESS)  {
#endif
                err = X502_ERR_SOCKET_OPEN;
            } else {
                struct timeval tval;

                FD_ZERO(&fd_w);
                FD_SET(s, &fd_w);
                FD_ZERO(&fd_e);
                FD_SET(s, &fd_e);

                f_set_timeval_left(&tmr, &tval);

                if (select((int)s+1, NULL, &fd_w, &fd_e, &tval) < 1)
                    err = X502_ERR_CONNECTION_TOUT;
            }

            if (err == X502_ERR_OK) {
                /* судя по man - если произошла ошибка, то сокет становится writable!
                    так что в fd_w тоже нужно проверять ошибку */
                socklen_t optlen = sizeof(sockerr);
                if (SOCKET_ERROR == getsockopt(s, SOL_SOCKET, SO_ERROR,
                                               (char*)&sockerr, &optlen)) {
                    err = X502_ERR_SOCKET_OPEN;
                } else if (sockerr) {
                    err = X502_ERR_SOCKET_OPEN;
                }
            }

            /* проверяем, что соединились успешно */
            if ((err == X502_ERR_OK) && !sockerr && (FD_ISSET(s, &fd_w)))
                connected = 1;
        } else {
            /* удалось соединится без ожидания */
            connected = 1;
        }
    } /* while (!connected && !err) */


    if (err != X502_ERR_OK) {
        if (s!=INVALID_SOCKET) {
            closesocket(s);
        }
    } else if (psock!=NULL) {
        *psock = s;
    }
    return err;
}


static int32_t f_recv(t_socket s, uint8_t *buf, uint32_t size, t_ltimer *ptmr) {
    int32_t err = X502_ERR_OK;
    uint32_t offset = 0;
    int timed_out = 0;
    fd_set fd_r;
    struct timeval tval;

    if ((err == X502_ERR_OK) && (size!=0)) {
        while (!err && !timed_out && (offset < size)) {
            FD_ZERO(&fd_r);
            FD_SET(s, &fd_r);
            f_set_timeval_left(ptmr, &tval);
            switch (select((int)s+1, &fd_r, NULL, NULL, &tval)) {
                case SOCKET_ERROR:
                    /* Если пришел сигнал, то это не ошибка приема.
                     * Но скорее всего управление стоит вернуть сразу, хотя
                     * может сделать опцию... */
                    if (SOCK_ERR_SIGBREAK()) {
                        ltimer_set(ptmr, 0);
                        timed_out = 1;
                    } else {
                        err = X502_ERR_RECV;
                    }
                    break;
                case 0: // таймаут
                    timed_out = 1;
                    break;
                default: { /* дождались готовности на чтение */
                        int res = recv(s, buf + offset, size - offset, 0);
                        if (SOCKET_ERROR == res) {
                            if (!L_SOCK_LAST_ERR_BLOCK()) {
                                err = X502_ERR_RECV;
                            }
                        } else if (0 == res) {
                            /* соединение закрыто */
                            err = X502_ERR_CONNECTION_CLOSED_BY_DEV;
                        } else {
                            offset += res;
                        }
                    }
                    break;
             }
        } /* switch (select(ch->sock+1, &fd_r, NULL, NULL, &tval)) */
    }

    return err ? err : (int32_t)offset;
}

int32_t f_send(t_socket s, const uint8_t *buf, uint32_t size, t_ltimer *ptmr) {
    int32_t err = X502_ERR_OK;
    uint32_t offset = 0;
    int timed_out = 0;
    fd_set fd_w;

    if ((err == X502_ERR_OK) && (size != 0)) {
        while ((err == X502_ERR_OK) && !timed_out && (offset < size)) {
            /* Сначала пробуем сделать запись без ожидания */
            int res = send(s, buf + offset, size - offset, MSG_NOSIGNAL);
            if (res == SOCKET_ERROR) {
                struct timeval tval;
                if (L_SOCK_LAST_ERR_BLOCK()) {
                    /* Надо ждать освобождения сокета */
                    FD_ZERO(&fd_w);
                    FD_SET(s, &fd_w);
                    f_set_timeval_left(ptmr, &tval);
                    switch (select((int)s+1, NULL, &fd_w, NULL, &tval)) {
                        case SOCKET_ERROR:
                            if (SOCK_ERR_SIGBREAK()) {
                                ltimer_set(ptmr, 0);
                                timed_out = 1;
                            } else {
                                err = X502_ERR_SEND;
                            }
                            break;
                        case 0: // таймаут
                            timed_out = 1;
                            break;
                        default:
                            if (ltimer_expired(ptmr))
                                timed_out = 1;
                            break;
                    }
                } else {
                    err = X502_ERR_SEND;
                }
            } else { // no error
                offset += res;
            }
        }
    }

    return (err) ? err : (int)offset;
}


static int32_t f_recv_exact(t_socket s, uint8_t *buf, uint32_t size, t_ltimer *ptmr) {
    int32_t ret = f_recv(s, buf, size, ptmr);
    return ret == (int32_t)size ? X502_ERR_OK : ret < 0 ? ret : X502_ERR_RECV_INSUFFICIENT_WORDS;
}

static int32_t f_send_exact(t_socket s, const uint8_t *buf, uint32_t size, t_ltimer *ptmr) {
    int32_t ret = f_send(s, buf, size, ptmr);
    return ret == (int32_t)size ? X502_ERR_OK : ret < 0 ? ret : X502_ERR_SEND_INSUFFICIENT_WORDS;
}



static int32_t f_iface_gen_ioctl(t_x502_hnd hnd, uint32_t cmd_code, uint32_t param,
                                 const void* snd_data, uint32_t snd_size,
                                 void* rcv_data, uint32_t recv_size, uint32_t* recvd_size, uint32_t tout) {
    t_e502_tcp_cmd_hdr cmd_hdr;
    t_e502_tcp_resp_hdr cmd_resp;
    int32_t err = X502_ERR_OK;
    t_ltimer tmr;
    t_socket s = ((t_tcp_iface_data*)hnd->iface_data)->cmd_sock;

    ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout == 0 ? E502_TCP_REQ_TOUT : tout));
    cmd_hdr.sign = E502_TCP_CMD_SIGNATURE;
    cmd_hdr.cmd = cmd_code;
    cmd_hdr.par = param;
    cmd_hdr.data_len = snd_size;
    cmd_hdr.resp_len = recv_size;

    err = f_send_exact(s, (uint8_t*)&cmd_hdr, E502_TCP_CMD_HDR_SIZE, &tmr);
    if ((err == X502_ERR_OK) && (snd_size > 0)) {
        err = f_send_exact(s, snd_data, snd_size, &tmr);
    }
    if (err == X502_ERR_OK) {
        err = f_recv_exact(s, (uint8_t*)&cmd_resp, E502_TCP_CMD_RESP_SIZE, &tmr);
        if (err == X502_ERR_RECV_INSUFFICIENT_WORDS) {
            err = X502_ERR_NO_CMD_RESPONSE;
        }
    }
    if ((err == X502_ERR_OK) && (cmd_resp.len > 0)) {
        if (cmd_resp.len > recv_size) {
            err = X502_ERR_IOCTL_INVALID_RESP_SIZE;
        } else {
            err = f_recv_exact(s, rcv_data, cmd_resp.len, &tmr);
        }
    }

    if (err == X502_ERR_OK) {
        if (recvd_size != NULL) {
            *recvd_size = cmd_resp.len;
        } else if (cmd_resp.len != recv_size) {
            err = X502_ERR_IOCTL_INVALID_RESP_SIZE;
        }
    }

    if ((err == X502_ERR_OK) && (cmd_resp.res!=0)) {
        err = cmd_resp.res;
    }

    return err;
}

static int32_t f_iface_free_devinfo_ptr(t_x502_devrec_inptr *devinfo_ptr) {
    free(devinfo_ptr->iface_data);
    free(devinfo_ptr);
    return X502_ERR_OK;
}

static int32_t f_iface_open(t_x502_hnd hnd, const t_x502_devrec *devinfo) {
    int32_t err = X502_ERR_OK;
    t_tcp_devinfo_data *devinfo_data = (t_tcp_devinfo_data*)devinfo->internal->iface_data;
    t_socket s;
    err = f_con_sock(&s, devinfo_data->ip_addr, devinfo_data->cmd_port, devinfo_data->open_tout);
    if (err == X502_ERR_OK) {
        int flag = 1;
        if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag))==SOCKET_ERROR)
            err = X502_ERR_SOCKET_OPEN;
    }

    if (err == X502_ERR_OK) {
        t_tcp_iface_data *iface_data = malloc(sizeof(t_tcp_iface_data));
        if (iface_data == NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        } else {
            t_lboot_devinfo lboot_info;
            iface_data->cmd_sock = s;
            iface_data->data_sock = INVALID_SOCKET;
            iface_data->ip_addr = devinfo_data->ip_addr;
            iface_data->open_tout = devinfo_data->open_tout;

            hnd->iface_data = iface_data;

            err = hnd->iface_hnd->gen_ioctl(hnd, E502_CM4_CMD_GET_MODULE_INFO, 0, NULL, 0, &lboot_info,
                                        sizeof(lboot_info), NULL, 0);

            if (err == X502_ERR_OK) {
                if (strcmp(lboot_info.devname, devinfo->devname)) {
                    err = X502_ERR_INVALID_DEVICE;
                } else {
                    strcpy(hnd->info.serial, lboot_info.serial);
                }
            }
        }
    }
    return err;
}

static int32_t f_iface_close(t_x502_hnd hnd) {
    int32_t err = X502_ERR_OK;
    t_tcp_iface_data *tcp_data = (t_tcp_iface_data*)hnd->iface_data;
    if (tcp_data->data_sock!=INVALID_SOCKET) {
        closesocket(tcp_data->data_sock);
        tcp_data->data_sock = INVALID_SOCKET;
    }

    if (tcp_data->cmd_sock!=INVALID_SOCKET) {
        closesocket(tcp_data->cmd_sock);
        tcp_data->cmd_sock = INVALID_SOCKET;
    }
    return err;
}

static int32_t f_iface_stream_cfg(t_x502_hnd hnd, uint32_t ch, t_x502_stream_ch_params *params) {
    int32_t err = X502_ERR_OK;
    t_tcp_iface_data *tcp_data = (t_tcp_iface_data*)hnd->iface_data;
    if (tcp_data->data_sock==INVALID_SOCKET) {
        err = f_iface_gen_ioctl(hnd, E502_CM4_CMD_DROP_DATA_CON, 0, NULL, 0, NULL, 0, NULL, 0);
        if (err == X502_ERR_OK)
            err = f_con_sock(&tcp_data->data_sock, tcp_data->ip_addr, E502_TCP_DEFAULT_DATA_PORT, tcp_data->open_tout);
    }

    if (err==X502_ERR_OK) {
        unsigned buf_size = params->buf_size*4;
        if (ch==X502_STREAM_CH_IN) {
            tcp_data->recv_part_size = 0;
            if (setsockopt(tcp_data->data_sock, SOL_SOCKET, SO_RCVBUF, (char*)&buf_size, sizeof(buf_size))==SOCKET_ERROR) {
                err = X502_ERR_SOCKET_SET_BUF_SIZE;
            }
        } else {
            tcp_data->send_part_size = 0;
            if (setsockopt(tcp_data->data_sock, SOL_SOCKET, SO_SNDBUF, (char*)&buf_size, sizeof(buf_size))==SOCKET_ERROR) {
                err = X502_ERR_SOCKET_SET_BUF_SIZE;
            }
        }
    }

    return err;
}

static int32_t f_iface_stream_start(t_x502_hnd hnd, uint32_t ch, uint32_t signle) {
    int32_t err = 0;

    err = f_iface_gen_ioctl(hnd, E502_CM4_CMD_STREAM_START, (ch<<16),
                            NULL, 0, NULL, 0, NULL, 0);
    return err;
}

static int32_t f_iface_stream_stop(t_x502_hnd hnd, uint32_t ch) {
    int32_t err = 0;
    int32_t running;

    err = hnd->iface_hnd->stream_running(hnd, ch, &running);
    if (!err && running) {
        err = f_iface_gen_ioctl(hnd, E502_CM4_CMD_STREAM_STOP, (ch << 16),
                                      NULL, 0, NULL, 0, NULL, 0);       
    }
    return err;
}

static int32_t f_iface_stream_free(t_x502_hnd hnd, uint32_t ch) {
    int32_t err =  hnd->iface_hnd->stream_stop(hnd, ch);
    if (err == X502_ERR_OK) {
        t_tcp_iface_data *tcp_data = (t_tcp_iface_data *)hnd->iface_data;
        if (tcp_data->data_sock!=INVALID_SOCKET) {
            closesocket(tcp_data->data_sock);
            tcp_data->data_sock = INVALID_SOCKET;
        }
    }
    return err;
}

static int32_t f_iface_stream_read(t_x502_hnd hnd, uint32_t *buf, uint32_t size, uint32_t tout) {
    t_tcp_iface_data *tcp_data = (t_tcp_iface_data*)hnd->iface_data;
    int32_t recvd;
    t_ltimer tmr;

    if (tcp_data->data_sock == INVALID_SOCKET) {
        recvd = X502_ERR_NO_DATA_CONNECTION;
    } else {
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        if (tcp_data->recv_part_size != 0) {
            buf[0] = tcp_data->recv_part_wrd;
        }
        recvd = f_recv(tcp_data->data_sock, (uint8_t*)buf + tcp_data->recv_part_size,
                            size *sizeof(buf[0]) - tcp_data->recv_part_size, &tmr);
        if (recvd > 0) {
            recvd += tcp_data->recv_part_size;
            tcp_data->recv_part_size = recvd % sizeof(buf[0]);
            recvd /= sizeof(buf[0]);
            if (tcp_data->recv_part_size!=0) {
                tcp_data->recv_part_wrd = buf[recvd];
            }
        }
    }
    return recvd;
}

static int32_t f_iface_stream_write(t_x502_hnd hnd, const uint32_t *buf, uint32_t size, uint32_t tout) {
    int32_t sent = 0;
    t_ltimer tmr;
    t_tcp_iface_data *tcp_data = (t_tcp_iface_data*)hnd->iface_data;

    if (tcp_data->data_sock == INVALID_SOCKET) {
        sent = X502_ERR_NO_DATA_CONNECTION;
    } else {
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        /* проверяем, не осталось ли не переданного некратного слова => если осталось
         * то пробуем сперва дослать его */
        if (tcp_data->send_part_size!=0) {
            sent = f_send(tcp_data->data_sock, (uint8_t*)&tcp_data->send_part_wrd,
                          tcp_data->send_part_size, &tmr);
            if (sent >= 0) {
                tcp_data->send_part_size -= (uint8_t)sent;
                if (tcp_data->send_part_size != 0) {
                    tcp_data->send_part_wrd >>= 8*sent;
                }
                sent = 0;
            }
        }

        /* новые данные пересылаем только если старое неполное слово точно ушло */
        if ((sent == 0) && (tcp_data->send_part_size==0)) {
            sent = f_send(tcp_data->data_sock, (uint8_t*)buf, size * sizeof(buf[0]), &tmr);
            if (sent >= 0) {
                /* если не полностью передали последнее слово, то нужно сохранить
                 * остаток слова, чтобы потом передать его */
                tcp_data->send_part_size = sent % sizeof(buf[0]);
                sent /= sizeof(buf[0]);
                if (tcp_data->send_part_size!=0) {
                    tcp_data->send_part_wrd = buf[sent] >> (8*tcp_data->send_part_size);
                    tcp_data->send_part_size = sizeof(buf[0]) - tcp_data->send_part_size;
                    sent++;
                }
            }
        }
    }

    return sent;
}

static int32_t f_iface_stream_get_rdy_cnt(t_x502_hnd hnd, uint32_t ch, uint32_t *rdy_cnt) {
    int32_t err = 0;
    t_tcp_iface_data *tcp_data = (t_tcp_iface_data *)hnd->iface_data;
#ifdef _WIN32
    if (ch == X502_STREAM_CH_IN) {
        u_long val;
        if (ioctlsocket(tcp_data->data_sock, FIONREAD, &val) == SOCKET_ERROR) {
            err = X502_ERR_IOCTL_FAILD;
        } else {
            *rdy_cnt = val/4;
        }
    } else {
        /** @todo */
    }
#else
    if (ch == X502_STREAM_CH_IN) {
        int val;
        if (ioctl(tcp_data->data_sock, SIOCINQ, &val)==-1) {
            err = X502_ERR_IOCTL_FAILD;
        } else {
            *rdy_cnt = val/4;
        }
    } else {
        int buf_len, val;
        socklen_t optlen = sizeof(buf_len);
        if (getsockopt(tcp_data->data_sock, SOL_SOCKET, SO_SNDBUF, (char*)&buf_len, &optlen)==SOCKET_ERROR) {
            err = X502_ERR_IOCTL_FAILD;
        } else if (ioctl(tcp_data->data_sock, SIOCOUTQ, &val)==-1) {
            err = X502_ERR_IOCTL_FAILD;
        } else {
            *rdy_cnt = (buf_len - val)/4;
        }
    }
#endif
    return err;
}

X502_EXPORT(int32_t) E502_MakeDevRecordByIpAddr(t_x502_devrec *devrec, uint32_t ip_addr,
                                               uint32_t flags, uint32_t tout) {
    int32_t err = (devrec == NULL) || (devrec->sign!=X502_DEVREC_SIGN) ?
                X502_ERR_INVALID_DEVICE_RECORD : X502_ERR_OK;

    X502_FreeDevRecordList(devrec, 1);

    if (err==X502_ERR_OK) {
        t_tcp_devinfo_data *devinfo_data = malloc(sizeof(t_tcp_devinfo_data));
        t_x502_devrec_inptr *devinfo_ptr = malloc(sizeof(t_x502_devrec_inptr));

        if ((devinfo_data==NULL) || (devinfo_ptr == NULL)) {
            err = X502_ERR_MEMORY_ALLOC;
        } else {
            strcpy(devrec->devname, E502_DEVICE_NAME);

            devinfo_data->cmd_port = E502_TCP_DEFAULT_CMD_PORT;
            devinfo_data->ip_addr = ip_addr;
            devinfo_data->open_tout = tout;
            devinfo_data->flags = flags;


            devinfo_ptr->iface = &f_tcp_iface;
            devinfo_ptr->iface_data = devinfo_data;


            devrec->internal = devinfo_ptr;
            devrec->iface = X502_IFACE_TCP;
            sprintf(devrec->location, "%d.%d.%d.%d",
                    (ip_addr>>24) & 0xFF,
                    (ip_addr>>16) & 0xFF,
                    (ip_addr>>8) & 0xFF,
                    (ip_addr>>0) & 0xFF);
        }

        if (err != X502_ERR_OK) {
            free(devinfo_data);
            free(devinfo_ptr);
        }
    }
    return err;
}


X502_EXPORT(int32_t) E502_OpenByIpAddr(t_x502_hnd hnd, uint32_t ip_addr, uint32_t flags, uint32_t tout) {
    int32_t err = X502_CHECK_HND(hnd);
    if (err == X502_ERR_OK) {
        t_x502_devrec devinfo;
        X502_DevRecordInit(&devinfo);
        err = E502_MakeDevRecordByIpAddr(&devinfo, ip_addr, flags, tout);
        if (err == X502_ERR_OK) {
            err = X502_OpenByDevRecord(hnd, &devinfo);

            X502_FreeDevRecordList(&devinfo, 1);
        }
    }
    return err;
}


X502_EXPORT(int32_t) E502_GetIpAddr(t_x502_hnd hnd, uint32_t *ip_addr) {
    int32_t err = X502_CHECK_HND_OPENED(hnd);
    if (err == X502_ERR_OK) {
        if (hnd->iface != X502_IFACE_TCP) {
            err = X502_ERR_INVALID_OP_FOR_IFACE;
        } else {
            t_tcp_iface_data *tcp_data = (t_tcp_iface_data *)hnd->iface_data;
            *ip_addr = tcp_data->ip_addr;
        }
    }
    return err;
}
