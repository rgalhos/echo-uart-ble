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
typedef enum
{
    e_ble_connected,
    e_ble_disconnected,
} ble_status;

typedef struct
{
    ble_status status;
    struct bt_conn *conn_ref;
} peripheral_state_t;

// methods:
int start_ble_peripheral(peripheral_state_t *state);

#endif // BLE_APPLICATION_H
