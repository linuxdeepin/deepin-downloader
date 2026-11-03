## Root Cause Analysis

The browser extension's `onHeadersReceived` listener registered with `urls: ["<all_urls>"]` in blocking mode, and `SUPPORT_REQUEST_TYPE_ARRAY` included `"media"` type. This caused every streaming video segment request (e.g., `.ts`/`.mp4` fragments from v.qq.com) to enter the download interception pipeline. Over 12-24 hours of continuous video playback, the accumulated per-request `console.log` output, blocking interception overhead, and `QTimer` memory leak in `sendUrlToDownloader` (C++ backend) exhausted resources, crashing the browser.

Key evidence: `background.js:503-504` (blocking listener on `<all_urls>`), `background.js:827` (`"media"` in `SUPPORT_REQUEST_TYPE_ARRAY`), `extensionservice.cpp:111-120` (`QTimer` never deleted after `stop()`).

## Fix Approach

Root cause layer + symptom layer combined fix:

1. Remove `"media"` from `SUPPORT_REQUEST_TYPE_ARRAY` so streaming media requests bypass download interception entirely
2. Convert `SUPPORT_MEDIA_EXT_ARRAY` to `Set` for O(1) lookup (was O(n) linear scan)
3. Remove per-request `console.log` in `onBeforeSendHeaders` handler
4. Fix `QTimer` memory leak in `sendUrlToDownloader` by adding `deleteLater()` after `stop()`
5. Add 5000ms timeout to `waitForFinished()` in `runPipeProcess` to prevent indefinite blocking

Implementation aligns with analysis-report.md section 10 recommended approach (root cause + symptom layer combined).

## Change Safety Assessment

### Code Safety

- **Risk Level**: Low
- All changes are internal logic modifications with no signature changes. `SUPPORT_REQUEST_TYPE_ARRAY` removal of `"media"` only affects which request types enter the interception pipeline — normal file downloads (main_frame, sub_frame, object, xmlhttprequest, other) are unaffected. `Set` conversion is semantically equivalent (exact match). `QTimer` fix adds standard Qt memory management. No historical fixes are reverted.
- No related PMS bugs found in commit history; no regression risk from associated fixes.

### Business Impact Scope

- **Browser extension download interception**: Normal file downloads (exe, zip, mp4, etc.) still intercepted by the downloader; streaming video playback requests no longer trigger interception, eliminating per-segment overhead
- **Extension service WebSocket management**: `sendUrlToDownloader` no longer leaks `QTimer` objects; `checkConnection`'s `runPipeProcess` no longer blocks indefinitely on `netstat`

### Verification Suggestion

Test that normal file downloads are still intercepted by the downloader. Test that online video playback (e.g., v.qq.com) plays normally without triggering download interception. Test long-running browser session (12h+) for memory stability.

---

## 根因分析

下载器浏览器扩展的 `onHeadersReceived` 监听器以 blocking 模式注册在 `<all_urls>` 上，且 `SUPPORT_REQUEST_TYPE_ARRAY` 包含 `"media"` 类型，导致每个流媒体视频分片请求（如 v.qq.com 的 `.ts`/`.mp4` 分片）都进入下载拦截管道。12-24 小时连续视频播放期间，逐请求 `console.log` 输出、blocking 拦截开销以及 C++ 后端 `sendUrlToDownloader` 中的 `QTimer` 内存泄漏累积，最终耗尽资源导致浏览器崩溃。

关键证据：`background.js:503-504`（blocking 监听器注册在 `<all_urls>`）、`background.js:827`（`"media"` 在 `SUPPORT_REQUEST_TYPE_ARRAY` 中）、`extensionservice.cpp:111-120`（`QTimer` 在 `stop()` 后从未删除）。

## 修复方案

根因层修复 + 症状层补丁联合实施：

1. 从 `SUPPORT_REQUEST_TYPE_ARRAY` 移除 `"media"`，使流媒体请求完全绕过下载拦截
2. 将 `SUPPORT_MEDIA_EXT_ARRAY` 转为 `Set`，实现 O(1) 查找（原为 O(n) 线性扫描）
3. 移除 `onBeforeSendHeaders` 处理器中逐请求的 `console.log`
4. 修复 `sendUrlToDownloader` 中 `QTimer` 内存泄漏，在 `stop()` 后添加 `deleteLater()`
5. 为 `runPipeProcess` 的 `waitForFinished()` 添加 5000ms 超时，防止无限阻塞

实现与分析报告第十节推荐方案一致（根因层 + 症状层联合修复）。

## 改动安全评估

### 代码安全评估

- **风险等级**: 低风险
- 所有改动均为内部逻辑修改，不改变函数签名。`SUPPORT_REQUEST_TYPE_ARRAY` 移除 `"media"` 仅影响进入拦截管道的请求类型——正常文件下载（main_frame、sub_frame、object、xmlhttprequest、other）不受影响。`Set` 转换语义等价（精确匹配）。`QTimer` 修复添加标准 Qt 内存管理。不撤销任何历史修复。
- commit 历史中未发现关联 PMS bug，无关联修复回归风险。

### 业务影响范围

- **浏览器扩展下载拦截**：正常文件下载（exe、zip、mp4 等）仍被下载器接管；流媒体视频播放请求不再触发拦截，消除逐分片开销
- **扩展服务 WebSocket 管理**：`sendUrlToDownloader` 不再泄漏 `QTimer` 对象；`checkConnection` 的 `runPipeProcess` 不再因 `netstat` 无限阻塞

### 验证建议

测试正常文件下载仍能被下载器接管。测试在线视频播放（如 v.qq.com）正常播放且不触发下载拦截。测试浏览器长时间运行（12小时以上）内存稳定性。
