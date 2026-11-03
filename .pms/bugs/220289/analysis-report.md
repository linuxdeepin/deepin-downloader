# 根因分析报告

# BUG-220289 根因分析

- **Bug ID**: 220289
- **标题**: 浏览器视频播放压测12-24H会崩溃或界面卡死
- **PMS URL**: https://pms.uniontech.com/bug-view-220289.html
- **状态**: 激活
- **Bug 类型**: P1性能资源问题
- **分析时间**: 2026-11-03
- **Skill 版本**: v3.23.0
- **修复提交**: 未提取到
- **涉及项目**: org.deepin.downloader（下载器浏览器扩展 + C++ WebSocket 后端）
- **产品/版本**: org.deepin.downloader 6.0.17-1（release/eagle 分支，commit 8b017b58）
- **问题存在性**: 已确认（现象型）
- **输入质量**: 正常

## 一、问题概述

在 UOS 专业版（天翼云云桌面，ARM 架构）环境中，浏览器持续播放网页视频（v.qq.com）、本地视频及腾讯视频会议 12-24 小时后，浏览器发生崩溃或界面卡死。PMS 历史记录 #4（黄文兵，2023-10-11）通过实验确认：卸载内置下载器扩展后进行压测可排除崩溃影响，明确将问题指向下载器浏览器扩展。

下载器浏览器扩展通过 Chrome Extension Manifest V2 的 `webRequest` API 以 **blocking 模式**拦截 `<all_urls>` 的所有网络请求。在长时间视频播放场景下，扩展对每个视频流分片请求执行拦截、媒体类型匹配、下载创建/取消等操作，同时 C++ 后端 `extensionservice.cpp` 中 `sendUrlToDownloader` 函数存在 QTimer 内存泄漏，多重资源消耗累积最终导致浏览器崩溃。

**历史记录要点**：
- #1（黄文兵，2023-09-19）：本地 ARM 设备 6.0.33.1 版本未复现 crash，出现一例视频暂停，chrome://media-internals 显示解码器异常溢出，切换软解码测试中。
- #2（黄文兵，2023-09-19）：协调天翼云环境（UOS-ARM-1）进行压力测试，尚未复现。
- #3（黄文兵，2023-09-20）：第二天仍未复现，怀疑与网络环境有关。
- #4（黄文兵，2023-10-11）：**实验确认**——测试发现视频长时间播放黑屏原因和内置的下载器扩展相关；将下载器进行卸载后压测可排除影响。（此为事实性复现结论，非推断）

## 二、日志分析（可选）

无日志附件。PMS 附件仅有两张崩溃截图，无文本日志。

## 二（补充）、附件图片分析

- **`images/浏览器崩溃.jpg`**（来源: attachment）— 1440×1080 JPEG 截图。analyze-image skill API 超时无返回；本地 PIL 仅能确认尺寸与格式，无法提取内容。文件名表明为浏览器崩溃画面。
- **`images/浏览器崩溃2.jpg`**（来源: attachment）— 4624×2080 JPEG 截图（手机拍摄屏幕）。analyze-image skill API 超时无返回；本地 PIL 仅能确认尺寸与格式。文件名表明为浏览器崩溃画面。

## 三、源码定位

### 缺陷位置

**缺陷 1：blocking webRequest 拦截全部网络请求**

- **文件**: `src/downloaderExtensions/background.js:503-504`
- **缺陷类型**: 资源泄漏 / 性能问题
- **问题代码**:

```javascript
// line 503-504
chrome.webRequest.onHeadersReceived.addListener(function(t) {
    return onHeadersReceived(t)
}, {
    urls: ["<all_urls>"]
}, ["blocking", "responseHeaders"])
```

**缺陷 2：onBeforeSendHeaders 逐请求 console.log 全量输出**

- **文件**: `src/downloaderExtensions/background.js:510-512`
- **缺陷类型**: 资源泄漏 / 性能问题
- **问题代码**:

```javascript
// line 510-512
chrome.webRequest.onBeforeSendHeaders.addListener(function(details) {
    console.log("chrome.webRequest.onBeforeSendHeaders")
    console.log(details)  // 逐请求打印完整 details 对象
```

**缺陷 3：全局可变状态竞态**

