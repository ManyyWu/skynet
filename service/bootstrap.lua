local skynet = require "skynet"
local harbor = require "skynet.harbor"
local service = require "skynet.service"
require "skynet.manager"	-- import skynet.launch, ...

skynet.start(function()
	local standalone = skynet.getenv "standalone"

	-- 启动luancher服务
	local launcher = assert(skynet.launch("snlua","launcher"))
	skynet.name(".launcher", launcher)

	local harbor_id = tonumber(skynet.getenv "harbor" or 0)
	if harbor_id == 0 then -- 单节点
		assert(standalone ==  nil)
		standalone = true
		skynet.setenv("standalone", "true")

		-- 启动cdummy服务，用于拦截对外广播的全局名字变更
		local ok, slave = pcall(skynet.newservice, "cdummy")
		if not ok then
			skynet.abort()
		end
		skynet.name(".cslave", slave)
	else -- 多节点
		-- master启动cmaster服务
		if standalone then
			if not pcall(skynet.newservice,"cmaster") then
				skynet.abort()
			end
		end

		-- master和slave都启动cslave服务，用于节点间的消息转发，以及同步全局名字
		local ok, slave = pcall(skynet.newservice, "cslave")
		if not ok then
			skynet.abort()
		end
		skynet.name(".cslave", slave)
	end

	-- 启动datacenterd服务，用于跨节点数据共享，https://github.com/cloudwu/skynet/wiki/DataCenter
	if standalone then
		local datacenter = skynet.newservice "datacenterd"
		skynet.name("DATACENTER", datacenter)
	end

	-- 启动service_mgr服务，https://github.com/cloudwu/skynet/wiki/UniqueService
	skynet.newservice "service_mgr"

	-- 启动ltls_holder服务
	local enablessl = skynet.getenv "enablessl"
	if enablessl then
		service.new("ltls_holder", function ()
			local c = require "ltls.init.c"
			c.constructor()
		end)
	end

	-- 启动逻辑服务，默认加载main.lua
	pcall(skynet.newservice, skynet.getenv "start" or "main")
	skynet.exit()
end)
