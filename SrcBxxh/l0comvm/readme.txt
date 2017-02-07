=================================================
== 将修改方式放在最前面，以提高效率
=================================================
**************接下来待完成主要内容**********************************************
CCL=> 最后剩下的任务是摄像头+IAP+节电，然后打开门狗就完美了
**************************************************************************

//= ZJL, 2017 Feb.6, CURRENT_SW_DELIVERY R03.115 =>CCL项目/BFSC
= 对SPSVIRGO/CCL中错误控制的处理改进
= 改进对CCL的错误控制处理过程
= 优化BFSC的错误控制处理过程
= 改进HUITP中有关MSG_TYPE的定义，完全将其跟老旧xml/zhb区分开来
= 清理VM的全局变量定义并归一化

//= ZJL, 2017 Feb.5, CURRENT_SW_DELIVERY R03.114 =>CCL项目
= 新定义#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_OTDR_01 0x0711
= 按照HCU NAME命名规则改进
= 增加软件升级方式的定义
= 将uboot物理区域增加到128BYTE
= 增加多启动区的配置
= 在移植HUITP到HCU的过程中，发现func_cloud_standard_xml_unpack处理函数中对结构指针进行初始化，应该停止！

//= ZJL, 2017 Feb.3, CURRENT_SW_DELIVERY R03.113 =>CCL项目
= 改造各个状态机入口函数，统一命名，降低重复概率
= 将硬件标识区修改为64字节，增加轮询启动标识以及公钥加密码
= 改进硬件标识区数据的获取方式
= 增加Huitp.h中有关设备的定义IE
= 增加huitp.h中支持图像传输的能力
= 完善后台格式的定义

//= ZJL, 2017 Feb.1, CURRENT_SW_DELIVERY R03.112 =>CCL项目
= 去激活CCL的SPI callback函数，不然会导致死机
= 依然没有发现寻卡成功的方法。修改SPI的初始化参数，CLKPolarity=SPI_POLARITY_LOW, CLKPhase = SPI_PHASE_1EDGE是缺省组合，其它配置组合下读取更困难
= RFID的读取过程都是官方驱动，问题依然可能在SPI参数配置这儿
= 准备买多张RFID卡，进行对照测试。
=>CCL项目卡顿在RFID寻卡不成功这儿，怀疑原因跟SPI配置不当有关。
>程序中需要清理的包括：SPI.H中管脚的定义，函数的统一化命名，以及调用ihu_vmmw_rfidmod_rc522_spi_read_id之后的打印信息
= 去掉README中对于上位机改进的记录条目

//= MYC, 2017 Jan.28-31, CURRENT_SW_DELIVERY R03.111 =>BFSC项目
= 开发BFSC的L3功能及第一条消息

//= ZJL/MYC, 2017 Jan.27, CURRENT_SW_DELIVERY R03.110 =>CCL项目/BFSC
= 将CCL任务数量缩减到极致11个，BFSC缩减到10个，正好相等于当前的任务数，从而将内存的占用优化到极致
= 将CCL的SPI速率改为256分频，速率83kbs，据说这个是影响寻卡不成功的因素之一，没反应
= 将IHU_VMWM_RFIDMOD_SCAN_RC522_MAX_TIME=40s，方便调测。
= 将BFSC的watchdog去掉，方便BFSC的调测

//= ZJL/MYC, 2017 Jan.26, CURRENT_SW_DELIVERY R03.109 =>CCL项目/BFSC
= 改善错误打印高级技巧IHU_ERROR_PRINT_TASK(taskid, arg...)
= 改善了CPUID的读出地址
= 将RFID的功能集成进来了，但每次reset外部管脚（复位NC522）会导致死机。RST管脚根本就没有连，而是随便连到一个空的管脚上了
= RC522的管脚，还未复用STM32CubeMX定义的管脚，待完善
= 有关CS置位的时序，是否该服从参考例子，待查
= 有关SPI的初始化，STM32CubeMX的生成代码是否足够完善，待查
= BFSC不能打开SPI Callback函数，不然会导致SPI重入死机
= 完善BFSC的初始化过程

//= ZJL, 2017 Jan.25, CURRENT_SW_DELIVERY R03.108 =>CCL项目
= 由于RFID是SPI接口，集成了SPI2 BSP和STM32CubeMX之SPI2接口
= 使用电流计，发现tickless开关打开关闭，电流并没有太大差异
= FreeRTOS中，PreSleepProcessing(), PostSleepProcessing(),暂时都是空的，原则上需要用户自行处理休眠前后。是不是这是休眠的核心？
= 目前去掉调试器的电流在20-30mA水平，距离理想的休眠电流1mA还相差很远，当然在实际板子上两个LED灯、频繁的触发机制和扫描机制，可能会降低功耗需求的

//= ZJL, 2017 Jan.24, CURRENT_SW_DELIVERY R03.107 =>CCL项目
= 完善并调测BLE，集成BLE功能到SPSVIRGO的AUTH消息生成函数中
= 结果发现，HC05的AT CMD模式根本就进不去，只有通过RESET KEY按住给其高电平并上电才能进入，这样在设备工作的情况下，只能进入透传模式
= 串口透传模式下，IHU将需要和手机APP直接进行通信。目前选用SPP蓝牙串口APP安装于手机上，选用了ASCI字符模式，让人工回复下位机的询问
= HC05/BLE在测试模式（Rimsys公司的HC05模块）下工作过程如此
  1. 云控锁触发IHU工作
  2. BLE模块上电进入透传模式
  3. 蓝牙缺省工作在38400bps/8N1/Master=1（主动连接手机外设）/CMODE=1主（可接受任何MAC手机）/IHU蓝牙设备名字固定为HC05
  4. IHU蓝牙模块自动连接手机并配对，手机需要提前打开蓝牙，收到配置通知后输入PIN码=1234，应该配对成功
  5. SPP蓝牙串口APP打开，启动连接，正常情况下应该能连上。如果是第二次巡视该站点，3/4两步将省略
  6. IHU主控程序在发送鉴权信息之前，先循环5秒在BLE串口发送MAC/MIMA=?请求，手机反馈MAC/MMIMA=12:34:56:78:90:AB\r\n做为回复，
     格式不一样将会使得IHU接收不成功头。注意：MAC/MIMA=为帧头，\r\n为帧尾
  7. IHU接收到合法的MAC地址以后，将会停止发送"MAC/MIMA=?"，并将MAC地址送到后台鉴权，同时在业务流程中去激活BLE模块
= 经过进一步研究，广州汇承官方正式贴片的HC05，用于批量贴板，将会有KEY拉高拉低的GPIO，可以用于我们控制模块进入AT命令模式还是透传模式
  如果采用这种方式，就没有这个APP的问题了   
= 基于HC05 BLE的通信速率，将其修改为38400Kbps
= 改进了 AUTH消息的结构，AUTH消息的测试数据和规范文档，一并更新
= 为了生成测试消息数据，需要将打印消息区换成900，完成后需要再改回来，因为内存不够，前面已经详尽说明过了

//= ZJL, 2017 Jan.23, CURRENT_SW_DELIVERY R03.106 =>CCL项目
= Puhui增加CAN L2FRAME的编解码功能
= 继续完善AT CMD的TCP/UDP工作机制
= 完全依靠CTRL+Z解决发送终止的问题，也使用AT+CIPSEND=%d解决了发送成功以后退出来的问题，两种方式均成功验证成功
= 完善了TCP和UDP的API函数，UDP只能做发送，不能做接收，比较搞，不知道为什么LJL那个所谓高手用UDP实现了程序下载的

//= ZJL, 2017 Jan.22, CURRENT_SW_DELIVERY R03.105 =>CCL项目/BFSC
= 继续全局VM任务创建的统一化，使用FuncHandler，消去分项目初始化创建任务的过程
= 改善VMFO的创建任务机制，统一称为循环和利用FuncHandler，不再分项目进行，从而大大简化了分项目处理的复杂度
= 改善VMFO中Heart-beat的接收处理
= 去掉各个模块的pnp全局配置，统一由VMLAYER.C中初始化进行，初始化不再判定sysconfig.h中的pnp配置
= 将任务数量简化到12个，贴近真实的任务数量（11个），从而大大优化内存的占用情形
  > CCL内存89KB，BFSC内存65KB，非常完美！

//= ZJL, 2017 Jan.22, CURRENT_SW_DELIVERY R03.104 =>CCL项目/BFSC/EMC68X
= 对CCL/BFSC的ErrorPrint做进一步的优化改造
= 将GPRS跟后台发送数据的时间从4秒改为6秒，以增加发送数据成功的概率
= 优化extern FsmState_t的引用定义，降低对于分项目定义的复杂度和风险
= 建立全局任务配置输入表单，简化任务创建的过程，特别是在程序部分分项目执行的过程
= TaskName全局改造，包括CCL/BFSC/EMC68X项目在内

//= ZJL, 2017 Jan.22, CURRENT_SW_DELIVERY R03.103 =>CCL项目/BFSC
= 改进#error定义，确保各个项目在编译时定义的完整性，将#error编译项用到极致！
= CCL在发送给后台消息以后，等待回收消息时的状态机的保护：确保回复的消息是目标消息，否则状态机会出错，导致状态乱序
= xml_unpack函数带回了这个这种期望消息之后，状态机再也没有出现无序错误了。毕竟这是涉及到后台远程的通信机制，
  这种方式将确保同步工作的L2FRAME模块跟上层的期望值一模一样，让整个状态机的工作方式更加稳定可靠。
= 系统硬件标识区的统一处理：完善了CCL/BFSC两个项目的统一处理，包括硬件唯一标识、升级标识等内容
= 增加了批量生产的标识，以此决定是读取硬件固定区，抑或编译区域的内容，做到调测和批量生产包的灵活配置性
= 增加IHU_HARDWARE_PRODUCT_CAT_TYPE的检查机制
= 启动IHU_LEDPISCES_GALOWAG_FUNC_ACTIVE，以便LED提供闪烁功能，并确保功能稳定性
= CCL状态机出错时的复位保护，确保回复到SLEEP缺省状态，而不是直接返回错误完事
= 同时改造了BFSC L3状态机，增加了保护机制确保回复到SCAN业务状态

//= ZJL, 2017 Jan.22, CURRENT_SW_DELIVERY R03.102 =>CCL项目
= 在第二次解码成功后，又发现hardfault的错误导致死机，跟之前打印区覆盖MsgQue的情形很类似
= 先将打印区限定在300BYTE，实验一下，是否还存在这种现象：死机现象果然消失了
= 未来这将成为一种标准调试方法：一旦HardFault死机，先要检查打印区是否覆盖了其它变量导致的情形！！！
= 进而推导：是否是任务堆栈区太小导致的覆盖问题。
= 最后，看VM-TASK启动是否正常，不然就是TOTAL_HEAP_SIZE和MINIMAL_STACK_SIZE设置的太小。这些参数之间都是有关联的，调整其中的一个可能会影响其它参数。
= 再实验：将任务堆栈IHU_TASK_STACK_SIZE从600放大到1000*sizeof(WORD)，打印区从300放大到Max_Que_Size+300 = 984，看实验结果：
  > 造成的第一个问题是，系统无法启动，因为整个VM_TASK的RAM区域不够，先调整FreeRTOS TOTAL_HEAP_SIZE系统堆栈，放大到0x10000可启动
  > TOTAL_HEAP_SIZE调整到0x10000以后，系统编译出的RAM=120532，非常接近于128KB，已经没法继续放大了
  > 终于可以启动所有任务成功，不然只能消减不重要的任务，从而达到减少内存占用的目的。
  > 这也说明，TASK_STACK_SIZE是非常耗内存的，因为这个参数是对所有的任务都起作用，特别是在新启动任务比较多的情况下，这个参数的放大是非常危险的。
  > 系统性的解决之道，要么将任务划分的比较均衡，特别是内存的占用方面划分的比较均衡，从而避免由于内存占用不同导致的内存占用浪费
  > 另外一个方式就是为每一个任务的内存占用单独设定常量，这个需要通过测试不断优化调整，在未来内存特别紧张的时候可以使用
  > 还有一个办法，就是减少任务的数量，将不太常常使用的任务归并到一起：比如ADC+LED共享同一个任务。这个做法好处是内存优化，但坏处是模块复用性降低，单个模块
    复杂性变得不可控制了
  > 没有最好的办法，我们需要针对应用的具体情形，采用不同的方式进行优化微调，但复用性、简单些、可扩展性、维护性应该放在首位，不然过于复杂的优化机制，将
    会消耗过多的调测与开发时间，最终得不偿失
  > 最终经过试验，确定了这种方式是可以工作的，不死机，是稳定可靠的，问题的根源得到解决！！！
  > 重新将打印区调整到300最大，将任务堆栈调回到400*sizeof(WORD)，TOTAL_HEAP_SIZE=0xAC00
= 再试验任务堆栈大小与打印区的关系：将VM系统总堆栈TOTAL_HEAP_SIZE = 0xAC00
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 1000/984可以工作（TOTAL_HEAP_SIZE = 0x10000 ==> 编译之后的RAM=120KB，非常接近于128KB的内存上限） 
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 600/300可以工作
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 600/984死机
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 400/300死机
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 500/300死机
  > IHU_TASK_STACK_SIZE/IHU_PRINT_FILE_LINE_SIZE = 600/300可以工作，且恢复正常状态，可以见得，这是最优组合了，打印区太小，适应性也不够的。
  > 在最优组合条件下，编译(O0/O1/O3)出的RAM SIZE = 98KB/98KB/98KB，RAM基本上没啥区别，FLASH占用的代码空间有较大的差异95KB/82KB/76KB
  > 缺省使用O1即可

//= ZJL, 2017 Jan.21, CURRENT_SW_DELIVERY R03.101 =>CCL项目
= 改善变长到定长，简化消息编码和解码
= 打印字符串系统定义长度改到IHU_QUEUE_MAX_SIZE + 300的超长长度，方便数据生成函数的执行func_cloud_standard_xml_generate_message_test_data()
= 去掉编解码中对变长消息的处理部分，处理过程得以保留，以便为未来可能存在的变长消息处理留下案例和抄袭方法
= 完善好接收消息的大小端处理
= 结构体的sizeof()取长度，并不是实际长度，而是内存占用的长度，在编码并发送出去时是不对的。。。
= 通过#progma pack(1)将消息结构顺利对齐到1字节，从而解决了问题

//= ZJL, 2017 Jan.21, CURRENT_SW_DELIVERY R03.100 =>CCL项目
= 改进本设备的编号，不做成固定定义方式，而是为IAP升级后的程序从固定FLASH区域读取打好基础
= 修正xml_pack()函数中访问输入数据的bug
= 遇到编码的大小端问题，ARM-Cortex3缺省是小端(Small Endian)，编码只能将其定义为高低字节分开搞了
= 编码和可变函数的解码初始化调整MsgLen部分搞完，解码的主要部分还未搞
= 大小端的宏定义将成为变换的利器
= 测试数据重新生成
= 测试xml_unpack()函数，完善的生成测试数据
= xml_unpack()中存在接收结构申请为指针的情况，结果系统没有为它申请内存，导致覆盖系统区而死机
= 改进控制命令字40-4C，新增4D/4E用于AUTH/STATE

