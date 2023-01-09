# DCA1000-EVM-remote-collection
这是一个基于Qt开发的DCA1000EVM远程采集的程序，有加入：

1. TCP/IP通讯部分
该部分又分为：离线采集、在线传输回来
2. 编码部分
该部分又分为：动态哈夫曼编码、无编码设计
3. 服務器、客戶端部分
該部分請自行查看吧
4. UDP通信部分
和DCA1000EVM通信
5. 串口通信部分
和TI雷達天綫板通信

我所使用的是AWR1843AOPEVM 如果你使用其他雷達 請自行修改cfg文件與 _AR_UI_.cpp 内部的參數部分
效果如下：

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E6%95%88%E6%9E%9C%E5%9C%96%E5%B1%95%E7%A4%BA.jpg?raw=true)

後續開發Linux-ARM版本。
