/*******************************************************************************
 * tcp_session.h — TCP 连接池（收发）
 ******************************************************************************/
#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

class TcpConnection {
public:
  bool connect(const std::string &ip, int port);
  void disconnect();
  bool is_connected() const { return sock_fd_ >= 0; }
  bool send(const std::vector<uint8_t> &data);
  bool send_hex(const std::string &hex);
  ssize_t recv(uint8_t *buf, size_t len);

private:
  int sock_fd_ = -1;
};

class TcpSessionPool {
public:
  static TcpSessionPool &instance() { static TcpSessionPool s; return s; }
  TcpConnection &get(const std::string &ip, int port);

private:
  std::mutex mtx_;
  std::unordered_map<std::string, TcpConnection> pool_;
};
