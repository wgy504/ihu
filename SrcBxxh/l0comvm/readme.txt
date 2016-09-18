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











Known Issues:
1. 收到sendDataResp， 出现 "received msg: no message content!"  从而导致 "! error: mpbledemo2 reseted"




Mission coming:
==1) FLASH Driver
  > 驱动中有READ/WRITE函数，官方文档比较明确
==2) OTP UBOOT
==SMART SNIPPETS
==Sleep: CFG_EXT_SLEEP / CFG_DEEP_SLEEP
  > 配置CFG文件，可以选择两种模式
  > 如何进行相应的功耗测试
==Drivers: POWER MANAGEMENT, ADC, LED
==Security
  > CFG中有开关可以控制
  > AIRSYNC CRC/AES亦相关
==2nd uboot, SPtOA and SW download features
==Merge of other projects: proximity tag, iBeaCon, etc.
==如果烧制MAC地址


[待验证问题]
- ADC sample
- BLE多连接
- GPS, to be accomplished by SAE cloud

[遗留问题]
- AirSync同步50%概率会Reboot，目前不知道元原因，但是连接上比较稳定
- 接收消息不会超过20 bytes，Init_resp（70bytes）收不全

[指望学生搞定的任务]
- FLASH
- SLEEP
- OTP UBOOT
- POWER METER
- LED driver
- BLE multi-connection
- Security
- SW Download
- Smart Snippets scripts
- OS adapt layer
- SDK 5.0.x new migiration

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

/*
**** send auth response ****
data len = 14
data dump = FE 01 00 0E 4E 21 00 01 0A 02 08 00 12 00 

**** send init request response ****
data len = 70
data dump = FE 01 00 46 4E 23 00 02 0A 02 08 00 10 B4 24 18 F8 AC 01 20 B9 AE 94 85 06 5A 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 8A 01 14 57 65 43 68 61 74 20 54 65 73 74 20 62 79 20 6D 61 74 74 21 

**** send SendData Response(echo request) ****
data len = 36
data dump = FE 01 00 24 4E 22 12 37 0A 02 08 00 12 16 0A 00 12 10 20 0E 00 06 00 01 02 03 04 05 01 02 03 04 05 06 18 10 

**** send ManufactureSvr Push ****
data len = 22
data dump = FE 01 00 16 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 00 

**** send Html Push ****
data len = 23
data dump = FE 01 00 17 75 31 00 00 0A 00 12 08 31 32 33 34 35 36 37 38 18 91 4E 

**** send wxWristBand Push ****
data len = 14
data dump = FE 01 00 0E 75 31 00 00 0A 00 12 00 18 01 

**** send EnterDeviceChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 01 

**** send Exit ChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 01 

**** send Enter HtmlChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 01 18 02 

**** send Exit HtmlChatView Push ****
data len = 14
data dump = FE 01 00 0E 75 32 00 00 0A 00 10 02 18 02 

**** send enterBackground Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 01 

**** send enterForground Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 02 

**** send enterSleep Push ****
data len = 12
data dump = FE 01 00 0C 75 33 00 00 0A 00 10 03 

*/
