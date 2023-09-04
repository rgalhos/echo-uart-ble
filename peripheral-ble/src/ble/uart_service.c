#include <ble/uart_service.h>

struct bt_uuid_32 uuid_uart_to_uppercase = BT_UUID_INIT_32(0xABCDEF01);
struct bt_uuid_32 uuid_uart_receive_data = BT_UUID_INIT_32(0xABCDEF02);
struct bt_uuid_32 uuid_uart_send_data = BT_UUID_INIT_32(0xABCDEF03);

struct bt_gatt_attr uart_gatt_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(&uuid_uart_to_uppercase),
    BT_GATT_CHARACTERISTIC(&uuid_uart_receive_data.uuid, BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE, NULL, ble_evt_on_receive_data, NULL),
    BT_GATT_CHARACTERISTIC(&uuid_uart_send_data.uuid, BT_GATT_CHRC_READ, BT_GATT_PERM_WRITE, ble_send_data, NULL, NULL),
};

struct bt_gatt_service uart_to_uppercase_service = BT_GATT_SERVICE(uart_gatt_attrs);

static peripheral_state_t *g_state;

// ------------------------------------------------------------

ssize_t ble_evt_on_receive_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    LOG_TRACE("ble_evt_on_receive_data");

    return 0;
} // ble_evt_on_receive_data

ssize_t ble_send_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    LOG_TRACE("ble_send_data");

    return 0;
} // ble_send_data

void uart_on_data_receive(const char *buffer, const size_t length)
{
    LOG_TRACE("uart_on_data_receive");
} // uart_on_data_receive

void initialize_uart_service(peripheral_state_t *state)
{
    LOG_TRACE("initialize_uart_service");

    g_state = state;
    LOG_DEBUG("Estado setado em initialize_uart_service");

    bt_gatt_service_register(&uart_to_uppercase_service);
} // initialize_uart_service
