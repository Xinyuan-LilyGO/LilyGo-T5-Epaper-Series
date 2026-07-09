
<div align="center" markdown="1">
  <img src=".github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align="center">LilyGo T5 2.13 E-Paper</h1>

### Product

| Product           | SOC   | Flash         | PSRAM |
| ----------------- | ----- | ------------- | ----- |
| [T5 2.13 Inch][1] | ESP32 | 4MB(Quad-SPI) | N.A   |

[1]: https://lilygo.cc/products/t5-v2-3-1


<h2 align="left">Quick Start</h2>


### PlatformIO Quick Start

1. Install the **CH9102 USB bridge** driver for the first time.
   * [Windows](https://www.wch-ic.com/downloads/CH343SER_ZIP.html)
   * [Mac OS](https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html)
2. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
3. Search for the `PlatformIO` plugin in the `Visual Studio Code` extension and install it.
4. After the installation is complete, restart `Visual Studio Code`.
5. Select `File` -> `Open Folder` in the upper left corner, then select the `LilyGo-T5-Epaper-Series` directory.
6. Wait for the installation of third-party dependent libraries to complete.
7. Click on the `platformio.ini` file.
8. Uncomment one of the `src_dir = xxxx` lines so that only one is active. For example, to enable `src_dir = examples/Factory`, remove the leading `;` and save the file.
9. Click the (checkmark) symbol in the lower left corner to compile.
10. Connect the board to the computer via Micro-USB.
11. Click (arrow) to upload firmware.
12. Click (plug symbol) to monitor serial output.
13. If the upload fails or the USB device keeps flashing, please check the **FAQ** below.


### Arduino IDE Quick Start

1. Install the **CH9102 USB bridge** driver for the first time.
   * [Windows](https://www.wch-ic.com/downloads/CH343SER_ZIP.html)
   * [Mac OS](https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html)
2. Install [Arduino IDE](https://www.arduino.cc/en/software).
3. Install [Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/).
4. Copy all folders in the `lib` directory to the `Sketchbook location` directory.
   How to find your library path, [see here](https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer).
   * Windows: `C:\Users\%USERNAME%\Documents\Arduino\libraries`
   * MacOS: `$HOME/Documents/Arduino/libraries`
   * Linux: `$HOME/Arduino/libraries`
5. Open the corresponding example:
   * Open the downloaded `LilyGo-T5-Epaper-Series`
   * Open `examples`
   * Select the sample file ending with `.ino`
6. In Arduino IDE, select the board and settings as shown below:

   | Name                                 | Value                                |
   | ------------------------------------ | ------------------------------------ |
   | Board                                | **ESP32 Dev Module**                 |
   | Port                                 | Your port                            |
   | CPU Frequency                        | 240MHz(WiFi/BT)                      |
   | Core Debug Level                     | None                                 |
   | Erase All Flash Before Sketch Upload | Disable                              |
   | Events Run On                        | Core1                                |
   | Flash Frequency                      | 80MHz                                |
   | Flash Mode                           | QIO                                  |
   | Flash Size                           | **4MB(32Mb)**                        |
   | JTAG Adapter                         | Disabled                             |
   | Arduino Runs On                      | Core1                                |
   | Partition Scheme                     | **Huge APP (3MB No OTA/1MB SPIFFS)** |
   | PSRAM                                | **Disable**                          |
   | Upload Speed                         | 921600                               |
   | Programmer                           | **Esptool**                          |

7. At the top of the sketch, select the screen model you are using: **DEPG0213BN** (default) or **GDEY0213B74**. Uncomment the corresponding line and comment out the other.

### Pins Map

| Name                  | GPIO NUM   |
| --------------------- | ---------- |
| Display power control | 12         |
| Display MISO          | No Connect |
| Display MOSI          | 23         |
| Display SCLK          | 18         |
| Display DC            | 17         |
| Display BUSY          | 4          |
| Display RST           | 16         |
| Display CS            | 5          |
| SD SCK                | 14         |
| SD MISO               | 2          |
| SD MOSI               | 15         |
| SD CS                 | 13         |
| Battery ADC Pin       | 35         |
| Button BOOT           | 0          |
| Button 1              | 39         |

* (Display power control) GPIO12 Only V2.4 Version , v2.3.1 version not have this pin

> [!IMPORTANT]
> The ESP32 uses IO2 as the SD card CS pin. You cannot upload software when the SD card is inserted, so remove the SD card before uploading.

### FAQ

- **What is the device's sleep current?**
   1. With the screen power completely off, the external wake-up rate of the ESP32 is approximately 30uA. A complete record of this can be found [here](./examples/DeepSleep/README.MD)

- **Can't upload any sketch? Please enter upload mode manually.**
  1. Connect the board via the USB cable.
  2. Press and hold the BOOT button.
  3. While holding BOOT, press the RST button.
  4. Release the RST button.
  5. Release the BOOT button.
  6. Upload the sketch.


### Resources

- [PCB Dimensions](./dimensions/T5_2.13.dxf)
- [PCB Step](./dimensions/LILYGO_T5%202.13.stp)
- [Schematic](./schematic/T5_2.13.pdf)
- [Cases from users](https://www.printables.com/search/models?ctx=models&q=TTGO+T5)
