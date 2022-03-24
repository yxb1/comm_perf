# comm_perf
Count the communication performance between two nodes, such as delay, packet loss rate, out of order, retransmission, etc. The two nodes include inter process, inter chip and inter device.
## cpp文件夹
cpp文件夹里的代码源文件用来放置在两个节点中的接收节点程序中。其中DSCommPerfWrite()函数的两个入参分别是unix时间戳（发送端）以及发送端sequence
## test文件夹
test文件夹给了简单的示例。这个实力用于生成最简单的数据文件，该文件只能用于plot.py(下面会提到)调试代码错误。本工程上传了一份真实的芯片间Eth通信的数据，位置在python/file/adcanif
## python文件夹
python文件夹中的plot.py程序就是用于画图的软件。生成的图表从左至右，从上到下分别是1，2，3，4.图1画出了丢包，乱序以及重传，其中x轴是真实时间，y轴是在某时刻丢包数等。在某些时候节点间的时延会从毫秒级上升到秒级，那么在一个图表中画出时延，会产生大多数数据堆积在一起形成横轴上的一条线，因此需要把大于一定范围的值单独领出来显示，图2就是显示大于25ms时延值的。图4是显示时延在【-25，25】范围内的值，超出就直接显示25或-25.图3用来显示时间上的跳动，如果某一帧的发送时间比上一帧快了好几秒，就把前后连续10帧的时间画出来，显示在图3中。
