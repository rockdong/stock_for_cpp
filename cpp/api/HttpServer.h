#ifndef API_HTTP_SERVER_H
#define API_HTTP_SERVER_H

#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace httplib { class Server; }

namespace api {

class HttpServer {
public:
    explicit HttpServer(int port = 3001);
    ~HttpServer();
    
    void start();
    void stop();
    bool isRunning() const;
    int getPort() const;

private:
    void setupRoutes();
    
    int port_;
    std::atomic<bool> running_;
    std::unique_ptr<httplib::Server> server_;
    std::thread serverThread_;
};

} // namespace api

#endif // API_HTTP_SERVER_H