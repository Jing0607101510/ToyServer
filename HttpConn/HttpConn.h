/*
@Author: Jing
@Date: 2020.10.05
@Desc: 封装http数据，及其处理函数
*/

#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include <memory>
#include <string>
#include <map>

#include "../Timer/Timer.h"
#include "../EventLoop/EventLoop.h"
#include "../Channel/Channel.h"

enum METHOD{
    GET = 0,
    HEAD,
    POST,
    OTHERS // 其他不支持的方法
}

enum VERSION{
    HTTP10 = 0,
    HTTP11
}

enum CHECK_STATE{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEADER,
    CHECK_STATE_CONTENT
}

enum HTTP_CODE{
    NO_REQUEST = 0,
    GET_REQUEST, // 200 正常
    BAD_REQUEST, // 400
    NO_RESOURCE, // 404
    FORBIDDEN_REQUEST, //403
    FILE_REQUEST, // 200 正常
    INTERNAL_ERROR, // 500
    CLOSED_CONNECTION, // 
    NOT_IMPLEMENTED // 501
}

enum LINE_STATE{
    LINE_OK = 0,
    LINE_BAD,
    LINE_AGAIN
}

enum WRITE_RESULT{
    WRITE_ALL = 0,
    WRITE_PART,
    WRITE_ERROR
}

class HttpConn : public std::enable_shared_from_this<HttpConn>{
    public:
        HttpConn(EventLoop* loop, int conn_fd, std::string&& conn_name);
        ~HttpConn();
        void setTimer(std::shared_ptr<Timer> timer);
        std::string getName();
        std::shared_ptr<Channel> getChannel();
        // 在Server中绑定这个函数，加入到相应的loop的pendingFunctors中
        void newConn(); 

        void closeHandler();

    private:
        EventLoop* m_loop;
        std::shared_ptr<Timer> m_timer;
        std::shared_ptr<Channel> m_channel;
        int m_conn_fd;

        // 读写缓冲区
        std::string m_inbuff;
        std::string m_outbuff;
        int m_read_idx; // 记录读取到读缓冲的哪一个位置，因为行不完整的原因
        char* m_file_ptr;
        struct iovec m_iovec[2];

        // 连接名
        std::string m_conn_name;

        // http请求报文相关
        METHOD m_method;
        VERSION m_version;
        std::string m_filename;
        std::string m_path;
        std::map<std::string, std::string> m_headers;
        bool m_keep_alive;
        int m_file_size;
        std::string m_content;
        bool m_error_exist;

        // 主状态机状态
        CHECK_STATE m_check_state; // 当前处在什么状态，做什么事情，导致状态转移

    private:

        bool read();
        bool write();
        
        LINE_STATE parseLine();
        HTTP_CODE analyseRequest();
        bool sendResponse(HTTP_CODE code);
        HTTP_CODE parseRequestLine();
        HTTP_CODE parseHeaders();
        HTTP_CODE parseContent();
        HTTP_CODE doRequest();

        void reset();
        void seperateTimer();

        void readHandler();
        void writeHandler();
        void errorHandler();

        void add_content(sting content);
        void add_status_line(int status);
        void add_headers(string header, string value);
        void add_content_type();
        void add_content_length();
        void add_linger(); // linger的用处？
        void add_blank_line();

};


#endif