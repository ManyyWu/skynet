#ifndef SKYNET_CONTEXT_HANDLE_H
#define SKYNET_CONTEXT_HANDLE_H

#include <stdint.h>

// reserve high 8 bits for remote id
// 24位用于服务标识符，8位用于节点标识符
#define HANDLE_MASK 0xffffff
#define HANDLE_REMOTE_SHIFT 24

struct skynet_context;

// 注册服务
uint32_t skynet_handle_register(struct skynet_context *);
// 移除服务
int skynet_handle_retire(uint32_t handle);
// 通过标识符获取服务
struct skynet_context * skynet_handle_grab(uint32_t handle);
// 移除所有服务
void skynet_handle_retireall();
// 通过服务名获取服务标识符
uint32_t skynet_handle_findname(const char * name);
// 设置服务名
const char * skynet_handle_namehandle(uint32_t handle, const char *name);
// 初始化服务管理器
void skynet_handle_init(int harbor);

#endif
