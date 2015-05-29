#include "dns_sd.h"
#include "ltimer.h"
#include "e502api_tcp_private.h"


#define E502_SVC_REC_SIGN     0xE502CCA5
#define E502_SVC_BROWSE_SIGN  0xE502BBA5

#define SERVICE_CONTEXT_MAX_CNT  1024


#define E502_TCP_SERVICE_NAME       "_lcard_acqdev._tcp"


#define E502_CHECK_SVC_REC(svc) ((svc != NULL) ? (svc)->sign == E502_SVC_REC_SIGN ? X502_ERR_OK \
    : X502_ERR_INVALID_SVC_RECORD_HANDLE : X502_ERR_INVALID_SVC_RECORD_HANDLE)

#define E502_CHECK_BROWSE_CONTEXT(svc) ((svc != NULL) ? (svc)->sign == E502_SVC_BROWSE_SIGN ? X502_ERR_OK \
    : X502_ERR_INVALID_SVC_BROWSE_HANDLE : X502_ERR_INVALID_SVC_BROWSE_HANDLE)

typedef struct st_e502_eth_svc_record {
    uint32_t sign;
    char devname[X502_DEVNAME_SIZE];
    char serial[X502_SERIAL_SIZE];
    char service_name[X502_INSTANCE_NAME_SIZE];
    char *domain;
    char *hosttarget;
    uint16_t port;
    uint32_t iface_idx;
    uint32_t ipv4_addr;
    int op_in_progr;
    int32_t op_err;
} t_svc_record;





typedef struct {
    DNSServiceRef sdref;
    t_e502_eth_svc_event event;
    int init_done;
    t_e502_eth_svc_record_hnd rec;
} t_service_context;

typedef struct st_e502_eth_svc_browse_context {
    uint32_t sign;
    DNSServiceRef main_ref;
    DNSServiceRef browse_ref;
    int socket;
    unsigned svc_cnt;
    t_service_context services[SERVICE_CONTEXT_MAX_CNT];
} t_e502_service_browse_context;

static LINLINE void f_set_timeval_left(t_ltimer *tmr, struct timeval *tval) {
    t_lclock_ticks left = ltimer_expiration(tmr);
    tval->tv_sec = left / LCLOCK_TICKS_PER_SECOND;
    tval->tv_usec = (left % LCLOCK_TICKS_PER_SECOND) * 1000000/LCLOCK_TICKS_PER_SECOND;
}



static t_e502_eth_svc_record_hnd f_service_record_create(void) {
    t_e502_eth_svc_record_hnd svc = calloc(1, sizeof(t_svc_record));
    if (svc != NULL) {
        svc->sign = E502_SVC_REC_SIGN;
    }
    return svc;
}

static t_e502_eth_svc_record_hnd f_service_record_create_copy(t_e502_eth_svc_record_hnd rec) {
    t_e502_eth_svc_record_hnd ret = NULL;
    if (E502_CHECK_SVC_REC(rec) == X502_ERR_OK) {
         ret = f_service_record_create();
         memcpy(ret, rec, sizeof(t_svc_record));
         if (rec->domain != NULL) {
             size_t len = strlen(rec->domain) + 1;
             ret->domain = malloc(len);
             if (ret->domain != NULL) {
                 strcpy(ret->domain, rec->domain);
             } else {
                 E502_EthSvcRecordFree(ret);
                 ret = NULL;
             }
         }
    }
    return ret;
}

X502_EXPORT(int32_t) E502_EthSvcRecordFree(t_e502_eth_svc_record_hnd rec) {
    int32_t err = E502_CHECK_SVC_REC(rec);
    if (err == X502_ERR_OK) {
        if (rec->domain != NULL)
            free(rec->domain);
        if (rec->hosttarget)
            free(rec->hosttarget);
        memset(rec, 0, sizeof(t_svc_record));
        free(rec);
    }
    return err;
}


static void f_svc_context_free(t_service_context *svc_context) {
    if (svc_context->sdref != NULL)
        DNSServiceRefDeallocate(svc_context->sdref);
    if (svc_context->rec != NULL)
        E502_EthSvcRecordFree(svc_context->rec);
    svc_context->event = E502_ETH_SVC_EVENT_NONE;
    svc_context->init_done = 0;
}

