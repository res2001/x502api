#ifdef ENABLE_BONJOUR
    #include "dns_sd.h"


    typedef uint32_t t_svc_iface_idx;
    typedef struct {
        uint16_t  txtLen;
        const unsigned char *txtStr;
    } t_svc_txt_record;
#elif defined ENABLE_AVAHI
    #include <netinet/in.h>
    #include <avahi-client/client.h>
    #include <avahi-client/lookup.h>

    #include <avahi-common/simple-watch.h>
    #include <avahi-common/malloc.h>
    #include <avahi-common/error.h>

    typedef AvahiIfIndex t_svc_iface_idx;
    typedef AvahiStringList* t_svc_txt_record;
#else
    #error Invalid DNSSD backend
#endif
#include "ltimer.h"
#include "e502api_tcp_private.h"



#include <stdlib.h>

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
    t_svc_iface_idx iface_idx;
    uint32_t ipv4_addr;
    int op_in_progr;
    int32_t op_err;
} t_svc_record;



typedef struct {
#if defined ENABLE_BONJOUR
    DNSServiceRef sdref;
    DNSServiceRef sdref_addr;
#elif defined ENABLE_AVAHI
    AvahiServiceResolver *resolver;
#endif
    t_e502_eth_svc_event event;
    int init_done;
    t_e502_eth_svc_record_hnd rec;
} t_service_context;

typedef struct st_e502_eth_svc_browse_context {
    uint32_t sign;
#if defined ENABLE_BONJOUR
    DNSServiceRef main_ref;
    DNSServiceRef browse_ref;
    int socket;
#elif defined ENABLE_AVAHI
    AvahiSimplePoll *poll;
    AvahiClient *client;
    AvahiServiceBrowser *sb;
#endif
    int32_t err;
    unsigned svc_cnt;
    t_service_context services[SERVICE_CONTEXT_MAX_CNT];
} t_e502_service_browse_context;



#if defined ENABLE_BONJOUR
    void DNSSD_API f_cb_resolve (DNSServiceRef sdRef, DNSServiceFlags flags,
                                 uint32_t interfaceIndex,
                                 DNSServiceErrorType errorCode,
                                 const char *fullname, const char *hosttarget,
                                 uint16_t  port,        /* In network byte order */
                                 uint16_t  txtLen,
                                 const unsigned char *txtRecord,
                                 void  *context);

#elif defined ENABLE_AVAHI

    static void f_resolve_callback(AvahiServiceResolver *r,  AvahiIfIndex interface,
                                    AvahiProtocol protocol, AvahiResolverEvent event,
                                    const char *name, const char *type,
                                    const char *domain,  const char *host_name,
                                    const AvahiAddress *address, uint16_t port,
                                    AvahiStringList *txt, AvahiLookupResultFlags flags,
                                    void* userdata);
#endif



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
         ret->hosttarget = NULL;
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
#if defined ENABLE_BONJOUR
    if (svc_context->sdref != NULL)
        DNSServiceRefDeallocate(svc_context->sdref);
    if (svc_context->sdref_addr)
        DNSServiceRefDeallocate(svc_context->sdref_addr);
#elif defined ENABLE_AVAHI
    if (svc_context->resolver)
        avahi_service_resolver_free(svc_context->resolver);
#endif
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
#if defined ENABLE_BONJOUR
        if (context->browse_ref != NULL)
            DNSServiceRefDeallocate(context->browse_ref);
        if (context->main_ref != NULL)
            DNSServiceRefDeallocate(context->main_ref);
#elif defined ENABLE_AVAHI
        if (context->sb != NULL)
            avahi_service_browser_free(context->sb);
        if (context->client != NULL)
            avahi_client_free(context->client);
        if (context->poll != NULL) {
            avahi_simple_poll_quit(context->poll);
            avahi_simple_poll_free(context->poll);
        }
#endif
        memset(context, 0, sizeof(t_e502_service_browse_context));
        free(context);
    }
    return err;
}


#if defined ENABLE_BONJOUR
    static const char* f_get_txt_rec(t_svc_txt_record txt_record, const char *name, uint8_t *len) {
        return TXTRecordGetValuePtr(txt_record.txtLen, txt_record.txtStr, name, len);
    }
