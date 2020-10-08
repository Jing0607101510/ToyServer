/*
@Author: Jing
@Date: 2020.09.27
@Desc: 封装http数据，及其处理函数
*/

#include <sys/epoll.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "HttpConn.h"
#include "../Log/Logger.h"

const int DEFAULT_EXPIRED_TIME = 2; // ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60; // ms
const int MAX_BUFF_SIZE = 1024;

const std::map<int, std::string> response_desc = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {500, "Internal Error"},
    {501, "Not Implemented"}
};

const std::map<int, std::string> response_content = {
    {400, "Your request has bad syntax or is inherently impossible to staisfy.\n"},
    {403, "You do not have permission to get file form this server.\n"},
    {404, "The requested file was not found on this server.\n"},
    {500, "There was an unusual problem serving the request file.\n"},
    {501, "THe request method is not implememted!\n"}
};

const std::map<std::string, std::string> mime = {
    {".html", "text/html"},
    {".avi", "video/x-msvideo"},
    {".bmp", "image/bmp"},
    {".c", "text/plain"},
    {".doc", "application/msword"},
    {".gif", "image/gif"},
    {".gz", "application/x-gzip"},
    {".htm", "text/html"},
    {".ico", "image/x-icon"},
    {".jpg", "image/jpeg"},
    {".png", "image/png"},
    {".txt", "text/plain"},
    {".mp3", "audio/mp3"},
    {"default", "text/html"}
};



HttpConn::HttpConn(EventLoop* loop, int conn_fd, std::string&& conn_name):
    m_loop(loop),
    m_conn_fd(conn_fd),
    m_channel(new Channel(conn_fd)),
    m_conn_name(conn_name),
    m_read_idx(0),
    m_check_state(CHECK_STATE_REQUESTLINE),
    m_error_exist(false)
{
    m_channel->setEvents(EPOLLET | EPOLLIN);
    m_channel->setReadHandler(std::bind(&HttpConn::readHandler, this));
    m_channel->setWriteHandler(std::bind(&HttpConn::writeHandler, this));
    m_channel->setErrorHandler(std::bind(&HttpConn::errorHandler, this));
    m_channel->setCloseHandler(std::bind(&HttpConn::closeHandler, this));
}

HttpConn::~HttpConn(){
    close(m_conn_fd);
    // TODO : 还有其他事情吗
}

void HttpConn::readHandler(){
    // 先读取数据
    bool ret = read();
    if(!ret){
        closeHandler(); // 若出错或者对端关闭，则关闭fd
        return;
    }
    // 对数据进行处理
    HTTP_CODE analyse_ret = analyseRequest();
    // 对不同的HTTP_CODE进行处理
    if(analyse_ret == NO_REQUEST){ // 数据不够，继续读取
        m_channel->setEvents(EPOLLIN | EPOLLET);
        int timeout = DEFAULT_EXPIRED_TIME;
        if(m_keep_alive)
            timeout = DEFAULT_KEEP_ALIVE_TIME;
        seperateTimer();
        m_loop->updatePoller(m_channel, timeout);
        return;
    } 
    if(analyse_ret != GET_REQUEST) m_error_exist = true; // 报文分析错误，或者请求的内容错误

    ret = sendResponse(analyse_ret); // 只是写到写缓冲区
    if(!ret){
        closeHandler();
        return;
    }
    
    writeHandler();
}

void HttpConn::writeHandler(){
    // 返回值应该区分：1.写失败，2.全部写完成，3.部分写，还需监听写
    WRITE_RESULT write_result = write(); // 从缓冲区写入网络中
    if(write_result == WRITE_ERROR) closeHandler();
    else if(write_result == WRITE_PART){
        m_channel->setEvents(EPOLLOUT | EPOLLET);
        int timeout = DEFAULT_EXPIRED_TIME; // 什么时候修改定时器？答：调用updatePoler时。所以先要解除原TImer和HttpConn的关系
        if(m_keep_alive)
            timeout = DEFAULT_KEEP_ALIVE_TIME;
        seperateTimer();
        m_loop->updatePoller(m_channel, timeout);
    }
    else{ // DONE: 这里的错误状态要在下一个write可见，不应该用analyse_ret来比较
        // 全部数据写完到网络中，重新设定计时器？
        // 什么时机设置计时器？答：每一次触发就修改超时时间
        if(m_error_exist) 
            closeHandler();
        else // 由于数据写完，所以设置IN事件，开启接收新的请求
        {
            m_channel->setEvents(EPOLLIN | EPOLLET);
            int timeout = DEFAULT_EXPIRED_TIME;
            if(m_keep_alive)
                timeout = DEFAULT_KEEP_ALIVE_TIME;
            seperateTimer();
            reset(); // 重新设置某些参数，开始接收新的请求报文
            m_loop->updatePoller(m_channel, timeout);
        }
    }
}

