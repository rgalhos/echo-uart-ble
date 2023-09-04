#include <ble/application.h>

static const struct bt_data adv_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

static const int adv_data_size = ARRAY_SIZE(adv_data);

static int ble_evt_on_ready(peripheral_state_t *state);
static void ble_evt_on_connect(struct bt_conn *conn, uint8_t err);
static void ble_evt_on_disconnect(struct bt_conn *conn, uint8_t err);
static int ble_start_advertising();

static peripheral_state_t *g_state = NULL;

// ------------------------------------------------------------

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = ble_evt_on_connect,
    .disconnected = ble_evt_on_disconnect,
};

static int ble_evt_on_ready(peripheral_state_t *state)
{
    LOG_TRACE("ble_evt_on_ready");
    LOG_INFO("Bluetooth inicializado");

    assert(NULL != state);

    g_state = state;
    LOG_DEBUG("Estado setado");

    int ret = ble_start_advertising();

    return ret;
} // ble_evt_on_ready

static void ble_evt_on_connect(struct bt_conn *conn, uint8_t err)
{
    LOG_TRACE("ble_evt_on_connect");

    assert(NULL != g_state);

    if (0 != err)
    {
        LOG_ERROR("Falha na conexao: %d", err);
        return;
    }

    g_state->conn_ref = bt_conn_ref(conn);
    g_state->status = e_ble_connected;

    LOG_INFO("Periférico conectado com sucesso!");
} // ble_evt_on_connect

static void ble_evt_on_disconnect(struct bt_conn *conn, uint8_t reason)
{
    LOG_TRACE("ble_evt_on_disconnect");
    LOG_INFO("Periferico desconectado: %d", reason);

    assert(NULL != g_state);

    if (NULL != g_state->conn_ref)
        bt_conn_unref(g_state->conn_ref);
    g_state->conn_ref = NULL;
    g_state->status = e_ble_disconnected;

    // Start advertising again
    ble_start_advertising();
} // ble_evt_on_disconnect

static int ble_start_advertising()
{
    LOG_TRACE("ble_start_advertising");

    int err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, adv_data, adv_data_size, NULL, 0);

    return 0;
    if (0 != err)
    {
        LOG_ERROR("Erro ao iniciar serviço de advertisement: %d\n", err);
        return err;
    }

    return 0;
} // ble_start_advertising

int start_ble_peripheral(peripheral_state_t *state)
{
    LOG_TRACE("start_ble_peripheral");

    int err = bt_enable(NULL);
    if (0 != err)
        return err;

    ble_evt_on_ready(state);

    return 0;
} // start_ble_peripheral
