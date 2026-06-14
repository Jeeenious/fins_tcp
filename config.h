/*******************************************************************************
 * config.h — TCP 连接 + AA55 帧头常量
 ******************************************************************************/
#pragma once

#include <cstdint>
#include <string>

namespace config {

inline const std::string PI_IP   = "192.168.138.2";
inline constexpr int     PI_PORT = 5000;

inline constexpr uint8_t FRAME_HDR1 = 0xAA;
inline constexpr uint8_t FRAME_HDR2 = 0x55;

}  // namespace config