void HttpConn::closeHandler(){
    // 1.设置timer为deleted状态，并且设置其callback为nullptr
    // 2.设置m_timer为nullptr，解除本对象和对应的timer的关系
    // 3.从epollfd中删除connfd，并且从poller的m_conns中删除对应的http_conn对象
    // 使得删除前，对应的HttpConn对象至少有一个shared_ptr引用
    std::shared_ptr<HttpConn> guard(shared_from_this());
    reset();
    m_loop->removeFromPoller(m_channel);
}


void HttpConn::errorHandler(){
    // TODO: 错误处理
}


void HttpConn::newConn(){
    // 使用loop的addToPoller加入到poller中
    m_loop->addToPoller(m_channel, DEFAULT_EXPIRED_TIME);
}


void HttpConn::setTimer(std::shared_ptr<Timer> timer){
    m_timer = timer;
}

std::string HttpConn::getName(){
    return m_conn_name;
}

std::shared_ptr<Channel> HttpConn::getChannel(){
    return m_channel;
}


bool HttpConn::read(){
    // 循环读取数据，直到没有数据
    char* buff[MAX_BUFF_SIZE];
    ssize_t nread = 0;
    while(true){
        nread = recv(m_conn_fd, buff, MAX_BUFF_SIZE, 0);
        if(nread < 0){
            if(errno == EINTR) continue;
            else if(errno == EAGAIN || errno == EWOULDBLOCK) return true;
            else{
                LOG_ERROR("HttpConn::read Error : %s.", strerror(errno));
                return false;
            }
        }
        else if(nread == 0){ // 对端关闭
            return false;
        }
        else{
            m_inbuff += std::string(buff, buff + nread);
        }
    }
    return true;
}


bool HttpConn::write(){
    // TODO: 非阻塞写数据
}

// 将需要发送的数据写入写缓冲区（进程空间）
bool HttpConn::sendResponse(HTTP_CODE code){
    // TODO: 将发送的内容加入到写缓冲
}



LINE_STATE HttpConn::parseLine(){
    char tmp;
    int size = m_inbuff.size();
    // m_read_idx 需要在解析完整一行后置为0
    // m_read_idx 在 LINE_OK 状态下，代表一行结尾的下一个位置
    for(; m_read_idx < size; m_read_idx++){
        tmp = m_inbuff[m_read_idx];
        if(tmp == '\r'){
            if(m_read_idx + 1 >= size){
                return LINE_AGAIN;
            }
            else if(m_inbuff[m_read_idx + 1] == '\n'){
                m_read_idx += 2;
                return LINE_OK;
            }
            else{
                return LINE_BAD;
            }
        }
        else if(tmp == '\n'){
            if(m_read_idx > 0 && m_inbuff[m_read_idx - 1] == '\r'){
                m_read_idx += 1;
                return LINE_OK;
            }
            else return LINE_BAD;
        }
    }
    return LINE_AGAIN;
}


HTTP_CODE HttpConn::analyseRequest(){
    LINE_STATE line_state = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    while((m_check_state == CHECK_STATE_CONTENT && line_state == LINE_OK)
        || ((line_state = parseLine()) == LINE_OK)){
        switch(m_check_state){
            case CHECK_STATE_REQUESTLINE:
                ret = parseRequestLine();
                if(ret == BAD_REQUEST)
                    return BAD_REQUEST;
                break;
            case CHECK_STATE_HEADER:
                ret = parseHeaders();
                if(ret == BAD_REQUEST)
                    return BAD_REQUEST;
                else if(ret == GET_REQUEST) // 读取完所有首部了,开始处理
                    return doRequest(); // doRequest返回什么
                break;
            case CHECK_STATE_CONTENT:
                ret = parseContent(); // 两个HTTP报文怎么分割？没有分割符号
                if(ret == GET_REQUEST)
                    return doRequest();
                // 若剩下的数据不够组成content，那么下一次循环会调用parseLine()函数，
                // 调用parseLine导致m_read_idx移动到最后面
                // 下一次数据达到，由于处于CHECK_STATE_CONTENT状态，且line_state = LINE_OK
                // 所以switch到达这个case
                // 获取到合适的content
                // m_inbuff应该裁剪掉相应的已经被使用部分
                // 此时m_read_idx应该被复位
                line_state = LINE_AGAIN;
                break;
            default:
                return INTERNAL_ERROR;
        }
    }
    if(line_state == LINE_BAD)
        return BAD_REQUEST;
    // 读取的数据不够
    else
        return NO_REQUEST;
}

