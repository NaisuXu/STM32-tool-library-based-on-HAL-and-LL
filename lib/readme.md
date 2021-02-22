# 基于STM32 HAL和LL的工具库使用说明

[toc]

## 工具库基础说明

该库大部分由C++编写而成，C++项目才能使用（根据情况可能还需要手动将main.c和stm32fxxx_it.c重命名为.cpp）。除了这点外该库可以直接在在STM32CubeMX生成的项目上直接使用。该库完全使用ST官方的HAL和LL库，不改变任何官方库代码。

下面工具中以 `lib_hal_` 开头的表示必须基于HAL库使用， `lib_ll_` 开头的表示必须基于LL库使用，其余工具并无此限制。



## lib_hal_ticker

该库是一个简易的调度器，在对精度要求不高的场合可以用这个库来处理需要定时运行的任务。

### 使用说明

使用时首先声明Lib_HAL_Ticker对象和任务函数（也可以用匿名函数）：

```c++
Lib_HAL_Ticker task1, task2; // 声明Lib_HAL_Ticker对象
void task1callback(int data) { // 编写回调函数
    // TODO
}
```

声明Lib_HAL_Ticker对象和任务函数后就可以使用 `attach`或 `once` 方法来运行任务，使用 `detach` 方法可以停止任务：

```c++
task1.attach(500, task1callback, 233); // 运行task1，每隔500毫秒反复调用task1callback()，传入参数为233
task2.once(4000, [] {task1.detach();}); // 运行一次task2，于4000毫秒后停止task1的任务
```

只是上面这样还无法正常工作，**需要在main函数的while(1)中添加下面代码**：

```c++
Lib_HAL_Ticker::handle(); // 所有处于就绪状态的任务将在这里运行，while(1)中最好不要进行其它工作
```

### API说明

- `bool attach(uint32_t milliseconds, callback_t callback)`

  `bool attach(uint32_t milliseconds, void (*callback)(T), T arg)`

  每隔milliseconds时间反复运行任务，callback为需要反复运行的函数，arg为不大于4字节的参数；

- `bool once(uint32_t milliseconds, callback_t callback)`

  `bool once(uint32_t milliseconds, void (*callback)(T), T arg)`

  隔milliseconds时间后运行一次任务，callback为需要反复运行的函数，arg为不大于4字节的参数；

- `void detach(void)`

  停止处于活动状态的任务；

- `bool active(void)`

  返回任务状态，如果任务处于活动状态则返回true；

- `static void handle(void)`

  运行处于就绪状态的任务；

### 其它说明

- 可以通过修改 `lib_hal_ticker.h` 中 `#define LIB_HAL_TICKER_MAX_SIZE 16` 来调整最大可声明lib_hal_ticker对象数量。
- lib_hal_ticker库重写了HAL库中定义的HAL_IncTick函数，见下面代码，其中涉及的内容请勿修改：

```c++
extern __IO uint32_t uwTick;
extern HAL_TickFreqTypeDef uwTickFreq;
void HAL_IncTick(void) { // HAL库Time回调函数，默认情况下每1ms会触发一次
	uwTick += uwTickFreq;
	Lib_HAL_Ticker::schedule();
}
```



## lib_hal_pilotlamp

该库是一个基于lib_hal_ticker的简单用于控制作为信号指示的LED的库，可以实现点亮、熄灭、闪烁等功能。

### 使用说明

该库中有Lib_HAL_PilotLampBasic和Lib_HAL_PilotLampAdvanced两个类，后者比前者多了个trigger()方法，下面以Lib_HAL_PilotLampAdvanced类做个简单演示：

```c++
Lib_HAL_PilotLampAdvanced lamp(LED_GPIO_Port, LED_Pin); // 声明一个对象，传入LED引脚编号
lamp.flicker(); // 使信号灯闪烁
```

只是上面这样还无法正常工作，因为它是基于lib_hal_ticker的，所以**需要在main函数的while(1)中添加下面代码**：

```c++
Lib_HAL_Ticker::handle(); // 所有处于就绪状态的任务将在这里运行，while(1)中最好不要进行其它工作
```

### API说明

- `Lib_HAL_PilotLampAdvanced(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool inversion = false, uint32_t oncems = 200, uint32_t offms = 150, uint32_t onms = 50, uint32_t reflashms = 3000)`

  构造函数，需要传入所绑定指示灯的端口和引脚编号，inversion为false表示高电平点亮指示灯、为true表示低电平点亮指示灯；

- `void off(void)`

  熄灭指示灯；

- `void on(void)`

  点亮指示灯；

- `void once(uint32_t ms)`

  `void once(void)`

  点亮指示灯oncems时间，然后熄灭；

- `void flicker(uint32_t offms, uint32_t onms)`

  `void flicker(void)`

  使指示灯进入闪烁状态，每次闪烁点亮onms时间、熄灭offms时间；

- `void trigger(void)`

  触发一次once()，同时开始计时，当计时到reflashms时间后转入flicker()状态；

### 其它说明

每个Lib_HAL_PilotLampBasic对象占用一个lib_hal_ticker空间；

每个Lib_HAL_PilotLampAdvanced对象占用两个lib_hal_ticker空间；



## lib_fakeheap

该库可以绑定一个全局静态数组，简单模拟堆的功能，用户可以从中申请空间供使用。

### 使用说明

