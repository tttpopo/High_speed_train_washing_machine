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
├─Core				核心文件 
│
└─my_drivers			驱动
    ├─brush			刷组上层，包含大小臂
    │      brush.c
    │      brush.h
    │
    ├─console
    │      console.c
    │      console.h
    │
    ├─easylogger
    │  ├─inc
    │  │      elog.h
    │  │      elog_cfg.h
    │  │
    │  ├─port
    │  │      elog_port.c
    │  │
    │  └─src
    │          elog.c
    │          elog_async.c
    │          elog_buf.c
    │          elog_utils.c
    │
    ├─fal
    │  ├─inc
    │  │      fal.h
    │  │      fal_cfg.h
    │  │      fal_def.h
    │  │
    │  ├─porting
    │  │      fal_cfg.h
    │  │      fal_flash_port.c
    │  │      fal_flash_sfud_port.c
    │  │      fal_flash_stm32f2_port.c
    │  │      README.md
    │  │
    │  └─src
    │          fal.c
    │          fal_flash.c
    │          fal_partition.c
    │          fal_rtt.c
    │
    ├─FlashDB
    │  ├─inc
    │  │      fdb_cfg.h
    │  │      fdb_def.h
    │  │      fdb_low_lvl.h
    │  │      flashdb.h
    │  │
    │  └─src
    │          fdb.c
    │          fdb_file.c
    │          fdb_kvdb.c
    │          fdb_tsdb.c
    │          fdb_utils.c
    │
    ├─flowmeter
    │      flowmeter_hl.c
    │      flowmeter_hl.h
    │
    ├─Host_computer_protocol
    │      hcp.c
    │      hcp.h
    │
    ├─Liquid_level
    │      liquid_level.c
    │      liquid_level.h
    │
    ├─motor
    │      motor_drive.c
    │      motor_drive.h
    │      motor_hal.c
    │      motor_hal.h
    │
    └─ws2812
            ws2812.c
            ws2812.h
```