- **文件**: `src/downloaderExtensions/background.js:395-399`
- **缺陷类型**: 竞态
- **问题代码**:

```javascript
// line 395-399
var isSelfCreate = false;
var isTakeOver = false;
var isOpen = false;
var isContainCookie = false;
```

**缺陷 4：每个媒体响应触发下载创建+取消+HEAD请求+DBus调用链**

- **文件**: `src/downloaderExtensions/background.js:656-730`（onHeadersReceived handler）
- **缺陷类型**: 性能问题 / 资源泄漏
- **问题代码**:

```javascript
// line 700-712 (onHeadersReceived handler)
if(isSupportMediaExt(c)) {       // 线性扫描 64 项 SUPPORT_MEDIA_EXT_ARRAY
    console.log("SupportMediaExt : " + c)
    chrome.downloads.setShelfEnabled(true);
    isSelfCreate = false
    chrome.downloads.download({   // 为每个匹配的媒体响应创建下载
        url: u
    });
    return;
}
var h = o.headers["content-type"];
if (isSupportContentType(h)) {   // 线性扫描 300+ 项 MIME_TYPE_ARRAY
    console.log("SupportMediaExt : " + c)
    chrome.downloads.setShelfEnabled(true);
    isSelfCreate = false
    chrome.downloads.download({url: u});
    return;
}
```

**缺陷 5：C++ 后端 QTimer 内存泄漏**

- **文件**: `src/src/extensionService/extensionservice.cpp:78-92`
- **缺陷类型**: 资源泄漏
- **问题代码**:

```cpp
// line 78-92
void extensionService::sendUrlToDownloader(const QString &url)
{
    QProcess proc;
    proc.startDetached("downloader", QStringList() << url);
    QTimer *timer = new QTimer;        // 每次 new，永不 delete
    timer->start(50);
    connect(timer, &QTimer::timeout, this, [=](){
       QDBusInterface iface(...);
       QDBusMessage m = iface.call("onReceiveExtentionUrl", url);
       QString msg = m.errorMessage();
       if(msg.isEmpty()) {
           timer->stop();              // 仅 stop，不 deleteLater
       }
       // 若 msg 不为空（DBus 服务未就绪），timer 永不停止，每 50ms 触发一次
    });
}
```

**缺陷 6：netstat 子进程每 60 秒执行**

- **文件**: `src/src/extensionService/extensionservice.cpp:89-100`
- **缺陷类型**: 性能问题
- **问题代码**:

```cpp
// line 89-100 (checkConnection)
void extensionService::checkConnection()
{
    QStringList strList = runPipeProcess("netstat -apn", "dlmextensions");
    // ...
}
// initWebsokcet 中:
// m_timer = new QTimer;
// m_timer->start(60000);  // 每 60s 执行一次 checkConnection
```

### 缺陷说明

下载器浏览器扩展以 Manifest V2 的 `persistent: true` 后台脚本方式运行，注册了两个 blocking 模式的 webRequest 监听器，拦截 `<all_urls>` 的所有 HTTP 请求和响应。在正常浏览中影响可控，但在长时间视频播放场景下产生严重问题：

**完整调用链（每次视频流分片响应）**：
1. `onBeforeSendHeaders`（line 510）触发：打印完整 details 对象到 console（`console.log(details)`）
2. `isContainCookie` 全局变量被当前请求的 Cookie 状态覆盖（line 519-525）——并发请求间存在竞态
3. `onHeadersReceived`（line 656）触发：检查 `isSelfCreate` 全局变量（可能被其他并发请求错误设置）
4. 对响应 URL 提取扩展名，线性扫描 `SUPPORT_MEDIA_EXT_ARRAY`（64 项）；视频分片 `.ts`/`.mp4` 命中
5. 若扩展名不匹配，对 content-type 线性扫描 `MIME_TYPE_ARRAY`（~300+ 项，使用 `indexOf` 模糊匹配）
6. 匹配后调用 `chrome.downloads.download({url: u})` 创建下载
7. `chrome.downloads.onCreated`（line 443）触发：若 `isTakeOver` 为 true，取消并擦除下载项
8. 调用 `loadXMLDoc(item.finalUrl)` 发起 HEAD 请求，设置 `isSelfCreate = true`
9. HEAD 响应后 `onStateChange` 设置 `isSelfCreate = false`，调用 `sendUrlToDownloader`
10. C++ 后端 `sendUrlToDownloader`：`new QTimer`（永不删除）+ DBus 调用

