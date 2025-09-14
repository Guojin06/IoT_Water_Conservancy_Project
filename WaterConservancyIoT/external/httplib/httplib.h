// 简化版httplib库头文件
// 实际项目中应该下载完整版本

#pragma once

#include <string>
#include <functional>
#include <map>

namespace httplib {

struct Request {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct Response {
    int status = 200;
    std::map<std::string, std::string> headers;
    std::string body;
    
    void set_content(const std::string& content, const std::string& content_type) {
        body = content;
        headers["Content-Type"] = content_type;
    }
};

using Handler = std::function<void(const Request&, Response&)>;

class Server {
public:
    Server() = default;
    
    void Get(const std::string& pattern, Handler handler) {
        m_get_handlers[pattern] = handler;
    }
    
    void Post(const std::string& pattern, Handler handler) {
        m_post_handlers[pattern] = handler;
    }
    
    void Put(const std::string& pattern, Handler handler) {
        m_put_handlers[pattern] = handler;
    }
    
    void Delete(const std::string& pattern, Handler handler) {
        m_delete_handlers[pattern] = handler;
    }
    
    bool listen(const std::string& host, int port) {
        // 简化实现 - 实际项目中需要完整的HTTP服务器
        return true;
    }
    
    void stop() {
        // 停止服务器
    }

private:
    std::map<std::string, Handler> m_get_handlers;
    std::map<std::string, Handler> m_post_handlers;
    std::map<std::string, Handler> m_put_handlers;
    std::map<std::string, Handler> m_delete_handlers;
};

class Client {
public:
    Client(const std::string& host) : m_host(host) {}
    
    struct Result {
        Response* operator->() { return &response; }
        operator bool() const { return success; }
        
        Response response;
        bool success = false;
    };
    
    Result Get(const std::string& path) {
        Result result;
        result.success = true;
        return result;
    }
    
    Result Post(const std::string& path, const std::string& body, const std::string& content_type) {
        Result result;
        result.success = true;
        return result;
    }

private:
    std::string m_host;
};

} // namespace httplib