//= ZJL, 2017 Jan.20, CURRENT_SW_DELIVERY R03.99 =>CCL项目
= 回归STMFLASH, CPUID, Camera BSP
= 定义#define IHU_MSG_BODY_L2FRAME_MAX_LEN MAX_IHU_MSG_BODY_LENGTH - 4也需要跟4对其，不然消息缓冲区可能会长度不够导致出错
= GPRSMOD目前还是采用半双工工作方式
  >> 突然发现，从GPRSMOD接收到的数据，还是在SPSVIRGO之中，并没有超越这个模块本身，这是一种半双工的工作方式，所以在里面再给自己发送L2FRAME_RCV消息，就是自己
  给自己发送消息，这样，对于MessageQue的数量，是有更高的要求的，如果MsgQue=2，可能会出现队列不够，要么导致队列覆盖到其它数据区，要么发送失败。
  >> 经过验证，需要将MessageQue=3, 整个VM缓冲区设置为0xBC00以后，才能自己给自己发送消息成功，这种实现方式得不偿失，不仅占用内存大增，而且互锁也变得更为复杂，
  将直接采用调用的方式进行工作，由发送消息来完成解码和后续处理。这样L2FRAME消息的来源将保留给全双工工作方式。
  >> 这种工作方式，也发现一个问题，跑两轮以后就死机。
= 恢复MessageQue=2/0xAC00缓冲区大小。
= 调整好SPSVIRGO接收消息的同步处理机制
= 优化HUITP xml_unpack()中的解码过程，将低效率的循环改为strncpy()

//= ZJL, 2017 Jan.19, CURRENT_SW_DELIVERY R03.98 =>CCL项目
= 验证xml_pack()函数全部打开后，是否完全正常， 单个验证GPRS发送函数， 接收串口返回必然直接回去，结果很稳定
= 准备将RTC的实时时间增加到打印信息中去，结果发现打印不正常，应该是硬件重入的问题。以后需要将RTC定时存入到系统全局变量中，这样任何模块就可以随时访问了，
  而不用各个模块独立再去访问RTC了。
= 打开所有发送消息中的http_data_post，跟xml_pack一起进行验证，以确定稳定性。
= 完善xml_pack的长度域，完善不同消息体的长度控制，从而适应所有的发送消息结构
= 调整了锁触发/震动触发的频率，由100%调整为1/4，方便测试
= 优化定时器长度，心跳和看门狗时长调整到位，唯有长周期汇报（8小时）暂时保持十分钟，以便充分测试
= 未打开xml_unpack()函数，先跑稳定性测试，确定稳定后，再增加cpuid/stmram/摄像头，以确定稳定性。最后再增加tcp/udp等数据连接方式，看门狗打开，BLE/RFID/摄像头研究等内容。

//= ZJL, 2017 Jan.19, CURRENT_SW_DELIVERY R03.97 =>CCL项目
= 先将LEDGALOWAG机制关闭
= 在SPSVIRGO中将xml_pack/unpack关闭
= 去掉摄像头BSP
= MessageQueue修改2个：立马出现Hardfault问题，可能是Que不够
= 继续消减BSP cpuid/STMRAM
= 消减Heart-beat机制的复杂机制，只让L3 TASK反馈，其它的都不考虑
= 以上消减均为必须的瘦身计划，所以将长久的放在系统中
= TASK_STACK/MSG_BODY/QueNbr = 400/566/2，立即死机
= TASK_STACK/MSG_BODY/QueNbr = 400/366/2，没有出现HardFault的情形，说明此时整个系统跑起来还是很稳定的。但零星也会出现无效消息的产生，不知道为什么产生的。
= TASK_STACK/MSG_BODY/QueNbr = 400/466/2，跑了2个小时，完美表现，零星的无效消息没有出现。
= TASK_STACK/MSG_BODY/QueNbr = 500/566/2，貌似比较稳定，也有零星的无效消息出现，不知道是不是暂态现象，跟连线不稳定有关？
= 无效消息之前调查出来的原因，主要是消息内容地址偏移N个字节，导致错误，原因未找到：消息头和消息体必须4BYTE对其，所以需要预留充足的字节空间
= 无效消息的另外一个来源是多个消息同时到达，造成消息队列满的情况发生。这在通信链路SPSVIRGO特别有此情况。
= 无效消息的还有一个来源是：如果SPSVIRGO正在处理任务，并且没有处理完成，再次过程中，其它任务给它发送消息，是否会造成消息格式出错？
= 无效消息发现内容体居然就是“MSGTRC”内容。。。指针赋值错误？是打印缓冲区太长，超过系统堆栈，从而占用了队列缓冲区了？
= 经过分析容易发现，MessageQueue和PrintBuffer基本上共享了地址空间，都是用的系统堆栈区。如果打印信息太长，而且没有及时清掉，或者在消息发送和接收之间又有打印
信息正好执行，就会发生打印信息被当做了消息内容体了。目前将系统堆栈放大到600，而且将打印区域固定到1个，而非之前的10个数组，暂时没有再重现该问题。
= 查找无效消息的方法：
  一旦出现消息接收的问题，可以在FsmProcessingLaunch的接收消息地方设置过滤机制，试图找到每一个无效消息的特性，比如是固定的任务号？是固定的来源？
  然后再通过发送消息的地方设置钩子，从而精确锁定无效问题消息的来源。
= 最终，TASK_STACK/MSG_BODY/QueNbr = 500/560/2, 消息总长+24（消息头按照4字节对其），打印缓冲区从10个改为1个。
= 继续为CCL系统减负，将L2FRAME任务中的"例行定时器"停止，不再启动，因为不用HEART-BEAT，所以也没啥必要启动他们。这样可以降低多条消息同时到达一个任务模块的概率。
  同时也降低整机功耗。

//= ZJL, 2017 Jan.17, CURRENT_SW_DELIVERY R03.96 =>CCL项目
= 继续试验AT CMD的POST方式，暂时没彻底解决。
= 通过串口监视命令，解决了HTTP数据发送的方式问题：就是发送给GPRS模块而已，适当控制好延时和发送的时机
= 完善发送过程中指令的顺序、多次发送的差错控制、时间延迟长度控制、出错打印信息的控制
= 抑制AT命令的回显
= HTTP基本上算好了，但连续跑，还会出现无效消息的产生、GPRSMOD模块在快速压力下会出现不服务的情形（实际情况下不可能需要如此快速的关停GPRSMOD）、以及
  程序出现Hardfault陷入的问题
= 放开所有的发送消息了，结果发现STATE REPORT消息的长度为122BYTE，非常长，总长度=122*2+300 = 544，所以将最长消息长度改为560了。
= 修改了最长消息长度以后，又发现内存不够，又只能消减MessageQue的数量，从4个改为3个，还得研究为什么MessageQue变短以后，会出现问题
= 跑3-5轮消息以后，就进入了Hardfault状态，需要改善稳定性

//= ZJL/MYC, 2017 Jan.16, CURRENT_SW_DELIVERY R03.95 =>CCL项目/BFSC
= 修改BFSC的头文件，包含了CAN的消息结构
= 将Msg_Que改为4个，最大编译之后的内存占用115KB，虽然不太美妙，但至少消灭了CLOSE_REPORT_CFM收到以后的VMFO接收差错问题
= AT命令集的正常时间改为4S钟非常必要，很多情况下，可以立即反馈的，但有些命令比较慢，改为4S以后，对于大多数命令比较有效。当然还有些命令，比这个更长的。
= 努力验证POST的缓冲区机制，按照官方介绍，应该是在DOWNLOAD与OK之间的缝隙直接发送，但采用AT COMMMAND TESTER V28官方工具的脚本Script Command方式，一直出现
400 Bad Request错误（如果将发送数据放在USERDATA时），或者出现200（OK）但数据长度只有42BYTE（将数据单独发送），而且发送框log见不到发送记录，不清楚是否真的发送
成功。另外，HTTPREAD也是成功的。
= 如果采用程序控制，也总是出现HardFault崩溃，说明连续如此发送必有硬伤。
= 还采用了Multi-part/Content-Type的格式方式，采用boundary进行控制，也没成功。缓冲区依然没找到。
= 使用官方工具的POST方式，则非常成功，唯一的差异是官方工具自带缓冲区。
= 后面再试试，如果实在搞不定，则需要尝试TCP方式了。
= 对于重复发送以后，第二次发送错误的问题，发送过程中采用判定或者忽略第二次错误的方式，得到了比较的解决，这种方式可以用于正式程序部分，比较出现错误退回
  的触发点很多，有些敏感的点在连接态下出错是正常现象，只要关键的发送是满足RETURN即可，也算是一种正常的处理方式。

//= ZJL, 2017 Jan.15, CURRENT_SW_DELIVERY R03.94 =>CCL项目
= 由于所有的L2FRAME消息必须装载到正式的消息结构中进行传送，其数据域长度不得超过MAX_IHU_MSG_BODY_LENGTH-2，全部更新，包括CCL和BFSC
= 由于这个因素，所有的BSP(UART/I2C/SPI/CAN)涉及到接收发送数据缓冲区的，都必须遵循最长长度-2
= 由于编译器对于奇数地址对其的问题，统一使用常量对L2FRAME的消息长度进行控制：IHU_MSG_BODY_L2FRAME_MAX_LEN = MAX_IHU_MSG_BODY_LENGTH-3
= 修改了BSP以及所有的消息定义(BFSC+CCL)
= 完善HTTP的控制方式

//= ZJL, 2017 Jan.14, CURRENT_SW_DELIVERY R03.93 =>CCL项目
= 继续研究AT CMD的HTTP工作模式
= 将消息长度改为455了，不然xml_pack不够长，必须大于400以上，但之前设置为505长度以后，message queue会出现莫名其妙的问题，将队列改为4个以后，内存又分紧张了。
= 在MSG_LEN=455长度下，CCL的状态汇报消息可能会有问题，等调测到那儿再看吧
= 调测的过程中发现，sizeof(structure)取得的长度，是以消息中字段的最大宽度对其的，比如内有UINT32，则UINT8也归为U32/4个字节，这也可能造成sizeof()长度膨胀的一个原因
  待研究如何弱化该问题。
= 将HTTP的设置过程全部打开分解，这样可以逐个命令进行分别控制，防止错误延续，本来也没有意义的  
= 多次连续操作，会对GPRS/HTTP的状态产生影响。改进的方式时，先来一轮HTTP/GPRS连接的断链操作。
= 网上经验号称，必须先操作AT+CSTT/设置APN，AT+CIICR/激活移动场景，AT+CIFSR/获得本地IP，有了这三部分的增加，才能避免HTTP初始化中SAPBR1,1设置错误的问题
  但实际上，暂时没有试验成功。

//= ZJL, 2017 Jan.13, CURRENT_SW_DELIVERY R03.92 =>CCL项目
= 调测LED闪灯任务：PC2上的207VC调测LED输出管脚，之前并没有在STM32CubeMX中拉出，导致没有初始化。拉出后，采用GALOWAG工作机制，非常正常。
= 修正xml_pack函数中导致hardfault的硬逻辑错误，地址访问出错
= HTTP的AT CMD走到最后一步发送数据，出错，导致再次出现HardFault，需要再研究SIM800A/HTTP的数据发送过程

//= ZJL, 2017 Jan.12, CURRENT_SW_DELIVERY R03.91 =>CCL项目
= 将系统堆栈改为0x8C00，试图简化RAM空间的耗用，结果空间不够
= 将系统堆栈改为0x9C00，不然空间不够，所有的任务不能一起创建出来
= 将消息队列改为3个，解决HAL_TIM_IRQHandler问题。原因不太明确，但应该跟系统堆栈不够相关
= 新出现了MSG_ID_SPS_CCL_CLOSE_REPORT_CFM发送之后必然出现无效消息的问题
= 将#define MAX_QUEUE_NUM_IN_ONE_TASK 4，解决了上面消息发送总出现错位比特的问题。带来的问题是，系统区改为了0xAC00，总内存占用115KB了
= 又将HUITP中有关总消息的长度由505改为405，队列数量由4改为3，貌似情况还算稳定，跑一晚上试试。

//= ZJL, 2017 Jan.11, CURRENT_SW_DELIVERY R03.90 =>CCL项目
= CCL中ADCLIBRA模块的sysconfig.h中打开标志位没开，造成message queue发送陷入系统错误，修正
= 由于iwdg造成系统不断重启，暂时去掉，未来再加入。
= FreeRTOS配置中的Minimal_STACK_SIZE从128扩大为256，不然会出现HardFault_Handler陷入的崩溃问题
= 将TIMER中的字节调整一下，去掉UINT32的定义
= xml_pack将导致hardfault，先注释掉，再调测。
= TIM调整到TIM6/7，时钟调整到位，跟BFSC保持一致
= 系统堆栈调整到0x9C00
= 修改sysconfig.h中的定时器，以期发现问题的相关性
= 将IHU_CCL_TIMER_DURATION_DIDO_TRIGGER_PERIOD_SCAN=500推向远处，系统死机的时间的确不断加长，这说明它跟BSP无关，的确是由DIDO触发引起的TIM异常
= 异常的函数是HAL_TIM_IRQHandler，这个中断不断进入导致死循环
= 陷入__weak void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim)以后，出不来，导致死机了
= STM32CubeMXz中去掉TIM试试，同时打开了所有的debug/trace信息

//= ZJL, 2017 Jan.10, CURRENT_SW_DELIVERY R03.89 =>CCL项目
= 尝试将DCMI的读取以及摄像头捕获功能集成进来，当做BSP方式。来源：STM32Cube_FW_F2_V1.4.0，官网和云上
= 由于这一部分涉及到大量的非确定性问题，待完善：CMOS摄像头需要连接到1.5米以远，需要选择驱动芯片，连线如何，摄像头的配置采用I2C或者其他方式进行配置？
= IAP存在多种烧录方式，详细见http://blog.chinaunix.net/uid-20788517-id-263475.html

//= ZJL, 2017 Jan.9, CURRENT_SW_DELIVERY R03.88 =>CCL项目
= 完成I2C主从模式下的BSP功能，采用固定长度的接收，发送随意长度。未来根据需求，可以自定义帧长度。
= I2C也支持基于TIMEOUT的接收，用于类似于AT CMD的收发半双工

//= ZJL, 2017 Jan.8, CURRENT_SW_DELIVERY R03.87 =>CCL项目
= 增加GALOWAG工作机制，解决输出方波和LED灯光闪烁的问题
= 继续完善对于LOCK三种工作模式的输出控制
= 将DIDO所有工作的传感器跟BSP均连接起来了
= 找回来ADC模块，因为电池BAT测量需要ADC通道
= 增加ADC对电池的读取

