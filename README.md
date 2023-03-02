//设置目标板卡相关
idf.py set-target esp32

//可配置工程属性
idf.py menuconfig

idf.py fullclean
idf.py build

//下载
idf.py -p /dev/ttyUSB0 flash
//监视器
idf.py -p /dev/ttyUSB0 monitor
//构建、下载、监视
idf.py -p /dev/ttyUSB0 flash monitor

git checkout -b name
git branch -a

git init
git add
git commit -m ""
git push origin 远程仓库链接
git push -u origin dev1

存储代码