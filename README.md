# 高铁洗车机项目-软件部分

## 项目概述
- 该版本代码适配4、5、6号洗车机，与3号洗车机不兼容  
***主控采用STM32F103VET6***  
项目的主控制器采用了STM32F103VET6微控制器，这是一款性能强大且功能丰富的ARM Cortex-M3处理器。它提供了丰富的外设和接口，使其成为嵌入式系统的理想选择。STM32F103VET6不仅具有高速时钟和内存资源，还具备丰富的通信接口（如SPI、I2C、USART）和定时器，以满足各种硬件需求。此外，它的低功耗特性对于长期运行的嵌入式系统尤为重要。  
***多线程技术和协程***  
项目的核心架构基于嵌入式实时操作系统（RTOS）的开发，以此实现多线程技术，从而提高整体系统的实时性。系统被拆分为大约十个线程，这种多线程的设计允许各个任务并行执行，最大程度地提高了系统的效率和性能。在某些线程下，还需要使用协程来进一步优化代码，实现更细粒度的任务切换，从而降低了上下文切换的开销。  
***事件消息中断驱动型***  
项目采用了事件消息中断驱动型的编程模型，这种模型的优点在于系统大部分时间处于待命态。通过等待事件触发或中断请求，系统可以最大程度地节约CPU资源，避免了无谓的循环等待，提高了系统的能效和实时响应性。这种异步事件处理的方式使得项目更容易扩展和维护，同时确保了高度的实时性。

## 开发环境
- 采用Keil V5编译器，不可使用高版本编译器
- 使用git做版本控制
- 使用DAP-LINK实现代码的下载与日志监控
- 使用STM32CUBEMX实现初始化代码的生成
- 使用QT6.4实现了上位机的软件开发
- 使用linux终端实现彩色日志的解析
- 使用串口助手实现自动化的命令发送
- 使用vscode实现代码的自动格式化

## 全功能概述
- 电机驱动
    - 实现对和利时绝对值电机的控制，包括模式设置，速度设置，脉冲设置
- 刹车控制
    - 通过判断电机的到位状态自动锁闭刹车，保证安全
- 液位读取
    - 可以通过电阻式液位计读取四种液位，包括清水箱液位，原液箱液位，~~混合箱（缓冲箱）液位~~，~~喷液箱液位~~
- 上位机通信
    - 具备上位机通信能力，可以响应工控机命令，回传心跳包，接受按钮板的电池信息等
- 流量计通信
    - 通过RS485接口和modbus协议与两块流量计通信，可以读取清水泵和原液泵的实时流量
- 单线RGB彩灯
    - 拥有两路WS2812单总线RGB彩灯控制，可以同时输出两路不同的单总线信号
- 水泵控制
    - 通过mos实现对水泵的开启关闭
- 滚刷控制
    - 通过对滚刷驱动器的使能控制实现滚刷的启停
- 到位传感器读取
    - 通过光耦隔离读接近传感器的到位状态
- 电池信息的读取
    - 通过RS485读取电池信息，包括剩余容量等信息
- 日志系统
    - 通过移植第三方日志库，能够实现彩色等级日志的输出功能
- Flash fal中间件
    - 通过移植三方fal库，实现了上下层软硬件层级之间的高度隔离
- 磨损均衡
    - 通过移植三方基于fal的磨损均衡库，实现了Flash的磨损均衡功能，大幅增加Flash的使用寿命
- 串口控制台
    - 通过串口实现了类Linux控制台的功能，可以实现非Debug情况下的运行时调试等动能
- 自动配液
    - 通过液位计与流量计与水泵开关的相互配合，可以实现高精度的溶液配比功能，误差极低
- 实时操作系统
    - 通过使用嵌入式实时操作系统，大幅提高代码的实时性，实现了多线程调度功能

## 技术细节
### 一、电机驱动、刹车控制
- 电机使用的是和利时的无刷伺服电机，通过对应的驱动器控制，驱动器使用can总线通信，控制器区域网络（Controller Area Network，CAN）是一种用于在嵌入式系统中进行高可靠性通信的串行通信协议。CAN总线广泛应用于汽车、工业自动化、航空航天、医疗设备等领域，以支持设备之间的数据传输和通信，CAN总线的物理层通常采用双绞线，以提高传输能力，CAN总线采用冲突检测机制，可以检测多个设备同时尝试发送消息时的冲突。这有助于确保数据的完整性，并且可以防止数据碰撞，CAN总线具有网络管理功能，包括设备的启动、停止、重启和错误处理。这些功能可用于确保网络中的设备能够协同工作，并在故障情况下能够自动进行恢复，在通信方面采用的是单线等待制方案，该方案的特点是准确度高，但是实时性略低，这里并没有采用支持高并发的方案，后续可以以高并发为方向重做电机驱动。由于驱动器自身缺陷，所以电机的刹车部分需要自行控制，刹车的开启需要启动时间，所以需要提前将开启时间考虑进程序中
### 二、液位读取
- 液位读取依靠电阻式液位计，所以通过识别当前的电阻值即可推算大致的液位，但是由于液位计的精度不高，所以导致读取的数据难以稳定。使用ADC进行电压的采样，为了提高采用的稳定性和精确性，程序中以500个采样点计算一次平均值，每次计算均保留前值，起到了滤波抗抖动的作用
### 三、 Flash存储
- FAL (Flash Abstraction Layer) Flash 抽象层，是对 Flash 及基于 Flash 的分区进行管理、操作的抽象层，对上层统一了 Flash 及 分区操作的 API (框架图如下所示)，并具有以下特性：
    - 支持静态可配置的分区表，并可关联多个 Flash 设备；
    - 分区表支持 **自动装载** 。避免在多固件项目，分区表被多次定义的问题；
    - 代码精简，对操作系统 **无依赖** ，可运行于裸机平台，比如对资源有一定要求的 Bootloader；
    - 统一的操作接口。保证了文件系统、OTA、NVM等对 Flash 有一定依赖的组件，底层 Flash 驱动的可重用性；
    - 自带基于 Finsh/MSH 的测试命令，可以通过 Shell 按字节寻址的方式操作（读写擦） Flash 或分区，方便开发者进行调试、测试；