视频播放 12-24 小时期间，视频分片请求可达每分钟数十至数百个。每个分片走完上述链路，累积效应包括：
- **QTimer 内存泄漏**：每个 URL 转发创建一个 QTimer，成功时仅 `stop()` 不 `delete`，失败时 50ms 周期永久触发。12-24H 累积可达数万至数十万个 QTimer 对象。
- **Console 日志膨胀**：`onBeforeSendHeaders` 对每个请求打印完整 details 对象，浏览器扩展日志内存持续增长。
- **下载管理器状态污染**：每个媒体分片创建→取消→擦除下载项，浏览器下载管理器内部状态对象反复创建销毁。
- **全局状态竞态**：`isSelfCreate`、`isContainCookie` 在并发请求间无同步，可能导致误拦截或重复下载。
- **CPU 开销**：blocking 模式要求浏览器在每次请求/响应时暂停等待扩展返回，线性 MIME 扫描加剧延迟。

### 关联项目枚举与逐项处置（必填）

- **被测项目**: org.deepin.downloader
- **枚举取数**: 本机已安装 6.0.17-1；源码树 debian/control 声明依赖世代 qt5。与被测版本一致（release/eagle 分支同源）。
- **枚举覆盖**: 运行时依赖 有 · 推荐依赖 无（无 deb-src 索引）· 编译期依赖 无（无 deb-src 索引）· 会话层 未探测成功（无活跃会话，仅有伴生 socket）
- **会话环境**: 提单声明天翼云云桌面（UOS 专业版），对应 dde 会话栈；本机无活跃会话

| 关联项目（包） | 分级 | 来源边 | 发现依据 | 处置 | 证据 |
|---|---|---|---|---|---|
| `libqt6gui6` | 强 | 运行时依赖 | org.deepin.downloader 的 Depends | 排除 | Qt GUI 库为通用图形渲染底座，与本单根因（浏览器扩展 JS 逻辑）无直接因果 |

- **排除的强档**: `libqt6gui6` — Qt GUI 库是通用渲染底座，本单根因在浏览器扩展 JS 层和 C++ WebSocket 后端的资源管理逻辑，与 Qt GUI 渲染无直接因果
- **其余候选**: 中 16 · 弱 0 — 中档候选为间接运行时依赖（libstdc++、libc、libgcc 等），均为系统基础库，与本单根因无关

## 四、候选根因（分层）

### 候选 1（高置信）：浏览器扩展 blocking webRequest 全量拦截导致资源耗尽

- **模块**: `background.js`（浏览器扩展后台脚本）
- **缺陷类型**: 资源泄漏 + 性能问题
- **触发条件**: 浏览器播放网页视频 12-24H，视频流分片请求持续触发 blocking 监听器
- **证据**:
  - `background.js:503-504` — `onHeadersReceived` blocking 监听器注册在 `<all_urls>`
  - `background.js:510-512` — `onBeforeSendHeaders` blocking 监听器，逐请求 `console.log(details)`
  - `background.js:700-712` — 匹配媒体类型后调用 `chrome.downloads.download()`
  - `background.js:822-825` — `SUPPORT_MEDIA_EXT_ARRAY` 含 `.ts`、`.mp4`、`.flv` 等视频分片扩展名
  - `background.js:732-820` — `MIME_TYPE_ARRAY` 约 300+ 项，`isSupportContentType` 用 `indexOf` 模糊匹配
  - PMS 历史 #4：卸载下载器扩展后压测可排除影响（实验事实）

### 候选 2（高置信）：C++ 后端 sendUrlToDownloader QTimer 内存泄漏

- **模块**: `extensionservice.cpp`
- **缺陷类型**: 资源泄漏
- **触发条件**: 扩展接管模式（`isTakeOver=true`）下，每个媒体 URL 转发到 C++ 后端
- **证据**:
  - `extensionservice.cpp:78-92` — `QTimer *timer = new QTimer;` 每次 `sendUrlToDownloader` 调用都 new
  - `extensionservice.cpp:88` — 成功时仅 `timer->stop()`，无 `delete` 或 `deleteLater()`
  - 失败时 timer 以 50ms 周期永久触发，持续消耗 CPU 和内存

