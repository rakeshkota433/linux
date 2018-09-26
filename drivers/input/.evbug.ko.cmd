cmd_drivers/input/evbug.ko := arm-poky-linux-gnueabi-ld -EL -r  -T ./scripts/module-common.lds --build-id  -o drivers/input/evbug.ko drivers/input/evbug.o drivers/input/evbug.mod.o
