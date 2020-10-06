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
    OTHER // 其他不支持的方法
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
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURCE,
    FORBIDDEN_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR, 
    CLOSED_CONNECTION
}

enum LINE_STATE{
    LINE_OK = 0,
    LINE_BAD,
    LINE_AGAIN
}


class HttpConn{
    public:
        HttpConn();
        ~HttpConn();
        void setTimer(std::shared_ptr<Timer> timer);

    private:
        EventLoop* m_loop;
        std::shared_ptr<Timer> m_timer;
        std::shared_ptr<Channel> m_channel;
        int m_conn_fd;

        // 读写缓冲区
        std::string m_in_buf;
        std::string m_out_buf;
        int m_read_idx;
        char* m_file_ptr;
        struct iovec m_iovec[2];

        // 连接名
        std::string m_conn_name;

        // http请求报文相关
        METHOD m_method;
        VERSION m_version;
        std::string m_filename;
        std::string m_path;
        std::map<std::string, std::string> m_header;
        bool m_keep_alive;

        // 主状态机状态
        CHECK_STATE m_check_state;

    private:
        void readHandler();
        void writeHandler();
        void closeHandler();
        void errorHandler();
        void connHandler();

        bool read();
        bool write();
        
        LINE_STATE parse_line();
        HTTP_CODE analyse_request();
        HTTP_CODE parse_request_line();
        HTTP_CODE parse_headers();
        HTTP_CODE parse_content();
        HTTP_CODE do_request();

        void reset();
};


#endif