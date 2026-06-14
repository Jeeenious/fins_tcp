# fins_tcp — TCP 传输代理插件

TcpProxySender（hex → TCP）+ TcpProxyReceiver（TCP → hex），纯传输，不解析协议。

## 节点

| 节点 | 类型 | 端口 | 功能 |
|------|------|------|------|
| TcpProxySender | 输入 | hex(string) → TCP | 收 hex 字符串，转二进制下发 |
| TcpProxyReceiver | 运行 | TCP → hex(string) | 收 TCP 字节，转 hex 输出 |

## 管线

```
发送:  Encoder(int → hex) → TcpProxySender(hex → TCP) → PI
接收:  PI → TCP → TcpProxyReceiver(TCP → hex) → Decoder(hex → int, 待实现)
```
