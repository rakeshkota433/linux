cmd_drivers/input/touchscreen/built-in.o :=  arm-poky-linux-gnueabi-ld -EL    -r -o drivers/input/touchscreen/built-in.o drivers/input/touchscreen/of_touchscreen.o drivers/input/touchscreen/ads7846.o drivers/input/touchscreen/elan_ts.o drivers/input/touchscreen/egalax_ts.o drivers/input/touchscreen/imx6ul_tsc.o drivers/input/touchscreen/max11801_ts.o drivers/input/touchscreen/mc13783_ts.o drivers/input/touchscreen/stmpe-ts.o drivers/input/touchscreen/tsc2007.o 