Lib_FakeHeap初始化时绑定一个静态全局数组，使用 `get` 方法从对象中申请一定长度连续空间：

```c++
#define FAKEHEAP_BUFSIZE 256 // 定义缓存长度
static uint8_t fakeheapbuf[FAKEHEAP_BUFSIZE] = { 0 }; // 声明一个数组作为缓存
Lib_FakeHeap fakeheap(fakeheapbuf, FAKEHEAP_BUFSIZE); // 将数组绑定到Lib_FakeHeap对象
```

通过下面方式使用：

```c++
uint8_t *ptr = fakeheap.get(len);
```

### API说明

- `Lib_FakeHeap(uint8_t *buf, uint16_t size)`

  构造函数，传入数组地址和长度；

- `uint8_t* get(uint16_t size)`

  申请size长度连续空间；

### 其它说明

 `get` 方法根据用户空间使用情况循环的从数组中返回指针给用户，并不会检查之前申请的空间是否使用完毕。



## lib_hal_uart

该库处理uart通讯相关事务，tx和rx分开进行处理。

### Lib_HAL_UartTx 使用说明

该库用于控制UART数据发送，通过该库发送数据时每次将要发送数据的地址与长度放入一个队列，然后调用HAL库的HAL_UART_Transmit_DMA()函数依次发送数据。

**Lib_HAL_UartTx使用前需要启用UART发送的DMA传输功能；**

Lib_HAL_UartTx绑定UART对象，然后在HAL_UART_TxCpltCallback回调函数中添加相应处理（不同UART共用HAL_UART_TxCpltCallback函数，只需要新增uarttx.dmaTcHandle(huart);语句就行）；

使用 `write` 方法将要发送数据的地址和长度添加到队列，如果当前没有数据发送时会自动进行发送；

```c++
//UART_HandleTypeDef huart1; // 由stm32cubemx生成

Lib_HAL_UartTx uarttx(&huart1); // Lib_HAL_UartTx对象声明
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) { // 发送完成中断回调函数
	uarttx.dmaTcHandle(huart); // dmaTcHandle函数中会判断是否还有数据需要发送
}
```

通过下面方式使用：

```c++
uint8_t buf[size];
uarttx.write(buf, size); // 将buf通过串口发送
// 在发送未完成时buf被释放存在风险，推荐数组全局声明或者与Lib_FakeHeap结合使用
```

### Lib_HAL_UartTx API说明

- `Lib_HAL_UartTx(UART_HandleTypeDef *uart)`

  构造函数，绑定串口对象以使用；

- `bool write(uint8_t *data, uint16_t size)`

  消息入队，Lib_HAL_UartTx库会控制消息依次发送；

### Lib_HAL_UartTx 其它说明

Lib_HAL_UartTx发送队列的容量可以通过 `lib_hal_uart.h` 中 `#define LIB_HAL_UARTTX_MAX_SIZE 8` 参数调节。

### Lib_HAL_UartRx 使用说明

该库用于处理UART数据接收，使用DMA接收串口数据，每次触发串口接收空闲中断时对收到的数据进行处理。

**使用前需要启用UART中断和接收的DMA传输功能；**

Lib_HAL_UartRx初始化时绑定一个静态全局数组作为串口数据接收缓存，绑定一个在串口接收空闲中断时运行的函数；

```c++
//UART_HandleTypeDef huart1; // 由stm32cubemx生成
//DMA_HandleTypeDef hdma_usart1_rx; // 由stm32cubemx生成

void rxdataParse(uint16_t rxsize){
    // TODO
}

#define UARTRX_BUFSIZE 256 // 定义缓存长度
static uint8_t uartrxbuf[UARTRX_BUFSIZE] = { 0 }; // 串口接收缓存
Lib_HAL_UartRx uartrx(&huart1, &hdma_usart1_rx, uartrxbuf, UARTRX_BUFSIZE, rxdataParse); // Lib_HAL_UartRx对象声明
```

接着需要开启串口空闲中断，比如可以在main函数while(1)之前执行下面语句：

```c++
uartrx.listen(); // 启动串口接收监听
```

