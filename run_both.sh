#!/bin/bash

CENTRAL_BIN="$(pwd)/central-ble/.pio/build/nrf52840_dk/firmware.elf"
PERIPHERAL_BIN="$(pwd)/peripheral-ble/.pio/build/nrf52840_dk/firmware.elf"

renode -e \
    "using sysbus

emulation CreateBLEMedium \"wireless\"

mach create \"central\"
machine LoadPlatformDescription @platforms/cpus/nrf52840.repl
connector Connect sysbus.radio wireless

showAnalyzer uart0

mach create \"peripheral\"
machine LoadPlatformDescription @platforms/cpus/nrf52840.repl
connector Connect sysbus.radio wireless

showAnalyzer uart0

emulation SetGlobalQuantum \"0.00001\"

macro reset
\"\"\"
    mach set \"central\"
    sysbus LoadELF @$CENTRAL_BIN

    mach set \"peripheral\"
    sysbus LoadELF @$PERIPHERAL_BIN 
\"\"\"

runMacro \$reset

start
"
