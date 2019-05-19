## 分布式监控系统

说明：分别运行在主机和树莓派上，用于实时检测并传输树莓派的硬件信息，并及时发送异常信息。

## 更新说明：

### epoll(DEMO)
  使用epoll做的demo，可供参考

### v3.1
  优化了makefile的写法,将错误信息写入日志中不输出

### v3.0
  加入了makefile
#### script:
  用于获取树莓派信息的脚本

#### master：
  1.将函数实现与主函数分离，监听4个端口，用于各种功能需要。
  2.实现与数据库的连接(需先建立数据库)并且对数据库进行操作。
  3.实现脚本传输到客户端，并接收返回数据按用户分文件夹存储。
  4.实现非阻塞select的心跳检测。

#### client:
  1.接收并运行脚本，定时发送数据给master。
  2.对过大保存文件进行数据的压缩，解压缩发送数据。
  3.采用多进程执行，并使用共享内存进行进程间通信。
  4.若master超时未连接client，client开启心跳检测通知master本机在线。(使用条件变量实现)

### v2.0-udp
  用udp实现客户端与服务端的连接，并不含检测功能。

### v1.0-DEMO

#### master:
  多线程存储上线server信息，并单开线程进行心跳检测实时检测在线server端。

#### default.conf:
  master配置文件。
#### server:
  tcp连接master端。

### PiHealth_v0.1
  简易版检测系统

### class-DEMO
  教学版本