#elif defined ENABLE_AVAHI
    static const char* f_get_txt_rec(t_svc_txt_record txt_record, const char *name, uint8_t *len) {
        const char *ret = NULL;
        int out = 0;
        int name_len = strlen(name);
        while (!out) {
            const char *label = (const char*)txt_record->text;
            int rec_len = strlen(label);
            const char *label_end = strchr(label, '=');
            int label_len = (label_end != NULL) ? label_end - label : rec_len;
            if ((name_len == label_len) && !memcmp(name, label, label_len)) {
                int val_len = label_end == NULL ? 0 : rec_len - label_len;
                if (val_len != 0) {
                    ret = label_end+1;
                } else {
                    ret = NULL;
                }
                *len = val_len;
                out = 1;
            }

            if (!out) {
                if (txt_record->next != NULL) {
                    txt_record = txt_record->next;
                } else {
                    out = 1;
                }
            }
        }
        return ret;
    }
#endif

/* Анализ текстовой информации о найденном сервисе и проверка, подходит ли он */
static int32_t f_svc_check_resolved(t_service_context *svc_context,
                                     t_svc_txt_record txt_record) {
    uint8_t len;
    int32_t err = X502_ERR_OK;
    const char *devname_ptr = f_get_txt_rec(txt_record, "devname", &len);
    const char *serial_ptr;
    if ((devname_ptr != NULL) && !strncmp(devname_ptr, E502_DEVICE_NAME, len)) {
        memcpy(svc_context->rec->devname, devname_ptr, len);
        serial_ptr = f_get_txt_rec(txt_record, "serial", &len);
        if (serial_ptr != NULL) {
            if (len >= X502_SERIAL_SIZE)
                len = X502_SERIAL_SIZE-1;
            memcpy(svc_context->rec->serial, serial_ptr, len);
            svc_context->rec->serial[len] = 0;
        }        
    } else {
        err = X502_ERR_INVALID_DEVICE;
    }
    return err;
}


static void f_add_new_svc(t_e502_eth_svc_browse_hnd browse_context, t_svc_iface_idx iface_idx,
                         const char *svc_name, const char *domain, const char *regtype) {
    if (browse_context->svc_cnt < SERVICE_CONTEXT_MAX_CNT) {
        t_service_context *svc_context = &browse_context->services[browse_context->svc_cnt];
        size_t domain_len = strlen(domain) + 1;

        memset(svc_context, 0, sizeof(t_service_context));
        svc_context->rec = f_service_record_create();
        if (svc_context->rec != NULL) {
            strncpy(svc_context->rec->service_name, svc_name, X502_INSTANCE_NAME_SIZE);
            svc_context->rec->domain = malloc(domain_len);
            if (svc_context->rec->domain != NULL) {
                strcpy(svc_context->rec->domain, domain);

                svc_context->rec->iface_idx = iface_idx;
#ifdef ENABLE_BONJOUR
                svc_context->sdref = browse_context->main_ref;
                if (DNSServiceResolve(&svc_context->sdref, kDNSServiceFlagsShareConnection,
                                      iface_idx, svc_name, regtype, domain,
                                      f_cb_resolve, browse_context) == kDNSServiceErr_NoError) {
                    browse_context->svc_cnt++;
                } else {
                    svc_context->sdref = NULL;
                    E502_EthSvcRecordFree(svc_context->rec);
                }
#elif defined ENABLE_AVAHI
                svc_context->resolver = avahi_service_resolver_new(browse_context->client,
                                                                   iface_idx, AVAHI_PROTO_INET,
                                                                   svc_name, regtype, domain,
                                                                   AVAHI_PROTO_UNSPEC, 0, f_resolve_callback, browse_context);
                if (svc_context->resolver != NULL) {
                    browse_context->svc_cnt++;
                } else {
                    E502_EthSvcRecordFree(svc_context->rec);
                }
#endif
            } else {
                E502_EthSvcRecordFree(svc_context->rec);
            }
        }
    }
}

static void f_remove_service_idx(t_e502_eth_svc_browse_hnd browse_context, unsigned i) {
    t_service_context *svc_context = &browse_context->services[i];
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
}

static void f_remove_service(t_e502_eth_svc_browse_hnd browse_context, const char *svc_name,
                             const char *domain) {
    unsigned i;
    for (i = 0; i < browse_context->svc_cnt; i++) {
        t_service_context *svc_context = &browse_context->services[i];
        if (!strcmp(svc_name, svc_context->rec->service_name) &&
                !strcmp(domain, svc_context->rec->domain)) {
            f_remove_service_idx(browse_context, i);
            break;
        }
    }
}

