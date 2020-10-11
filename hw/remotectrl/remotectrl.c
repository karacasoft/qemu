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

#define REMOTE_CTRL_MESSAGE_MAGIC 0xDEADBEEB

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

static void remote_ctrl_default_send_message(RemoteCtrlState *s, void *data, size_t size)
{
    RemoteCtrlClass *rcc = REMOTE_CTRL_GET_CLASS(s);
    if(rcc->mqd_out != -1)
    {
        qemu_mutex_lock(&rcc->mq_mutex);
        mq_send(rcc->mqd_out, data, size, 1);
        qemu_mutex_unlock(&rcc->mq_mutex);
    }
}

static void *remote_ctrl_receive_message_thread(void *arg)
{
    RemoteCtrlClass *rcc = (RemoteCtrlClass *) arg;
    char buffer[32];
    RemoteCtrlMessage *msg = (RemoteCtrlMessage *) buffer;

    while(rcc->mqd != -1)
    {
        int res = mq_receive(rcc->mqd, buffer, 32, NULL);
        if(res < 0)
        {
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl message receive error"
                    "\n", __func__);
        }
        if(res != 32)
        {
            continue;
        }
        if(msg->magic != REMOTE_CTRL_MESSAGE_MAGIC)
        {
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl message integrity check failed"
                    "\n", __func__);
        }
        CallbackEntry *cb;
        QLIST_FOREACH(cb, &rcc->callbacks, entries)
        {
            cb->callback(cb->dev, msg);
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
    

    // TODO initialize POSIX message queue
    struct mq_attr remote_ctrl_mq_attr = {
        .mq_curmsgs = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 32,
        .mq_flags = 0,
    };
    mq_unlink(REMOTE_CTRL_MQ_NAME);
    mqd_t mqd = mq_open(REMOTE_CTRL_MQ_NAME,
            O_RDONLY | O_CREAT | __O_CLOEXEC,
            S_IRUSR | S_IWUSR, &remote_ctrl_mq_attr);
    mqd_t mqd_out = mq_open(REMOTE_CTRL_MQ_NAME_OUT,
            O_WRONLY | O_CREAT | __O_CLOEXEC,
            S_IRUSR | S_IWUSR, &remote_ctrl_mq_attr);
    if(mqd == -1 || mqd_out == -1) {
        DPRINTF("POSIX message queue cannot be opened\n");
        DPRINTF("%s\n", strerror(errno));
        rcc->mqd = -1;
        rcc->mqd_out = -1;
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Remote ctrl message queue cannot be opened"
                    "\n", __func__);
        return;
    }
    qemu_mutex_init(&rcc->mq_mutex);
    rcc->mqd = mqd;
    rcc->mqd_out = mqd_out;
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