static int32_t f_browse_context_free(t_e502_eth_svc_browse_hnd context) {
    int32_t err = E502_CHECK_BROWSE_CONTEXT(context);
    if (err == X502_ERR_OK) {
        unsigned i;
        for (i=0; i < context->svc_cnt; i++) {
            f_svc_context_free(&context->services[i]);
        }

        if (context->browse_ref != NULL)
            DNSServiceRefDeallocate(context->browse_ref);
        if (context->main_ref != NULL)
            DNSServiceRefDeallocate(context->main_ref);

        memset(context, 0, sizeof(t_e502_service_browse_context));
        free(context);
    }
    return err;
}



static void DNSSD_API f_get_addr_info_reply(
    DNSServiceRef                    sdRef,
    DNSServiceFlags                  flags,
    uint32_t                         interfaceIndex,
    DNSServiceErrorType              errorCode,
    const char                       *hostname,
    const struct sockaddr            *address,
    uint32_t                         ttl,
    void                             *context
    ) {
    t_e502_eth_svc_record_hnd rec = (t_e502_eth_svc_record_hnd)context;
    if (errorCode != kDNSServiceErr_NoError) {
        rec->op_err = X502_ERR_DNSSD_COMMUNICATION;
        rec->op_in_progr = 0;
    } else {
        if (address && address->sa_family == AF_INET) {
            const unsigned char *b = (const unsigned char *) &((struct sockaddr_in *)address)->sin_addr;
            rec->ipv4_addr =  ((uint32_t)b[0] << 24) |
                              ((uint32_t)b[1] << 16) |
                              ((uint32_t)b[2] << 8)  |
                               b[3];
            rec->op_err = X502_ERR_OK;
            rec->op_in_progr = 0;
        }
    }
}

void DNSSD_API f_cb_gethosttarget(DNSServiceRef sdRef, DNSServiceFlags flags,
                                uint32_t interfaceIndex,
                                DNSServiceErrorType errorCode,
                                const char *fullname, const char *hosttarget,
                                uint16_t  port,        /* In network byte order */
                                uint16_t  txtLen,
                                const unsigned char *txtRecord,
                                void  *context) {

    t_e502_eth_svc_record_hnd rec = (t_e502_eth_svc_record_hnd)context;
    if (errorCode != kDNSServiceErr_NoError) {
        rec->op_err = X502_ERR_DNSSD_COMMUNICATION;
    } else {
        size_t hostlen = strlen(hosttarget)+1;
        rec->port = htons(port);
        if (rec->hosttarget != NULL)
            free(rec->hosttarget);
        rec->hosttarget = malloc(hostlen);
        if (rec->hosttarget != NULL) {
            strcpy(rec->hosttarget, hosttarget);
            rec->op_err = X502_ERR_OK;
        } else {
            rec->op_err = X502_ERR_MEMORY_ALLOC;
        }
    }
    rec->op_in_progr = 0;
}


void DNSSD_API f_cb_resolve (DNSServiceRef sdRef, DNSServiceFlags flags,
                             uint32_t interfaceIndex,
                             DNSServiceErrorType errorCode,
                             const char *fullname, const char *hosttarget,
                             uint16_t  port,        /* In network byte order */
                             uint16_t  txtLen,
                             const unsigned char *txtRecord,
                             void  *context) {
    t_e502_eth_svc_browse_hnd browse_context = (t_e502_eth_svc_browse_hnd)context;

    unsigned i;

    for (i=0; i < browse_context->svc_cnt; i++) {
        t_service_context *svc_context = &browse_context->services[i];
        if (svc_context->sdref == sdRef) {
            uint8_t len;
            const char *devname_ptr = TXTRecordGetValuePtr(txtLen, txtRecord, "devname", &len);
            const char *serial_ptr;
            if ((devname_ptr != NULL) && !strncmp(devname_ptr, E502_DEVICE_NAME, len)) {

                memcpy(svc_context->rec->devname, devname_ptr, len);
                serial_ptr = TXTRecordGetValuePtr(txtLen, txtRecord, "serial", &len);
                if (serial_ptr != NULL) {
                    if (len > X502_SERIAL_SIZE)
                        len = X502_SERIAL_SIZE;
                    memcpy(svc_context->rec->serial, serial_ptr, len);
                }
                svc_context->rec->port = port;

                if (svc_context->init_done == 0) {
                    svc_context->init_done = 1;
                    svc_context->event = E502_ETH_SVC_EVENT_ADD;
                } else {
                    svc_context->event = E502_ETH_SVC_EVENT_CHANGED;
                }
            }
        }
    }
}