//= ZJL, 2017 Jan.7, CURRENT_SW_DELIVERY R03.86 =>CCL项目
= 集成BLE蓝牙操纵模块的完整功能，这个本来是用在5204型号之中

//= ZJL, 2017 Jan.6, CURRENT_SW_DELIVERY R03.85 =>CCL项目
= 精简CCL项目的硬件配置
= 去掉无用的BSP和任务模块
= 极大的优化了FLASH和RAM占用，预期进一步优化了功耗。
= 将DIDO对锁具的操作，跟底层BSP操作API连起来

//= ZJL, 2017 Jan.6, CURRENT_SW_DELIVERY R03.84 =>CCL项目
= 建立设备信息StrStm32F2EquidStorage_t读取的API
= 完善HW TYPE PRODUCT CATELOG信息域，在SYSCONFIG.H进行了单独的明确的定义，以便未来可以跟StrStm32F2EquidStorage_t进行交叉验证
= 集成DHT11温湿度传感器
= 集成PMU6050加速度陀螺仪传感器
= 找到了一种BSP优化#ifdef的方法：如果某种设备没有被MAIN定义，则需要在BSP中重新定义下，然后就可以共享所有的BSP函数了
= 如果某个设备完全在该项目都没有定义，则将该设备的BSP从该项目中清除即可
= 优化完成了所有的BSP兼容性定义
= 去掉了所有项目的ETH模块
= 为BFSC项目，去掉了DIDO、SPS、PWM模块

//= ZJL, 2017 Jan.5, CURRENT_SW_DELIVERY R03.83 =>CCL项目
= 时间time()调整完成，跟当前RTC时间连在一起了
= 设备型号，HUITP协议需要调整，且调整完毕。
= 垃圾集成STM32_FLASH内部FLASH的访问函数，以及IAP升级程序。
= IAP程序将成为独立的uboot，暂时不再继续搞了。STMFLASH访问程序暂时留着，因为未来还将需要方位FLASH固定地址的数据段。
= 当前先定义32BYTE，用于存储设备标签（20B），设备型号（2B），硬件标识（2B），软件大版本（2B），软件小版本（2B），是否升级以及升级类型（1B）、保留（3B）。
= IAP做成的uboot，等待独立做成项目任务
= 增加了PrjUbootStm32F2基于UART/ETH的官方IAP程序例子，放在IHU目录下了。在项目清单中，只放了UART的IAP例子，供研究参考之用。
= 等待外包IAP任务完成后，再行集成

//= ZJL, 2017 Jan.4, CURRENT_SW_DELIVERY R03.82 =>BFSC项目
= 设置BFSC的原理图，并大范围调整管脚
= 生成的自动代码，双编译通过
= 基于207VC生成的管脚配置图，已经无法下行IMPORT到205RG了，所以准备放弃205RG项目了
= 将LED的BFSC独特业务部分做好

//= ZJL, 2017 Jan.3, CURRENT_SW_DELIVERY R03.81 =>CCL项目
= 增加ADC1采样CPU内部温度的工作逻辑，目前按照官方例子，ADC1采样CPU温度的参数配置还存在一定错误，不知是否能正常工作，待调查调测。
= 完善ADC的工作函数，清理完毕ADC BSP的整个逻辑
= 完善DIDO的所有控制逻辑
= 统一格式化BLEMOD的函数命名
= 统一格式化RFIDMOD的函数命名

//= ZJL, 2017 Jan.2, CURRENT_SW_DELIVERY R03.80 =>CCL项目
= 完善CRC过程，包括CRC硬件逻辑在STM32CubeMX中没有选用的情形下，需要通过sysconfig.h中的开关关闭，不然会出现编译问题
= 完善RCC的配置，完全依靠STM32CubeMX
= 创建RTC功能，包括日历功能
= 发现日历有问题，双字节汉子无法编译通过，待研究完善
= 日期的设定，目前依赖于STM32CubeMX的设定，人工设定暂时注释掉了（官方例子没这个问题），未来希望改进为同网络自动同步

//= ZJL, 2017 Jan.2, CURRENT_SW_DELIVERY R03.79 =>CCL项目
= 将STM32CubeMX配置上了，IWdog/BEEP-LED/CRC硬件选上了
= 删去FTP的异类定义
= 删去CCL@205RG项目，207VC将成为唯一的工作环境了
= WATCH_DOG的使用方法：
  如果某一个项目工程想要打开WATCH_DOG，就需要将STM32CubeMX中的iwatch_dog打开使能，且将分频器设置为256/4095，最长32s的定时长度，然后将L1VMFO
  的定时器设置为20s以内，并打开sysconfig.h中的IHU_WATCH_DOG_SET_ENABLE_OR_DISABLE使能标识
  如果STM32CubeMX中的WATCH_DOG关闭，也需要将sysconfig.h中的项目IHU_WATCH_DOG_SET_ENABLE_OR_DISABLE使能标识关闭，不然编译不过
  如果STM32CubeMX中的WATCH_DOG打开，但sysconfig.h中的项目IHU_WATCH_DOG_SET_ENABLE_OR_DISABLE使能标识关闭，会造成REFRESH不成立，会造成硬件板子不断重启
= 归一化STM32CubeMX中管脚的命名，都形成了CUBEMX_PIN_F2_xxx的风格
= 由于管脚冲突，SPI1以及USART6暂时删掉了。未来如果管脚不够，需要换144封装
= 增加了GPRSMOD/RFID/BLE/SENSOR的电源控制GPIO口
= 增加了LED指示，包括电源、通信状态以及主控程序工作状态
= 增加了蜂鸣器BEEP的指示
= 增加明确的LED相应拉高拉低程序，让上层简单调用工作
= 完善了LED_POWER/LED_COMMU/LED_WORK_STATE/LED_BEEP的工作过程

//= ZJL, 2017 Jan.1, CURRENT_SW_DELIVERY R03.78 =>CCL项目
= 格式化BSP程序格式模板，预计命名规则
= WATCH_DOG功能的完善，LSI分频器的设置，待研究
= 增加BEEP BSP功能，基本配置等待STM32CubeMX的配置，继承自MAIN.h，初始化由MAIN自动搞定
= 增加CRC BSP功能，基本配置等待STM32CubeMX的配置，继承自MAIN.h，初始化由MAIN自动搞定

//= ZJL, 2016 Dec.31, CURRENT_SW_DELIVERY R03.77 =>CCL项目
= 清理BSP程序模块中的老旧库配置参数部分
= 所有管脚配置必须都来自于STM32CubeMX，而且都应该由STM32CubeMX进行命名
= 统一I2C/CAN/SPI接口的常量命名规范，保持跟UART一致
= 准备同步测试FreeRTOS的休眠功能
= 准备增加IAP的升级部分，将做为一种中间件放在L1VMFO目录下层次结构下，作为VMMW的核心功能。由于它要用到GPRSMOD的FTP功能，放在
  VMMW中在层次上讲是合适的。

//= ZJL, 2016 Dec.31, CURRENT_SW_DELIVERY R03.76 =>CCL项目
= 发送的GPRS AT CMD的所有VMMW API需要清理完善，包括增加FTP API
= GPRS AT CMD的接收需要寻找</xml>关键字，提高效率，避免纯粹依靠TIME OUT
= BSP串口接收中断改进：L2FRAME放在了SPARE1中，暂时不影响GPRS的使用
= 因为GPRS的半双工模式，跟L2FRAME帧结构模式不兼容，暂时无法融合到一起，除非采用项目#ifdef方式，但本质上没啥区别了。

//= ZJL, 2016 Dec.30, CURRENT_SW_DELIVERY R03.75 =>CCL项目
= 统一命名SPS中的全局变量，将代码风格标准化
= 清理掉UART_BPS模块中的无用代码，将基于v3.5的老库彻底去掉，全部基于HAL标准库
= 着重清理GPRSMOD的VMMW程序
= 支持RSSI VALUE的读取
= 改进所有传感器读取的函数API命名

//= ZJL, 2016 Dec.29, CURRENT_SW_DELIVERY R03.74 =>CCL项目
= Auth消息组装好，等待发送测试
= 清理L1COMDEF.H中的无效定义，完全转向HUITP架构和消息结构定义
= HUITP unpack接收消息格式从裸格式转换为xml格式完成
= 其它三组event消息（Event/Close/Fault)

//= ZJL, 2016 Dec.28, CURRENT_SW_DELIVERY R03.73 =>CCL项目
= 修改vmmw_gprs中函数的名称，统一到VMMW这一个关键字上来
= 修改BSP程序中的函数及常量，统一命名方式
= VMMW/APP模块只能访问VMFO的API，从而断绝上层与底层的关联，提高复用性
= BSP层面的常量全部修改完成，统一到命名规则上了

//= ZJL, 2016 Dec.28, CURRENT_SW_DELIVERY R03.72 =>CCL项目
= 继续解决VMMW_GPRS中间件在AT CMD下与CCL状态机组合工作的问题
= 梳理整体通信机制=>
==============
1. 这样IHU中的通信机制是这样的，L3 CCL State Machine -> L2 SPSVIRGO FRAME -> HUITP code/decode -> VM_MW_GPRS (AT CMD) -> 
  VM_FreeRTOS API -> BSP_USART(串口通信逻辑) -> STM32 HAL USART API。反过来就倒回来。
2.L3状态机靠状态+时钟超时来控制业务逻辑，L2FRAME编解码完成独特的HUIXML格式， VMMW在正常的串口通信上，叠加了TCP/UDP/FTP/HTTP/SMS
  /通信初始化/通信参数设定，让我们只需要关注通信内容本身，同时将完全的全双工变成半双工通信机制。
3.VM_RTOS+BSP是在HAL官方库的基础上，挂载自己的处理方式，本质上是将串口流逻辑转变为消息逻辑，也就是通过中断、时间延迟段，将连续不
  断的数据流转变为上层可以处理的消息包，所以需要叠加最为基础的链路头以用于消息报文分段，或者通过延迟达成消息报文的逻辑。
4.FreeRTOS处于BSP和VM之间，在串口通信上没有帮上忙，但对于其他资源如时钟定时器、内存管理、消息报文队列、任务模块管理，提供了简便
  稳定可靠的标准API，从而加速了整个程序架构逻辑的达成。
5.别看就一个串口通信机制，在其上照样可以搭建起一个完整的世界观。它也远不是一行printf()可以解决的。通信系统，远远不是那几个TELECOM 
  FEATURE，相比较而言，TELECOM FEATURE是最快最简单的，反而通信平台、管理逻辑、维护功能，更加困难和重要，而这些都是可以在不同项目
  之间复用传承的。
==============
= 完成SPSVIRGO从L2FRAME/HUITP接收消息的处理程序框架和处理逻辑。

//= ZJL, 2016 Dec.27, CURRENT_SW_DELIVERY R03.71 =>CCL项目
= 解码基础部分
= 解码函数完成：是基于非XML结构
= 跟L2FRAME_RCV连起来了
= 整个接收解码使用了强制消息结构转换，是否可行，未来需要进一步测试并验证
= 挂载VM_MiddleWare_GPRS模块，并跟BSP/HAL逻辑相连

//= ZJL, 2016 Dec.27, CURRENT_SW_DELIVERY R03.70 =>CCL项目
= 为HUITP定义常量，解决字符串长度问题
= 编译205VC，待删除，因为全部换成了207VC板子了
= 抽空将BFSC编译过了，不少CCL定义的独特变量，使用开关项单独处理了
= 完成了HUIXML的编码函数，同时改善了发送消息的长度，将消息缓冲区从205改为504，从而达到前后一致的效果。在此情况下，最长消息IE数据区不得超过100BYTE。
= 消息队列从4个减小为2个，不然内存占用过大。如果内存持续不够，未来还可以继续优化不需要用到的模块。

//= ZJL, 2016 Dec.26, CURRENT_SW_DELIVERY R03.69 =>CCL项目
= 继续完善HUITP协议的编解码
= 修改过多，check-in先

//= ZJL, 2016 Dec.25, CURRENT_SW_DELIVERY R03.68 =>CCL项目
= 持续HUIXML的编解码工作
= 暂时完成了MSGID/IEID/IE Structure的定义
= 由于内容居多，先CHECK-IN

//= ZJL, 2016 Dec.19, CURRENT_SW_DELIVERY R03.67 =>CCL项目
= 后台编解码探索
= 将HCU的代码先移植部分过来，待完善
= 24>>继续HUIXML的编解码任务

//= ZJL, 2016 Dec.18, CURRENT_SW_DELIVERY R03.66 =>CCL项目
= 提高数据源产生的频率，从1000分之一提高到1/2
= 为MSG_ID_SPS_CCL_FAULT_REPORT_CFM/MSG_ID_SPS_CCL_CLOSE_REPORT_CFM在SLEEP态建立入口，不然会造成时钟超时与SPS后台通信太久之间的矛盾
= 出现了一个特别奇怪的问题，DICOCAP中送来的命令，比如EVENT_TRIGGER等，CCL再发送CTRL_CMD给L2FRAME模块，耗费3s甚至更长的时间，这是为什么？是消息打印口的问题
  还是中间调度的问题？待研究调查
= 这个问题研究的结果是，串口助手打印时间戳出现的问题，消息发送正常，该问题关闭。

//= ZJL, 2016 Dec.16, CURRENT_SW_DELIVERY R03.65 =>CCL项目
= 核心堆栈不够，从8C00改为9C00，利用CubeMX修改
= 未来可以消减不适用的任务，达到降低内存的目标。目前内存还够，所有其它任务模块就一起带着吧。
= 修正了一些特别关键的BUG，比如VM中打印MSGID的小BUG，以及DIDOCAP中在EVENT_REPORT模式下不应该允许继续扫描外围传感器
= 还修正了CCL DOOR_TO_OPEN状态下等待SPSVIRGO发送CLOSE_REPORT_CFM报告的问题，不然会导致状态机混乱
= 修正了DIDOCAP中func_didocap_time_out_external_trigger_period_scan的执行条件：在后台SPSVIRGO通信未结束的情况下，不能允许二次重入的FAULT_TRIGGER
  事件。增加了判定条件FsmGetState(TASK_ID_SPSVIRGO) == FSM_STATE_SPSVIRGO_ACTIVED，而且在通信条件下TASK_ID_SPSVIRGO将进入FSM_STATE_SPSVIRGO_COMMU状态 

//= ZJL, 2016 Dec.16, CURRENT_SW_DELIVERY R03.64 =>CCL项目
= 完善TIME OUT的处理过程
= 完善DIDOCAP的事件处理过程
= 完善差错报告上传到CCL时，故障和故障恢复两种模式下的处理过程
= 在故障模式下，因为会有工作模式的定时器以及故障定时扫描两种处理均打开了，这两种模式下的扫描，可能会出现双发的问题，
  需要考虑是否需要故障恢复功能。
= 暂时的思路是，将门锁故障从故障恢复中去掉，以免冲突

