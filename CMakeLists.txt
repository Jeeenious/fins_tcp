cmake_minimum_required(VERSION 3.16)

project(fins_tcp LANGUAGES CXX)

fins_add_node(${PROJECT_NAME}
  nodes.cpp
  tcp_session.cpp
)
