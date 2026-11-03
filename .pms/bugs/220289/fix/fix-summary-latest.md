# 根因分析与修复方案（2026-11-03 18:15）

**方案层级**: 根因层修复 + 症状层补丁联合实施

## 根因分析

下载器浏览器扩展以 blocking 模式拦截所有网络请求（`urls: ["<all_urls>"]`），且 `SUPPORT_REQUEST_TYPE_ARRAY` 包含 `"media"` 类型，导致每个流媒体视频分片请求都进入下载拦截管道。12-24 小时连续视频播放期间，逐请求 `console.log` 输出、blocking 拦截开销以及 C++ 后端 `sendUrlToDownloader` 中 `QTimer` 内存泄漏累积，耗尽资源导致浏览器崩溃或界面卡死。

关键证据：`background.js:503-504`（blocking 监听器）、`background.js:827`（`"media"` 在拦截类型中）、`extensionservice.cpp:111-120`（QTimer 泄漏）；PMS 历史 #4 实验确认卸载扩展可消除崩溃。

## 修复概述

根因层：从 `SUPPORT_REQUEST_TYPE_ARRAY` 移除 `"media"` 使流媒体请求绕过下载拦截，将 `SUPPORT_MEDIA_EXT_ARRAY` 转为 `Set` 提升 O(1) 查找性能。症状层：移除 `onBeforeSendHeaders` 逐请求 `console.log`，修复 `sendUrlToDownloader` QTimer 内存泄漏（添加 `deleteLater`），为 `runPipeProcess` 添加 `waitForFinished` 超时。

## 实际改动

- 文件：`src/downloaderExtensions/background.js`，改动：移除 `SUPPORT_REQUEST_TYPE_ARRAY` 中的 `"media"` 类型；将 `SUPPORT_MEDIA_EXT_ARRAY` 转为 `Set` 并简化 `isSupportMediaExt` 为 `Set.has()`；移除 `onBeforeSendHeaders` 中两行 `console.log`
- 文件：`src/src/extensionService/extensionservice.cpp`，改动：`sendUrlToDownloader` 中 `timer->stop()` 后添加 `timer->deleteLater()`；`runPipeProcess` 中 `waitForFinished()` 添加 5000ms 超时

## 与分析报告方案的偏离

本次修复与阶段 2 分析报告方案一致。

## 影响范围

- **浏览器扩展下载拦截**：正常文件下载仍被下载器接管；流媒体视频播放不再触发下载拦截。涉及 `background.js:onHeadersReceived`、`background.js:onBeforeSendHeaders`
- **扩展服务 WebSocket 连接管理**：`sendUrlToDownloader` 不再泄漏 QTimer；`checkConnection` 的 `runPipeProcess` 不再无限阻塞。涉及 `extensionservice.cpp:sendUrlToDownloader`、`extensionservice.cpp:checkConnection`

复现状态为"未建立"（x86_64 无法构造 ARM 云桌面压测环境），改后验证为 diff 逐块对账（载体与项目代码脱耦，改后不适用）。

## 验证建议

测试正常文件下载（exe、zip、mp4 等）仍能被下载器接管；测试在线视频播放（如 v.qq.com）正常播放且不触发下载拦截；测试浏览器长时间运行（12 小时以上）内存稳定性。
