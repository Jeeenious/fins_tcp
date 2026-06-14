/*******************************************************************************
 * sender_nodes.hpp — TcpProxySender (hex → TCP)
 ******************************************************************************/

#pragma once

#include <fins/node.hpp>
#include "config.h"
#include "tcp_session.h"

class TcpProxySender : public fins::Node {
public:
  void define() override {
    set_name("TcpProxySender");
    set_description("hex 字符串 → TCP 下发");
    set_category("Contact");
    register_input<std::string>("hex", &TcpProxySender::on_hex);
  }
  void initialize() override {
    logger->info("TcpProxySender 初始化.");
    auto &c = TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT);
    if (!c.is_connected()) logger->warn("TCP 连接失败");
    else logger->info("TCP 已连接 {}:{}", config::PI_IP, config::PI_PORT);
  }
  void pause() override {
    TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT).disconnect();
    logger->info("TcpProxySender 已暂停.");
  }
  void on_hex(const std::string &hex, fins::AcqTime) {
    if (!hex.empty())
      TcpSessionPool::instance().get(config::PI_IP, config::PI_PORT).send_hex(hex);
  }
};
EXPORT_NODE(TcpProxySender)
