#include <zephyr.h>
#include <kernel.h>

#include <ble/application.h>
// #include <ble/uart_service.h>
#include <utils.h>

int main()
{
    LOG_TRACE("main");

    peripheral_state_t state = {
        .conn_ref = NULL,
        .status = e_ble_disconnected,
    };

    int err = start_ble_peripheral(&state);
    if (0 != err)
    {
        LOG_ERROR("Erro ao inicalizar bluetooth: %d", err);
        return err;
    }

    // initialize_uart_service(&state);

    while (1)
    {
        k_sleep(K_SECONDS(1));
        LOG_DEBUG("iteration");
    }

    LOG_TRACE("fin");

    return 0;
} // main