//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.63 =>CCL项目
= 统一消息的定义，删掉无用的休息定义，以节省内存
= 基本上完成上层应用的状态机流程

//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.62 =>CCL项目
= 完善正常的工作流程
= ADC/I2C/DIDO/DCMI/SPS都搭建了工作模式，可以不用，但如果需要就可以用起来
= 大致将CCL的工作分为SLEEP、FAULT、WORKING三种工作模式
= FAULT对应于暴力开锁、水浸等情形
= 目前的设计假设是：一旦进入到FAULT，模式后，不再能退出来，除非错误得到了恢复
= 合并MSG_ID_CCL_COM_SENSOR_STATUS_REQ / MSG_ID_CCL_COM_CTRL_CMD消息，从而节省内存

//= ZJL, 2016 Dec.15, CURRENT_SW_DELIVERY R03.61 =>CCL项目
= 完善状态机
= 完善EVENT_REPORT状态下的报告模式
= 改动量非常大，故而形成一个单独的版本

//= ZJL, 2016 Dec.14, CURRENT_SW_DELIVERY R03.60 =>CCL项目
= 重新编译
= 为了让L1VMFO有很高的优先级处理HEART_BEAT消息，从而完成喂狗任务，在创建任务的时候，特地为L1VMFO的优先级+1，很好的解决了该问题
= 清理无效的CCL消息
= 将SYSDIM中的参数调整到与BFSC一致
= 增加DCMIARIS模块，抑制了PWM模块

//= ZJL, 2016 Dec.13, CURRENT_SW_DELIVERY R03.59 =>BFSC项目
= 在FreeRTOS中，通过使能#define configUSE_TICKLESS_IDLE 1，就可以达成休眠的状态
= 未来可能还需要进一步优化功耗，可参看《安富莱_STM32-V4开发板_FreeRTOS教程（V1.0）》
= 休眠模式下，调试器无法使用，可以使用以下函数使能调试器
  调用库函数：DBGMCU_Config(DBGMCU_SLEEP, ENABLE);即可
  调用库函数：DBGMCU_Config(DBGMCU_STOP, ENABLE);即可
= 增加VMFO模块，将看门狗任务放在VMFO任务模块中
= 因为增加了一个新任务，任务模块太多导致新任务创建不成功，将核心堆栈从7C00改为8C00后问题解决了
= 将CCL/BFSC总共4个任务的核心堆栈都改为了8C00
= 喂狗的过程需要通过CubeMX搞定，未来再搞，程序框架在VMFO中搭建完成。此为公共任务功能，也可以在其他项目中搞定，
	但要记得及时同步到其它项目中来，不然初始化不匹配
= 扬声器BEEP函数将写入LED模块中，未来需要搞一搞这个，这个就是标准的GPIO HAL接口函数而已
= BFSC中还存在3个告警，是属于形参的问题，需要改进为结构并传递指针就没问题，等待未来搞算法时再优化

//= ZJL, 2016 Dec.11, CURRENT_SW_DELIVERY R03.58 =>BFSC项目
= 增加Init_Req的测试代码，启动这个流程
= 在STM32板子上进行具体的测试

//= ZJL, 2016 Dec.8, CURRENT_SW_DELIVERY R03.57 =>BFSC项目
= 进一步改进ADC读写以及L2FRAME接收
= 将ADC/I2C/CAN的接口操作跟HAL函数连在一起了
= ADC的HAL函数待完善
= 增加了以下CAN的新的操作字，这些命令字，后面需要自行设计CAN接口
	IHU_CANVELA_OPTID_material_out_normal = 0x27,  //正常出料
	IHU_CANVELA_OPTID_material_give_up = 0x27,  //放弃出料
	IHU_CANVELA_OPTID_error_status = 0x28,  //错误状态	
= 完成了心跳被动反馈。是否要下位机主动，待后期定义

//= ZJL, 2016 Dec.8, CURRENT_SW_DELIVERY R03.56 =>BFSC项目
= 对WS、MOTO的控制与启动、停止等命令
= 对WS、MOTO命令控制的框架完成
= 下面就需要继续研究ADC硬件的具体工作模式，形成ADC读数到称重的具体滤波算法，从而将读数转变为向上汇报的EVENT
= MOTO亦然了。

//= ZJL, 2016 Dec.7, CURRENT_SW_DELIVERY R03.55 =>BFSC项目
= 完善整个命令系列
= 完成CAN与ADC/I2C之间的控制命令框架

//= ZJL, 2016 Dec.7, CURRENT_SW_DELIVERY R03.54 =>BFSC项目
= 定时器处理过程完成
= ADC测试假数据搞好

//= ZJL, 2016 Dec.6, CURRENT_SW_DELIVERY R03.53 =>BFSC项目
= 搭建BFSC应用层状态机框架
= 完成BFSC的基本工作流程，特别是L3BFSC和CANVELA的工作交互消息
= ADC采用假数据方式
= 马达暂时没有反馈，待研究物理器件的操控方式来决定
= CAN编解码，从HCU中复制过来

L3 DEVELOPMENT FEATURE AND CONTENT LIST
========================================R1.0 START============================================
Update log: 2015, July - Sep, no version control
Accomplished Features / Function:
==MYC 3.150.15 migiration to 3.150.2
  > AirSyncDebug 6 steps accomplish
  > AirSync statemachine control via SPS_SERVER_TASK/gattc_write_cmd_ind_handler incoming message processing, but AUTH message still go through counter
  > Deactive GPIO Debug LED-ON/OFF and AirSyncPermSetting
  > To be evaluated SPS_SERVER_TASK/sps_server_create_db_req_handler initialized setting
  > To test UL/DL E2E chain
  > To test accuracy of PUSH/REQ/RESP message content and BASE64 code/encode  

==BXXH_L3 architecture and framework: init and mainloop entry
  > Move most high level modification to bxxh_l3 directory
  > Keep driver related content still in exisitng SPS project
  > Untight new development add-on features and existing official project
  > Fully using SPS as basic foundation features

==Print console
  > Function active by official guidance (CFG_PRINT): doc <software development> v1.7
  > periph_setup.c file active uart2_init_func, modify GPIO_reservations and set_pad_functions
  > GPIO_PIN_1 -> UART2_TX, GPIO_PIN_2 -> UART2_RX, uing GPIO_PORT_1
  > To be tested: Pin to USB hardware
  > 串口测试功能，完成 
  > Printf的字符串，必须以\r\n结束

==Task creation and FSM framework
  > Task create procedure described at bxxh_adapt.h
  > TRACE switch defined at bxxh_adapt.h
  > All L3 message structure defined at relevant task.h

==AirSync framework
  > Migration from main_arch.c to L3_airsync_task.c
  > Active message send function
  > Create main FSM, leave full FSM to future

==Message send/receive and trace hook
  > Standard internal message structure
  > Landing trace method
  > Message reach with special issue: state machine FSM can not be split by message group, but has to be group together
  > 字符串连接功能: strcpy(s1, s2), strcat(s1, s2)
  > TRACE解码功能: 需要使用 DA官方自带工具 SmartSnippets / Connection Manager
  > Bxxh_message_send技巧：如果带BUFFER，则新消息结果尽量保持跟收到的消息结构一致，以便方便转发。如果不带BUFFER，则申请新的变量m，将变量m赋值好后，作为参数传递进去
  > 如果不采用以上方式，会遇到内存堆栈越界、数据的大小头等疑难杂症问题
  > bxxh_data_send_to_ble()数据从WXAPP发送到BLE，没有采用SPS_SERVER_INIT_BLE_TX_REQ消息，而是使用了异步的UARTTOBUFFER推送函数，是因为消息没法带数据体
  
==完整AIRSYNC状态机
  > Google protobuf集成进来，以便实现L3-AIRSYNC的解码
  > 内存不够的问题：先禁止AES的功能，未来想办法选择O3进行代码量的优化
  > Keil在调试过程中，无法看到变量的原因：将c/c++中的optimization -o3选项改为最低级的o1选项即可
  > 在此过程中，需要仔细设计RAM的MAPPING/SACTTING问题，并在编译界面选择ROM(CODR)及RAM(DATA)的空间位置
  > Keil: 未来产品递交，再改为O3高级优化，缩小代码量达到30%左右
  > Auth/init-req: 由于空间的显示，该函数无法采用编码函数实现，而只能直接生成二进制码流实现发送
  > 超过20BYTE长度的消息：分段在BLE底层自动完成，AIRSYNC不用考虑分段问题，可以申请长数据段，一次性搞定

==闪灯功能恢复：完成，效果不错
  > LED的闪动功能，放到了BXXH_ADAPT中，以便泛化。其它任务只需要调用即可。
  > 未来需要专门为LED写驱动及控制API，以便上层调用

==L3 WXAPP framework
  > 基本框架搭建完成
  > 数据采集，依赖于AIRSYNC编解码完成后，再细化
  > 下载数据以及程序，等待AIRSYNC完善，而且还要配合其他PROJECT一起综合考虑

==Timer mechanism
  > app_connect(): set timer as example
  > app_state_wechat -> start timer of 350ms to trigger AIRSYNC transform from INIT into BLE_CONNECTED state, also add INIT state
  > 使用标准的bxxh_timer_set搞定
  > 待解决：定时器是否与SLEEP MODE相互冲突，需要再研究

==NVDS MAC ADDRESS: 只需要在NVDS.C中输入一次，程序中将自动全部从这儿读取，不再考虑每个地方单独修改

==Using and validation on test task: done!

==L3 features validation
  > REQ/RESP SEQ递增机制：完成，微信不支持SEQID和DATA_RESP消息
  > 瞬时读取: 完成
  > 网络无连接，数据存入FLASH中：需要增加一个状态，即无连接工作状态，而且需要FLASH工作正常，从而触发进入该状态，并持续读取数据并存入FLASH
  > L3-WXAPP一旦进入BLE连接状态，将立即发送所有FLASH的数据到BLE后台，一直到清零FLASH为止
  > 下位机如何维持一个时钟计时器？ 没有这个功能，无法上传TIMESLOT信息，解决方式是发送上行记录N，从而推算距现在的历史N次远网格数据
	- nTimes机制完成
	- 每一次BLE连接成功后，都来一次时间下行同步，UNIX时间戳下传到下位机
  > 下位机缺乏GPS信息，需要手机补充，但手机APP采用微信上传时，显然无法带入该信息，只能在每个定时网格采集手机的GPS信息，当做信息采集的地址
  > WXAPP完美状态机：增加离线继续工作的状态机，以便数据存在FLASH中

==CLOCK
  > 简易时钟机制完成
  > 主要采用BLE连接态主动同步系统后台时钟 + 3分钟定时器计数的方式

==公共问题及改进
  1> GPS换为4Bx2 = 8B，4字节为GPS-X，4B为GPS-Y，生产源保留GPS数据生产
  2> TIMESTAMP统一都改为4B的Unix时间，后期一起搞，待完成
  3> 如果AIRSYNC的长度超过2个20字节长度，会出现差错，导致上传到后台不成功，只能将GPS地址去掉，反正目前也没用，硬件获得不了GPS地址
  4> 如果两个定时认为均为3分钟级别，则AIRSYNC链路在3分钟内没有任何数据传输，BLE将进入SLEEP并断掉，造成手机屏保，从而不
     再活跃，暂时将MODBUS定时上报改为1分钟一次: 这个问题没有再出现，现在又改回3分钟了
  5> 消息如果放在不同的TASK中，出现不同地方定义的消息，在目的任务中，消息路由错误的问题。改进的方式是将所有的消息全部定
	 义在BXXH.h中，进行统一编码，暂时没出现错误
  6> 两个任务的时钟如果过于相同，他们的定时器启动基本上在同时启动的，造成消息发送也在几乎同一时刻，容易出现微信ARISYNC
     接收解码合并的错误
  7> 通过AIRSYNC中设置定时器去轮循，暂时解决了多任务同时发送BLE冲突的问题，未来需要设置多连接，彻底解决该问题
  8> DATA REQ/RESP的确成对，但RESP是由微信后台自动发起，不用后台的SAE云负责处理。下位机对于RESP实现WAIT-STOP机制，确保
     数据接收的证实，需要在AIRSYNC中引入新的状态机，并开放一个BUFFER用于存储各个任务模块来的数据。
  9> BLE向微信后台发送数据REQ后收到RESP证实帧比较慢，大约10s左右，所以下位机发送的频率受到极大的限制
  10>strcat涉及到memory allock问题，不能使用，故而不能使用简单的字符串连接功能
  11>Trace机制完善，各个任务中的处理，在例外情况，需要放入足够的打印串口内容，以便了解潜在的问题
  12>UART1留着给MODBUS专用，UART2专用于打印口

==UART1
  > RTS/CTS在CONFIG中关闭，才能让UART正常工作，不然会干扰数据的发送
  > 50001000/THR-LCR寄存器的设置，保持原样，以便维持FIFO的收发稳定和性能最佳状态
  > UART1只能工作于115200速率，对于其他速率，比如9600，顺利搞定
  > UART1缺省配置为9600，UART2缺省配置为115200

==多板配置
  > 多个版本的PEM1.0/PEM1.1/PEM2.0不同配置情况下的配置，在da14580_config.h中，通过关键字进行配置选择，一次只能DEFINE一个，不然会出现不可预知的错误

==MODBUS
  > 框架搭建完成
  > void bxxh_data_send_to_uart(unsigned char *p, int n): 完成
  > data_push_pm1025_report的数据汇报方式，在AIRSYNC中进行数据字段的逐个拷贝处理，吃相太难看，未来需要进一步优化，节省代码空间
  > 配置MODBUS，使其未来容易被配置到不存在，待处理
  > 通过O1优化选项/调试WATCH窗口，可以观察全局变量状态，代替串口打印或者单步（无法重复设置），VIEW中需要选中“Period Window Update”
  > RTS/CTS信号如何处理？
  > MODBUS 485属于半双工，数据是定时读取，然后数据的接收还需要等待串口数据的主动上发，如何处理状态机: 暂时使用最简化方式，由外部硬件进行处理
  > 截断app_sps_scheduler文件中uart_rx_callback中对app_uart_push的处理，以便数据不是从串口发送到BLE，而是直接发送到MODBUS进行解析
  > MODBUS从外设读取数据，可能会出现外设也已经存储数千个数据的情形。暂时不考虑外设中的缓存数据，一切以IHU定时为准，定时到达便读取当前最新数据
  > MODBUS的状态分为BLE在线及BLE离线：在线直接发送数据，离线缓存数据到FLASH，然后利用在线空隙二次发送到后台
  > MODBUS在读取UART1的数据过程中，采取UART1第一次发送触发->等待50ms全部读取的方式，不然需要采用主循环去轮的方式
  > MODBUS在收到UART1数据时，只能解码一种数据格式，即40004-40009的PM1.0/PM2.5/PM10的解码，其它暂时均不支持

