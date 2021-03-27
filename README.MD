
<h1 align = "center">🌟LilyGo T5 Epaper Series🌟</h1>

### English | [中文](docs/details_cn.md) 

--------------------------------------


<h2 align = "left">Quick start ⚡:</h2>

1. Install the following two dependencies
     - [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)
     - For the 1.54" red black and white, the sample program uses `GxEPD2` as the driver, you need to install [ZinggJM/GxEPD2](https://github.com/ZinggJM/GxEPD2), for other models, please use this branch [lewisxhe/fork/GxEPD](https://github.com/lewisxhe/GxEPD),Please note that referencing the wrong branch will cause the program to fail to compile 
2. At the top of the sketch, define the model of the board and screen to be used 
3. Select `ESP32 Dev Module` in the development board, and keep the other options as default
4. If you do not find the ESP32 series in the development board, then you should see the following installation method [How to install ESP32 in ArduinoIDE](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)
5. For the version of the board you purchased, please check the product link below 
6. When you think there is a problem with your board, you can pull the binary file corresponding to your screen model in the firmware directory, follow the `SETTING.png` settings in the directory, download it to the board, and then observe the execution. 



<h2 align = "left">Product link📷:</h2>

|   Examples   |                          Product  Link                           |              Schematic              | Status |
| :----------: | :--------------------------------------------------------------: | :---------------------------------: | :----: |
| [ T5 V2.2]() | [Product link](https://www.aliexpress.com/item/32850386996.html) | [Schematic](./schematic/T5V2.2.pdf) |   ✅    |
| [T5 V2.3]()  | [Product link](https://www.aliexpress.com/item/32869729970.html) | [Schematic](./schematic/T5V2.3.pdf) |   ✅    |
| [T5 V2.4]()  | [Product link](https://www.aliexpress.com/item/32860674791.html) | [Schematic](./schematic/T5V2.4.pdf) |   ✅    |
| [T5 V2.8]()  | [Product link](https://www.aliexpress.com/item/32867880658.html) | [Schematic](./schematic/T5v2.8.pdf) |   ✅    |
| [T5 V1.02]() |                         [Product link]()                         |            [Schematic]()            |   X    |



<h2 align = "left">Example description:</h2>

```
examples
├── GxEPD2_Z90_Eaxmple               # 1.54" red black and white three-color electronic paper example, 
├── GxEPD_Example                    # Suitable for all LilyGo electronic paper 
├── GxEPD_Example2                   # Suitable for all LilyGo electronic paper 
├── GxEPD_Example_MP3                # Example of playing MP3 
├── GxEPD_U8G2_Fonts_Example         # U8g2 font example 
└── GxEPD_Partial_Update_Example     # Partial refresh example 
```

# Change: 
- The current version of LilyGo V2.13, the ink screen version is DEPG0213BN, please select `DEPG0213BN` in the sketch 