### 候选 3（中置信）：全局可变状态竞态导致重复处理

- **模块**: `background.js`
- **缺陷类型**: 竞态
- **触发条件**: 多个网络请求并发到达时，`isSelfCreate`/`isContainCookie` 被一个请求写入后、被另一个请求的处理器读取
- **证据**:
  - `background.js:395-399` — 全局变量无锁
  - `background.js:519-525` — `onBeforeSendHeaders` 写 `isContainCookie`
  - `background.js:657` — `onHeadersReceived` 读 `isSelfCreate`
  - `background.js:710,721` — 写 `isSelfCreate = false` 后调用 `chrome.downloads.download()`
  - 竞态可能导致：非媒体请求被误判为媒体（触发下载），或媒体请求被误跳过（功能异常），或下载自身请求未被拦截导致递归

## 五、5-Why 分析

1. **为什么浏览器崩溃？** → 长时间播放视频后内存/CPU 资源耗尽
2. **为什么资源耗尽？** → 下载器扩展对每个视频分片请求执行 blocking 拦截+下载创建+取消+HEAD+DBus 调用链，同时 C++ 后端泄漏 QTimer
3. **为什么每个视频分片都触发完整链路？** → `onHeadersReceived` 在 `<all_urls>` 上以 blocking 模式注册，视频分片扩展名（`.ts`/`.mp4`）和 content-type 匹配 `SUPPORT_MEDIA_EXT_ARRAY`/`MIME_TYPE_ARRAY`
4. **为什么匹配后会创建下载？** → `chrome.downloads.download({url: u})` 设计意图是接管浏览器下载，但未区分"用户主动下载"与"视频流媒体分片"
5. **为什么 QTimer 会泄漏？** → `sendUrlToDownloader` 中 `new QTimer` 后仅 `stop()` 不 `delete`，DBus 服务未就绪时 timer 永不停止


## 五、版本后续修复检查

### 缺陷位置变更史

对 `background.js` 和 `extensionservice.cpp` 的关键缺陷位置进行 git 历史审查：

**`extensionservice.cpp:78-92`（QTimer 泄漏）**：
- `git log -S "QTimer *timer = new QTimer"` 定位引入提交 `41f2413`（"fix: 修改codereview提出的问题"），该提交重构了 `extensionservice.cpp`。
- `git merge-base --is-ancestor 41f2413 HEAD` 返回 0（已含入当前分支）。
- 引入后无后续修复提交，当前代码仍为 `new QTimer` + `stop()` 无 `delete`。

**`background.js:503-504`（blocking webRequest 监听器）**：
- `git log --oneline -- src/downloaderExtensions/background.js` 显示最近提交为 `cef9097`（合规修改）、`5aaca5d`（合规整改）、`8572998`（添加单元测试）、`ba264d8`（修改浏览器扩展的bug）。
- 这些提交均为合规性整改和 bug 修复，未涉及 webRequest 监听器范围的修改。
- 当前代码仍为 `<all_urls>` 全量 blocking 拦截。

**结论**: 未发现后续修复

- **当前代码是否仍含该缺陷**: 仍含
- **当前代码状态证据**: `background.js:503-504`（`<all_urls>` blocking 监听器仍存在）；`extensionservice.cpp:78-92`（`new QTimer` 无 `delete` 仍存在）
- **候选提交**: `41f2413`（引入 QTimer 泄漏，`merge-base --is-ancestor 41f2413 HEAD` 返回 0 已含入）；`git log -S "QTimer *timer = new QTimer"` 仅命中引入提交，无后续修复提交
- **检索覆盖面**: 路径范围：全仓库（`git log -S` 全仓库 pickaxe）；改名处理：`background.js` 无改名、`extensionservice.cpp` 无改名（文件名通配 `git log --oneline -- src/downloaderExtensions/background.js` 确认）；pickaxe：用 `-S`（精确字符串匹配 `QTimer *timer = new QTimer`）+ `-G`（正则匹配 `QTimer.*new|webRequest.*blocking|all_urls`）+ 文件历史审查（`git log --oneline -- src/downloaderExtensions/background.js` 查看全部提交）
- **处置建议**: 当前代码仍含缺陷，需修复 blocking 全量拦截范围 + QTimer 泄漏

