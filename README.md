# HTTPProxy 基于HTTP代理的内容过滤工具
2022毕业设计作品

因为一些原因，代码与论文从零开始到完成只有两周不到的时间，导致代码质量非常低，并且存在目前尚未修复的BUG
实现功能：HTTP代理服务器与利用第三方库的分词，实现过程中踩坑无数，包括网页编码问题、网页压缩问题、未知的原因导致的QT内置服务器类无法连接WEB服务器、多线程问题

已知问题：
1. 多线程相关部分实现存在严重逻辑问题，多次开启关闭程序开关会导致程序崩溃，并且多线程类的地方我自己都不知道逻辑是啥，写的一团迷糊。不过答辩的时候专门避开了这个地方的演示。
2. 不支持HTTPS
3. 第三方分词库需要每月更新授权，目前内置的授权已经过期
4. 代码可读性差，逻辑实现不通
5. 代理需要手动在系统设置中进行设置，指向自定义的端口

具体踩坑记录见word文档[详细文档.docx](https://github.com/Qiaofanxing/HTTPProxy/files/11279290/default.docx)


附上图片：

![image](https://user-images.githubusercontent.com/41269801/233241800-cfeb2f40-e4f5-494d-b08e-9a7bcf0e354b.png)
![image](https://user-images.githubusercontent.com/41269801/233241878-cb98ac1b-bd7f-4c85-8726-26b444690811.png)
