# 改动安全评估

## 风险等级

**风险等级**: 低风险

本次改动涉及两个文件：
- `src/downloaderExtensions/background.js`：移除 `"media"` 请求类型、将 `SUPPORT_MEDIA_EXT_ARRAY` 转为 `Set`、移除 `onBeforeSendHeaders` 中的冗余 `console.log`。均为内部逻辑改动，不改变函数签名，不影响外部调用者。
- `src/src/extensionService/extensionservice.cpp`：在 `timer->stop()` 后添加 `timer->deleteLater()` 修复 QTimer 内存泄漏；为 `waitForFinished()` 添加 5000ms 超时。均为局部内部逻辑修复。

## 历史洞察

`sendUrlToDownloader` 函数由 commit `7e82ab82`（zhaoyue, 2021-04-30）引入，后续 commit `41f24131`（zhaoyue, 2021-06-29）在 code review 修复中加入了 `QTimer *timer = new QTimer;` 模式但未处理 timer 生命周期。commit `14fce9af`（dengzhongyuan, 2026-01-31）将 `proc.start()` 改为 `proc.startDetached()` 但未涉及 timer 泄漏问题。

`runPipeProcess` 中 `process.waitForFinished()` 无超时自函数创建时（commit `7e82ab82`）即存在，无历史修复尝试。

`background.js` 中 `SUPPORT_REQUEST_TYPE_ARRAY` 包含 `"media"` 自扩展功能引入即存在，未被后续 commit 修改。

**是否将撤销历史修复**: 目标行非历史修复产物，本次改动不撤销任何已有修复。

### PMS 关联 bug 检查

目标代码行涉及的 commit message 中未引用其他 PMS bug，无关联 bug 回归风险。

## 风险评估

- **`src/downloaderExtensions/background.js:827`** — `SUPPORT_REQUEST_TYPE_ARRAY`：移除 `"media"` 类型后，流媒体请求不再进入下载拦截管道。其他请求类型（main_frame, sub_frame, object, xmlhttprequest, other）行为不变。**兼容**。
- **`src/downloaderExtensions/background.js:863`** — `isSupportMediaExt`：从线性扫描改为 `Set.has()`，语义等价（精确匹配），仅性能提升。**兼容**。
- **`src/downloaderExtensions/background.js:509`** — `onBeforeSendHeaders`：移除两行 `console.log`，不影响任何逻辑。**兼容**。
- **`src/src/extensionService/extensionservice.cpp:55`** — `runPipeProcess`：`waitForFinished()` 添加 5000ms 超时，防止无限阻塞。调用方 `checkConnection` 在超时后仍能读取已输出的数据，不影响功能。**兼容**。
- **`src/src/extensionService/extensionservice.cpp:120`** — `sendUrlToDownloader`：`timer->deleteLater()` 确保定时器在事件循环中安全释放，不影响定时器在 stop 前的行为。**兼容**。

## 业务影响面分析

### 受影响业务模块

- **浏览器扩展下载拦截** — 处理浏览器请求并判断是否接管下载
  - 涉及调用者：`background.js:onHeadersReceived`、`background.js:onBeforeSendHeaders`
  - 测试范围：验证正常下载（exe、zip、mp4 等文件扩展名）仍能被下载器接管；流媒体视频播放请求不再被拦截
- **扩展服务 WebSocket 连接管理** — 管理浏览器扩展与下载器的 WebSocket 通信
  - 涉及调用者：`extensionservice.cpp:checkConnection`、`extensionservice.cpp:sendUrlToDownloader`
  - 测试范围：验证 URL 传递功能正常；验证长时间运行无内存泄漏

### 建议测试范围

- 浏览器中正常下载各类文件（exe、zip、mp4 等）是否仍被下载器接管
- 浏览器播放在线视频（如腾讯视频）是否正常播放且不触发下载器拦截
- 下载器扩展长时间运行后内存占用是否稳定

### 面向用户的场景

用户在浏览器中播放在线视频时，下载器扩展不再拦截流媒体请求，避免长时间播放导致浏览器崩溃或界面卡死。

## 仓库约定核对

| 约定出处 | 要求 | 本次改动是否涉及 | 处置 |
|---|---|---|---|
| 输入未提供仓库约定 | 无 | 不涉及 | 输入侧未提供共享 spec 或项目 KB 命中项 |

## 建议

低风险，按原方案继续。所有改动均为内部逻辑修复，不改变函数签名或外部行为，无回归风险。