最后由于该库利用的是串口空闲中断，所以需要在串口中断添加对空闲中断的处理。以下内容需要添加在stm32fxxx_it.cpp中：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void USARTx_IRQHandler(void){ 
	extern Lib_HAL_UartRx uartrx; // 需要添加语句一：引用接收控制对象
	uartrx.uartIdleHandle(); // 需要添加语句二：串口接收空闲中断处理
    
    HAL_UART_IRQHandler(&huartx); // 该行由STM32CubeMX自动生成
}
```

通过上面三步该库的使用就完成了，每次串口接收空闲中断时会自动运行rxdataParse()，然后自动重新开启串口DMA接收。理论上到上面就可以了，但有的系列中重启接收有可能会失败，所以最好还需要在主循环中（main函数while(1)中）添加下面语句：

```c++
uartrx.reset(); // 理想状态下不需要这个，但实际测试中F0没问题、F4就必须加这个、其他系列未测试，保险起见全都加上吧
```

### Lib_HAL_UartRx API说明

- `Lib_HAL_UartRx(UART_HandleTypeDef *uart, DMA_HandleTypeDef *dma, uint8_t *buf, size_t bufsize, void (*dataParse)(uint16_t rxsize))`

  构造函数，绑定串口和DMA对象，绑定串口接收缓存和缓存长度，绑定串口接收空闲中断回调函数；

- `void listen(void)`

  开启串口接收空闲中断和串口DMA接收；

- `void reset(void)`

  如果重新启动接收失败运行该函数就会再次尝试重新启动接收；

- `void uartIdleHandle(void)`

  串口接收空闲中断处理；

### Lib_HAL_UartRx 其它说明

实际使用中一般还会结合通讯协议使用，可以参考lib_hal_modbus_rtu、lib_hal_naisu_protocol等协议。



## lib_hal_modbus_rtu

这是一个简单的用于Modbus-Rtu通讯协议的库。

### 协议描述

**Modbus-Rtu协议格式如下：**

<table>
   <tr>
      <td>名称</td>
      <td>地址域</td>
      <td>功能码</td>
      <td colspan="4">数据</td>
      <td>校验低字节</td>
      <td>校验高字节</td>
   </tr>
   <tr>
      <td>长度</td>
      <td>1byte</td>
      <td>1byte</td>
      <td colspan="4">n byte</td>
      <td>1byte</td>
      <td>1byte</td>
   </tr>
</table>

**地址域：**取值0~247，其中0为广播地址，1~247为从机地址；

**功能码：**功能码由Modbus协议规定，不同功能码对应不同的数据结构和功能操作；

**数据：**对于不同功能码的命令而言数据结构可能有较大不同。目前常用的寄存器操作相关指令中寄存器数据(16bit)通常以大端形式组织，例如数据0x1234，在通讯中先传输0x12、再传输0x34；

**校验：**该协议中采用Modbus模型的CRC16校验，目前常见的情况下校验结果通讯时以小端形式组织，例如数据0xABCD，在通讯中先传输0xCD、再传输0xAB；



**本库中目前对下面几个功能码的消息进行了检索：**

| 功能码 |    功能描述     |               注释               |
| :----: | :-------------: | :------------------------------: |
|  0x01  |     读线圈      |     线圈为bit形式，可读可写      |
|  0x02  |    读离散量     |      离散量为bit形式，只读       |
|  0x03  |  读保持寄存器   | 保持寄存器为2bytes形式，可读可写 |
|  0x04  |  读输入寄存器   |   输入寄存器为2bytes形式，只读   |
|  0x05  |   写单个线圈    |                                  |
|  0x06  |  写单个寄存器   |     这里的寄存器指保持寄存器     |
|  0x0F  |   写多个线圈    |                                  |
|  0x10  |  写多个寄存器   |     这里的寄存器指保持寄存器     |
|  0x14  |   读文件记录    |                                  |
|  0x15  |   写文件记录    |                                  |
|  0x16  |  屏蔽写寄存器   |                                  |
|  0x17  | 读/写多个寄存器 |                                  |
|  0x2B  |  读设备识别码   |                                  |

### 使用说明

该库含有 **Lib_HAL_ModbusRtu_Master** 和 **Lib_HAL_ModbusRtu_Slave** 两个类，前者用在主机中用于解析来自从机的消息，后者用在从机中用于解析来自主机的消息，两者使用上非常相似，这里仅以前者作为演示说明。该库使用时需要结合Lib_HAL_UartRx使用（**Lib_HAL_UartRx使用前需要启用UART中断和接收的DMA传输功能**）。首先进行下面声明：

```c++
//UART_HandleTypeDef huart1; // 由stm32cubemx生成
//DMA_HandleTypeDef hdma_usart1_rx; // 由stm32cubemx生成

static uint8_t uartrxbuf[LIB_HAL_MODBUS_RTU_RXBUF_SIZE] = { 0 }; // 串口接收缓存
Lib_HAL_ModbusRtu_Master modbus(uartrxbuf); // 协议对象
Lib_HAL_UartRx uartrx(&huart1, &hdma_usart1_rx, uartrxbuf, LIB_HAL_MODBUS_RTU_RXBUF_SIZE,
                      [](uint16_t rxsize) {
	modbus.parse(rxsize);
}); // Lib_HAL_UartRx对象声明

void command03(uint8_t addr, uint16_t msgsize){} // 指令回调函数
void command06(uint8_t addr, uint16_t msgsize){} // 指令回调函数
```

对于Lib_HAL_UartRx还需要开启串口空闲中断，比如可以在main函数while(1)之前执行下面语句：

```c++
modbus.addCbFn03(command03); // 绑定指令，功能码为03的消息时触发
modbus.addCbFn06(command06); // 绑定指令，功能码为06的消息时触发

uartrx.listen(); // 启动串口接收监听
```

最后对于Lib_HAL_UartRx还需要按以下方式在stm32fxxx_it.cpp中添加代码：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void USARTx_IRQHandler(void){ 
	extern Lib_HAL_UartRx uartrx; // 需要添加语句一：引用接收控制对象
	uartrx.uartIdleHandle(); // 需要添加语句二：串口接收空闲中断处理
    
    HAL_UART_IRQHandler(&huartx); // 该行由STM32CubeMX自动生成
}
```

理论上到上面就可以了，但有的系列中重启接收有可能会失败，所以最好还需要在主循环中（main函数while(1)中）添加下面语句：

```c++
uartrx.reset(); // 理想状态下不需要这个，但实际测试中F0没问题、F4就必须加这个、其他系列未测试，保险起见全都加上吧
```