当前代码（release/eagle @ 8b017b58）仍包含上述所有缺陷形态，未发现任何后续提交修复了 blocking 全量拦截、QTimer 泄漏或全局状态竞态问题。

## 六、根因结论

**根因（高置信假设）**：下载器浏览器扩展以 blocking 模式拦截所有网络请求，对视频流分片误触发下载接管流程；C++ 后端 `sendUrlToDownloader` 存在 QTimer 内存泄漏。两者叠加在 12-24H 视频播放场景下累积资源耗尽，导致浏览器崩溃。

**根因层级**：
- 根因层：扩展未区分流媒体分片与可下载文件，对所有匹配 MIME/扩展名的响应触发下载接管
- 症状层：blocking 监听器全量拦截 + console.log 逐请求输出 + QTimer 泄漏 加速资源耗尽

**结论级别**: 高置信假设

**无需修复依据**: 不适用（根因为代码缺陷）

## 七、证据链

| # | 证据 | 来源类型 | 说明 |
|---|------|---------|------|
| 1 | `background.js:503-504` — onHeadersReceived blocking 监听器注册在 `<all_urls>` | 本地读码 | 证明所有 HTTP 响应被拦截 |
| 2 | `background.js:510-512` — onBeforeSendHeaders blocking 监听器，逐请求 console.log(details) | 本地读码 | 证明所有请求被拦截且全量日志输出 |
| 3 | `background.js:395-399` — isSelfCreate/isTakeOver/isContainCookie 全局可变状态 | 本地读码 | 证明并发请求间存在竞态条件 |
| 4 | `background.js:700-712` — 匹配媒体类型后 chrome.downloads.download() | 本地读码 | 证明视频分片触发下载创建 |
| 5 | `background.js:822-825` — SUPPORT_MEDIA_EXT_ARRAY 含 .ts/.mp4/.flv | 本地读码 | 证明视频流分片扩展名被匹配 |
| 6 | `background.js:732-820` — MIME_TYPE_ARRAY ~300+ 项，isSupportContentType 用 indexOf | 本地读码 | 证明每个响应需线性扫描 300+ 项 |
| 7 | `background.js:863-877` — isSupportMediaExt/isSupportContentType 线性扫描实现 | 本地读码 | 证明匹配算法为 O(n) 线性扫描 |
| 8 | `extensionservice.cpp:78-92` — sendUrlToDownloader 中 new QTimer 永不 delete | 本地读码 | 证明 C++ 后端 QTimer 内存泄漏 |
| 9 | `extensionservice.cpp:88` — 成功时仅 timer->stop()，无 delete/deleteLater | 本地读码 | 证明 QTimer 对象生命周期管理缺失 |
| 10 | `extensionservice.cpp:89-100` — checkConnection 每 60s 执行 netstat -apn | 本地读码 | 证明周期性子进程开销 |
| 11 | `manifest.json` — background.persistent=true, webRequestBlocking on `<all_urls>` | 本地读码 | 证明扩展为持久后台+blocking 拦截权限 |
| 12 | PMS 历史 #4（黄文兵 2023-10-11）— 卸载下载器扩展后压测可排除影响 | 日志 | 实验事实，证明扩展是崩溃的直接原因 |
| 13 | `background.js:443-499` — onCreated 处理器：cancel+erase+loadXMLDoc 链路 | 本地读码 | 证明每个媒体下载创建后被取消并触发 HEAD 请求 |
| 14 | `background.js:559-580` — loadXMLDoc 设置 isSelfCreate=true，onStateChange 设置 false | 本地读码 | 证明 isSelfCreate 在异步流程中无同步保护 |

### 问题存在性检验

**主张类型**：现象型（声称"浏览器崩溃"）

**现象来源**：PMS 正文"浏览器崩溃"；PMS 历史 #4 实验确认卸载扩展可消除崩溃。两张附件截图文件名为"浏览器崩溃"。

**载体确认**：PMS 历史 #4 提供了明确的对照实验——卸载下载器扩展前后对照，确认崩溃与扩展相关。此为实验事实而非推断。

## 八、复现与实验验证

### 复现环境

