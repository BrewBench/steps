# BrewBench Steps

http://steps.brewbench.co

<img src="src/assets/img/brewbench-logo-265.png?raw=true" alt="BrewBench logo" title="BrewBench" align="right" />

BrewBench Steps is an Arduino step processing front end.

The initial use for this came from Greg Kallfa at [Vision Quest Brewing](http://www.visionquestbrewing.com) who built an Arduino keg washer and needed to easily adjust the timing for the washing steps.

You can also import a step manifest, check out [washer.yaml](src/assets/data/washer.yaml) or [washer-groups.yaml](src/assets/data/washer-groups.yaml)

```sh
# build code
yarn
# start webserver
gulp
```

## Sketches

### Arduino Board Options

1. Power up and connect to the default IP http://192.168.240.1
1. Set the digital pin or analog pin depending on your relay setup

#### Yun Arduino.cc setup
  * If the WiFi network starts with Arduino use password: arduino
  * Set REST API access to openÂ
  * Change or remember the host name or IP address (you will need this later)
  * Save to reboot
  * Using the [Arduino IDE](https://www.arduino.cc/en/Main/Software) upload the [Yun sketch](arduino/Yun/Yun.ino)

#### Yun Arduino.org (aka. Linino) setup
  * If the WiFi network starts with Linino use password: doghunter
  * Set REST API access to open
  * Change or remember the board name or IP address (you will need this later)
  * Save to reboot
  * Using the [Arduino IDE](https://www.arduino.cc/en/Main/Software) upload the [Yun Linino sketch](arduino/YunLinino/YunLinino.ino)

#### Mega

  * TBD

<img src="src/assets/img/screenshot.png?raw=true" alt="BrewBench Steps" align="center" />
