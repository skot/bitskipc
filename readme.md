First attempt at a new BM1397 driver. Mostly based off [kano's cgminer](https://github.com/kanoi/cgminer) and [sniffing communication](bm1397_protocol.md) with the [bitaxe](https://github.com/skot/bitaxe).

This uses [libftdi](https://www.intra2net.com/en/developer/libftdi/) to communicate with a [FTDI FT230X](https://ftdichip.com/products/ft230xq/). I have had good luck with the [jim.sh micro1v8](https://jim.sh/1v8/) adapter, but you need to [modify cgminer](https://github.com/skot/cgminer) to use RTS for reset instead of CBUS0. 

1.8V IO voltage is very important when interfacing with the BM1397.