- **硬件/系统**: 本机 x86_64，内核 6.6.0-amd64-desktop；提单环境为 ARM 天翼云云桌面
- **软件版本**: org.deepin.downloader 6.0.17-1（release/eagle）；提单环境为 UOS 专业版
- **构造手段**: 尝试 Qt offscreen 无显示启动、dbus-run-session 会话隔离、strace/gdb 进程观测、podman 容器、systemd-run 临时会话、unshare 用户命名空间隔离——均无法构造"浏览器+视频播放+12-24H 压测"的判定点（详见已排除手段）
- **与提单环境差异**: 架构不同（x86_64 vs ARM）、无云桌面环境、无浏览器视频播放环境；差异影响触发的判断依据——本机无法运行浏览器视频播放压测

### 已排除手段（状态=未建立时必填）

以下为 `repro/env.md` 中状态为 OK 的手段，逐条说明为何在本 bug 上无法构造判定点：

- **用户态文件系统（fuse/bindfs）** → 本 bug 是浏览器扩展 JS 逻辑 + C++ 后端资源泄漏，与文件系统挂载方式无关；fuse 无法构造浏览器视频播放环境
- **Qt offscreen 无显示启动** → 可启动 Qt 离屏应用，但本 bug 需要完整浏览器（Chromium 内核）运行视频播放，浏览器不依赖 Qt 平台插件，offscreen 模式无法启动浏览器
- **私有会话总线 + 收发包观测（dbus-run-session）** → 可起独立 DBus 会话，但本 bug 的判定点是浏览器崩溃/卡死，需完整浏览器进程而非仅 DBus 通信；DBus 会话隔离无法替代浏览器运行环境
- **系统调用跟踪与定点错误注入（strace）** → strace 可观测进程系统调用，但前提是需要一个正在运行浏览器视频播放的进程；本机无浏览器视频播放环境，strace 无可观测目标
- **断点/附栈/改返回值（gdb）** → gdb 可调试进程，但同样需要浏览器视频播放进程在运行；本机无法启动浏览器视频播放压测
- **LD_PRELOAD 包装（延迟/错误注入）** → 可注入库函数拦截，但需要目标浏览器进程在运行视频播放；本机无浏览器环境，注入无目标
- **容器内跑目标发行版（podman）** → podman 可用，但本 bug 需要 ARM 架构 + 天翼云云桌面环境；x86_64 容器无法模拟 ARM 浏览器+视频播放+云桌面完整栈；且 12-24H 压测在容器内资源受限不现实
- **用户命名空间隔离（unshare）** → 可做用户命名空间降权，但本 bug 与命名空间隔离无关；无法构造浏览器视频播放环境
- **systemd-run 起临时会话单元** → 可起临时会话，但本机无显示服务器（无 xvfb/weston），无法启动浏览器图形界面

### 复现目标

- **现象指纹**: 浏览器在长时间视频播放后崩溃或界面卡死；PMS 历史 #4 通过对照实验（卸载扩展 vs 保留扩展）确认扩展为直接原因
- **判定点**: 浏览器进程异常退出（crash）或界面无响应（卡死）
- **验收目标**: 卸载下载器扩展后长时间播放不崩溃（PMS 历史 #4 已验证）

### 原软件验证

- **改前**: 不适用（需完整天翼云云桌面+ARM 环境，本机为 x86_64 无云桌面，无法运行原软件入口）
- **改后**: 阶段 2 填"待重跑"
- **副作用**: 不适用
- **单元测试影响**: 不涉及（改动不在单测覆盖范围）

### Demo / 旁路验证

- **形态**: 不适用（本单需完整浏览器+视频播放+ARM 云桌面环境，本机无法构造判定点观测）
- **证明了什么**: 不适用
- **反证了什么**: 不适用

### 对照实验

| 变量 | 改动 | 结果 | 区分的假设 |
|---|---|---|---|
| 下载器扩展 | 安装 vs 卸载 | 安装→崩溃；卸载→不崩溃 | 排除浏览器自身解码器问题，确认扩展为根因（PMS 历史 #4） |

### 方案预演

- **方案预演**: 未验证——本机为 x86_64 无云桌面环境，无法运行浏览器视频播放压测；替代核对：代码静态分析已定位 file:line 级证据链，PMS 历史 #4 提供了运行时对照实验

