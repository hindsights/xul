require("xul")


function startHttpServer(port)
    print("startHttpServer", port)
    xul.log_manager.start_console_log_service("httpsvr")
    local app = {}
    app.download_root = "download"
    app.upload_root = "upload"
    app.ios = xul.io_service.new()
    print("ios is started")
    app.ios:start()
    app.server = xul.http_server.new(app.ios)
    app.router = xul.simple_http_server_router.new()
    app.router:add("/down/", function () return createDownloadHandler(app) end)
    app.router:add("/up/", function () return createUploadHandler(app) end)
    app.router:add("/hello/", createHelloHandler)
    app.router:set_not_found_handler(createNotFoundHandler)
    print("app.router", app.router)
    app.server:set_router(app.router)
    if not app.server:start(port) then
        print("failed to start http server at port:" .. port)
        app.ios:stop()
        return
    end
    print("http server is started", port)
    return app
end

function createDownloadHandler(app)
    local handler = {}
    handler.handle_request = function (session, req)
        local path = req:get_uri():get_path()
        local fullpath = xul.paths.join(app.download_root, path:sub(string.len("/down/") + 1))
        print("DownloadHandler.handle_request", session, req, app.download_root, path, path:sub(string.len("/down/") + 1), fullpath)
        handler.fin = io.open(fullpath, "rb")
        if handler.fin == nil then
            session:send_empty_response(404)
            return
        end
        handler.filesize = handler.fin:seek("end")
        handler.fin:seek("set")
        contentrange = string.format("Content-Range: bytes 0-%d/%d", handler.filesize-1, handler.filesize)
        local resp = session:get_response()
        resp:set_header("Content-Type", "application/octet-stream")
        resp:set_header("Content-Range", contentrange)
        resp:set_header("Content-Length", string.format("%d", handler.filesize))
        resp:set_status_code(200)
        session:send_header(resp)
        handler.sent_size = 0
        handler.send_file_data(session)
    end
    handler.send_file_data = function (session)
        if handler.fin == nil then
            return
        end
        local data = handler.fin:read(256*1024)
        if data == nil then
            print("send_file_data finish", session)
            handler.fin:close()
            handler.fin = nil
            session:finish()
            return
        end
        session:send_data(data)
    end
    handler.handle_data_sent = function (session, size)
        -- print("DownloadHandler.handle_data_sent", session, size, handler.filesize, handler.sent_size)
        handler.sent_size = handler.sent_size + size
        handler.send_file_data(session)
    end
    return handler
end

function createUploadHandler(app)
    local handler = {}
    handler.handle_request = function (session, req)
        local path = req:get_uri():get_path()
        local fullpath = xul.paths.join(app.upload_root, path:sub(string.len("/up/") + 1))
        print("UploadHandler.handle_request", session, req, path, fullpath)
        handler.content_length = tonumber(req:get_header("Content-Length"), 10)
        if handler.content_length == nil or handler.content_length <= 0 then
            session:send_empty_response(400)
            return
        end
        handler.fout = io.open(fullpath, "wb")
        if handler.fout == nil then
            session:send_empty_response(404)
            return
        end
        handler.received_size = 0
    end
    handler.handle_body_data = function (session, data)
        handler.fout:write(data)
        handler.received_size = handler.received_size + data:len()
        if handler.received_size >= handler.content_length then
            print("handle_body_data finish", session)
            handler.fout:close()
            session:send_plain_text(200, "File is uploaded successfully.")
            return
        end
    end
    return handler
end

function createHelloHandler()
    local handler = {}
    handler.handle_request = function (session, req)
        print("HelloHandler.handle_request", session, req)
        session:send_plain_text(200, "Hello and welcome!")
    end
    return handler
end

function createNotFoundHandler()
    local handler = {}
    handler.handle_request = function (session, req)
        print("not found handler", session, req)
        session:send_plain_text(200, "Sorry, you have no clearance!")
    end
    return handler
end

local app = startHttpServer(18080)
io.read("*l")
print("stopping the server...")
app.server:stop()
app.ios:stop()
print("the server is stopped.")