#ifdef ENABLE_BONJOUR
static uint32_t f_get_addr(const struct sockaddr *address) {
    uint32_t ipv4_addr = 0;
    if (address && address->sa_family == AF_INET) {
        const unsigned char *b = (const unsigned char *) &((struct sockaddr_in *)address)->sin_addr;
        ipv4_addr =  ((uint32_t)b[0] << 24) |
                          ((uint32_t)b[1] << 16) |
                          ((uint32_t)b[2] << 8)  |
                           b[3];
    }
    return ipv4_addr;
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
            rec->ipv4_addr = f_get_addr(address);
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
        rec->port = ntohs(port);
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


static void DNSSD_API f_cb_browse_addr(
    DNSServiceRef                    sdRef,
    DNSServiceFlags                  flags,
    uint32_t                         interfaceIndex,
    DNSServiceErrorType              errorCode,
    const char                       *hostname,
    const struct sockaddr            *address,
    uint32_t                         ttl,
    void                             *context
    ) {
    t_e502_eth_svc_browse_hnd browse_context = (t_e502_eth_svc_browse_hnd)context;

    unsigned i;

    for (i=0; i < browse_context->svc_cnt; i++) {
        t_service_context *svc_context = &browse_context->services[i];
        if (svc_context->sdref_addr == sdRef) {
            if (errorCode == kDNSServiceErr_NoError) {
                if (address && address->sa_family == AF_INET) {
                    uint32_t new_addr = f_get_addr(address);
                    if (svc_context->init_done == 0) {
                        svc_context->rec->ipv4_addr = new_addr;
                        svc_context->init_done = 1;
                        svc_context->event = E502_ETH_SVC_EVENT_ADD;
                    } else if (svc_context->event == E502_ETH_SVC_EVENT_NONE) {
                        if (new_addr != svc_context->rec->ipv4_addr) {
                            svc_context->rec->ipv4_addr = new_addr;
                            svc_context->event = E502_ETH_SVC_EVENT_CHANGED;
                        }
                    }
                }
            }
        }
    }
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
            t_svc_txt_record txt_rec;
            txt_rec.txtLen = txtLen;
            txt_rec.txtStr = txtRecord;            
            if (f_svc_check_resolved(svc_context, txt_rec) != X502_ERR_OK) {
                f_remove_service_idx(browse_context, i);
            } else {
                if (svc_context->init_done) {
                    if (svc_context->event == E502_ETH_SVC_EVENT_NONE)
                        svc_context->event = E502_ETH_SVC_EVENT_CHANGED;
                } else {
                    svc_context->sdref_addr = browse_context->main_ref;
                    if (DNSServiceGetAddrInfo(&svc_context->sdref_addr, kDNSServiceFlagsShareConnection,
                                              interfaceIndex,
                                              kDNSServiceProtocol_IPv4,
                                              hosttarget, f_cb_browse_addr,
                                              browse_context)
                            != kDNSServiceErr_NoError) {
                        svc_context->sdref_addr = NULL;
                        f_remove_service_idx(browse_context, i);
                    }
                }
            }
        }
    }
}


