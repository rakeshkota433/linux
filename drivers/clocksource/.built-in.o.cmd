cmd_drivers/clocksource/built-in.o :=  arm-poky-linux-gnueabi-ld -EL    -r -o drivers/clocksource/built-in.o drivers/clocksource/clksrc-of.o drivers/clocksource/mmio.o drivers/clocksource/arm_arch_timer.o drivers/clocksource/arm_global_timer.o drivers/clocksource/dummy_timer.o 