上面三/四步就是该库的使用流程，这样每当接到符合协议的数据的时候就是触发对应的回调函数，比如上面代码中如果接到 `01 03 02 AB CD 06 E1` 、 `01 03 04 11 22 33 44 4B C6` 等时候就会触发command03()，收到 `01 06 00 00 AB CD 37 6F` 、 `01 06 00 07 12 34 35 7C` 等的时候会触发command06()；

### API说明

该库含有 **Lib_HAL_ModbusRtu_Master** 和 **Lib_HAL_ModbusRtu_Slave** 两个类，两者非常相似，这里仅以前者作为演示说明（后者多一个设置自身地址的方法）：

- `Lib_HAL_ModbusRtu_Master(uint8_t *buf, uint16_t bufsize = LIB_HAL_MODBUS_RTU_RXBUF_SIZE, bool crchighbytefirst = false)`

  构造函数，需要绑定串口接收缓存、缓存长度， `crchighbytefirst` 参数用于表示CRC校验高低字节顺序，默认 `false` 表示低字节在前；

- `void parse(uint16_t rxsize)`

  解析数据；

- `uint8_t read(uint16_t offset)`

  `bool read(uint8_t *dest, uint16_t offset, uint16_t size)`

  读取当前指令数据，只能在指令回调函数中使用；

  offset表示当前指令数据下标，dest为用于接收数据的数组地址，size表示要读取数据长度；

- `void addCbFnUnknown(void (*callback)(uint8_t addr, uint16_t msgsize))`

  添加接收到地址正确、功能码无法识别的消息的回调函数；

- `void addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback)`

  `...`

  `void addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback)`

  添加对应功能码的回调函数和异常响应回调函数， `addCbFn01` 用于添加功能码为0x01的消息回调函数， `addCbFn2B` 用于添加功能码为0x2B的消息回调函数。



## lib_hal_naisu_protocol

这是一个用于通讯的通讯协议库，可以和Lib_HAL_UartRx等结合使用。

### 协议格式

<table>
   <tr>
      <td rowspan="2">名称</td>
      <td>STX</td>
      <td>CMD</td>
      <td colspan="2">LEN</td>
      <td colspan="3">DATA</td>
      <td>BCC</td>
   </tr>
   <tr>
      <td>指令头</td>
      <td>指令类型</td>
      <td colspan="2">数据（DATA）长度</td>
      <td colspan="3">数据</td>
      <td>异或校验</td>
   </tr>
   <tr>
      <td>格式</td>
      <td></td>
      <td></td>
      <td>LEN-H</td>
      <td>LEN-L</td>
      <td>DATA[0]</td>
      <td>...</td>
      <td>DATA[LEN-1]</td>
      <td></td>
   </tr>
   <tr>
      <td>长度</td>
      <td>1byte</td>
      <td>1byte</td>
      <td>1byte</td>
      <td>1byte</td>
      <td>1byte</td>
      <td>...</td>
      <td>1byte</td>
      <td>1byte</td>
   </tr>
</table>


### 使用说明

该库用于通讯中使用，这里以和Lib_HAL_UartRx结合使用作为使用说明（**Lib_HAL_UartRx使用前需要启用UART中断和接收的DMA传输功能**）。首先进行下面声明：

```c++
//UART_HandleTypeDef huart1; // 由stm32cubemx生成
//DMA_HandleTypeDef hdma_usart1_rx; // 由stm32cubemx生成

#define UARTRX_BUFSIZE 256 // 定义缓存长度
static uint8_t uartrxbuf[UARTRX_BUFSIZE] = { 0 }; // 串口接收缓存
Lib_HAL_NaisuProtocol protocol(uartrxbuf, UARTRX_BUFSIZE); // 协议对象
Lib_HAL_UartRx uartrx(&huart1, &hdma_usart1_rx, uartrxbuf, UARTRX_BUFSIZE, 
                      [](uint16_t rxsize) {
	protocol.parse(rxsize);
}); // Lib_HAL_UartRx对象声明

void command1(uint16_t size){} // 指令回调函数
void command2(uint16_t size){} // 指令回调函数
```

对于Lib_HAL_UartRx还需要开启串口空闲中断，比如可以在main函数while(1)之前执行下面语句：

```c++
protocol.add(command1, 0xA0, 0xC1); // 绑定指令，stx=A0 cmd=C1时触发
protocol.add(command2, 0xA0, 0xC2); // 绑定指令，stx=A0 cmd=C2时触发

uartrx.listen(); // 启动串口接收监听
```