static void DNSSD_API f_browse_replay (DNSServiceRef sdRef, DNSServiceFlags flags,
                                       uint32_t  interfaceIndex, DNSServiceErrorType errorCode,
                                       const char *serviceName, const char *regtype,
                                       const char *replyDomain, void *context) {
    if (errorCode == kDNSServiceErr_NoError) {
        t_e502_eth_svc_browse_hnd browse_context = (t_e502_eth_svc_browse_hnd)context;
        if (flags & kDNSServiceFlagsAdd) {
            if (browse_context->svc_cnt < SERVICE_CONTEXT_MAX_CNT) {
                t_service_context *svc_context = &browse_context->services[browse_context->svc_cnt];
                int32_t err;
                size_t domain_len = strlen(replyDomain) + 1;

                memset(svc_context, 0, sizeof(t_service_context));


                svc_context->sdref = browse_context->main_ref;
                svc_context->rec = f_service_record_create();
                if (svc_context->rec != NULL) {
                    strncpy(svc_context->rec->service_name, serviceName, X502_INSTANCE_NAME_SIZE);
                    svc_context->rec->domain = malloc(domain_len);
                    if (svc_context->rec->domain != NULL) {
                        strcpy(svc_context->rec->domain, replyDomain);



                        svc_context->rec->iface_idx = interfaceIndex;
                        err = DNSServiceResolve(&svc_context->sdref, kDNSServiceFlagsShareConnection,
                                            interfaceIndex, serviceName, regtype, replyDomain,
                                                                f_cb_resolve, context);
                        if (err == kDNSServiceErr_NoError) {
                            browse_context->svc_cnt++;
                        } else {
                            E502_EthSvcRecordFree(svc_context->rec);
                        }
                    } else {
                        E502_EthSvcRecordFree(svc_context->rec);
                    }
                }
            }
        } else {            
            unsigned i;
            for (i = 0; i < browse_context->svc_cnt; i++) {
                t_service_context *svc_context = &browse_context->services[i];
                if (!strcmp(serviceName, svc_context->rec->service_name) &&
                       (interfaceIndex == svc_context->rec->iface_idx) &&
                        !strcmp(replyDomain, svc_context->rec->domain)) {
                    if (svc_context->init_done) {
                        svc_context->event = E502_ETH_SVC_EVENT_REMOVE;
                    } else {
                        f_svc_context_free(svc_context);
                        if (i != (browse_context->svc_cnt-1)) {
                            memmove(&browse_context->services[i], &browse_context->services[i+1],
                                    (browse_context->svc_cnt-i-1)*sizeof(browse_context->services[0]));
                        }
                        browse_context->svc_cnt--;
                    }
                    break;
                }
            }
        }
    }
}



