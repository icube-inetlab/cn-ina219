# cn-ina219

Current ControlNode based on Adafruit INA219 sensor and an Raspberry Pi (2 or 3) OpenGateway.

## Wiring  

```
OpenNode --- USB-OUT -- INA219 -- USB-IN --- Raspberry Pi
```

See schema ``docs/usb_energy_monitoring.png`` for a detailled electronic component setup.

## Software requirements

* Tested with Raspbian 8 and Raspberry Pi 2 board.
* Enable i2c module

## Build

```
cd src
make
```

## Usage

```
./energy
```

## TODO

```
* Generate OML output log format, same as FIT IoT-LAB
* Add program arguments for sampling frequency, sampling accuracy, etc.
```
