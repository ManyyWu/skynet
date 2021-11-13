#ifndef SKYNET_IMP_H
#define SKYNET_IMP_H

struct skynet_config {
	int thread;               // worker数量
	int harbor;               // 节点标识符1-255，为0时表示单节点
	int profile;              // 是否开启统计功能，统计每个服务使用的cpu时间，默认为1
	const char * daemon;      // 守护进程进程id存储目录
	const char * module_path; // C服务模块的路径
	const char * bootstrap;   // skynet启动的第一个服务，默认为"snlua bootstrap"
	const char * logger;      // log输出路径，nil表示标准输出
	const char * logservice;  // log服务，默认为"logger"
};

#define THREAD_WORKER 0
#define THREAD_MAIN 1
#define THREAD_SOCKET 2
#define THREAD_TIMER 3
#define THREAD_MONITOR 4

void skynet_start(struct skynet_config * config);

#endif
