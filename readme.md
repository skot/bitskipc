## bitskipc
First attempt at a new BM1397 driver. Mostly based off [kano's cgminer](https://github.com/kanoi/cgminer) and [sniffing communication](bm1397_protocol.md) with the [bitaxe](https://github.com/skot/bitaxe).

Massive help from @Georges760 [BM1397 register documentation](https://github.com/skot/BM1397/blob/master/registers.md) effort

This uses [libftdi](https://www.intra2net.com/en/developer/libftdi/) to communicate with a [FTDI FT230X](https://ftdichip.com/products/ft230xq/). I have had good luck with the [jim.sh micro1v8](https://jim.sh/1v8/) adapter, but any 1.8V FTDI adapter could work as long as it has the RTS pin broken out.

1.8V IO voltage is very important when interfacing with the BM1397.

## Prerequisites
- cmake
- 1.8V FTDI usbserial adapter attached
    - If the ProductID:VendorID of your FTDI adapter is not 0x0403:0x6015, then you should change this in serialmonitor.c

## Building
1. `mkdir lib; cd lib`
2. `git clone https://github.com/wjakob/libftdi.git`
3. `cd ..`
4. `mkdir build; cd build`
5. `cmake ..`
6. `make`

## Running
1. `./bitskipc`