最后对于Lib_HAL_UartRx还需要按以下方式在stm32fxxx_it.cpp中添加代码：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void USARTx_IRQHandler(void){ 
	extern Lib_HAL_UartRx uartrx; // 需要添加语句一：引用接收控制对象
	uartrx.uartIdleHandle(); // 需要添加语句二：串口接收空闲中断处理
    
    HAL_UART_IRQHandler(&huartx); // 该行由STM32CubeMX自动生成
}
```

理论上到上面就可以了，但有的系列中重启接收有可能会失败，所以最好还需要在主循环中（main函数while(1)中）添加下面语句：

```c++
uartrx.reset(); // 理想状态下不需要这个，但实际测试中F0没问题、F4就必须加这个、其他系列未测试，保险起见全都加上吧
```

上面三/四步就是该库的使用流程，这样每当接到符合协议的数据的时候就是触发对应的回调函数，比如上面代码中如果接到 `A0 C1 00 00 61` 、 `A0 C1 00 02 22 33 72` 等时候就会触发command1()，收到 `A0 C2 00 00 62` 、 `A0 C2 00 01 DA B9` 等的时候会触发command2()；

### API说明

- `Lib_HAL_NaisuProtocol(uint8_t *buf, uint16_t bufsize)`

  构造函数，需要绑定串口接收缓存和缓存长度；

- `bool add(void (*callback)(uint16_t msgsize), uint8_t stx, uint8_t cmd)`

  添加指令，分别需要提供回调函数、指令头、指令类型；

- `void parse(uint16_t rxsize)`

  解析数据；

- `uint8_t read(uint16_t offset)`

  `bool read(uint8_t *dest, uint16_t offset, uint16_t size)`

  读取当前指令数据，只能在指令回调函数中使用；

  offset表示当前指令数据下标，比如按上面代码在command1()中protocol.read(0)返回0xA0、protocol.read(1)返回0xC1；

  dest为用于接收数据的数组地址，size表示要读取数据长度；

- `uint8_t calBcc(uint8_t *data, uint16_t size)`

  用于计算异或校验值；

### 其它说明

lib_hal_naisu_protocol.h中 `#define LIB_HAL_NAISU_PROTOCOL_MAX_SIZE 8` 参数用来控制最大可添加指令数量；



## lib_hal_flash

该库用了芯片FLASH上最后一个PAGE，模拟EEPROM使用，用来固化数据。

### 使用说明

这个库使用比较简单，并且已经内置了一个对象 `libhalflash` ，直接拿来用就可以：

```c++
uint8_t buf[size];

libhalflash.read(0); // 读取地址0上的一字节数据
libhalflash.read(buf, 10, size); // 从地址10开始读取size字节数据到buf

libhalflash.write(0xDA, 5); // 将数据0xDA写到地址5
libhalflash.write(buf, 25, size); // 从地址25开始写入长度为size的数据，数据源是buf数组
```

### API说明

- `uint8_t read(size_t addr)`

  `bool read(uint8_t *data, size_t startaddr, size_t size)`

  读取数据；

- `bool write(uint8_t data, size_t addr)`

  `bool write(uint8_t *data, size_t startaddr, size_t size)`

  写入数据；

- `void erase(void)`

  擦除所有数据；

### 其它说明

- 可以通过 `lib_hal_flash.h` 中的 `#define LIB_HAL_FLASH_USER_MAX_SIZE 100` 调节该库存储区域大小，必须小于 `FLASH_PAGE_SIZE` ，并且必须为偶数；
- 对flash的写入和擦除动作比较耗时，使用时请注意；
- flash的写入有寿命限制，stm32系列的微控制器最少可写入次数为1K，典型值为10K，设计时需要充分考虑；
- flash读写涉及到的部分代码不兼容stm32所有系列，有些情况下可能需要调整库代码；



## lib_hal_chip595

该库用于驱动595系列芯片，使用该库需将595的MR引脚接到高电平，OE引脚可由外部电路控制也可接到MCU。

### 使用说明

595控制比较简单，写数据的方式有点像SPI，这里做下使用说明：

```c++
Lib_HAL_Chip595 chip595(portSH, pinSH, portDS, pinDS, portST, pinST, portOE, pinOE);

chip595.enable(); // 使能595输出

uint8_t buf[2] = {0x22, 0x33};
chip595.write(buf, 2); // 向595总线写两个字节数据， buf[0]数据对应离MCU最近的595，buf[0]的bit0对应该595的Q0输出，bit7对应595的Q7输出；buf[1]数据对应离MCU次近的595；
```



### API说明

- `Lib_HAL_Chip595(GPIO_TypeDef *gpioSH, uint16_t pinSH, GPIO_TypeDef *gpioDS, uint16_t pinDS, GPIO_TypeDef *gpioST, uint16_t pinST, GPIO_TypeDef *gpioOE = nullptr, uint16_t pinOE = -1, uint32_t holdrate = 6)`

  构造函数，依次绑定对应控制管脚。holdrate为延时系数，595控制的时候对信号时序有一定要求，如果不能正常工作可以调节该参数试试；

- `void enable(void)`

  拉低OE，使能输出；

- `void disable(void)`

  拉高OE，失能输出（输出变高阻态）；

- `void write(uint8_t *data, int size)`

  向595总线写数据；



## lib_hal_tlv5630

该库用于驱动 tlv5630 / tlv5631 / tlv5632 芯片。

### 使用说明

tlv5630系列芯片操作需要用到SPI，使用时需要启用SPI功能，相关参数如下：

> SPI working as master
> hardware nss disable
> frame format Motorola
> data size 16bit, MSB first
> cpol = Low,  cpha = 2Edge
> SPI working on Polling mode

STM32CubeMX生成SPI和相关IO口初始化代码后就可以使用tlv5630了：

```
//SPI_HandleTypeDef hspi1; // 由stm32cubemx生成
Lib_HAL_Tlv5630 tlv5630(&hspi1, nssGPIO, nssPin, preGPIO, prePin, ldacGPIO, ldacPin); // 声明对象

tlv5630.init(1000) //设置所有通道输出
tlv5630.write(0, 2000); // 向通道A写数据
tlv5630.load(0); // 将通道的数据输出到端口上
```



