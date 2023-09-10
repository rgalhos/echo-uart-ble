#include <zephyr.h>
#include <kernel.h>
#include <console/console.h>

#include <ble/application.h>
#include <ble/gatt_uart_service.h>
#include <utils.h>

central_state_t state;

int main()
{
    LOG_TRACE("main");

    state.conn_ref = NULL;
    state.on_connection_success = on_connection_success;

    int err = start_ble_central(&state);
    if (0 != err)
    {
        LOG_ERROR("Erro ao inicializar bluetooth: %d", err);
        return err;
    }

    LOG_TRACE("fin");

    return 0;
} // main