==FLASH桶形数据处理
  > EMC FLASH数据存储的桶形算法完成，暂时存在内存中，未来需要移植到FLASH中
  > 将不同传感器采样到的数据，都存到桶形缓冲区中，然后由WXAPP统一发送到后台，便于程序的设计
  > 这样大大降低新增传感器的处理复杂度：一旦增加新的传感器，它将具备两个状态：在线状态下，直接发送到后台；离线状态下，存入到桶形缓冲区

==AIRSYNC中发送数据的机制
  > 目前实现的方式采用BOOL变量控制的方式，外加TIMEOUT轮循，需要考虑一种新机制，以便泛化：后期增加新的传感器，可以方便的实现
  > 采用数据缓存的方式，需要更多的内存空间，暂时等等
  > 一旦增加温度、湿度等其他传感器维度，需要及时改善这部分的设计机制  

Update log: 2015 Oct.3, SW Version: XQ.WEMC.SW.R01.001
=版本信息
  > 增加版本信息打印口输出
  > 增加版本信息对后台微信命令的反应，通过0xF0命令查询
=OTP
  > OTP与DEBUG不能同时进行，启动一个，必须禁止另外一个
  > CFG中可以配置OTP及NVDS的启动
  > 烧录在这个版本时间段被MYC搞定！
  > 硬件启动
=MODBUS PM2.5返回的4BYTE数据，高低16BIT不是按顺序存放，而是高16BIT存后面，低16BIT存前面，程序进行必要的调整，以方便后台取数据
=JSAPI
  > 增加基本的反应，以便JSAPI WXAPP上的开发。复杂的情形，留待进一步的定义和开发。
  > WXAPP/MODBUS内部处理完成
  > 在AIRSYNC的数据发送，暂时等待状态，并未成帧发送出去，留待JSAPI WX界面的开发进度。
=优化绝大部分的编译WARNING
=Watch Dog

Update log: 2015 Oct.4, SW Version: XQ.WEMC.SW.R01.002
= //2014/10/04, MYC, IHU_EMCWX_CURRENT_SW_DELIVERY 2
= 清理了一部分Trace，增加消息长度，以及汇报值EMC/PM25，作为调试
= 将打印函数hexbyte2string替换为sprintf，增加Buffer
= OTP配置: 
  > #define DEVELOPMENT_DEBUG   0
  > #define APP_BOOT_FROM_OTP
  > #define CFG_WDOG
  > #define CFG_HW_PEM1_0 //PEM1.0板子
= WORKAROUND in arch_main.c, main_func(), 临时解决OTP MAC地址不从配置变量读取的问题:
    uint8_t *otp_bdaddr = (uint8_t *)0x20000000 + 0x7fd4;
    otp_bdaddr[0] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[0];
    otp_bdaddr[1] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[1];
    otp_bdaddr[2] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[2];
    otp_bdaddr[3] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[3];
    otp_bdaddr[4] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[4];
    otp_bdaddr[5] = nvds_data_storage.NVDS_TAG_BD_ADDRESS[5];

Update log: 2015 Oct.7, SW Version: XQ.WEMC.SW.R01.003
= ZJL, 2015 Oct.7, IHU_EMCWX_CURRENT_SW_DELIVERY 3
= 将MAC地址改为23，对应ZJL的测试板子
= MODBUS中数据读取缺省赋值的删除注销，未来数据只能从PM25外设读到，不然就没有返回。如果不确定是否有问题，可以读取EMC试验一下

Update log: 2015 Oct.15, SW Version: XQ.WEMC.SW.R01.004
= ZJL, 2015 Oct.15, IHU_EMCWX_CURRENT_SW_DELIVERY 4
= JSAPI FEATURE的完善和启用
= 18 00/18 02 27 11, 各个消息中IE_Type字段的处理，包括生成
= InitReq中TAG_InitRequest_RespFieldFilter = 0x7F，从而要求WXAPP回送所有10个参量
= InitResp中处理回送的参量，存储在全局变量中：框架搭建完成，但解码函数无法复用，简单的解码过于复杂，未来需要单独写解码函数，一并搞定
= DataEmc/DataPm25数据通过Req消息发送，均支持IE_TYPE的区分
= 由于IE_TYPE的存在，DATA_REQ消息编码变得更为复杂，未来需要尽快搞定统一完善的编码函数，否则编码效率越来越低了
= AIRSYNC中数据发送的轮循，之前为1500ms(airsync.h中定义)，需要顺应JSAPI实时控制的需求，优化到最小值，预计500ms水平

Update log: 2015 Oct.18, SW Version: XQ.WEMC.SW.R01.005
= ZJL, 2015 Oct.18, IHU_EMCWX_CURRENT_SW_DELIVERY 5
= 定时汇报4种气象参数
= 离线记录，并在线发送回去
= 不支持瞬时读取模式，定时回送给云后台的消息体中不带时间戳，全靠nTimes进行判断，简化版

Update log: 2015 Oct.21, SW Version: XQ.WEMC.SW.R01.006
= ZJL, 2015 Oct.21, IHU_EMCWX_CURRENT_SW_DELIVERY 6
= 更新GPIO配置管脚跟PEM2硬件设计一致

Update log: 2015 Oct.29, SW Version: XQ.WEMC.SW.R01.007
= MYC, 2015 Oct.29, IHU_EMCWX_CURRENT_SW_DELIVERY 7
= 更新GPIO配置管脚跟PEM2硬件设计一致,修正BEEP和UART2打印的管脚
		#define UART2_TX_PORT   GPIO_PORT_0 //MYC
		#define UART2_TX_PIN    GPIO_PIN_7  //MYC
		#define UART2_RX_PORT   GPIO_PORT_2 //MYC
		#define UART2_RX_PIN    GPIO_PIN_4  //MYC
		#define BEEP_PORT  			GPIO_PORT_1 //MYC
		#define BEEP_PIN   			GPIO_PIN_0  //MYC
= 增加Buffer管理的机制,对于AirSync收消息会进行超过20bytes的组包: buffer_man.c, buffer_man.h
= 增加DebugPrint的开关和API，用来控制DebugTrace
= 目前在启动之后，AirSync建链之前关闭打印，怀疑连接过程中Trace过多导致死机
= InitReq，不申请微信的各种信息(InitResp从107bytes下降到46bytes):
		//}	这些领域，全部设置为1，总数值=0x7F，从而所有参量都从InitResp中回送
		InitReq[18] = 0x00;	
= MAC地址使用的是0x28, 0xEF, 0xA5, 0x72, 0x39, 0xD0
= 关闭OTP宏
= 选择开发模式宏
		/*Build for OTP or JTAG*/
		#define DEVELOPMENT_DEBUG   1       //0: code at OTP, 1: code via JTAG
		/*Application boot from OTP memory - Bootloader copies OTP Header to sysRAM */
		#undef APP_BOOT_FROM_OTP  //code via JTAG
		//#define APP_BOOT_FROM_OTP   //code via OTP
= LZH发现的DataReqEmc[24]的问题还没有改(-> 仔细检查没有错误)

Update log: 2015 Nov.2, SW Version: XQ.WEMC.SW.R01.008
= MYC, 2015 Nov.2, IHU_EMCWX_CURRENT_SW_DELIVERY 8
= 改正一个错误,原来的ID为26/27/28, 应该是27/28/29
	L3CI_wind_direction = 0x27, //Wind Direction
	L3CI_temperature = 0x28, //Temperature
	L3CI_humidity = 0x29, //Humidity	

Update log: 2015 Nov.8, SW Version: XQ.WEMC.SW.R01.009
= MYC, 2015 Nov.8, IHU_EMCWX_CURRENT_SW_DELIVERY 8
= 增加IC2 LED的驱动以及API(i2c_led.c, led_i2c.h) 
= 增加LED与AirSync/Modbus的状态显示:
     - BLE CONNECTED (AIRSYNC NOT CONNECT YET) -> LED_ID_7 => ON
	 - BLE CONNECTED, AIRSYNC CONNECTED (AFTER RECEIVED AUTH RESP FROM WeChat) -> LED_ID_7 => OFF
	 - BLE CONNECTED TO DISCONNECTED  -> LED_ID_7 Keep OFF
	 - BLE DATA TRANS/RECIEVE -> LED_ID_7 Blink Once
	 - UART(MODBUS) TRANS/RECEIVE -> LED_ID_6 Blink Once
= 已知问题：PEM2的三色LED不工作，单色的也要加上拉电阻分压，否则会影响寿命


========================================R2.0 START============================================
Update log 2016.Jan.17, SW Version: XQ.WEMC.SW.R02.001
= 恢复到初始状态，准备重新架构程序
= 更新GPIO管脚，如下的改动，然后UART2打印口可以正常工作了
//#define UART2_TX_PORT   GPIO_PORT_0 //MYC
//#define UART2_TX_PIN    GPIO_PIN_7  //MYC
//#define UART2_RX_PORT   GPIO_PORT_2 //MYC
//#define UART2_RX_PIN    GPIO_PIN_4  //MYC
#define UART2_TX_PORT   GPIO_PORT_1 //ZJL
#define UART2_TX_PIN    GPIO_PIN_2  //ZJL
#define UART2_RX_PORT   GPIO_PORT_1 //ZJL
#define UART2_RX_PIN    GPIO_PIN_3  //ZJL
= 硬件使用了最新版PEM2.0，并为PM2.1做准备，IHU使用了外接DB15接口，方便连线
= Adding defination of #define CFG_HW_PEM2_1  //PEM2.1板子

Update log 2016.Jan.26, SW Version: XQ.WEMC.SW.R02.002
= 使用CFG_MODBUS的#if (CFG_MODBUS)模式，将程序恢复到没有MODBUS和风速风向等传感器的状态，只保留EMC任务
= 在EMC任务中，依然保留了AIRSYNC和WXAPP模块
= 采用O0编译方式，CODE SIZE = 26KB，使用O3方式，CODE SIZE = 21KB，该程序还需要进一步优化，但已经好多了

Update log 2016.Jan.26, SW Version: XQ.WEMC.SW.R02.003
= 完全去掉MODBUS相应的模块以及任务，将MODBUS从咱们的任务中去掉了

Update log 2016.Jan.26, SW Version: XQ.WEMC.SW.R02.004
= 架设VMLAYER的雏形，移植BXXH_ADAPT到公共VM结构体系开始了！
= 原先本来计划先将BXXH_ADAPT搞出一个较为完整的版本，后来考虑到架构不合理，这个中间结果存在的价值比较低，所以直接向目标架构VM直接演进！
= 完全复用HCU中VM的公共文件，当然经过必要的更新，以适应IHU工程
= 此种方式，是为了分开诸多功能和定义，方便未来持续的优化演进，更为为了程序更新上的效率着想
= 增加L0COMVM/L0VMDA/L0VMSVR等文件目录集合，方便自行设计的程序按照不同的层次关系进行组织，也是为了以后进行移植到其他硬件平台
= 将文件按照UTF-8进行存储，该文件的中文字体就可以在不同系统下共享以及拷贝，非常好！
= BXXH_L3的执行逻辑还未发生变化，只是VM中的文件目录结构已经整理好了，后面需要先将bxxh_adapt中的内容拆解到VM层中，以便分步实施

Update log 2016.Jan.26, SW Version: XQ.WEMC.SW.R02.005
= BXXH_ADAPT.x进行拆解，分别分到不同的COMVM文件中去。

Update log 2016.Jan.27, SW Version: XQ.WEMC.SW.R02.006
= 建立VMSHELL机制
= 搭建完成基础的VMDA模块架构
= 搭建出基础的VM和任务框架
= 搭建出所有程序的模子

Update log 2016.Jan.28, SW Version: XQ.WEMC.SW.R02.007
= VM虚拟机的深化
= 消息队列和状态机机制完成

Update log 2016.Jan.30, SW Version: XQ.WEMC.SW.R02.008
= 继续VM虚拟机工作机制
= 虚拟机的初始化过程具备

Update log 2016.Jan.31, SW Version: XQ.WEMC.SW.R02.009
= 继续VM虚拟机的搭建
= 消息发送与接收机制完成
= AIRSYNCAPI目录重新规整

Update log 2016.Feb.1, SW Version: XQ.WEMC.SW.R02.010
= 基本完成VM虚拟机的搭建
= 搭建定时器任务的完整内容
= 搭建EMC/ADCARIES/AIRSYNC的内容框架
= 优化了任务数量，从最合适的8个优化到6个，不想继续优化到极致的4个，因为这样会让程序编写的效率和可读性下降
= 消息转发机制还未开始，业务逻辑待完成，内存空间是极大的挑战
= 内存优化还可以考虑动态内存技术，暂时不考虑，这是以程序的不稳定性和不可读性为代价的

Update log 2016.Feb.2, SW Version: XQ.WEMC.SW.R02.011
= 调试VM虚拟机完成
= 内存严重不够，已经开始出现打印信息影响全局变量正确与否的情况，导致执行出错
= DEBUG工作的非常不好，关闭所有DEBUG及TRACE，不然内存出错
= 只打开了很少的关键打印信息，以便知晓程序运行时候正常

Update log 2016.Feb.2, SW Version: XQ.WEMC.SW.R02.012
= 开始搭建任务逻辑
= 为了节省内存，将消息结构体和所有任务模块中的UINT32改为UINT8，MessageId从UINT32改为UINT16
= 为了AIRSYNC状态机够用，SYSDIMENSION中最大状态机从5改为6个
= 消息数量从28减少为23，比较危险够用，但内存不够用，暂时没有其他好办法
= MAINLOOP_HOOK的处理，采用1/5000的处理方式，因为VM上层应用本来就是低烈度处理负荷，从而提升系统的稳定性
= AUTH启动时的延迟需求，正好通过上面的这种延迟，得到改善
= 采用编译选项，分别编译任务模块
= 搭建起AIRSYNC和EMC的程序框架

Update log 2016.Feb.3, SW Version: XQ.WEMC.SW.R02.013
= 完善AIRSYNC模块
= 将L2AirsyncApi目录移动到L2_ASYAPI_TC目录下，明确这是TECENT公司提供的API接口函数
= 将AIRSYNC改为ASYLIBRA，意为LIBRA天枰座，Asylibra
= 将AIRKISS改为AksLeo，意为狮子座
= 清理所有的文件结构，将不需要不应该包含的文件全部去掉，从而简化包含关系
= 文件包含关系都是VMLAYER.H为核心基础，包括BXXH_L3相关的程序文件
= 增加[VM的使用方法：以3.20版的SPS为例]，以便未来方便的移植该程序模块
= AIRSYNC中的不同状态改变，都会促使拉灯行为，为了方便程序的编写，拉灯的具体行为模式均定义在asylibra.h文件中，以方便未来程序的维护
= 手工编解码的AIRSYNC_LIBRA协议部分完成，胜利在望！