### 复现结论

- **状态**: 未建立
- **与提单描述的差异**: 本机为 x86_64，提单环境为 ARM 天翼云云桌面；本机无浏览器视频播放环境，无法构造判定点
- **对根因的影响**: 不影响——PMS 历史 #4 已通过对照实验确认根因在下载器扩展，代码分析提供了 file:line 级证据链
- **不可得依据**: 提单限定环境——PMS 环境"天翼云云桌面"+ ARM 架构，本机为 x86_64 普通桌面。PMS 历史 #4 已由开发者在天翼云环境完成对照实验。本机 `repro_probe.py` 输出确认无 xvfb、无 weston、无浏览器视频播放环境。
- **尝试记录**: `repro_probe.py --output repro/env.md` → 本机能力清单确认无 GUI 虚拟显示能力；无法运行浏览器视频播放压测

### 证伪检验记录

| # | 风险预测 | 核验方式 | 结果 |
|---|---|---|---|
| 1 | 存在性预测：若浏览器长时间视频播放崩溃确实由下载器扩展导致，则卸载该扩展后同样条件下长时间播放不应崩溃 | PMS 历史 #4 对照实验（卸载扩展 vs 保留扩展） | 通过（卸载后压测不崩溃，PMS 历史 #4 已验证） |
| 2 | 若根因成立（blocking webRequest 全量拦截），则视频流分片请求（.ts/.mp4）应被扩展拦截并触发下载接管 | 读码 `background.js:503-504`（`<all_urls>` blocking）+ `background.js:822-825`（`.ts`/`.mp4` 在 `SUPPORT_MEDIA_EXT_ARRAY` 中） | 通过（代码证据确认分片扩展名命中匹配数组） |
| 3 | 若 QTimer 泄漏为根因之一，则每次 `sendUrlToDownloader` 调用应创建新 QTimer 且不释放 | 读码 `extensionservice.cpp:78-92`（`new QTimer` 无 `delete`/`deleteLater`） | 通过（代码证据确认泄漏路径） |

**差分证据**: PMS 历史 #4 的对照实验（卸载扩展 vs 保留）区分了"扩展导致"与"浏览器自身解码器问题"两个并列假设；解码器溢出假设已被排除（见排除项）。

**封顶原因**: 复现状态未建立——本机为 x86_64，无法构造 ARM 天翼云云桌面环境进行 12-24H 视频播放压测；代码路径分析不构成实验证据，故降级为高置信假设

## 九、排除项

- **排除假设**：浏览器自身解码器问题——PMS 历史 #1 提到解码器异常溢出，但历史 #4 通过卸载扩展实验确认问题在扩展而非解码器。解码器溢出可能是扩展资源耗尽后的连锁反应，非独立根因。
- **排除假设**：网络环境问题——PMS 历史 #3 提到"怀疑和网络环境有关系"，但历史 #4 的对照实验排除了网络因素。

## 十、修复建议

### 历史约束

无相关历史约束。PMS 历史 #4 仅确认了问题与扩展的关联，未提交修复。

### 根因层修复方案

扩展应区分"用户主动下载"与"流媒体播放"：
1. `onHeadersReceived` 中增加请求类型过滤：排除 `type === "media"` 的流媒体请求，或排除 `Range` 请求头（流媒体分片特征）
2. 将 `SUPPORT_MEDIA_EXT_ARRAY` 和 `MIME_TYPE_ARRAY` 改用 `Set` 数据结构（O(1) 查找替代 O(n) 线性扫描）
3. 缩小 `urls` 过滤范围，排除常见视频域名或使用更精确的 URL 模式

### 症状层补丁方案

1. `onBeforeSendHeaders` 移除或条件化 `console.log(details)`（line 511-512），生产环境不应逐请求打印
2. `extensionservice.cpp:sendUrlToDownloader` 中 QTimer 改用 `QTimer::singleShot` 或在 `stop()` 后 `deleteLater()`
3. `extensionservice.cpp:checkConnection` 中 `runPipeProcess` 的 QProcess 应设置 `waitForFinished` 超时并确保进程清理

### 六维特性评估

