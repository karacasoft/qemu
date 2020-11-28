/**
 * LPC4088FET208
 * 
 * Written by: Mahmut Karaca
 * 
 * This code is licensed under GPLv3
 */
#ifndef __REMOTE_CTRL_H__
#define __REMOTE_CTRL_H__

#include "hw/qdev-core.h"
#include "qemu/thread.h"
#include "qemu/queue.h"

#include <sys/socket.h>
#include <netinet/in.h>


#include <stddef.h>

#define TYPE_REMOTE_CTRL "remotectrl"
#define REMOTE_CTRL_GET_CLASS(obj) OBJECT_GET_CLASS(RemoteCtrlClass, obj, TYPE_REMOTE_CTRL)
#define REMOTE_CTRL_CLASS(klass) OBJECT_CLASS_CHECK(RemoteCtrlClass, klass, TYPE_REMOTE_CTRL)
#define REMOTE_CTRL(obj) OBJECT_CHECK(RemoteCtrlState, obj, TYPE_REMOTE_CTRL)

typedef struct RemoteCtrlState RemoteCtrlState;
typedef struct RemoteCtrlClass RemoteCtrlClass;
typedef struct RemoteCtrlMessage RemoteCtrlMessage;

typedef void (*RemoteCtrl_ReceiveMessageCallback)(RemoteCtrlState *s, RemoteCtrlMessage *data);
typedef void (*RemoteCtrl_SendMessageFunction)(RemoteCtrlState *s, void *data, size_t size);

typedef struct CallbackEntry CallbackEntry;

struct CallbackEntry
{
    RemoteCtrlState *dev;
    RemoteCtrl_ReceiveMessageCallback callback;
    QLIST_ENTRY(CallbackEntry) entries;
};

struct RemoteCtrlClass {
    DeviceClass parent;

    RemoteCtrl_SendMessageFunction send_message;

    int socket;
    int client_socket;
    struct sockaddr_in server_addr;
    struct sockaddr_storage server_storage;

    QemuThread mq_thread;
    QemuMutex mq_mutex;

    QLIST_HEAD(, CallbackEntry) callbacks;
};

struct RemoteCtrlState {
    /*<private>*/
    DeviceState parent;

    /*<public>*/
    CallbackEntry *entry;
    RemoteCtrl_ReceiveMessageCallback callback;

    DeviceState *connected_device;
};

struct RemoteCtrlMessage {
    uint32_t magic;
    uint32_t cmd;
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
    uint32_t arg5;
    uint32_t arg6;
};



void remote_ctrl_send_message(RemoteCtrlState *s, void *data, size_t size);

#endif // __REMOTE_CTRL_H__