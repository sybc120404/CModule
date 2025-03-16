# socket

## socket简介

套接字（socket）是 Linux 下的一种进程间通信机制（socket IPC），使用 socket IPC 可以使得在不同主机上的应用程序之间进行通信（网络通信），当然也可以是同一台主机上的不同应用程序。 socket IPC 通常使用客户端<--->服务器这种模式完成通信，多个客户端可以同时连接到服务器中，与服务器之间完成数据交互。

内核向应用层提供了 socket 接口，对于应用程序开发人员来说，我们只需要调用 socket 接口开发自己的应用程序即可！ socket 是应用层与 TCP/IP 协议通信的中间软件抽象层，它是一组接口。在设计模式中，socket 其实就是一个门面模式，它把复杂的 TCP/IP 协议隐藏在 socket 接口后面，对用户来说，一组简单的接口就是全部，让 socket 去组织数据，以符合指定的协议。所以，我们无需深入的去理解 tcp/udp 等各种复杂的 TCP/IP 协议， socket 已经为我们封装好了，我们只需要遵循 socket 的规定去编程，写出的程序自然遵循 tcp/udp 标准的。

当前网络中的主流程序设计都是使用 socket 进行编程的，因为它简单易用，它还是一个标准（ BSD socket），能在不同平台很方便移植，比如你的一个应用程序是基于 socket 接口编写的，那么它可以移植到任何实现 BSD socket 标准的平台，譬如 LwIP，它兼容 BSD Socket；又譬如 Windows，它也实现了一套基于
socket 的套接字接口，更甚至在国产操作系统中，如 RT-Thread，它也实现了 BSD socket 标准的 socket 接
口

## socket编程接口

需要包含头文件<sys/types.h>和<sys/socket.h>

### socket()

`int socket(int domain, int type, int protocol);`

socket()函数类似于 open()函数，它用于创建一个网络通信端点（打开一个网络通信），如果成功则返回一个网络文件描述符，通常把这个文件描述符称为 socket 描述符（socket descriptor），这个 socket 描述符跟文件描述符一样，后续的操作都有用到它，把它作为参数，通过它来进行一些读写操作。

该函数包括 3 个参数:

参数 `domain` 用于指定一个通信域； 这将选择将用于通信的协议族。对于 TCP/IP 协议来说，通常选择 `AF_INET` 就可以了，当然如果你的 IP 协议的版本支持 IPv6，那么可以选择 `AF_INET6`

