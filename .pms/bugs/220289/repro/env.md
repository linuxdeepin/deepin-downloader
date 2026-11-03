### 能力探测（repro_probe.py，2026-11-03 17:50）

内核 `6.6.0-amd64-desktop`｜uid `1001`｜root=`0`｜容器=`0`

capability：`dac_read_search=0 net_admin=0 sys_module=0 sys_chroot=0 sys_ptrace=0 sys_admin=0`

| 手段 | 状态 | 证据 |
|---|---|---|
| loop 设备（losetup + 普通文件） | NO | /sbin/losetup 与 /dev/loop-control 均在，但非 root 且无 CAP_SYS_ADMIN |
| 完整 SCSI 设备（scsi_debug） | ?? | modprobe -n scsi_debug 解析通过（可加载）；但非 root 且无 CAP_SYS_MODULE，加载需实测 |
| 可编程块设备后端（nbd） | NO | 无 /dev/nbd0、无 nbd-client/nbdkit、无 CAP_SYS_ADMIN |
| 用户态文件系统（fuse/bindfs） | OK | /usr/bin/fusermount3；/dev/fuse 存在 |
| Qt offscreen 无显示启动 | OK | /usr/lib/x86_64-linux-gnu/qt5/plugins/platforms/libqoffscreen.so |
| 虚拟 X（xvfb-run） | NO | 无 xvfb-run/Xvfb |
| Wayland headless（weston） | NO | weston 未安装 |
| 私有会话总线 + 收发包观测 | OK | /usr/bin/dbus-run-session、/usr/bin/dbus-monitor；当前无会话总线（用 dbus-run-session 起） |
| 系统调用跟踪与定点错误注入 | OK | /usr/bin/strace |
| 断点 / 附栈 / 改返回值 | OK | /usr/bin/gdb |
| 库调用跟踪 | NO | ltrace 未安装 |
| 内核态跟踪（延迟、阻塞点） | NO | bpftrace 未安装 |
| 采样与调度分析 | NO | perf 未安装 |
| 网络包观测 | NO | tcpdump 未安装 |
| LD_PRELOAD 包装（延迟/错误注入） | OK | /usr/bin/gcc；glibc 2.38 |
| 网络延迟/丢包注入（tc netem） | NO | /sbin/tc 在，但无 CAP_NET_ADMIN |
| 容器内跑目标发行版（podman） | OK | /usr/bin/podman |
| 容器内跑目标发行版（docker） | NO | docker 未安装 |
| 用户命名空间隔离 | OK | unshare -Ur true 成功（可用用户命名空间降权构造） |
| chroot + bind mount | NO | 缺 CAP_SYS_CHROOT |
| systemd-run 起临时会话单元 | OK | /usr/bin/systemd-run |
| 整机虚拟机（qemu） | NO | 无 qemu-system-*/qemu-kvm |
| 安装依赖包 | ?? | /usr/bin/apt-get 在，但非 root；能否装包取决于用户权限与源可达 |

**可用手段**：用户态文件系统（fuse/bindfs）；Qt offscreen 无显示启动；私有会话总线 + 收发包观测；系统调用跟踪与定点错误注入；断点 / 附栈 / 改返回值；LD_PRELOAD 包装（延迟/错误注入）；容器内跑目标发行版（podman）；用户命名空间隔离；systemd-run 起临时会话单元
