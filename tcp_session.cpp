/*******************************************************************************
 * tcp_session.cpp — 实现（收发）
 ******************************************************************************/

#include "tcp_session.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>

bool TcpConnection::connect(const std::string &ip, int port) {
  sock_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd_ < 0) return false;

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<uint16_t>(port));
  if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
    ::close(sock_fd_); sock_fd_ = -1; return false;
  }

  // 非阻塞 connect, 超时 2 秒
  int flags = ::fcntl(sock_fd_, F_GETFL, 0);
  ::fcntl(sock_fd_, F_SETFL, flags | O_NONBLOCK);
  int ret = ::connect(sock_fd_, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0 && errno == EINPROGRESS) {
    struct pollfd pfd;
    pfd.fd = sock_fd_;
    pfd.events = POLLOUT;
    ret = ::poll(&pfd, 1, 2000);
    if (ret <= 0) { ::close(sock_fd_); sock_fd_ = -1; return false; }
    int err = 0; socklen_t len = sizeof(err);
    ::getsockopt(sock_fd_, SOL_SOCKET, SO_ERROR, &err, &len);
    if (err) { ::close(sock_fd_); sock_fd_ = -1; return false; }
  } else if (ret < 0) {
    ::close(sock_fd_); sock_fd_ = -1; return false;
  }
  ::fcntl(sock_fd_, F_SETFL, flags);  // 恢复阻塞模式
  std::cout << "[TCP] Connected " << ip << ":" << port << "\n";
  return true;
}

void TcpConnection::disconnect() {
  if (sock_fd_ >= 0) { ::close(sock_fd_); sock_fd_ = -1; }
}

bool TcpConnection::send(const std::vector<uint8_t> &data) {
  if (sock_fd_ < 0) return false;
  return ::send(sock_fd_, data.data(), data.size(), 0) ==
         static_cast<ssize_t>(data.size());
}

bool TcpConnection::send_hex(const std::string &hex) {
  std::vector<uint8_t> bytes;
  for (size_t i = 0; i + 1 < hex.size(); i += 2) {
    unsigned int b;
    if (std::sscanf(hex.c_str() + i, "%2x", &b) == 1)
      bytes.push_back(static_cast<uint8_t>(b));
  }
  return send(bytes);
}

ssize_t TcpConnection::recv(uint8_t *buf, size_t len) {
  if (sock_fd_ < 0) return -1;
  return ::recv(sock_fd_, buf, len, 0);
}

TcpConnection &TcpSessionPool::get(const std::string &ip, int port) {
  std::string key = ip + ":" + std::to_string(port);
  std::lock_guard<std::mutex> lk(mtx_);
  auto &conn = pool_[key];
  if (!conn.is_connected())
    conn.connect(ip, port);
  return conn;
}
