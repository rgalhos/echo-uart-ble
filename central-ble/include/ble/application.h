#ifndef BLE_APPLICATION_H
#define BLE_APPLICATION_H

#include <stdint.h>
#include <assert.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <utils.h>

// typedef:
typedef struct
{
    struct bt_conn *conn_ref;
    void (*on_connection_success)(void *central_state);
} central_state_t;

// methods:
int start_ble_central();

#endif // BLE_APPLICATION_H