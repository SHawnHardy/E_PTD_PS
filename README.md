# E_PTD_PS

[![Build Status](https://travis-ci.org/SHawnHardy/E_PTD_PS.svg?branch=master)](https://travis-ci.org/SHawnHardy/E_PTD_PS)
[![codecov](https://codecov.io/gh/SHawnHardy/E_PTD_PS/branch/master/graph/badge.svg)](https://codecov.io/gh/SHawnHardy/E_PTD_PS)
![Lisence](https://img.shields.io/github/license/SHawnHardy/E_PTD_PS.svg?style=flat)

Effects of partial time delays on phase synchronization in Watts-Strogatz small-world FitzHugh-Nagumo neuronal networks.

## 简介

本项目是

部分时滞诱发 Watts-Strogatz小世界 FitzHugh-Nagumo神经元网络 产生随机多共振

的数值模拟

采用 Python + C++ 的方式

数值运算部分由C++完成

控制，数据可视化由Python处理

## 进度

* ws_network(v0.5) **Testing...**
* time_delay_matrix(v0.3) **Testing...**
* fhn_model(v0.6) **In development...**

## 代码质量

整个项目由[Travis CI](https://travis-ci.org)提供持续集成服务，[Codecov](https://codecov.io)提供单元测试代码覆盖率信息

测试代码(test文件夹)仅针对include文件夹，代码覆盖率信息也仅包含这两个文件夹

### 单个文件的代码质量信息

* ws_network.h
  * 保证在Reconnection_Pr_小于0时为规则网络
  * Reconnection_Pr_大于0时，根据论文：**Eur. Phys. J. B 13, 547–560 (2000)** 
  On the properties of small-world network models 
  (A. Barrat; M. Weigt)
  验证其聚集系数与Reconnection_Pr_的关系符合预期
* time_delay_matrix.h 基本可靠
* fhn_model.h 尚未完成

## 许可信息

除部分明确标明的文件外，均为[MIT许可证](https://opensource.org/licenses/MIT)
