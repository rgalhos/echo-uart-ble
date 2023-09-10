#ifndef GATT_UART_SERVICE_H
#define GATT_UART_SERVICE_H

#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/att.h>
#include <console/console.h>
#include <kernel/thread.h>

#include <ble/application.h>
#include <utils.h>

// defines:
#define MAX_BLE_MSG_SIZE 21

// methods:
void on_connection_success(void *state);

#endif // GATT_UART_SERVICE_H