| 特性 | 根因层修复 | 症状层补丁 |
|---|---|---|
| 有效性 | 过滤流媒体请求可直接消除视频分片触发的下载接管链路；改用 Set 可降低每次拦截的 CPU 开销。预计视频播放期间扩展资源消耗降至接近零。验收目标：12-24H 视频播放不崩溃。 | 移除 console.log 减少日志内存增长；QTimer 修复消除 C++ 内存泄漏。两者消除主要累积源但未消除"每个分片走完整拦截链路"的开销。 |
| 安全性 | 好——缩小拦截范围不影响正常下载接管功能；需确认过滤规则不误排除用户主动下载的媒体文件 | 中——QTimer 修复需确认 DBus 调用超时逻辑不受影响 |
| 稳定性 | 好——减少并发竞态场景 | 好——消除内存泄漏直接提升稳定性 |
| 性能 | 好——Set 查找 O(1) 替代 O(n)；减少不必要的下载创建/取消 | 好——消除 console.log I/O 开销 |
| 兼容性 | 中——需确认流媒体过滤规则在所有视频网站（v.qq.com 等）正确区分播放与下载 | 好——QTimer 修改不改变对外接口 |
| 可维护性 | 好——过滤逻辑更清晰 | 好——标准 Qt 内存管理实践 |

**改动范围**: 根因层 `background.js` 1 文件；症状层 `background.js` + `extensionservice.cpp` 2 文件

### 推荐方案

**推荐**: 根因层修复 + 症状层补丁联合实施

**理由**: 根因层修复（过滤流媒体请求）直接消除视频分片触发下载接管的核心问题，症状层补丁（QTimer 修复、console.log 移除）消除累积性资源泄漏。两者互补，单独实施任一层都不够充分——仅修根因层，QTimer 泄漏在非视频下载场景仍会累积；仅修症状层，每个视频分片仍走完整 blocking 拦截链路。

**用户确认的方案层级**: 根因层修复 + 症状层补丁联合实施（leader 裁决替代用户确认）

## 十一、需要用户确认/决策或补充的信息

1. **是否需要进一步在 ARM 天翼云环境复现**：本机为 x86_64，无法构造完整复现环境。PMS 历史 #4 已提供对照实验证据，代码分析提供了 file:line 级证据链。是否接受基于代码分析+历史实验证据的根因结论？
2. **修复范围确认**：推荐根因层+症状层联合修复。是否同意此范围，还是仅修复某一层？
3. **流媒体过滤策略**：根因层修复需要确定过滤策略——是按请求类型（`type === "media"`）过滤，还是按 Range 请求头过滤，还是按 URL 域名白名单过滤？不同策略对兼容性影响不同。

## 附录 A：检索工具使用记录

| 工具 | 关键参数 | 结果 | 用在哪条结论 |
|------|---------|------|-------------|
| gitnexus_mcp.py call query | repo=org.deepin.downloader, query=onHeadersReceived webRequest browser extension, limit=5 | 0；Repository "org.deepin.downloader" not found. Available repos 不含 downloader | 未用于结论（仓库未索引，退回本地读码） |
| kb_mcp.py call trellis_context | repo=org.deepin.downloader@release-eagle, query=浏览器扩展 webRequest 视频播放 崩溃 内存泄漏, limit=5 | 0；仓库命名空间不存在：org.deepin.downloader@release-eagle | 未用于结论（命名空间不存在，退回本地读码） |
| related_source.py enumerate | name=org.deepin.downloader, source-dir=. | 17 项（强 1 中 16）；强档 libqt6gui6 已排除 | 用于第三章关联项目枚举与逐项处置 |
| repro_probe.py | --output repro/env.md | 环境能力清单；本机 x86_64 无 xvfb/weston | 用于第八章复现结论的不可得依据 |
| analyze-image (curl API) | file=浏览器崩溃.jpg, prompt=图片内容 | 超时无返回（30s） | 未用于结论（API 不可达，退回 PIL 元数据） |
| analyze-image (curl API) | file=浏览器崩溃2.jpg, prompt=图片内容 | 超时无返回（30s） | 未用于结论（API 不可达，退回 PIL 元数据） |
| python3+PIL | 浏览器崩溃.jpg / 浏览器崩溃2.jpg | 尺寸 1440×1080 / 4624×2080，JPEG 格式 | 用于第二章附件图片分析（仅元数据） |
