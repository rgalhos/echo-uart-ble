#include <ble/application.h>

static int ble_evt_on_ready();
static void ble_evt_on_device_found(const bt_addr_le_t *addr, int8_t rssi,
                                    uint8_t adv_type, struct net_buf_simple *buf);
static void ble_evt_on_connect(struct bt_conn *conn, uint8_t err);
static void ble_evt_on_disconnect(struct bt_conn *conn, uint8_t err);
static int ble_start_scan();

static central_state_t *g_state = NULL;

// ------------------------------------------------------------

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = ble_evt_on_connect,
    .disconnected = ble_evt_on_disconnect,
};

static void ble_evt_on_device_found(const bt_addr_le_t *addr, int8_t rssi,
                                    uint8_t adv_type, struct net_buf_simple *buf)
{
    LOG_TRACE("ble_evt_on_device_found");

    assert(NULL != g_state);

    // Já está conectado a algum periférico
    if (NULL != g_state->conn_ref)
    {
        LOG_DEBUG("Já conectado: %p", g_state->conn_ref);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, BT_ADDR_LE_STR_LEN);

    LOG_INFO("Dispositivo encontrado: %s\n", addr_str);

    // escaneável e conectável
    if (adv_type != BT_GAP_ADV_TYPE_ADV_IND)
    {
        LOG_DEBUG("Não escaneável nem conectável: %s (%d)", addr_str, adv_type);
        return;
    }

    // Para de escanear ou erro
    if (0 != bt_le_scan_stop())
    {
        LOG_ERROR("Erro ao parar de escanear");
        return;
    }
    else
    {
        LOG_DEBUG("Parando de escanear");
    }

    struct bt_conn *new_conn;
    int err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &new_conn);
    if (0 != err)
    {
        LOG_ERROR("Falha ao se conectar com %s. Erro: %d", addr_str, err);

        ble_start_scan();
    }

    g_state->conn_ref = new_conn;

    return;
} // ble_evt_on_device_found

static int ble_evt_on_ready(central_state_t *state)
{
    LOG_TRACE("ble_evt_on_ready");
    LOG_INFO("Bluetooth inicializado");

    assert(NULL != state);

    g_state = state;
    LOG_DEBUG("Estado setado");

    return ble_start_scan();
} // ble_evt_on_ready

static void ble_evt_on_connect(struct bt_conn *conn, uint8_t err)
{
    LOG_TRACE("ble_evt_on_connect");

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, BT_ADDR_LE_STR_LEN);

    if (0 != err)
    {
        LOG_ERROR("Erro ao se conectar ao periférico %s. Código: %d", addr_str, err);

        if (g_state->conn_ref)
            bt_conn_unref(g_state->conn_ref);
        g_state->conn_ref = NULL;

        ble_start_scan();

        return;
    }

    if (conn != g_state->conn_ref)
    {
        return;
    }

    // g_state->conn_ref = bt_conn_ref(conn);

    LOG_INFO("Conectado ao periférico %s", addr_str);

    if (NULL != g_state->on_connection_success)
    {
        g_state->on_connection_success(g_state);
    }
} // ble_evt_on_connect

static void ble_evt_on_disconnect(struct bt_conn *conn, uint8_t reason)
{
    LOG_TRACE("ble_evt_on_disconnect");

    if (conn != g_state->conn_ref)
    {
        LOG_DEBUG("conn != g_state->conn_ref");
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, BT_ADDR_LE_STR_LEN);

    LOG_INFO("Desconectado: %s (0x%02x)\n", addr_str, reason);

    bt_conn_unref(g_state->conn_ref);
    g_state->conn_ref = NULL;

    // Start scanning again
    ble_start_scan();
} // ble_evt_on_disconnect

static int ble_start_scan()
{
    LOG_TRACE("ble_start_scan");

    int err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, ble_evt_on_device_found);
    if (0 != err)
    {
        LOG_ERROR("Erro ao inicializar scan de dispositivos");
        return err;
    }

    LOG_INFO("Escaneando dispositivos...");

    return 0;
} // ble_start_scan

int start_ble_central(central_state_t *state)
{
    LOG_TRACE("start_ble_central");

    int err = bt_enable(NULL);

    if (0 != err)
        return err;

    ble_evt_on_ready(state);

    return 0;
} // start_ble_central
