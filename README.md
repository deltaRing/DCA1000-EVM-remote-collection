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

注意：
我所使用的是AWR1843AOPEVM 如果你使用其他雷達 請自行修改cfg文件與 _AR_UI_.cpp 内部的參數部分
同時，你需要燒錄你的雷達板子進CLI文件，我會一并上傳

效果如下：

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E6%95%88%E6%9E%9C%E5%9C%96%E5%B1%95%E7%A4%BA.jpg?raw=true)

後續開發Linux-ARM版本。

目前已知問題：
1. UDP有可能丟包，這通常出現在電腦較卡的情況中

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E4%B8%9F%E5%8C%85%E8%AA%AC%E6%98%8E.png?raw=true)

2. 波形有可能會不對，但是在進一步的RD譜圖分析中並沒發現這個問題

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E9%80%B2%E4%B8%80%E6%AD%A5%E6%B3%A2%E5%BD%A2%E5%88%86%E6%9E%90.png?raw=true)

3. 連接會一直出現-9的問題，這個問題我也不是很清楚，因爲我并不是專業做TCP通信的

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E9%80%A3%E6%8E%A5%E6%AA%A2%E6%B8%AC%E5%95%8F%E9%A1%8C.png?raw=true)

4. 編譯大概率出現一堆radar_client 與雷達服務器的問題，請在對應的文件開頭與末尾加入：

![alt text](https://github.com/deltaRing/DCA1000-EVM-remote-collection/blob/main/%E7%B7%A8%E8%AD%AF%E5%95%8F%E9%A1%8C.png?raw=true)

其他問題待補充。
