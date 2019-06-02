#ifndef MIRRORACCEL_CONNINCOMING_H_
#define MIRRORACCEL_CONNINCOMING_H_

#include "mongoose.h"
#include "curl/curl.h"
#include <list>
#include <set>
#include <thread>
#include <memory>
#include <regex>
#include "mirroraccel_server.h"

namespace mirroraccel
{
class Server;
class ConnOutgoing;
class MirrorItem;
struct Response;
class Request;

class ConnIncoming
{
    friend class ConnOutgoing;
public:
    ConnIncoming(
        Server& server,
		std::shared_ptr<Request> request);
    ~ConnIncoming();
public:
    enum Status {
        ST_QUERY = 0,
        ST_QUERY_END,
        ST_TRANS,
    };
public:
	std::shared_ptr<Request> getRequest();
    void reset(std::shared_ptr<Request> request);

private:
    void stop();
    void start();
    void dispatch();
    void eventWait();
    void perform();

    bool poll();
    CURLM* handle();

    void removeQueryConn();
    bool onQueryEnd(ConnOutgoing* conn, std::shared_ptr<Response> response);
private:
	//reset pending data
    Status status = ST_QUERY;

    //connection stop signal
    bool stopSignal = false;
    //mongoose poll thread
    std::shared_ptr<std::thread> pollThread = nullptr;
    //HTTP server
    Server& server;
    //mirror connections
    std::set<std::shared_ptr<ConnOutgoing>>  conns;
    std::mutex connMux;
    //target url
    std::string url;
    //HTTP range
    std::pair<std::int64_t, std::int64_t> range;
    //curl multi handle
    CURLM* curlMutil = nullptr;
    //HTTP request
	std::shared_ptr<Request> request = nullptr;
    //匹配http头正则
    std::regex regHeader;
    //返回responseHeader;
    std::string responseHeader;
    //正在运行的easycurl
    int stillRunning = 0;
};
} // namespace mirroraccel
#endif