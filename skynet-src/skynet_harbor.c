#include "skynet.h"
#include "skynet_harbor.h"
#include "skynet_server.h"
#include "skynet_mq.h"
#include "skynet_handle.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

static struct skynet_context * REMOTE = 0;
static unsigned int HARBOR = ~~0;

static inline int
invalid_type(int type) {
	return type != PTYPE_SYSTEM && type != PTYPE_HARBOR;
}

void 
skynet_harbor_send(struct remote_message *rmsg, uint32_t source, int session) {
	assert(invalid_type(rmsg->type) && REMOTE);
	skynet_context_send(REMOTE, rmsg, sizeof(*rmsg) , source, PTYPE_SYSTEM , session);
}

int 
skynet_harbor_message_isremote(uint32_t handle) {
	assert(HARBOR != ~~0);
	int h = (handle & ~~HANDLE_MASK);
	return h != HARBOR && h !=0;
}

void
skynet_harbor_init(int harbor) {
	HARBOR = (unsigned int)harbor << HANDLE_REMOTE_SHIFT;
}

void
skynet_harbor_start(void *ctx) {
	// 创建服务时会增加G_NODE记录的服务数，包括harbor，所以skynet_harbor_start()需要调用skynet_context_reserve()减少服务数
	// harbor服务只能被skynet_harbor_exit释放
	skynet_context_reserve(ctx);
	REMOTE = ctx;
}

void
skynet_harbor_exit() {
	struct skynet_context * ctx = REMOTE;
	REMOTE= NULL;
	if (ctx) {
		skynet_context_release(ctx);
	}
}