static void DNSSD_API f_browse_replay (DNSServiceRef sdRef, DNSServiceFlags flags,
                                       uint32_t  interfaceIndex, DNSServiceErrorType errorCode,
                                       const char *serviceName, const char *regtype,
                                       const char *replyDomain, void *context) {
    t_e502_eth_svc_browse_hnd browse_context = (t_e502_eth_svc_browse_hnd)context;
    if (errorCode != kDNSServiceErr_NoError) {
        browse_context->err = X502_ERR_DNSSD_COMMUNICATION;
    } else {
        if (flags & kDNSServiceFlagsAdd) {
            f_add_new_svc(browse_context, interfaceIndex, serviceName, replyDomain, regtype);
        } else {            
            f_remove_service(browse_context, serviceName, replyDomain);
        }
    }
}
#elif defined ENABLE_AVAHI
    static void f_resolve_addr_callback(AvahiServiceResolver *r,  AvahiIfIndex interface,
                                AvahiProtocol protocol, AvahiResolverEvent event,
                                const char *name, const char *type,
                                const char *domain,  const char *host_name,
                                const AvahiAddress *address, uint16_t port,
                                AvahiStringList *txt, AvahiLookupResultFlags flags,
                                void* userdata) {
        t_e502_eth_svc_record_hnd rec = (t_e502_eth_svc_record_hnd) userdata;
        switch (event) {
            case AVAHI_RESOLVER_FAILURE:
                rec->op_err = X502_ERR_DNSSD_COMMUNICATION;
                rec->op_in_progr = 0;
                break;
            case AVAHI_RESOLVER_FOUND: {
                    size_t hostlen = strlen(host_name);
                    rec->port = port;
                    if (rec->hosttarget != NULL)
                        free(rec->hosttarget);
                    rec->hosttarget = malloc(hostlen);
                    if (rec->hosttarget != NULL) {
                        strcpy(rec->hosttarget, host_name);
                        rec->op_err = X502_ERR_OK;
                        rec->ipv4_addr =  ntohl(address->data.ipv4.address);
                    } else {
                        rec->op_err = X502_ERR_MEMORY_ALLOC;
                    }
                    rec->op_in_progr = 0;
                }
                break;
        }
    }

    static void f_resolve_callback(AvahiServiceResolver *r,  AvahiIfIndex interface,
                                    AvahiProtocol protocol, AvahiResolverEvent event,
                                    const char *name, const char *type,
                                    const char *domain,  const char *host_name,
                                    const AvahiAddress *address, uint16_t port,
                                    AvahiStringList *txt, AvahiLookupResultFlags flags,
                                    void* userdata) {
        t_e502_eth_svc_browse_hnd browse_context = (t_e502_eth_svc_browse_hnd)userdata;
        unsigned i;
        for (i=0; i < browse_context->svc_cnt; i++) {
            t_service_context *svc_context = &browse_context->services[i];
            /* Called whenever a service has been resolved successfully or timed out */
            switch (event) {
                case AVAHI_RESOLVER_FAILURE:
                    f_remove_service_idx(browse_context, i);
                    break;
                case AVAHI_RESOLVER_FOUND:
                    if (svc_context->resolver == r) {
                        int32_t err = f_svc_check_resolved(svc_context, txt);
                        if (err != X502_ERR_OK) {
                            f_remove_service_idx(browse_context, i);
                        } else {
                            if (svc_context->init_done == 0) {
                                svc_context->init_done = 1;
                                svc_context->event = E502_ETH_SVC_EVENT_ADD;
                            } else if (svc_context->event == E502_ETH_SVC_EVENT_NONE) {
                                svc_context->event = E502_ETH_SVC_EVENT_CHANGED;
                            }

                        }
                    }
                    break;
            }
        }
    }

    static void f_browse_callback(AvahiServiceBrowser *b,  AvahiIfIndex interface,
                                  AvahiProtocol protocol, AvahiBrowserEvent event,
                                  const char *name, const char *type,
                                  const char *domain, AvahiLookupResultFlags flags,
                                  void* userdata) {
        t_e502_eth_svc_browse_hnd context = (t_e502_eth_svc_browse_hnd)userdata;
        switch (event) {
            case AVAHI_BROWSER_FAILURE:
                context->err = X502_ERR_DNSSD_COMMUNICATION;
                avahi_simple_poll_quit(context->poll);
                return;

            case AVAHI_BROWSER_NEW:
                f_add_new_svc(context, interface, name, domain, type);
                break;

            case AVAHI_BROWSER_REMOVE:
                f_remove_service(context, name, domain);
                break;

            case AVAHI_BROWSER_ALL_FOR_NOW:
            case AVAHI_BROWSER_CACHE_EXHAUSTED:

                break;
        }
    }

    static void f_client_callback(AvahiClient *c, AvahiClientState state, void * userdata) {

        /* Called whenever the client or server state changes */

        if (state == AVAHI_CLIENT_FAILURE) {
            t_e502_eth_svc_browse_hnd context = (t_e502_eth_svc_browse_hnd)userdata;
            context->err = X502_ERR_DNSSD_NOT_RUNNING;
            avahi_simple_poll_quit(context->poll);
        }
    }



#endif


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


#ifdef ENABLE_BONJOUR
static int32_t f_wait_result(DNSServiceRef ref, t_e502_eth_svc_record_hnd rec, t_ltimer *ptmr) {
    int32_t err = X502_ERR_OK;
    int sock = DNSServiceRefSockFD(ref);
    int nfds = sock + 1;
    int out = 0;
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
                    out = 1;
                }
            } else {
                err = X502_ERR_DNSSD_COMMUNICATION;
            }
        } else if (ltimer_expired(ptmr) && rec->op_in_progr) {
            err = X502_ERR_SVC_RESOLVE_TIMEOUT;
            rec->op_in_progr = 0;
        }
    } while ((err == X502_ERR_OK)  && !out);
    DNSServiceRefDeallocate(ref);
    return err;
}
#endif