Update log 2016.Feb.4, SW Version: XQ.WEMC.SW.R02.014
= 完善AIRSYNC任务模块
= 完善EMC任务模块
= 恢复SequenceId链路层的处理功能
= 恢复EDDT_manufatureSvr/WX-H5的处理过程
= 恢复数据MEMDISK存储过程
= 恢复链路在线后立即回送的过程
= 继续保持FLASHDISK存储过程的开放性
= 时间同步
= 设备信息同步过程
= 去掉MODBUS其它传感器支线
= 去掉AIRSYNC定时发送WX数据过程
= 引入EMC_ONLINE和EMC_ONLINE_BLOCK状态，从而根本上解决了离线数据覆盖重发的问题：
  这是通过AIRSYNC DATA_RESP过程，确保数据发送的完整性和稳定性。
  坏处是明显的，就是发送数据变得低速了不少。从穿戴设备应用场景的角度，这是可以接受的。
  = AIRSYNC链路在数据发送中可能出现断链，相应的恢复机制完成
= 周期性定时数据发送完成
= 瞬时请求数据完成
= 基本完成了BXXH_L3程序到VM架构的移植，但程序编解码依然采用了字节对其方式，待完善


========================================R2.1 START============================================
Update log 2016.Feb.5, SW Version: XQ.WEMC.SW.R02.101
= 将所有的编解码换成Protobuf编解码函数，原先的手工编解码依然保留做对照
= 增加完善的L3 DATA编解码过程，放在EMC L3模块中
= 后台的PUSH/REQ数据流做单独的编解码，这部分是属于L3的编解码，跟L2无关
= 去掉设备信息、时间同步、数据发送等不同的编解码，因为这属于L3，编解码放在EMC中了
  ->去掉MSG_ID_EMC_ASYLIBRA_TIME_SYNC以及MSG_ID_EMC_ASYLIBRA_EQU_INFO过程，完全合并到DATA_REQ过程中
= 将所有定义在BLE底层的消息体提到COMMSG.H中来，以减少底层移植带来的挑战
= 还原buffer_man.*到l3_bxxh文件系统中去
= 将时间同步、设备信息以及EMC REPORT在EMC L3模块中进行合并，通过DATA_REQ消息一起发送到AIRLIBRA，从而简化L2协议的处理，完全遵从协议层次

Update log 2016.Feb.5, SW Version: XQ.WEMC.SW.R02.102
= 实验去掉bxxh_l3目录后，编译正常
= 为了对照，又增加了bxxh_l3目录，纯粹是为了留下一个对照的程序文件记录。
= BXXH_L3在程序中没有钩在任何地方，所以不占用任何内容空间
= 由于内存空间的限制，VM程序的编译选项（sysconfig.h）也关闭了，出了EMC之外，最主要的AIRSYNC模块并没有在编译之列
= AIRSYNC编解码非常占内存，它的打开，使得VM/TIMER/ASYLIBRA等都编译不过
= VM及以上引用模块全部关闭时，生成越18KB的空间，打开任何一个，就达到27KB了，总程序在28KB左右编译不出


========================================R3.0 START============================================
Update log 2016.Feb.5, SW Version: XQ.WEMC.SW.R03.01
= 采用IOT-WECHAT 5.190.2例子，并基于SDK5
= 增加VM程序组，但钩子并没有钩上WECHAT例子，因为并没有起作用，只是编译完成
= 为了编译通过，修改
  - rwip_config.h中的TASK_VMDA定义
  - user_periph_setup.h中有关RESET_LED_GPIO定义
  - i2c_led.*的部分数据结构重复定义
  - bxxh_l3未能加入，不然错误太多，因为它跟sps例子结合太紧密
  - 很少且必要的l2_asyapi_tc文件，主要是aes相关，因为其它文件函数都存在于WECHAT-IOT例程中
= 是为起点
  
Update log 2016.Feb.5, SW Version: XQ.WEMC.SW.R03.02
= 配置GPIO为PEM2.0/PEM2.1: user_periph_setup.h
= 增加PEM1-PEM2.1的开关选项：da1458x_config_basic.h
= 修改BLE设备的广播标签：user_config.h
= 修改MAC地址：da1458x_config_advanced.h
= 修改AIRSYNC中MD5方式，从 //#define EAM_md5AndAesEnrypt 1改为 #define EAM_macNoEncrypt 2： mpbledemo2.h
= 增加get_mac_addr函数，以便让mpbledemo2.c编译通过：修改vmlayer.h/vmlayer.c，
  => 将函数  OPSTAT ihu_mac_add_get(UINT8* mac, UINT8 len)  映射到  OPSTAT get_mac_addr(UINT8* mac)
= 整个程序可以通过ASD的BLE链路建立过程，直接到达DATA_PUSH/DATA_REQ/DATA_RESP阶段
= UART2打印口工作在115200bps下，解码及显示均完美呈现
= 在WATCH_DOG打开时，进入单步调试模式，容易出现硬件中断停止状态，关闭看门狗以后恢复正常，也行以后OTG或者FLASH程序烧制时可以打开，现在不合适。

Update log 2016.Feb.6, SW Version: XQ.WEMC.SW.R03.03
= 试着测试端到端
= 引入L3DATA编解码过程，方便前后端的对测对调
= 将user_app下头文件*.h引入到合适的位置
= 试着改写了第一个ReadEmc的函数，使用了内部消息机制
= 由于命令定义的问题，对于BleDempHead可能需要重新考虑，如何才能更为有效的利用其结构和CMDID，待探究
= 命令定义了CMD_SENDDAT_TIME_SYNC / CMD_SENDDAT_EQU_INFO / CMD_SENDDAT_EMC_REPORT，放在一起进行了处理
= 增加了这部分代码后，打开MD5空间编译不够，只能先使用MAC地址方式

Update log 2016.Feb.21, SW Version: XQ.WEMC.SW.R03.04
= 修改MAC地址，顺序还是需要反着
= EMC的PUSH读取以及反馈机制，暂时实现不了定时机制，内存空间严重不够
= EMC手机/后台发送PUSH下行命令，回REQ信息(EMC数据）给后台，程序沿用03版，基本上没有变
= LightOn/Off命令的编码改为0x1101/0x1102，暂时为了更EMC的0x2001/0x2081相区别

Update log 2016.Feb.25, SW Version: XQ.WEMC.SW.R03.05
= 修改发送回去EMC INSTANCE REPORT的工作函数，经过测试，工作正常
= 发送回数据后，本来应该收到L2的DATA_RESP帧，但还出现20002/DATA_RESP消息收到后，IHU进入DEBUG/死机的情况，需要再调查原因
  (__asm("BKPT #0\n");b)

Update log 2016.Feb.27, SW Version: XQ.WEMC.SW.R03.06
= 修正收到DataResp消息后，进入(__asm("BKPT #0\n");b)的错误状态，通过检查保护搞定

Update log 2016.Feb.27, SW Version: XQ.WEMC.SW.R03.07
= 修正SPI-FLASH的DI/DO线序，交换，以修正该错误，这是user_config中的配置管脚信息

//= ZJL, 2016 July.27, IHU_EMCWX_CURRENT_SW_DELIVERY R3.08
= 将PEM定义移到sysversion.h中，以便将应用部分的修改全部集中到应用区，而不再改动SDK部分，确保SDK部分与应用部分的分离，方便未来SDK升级

//= ZJL, 2016 Aug.4, IHU_EMCWX_CURRENT_SW_DELIVERY R03.09
= 升级到KEIL5对应的工程项目
= 还是以DA14580/Cortex-M0为蓝本，内存空间不足，MAC验证是标配，加密方式不能增加进来
= 程序的处理部分非常简化，不具备离线自动上报功能，而必须采用微信界面手动点击采样的方式
= 准备考虑将程序的架构改为同时支持STM32
= 整个业务目录从emcapp改为bxxhapp，程序配置中，c/c++中的inlcude配置项，必须将bxxhapp中对应的文件目录全部加上，不然有可能部分文件找不到
= VM中的程序模块，全部是VM以上单独形成的，跟底层需要创建的业务模块没有必然关系，我们可以如此理解：TASK_VMDA相当于进程，VM的任务都是
  此进程中的线程任务模块
= 暂时将支持的目标VM设置为DA14x系列，STM32系列，NORDIC系列，以及TI系列，但本项目工程IHU_PRG_EMCWX只需要支持VMDA即可
= 基于WEHAT SDK5的程序，ASYLIBRA/EMC等任务模块程序都没有用上，数据的采样过程直接通过WECHAT参考用例程序完成的。程序留着，只是为了怀念。
= 等待DA14680出来后，内存空间充裕后，再行将EMC等L3任务模块用上，从而达到分离的目的。一旦达成，这种情形下，ASYLIBRA将会永远不用的，
  毕竟管方程序更加稳定可靠

//= ZJL, 2016 Aug.4, IHU_EMCWX_CURRENT_SW_DELIVERY R03.10
= 合并WECHAT、SPS以及STM32到同一个PROGRAM

//= ZJL, 2016 Aug.5, IHU_EMCWX_CURRENT_SW_DELIVERY R03.11
= 修改完善全局常量的命名规则，以便适应多项目模式下的严格区分
= 完善LED BLINK中对PEM2.0判定的条件
= 切换PEM2.0/PEM2.1，确定后台连不上的原因，发现PEM2.0/PEM2.1在管件连接上没有啥区别，除了两个不太用的管脚。锚定PEM2.1进行测试。
= 增加了定时机制，以便在链路建立起来后，定时回报数据，待测试完善
= 统一命名，将VMDA对应的函数改为VMDA开头的函数，以便未来支持更多的VM虚拟机函数入口，并且命名有规律

//= ZJL, 2016 Aug.13, IHU_EMCWX_CURRENT_SW_DELIVERY R03.12
= 完善VMDA的函数空间
= 统一PEM的#if (表达式)定义方式
= 统一ihu函数对不同项目的分类处理
= 统一对VMDASHELL和vmda1458x底层函数的命名
= TASK_VMDA1458X和TASK_ID_VMDASHELL任务分别对应SHELL上层任务模块以及底层SDK认可的任务模块
= 将PEM定义以及MAC地址定义放到da1458x_config_basic.h中，以实现多个人共享程序代码的能力

//= ZJL, 2016 Aug.13, IHU_EMCWX_CURRENT_SW_DELIVERY R03.13
= 实验FLASH读取，以及LED灯状态的控制
= 定时读取EMC数据依然有问题
= 在user_periph_setup.c中注册LED、FLASH等管脚
= 增加外设的初始化过程

//= ZJL, 2016 Aug.16, IHU_EMCWX_CURRENT_SW_DELIVERY R03.14
= 验证定时读取过程，设置定时器的准确时间长度，DELAY网格为10ms，所以长度应该设置为1000，表示10s
= 增加PEM3.0/PEM3.1的配置管脚，为烧制UART1的配置（P04/P05）做好准备

//= ZJL, 2016 Aug.27, IHU_EMCWX_CURRENT_SW_DELIVERY R03.15
= 增加Flash_Programmer以及Sencond_bootloader程序代码到本项目工程
= 修改并完善PEM3.0/PEM3.1的关键定义，包括ADC0在PEM3.x中采用了ADC1通道，而非之前确定的ADC0/P00通道
= ADC_READ读取N次并取最大值，依然没有解决实际读书偏小的问题。给天线输入3.3v，读数是满量程3FFH = 1024，说明整个后向通道是好使的
= 增加EMC数据格式的CMDID字头部分，采用垃圾编码的方式，纯粹为了方便后台云程序的编解码

//= ZJL/MYC, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.16
= 适配PEM3.0板子，清理PEM1.x/2.x的老旧程序和配置信息，从这一版开始，不要再试图测试和使用PEM1.x/PEM2.x板子了，因为不再支持
= MYC增加亮灯DEMO程序
= ADC通道改为P02后，读书正常。本来以为ADC通道为P01，端口Port0/Pin1，怪哉。
= 去掉EMC最大值机制，换成了平均值，更加稳定可靠
= 最终平均500次，取非0的均值

//= ZJL/MYC, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.17
= 完善调试LED灯，规定：
	单色灯LED6表示BLE链路是否连上
	三色灯LED0/1/2三色等表示数据传输状态，目前貌似还有些问题
= app_task.c: gapc_disconnect_ind_handler中增加 vmda1458x_led_set(LED_ID_6, LED_MODE_OFF)，以便链路断开时灭灯，测试结果良好
= 删掉vmcomapi目录，调整VMLAYER的命名，为增加其他VM虚拟机程序，打基础
= 增加freeRTOS/uCOSii对应的目录
= 增加emc1468x和数采仪大板scycb对应的项目目录，就是为了KEIL下的编译和VM体系建立

//= ZJL, 2016 Sep.17, IHU_EMCWX_CURRENT_SW_DELIVERY R03.18
= 建立SCY/EMC68X的版本控制全局信息
= 建立vmfreeoslayer和vmucoslayer程序框架
= 由于在emcwx项目下，KEIL无法编译通过，所以只能单独再创建一个全新的项目，用来编译和处理FREEOS/UCOS的VM
= 由于L1timer, L1comdef, commsg等，都要复用到VMDA中的环境，导致新项目无法编译，故而只能将不同的VM文件拆开
= L1VMFREEOS/L1VMUCOS中存有L1COMDEF/L1COM/VMLAYER等，不再单独放到不同的文件中去，以避免文件结构过于复杂

//= ZJL, 2016 Sep.18, IHU_EMCWX_CURRENT_SW_DELIVERY R03.19
= 继续VMFREEOS框架程序建立
= FreeRTOS待完成的函数体：
  => 不支持<system.h>中的sleep, usleep，导致未完成
	void ihu_sleep(UINT32 second); 
	void ihu_usleep(UINT32 usecond);
=> 不支持<sys/ipc.h>中的msgctl, msgget, IPC_EXCL/IPC_RMID/IPC_CREATE，导致以下函数出现问题
	extern OPSTAT ihu_msgque_create(UINT8 task_id);
	extern OPSTAT ihu_msgque_delete(UINT8 task_id);
	UINT32 ihu_msgque_inquery(UINT8 task_id);
	extern OPSTAT ihu_msgque_resync(void);
	extern OPSTAT ihu_message_queue_clean(UINT8 dest_id);
	extern OPSTAT ihu_message_send(UINT16 msg_id, UINT8 dest_id, UINT8 src_id, void *param_ptr, UINT16 param_len); //message send
	extern OPSTAT ihu_message_rcv(UINT8 dest_id, IhuMsgSruct_t *msg);	
=> 不支持<pthread.h>，导致以下函数不能支持
	extern OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio);
	extern OPSTAT ihu_task_delete(UINT8 task_id);
	=> CALLBACK不支持，导致以下函数可能不正常
	extern OPSTAT ihu_task_create_and_run(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem);
=> VM以及各个任务的自启动RESTART机制，待定
=> 打印时候需要\n做为结束符，先修改完善
=> 基本上完成FreeRTOS VM的框架环境，但还未跟操作系统直接相连接
=> 确保了本VM_TEST项目和EMCWX项目的编译成功
=> 完成了EMC68x对应的所有任务模块的框架建立