![domain1](https://github.com/sybc120404/image4md/blob/main/domain1.png)

![domain2](https://github.com/sybc120404/image4md/blob/main/domain2.png)

参数 `type` 指定套接字的类型，当前支持的类型有：

![type](https://github.com/sybc120404/image4md/blob/main/type.png)

参数 `protocol` 通常设置为 `0`，表示为给定的通信域和套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，可以使用 protocol 参数选择一个特定协议。在 AF_INET 通信域中，套接字类型为SOCK_STREAM 的默认协议是传输控制协议（Transmission Control Protocol， TCP 协议）。在 AF_INET 通信域中，套接字类型为 SOCK_DGRAM 的默认协议时 UDP。

调用 socket()与调用 open()函数很类似，调用成功情况下，均会返回用于文件 I/O 的文件描述符，只不过对于 socket()来说，其返回的文件描述符一般称为 socket 描述符。当不再需要该文件描述符时，可调用close()函数来关闭套接字，释放相应的资源。

如果 socket()函数调用失败，则会返回-1，并且会设置 errno 变量以指示错误类型。

### bind()

`int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

bind()函数用于将一个 IP 地址或端口号与一个套接字进行绑定（将套接字与地址进行关联）。将一个客户端的套接字关联上一个地址没有多少新意，可以让系统选一个默认的地址。一般来讲，会将一个服务器的套接字绑定到一个众所周知的地址---即一个固定的与服务器进行通信的客户端应用程序提前就知道的地址（注意这里说的地址包括 IP 地址和端口号）。因为对于客户端来说，它与服务器进行通信，首先需要知道服务器的 IP 地址以及对应的端口号，所以通常服务器的 IP 地址以及端口号都是众所周知的。

调用 bind()函数将参数 sockfd 指定的套接字与一个地址 addr 进行绑定，成功返回 0，失败情况下返回-1，并设置 errno 以提示错误原因。

参数 addr 是一个指针，指向一个 struct sockaddr 类型变量，如下所示：

```c
struct sockaddr{
    sa_family_t sa_family;
    char sa_data[14];
}
```

第二个成员 sa_data 是一个 char 类型数组，一共 14 个字节，在这 14 个字节中就包括了 IP 地址、端口号等信息，这个结构对用户并不友好，它把这些信息都封装在了 sa_data 数组中，这样使得用户是无法对sa_data 数组进行赋值。事实上，这是一个通用的 socket 地址结构体。

一般我们在使用的时候都会使用 struct sockaddr_in 结构体， sockaddr_in 和 sockaddr 是并列的结构（占用的空间是一样的），指向 sockaddr_in 的结构体的指针也可以指向 sockadd 的结构体，并代替它，而且sockaddr_in 结构对用户将更加友好，在使用的时候进行类型转换就可以了。该结构体内容如下所示：

```c
struct sockaddr_in{
    sa_family_t sa_family;  /*协议族*/
    in_port_t sin_port;     /*端口号*/
    struct in_addr sin_addr;    /*IP*/
    unsigned char sin_zero[8];
}
```

这个结构体的第一个字段是与 sockaddr 结构体是一致的，而剩下的字段就是 sa_data 数组连续的 14 字节信息里面的内容，只不过从新定义了成员变量而已， sin_port 字段是我们需要填写的端口号信息， sin_addr字段是我们需要填写的 IP 地址信息，剩下 sin_zero 区域的 8 字节保留未用。

最后一个参数 addrlen 指定了 addr 所指向的结构体对应的字节长度。

### listen()

`int listen(int sockfd, int backlog);`

listen()函数只能在服务器进程中使用，让服务器进程进入监听状态，等待客户端的连接请求， listen()函数在一般在 bind()函数之后调用，在 accept()函数之前调用

无法在一个已经连接的套接字（即已经成功执行 connect()的套接字或由 accept()调用返回的套接字）上执行 listen()。

参数 backlog 用来描述 sockfd 的等待连接队列能够达到的最大值。在服务器进程正处理客户端连接请求的时候，可能还存在其它的客户端请求建立连接，因为 TCP 连接是一个过程，由于同时尝试连接的用户过多，使得服务器进程无法快速地完成所有的连接请求，那怎么办呢？直接丢掉其他客户端的连接肯定不是一个很好的解决方法。因此内核会在自己的进程空间里维护一个队列，这些连接请求就会被放入一个队
列中，服务器进程会按照先来后到的顺序去处理这些连接请求，这样的一个队列内核不可能让其任意大，所以必须有一个大小的上限，这个 backlog 参数告诉内核使用这个数值作为队列的上限。而当一个客户端的连接请求到达并且该队列为满时，客户端可能会收到一个表示连接失败的错误，本次请求会被丢弃不作处理。

### accept()

`int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

服务器调用 listen()函数之后，就会进入到监听状态，等待客户端的连接请求，使用 accept()函数获取客户端的连接请求并建立连接

为了能够正常让客户端能正常连接到服务器，服务器必须遵循以下处理流程：
①、调用 socket()函数打开套接字；
②、调用 bind()函数将套接字与一个端口号以及 IP 地址进行绑定；
③、调用 listen()函数让服务器进程进入监听状态，监听客户端的连接请求；
④、调用 accept()函数处理到来的连接请求;

accept()函数通常只用于服务器应用程序中，如果调用 accept()函数时，并没有客户端请求连接（等待连接队列中也没有等待连接的请求），此时 accept()会进入阻塞状态，直到有客户端连接请求到达为止。当有客户端连接请求到达时， accept()函数与远程客户端之间建立连接， accept()函数返回一个新的套接字。这个
套接字与 socket()函数返回的套接字并不同， socket()函数返回的是服务器的套接字（以服务器为例），而accept()函数返回的套接字连接到调用 connect()的客户端，服务器通过该套接字与客户端进行数据交互，譬如向客户端发送数据、或从客户端接收数据。

所以，理解 accept()函数的关键点在于它会创建一个新的套接字，其实这个新的套接字就是与执行connect()（客户端调用 connect()向服务器发起连接请求）的客户端之间建立了连接，这个套接字代表了服务器与客户端的一个连接。如果 accept()函数执行出错，将会返回-1，并会设置 errno 以指示错误原因。

参数 addr 是一个传出参数，参数 addr 用来返回已连接的客户端的 IP 地址与端口号等这些信息。参数addrlen 应设置为 addr 所指向的对象的字节长度，如果我们对客户端的 IP 地址与端口号这些信息不感兴趣，可以把 arrd 和 addrlen 均置为空指针 NULL。

### connect()

connect()函数原型如下所示：

`int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

该函数用于客户端应用程序中，客户端调用 connect()函数将套接字 sockfd 与远程服务器进行连接，参数 addr 指定了待连接的服务器的 IP 地址以及端口号等信息，参数 addrlen 指定了 addr 指向的 struct sockaddr对象的字节大小。

客户端通过 connect()函数请求与服务器建立连接，对于 TCP 连接来说，调用该函数将发生 TCP 连接的握手过程，并最终建立一个 TCP 连接，而对于 UDP 协议来说，调用这个函数只是在 sockfd 中记录服务器IP 地址与端口号，而不发送任何数据。

函数调用成功则返回 0，失败返回-1，并设置 errno 以指示错误原因

