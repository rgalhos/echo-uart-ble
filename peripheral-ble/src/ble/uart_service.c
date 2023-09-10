#include <ble/uart_service.h>

// methods:
static int on_receive_data_callback(const uint8_t *data, const int len);
static void ccc_evt_on_change(const struct bt_gatt_attr *attr, uint16_t value);

// structs:
static struct bt_uuid_16 uuid_uart_to_uppercase = BT_UUID_INIT_16(0xAB01);
static struct bt_uuid_16 uuid_uart_receive_data = BT_UUID_INIT_16(0xAB02);
static struct bt_uuid_16 uuid_uart_send_data = BT_UUID_INIT_16(0xAB03);

static struct bt_gatt_attr uart_gatt_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(&uuid_uart_to_uppercase),
    BT_GATT_CHARACTERISTIC(&uuid_uart_receive_data.uuid,    // uuid
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP, // properties
                           BT_GATT_PERM_WRITE,              // permissions
                           NULL,                            // function: read
                           ble_evt_on_receive_data,         // function: write
                           NULL                             // user data
                           ),
    BT_GATT_CHARACTERISTIC(&uuid_uart_send_data.uuid, // uuid
                           BT_GATT_CHRC_READ,         // properties
                           BT_GATT_PERM_WRITE,        // permissions
                           ble_send_data,             // function: read
                           NULL,                      // functionm: write
                           NULL                       // user data
                           ),
    BT_GATT_CCC(ccc_evt_on_change, BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service uart_to_uppercase_service = BT_GATT_SERVICE(uart_gatt_attrs);

// globals:
static peripheral_state_t *g_state;

// ------------------------------------------------------------

static int on_receive_data_callback(const uint8_t *data, const int len)
{
    LOG_TRACE("on_receive_data_callback");

    assert(NULL != g_state->conn_ref);

    if (!data || !len)
        return -1;

    uint8_t buffer[MAX_BLE_MSG_SIZE];
    memcpy(buffer, data, len);

    for (int i = 0; i < len; i++)
    {
        if (data[i] >= 'a' && data[i] <= 'z')
        {
            buffer[i] ^= 'a' - 'A';
        }
    }

    int err = bt_gatt_notify(g_state->conn_ref, &uart_gatt_attrs[2], buffer, len);
    if (0 != err)
    {
        LOG_INFO("Erro ao notificar central: 0x%x", err);
        return err;
    }

    LOG_INFO("Notificação enviada à central");

    return 0;
} // on_receive_data_callback

static void ccc_evt_on_change(const struct bt_gatt_attr *attr, uint16_t value)
{
    LOG_TRACE("ccc_evt_on_change");
    LOG_DEBUG("attr->uuid: [ %x ] value: [ %u ]", attr->uuid, value);
} // ccc_evt_on_change

ssize_t ble_evt_on_receive_data(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                const void *buf,
                                uint16_t len,
                                uint16_t offset,
                                uint8_t flags)
{
    LOG_TRACE("ble_evt_on_receive_data");

    int datalen = len >= MAX_BLE_MSG_SIZE ? (MAX_BLE_MSG_SIZE - 1) : len;
    uint8_t data[MAX_BLE_MSG_SIZE];

    memcpy(data, buf, datalen);
    data[datalen] = '\0';

    LOG_INFO("Dados recebidos: [ %s ] (%u/%u bytes)", data, datalen, len);

    if (flags & BT_GATT_WRITE_FLAG_PREPARE)
    {
        LOG_DEBUG("BT_GATT_WRITE_FLAG_PREPARE");
        return BT_GATT_ERR(BT_ATT_ERR_SUCCESS);
    }

    on_receive_data_callback(data, datalen);

    return datalen;
} // ble_evt_on_receive_data

ssize_t ble_send_data(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                      void *buf, uint16_t len, uint16_t offset)
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
