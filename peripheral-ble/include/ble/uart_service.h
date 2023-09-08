#ifndef BLE_UART_SERVICE_H
#define BLE_UART_SERVICE_H

#include <stdint.h>
#include <string.h>
#include <bluetooth/gatt.h>

#include <ble/application.h>
#include <utils.h>

// defines:
#define MAX_DATA_LEN 256

// methods:
// bt_gatt_attr_write_func_t
ssize_t ble_evt_on_receive_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
// bt_gatt_attr_read_func_t
ssize_t ble_send_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);

void uart_on_data_receive(const char *buffer, const size_t length);
void initialize_uart_service(peripheral_state_t *state);

#endif // BLE_UART_SERVICE_H
