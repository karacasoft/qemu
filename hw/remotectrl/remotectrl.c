/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#include "qemu/osdep.h"
#include "hw/remotectrl/remotectrl.h"

#include "qom/object.h"
#include "hw/sysbus.h"
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"
#include "qemu/log.h"
#include "qemu/main-loop.h"

#include <zlib.h>

#define REMOTE_CTRL_MESSAGE_MAGIC 0xDEADBEEB
// Big endian order is default for network byte ordering.
#define REMOTE_CTRL_MESSAGE_MAGIC_SEQUENCE ((uint8_t []){ 0xDE, 0xAD, 0xBE, 0xEB })

#define REMOTE_CTRL_TCP_PORT 6942

#define DEBUG_REMOTE_CTRL 1

#define DPRINTF(fmt, args...) \
    if(DEBUG_REMOTE_CTRL) { \
        fprintf(stderr, "[%s]%s: " fmt, TYPE_REMOTE_CTRL, \
                __func__, ##args); \
    }

static void remote_ctrl_default_callback(RemoteCtrlState *s, RemoteCtrlMessage *data)
{
    DPRINTF("Data geldi lul\n");
    DPRINTF("magic = 0x%" PRIx32 "\n", data->magic);
    DPRINTF("cmd = 0x%" PRIx32 "\n", data->cmd);
    DPRINTF("arg1 = 0x%" PRIx32 "\n", data->arg1);
    DPRINTF("arg2 = 0x%" PRIx32 "\n", data->arg2);
    DPRINTF("arg3 = 0x%" PRIx32 "\n", data->arg3);
    DPRINTF("arg4 = 0x%" PRIx32 "\n", data->arg4);
    DPRINTF("arg5 = 0x%" PRIx32 "\n", data->arg5);
    DPRINTF("arg6 = 0x%" PRIx32 "\n", data->arg6);
    qemu_log_mask(LOG_STRACE, "%s: The callback function of remote_ctrl"
            " is meant to be overridden by the parent device. Please contact"
            "the developer if you see this message.\n", __func__);
}

static void remote_ctrl_message_hton(RemoteCtrlMessage *msg) {
    msg->magic = htonl(msg->magic);
    msg->cmd = htonl(msg->cmd);
    msg->arg1 = htonl(msg->arg1);
    msg->arg2 = htonl(msg->arg2);
    msg->arg3 = htonl(msg->arg3);
    msg->arg4 = htonl(msg->arg4);
    msg->arg5 = htonl(msg->arg5);
    msg->arg6 = htonl(msg->arg6);
}

static void remote_ctrl_default_send_message(RemoteCtrlState *s, void *data, size_t size)
{
    RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(s);
    uint8_t *data_cp = malloc((size) * sizeof(uint8_t));
    memcpy(data_cp, data, size);
    RemoteCtrlMessage *msg = (RemoteCtrlMessage *) data_cp;

    remote_ctrl_message_hton(msg);
    uint32_t crc = htonl((uint32_t) crc32(0, data_cp, 32));

    if(rcc->client_socket != -1)
    {
        qemu_mutex_lock(&rcc->mq_mutex);
        send(rcc->client_socket, data_cp, 32, 0);
        send(rcc->client_socket, (void *)&crc, 4, 0);
        qemu_mutex_unlock(&rcc->mq_mutex);
    }
    free(data_cp);
}

static bool remote_ctrl_check_magic_nr_sequence(RemoteCtrlClass *rcc) {
    char buffer[1];
    uint8_t i;
    for(i = 0; i < 4;) {
        ssize_t ret = recv(rcc->client_socket, buffer, 1, MSG_WAITALL);
        if(ret != 1) {
            // TODO maybe check if the socket is closed
        /*    DPRINTF("closed lol");
            close(rcc->client_socket);
            rcc->client_socket = -1;
            return false;*/
        } else {
            if((uint8_t) (buffer[0]) == REMOTE_CTRL_MESSAGE_MAGIC_SEQUENCE[i]) {
                i++;
            } else {
                i = 0;
            }
        }
    }
    return true;
}

static void remote_ctrl_message_ntoh(RemoteCtrlMessage *msg) {
    msg->magic = ntohl(msg->magic);
    msg->cmd = ntohl(msg->cmd);
    msg->arg1 = ntohl(msg->arg1);
    msg->arg2 = ntohl(msg->arg2);
    msg->arg3 = ntohl(msg->arg3);
    msg->arg4 = ntohl(msg->arg4);
    msg->arg5 = ntohl(msg->arg5);
    msg->arg6 = ntohl(msg->arg6);
}

static void *remote_ctrl_receive_message_thread(void *arg)
{
    RemoteCtrlClass *rcc = (RemoteCtrlClass *) arg;
    char buffer[36];
    RemoteCtrlMessage *msg = (RemoteCtrlMessage *) buffer;

    while(rcc->socket != -1)
    {
        while(rcc->client_socket == -1)
        {
            socklen_t addr_size = sizeof rcc->server_storage;
            rcc->client_socket = accept(rcc->socket, (struct sockaddr *)&rcc->server_storage, &addr_size);
        }
        if(remote_ctrl_check_magic_nr_sequence(rcc)) {
            msg->magic = htonl(REMOTE_CTRL_MESSAGE_MAGIC);
            ssize_t ret = recv(rcc->client_socket, buffer + 4, 32, MSG_WAITALL);
            if(ret == 32) {
                unsigned long crc = crc32(0, (uint8_t *) buffer, 32);
                if((uint32_t)crc == ntohl(*((uint32_t *) (buffer+32)))) {
                    remote_ctrl_message_ntoh(msg);
                    CallbackEntry *cb;
                    QLIST_FOREACH(cb, &rcc->callbacks, entries)
                    {
                        cb->callback(cb->dev, msg);
                    }
                }
            }
        } else {
            continue;
        }
    }
    return NULL;
}

static Property remote_ctrl_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void remote_ctrl_realize(DeviceState *ds, Error **errp)
{
    RemoteCtrlState *rcs = REMOTE_CTRL(ds);
    RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(rcs);

    CallbackEntry *entry = g_new(CallbackEntry, 1);
    entry->callback = rcs->callback;
    entry->dev = rcs;
    rcs->entry = entry;

    QLIST_INSERT_HEAD(&rcc->callbacks, entry, entries);
}

static void remote_ctrl_unrealize(DeviceState *ds)
{
    RemoteCtrlState *rcs = REMOTE_CTRL(ds);

    CallbackEntry *entry = rcs->entry;
    QLIST_REMOVE(entry, entries);
    rcs->entry = NULL;
    g_free(entry);
}

static void remote_ctrl_class_init(ObjectClass *klass, void *class_data)
{
    RemoteCtrlClass *rcc = REMOTE_CTRL_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    rcc->send_message = remote_ctrl_default_send_message;

    QLIST_INIT(&rcc->callbacks);

    dc->realize = remote_ctrl_realize;
    dc->unrealize = remote_ctrl_unrealize;
    device_class_set_props(dc, remote_ctrl_properties);
    
    

    while(1) {
        rcc->socket = socket(AF_INET, SOCK_STREAM, 0);
        rcc->client_socket = -1;

        if(rcc->socket == -1) {
            DPRINTF("Socket cannot be opened\n");
            DPRINTF("%s\n", strerror(errno));
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl socket cannot be opened"
                        "\n", __func__);
            continue;
        }
        break;
    }
    while(1) {
        rcc->server_addr.sin_family = AF_INET;
        rcc->server_addr.sin_port = htons(REMOTE_CTRL_TCP_PORT);
        rcc->server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(rcc->server_addr.sin_zero, '\0', sizeof (rcc->server_addr.sin_zero));

        int ret = bind(rcc->socket, (struct sockaddr *)&rcc->server_addr, sizeof(rcc->server_addr));
        
        if(ret == -1) {
            DPRINTF("TCP port cannot be opened\n");
            DPRINTF("%s\n", strerror(errno));
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl socket cannot be opened"
                        "\n", __func__);
            continue;
        }
        break;
    }
    while(1)
    {
        int ret = listen(rcc->socket, 1);
        if(ret != 0) {
            DPRINTF("Cannot listen port\n");
            DPRINTF("%s\n", strerror(errno));
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl socket cannot be opened"
                        "\n", __func__);
            continue;
        }
        break;
    }

    qemu_mutex_init(&rcc->mq_mutex);
    qemu_thread_create(&rcc->mq_thread, "rcc-mq", remote_ctrl_receive_message_thread,
            (void *)rcc, QEMU_THREAD_JOINABLE);
}


static void remote_ctrl_instance_init(Object *obj)
{
    RemoteCtrlState *rcs = REMOTE_CTRL(obj);

    rcs->callback = remote_ctrl_default_callback;
}


void remote_ctrl_send_message(RemoteCtrlState *s, void *data, size_t size)
{
    RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(s);

    rcc->send_message(s, data, size);
}


static const TypeInfo remote_ctrl_info = {
    .name = TYPE_REMOTE_CTRL,
    .parent = TYPE_DEVICE,
    .instance_init = remote_ctrl_instance_init,
    .instance_size = sizeof(RemoteCtrlState),
    .class_init = remote_ctrl_class_init,
    .class_size = sizeof(RemoteCtrlClass)

};

static void remote_ctrl_register_types(void)
{
    type_register_static(&remote_ctrl_info);
}

type_init(remote_ctrl_register_types)