static t_e502_eth_svc_event f_get_event(t_e502_eth_svc_browse_hnd browse_context,
                                 t_e502_eth_svc_record_hnd *svc_hnd, uint32_t *flags) {
    t_e502_eth_svc_event ret = E502_ETH_SVC_EVENT_NONE;
    unsigned i;

    for (i=0; (i < browse_context->svc_cnt) && (ret == E502_ETH_SVC_EVENT_NONE); i++) {
        t_service_context *svc = &browse_context->services[i];
        if (svc->event != E502_ETH_SVC_EVENT_NONE) {
            ret = svc->event;


            if (svc->event != E502_ETH_SVC_EVENT_REMOVE) {
                /* если событие ADD/CHANGE, то сбрасываем флаг события */
                svc->event = E502_ETH_SVC_EVENT_NONE;
                /* далаем копию записи, чтобы пользователь мог ее сохранить и
                    после завершения */
                *svc_hnd = f_service_record_create_copy(svc->rec);
            } else {
                /* так при удалении запись нам больше не нужна, то можно ее
                 * передать пользователю, без создания копии */
                *svc_hnd = svc->rec;
                svc->rec = NULL;
                f_svc_context_free(svc);
                if (i != (browse_context->svc_cnt-1)) {
                    memmove(&browse_context->services[i], &browse_context->services[i+1],
                            (browse_context->svc_cnt-i-1)*sizeof(browse_context->services[0]));
                }
                browse_context->svc_cnt--;
            }
        }
    }
    return ret;
}




X502_EXPORT(int32_t) E502_EthSvcRecordGetInstanceName(t_e502_eth_svc_record_hnd rec, char *name) {
    int32_t err = name == NULL ? X502_ERR_INVALID_POINTER : E502_CHECK_SVC_REC(rec);
    if (err == X502_ERR_OK) {
        strcpy(name, rec->service_name);
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthSvcRecordGetDevSerial(t_e502_eth_svc_record_hnd rec, char *serial) {
    int32_t err = serial == NULL ? X502_ERR_INVALID_POINTER : E502_CHECK_SVC_REC(rec);
    if (err == X502_ERR_OK) {
        strcpy(serial, rec->serial);
    }
    return err;
}


static int32_t f_wait_result(DNSServiceRef ref, t_e502_eth_svc_record_hnd rec, t_ltimer *ptmr) {
    int32_t err = X502_ERR_OK;
    int sock = DNSServiceRefSockFD(ref);
    int nfds = sock + 1;
    do {
        fd_set readfds;
        struct timeval tv;
        int result;

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        f_set_timeval_left(ptmr, &tv);
        result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
        if (result > 0) {
            DNSServiceErrorType dnssd_err = DNSServiceProcessResult(ref);
            if (dnssd_err == kDNSServiceErr_NoError) {
                if (!rec->op_in_progr) {
                    err = rec->op_err;
                }
            } else {
                err = X502_ERR_DNSSD_COMMUNICATION;
            }
        } else if (ltimer_expired(ptmr) && rec->op_in_progr) {
            err = X502_ERR_SVC_RESOLVE_TIMEOUT;
            rec->op_in_progr = 0;
        }
    } while ((err == X502_ERR_OK)  && rec->op_in_progr);
    DNSServiceRefDeallocate(ref);
    return err;
}

X502_EXPORT(int32_t) E502_EthSvcRecordResolveIPv4Addr(t_e502_eth_svc_record_hnd rec,
                                                      uint32_t *addr, uint32_t tout) {
    int32_t err = addr == NULL ? X502_ERR_INVALID_POINTER : E502_CHECK_SVC_REC(rec);
    if (err == X502_ERR_OK) {
        DNSServiceRef ref;
        t_ltimer tmr;
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));


        rec->op_in_progr = 1;
        if (DNSServiceResolve(&ref, 0, rec->iface_idx, rec->service_name,
                              E502_TCP_SERVICE_NAME, rec->domain,
                              f_cb_gethosttarget, rec) == kDNSServiceErr_NoError) {
            err = f_wait_result(ref, rec, &tmr);
        } else {
            err = X502_ERR_DNSSD_COMMUNICATION;
        }

        if (err == X502_ERR_OK) {
            rec->op_in_progr = 1;
            if (DNSServiceGetAddrInfo(&ref, 0, rec->iface_idx, kDNSServiceProtocol_IPv4,
                                        rec->hosttarget, f_get_addr_info_reply, rec)
                    == kDNSServiceErr_NoError) {;
                err = f_wait_result(ref, rec, &tmr);
            } else {
                err = X502_ERR_DNSSD_COMMUNICATION;
            }
        }

        if (err == X502_ERR_OK) {
            *addr = rec->ipv4_addr;
        }
    }
    return err;
}