//= ZJL, 2016 Sep.19, IHU_EMCWX_CURRENT_SW_DELIVERY R03.20
= 建立scycb的程序框架，以及VMUO所对应的VM程序框架
= 完成了VMFO和VMUO两种VM的框架。VMFO未来将不保证完善的更新，主要任务将放在VMUO的更新上，以便与UCOS紧密结合并对其

//= ZJL, 2016 Sep.20, IHU_EMCWX_CURRENT_SW_DELIVERY R03.21
= 增加LPCCCB/PLCSB两个项目的配置，修改sysconfig/L2frame等部分
= 分离l1timer_freeos和l1timer_ucos
=>  不支持<sys/time.h>的sigval, sigevent, itimerspec, timer_t导致以下函数无法继续
	void func_timer_routine_handler_1s(union sigval v);
	void func_timer_routine_handler_10ms(union sigval v);
	void func_timer_routine_handler_1ms(union sigval v);
	OPSTAT fsm_timer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
= 时钟全局DIMENSION定义，移到L1TIMER中去了
= 完成所有项目的框架改造，以支持l1timer的改进

//= ZJL, 2016 Sep.20, IHU_EMCWX_CURRENT_SW_DELIVERY R03.22
= 集成ucosIII的操作系统到Scycb项目工程中来，以便开始集成ucos操作系统
= 暂时遇到uCosIII的最小DEMO程序不能下载到STM32205RC评估班子中去，这个应用最初是支持STM32F207IGH6，待搞定，不然无法调测
= 之前的项目工程Scycb VMUO暂时保留，待清除

//= ZJL, 2016 Sep.21, IHU_EMCWX_CURRENT_SW_DELIVERY R03.23
= Sleep / uSleep通过OSTimeDlyHMSM()解决
	//如果RATE_HZ配置是100u，意味着步进编程10ms，这样所有的实际时间将大10倍，所有在VMUO和OS的配置之中，需要严格对其这一点。
	// #define  OS_CFG_TICK_RATE_HZ 1000u
= 为了生成10ms的任务级时钟，以下缺省配置从10u改为了100u，从而将任务级的时钟频率改为100Hz
	#define  OS_CFG_TMR_TASK_RATE_HZ          100u              /* Rate for timers (10 Hz Typ.)                           */
= 使用了OSTmrCreate / OSTmrStart函数后，解决了L1Timer的初始时钟创建问题，同时以下函数体得到解决
	OPSTAT fsm_timer_init(UINT8 dest_id, UINT8 src_id, void * param_ptr, UINT16 param_len)
	void func_timer_routine_handler_1s(OS_TMR *p_tmr, void *p_arg);
	void func_timer_routine_handler_10ms(OS_TMR *p_tmr, void *p_arg);
	void func_timer_routine_handler_1ms(OS_TMR *p_tmr, void *p_arg);
= L1timer基本解决，等待最终测试
= 调整全局消息数量和任务数量等性能数据，最终内存占用在70KB附近，以便充分测试128KB的内存以及配置。程序代码71KB，必须放在FLASH中执行。

//= ZJL, 2016 Sep.21, IHU_EMCWX_CURRENT_SW_DELIVERY R03.24
= 应用ucosiii的函数OSTaskCreate / OSTaskDel，完成了以下函数体
	extern OPSTAT ihu_task_create(UINT8 task_id, void *(*task_func)(void *), void *arg, int prio);
	extern OPSTAT ihu_task_delete(UINT8 task_id);
    => CALLBACK不能支持修饰带来的风险
	OPSTAT ihu_task_create_and_run(UINT8 task_id, IhuFsmStateItem_t* pIhuFsmStateItem);
= 为了支持删除任务，全局变量开关被打开 #define OS_CFG_TASK_DEL_EN              1u   /* Include code for OSTaskDel()     
= 为了支持删除队列，全局变量开关被打开 #define OS_CFG_Q_DEL_EN                 1u   /*     Include code for OSQDel()  
= 为了支持清理队列，全局变量开关被打开#define OS_CFG_Q_FLUSH_EN               1u   /*     Include code for OSQFlush() 
= 创建了新的MEM PARTITION，以支持队列的数据发送和接收
= 初步完成ucosIII和VMUO的改造与对齐

//= ZJL, 2016 Sep.22, IHU_EMCWX_CURRENT_SW_DELIVERY R03.25
= EMC: 打开CFG_MEM_MAP_DEEP_SLEEP模式，进一步省电。由于符号表占用的内存不够，没做成，需要未来进一步优化内存空间试试。
= EMC: 打开WATCH_DOG
= EMC: 关掉CFG_DEVELOPMENT_DEBUG
= 简化EMC项目的程序文件，去掉VMDA的VM机制，因为本来就用不上，而且还占用大量的符号表，造成无法扩展已有功能

//= ZJL, 2016 Sep.22, IHU_EMCWX_CURRENT_SW_DELIVERY R03.26
= 继续调测EMC项目，将VMDA中的不用函数全部注释掉，去掉L2/L3的暂时无用函数，减少符号的产生，终于而已打开CFG_MEM_MAP_DEEP_SLEEP模式，并关闭CFG_DEVELOPMENT_DEBUG
= 最终推荐两种配置方式（均打开了深度休眠模式CFG_MEM_MAP_DEEP_SLEEP）
  ==> 调试工作模式：O1编译选项，关闭WATCH_DOG，打开CFG_DEVELOPMENT_DEBUG，打开<mpbledemo2.h>中的#define EAM_macNoEncrypt 2
      Program Size: Code=27132 RO-data=2992 RW-data=120 ZI-data=7556  
  ==> 批量生产模式：O3编译选项，打开WATCH_DOG，关闭CFG_DEVELOPMENT_DEBUG，打开<mpbledemo2.h>中的#define EAM_md5AndNoEnrypt 1 
      Program Size: Code=26472 RO-data=2684 RW-data=116 ZI-data=7804
= 烧录程序，SPI的管脚P0/3/5/6，记录在这儿
= 电池读取API，验证好使。现在关键是如何表达电量，初步想法是：
  1. 电源电量在低压时，使用快闪表达。函数挂在哪儿，是个问题。
  2. BLE连上时，电源单色灯使用慢闪表达，断掉时，灭灯。目前函数挂在BLE连接以及DISC函数中。
  3. 状态三色灯，用来表达数据传输的状态。目前每一次数据传输，只快闪一次，功能挂在数据读取函数中。
= 需要继续验证定时器问题，不仅解决电源低压的状态展示，也用来搞定周期汇报的问题。
= 为了测试TIME OUT的问题，在wechat_task以及app_wechat_task中，均植入了探针
  arch_printf("\r\n Wechat Task Time Out received No 1. ");  
  试图找到定时工作的模式。未来可以将这两个任务中的打印删除。
  另外，从目前的TRACE看，wechat_task参与了，但app_wechat_task貌似根本就没参与。
  一旦确认，需要删去相应代码，以减少代码符号量溢出的可能性。
= 另外，mpbledemo2_readEmcDataResp(NULL, 0);这里的指针全部是NULL，造成汇报一两次后，BLE链路在微信上就断掉，AIRSYNC工具下
  还可以工作，但微信上一旦从设备上发出数据，微信BLE立即断掉，使用了三种数据函数调用方式都不行
= 对APP_WECHAT_TASK中的TIMEOUT代码做了清理

//= ZJL, 2016 Sep.23, IHU_EMCWX_CURRENT_SW_DELIVERY R03.27
= 将循环读取次数修改为IHU_EMC_LOOP_READ_TIMES = 10次以后，修复手工读取中断BLE链路的问题，主要是adc_get_sample超时导致的
= 但手工读取又会导致返回的数据=941/942/943，明显不合理，自动读取不出现该问题，貌似是内存越界造成的，不明白为什么
= 定时和瞬时模式好使了，待定义明确的工作方式后再行完善可控工作模式，目前完全混合支持
= 目前定时工作模式是永远打开定时汇报，这也是痛苦的，因为系统永远定时工作，打扰得厉害
= 支持了定时读取的开关控制，如下，等待后台修改好了之后进行验证
	readEmcPeriodOpen  = 0x2002,  //后台送到DEVICE
	readEmcPeriodClose = 0x2003,  //后台送到DEVICE
	该命令无返回，只是闪灯表示结果

//= ZJL, 2016 Sep.23, IHU_EMCWX_CURRENT_SW_DELIVERY R03.28
= 添加SCY项目中的ISR工作机制
= 添加SCY项目中的ADC/SPI数据消息
= 建立了SCY初步的消息处理状态机框架

//= ZJL, 2016 Sep.24, IHU_EMCWX_CURRENT_SW_DELIVERY R03.29
= 建立查询电量机制
= 确定颜色
	vmda1458x_led_blink_once_on_off(LED_ID_0); //BLUE
	vmda1458x_led_blink_once_on_off(LED_ID_1); //GREEN
	vmda1458x_led_blink_once_on_off(LED_ID_2); //RED
= 更新了EMC项目的管脚配置
= 更新了2nd boot loader的管脚配置
= 每次都初始化ADC通道，读取后，DISABLE_ADC()，比较圆满的解决了读数大的问题。应该是9个ADC通道相互串的问题。
= VIN_OK表征真正的电池量，但没有接到ADC通道上，无法正确读出，下一版改进
= MYC改进了闪灯工作状态


//= ZJL, 2016 Sep.25, IHU_EMCWX_CURRENT_SW_DELIVERY R03.30
= 恢复10次读取取最大值
= 改进VIN_OK的电源指示读取，从而补全单色电源指示灯的工作状态：电量低于阈值=》快闪，BLE连接=》慢闪，电量正常但BLE无连接，不闪。
= 发现当电池低电压的时候，插上miniUSB充电，VIN_OK处于高，插上JTAG供电工作时，VIN_OK是低
= 优化vmda1458x_led_set(xx, LED_MODE_OFF)的时机，在AIRSYNC链路出现error，以及BLE链路断掉时，均主动关闭BLE链路指示慢闪
= 周期汇报处理函数中，增加BLE链路上AIRSYNC的状态判断，以防止非正常状态下强行发送数据，虽然这个时候，一样会导致不成功
= FLASH的读写函数spi_flash_read_data，是可以访问的
= 在有FLASH外部空间，且OTP不支持的情况下，为了修改MAC地址，可以使用如下方式
	方式1：烧录到FLASH中的MAC地址为0x366以及0x3E0，记录是为了对照，以便手工修改
	方式2：通过在FLASH的0x7FF8（最后的8个字节）中存入MAC地址，然后让nvds_read_bdaddr_from_otp强行读取，或者定义一个标志位，以便OTP真的起作用时进行隔离风险。

//= ZJL, 2016 Sep.26, IHU_EMCWX_CURRENT_SW_DELIVERY R03.31
= 如果再出现GPIO_RESVR对应的调试问题，需要查阅mpbledemo2_init_peripheral_func中的BUTTON所对应的GPIO管脚是否冲突。当前板子被缺省
 定义为 HW_CONFIG_BASIC_DK
= mpbledemo2_data_error_func函数中的NVIC_SystemReset()函数没有打开，先得解决 "Known Issues:.1"，不然每一收到一个数据都会RESET板子
= 1. 收到sendDataResp， 出现 "received msg: no message content!"  从而导致 "! error: mpbledemo2 reseted"
=>通过一个临时补丁，解决规避该问题。因为服务器就是返回如此的FFFFFFFF=-1=SYSTEM ERROR差错，导致板子RESET的
  ##Received data:  fe 1 0 19 4e 22 0 3 a d 8 ff ff ff ff ff ff ff ff ff 1 12 0 12 0
= 打开mpbledemo2_data_error_func函数中的NVIC_SystemReset()
= 设置2nd bootloader的配置参数，其中将两个项目的MISO/MOSI都改为统一标识，其中MISO-DI，MOSI-DO，这样，就跟所有的文档
  (ANB001/ANB023)以及SmartSnippets工具中的配置都统一和一致起来了。同时订正2nd bootloader中的FLASH DI/DO。
= 关闭2nd bootloader中的UART启动，打开FLASH支持。原则上，这个版本可以用来直接烧录OPT了

//= ZJL, 2016 Sep.26, IHU_EMCWX_CURRENT_SW_DELIVERY R03.32
= 打开CFG_PRODUCTION_DEBUG_OUTPUT，发现较多的问题。这个时候在生产时需要，待定。
= 验证CFG_BOOT_FROM_OTP打开，则MAC地址就是全FF，说明的确是从OTP Header中读取的。这说明，未来一旦OTP中烧录了MAC地址，而且想起作用，这个标志必须打开。
  幸好FLASH代码是可以多次烧录的。
= MYC订正app.c中的MAC地址顺序，解决IOS不能识别的问题
= OTP独立烧制，以及PLT烧制，都是好使了，这一版可以当做了官方DELIVERY程序了
= 当前本地MAC=MYC，CFG_BOOT_FROM_OTP已经打开。如果需要本地调测，则需要将MAC改为自己的，而且需要将BOOT_OTP关闭，才能通过JTAG下载到FLASH进行调测
= PLT上的RF测试还有各种问题，待解决
= 生产流程文档《DA14580开发测试环境与过程V06.docx》以及相应的PLT批量版本均在云上了

//= ZJL, 2016 Oct.4, CURRENT_SW_DELIVERY R03.33
= 准备EMCWX以及SCYCB项目的持续完善
= 重新清理了一下任务清单和目录结构，将老旧vmuo@stm32裸系统的程序例子删掉，将sps_device任务删掉，已经没有用处，但sps_device源代码还保留。
= 修正vmfo编译中的问题：l2adc模块将需要分开，不然他们必须复用msgxxx.h的消息定义部分