### API说明

- `Lib_HAL_Tlv5630(SPI_HandleTypeDef *spi, GPIO_TypeDef *fsIOx, uint16_t fsPin, GPIO_TypeDef *preIOx, uint16_t prePin, GPIO_TypeDef *ldacIOx, uint16_t ldacPin, uint32_t holdrate = 2)`

  构造函数，传入SPI对象和其它用到的IO口；

- `bool init(uint16_t data = 0)`

  设置全部输出通道的初始值，取值见下文write()方法；

- `bool write(uint8_t addr, uint16_t data)`

  设置指定通道的值，地址取值如下：

  > out_a = 0,
  >  out_b = 1,
  >  out_c = 2,
  >  out_d = 3,
  >  out_e = 4,
  >  out_f = 5,
  >  out_g = 6,
  >  out_h = 7,
  >  out_a&b = 12,
  >  out_c&d = 13,
  >  out_e&f = 14,
  >  out_g&h = 15

  数据取值范围如下：

  >  TLV5630 DATA range is 0 to 4095
  >  TLV5631 DATA range is 0 to 4092
  >  TLV5632 DATA range is 0 to 4080

  数据与电压关系为 `Vout = 2REF * DATA / 4096` ；

  该操作不会立即将数据反应在DAC外部引脚上，只是缓存了数据，需要调用load()方法才会更新引脚上的电压；

- `void load(void)`

  将缓存的数据以电压形式更新到外部引脚上；

- `bool setPowerDown(bool enable)`

  设置电源模式 0 =  normal operation 、 1 = power down ；

- `bool setDout(bool enable)`

  设置数据输出 0 = disabled 、 1 = enabled ，使能时从DIN输入的数据将在16个周期后串行的从DOUT输出；

- `bool setReference(uint8_t ref)`

  设置参考电压 0或1 = 外部电压基准 、 2 = 内部1.024 V 、 3 = 内部2.048 V ；

- `bool setInputMode(uint8_t mode)`

  设置数据输入模式 0 = straight binary（普通形式） 、 1 = twos complement（二进制补码形式）；

- `bool setPowerDownXY(uint8_t addr, bool enable)`

  设置指定组（AB、CD、EF、GH）的电源模式 0 =  normal operation 、 1 = power down ；

  分组地址见上面 write() 方法；

- `bool setSpeedXY(uint8_t addr, uint8_t speed)`

  设置指定组（AB、CD、EF、GH）的速度模式 0 = slow mode 、 1 = fast mode ；

  分组地址见上面 write() 方法；

### 其它说明

- 使用该库时 TLV5630 working on µC Mode(pin-MODE High) ;
- 构造函数中的holdrate参数用来调节手动控制的操作后的保持时间，如果不能正常工作的话可以调节该参数试试；



## lib_hal_tlv5638

该库用于驱动tlv5638芯片。

### 使用说明

tlv5638芯片操作需要用到SPI，使用时需要启用SPI功能，相关参数如下：

> SPI working as master
> hardware nss disable
> frame format Motorola
> data size 16bit, MSB first
> cpol = High,  cpha = 1Edge
> SPI working on Polling mode

STM32CubeMX生成SPI和片选IO口初始化代码后tlv5638使用就比较简单了：

```c++
//SPI_HandleTypeDef hspi1; // 由stm32cubemx生成
Lib_HAL_Tlv5638 tlv5638(&hspi1, nssGPIO, nssPin); // 声明对象

tlv5638.setDAC(1000, 2000); // 设置A和B通道输出
```

### API说明

- `Lib_HAL_Tlv5638(SPI_HandleTypeDef *spi, GPIO_TypeDef *csIOx, uint16_t csPin, uint32_t holdrate = 2)`

  构造函数，需要传入SPI对象和片选信号引脚；

- `bool setSpeed(uint8_t speed)`

  设置速度模式 0 = slow mode 、 1 = fast mode ；

- `bool setPowerDown(bool enable)`

  设置电源模式 0 =  normal operation 、 1 = power down ；

- `bool setReference(uint8_t ref)`

  设置参考电压 0或3 = 外部电压基准 、 1 = 内部1.024 V 、 2 = 内部2.048 V ；

- `bool setDAC(uint16_t dataA, uint16_t dataB)`

  `bool setDACB(uint16_t data)`

  `bool setDACA(uint16_t data)`

  设置A和B通道的输出，取值为0~4095，输出电压为 `Vout = 2REF * DATA / 4096` ；

### 其他说明

- 在STM32CubeMX设置SPI时除上面参数外还需要通过设置时钟分频系数来调节波特率，根据tlv5638工作模式的不同可选择波特率最大从几百K到1M左右，如果不能正常工作的话可以降低波特率试试；

- 构造函数中的holdrate参数用来调节片选信号（CS或NSS）操作后的保持时间，如果不能正常工作的话可以调节该参数试试；



## lib_ll_uart

该库处理uart通讯相关事务，tx和rx分开进行处理。

### Lib_LL_UartTx使用说明

该库用于控制UART数据发送，通过该库发送数据时每次将要发送数据的地址与长度放入一个队列，然后调用HAL库的HAL_UART_Transmit_DMA()函数依次发送数据。

**Lib_LL_UartTx使用前需要启用UART发送的DMA传输功能；**

使用时首先声明对象：

