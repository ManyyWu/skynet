#ifndef SKYNET_MODULE_H
#define SKYNET_MODULE_H

struct skynet_context;

typedef void * (*skynet_dl_create)(void);
typedef int (*skynet_dl_init)(void * inst, struct skynet_context *, const char * parm);
typedef void (*skynet_dl_release)(void * inst);
typedef void (*skynet_dl_signal)(void * inst, int signal);

struct skynet_module {
	const char * name;         // 模块名
	void * module;             // .so模块句柄
	skynet_dl_create create;   // 创建模块回调
	skynet_dl_init init;       // 初始化模块回调
	skynet_dl_release release; // 释放模块回调
	skynet_dl_signal signal;   // 模块信号处理回调
};

// 添加模块
void skynet_module_insert(struct skynet_module *mod);
// 查询模块
struct skynet_module * skynet_module_query(const char * name);
// 创建模块
void * skynet_module_instance_create(struct skynet_module *);
// 初始化模块
int skynet_module_instance_init(struct skynet_module *, void * inst, struct skynet_context *ctx, const char * parm);
// 释放模块
void skynet_module_instance_release(struct skynet_module *, void *inst);
// 模块信号处理
void skynet_module_instance_signal(struct skynet_module *, void *inst, int signal);
// 初始化模块管理器
void skynet_module_init(const char *path);

#endif