X502_EXPORT(int32_t) E502_EthSvcRecordResolveIPv4Addr(t_e502_eth_svc_record_hnd rec,
                                                      uint32_t *addr, uint32_t tout) {
    int32_t err = addr == NULL ? X502_ERR_INVALID_POINTER : E502_CHECK_SVC_REC(rec);
    if (err == X502_ERR_OK) {
#if defined ENABLE_BONJOUR
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
#elif defined ENABLE_AVAHI
        AvahiSimplePoll *poll = NULL;
        AvahiClient *client = NULL;
        AvahiServiceResolver *resolver = NULL;
        poll = avahi_simple_poll_new();
        if (poll == NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        } else {
            client = avahi_client_new(avahi_simple_poll_get(poll), 0,
                                      NULL, NULL, NULL);
            if (client == NULL) {
                err = X502_ERR_DNSSD_COMMUNICATION;
            }
        }

        if (err == X502_ERR_OK) {
            t_ltimer tmr;
            int out = 0;
            ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));
            resolver = avahi_service_resolver_new(client, rec->iface_idx, AVAHI_PROTO_INET,
                                                  rec->service_name, E502_TCP_SERVICE_NAME,
                                                  rec->domain, AVAHI_PROTO_UNSPEC, 0,
                                                  f_resolve_addr_callback, rec);
            do {
                rec->op_in_progr = 1;
                avahi_simple_poll_iterate(poll, LTIMER_CLOCK_TICKS_TO_MS(ltimer_expiration(&tmr)));
                if (rec->op_in_progr == 0) {
                    err = rec->op_err;
                    out = 1;
                } else if (ltimer_expired(&tmr)) {
                    err = X502_ERR_SVC_RESOLVE_TIMEOUT;
                }
            }  while ((err == X502_ERR_OK) && !out);
        }

        if (resolver != NULL)
            avahi_service_resolver_free(resolver);
        if (client != NULL)
            avahi_client_free(client);
        if (poll != NULL) {
            avahi_simple_poll_quit(poll);
            avahi_simple_poll_free(poll);
        }
#endif
        if (err == X502_ERR_OK) {
            *addr = rec->ipv4_addr;
        }
    }
    return err;
}

X502_EXPORT(int32_t) E502_EthSvcRecordIsSameInstance(t_e502_eth_svc_record_hnd svc1,
                                                     t_e502_eth_svc_record_hnd svc2) {
    int32_t err = E502_CHECK_SVC_REC(svc1);
    if (err == X502_ERR_OK)
        err = E502_CHECK_SVC_REC(svc2);
    if (err == X502_ERR_OK) {
        if (strcmp(svc1->service_name, svc2->service_name) || strcmp(svc1->domain, svc2->domain)) {
            err = X502_ERR_INSTANCE_MISMATCH;
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

#ifdef ENABLE_BONJOUR
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
#elif defined ENABLE_AVAHI
    if (err == X502_ERR_OK) {
        context->poll = avahi_simple_poll_new();
        if (context->poll == NULL) {
            err = X502_ERR_MEMORY_ALLOC;
        } else {
            context->client = avahi_client_new(avahi_simple_poll_get(context->poll), 0,
                                               f_client_callback, context, NULL);
            if (context->client == NULL) {
                err = X502_ERR_DNSSD_COMMUNICATION;
            } else if (context->err != X502_ERR_OK) {
                err = context->err;
            }

            if (err == X502_ERR_OK) {
                context->sb = avahi_service_browser_new(context->client, AVAHI_IF_UNSPEC,
                                                        AVAHI_PROTO_INET,
                                                        E502_TCP_SERVICE_NAME, NULL, 0,
                                                        f_browse_callback, context);
                if (context->sb == NULL)
                    err = X502_ERR_DNSSD_COMMUNICATION;
            }
        }
    }
#endif

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
        t_ltimer tmr;
        t_e502_eth_svc_event evt = f_get_event(context, svc, flags);
        ltimer_set(&tmr, LTIMER_MS_TO_CLOCK_TICKS(tout));
        context->err = X502_ERR_OK;

        while ((evt == E502_ETH_SVC_EVENT_NONE) && !ltimer_expired(&tmr) && (err == X502_ERR_OK)) {
#ifdef ENABLE_BONJOUR
            fd_set readfds;
            struct timeval tv;
            int result;

            FD_ZERO(&readfds);
            FD_SET(context->socket, &readfds);
            f_set_timeval_left(&tmr, &tv);

            result = select(context->socket + 1, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
            if (result > 0) {
                DNSServiceErrorType dnssd_err = DNSServiceProcessResult(context->main_ref);
                if (dnssd_err == kDNSServiceErr_NoError) {
                    evt = f_get_event(context, svc, flags);
                } else {
                    err = X502_ERR_DNSSD_COMMUNICATION;
                }
            }
#elif defined ENABLE_AVAHI
             avahi_simple_poll_iterate(context->poll, LTIMER_CLOCK_TICKS_TO_MS(ltimer_expiration(&tmr)));
             if (context->err != X502_ERR_OK) {
                 err = context->err;
             } else {
                evt = f_get_event(context, svc, flags);
             }
#endif
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