```c++
Lib_LL_UartTx uarttx(USART1, DMA2, LL_DMA_STREAM_7); // Lib_LL_UartTx对象声明
```

然后在stm32fxxx_it.cpp中添加发送完成中断处理：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void DMA2_Stream7_IRQHandler(void)
{
	extern Lib_LL_UartTx uarttx; // 需要添加语句一：引用接收控制对象
	uarttx.dmaTcHandle(LL_DMA_IsActiveFlag_TC7, LL_DMA_ClearFlag_TC7); // 需要添加语句二：传输完成中断处理
}
```

通过下面方式使用：

```c++
uint8_t buf[size];
uarttx.write(buf, size); // 将buf通过串口发送
// 在发送未完成时buf被释放存在风险，推荐数组全局声明或者与Lib_FakeHeap结合使用
```

### Lib_LL_UartTx API说明

- `Lib_LL_UartTx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel)`

  构造函数，依次绑定对应uart、DMA、DMA流或通道；

- `bool write(uint8_t *data, uint16_t size)`

  消息入队，Lib_LL_UartTx库会控制消息依次发送；

- `void dmaTcHandle(uint32_t (*isactiveflag)(DMA_TypeDef *DMAx), void (*clearflag)(DMA_TypeDef *DMAx))`

  DMA传输完成中断处理；

### Lib_LL_UartTx 其它说明

Lib_LL_UartTx发送队列的容量可以通过 `lib_ll_uart.h` 中 `#define LIB_LL_UARTTX_MAX_SIZE 8` 参数调节。

### Lib_LL_UartRx使用说明

该库用于处理UART数据接收，使用DMA接收串口数据，每次触发串口接收空闲中断时对收到的数据进行处理。

**使用前需要启用UART中断和接收的DMA循环传输功能；**

Lib_LL_UartRx初始化时绑定一个静态全局数组作为串口数据接收缓存，绑定一个在串口接收空闲中断时运行的函数；

```c++
void rxdataParse(uint16_t rxrear){
    // TODO
}

static uint8_t uartrxbuf[UARTRX_BUFSIZE] = { 0 }; // 串口接收缓存

Lib_LL_UartRx uartrx(USART1, DMA2, LL_DMA_STREAM_2, uartrxbuf, UARTRX_BUFSIZE, rxdataParse); // Lib_LL_UartRx对象声明
```

接着需要开启串口空闲中断，比如可以在main函数while(1)之前执行下面语句：

```c++
uartrx.listen(); // 启动串口接收监听
```

最后由于该库利用的是串口空闲中断，所以需要在串口中断添加对空闲中断的处理。以下内容需要添加在stm32fxxx_it.cpp中：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void USARTx_IRQHandler(void){ 
	extern Lib_LL_UartRx uartrx; // 需要添加语句一：引用接收控制对象
	uartrx.uartIdleHandle(); // 需要添加语句二：串口接收空闲中断处理
}
```

通过上面三步该库的使用就完成了，每次串口接收空闲中断时会自动运行rxdataParse()。

### Lib_LL_UartRx API说明

- `Lib_LL_UartRx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel, uint8_t *buf, uint32_t bufsize, void (*dataParse)(uint16_t rxrear))`

  构造函数，依次绑定对应uart、DMA、DMA流或通道，绑定串口接收缓存和缓存长度，绑定串口接收空闲中断回调函数；

- `void listen(void)`

  开启串口接收空闲中断和串口DMA接收；

- `void uartIdleHandle(void)`

  串口接收空闲中断处理；

### Lib_LL_UartRx 其它说明

实际使用中一般还会结合通讯协议使用，可以参考lib_ll_modbus_rtu等协议。



## lib_ll_modbus_rtu

这是一个简单的用于Modbus-Rtu通讯协议的库。

### 协议描述

**Modbus-Rtu协议格式如下：**

<table>
   <tr>
      <td>名称</td>
      <td>地址域</td>
      <td>功能码</td>
      <td colspan="4">数据</td>
      <td>校验低字节</td>
      <td>校验高字节</td>
   </tr>
   <tr>
      <td>长度</td>
      <td>1byte</td>
      <td>1byte</td>
      <td colspan="4">n byte</td>
      <td>1byte</td>
      <td>1byte</td>
   </tr>
</table>


**地址域：**取值0~247，其中0为广播地址，1~247为从机地址；

**功能码：**功能码由Modbus协议规定，不同功能码对应不同的数据结构和功能操作；

**数据：**对于不同功能码的命令而言数据结构可能有较大不同。目前常用的寄存器操作相关指令中寄存器数据(16bit)通常以大端形式组织，例如数据0x1234，在通讯中先传输0x12、再传输0x34；

**校验：**该协议中采用Modbus模型的CRC16校验，目前常见的情况下校验结果通讯时以小端形式组织，例如数据0xABCD，在通讯中先传输0xCD、再传输0xAB；



**本库中目前对下面几个功能码的消息进行了检索：**

| 功能码 |    功能描述     |               注释               |
| :----: | :-------------: | :------------------------------: |
|  0x01  |     读线圈      |     线圈为bit形式，可读可写      |
|  0x02  |    读离散量     |      离散量为bit形式，只读       |
|  0x03  |  读保持寄存器   | 保持寄存器为2bytes形式，可读可写 |
|  0x04  |  读输入寄存器   |   输入寄存器为2bytes形式，只读   |
|  0x05  |   写单个线圈    |                                  |
|  0x06  |  写单个寄存器   |     这里的寄存器指保持寄存器     |
|  0x0F  |   写多个线圈    |                                  |
|  0x10  |  写多个寄存器   |     这里的寄存器指保持寄存器     |
|  0x14  |   读文件记录    |                                  |
|  0x15  |   写文件记录    |                                  |
|  0x16  |  屏蔽写寄存器   |                                  |
|  0x17  | 读/写多个寄存器 |                                  |
|  0x2B  |  读设备识别码   |                                  |

### 使用说明

该库含有 **Lib_LL_ModbusRtu_Master** 和 **Lib_LL_ModbusRtu_Slave** 两个类，前者用在主机中用于解析来自从机的消息，后者用在从机中用于解析来自主机的消息，两者使用上非常相似，这里仅以前者作为演示说明。该库使用时需要结合Lib_LL_UartRx使用（**Lib_LL_UartRx使用前需要启用UART中断和接收的DMA循环传输功能**）。首先进行下面声明：

```c++
static uint8_t uartrxbuf[LIB_LL_MODBUS_RTU_RXBUF_SIZE] = { 0 }; // 串口接收缓存
Lib_LL_ModbusRtu_Master modbus(uartrxbuf); // 协议对象
Lib_LL_UartRx uartrx(USART1, DMA2, LL_DMA_STREAM_2, uartrxbuf, LIB_LL_MODBUS_RTU_RXBUF_SIZE, [](uint16_t rxrear) {
	modbus.parse(rxrear);
}); // Lib_LL_UartRx对象声明

