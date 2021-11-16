-- skynet module two-step initialize . When you require a skynet module :
-- 1. Run module main function as official lua module behavior.
-- 2. Run the functions register by skynet.init() during the step 1,
--      unless calling `require` in main thread .
-- If you call `require` in main thread ( service main function ), the functions
-- registered by skynet.init() do not execute immediately, they will be executed
-- by skynet.start() before start function.

local M = {}

local mainthread, ismain = coroutine.running()
assert(ismain, "skynet.require must initialize in main thread")

local context = {
	[mainthread] = {},
}

do
	local require = _G.require
	local loaded = package.loaded
	local loading = {}

	function M.require(name) -- 闭包
		local m = loaded[name]
		if m ~= nil then
			return m
		end

		-- 主协程直接调用_G.require
		local co, main = coroutine.running()
		if main then
			return require(name)
		end

		local filename = package.searchpath(name, package.path)
		if not filename then
			return require(name)
		end

		local modfunc = loadfile(filename)
		if not modfunc then
			return require(name)
		end

		-- 其他协程正在加载则挂起
		local loading_queue = loading[name]
		if loading_queue then
			assert(loading_queue.co ~= co, "circular dependency")
			-- Module is in the init process (require the same mod at the same time in different coroutines) , waiting.
			local skynet = require "skynet"
			loading_queue[#loading_queue+1] = co
			skynet.wait(co)
			local m = loaded[name]
			if m == nil then
				error(string.format("require %s failed", name))
			end
			return m
		end

		loading_queue = {co = co}
		loading[name] = loading_queue

		-- require脚本时可能递归调用require，防止覆盖上下文，闭包内暂存old_init_list
		local old_init_list = context[co]
		local init_list = {}
		context[co] = init_list

		local function execute_module()
			-- 执行脚本
			local m = modfunc(name, filename)

			--[[
			skynet.init用于注册函数，在模块加载完之前执行，见skynet.init_service
			skynet.init使用场景举例：
			=== new_service.lua ===
				local mc = require "skynet.multicast"
				skynet.start(funciton modfunc()
					-- do something...
				end)
				-- multicast.lua加载时会调用skynet.init注删函数查询multicastd服务的地址，挂起直至查询返回，因此init_list执行完之后rquire不会返回
				-- 如果脚本内使用了skynet.init，require必须在skynet.start前调用
			=======================
			]]--
			-- 调用脚本使用skynet.init注册的函数
			for _, f in ipairs(init_list) do
				f()
			end

			if m == nil then
				m = true
			end

			loaded[name] = m
		end

		local ok, err = xpcall(execute_module, debug.traceback)

		context[co] = old_init_list

		-- 唤醒正在等待该模块的协程
		local waiting = #loading_queue
		if waiting > 0 then
			local skynet = require "skynet"
			for i = 1, waiting do
				skynet.wakeup(loading_queue[i])
			end
		end
		loading[name] = nil

		if ok then
			return loaded[name]
		else
			error(err)
		end
	end
end

function M.init_all()
	for _, f in ipairs(context[mainthread]) do
		f()
	end
	context[mainthread] = nil
end

function M.init(f)
	assert(type(f) == "function")
	local co = coroutine.running()
	table.insert(context[co], f)
end

return M