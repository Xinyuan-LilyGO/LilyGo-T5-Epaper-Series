

<h1 align = "center">🌟LilyGo T5 Epaper Series🌟</h1>

### [English](../README.MD) | 中文

--------------------------------------


<h2 align = "left">快速开始⚡:</h2>

1. 把lib 文件夹里所有的文件夹 拷贝到 `"C:\User\<YourName>\Documents\Arduino\libraries"`
2. 在草图的最上方,定义要使用的板子和屏幕的型号
3. 在开发板中选择`ESP32 Dev Module`，其他选项保持默认
4. 如果你在开发板中没找到ESP32系列，那么你该看下面安装方法[如何在ArduinoIDE中安装ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)
5. 关于你购买的板子的版本,请查看下面的产品连接
6. 当你认为你的板子有问题时,你可以在`firmware`目录拉取与你对应屏幕型号的二进制文件，按照目录内的`SETTING.png`设置后,将它下载到板子，然后观察执行情况.



<h2 align = "left">产品链接📷:</h2>

|          Examples           |                             Product  Link                             |                     Schematic                      | Status |
| :-------------------------: | :-------------------------------------------------------------------: | :------------------------------------------------: | :----: |
|         [T5 V2.2]()         |   [Product link](https://www.aliexpress.com/item/32850386996.html)    |        [Schematic](./schematic/T5V2.2.pdf)         |   ✅    |
|         [T5 V2.3]()         |   [Product link](https://www.aliexpress.com/item/32869729970.html)    |        [Schematic](./schematic/T5V2.3.pdf)         |   ✅    |
|         [T5 V2.4]()         |   [Product link](https://www.aliexpress.com/item/32860674791.html)    |        [Schematic](./schematic/T5V2.4.pdf)         |   ✅    |
|         [T5 V2.8]()         |   [Product link](https://www.aliexpress.com/item/32867880658.html)    |        [Schematic](./schematic/T5v2.8.pdf)         |   ✅    |
|        [T5 2.66"]()         | [Product link](https://www.aliexpress.com/item/1005002474854718.html) |        [Schematic](./schematic/T5_2.66.pdf)        |   ✅    |
|   [MINI E-Paper 1.02"]()    | [Product link](https://www.aliexpress.com/item/1005002857956100.html) | [Schematic](./schematic/MINI%20E-Paper%201.02.pdf) |   ✅    |
| [T-Dispay E-paper 1.02" ]() | [Product link](https://www.aliexpress.com/item/1005002536380666.html) |                   [Schematic]()                    |   ✅    |


<h2 align = "left">示例描述:</h2>

```
examples
├── GxEPD2_Z90_Eaxmple              # 1.54" 红黑白三色示例 
├── GxEPD_Example                   # 适配所有LilyGo 电子纸
├── GxEPD_Example_MP3               # 播放MP3示例,V2.13不适用 
├── GxEPD_U8G2_Fonts_Example        # U8g2 字体示例
├── GxEPD_Hello_world               # 你好世界 示例
├── GxEPD_RGB_LED                   # RGB LED 示例
├── GxEPD_SD_1.02_Example           # 1.02" SD 示例
└── GxEPD_Partial_Update_Example    # 局部刷新示例
```
