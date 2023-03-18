

## 字体库下载
##                                       端口           波特率                   flash位置     字体库
python esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x15D000 DesktopScreenFont.bin