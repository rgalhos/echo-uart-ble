#include <zephyr.h>
#include <kernel.h>

#include <ble/application.h>
#include <utils.h>

void init_gatt_uart_blabla_to_do(void *central_state)
{
    LOG_TRACE("init_gatt_uart_blabla_to_do");
    central_state_t *state = (central_state_t *)central_state;

    if (NULL == state->conn_ref)
    {
        LOG_DEBUG("Sem conexão???");
        return;
    }

    // WIP

    return;
} // init_gatt_uart_blabla_to_do

int main()
{
    LOG_TRACE("main");

    central_state_t state = {
        .conn_ref = NULL,
        .on_connection_success = init_gatt_uart_blabla_to_do,
    };

    int err = start_ble_central(&state);
    if (0 != err)
    {
        LOG_ERROR("Erro ao inicializar bluetooth: %d", err);
        return err;
    }

    while (1)
    {
        k_sleep(K_SECONDS(1));
        LOG_DEBUG("iteration");
    }

    LOG_TRACE("fin");

    return 0;
} // main
