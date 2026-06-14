/*******************************************************************************
 * receiver_nodes.hpp — TcpProxyReceiver (TCP → hex)
 ******************************************************************************/

#pragma once

#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

#include <fins/node.hpp>
#include "config.h"
#include "tcp_session.h"

class TcpProxyReceiver : public fins::Node {
public:
  void define() override {
    set_name("TcpProxyReceiver");
    set_description("TCP → hex 字符串输出");
    set_category("Contact");
    register_output<std::string>("hex");
  }
  void initialize() override {
    logger->info("TcpProxyReceiver 初始化.");
    auto &c = TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT);
    if (!c.is_connected()) logger->warn("TCP 连接失败");
    else logger->info("TCP 已连接 {}:{}", config::PI_IP, config::PI_PORT);
    running_ = false;
  }
  void run() override {
    if (running_) return;
    running_ = true;

    worker_ = std::thread([this]() {
      auto &c = TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT);
      uint8_t buf[1024];
      while (running_) {
        auto n = c.recv(buf, sizeof(buf));
        if (n > 0) {
          std::ostringstream oss;
          oss << std::hex << std::setfill('0');
          for (ssize_t i = 0; i < n; ++i)
            oss << std::setw(2) << static_cast<int>(buf[i]);
          send("hex", oss.str());
        } else if (n < 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } else {
          logger->warn("TCP 连接断开");
          break;
        }
      }
    });
  }
  void pause() override {
    running_ = false;
    if (worker_.joinable()) worker_.join();
    TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT).disconnect();
    logger->info("TcpProxyReceiver 已暂停.");
  }
  void reset() override { logger->info("TcpProxyReceiver 重置."); }
private:
  std::thread worker_;
  std::atomic<bool> running_{false};
};
EXPORT_NODE(TcpProxyReceiver)
