# 高性能HTTP服务器
## 高性能网络编程框架
### 一，reactor模型
是一个基于事件分发和回调的反应堆。
主要包括：
1. event_loop 和**线程绑定**的无限事件循环
2. channel    注册到event_loop上对象的抽象
3. acceptor   服务器监听套接字的抽象
4. event_dispatcher  poll，epoll的抽象
5. channel_map    
### 二，支持多线程

### 三，封装读写操作到buffer对象中。