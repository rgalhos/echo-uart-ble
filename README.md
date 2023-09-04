Projeto da disciplina de Sistemas Embarcados 2023.1 - ECOM042-T: echo UART pelo BLE, com conversão de letras minúsculas

> ROTEIRO
>
> - Implementar o periférico que deverá
>
>   - ter uma característica com permissão de escrita e que irá receber os dados
>   - ter uma característica sem permissão e que realiza notificação
>     - a notificação deverá ser enviada sempre que um dado for recebido
>     - o conteúdo da notificação é o dado recebido com as letras minúsculas convertidas para maiúsculas
>
> - Implementar o central que deverá
>
>   - procurar e se conectar ao periférico
>   - enviar ao periférico o que o usuário escreve no terminal
>   - imprimir a resposta enviada pelo periférico
>
> - Testar a implementação no Renode

### Referências

- Documentação do Zephyr
- Exemplos encontrados no repositório do Zephyr
  - central_gatt_write: https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/bluetooth/central_gatt_write
  - central: https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/bluetooth/central
  - peripheral: https://github.com/zephyrproject-rtos/zephyr/tree/main/samples/bluetooth/peripheral
- Rede BLE no renode com dois dispositivos: https://github.com/renode/renode/blob/master/scripts/multi-node/nrf52840-ble-zephyr.resc

### Como rodar

`./run_both.sh` Para rodar a central e o dispositivo na rede BLE

`./peripheral-ble/run_peripheral.sh` Para rodar o periférico

`./central-ble/run_central.sh` Para rodar a central