HTTP_CODE HttpConn::parseRequestLine(){
    std::string line = m_inbuff.substr(0, m_read_idx);
    m_inbuff.erase(0, m_read_idx);
    m_read_idx = 0;

    int first_sep_pos = line.find(' ');
    if(first_sep_pos == std::string::npos){
        return BAD_REQUEST;
    }
    // 解析请求方法
    std::string method = line.substr(0, first_sep_pos);
    if(strcasecmp(method.c_str(), "GET"))
        m_method = GET;
    else if(strcasecmp(method.c_str(), "HEAD"))
        m_method = HEAD;
    else if(strcasecmp(method.c_str(), "POST"))
        m_method = POST;
    else{
        m_method = OTHERS;
        return NOT_IMPLEMENTED;
    }
    
    int second_sep_pos = line.find(' ', first_sep_pos + 1);
    if(second_sep_pos == std::string::npos){
        return BAD_REQUEST;
    }

    // 解析URI
    m_filename = line.substr(first_sep_pos + 1, second_sep_pos - first_sep_pos - 1);
    if(m_filename == '/')
        m_filename += "index.html";
    else{
        int pos = m_filename.find('?');
        if(pos != std::string::npos)
            m_filename.erase(pos);
    }
 
    // 解析HTTP版本
    std::string version = line.substr(second_sep_pos, 8);
    if(strcasecmp(version.c_str(), "HTTP/1.0")){
        m_version = HTTP10;
    }
    else if(strcasecmp(version.c_str(), "HTTP/1.1")){
        m_version = HTTP11;
    }
    else{
        return BAD_REQUEST;
    }
    // 转移状态
    m_check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

HTTP_CODE HttpConn::parseHeaders(){
    std::string header = m_inbuff.substr(0, m_read_idx - 2);
    m_inbuff.erase(0, m_read_idx);
    m_read_idx = 0;

    // 如果是空行
    if(header == ""){
        if(m_method == POST){ // 如果是POST方法，还需要接收content
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        else return GET_REQUEST; // 否则得到一个完整的请求
    }
    else{
        int colon_pos = header.find(':');
        if(colon_pos == std::string::npos)
            return BAD_REQUEST;
        std::string key = header.substr(0, colon_pos);
        std::string value = header.substr(colon_pos + 2);
        m_headers[key] = value;
        return NO_REQUEST;
    }
}

HTTP_CODE HttpConn::parseContent(){
    if(m_headers.find("content-length") == m_headers.end())
        return BAD_REQUEST;
    int content_len = m_headers.find("content-length");
    if(m_inbuff.size() >= content_len){
        m_content = m_inbuff.substr(0, content_len);
        m_inbuff.erase(0, content_len); // 清楚前面已经处理的部分
        m_read_idx = 0;
        return GET_REQUEST;
    }
    return NO_REQUEST;
}


HTTP_CODE HttpConn::doRequest(){
    // TODO: 根据GET、POST、HEAD请求，做出相应的操作，返回相应的结果，GET、HEAD主要判断文件是否存在
    std::string full_filename = m_path + "/" + m_filename;
    if(m_method == POST){
        
    }
    else if(m_method == GET || m_method == HEAD){
        struct stat st;
        int ret = stat(full_filename.c_str(), &st);
        if(ret < 0) return NO_RESOURCE; // 不存在这个文件
        if(!(st.st_mode & S_IROTH)) return FORBIDDEN_REQUEST; // 没有权限
        if(S_ISDIR(st.st_mode)) return BAD_REQUEST; // 请求的是一个目录
        // 正常的请求
        if(m_method == HEAD) return FILE_REQUEST;
        m_file_size = st.st_size;
        int fd = open(full_filename.c_str(), O_RDONLY);
        m_file_ptr = (char*)mmap(0, m_file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if(m_file_ptr == (char*)(-1)){
            munmap((void*)m_file_ptr, m_file_size);
            LOG_ERROR("HttpConn::doRequest() mmap Error.");
            return INTERNAL_ERROR;
        }
        close(fd);
        return FILE_REQUEST;
    }
    else{
        return NOT_IMPLEMENTED;
    }
}

// 设置对应的Timer的delete标志为true，设置对应的Callback为nullptr
// 设置m_timer成员reset
void HttpConn::reset(){
    // 其他需要在一次请求的完成后清空的内容，注意是请求完成后
    m_filename.clear();
    // m_path.clear(); // path设置为恒定的。
    m_headers.clear();
    m_keep_alive = false;
    m_version = HTTP11;
    m_method = GET;
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_content.clear();
    m_error_exist = false;
    m_file_size = 0;
    m_file_ptr = nullptr;
    // TODO: 可能还需要添加
    seperateTimer();
}

void HttpConn::seperateTimer(){
    if(m_timer){
        m_timer->setDeleted();
        m_timer.reset();
    }
}


void HttpConn::add_content(string content){
    m_outbuff += content;
}

void HttpConn::add_status_line(int status){

}

void HttpConn::add_headers(string header, string value){

}

void HttpConn::add_content_type(){

}

void HttpConn::add_content_length(){

}

void HttpConn::add_linger(){ // linger的用处?

}

void HttpConn::add_blank_line(){
    m_outbuff += "\r\n";
}