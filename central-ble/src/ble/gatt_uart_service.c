#include <ble/gatt_uart_service.h>

// methods:
static int gatt_discover();
static int gatt_subscribe();
static void start_transmitter();
static void transmit_message(char *message);
static void bt_evt_subscribe_write(struct bt_conn *conn,
                                   uint8_t err,
                                   struct bt_gatt_write_params *params);
static uint8_t bt_evt_subscribe_notify(struct bt_conn *conn,
                                       struct bt_gatt_subscribe_params *params,
                                       const void *data,
                                       uint16_t length);
static uint8_t gatt_discover_fn(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                struct bt_gatt_discover_params *params);

// structs:
static struct bt_uuid_16 uuid_uart_to_uppercase = BT_UUID_INIT_16(0xAB01);
static struct bt_uuid_16 uuid_uart_receive_data = BT_UUID_INIT_16(0xAB02);
static struct bt_uuid_16 uuid_uart_send_data = BT_UUID_INIT_16(0xAB03);

static struct bt_gatt_discover_params discover_params = {
    .uuid = &uuid_uart_to_uppercase.uuid,
    .func = gatt_discover_fn,
    .start_handle = BT_ATT_FIRST_ATTTRIBUTE_HANDLE,
    .end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE,
    .type = BT_GATT_DISCOVER_PRIMARY,
};

static struct bt_gatt_subscribe_params subscription_params = {
    .notify = bt_evt_subscribe_notify,
    .write = bt_evt_subscribe_write,
    .ccc_handle = 0,                 /* Auto-discover CCC*/
    .disc_params = &discover_params, /* Auto-discover CCC */
    .end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE,
    .value = BT_GATT_CCC_NOTIFY,
};

// globals:
static central_state_t *g_state;
static uint16_t g_handle_receive;
static atomic_t ready_to_write = false;

// ------------------------------------------------------------

K_THREAD_DEFINE(
    transmitter_thread, // name
    1024,               // stack size
    start_transmitter,  // entry point
    NULL, NULL, NULL,   // entry point parameters
    1,                  // priority
    0,                  // options flags
    0                   // delay
);

static void start_transmitter()
{
    LOG_TRACE("start_transmitter");

    while (!atomic_get(&ready_to_write))
        k_sleep(K_MSEC(100));

    LOG_TRACE("ready_to_write = true");

    console_getline_init();

    while (1)
    {
        printk(">>> ");
        char *line = console_getline();

        if (NULL == line)
        {
            LOG_ERROR("Não foi possível ler a entrada");
            continue;
        }

        transmit_message(line);

        k_sleep(K_MSEC(100));
    }
} // start_transmitter

// ------------------------------------------------------------

static void transmit_message(char *message)
{
    LOG_TRACE("transmit_message");

    assert(NULL != g_state->conn_ref);

    int err = bt_gatt_write_without_response(g_state->conn_ref,
                                             g_handle_receive,
                                             message,
                                             strlen(message),
                                             false);
    if (0 != err)
    {
        LOG_ERROR("Erro ao enviar mensagem: %d", err);
        return;
    }

    LOG_INFO("Mensagem enviada ao periférico");
} // transmit_message

static uint8_t bt_evt_subscribe_notify(struct bt_conn *conn,
                                       struct bt_gatt_subscribe_params *params,
                                       const void *data, uint16_t length)
{
    LOG_TRACE("bt_evt_subscribe_notify");

    short response_len = length >= MAX_BLE_MSG_SIZE ? (MAX_BLE_MSG_SIZE - 1) : length;
    char response[MAX_BLE_MSG_SIZE];
    memcpy(response, data, response_len);
    response[response_len] = '\0';

    LOG_INFO("Resposta do periférico: [ %s ] (%u bytes)", response, length);

    return BT_GATT_ITER_CONTINUE;
} // bt_evt_subscribe_notify

static void bt_evt_subscribe_write(struct bt_conn *conn,
                                   uint8_t err,
                                   struct bt_gatt_write_params *params)
{
    LOG_TRACE("bt_evt_subscribe_write");

    if (0 != err)
    {
        LOG_ERROR("Erro ao se inscrever na característica %d (erro: %d)", params->handle, err);
        return;
    }

    LOG_INFO("Inscrito na característica de envio");

    atomic_set(&ready_to_write, true);
} // bt_evt_subscribe_write

static uint8_t gatt_discover_fn(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                struct bt_gatt_discover_params *params)
{
    LOG_TRACE("gatt_discover_fn");

    int err;

    if (NULL == attr)
    {
        LOG_INFO("GATT discover completo");
        memset(params, 0, sizeof(*params));

        gatt_subscribe();

        return BT_GATT_ITER_STOP;
    }

    LOG_DEBUG("Handle %u", attr->handle);

    if (0 == bt_uuid_cmp(params->uuid, &uuid_uart_to_uppercase.uuid))
    {
        LOG_INFO("Encontrou o serviço uart_to_uppercase");

        params->uuid = NULL;
        params->start_handle = attr->handle + 1;
        params->type = BT_GATT_DISCOVER_CHARACTERISTIC;

        err = bt_gatt_discover(conn, params);
        if (0 != err)
            LOG_ERROR("Erro ao buscar características: %d", err);

        return BT_GATT_ITER_STOP;
    }
    else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC)
    {
        struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;

        if (0 == bt_uuid_cmp(chrc->uuid, &uuid_uart_receive_data.uuid))
        {
            LOG_INFO("Característica de leitura encontrada");
            g_handle_receive = chrc->value_handle;
        }
        else if (0 == bt_uuid_cmp(chrc->uuid, &uuid_uart_send_data.uuid))
        {
            LOG_INFO("Característica de escrita encontrada");
        }
    }

    return BT_GATT_ITER_CONTINUE;
} // gatt_discover_fn

static int gatt_discover()
{
    LOG_TRACE("gatt_discover");

    assert(NULL != g_state);
    assert(NULL != g_state->conn_ref);

    int err = bt_gatt_discover(g_state->conn_ref, &discover_params);
    if (0 != err)
    {
        LOG_ERROR("Erro ao iniciar o GATT discover: %d", err);
        return err;
    }

    return 0;
} // gatt_discover

static int gatt_subscribe()
{
    LOG_TRACE("gatt_subscribe");

    assert(NULL != g_state);
    assert(NULL != g_state->conn_ref);

    subscription_params.value_handle = g_handle_receive;
    int err = bt_gatt_subscribe(g_state->conn_ref, &subscription_params);
    if (0 != err)
    {
        LOG_ERROR("Erro ao se inscrever na característica de leitura");
        return err;
    }

    LOG_INFO("Inscrito na característica de leitura");

    return 0;
} // gatt_subscribe

void on_connection_success(void *state)
{
    LOG_TRACE("on_connection_success");

    g_state = (central_state_t *)state;

    int err = gatt_discover();
    if (0 != err)
        return;
} // on_connection_success