//= ZJL, 2016 Oct.11, CURRENT_SW_DELIVERY R03.34
= 首次将FreeRTOS VMFO嫁接到FreeRTOS上的1468x SDK 1.0.6官方例子上，基于SmartSnippets Studio v1.2.3编译通过
= 编译环境不支持最新版Eclipse，下载调测还不明白是否支持KEIL5
= 为了更好是使用该环节，请遵循以下步骤进行项目工程创建和环境设置，不然会出现意想不到的问题
  1. 不用使用自己的Eclipse，而使用SmartSnippets Studio V1.2.3，假设装在C盘上，c:\DiaSemi目录下
  2. 进去后选择SDK Boot Root选择另外一个目录，比如D:\TEMP，设置为缺省工作目录，不要跟git sourcecde放在一起，IDE工具将会在该目录下创建
             工作环境和临时文件。
  3. 选择IDE进去，打开IDE工作环境，如果已经存在项目程序，删除清空
  4. Import项目，选择DA1468x_SDK_BTLE_v_xxx，（Existing Projects），系统会列出SDK下的所有项目，Deselct所有的，只保留Freetos_retarget
  5. 如果已经存在l0comvm/l1vmfreeos/l2frame/l3emc68x，请删去，这只是link，并不会影响项目的编译。
  6. 点击freertos_regarget项目名字，右键选择new=>Folder->advance->link to location->选择SrcBxxh目录下的l0comvm，保持创建后的名字也是l0comvm
  7. 如法炮制创建剩下的三个目录，但l1vmfreeos， l3emc68x可以修改，以便保持跟其他开发者一致
  8. 将SrcBxxh整个git source下的目录做备份，很重要！如果这个不做，后面的操作会删掉源代码！！！
  9. 在刚才创建好的项目工程目录中，l0comvm删掉其他项目的commsgemcwx.h/commsgscycb.h文件，只保留commsgemc68.h，其他的公共.h文件不要动。
  10. 在l1vmfreeos目录下，删掉ucos相关的文件
  11. 在l2frame目录下，删掉adcaries.c/h，asy*.*, aks*.*
  12. 在l3emc68x目录下，只保留l3emc68x.*，删掉其他文件
  13. 修改sysdim.h中的项目参数，改为emc68x项目
  14. 编译，这时应该编译通过。如果不通过，以上操作一定有什么差错，可以重新来过。
  15. 将SrcBxxh目录拷贝回来git source，注意，这里可以选择不重复拷贝，只拷贝不存在删掉的文件。
  16. 完善IDE的工作界面，Windows->Preference->General->Apperance->Colors and Fonts -> Basic -> Text Font-> Edit -> Courier New, Reglar, 10
                如果Courier New字体不存在，去c:\Windows\Fonts目录下将Courier New改为显示
  17. Windows->Preference->General->Editors -> Text Editor -> Display Tab Width = 2, Inserts Spaces for Tabs勾选
  18. Windows->Preference-> c/c++-> code style -> Formatter -> Edit -> Indentation -> Tab policy = "Space only", Indentation size = 2, Tab size = 2
  19. 打开l2frame/adclibra.c，看看状态机消息路由表是否排列整齐，如果不整齐，说明上述步骤没有设置完整。
  20. 原则上，下一次再打开项目，这个环境就是保持状态，而不再需要重新设置。

//= ZJL, 2016 Oct.12, CURRENT_SW_DELIVERY R03.35
= 完善修改EMC68x项目中FreeRTOS VMFO的任务、消息、定时器、DELAY等函数体，跟FreeRTOS严格绑定，从而完成VMFO对FreeRTOS的落地
= 该VMFO是以freertos_retarget项目参考例子为基础的，其他例子暂时不能编译通过，等待新的SDK出来后再行转换。原则上，我们需要WECHAT例程
= ihu_vm_main(void)挂在main函数里面，做为入口。未来一旦新SDK出来后，这个需要保持更迭。
= 统一更新L3CI/L3PO的定义，在L1COM_DEF.H文件中
= SCYCB项目中的L1TIMER任务不再进入无线循环SLEEP，因为还要等待接收消息处理，不然任何额外的系统消息再也发不进来了
= EMC68x在FreeRTOS没有优化，也没有WECHAT任务例程的情况下，占用DATA内存=28+75=103KB，CODE内存=46KB。未来期望CODE可以多些，内存还可以再优化些
   text    data     bss     dec     hex filename
  45896     856   28396   75148   1258c freertos_retarget.elf

//= XX, 2016 Oct.12, CURRENT_SW_DELIVERY R03.36
= 修改完善ucosiii VMUO Printf，使用了PC10/11作为USART3的打印输出口，115200, TTL电平输出
= 修正SCYCB中ACTIVED状态，以及INIT状态下的处理
= 增加SCYCB中TIME_OUT的处理样例
= 修改MESSAGE_MAX_LENGTH = 64-6 = 58。一方面，PARTITION必须跟256/128/64/32对其，另一方面，TIME_OUT消息发送时，如果TRACE_ALL打印打开了，就不工作了
= 由于系统堆栈的问题，目前消息最长长度不能超过128，如果设置为256则会出错
= 修改TRACE选项为TRACE_ALL_BUT_TIME_OUT

//= ZJL/XH, 2016 Oct.22, CURRENT_SW_DELIVERY R03.37
= XH专门完善SPI接口调用功能
= 测试GPIO读取能力
= 增加l1hd_stm32f2的目录，专门用来存放所有的STM32F2系列的驱动函数
= 验证LED，修改LED_OUT到管脚PC03，的确可以点亮LED灯。目前程序是复用了例子程序中的LED点灯功能，该灯纯属205RB板载LED灯。
= 根据EMC68X_VMFO的要求，将任务目录进行了分离修改，以便适应Eclipse完整目录的要求
= 完善了整个目录结果，删去VMFO的老旧项目，重新将项目和目录结构恢复到最简化的程度

//= ZJL/XH, 2016 Oct.23, CURRENT_SW_DELIVERY R03.38
= 串口调测USART1， TX/RX=PA9/PA10
= UART4映射到PC10/PC11是固定无可选择的
= 为了避免UART4和USART3的冲突，将打印口USART3换到了PB10/PB11
= USART1=>GPRS, USART2=>RFID, UART4=>BLE
= USART/UART都正常了，基本上将串口的配置完善了，测试全部通过了

//= ZJL/XH, 2016 Oct.25, CURRENT_SW_DELIVERY R03.39
= 完善LED的配置和规则化命名
= USART/LED驱动到VMUO的函数映射
= 增加LED在板灯以及时钟到达灯的定时取反控制
= 参照例子，增加完整的GPRS通信例程
= 完善SPI2的通信控制机制
= 机械的加入GSM/GPRS程序例子

//= ZJL/XH, 2016 Oct.27, CURRENT_SW_DELIVERY R03.40
= 完善GSM/GPRS控制的过程，整合不同例子中的精华并结合到一起
= PRINT暂时保留，还没有被优化掉
= TIM_Cmd(TIM2, ENABLE); 使能TIM2时，程序挂了。如果不打开TIM2，则SPS_GPRS_Send_AT_Cmd需要等待定时器超时后才能得到AT回复，此时也会死等而挂掉
= GPRS通了，基本达成了验证的目标

//= ZJL/XH, 2016 Oct.29, CURRENT_SW_DELIVERY R03.41
= 尝试STM32CubeMX工具，并创建<PrjCclFreeRTOS>项目，看看是否可以自动生成底层HAL驱动，并是被FreeRTOS形成底层平台系统
= 增加CCL项目ID
= 在VMFREEOS.x中区分不同OSAL和BARE两种情形：OSAL是DIALOG公司在1468x中增加的一层包络，但STM32CubeMX并没有这个东西，只能简单的使用BARE裸FreeRTOS操作系统
= VM中项目的初始化，需要区分不同项目ID的情形

//= ZJL/XH, 2016 Oct.31, CURRENT_SW_DELIVERY R03.42
= 集成并移植FreeROTS VMFO到CCL项目中
= 集成了所有L2L3任务模块，外加VMFO挂载，最终在L0无优化条件下生成如下
  Program Size: Code=52096 RO-data=3380 RW-data=1152 ZI-data=43216  
  "PrjCclFreeRTOS\PrjCclFreeRTOS.axf" - 0 Error(s), 0 Warning(s).
= 对其SCYCB项目和CCL项目在SYSTEM DIMENSION方面的定义
= 垃圾集成GPRS模块到CCL项目
= 验证了STM32CubeMX工具对于配置的改动，并不会造成手工录入代码的丢失问题
= 删去PrjCclNoOs项目，简化整个目录结构

//= ZJL/XH, 2016 Oct.31, CURRENT_SW_DELIVERY R03.43
= 继续修改和移植FreeRTOS的VMFO部分
= 引入驱动程序部分L1BSP_STM32
= 修改管脚配置，先将阻塞主程序执行的外设禁止掉，未来再完善
= SP2必须以DMA方式工作，否则无法初始化成功
= 任务创建的优先级必须小于7，否则无法创建成功
= 打印在USART3口上，通过L0BSP_STM32进行映射
= 核心任务的current_taskid机制不再起作用，因为任务启动和任务创建不在同一个时刻完成，而必须全部创建完成后才能进行
= 选择合适大小的任务堆栈，核心OS堆栈不够，0x3C00 (15KB)改动到0x7C00，不然无法将9个应用级任务创建成功  
= FsmProcessingLaunch(void)机制，将其改造OPSTAT FsmProcessingLaunch(UINT8 *p)，传入task_id参数，不然无法创建所有任务成功
= 禁止xxxxbarexxx类型的API，暂时用不到，以免影响本VMFO的顺利执行
= 除了第一个TIMER任务之外，其他任务无法被正常调度。。。验证TASK02可以被正常调度
= 去掉了所有TIMER（6/7），改掉了TIM2位SYSTICK/System-wake，选择了USE_PORT_OPTIMISED_TASK_SELECTION选项，均没有效果
= FsmProcessingLaunch传入的参数错误，导致了新任务启动不起来。修改了一种精巧的方式，应该成功了。
= 验证了多各任务启动正常

//= ZJL/XH, 2016 Nov.3, CURRENT_SW_DELIVERY R03.44 =>CCL项目
= 着手解决CCL/Print问题：基本上解决，SLICING分片式关掉，不然一定会导致多个任务的打印内容重叠在一起
= 未来如果的确需要SLICING机制，则需要启动独立的打印任务，以彻底解决该问题
= LED灯恢复正常了
= 时间戳是软件版本编译的固定日期时间，需要改进
= 串口垃圾集成完成，待完善调测

//= ZJL/XH, 2016 Nov.4, CURRENT_SW_DELIVERY R03.45 =>CCL项目
= 短信发送完成
= 串口延时接收依然存在问题，OK并没有真正收到。。。
= 每次使用STM32CubeMX工具修改完之后，需要将FreeRTOS.h中的SLICING关掉，不然缺省会打开的
= SLICING轮片式模式下，L1TIMER暂时存在问题，导致软定时器工作不正常，未来待解决
= 串口接收完全搞定，GPRS/SMS函数完美的改好了！
= XH恢复SPI接口的函数
= HAL_SPI_MspInit / HAL_SPI_MspDeInit函数在bsp_spi中必须被禁止，不然重复定义

//= ZJL/XH, 2016 Nov.7, CURRENT_SW_DELIVERY R03.46 =>CCL项目
= 继续完善GPRS模块的呼叫功能
= 完成了GPRS的子函数功能，可以对GPRS模块进行呼叫、短信、GPRS连接等各种操作了

//= ZJL/XH, 2016 Nov.8, CURRENT_SW_DELIVERY R03.47 =>CCL项目
= 更新PrjCclFreeRTOS到PrjCclFo@205RG项目，其它一切保持不变
= 增加PrjCclFo@207VC项目，保持跟PrjCclFo@205RG完全一致
= 需要修改Main.c以及FreeRTOSConfig.h，将VMFO以及SLICING配置项都改进去，然后一切都一模一样了
= 增加PrjBfscFo@205RG项目，初始化BFSC组合秤的一些基本接口
= 增加PrjBfscFo@207VC项目

//= ZJL/XH, 2016 Nov.9, CURRENT_SW_DELIVERY R03.48 =>CCL项目
= 修改GSM模块为SIM800A
= 基于HC05的BLE串口模块，基本完成获取MAC地址的函数
= 新建RFID UART的基础函数
= 试着利用HAL修改SPI访问函数
= UART/SPI的终端成帧处理
= UART/SPI发送L2FRAME到SPSVIRGO/SPILEO模块
= SPSVIRGO/SPILEO对于L2FRAME数据的处理，暂时没有进一步进行消息级别的分解，这需要等待消息结构定义清楚后再行处理
= CCL状态机也有待完善

//= ZJL/XH, 2016 Nov.10, CURRENT_SW_DELIVERY R03.49 =>CCL项目
= I2C、DIDO、CAN接口完成了垃圾集成。未来需要进一步优化。CAN的帧编解码，未来需要进一步修正。
= SPI中断会Calback函数导致无限循环，暂时禁止掉。
= CAN总线由于没有对端，导致初始化超时失败。将Error_Handle函数禁止掉了，满足调试状态需求。未来正式情形下，需要打开，
从而让WATCH_DOG来处理，从而自动RESET，不然会导致系统不稳定。
= RTC给4个项目激活。IWDG/WWDG一旦激活，会出现故障，原因待调查清楚。
= 完成CCL状态机框架

//= ZJL/XH, 2016 Nov.11, CURRENT_SW_DELIVERY R03.50 =>CCL项目
= 完成CCL状态机状态的定型
= 完成CCL函数体的伪代码逻辑
= CCL状态机机制改为EVENT触发，触发逻辑在L2模块中进行判决，整个状态机比较理想了。

//= ZJL/JT, 2016 Nov.12, CURRENT_SW_DELIVERY R03.51 =>EMCWX项目
= 修改da1458x_config_basic.h中的SLEEP模式，从CFG_MEM_MAP_DEEP_SLEEP改为CFG_EXT_SLEEP，实验为什么系统没有进入SLEEP模式

//= ZJL, 2016 Dec.6, CURRENT_SW_DELIVERY R03.52 =>EMCWX项目
= 为了SmartSnippets下的编译，修改了以下打印函数，不然编译不过。这个函数是单为EMC68x设计，以后还需要进一步优化。
void IhuDebugPrintFoEmc68x(char *format, ...);
void IhuErrorPrintFoEmc68x(char *format, ...);
= 将IHU_WORKING_PROJECT_NAME_UNIQUE_STM32_EMC68X_ID修改为IHU_WORKING_PROJECT_NAME_UNIQUE_DA_EMC68X_ID，以反映实际情况
= 修改所有L2FRAME对应的程序模块中有关EMC68X的名字改动





CCL项目未来任务目标
= SPI-GPIO互通小目标
= 看门狗开启，自动重启
= FreeRTOS/ucosiii/TICK级的休眠省电
= 打印任务的搞法，以便打开SLICING时间轮片机制功能
= 时间戳是软件版本编译的固定日期时间，需要改进
= 软件下载
= FLASH磁盘功能




EMC Mission coming:
==Security
  > CFG中有开关可以控制
  > AIRSYNC CRC/AES亦相关
==BLE multi-connection


[VM的使用方法：以3.20版的SPS为例]
1. 升级到SDK最新版本，当前最新版本为SDK5
2. 复用一个例程，当前复用了SPS串口例程和IOT-WX例程
3. 将整个VM的目录放在xxx\dk_apps\src\modules\app\src\app_project下
4. 在KEIL项目图中，IMPORT所有VM代码
5. 修改arch_main.c，包含vmlayer.h，钩入VM初始化和VM执行体函数
6. 修改app_task.c，包含vmlayer.h，钩入BLE建链和断链过程
7. 修改sps_server_task.c，包含vmlayer.h，钩入BLE数据下行的上传过程
8. 数据上行下传过程，由SPS终端自动搞定，确保工作机制适合
9. 修改da14580_config.h，配置打印、串口流控（无）、串口速率（PRINT=115200/UART2, 9600/UART1），CFG_WDOG喂狗，去掉SLEEP模式等
10. 修改MAC地址、BLE广播、Profile配置、驱动配置GPIO
11. 在rwip_config.h中，增加TASK_VMDA模块
11. 编译并更正潜在的错误