X502_EXPORT(int32_t) E502_EthSvcBrowseStart(t_e502_eth_svc_browse_hnd *pcontext, uint32_t flags) {
    int32_t err = X502_ERR_OK;
    t_e502_eth_svc_browse_hnd context = calloc(1, sizeof(t_e502_service_browse_context));

    if (context != NULL) {
        context->sign = E502_SVC_BROWSE_SIGN;
    } else {
        err = X502_ERR_MEMORY_ALLOC;
    }

    if (err == X502_ERR_OK) {
        DNSServiceErrorType dnssd_err = DNSServiceCreateConnection(&context->main_ref);
        if (dnssd_err == kDNSServiceErr_NoError) {
            context->browse_ref = context->main_ref;
            dnssd_err = DNSServiceBrowse(&context->browse_ref, kDNSServiceFlagsShareConnection, 0,
                               E502_TCP_SERVICE_NAME, NULL, f_browse_replay, context);

            if (dnssd_err == kDNSServiceErr_NoError) {
                context->socket = DNSServiceRefSockFD(context->main_ref);
            }
        }

        if (dnssd_err != kDNSServiceErr_NoError) {
            if (dnssd_err == kDNSServiceErr_ServiceNotRunning) {
                err = X502_ERR_DNSSD_NOT_RUNNING;
            } else {
                err = X502_ERR_DNSSD_COMMUNICATION;
            }
        }
    }

    if (err != X502_ERR_OK) {
        if (context != NULL)
            f_browse_context_free(context);
    } else {
        *pcontext = context;
    }

    return err;
}

X502_EXPORT(int32_t) E502_EthSvcBrowseGetEvent(t_e502_eth_svc_browse_hnd context,
                                                 t_e502_eth_svc_record_hnd *svc, uint32_t *pevent,
                                                 uint32_t *flags, uint32_t tout) {
    int32_t err = (svc == NULL) || (pevent == NULL) ? X502_ERR_INVALID_POINTER :
                                                     E502_CHECK_BROWSE_CONTEXT(context);

    if (err == X502_ERR_OK) {
        int nfds = context->socket + 1;
        t_ltimer tmr;
        t_e502_eth_svc_event evt = f_get_event(context, svc, flags);

        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));

        while ((evt == E502_ETH_SVC_EVENT_NONE) && !ltimer_expired(&tmr)) {
            fd_set readfds;
            struct timeval tv;
            int result;

            FD_ZERO(&readfds);
            FD_SET(context->socket, &readfds);
            f_set_timeval_left(&tmr, &tv);

            result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
            if (result > 0) {
                DNSServiceErrorType dnssd_err = DNSServiceProcessResult(context->main_ref);
                if (dnssd_err == kDNSServiceErr_NoError) {
                    evt = f_get_event(context, svc, flags);
                } else {
                    err = X502_ERR_DNSSD_COMMUNICATION;
                }
            }
        }

        *pevent = evt;
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthSvcBrowseStop(t_e502_eth_svc_browse_hnd context) {
    return f_browse_context_free(context);
}


X502_EXPORT(int32_t) E502_MakeDevRecordByEthSvc(t_x502_devrec *devrec, t_e502_eth_svc_record_hnd svc,
                                                uint32_t flags, uint32_t tout) {

    int32_t err = e502_make_tcp_rec(devrec, flags, tout);
    if (err==X502_ERR_OK) {
        t_tcp_devinfo_data *devinfo_data = (t_tcp_devinfo_data *)devrec->internal->iface_data;
        devinfo_data->svc_rec = f_service_record_create_copy(svc);

        strcpy(devrec->location, svc->service_name);
        devrec->location_type = X502_LOCATION_TYPE_INSTANCE_NAME;
        strcpy(devrec->serial, svc->serial);
    }
    return err;
}

int32_t e502_svc_fill_devinfo(t_tcp_devinfo_data *data) {
    uint32_t addr;
    int32_t err = E502_EthSvcRecordResolveIPv4Addr(data->svc_rec, &addr, data->open_tout);
    if (err == X502_ERR_OK) {
        data->ip_addr = addr;
        data->cmd_port = data->svc_rec->port;
    }
    return err;
}
