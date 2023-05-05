# 高铁洗车机项目-软件部分


## 概述
- 用于实现相关硬件的对应需求，所有的输入输出均作了隔离，输出驱动使用MOS管，主控采用stm32f103vet6

## 实现功能
- 电机驱动
    - 实现对和利时绝对值电机的控制，包括设置模式，设置速度，设置脉冲等
- 液位读取
- 上位机通信
- 两块流量计通信
- 控制RGB彩灯
- 水泵控制
- 滚刷控制
- 到位传感器读取
 
## 目录结构
```
├─Core				            核心文件 
│
└─my_drivers			        驱动
    ├─brush			            刷组上层，包含大小臂
    │
    ├─console                   控制台驱动
    │
    ├─easylogger                日志库
    │
    ├─fal                       flash fal层
    │
    ├─FlashDB                   flash 数据库
    │
    ├─flowmeter                 流量计驱动
    │
    ├─Host_computer_protocol    上位机驱动
    │
    ├─Liquid_level              液位驱动
    │
    ├─motor                     电机底层驱动
    │
    └─ws2812                    RGB驱动
```