- FlashDB是一款超轻量级的嵌入式数据库，专注于提供嵌入式产品的数据存储方案。与传统的基于文件系统的数据库不同，FlashDB 结合了 Flash 的特性，具有较强的性能及可靠性。并在保证极低的资源占用前提下，尽可能延长 Flash 使用寿命。FlashDB 提供两种数据库模式：
    - **键值数据库** ：是一种非关系数据库，它将数据存储为键值（Key-Value）对集合，其中键作为唯一标识符。KVDB 操作简洁，可扩展性强。
    - **时序数据库** ：时间序列数据库 （Time Series Database , 简称 TSDB），它将数据按照 **时间顺序存储** 。TSDB 数据具有时间戳，数据存储量大，插入及查询性能高。
- 主要特性
    - 资源占用极低，内存占用几乎为 **0** ;
    - 支持 多分区，**多实例** 。数据量大时，可细化分区，降低检索时间；
    - 支持 **磨损平衡** ，延长 Flash 寿命；
    - 支持 **掉电保护** 功能，可靠性高；
    - 支持 字符串及 blob 两种 KV 类型，方便操作；
    - 支持 KV **增量升级** ，产品固件升级后， KVDB 内容也支持自动升级；
    - 支持 修改每条 TSDB 记录的状态，方便管理；
### 四、RGB单总线技术
- WS2812是一种广泛用于LED照明和显示应用的智能RGB LED灯珠，也被称为NeoPixel。它是一种可编程的LED设备，具有内置的驱动电路和控制逻辑，可以通过串行通信接口进行控制，WS2812包括三个彩色LED（红色、绿色和蓝色）和一个控制芯片，所有这些都封装在一个小型塑料外壳中。这种集成结构使其非常适合高密度LED照明和显示，WS2812通过单线串行通信接口接收数据，通过不同电平的时间持续性，WS2812能够解释数据，一个WS2812的数据包包括24位数据，分别代表红色、绿色和蓝色的亮度值，由于每个WS2812都是独立控制的，因此可以实现各种动态和多彩的照明效果，WS2812内置了驱动电路和控制逻辑，使得控制非常简单，不需要额外的外部组，WS2812可以级联连接，使得多个WS2812可以通过同一通信线路进行控制
    - 一代控制方法：CPU延时翻转IO
        - 使用通用I/O引脚，通过cpu的延时翻转IO实现数据的通信，但是过于消耗cpu资源，因此弃用
    - 二代控制方法：SPI接口
        - 通过使用单片机的SPI（Serial Peripheral Interface）接口，可以控制WS2812，通过配置SPI接口，并设置数据传输的速率以匹配WS2812的时序，使用SPI接口发送正确格式的数据帧，其中包括RGB颜色数据，使用SPI的硬件时钟和数据传输机制，减少了CPU的无意义消耗，但是由于SPI接口会多浪费一个IO口，因此弃用
    - 三代控制方法：DMA+PWM+TIM
        - 通过高级定时器生成对应频率的脉宽调制波，然后将要发送的数据放在一个缓冲区中，使用直接内存访问技术代替cpu搬运数据，极大的释放了CPU的占用时间，同时只使用了一个IO口，避免了无意义的IO口浪费
### 五、串口控制台
- 自实现了串口控制台的功能，串口控制台是一种通过串行通信接口连接到计算机或嵌入式系统的命令行界面，用于与系统进行交互、配置和监控，可以很方便的调试、配置和维护嵌入式系统、嵌入式设备、微控制器等，特别适用于无图形界面的设备，串口接收部分使用FreeRTOS操作系统+任务通知+DMA+空闲中断实现，首先初始化控制台，主要是启动串口接收，因为控制台使用了动态单向链表，所以还需要为头节点分配内存，初始化完成后即可将自己需要的命令字和功能回调函数注册进去（命令字和功能函数是一一对应的，每个命令都对应了一个特定的功能函数），之后由串口接收任务等待上位机命令，接收到命令后根据命令去调用指定的功能函数即可
- 程序上提供了注册命令的函数，用于给一个命令名注册一个对应的回调函数，但是回调函数的类型必须保持一致，只可以用于接受一个由控制台传入的命令行参数，由函数内部自行实现命令的二次解析
### 六、事件通知
- 在整个系统当中，所有线程间的通信几乎都是由事件通知来完成的，每个RTOS任务都有一个任务通知数组。每条任务通知 都有“挂起”或“非挂起”的通知状态， 以及一个 32 位通知值
- 直达任务通知是直接发送至任务的事件， 而不是通过中间对象 （如队列、事件组或信号量）间接发送至任务的事件。 向任务发送“直达任务通知” 会将目标任务通知设为“挂起”状态。 正如任务可以阻塞中间对象（如等待信号量可用的信号量），任务也可以阻塞任务通知， 以等待通知状态变为“挂起”。
- 性能优势和使用限制
    - 任务通知具有高度灵活性，使用时无需创建单独队列、二进制信号量、计数信号量或事件组。通过直接通知解除RTOS任务阻塞状态的速度和使用中间对象（如二进制信号量）相比快了45%，使用的RAM也更少



## 目录结构
```
├─Core				            核心文件 
│
└─my_drivers			        所有驱动
    ├─brush			            刷组上层，包括所有的洁具控制相关部分
    │
    ├─console                   串口控制台驱动
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
