#ifndef SKYNET_SERVER_H
#define SKYNET_SERVER_H

#include <stdint.h>
#include <stdlib.h>

struct skynet_context;
struct skynet_message;
struct skynet_monitor;

// 创建新的服务
struct skynet_context * skynet_context_new(const char * name, const char * parm);
// 增加服务引用计数
void skynet_context_grab(struct skynet_context *);
// 增加harbor服务引用计数
void skynet_context_reserve(struct skynet_context *ctx);
// 引用为0时释放服务
struct skynet_context * skynet_context_release(struct skynet_context *);
// 获取服务的标识符
uint32_t skynet_context_handle(struct skynet_context *);
// 将消息添加到私有队列的队尾
int skynet_context_push(uint32_t handle, struct skynet_message *message);
// 向服务发送消息
void skynet_context_send(struct skynet_context * context, void * msg, size_t sz, uint32_t source, int type, int session);
// 获取新的会话id
int skynet_context_newsession(struct skynet_context *);
// 处理私有队列的多个消息，并返回下一个需要处理消息的私有队列
struct message_queue * skynet_context_message_dispatch(struct skynet_monitor *, struct message_queue *, int weight);	// return next queue
// 获取服务数
int skynet_context_total();
// 程序初始化出错时logger打印日志
void skynet_context_dispatchall(struct skynet_context * context);	// for skynet_error output before exit
// 标记服务可能死循环
void skynet_context_endless(uint32_t handle);	// for monitor

// skynet全局环境处理
void skynet_globalinit(void);
void skynet_globalexit(void);
void skynet_initthread(int m);
void skynet_profile_enable(int enable);

#endif
