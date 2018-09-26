cmd_arch/arm/boot/dts/imx7d-sdb.dtb := mkdir -p arch/arm/boot/dts/ ; arm-poky-linux-gnueabi-gcc -E -Wp,-MD,arch/arm/boot/dts/.imx7d-sdb.dtb.d.pre.tmp -nostdinc -I./arch/arm/boot/dts -I./arch/arm/boot/dts/include -I./drivers/of/testcase-data -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.imx7d-sdb.dtb.dts.tmp arch/arm/boot/dts/imx7d-sdb.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/imx7d-sdb.dtb -b 0 -i arch/arm/boot/dts/  -d arch/arm/boot/dts/.imx7d-sdb.dtb.d.dtc.tmp arch/arm/boot/dts/.imx7d-sdb.dtb.dts.tmp ; cat arch/arm/boot/dts/.imx7d-sdb.dtb.d.pre.tmp arch/arm/boot/dts/.imx7d-sdb.dtb.d.dtc.tmp > arch/arm/boot/dts/.imx7d-sdb.dtb.d

source_arch/arm/boot/dts/imx7d-sdb.dtb := arch/arm/boot/dts/imx7d-sdb.dts

deps_arch/arm/boot/dts/imx7d-sdb.dtb := \
  arch/arm/boot/dts/include/dt-bindings/input/input.h \
  arch/arm/boot/dts/imx7d.dtsi \
  arch/arm/boot/dts/include/dt-bindings/clock/imx7d-clock.h \
  arch/arm/boot/dts/include/dt-bindings/gpio/gpio.h \
  arch/arm/boot/dts/include/dt-bindings/interrupt-controller/arm-gic.h \
  arch/arm/boot/dts/include/dt-bindings/interrupt-controller/irq.h \
  arch/arm/boot/dts/imx7d-pinfunc.h \
  arch/arm/boot/dts/imx7d-pinfunc-lpsr.h \
  arch/arm/boot/dts/skeleton.dtsi \

arch/arm/boot/dts/imx7d-sdb.dtb: $(deps_arch/arm/boot/dts/imx7d-sdb.dtb)

$(deps_arch/arm/boot/dts/imx7d-sdb.dtb):