void command03(uint8_t addr, uint16_t msgsize){} // 指令回调函数
void command06(uint8_t addr, uint16_t msgsize){} // 指令回调函数
```

对于Lib_LL_UartRx还需要开启串口空闲中断，比如可以在main函数while(1)之前执行下面语句：

```c++
modbus.addCbFn03(command03); // 绑定指令，功能码为03的消息时触发
modbus.addCbFn06(command06); // 绑定指令，功能码为06的消息时触发

uartrx.listen(); // 启动串口接收监听
```

最后对于Lib_LL_UartRx还需要按以下方式在stm32fxxx_it.cpp中添加代码：

```c++
// 下面中断回调函数由STM32CubeMX自动生成
void USARTx_IRQHandler(void){ 
	extern Lib_LL_UartRx uartrx; // 需要添加语句一：引用接收控制对象
	uartrx.uartIdleHandle(); // 需要添加语句二：串口接收空闲中断处理
}
```

上面三步就是该库的使用流程，这样每当接到符合协议的数据的时候就是触发对应的回调函数，比如上面代码中如果接到 `01 03 02 AB CD 06 E1` 、 `01 03 04 11 22 33 44 4B C6` 等时候就会触发command03()，收到 `01 06 00 00 AB CD 37 6F` 、 `01 06 00 07 12 34 35 7C` 等的时候会触发command06()；

### API说明

该库含有 **Lib_LL_ModbusRtu_Master** 和 **Lib_LL_ModbusRtu_Slave** 两个类，两者非常相似，这里仅以前者作为演示说明（后者多一个设置自身地址的方法）：

- `Lib_LL_ModbusRtu_Master(uint8_t *buf, uint16_t bufsize = LIB_LL_MODBUS_RTU_RXBUF_SIZE, bool crchighbytefirst = false)`

  构造函数，需要绑定串口接收缓存、缓存长度， `crchighbytefirst` 参数用于表示CRC校验高低字节顺序，默认 `false` 表示低字节在前；

- `void parse(uint16_t rxrear)`

  解析数据；

- `uint8_t read(uint16_t offset)`

  `bool read(uint8_t *dest, uint16_t offset, uint16_t size)`

  读取当前指令数据，只能在指令回调函数中使用；

  offset表示当前指令数据下标，dest为用于接收数据的数组地址，size表示要读取数据长度；

- `void addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_LL_ModbusRtu_ERROR_Callback)`

  `...`

  `void addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_LL_ModbusRtu_ERROR_Callback)`

  添加对应功能码的回调函数和异常响应回调函数， `addCbFn01` 用于添加功能码为0x01的消息回调函数， `addCbFn2B` 用于添加功能码为0x2B的消息回调函数。



## lib_crc

该库用于计算CRC校验值，含有 **计算法** 和 **查表法** 两种方法，查表法收集了常用的CRC参数模型。下面是使用查表法计算的几个函数（详细的数据模型可以查看lib_crc.h文件）：

```c
unsigned char Lib_Crc8_Tab(unsigned char *data, unsigned int size);
unsigned char Lib_Crc8_Tab_ITU(unsigned char *data, unsigned int size);
unsigned char Lib_Crc8_Tab_ROHC(unsigned char *data, unsigned int size);
unsigned char Lib_Crc8_Tab_MAXIM(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_IBM(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_MAXIM(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_USB(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_MODBUS(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_CCITT(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_CCITTFALSE(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_X25(unsigned char *data, unsigned int size);
unsigned short int Lib_Crc16_Tab_XMODEM(unsigned char *data, unsigned int size);
unsigned int Lib_Crc32_Tab(unsigned char *data, unsigned int size);
unsigned int Lib_Crc32_Tab_MPEG2(unsigned char *data, unsigned int size);
```







———————————————————— End of Document ————————————————

