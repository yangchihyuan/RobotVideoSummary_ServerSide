#include <SocketToServer.hpp>
#include <mutex>

char read_buffer[1024];
char debug_buffer[20];
char frame_buffer[200000];      //frame size is usually 45k, I conservatively set 200k
int buffer_length = 0;
int expected_frame_length = 0;

char *frame_buffer1 = NULL;
char *frame_buffer2 = NULL;
char *frame_buffer3 = NULL;
int status_frame_buffer1 = 0;
int status_frame_buffer2 = 0;
int frame_buffer1_length = 0;
int frame_buffer2_length = 0;

char str_results[122880];       //Chih-Yuan: Is it good practice to declare a large variable?

std::mutex gMutex;
std::mutex gMutex_save_JPEG;
std::mutex gMutex_send_results;

class session
    : public std::enable_shared_from_this<session>
{
  public:

    session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(read_buffer, 1024), 
            [this, self](boost::system::error_code ec, std::size_t length) {
                if( !ec)
                {
                    if( strncmp(read_buffer, "Begin:", 6 ) == 0 )
                    {
                        buffer_length = 0;
                        expected_frame_length = 0;
                    }

                    //accumulate data into the buffer
                    if( length > 0 && length + buffer_length <= 200000)
                    {
                        memcpy(frame_buffer + buffer_length, read_buffer, length);
                    }
                    buffer_length += length;

                    if( buffer_length == expected_frame_length && expected_frame_length > 0 )
                    {
                        //check the correctness of the JPEG data
                        if( static_cast<int>(static_cast<unsigned char>(frame_buffer[30])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[31])) == 0xD8 &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[32])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[33])) == 0xE0 &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[buffer_length-2])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[buffer_length-1])) == 0xD9 )
                        {
                            if( status_frame_buffer1 == 0 )
                            {
//                                std::cout << "save a frame to buffer 1" << std::endl;
                                memcpy(frame_buffer1, frame_buffer+6, expected_frame_length-6);     //6 is the length of the string "Begin:"
                                frame_buffer1_length = expected_frame_length-6;
                                status_frame_buffer1 = 1;
                                gMutex.unlock();
                            }
                            else
                            {
//                                std::cout << "save a frame to buffer 2" << std::endl;
                                memcpy(frame_buffer2, frame_buffer+6, expected_frame_length-6);
                                frame_buffer2_length = expected_frame_length-6;
                                status_frame_buffer2 = 1;
                                gMutex_save_JPEG.unlock();
                            }
                        }
                    }

                    bool bJPEG_data_corrupt = false;
                    //corrupted cases
                    if( buffer_length > expected_frame_length && expected_frame_length > 0 )
                    {
//                        std::cout << "expected_frame_length " << expected_frame_length << " but actual length " << buffer_length << std::endl;
                        
                        std::string key_info(frame_buffer+6);
//                        std::cout << "key " << key_info << std::endl;
                        bool bHeadOK = false, bEndOK = false, bMiddleAsEnd = false;
                        if( static_cast<int>(static_cast<unsigned char>(frame_buffer[30])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[31])) == 0xD8 &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[32])) == 0xFF && 
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[33])) == 0xE0)
                        {
//                            std::cout << "JPEG signature beginning ok" << std::endl;
                            bHeadOK = true;
                        }
                        else{
                            std::cout << "JPEG signature beginning failed" << std::endl;
                        }

                        if( static_cast<int>(static_cast<unsigned char>(frame_buffer[buffer_length-2])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[buffer_length-1])) == 0xD9 )
                        {
//                            std::cout << "JPEG signature end ok" << std::endl;
                            bEndOK = true;
                        }
                        else{
//                            std::cout << "JPEG signature end failed" << std::endl;
                        }

                        //extra check
                        if( static_cast<int>(static_cast<unsigned char>(frame_buffer[expected_frame_length-2])) == 0xFF &&
                            static_cast<int>(static_cast<unsigned char>(frame_buffer[expected_frame_length-1])) == 0xD9 )
                        {
//                            std::cout << "JPEG signature middle as end" << std::endl;
                            bMiddleAsEnd = true;
                        }
                        else{
//                            std::cout << "JPEG signature middle not as end" << std::endl;
                        }

                        if( bHeadOK && bEndOK )
                        {
                            std::cout << "suspicious case" << std::endl;
/*                            if( status_frame_buffer1 == 0 )
                            {
                                memcpy(frame_buffer1, frame_buffer+6, buffer_length-6);     //6 is the length of the string "Begin:"
                                frame_buffer1_length = buffer_length-6;
                                status_frame_buffer1 = 1;
                                gMutex.unlock();
                            }
                            else
                            {
                                memcpy(frame_buffer2, frame_buffer+6, buffer_length-6);
                                frame_buffer2_length = buffer_length-6;
                                status_frame_buffer2 = 1;
                                gMutex_save_JPEG.unlock();
                            }
*/
                        }
                        //this is the normal case
                        else if( bHeadOK && bMiddleAsEnd )
                        {
                            if( status_frame_buffer1 == 0 )
                            {
                                memcpy(frame_buffer1, frame_buffer+6, expected_frame_length-6);
                                memcpy(frame_buffer, frame_buffer+expected_frame_length, buffer_length - expected_frame_length);
                                buffer_length -= expected_frame_length;
                                frame_buffer1_length = expected_frame_length-6;
                                status_frame_buffer1 = 1;
                                gMutex.unlock();
                                expected_frame_length = 0;  //set to 0, to recompute the expected length again.
                            }
                            else
                            {
                                memcpy(frame_buffer2, frame_buffer+6, expected_frame_length-6);
                                memcpy(frame_buffer, frame_buffer+expected_frame_length, buffer_length - expected_frame_length);
                                buffer_length -= expected_frame_length;
                                frame_buffer2_length = expected_frame_length-6;
                                status_frame_buffer2 = 1;
                                gMutex_save_JPEG.unlock();
                                expected_frame_length = 0;  //set to 0, to recompute the expected length again.
                            }
                        }
                        else
                        {
                            bJPEG_data_corrupt = true;
                            buffer_length = 0; //reset

                            std::cout << "error: JPEG data corrupted" << std::endl;
                            system_clock::time_point current_time = system_clock::now();
                            std::time_t t = system_clock::to_time_t(current_time);
                            std::string dt = ctime(&t);
                            std::cout << "current time:" << dt <<std::endl;
                        }
                    }

                    //to know the expected length
                    if( buffer_length >= 30 && expected_frame_length == 0)
                    {
                        std::string key_info(frame_buffer+6);
                        int key_length = key_info.length();
                        if( key_length == 17 )
                        {
                            std::string str_JPEG_length(frame_buffer + 6 + key_info.length() + 1);
                            int expected_JPEG_length = 0;
                            try{
                                expected_JPEG_length = std::stoi(str_JPEG_length);      //sometimes the frame_buffer is corrupted, why?
                            }
                            catch( std::exception &e)
                            {
                                std::cout << "error. We get an exception" << std::endl;
                                std::cout << "buffer_length " << buffer_length << std::endl;
                                std::cout << "key_info " << key_info << std::endl;
                                std::cout << "str_JPEG_length " << str_JPEG_length << std::endl;
                                std::cout << e.what() << std::endl;
                            }
                            expected_frame_length = expected_JPEG_length + 6 + key_length + 2 + str_JPEG_length.length();
                        }
                        else{
                            //this frame must be corrupted. Skip it.
                            std::cout << "frame buffer corrupted" << std::endl;
                            std::cout << "key_length " << key_length << std::endl;
                            std::cout << "key_info " << key_info << std::endl;
                            bJPEG_data_corrupt = true;   
                        }
                    }

                    if( !bJPEG_data_corrupt)
                        do_read();
                }
                else{
//                    std::cout << "error_code " << ec.message() << std::endl;
                }
            });
    }

    private:
    tcp::socket socket_;
};

void receive_socket(short port_number)
{
    frame_buffer1 = new char[100000];
    frame_buffer2 = new char[100000];
    frame_buffer3 = new char[100000];

    try
    {
        boost::asio::io_service io_service;
        server s(io_service, port_number);
        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    free(frame_buffer1);
    free(frame_buffer2);
    free(frame_buffer3);

}

server::server(boost::asio::io_service &io_service, short port_number)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port_number)),
          socket_(io_service),
          port_number(port_number)
{
    do_accept();
    memset(frame_buffer, 0, 200000);
}

void server::do_accept()
{
    std::cout << "wait for connection " << port_number << "\n";
    acceptor_.async_accept(socket_,
                            [this](boost::system::error_code ec) {
                                if (!ec)
                                {
                                    std::cout << "port " << port_number << " connection accepted" << "\n";
                                    std::make_shared<session>(std::move(socket_))->do_read();
                                }

                                do_accept();
                            });
}

class session_report_result
    : public std::enable_shared_from_this<session_report_result>
{
  public:
    session_report_result(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_write();
    }
    bool bVerbose = true;

  private:
    void do_write()
    {
        auto self(shared_from_this());
        int str_result_length = strlen(str_results);
        gMutex_send_results.lock();
        strcat(str_results, "EndToken\r\n");
        if(bVerbose)
        {
            std::cout << "session_report_result do_write()" << std::endl;
            std::cout << "strlen(str_results) " << strlen(str_results) << std::endl;
            std::cout << str_results << std::endl;
        }
        boost::asio::async_write(socket_, boost::asio::buffer(str_results, strlen(str_results)),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec)
                                    {
                                        do_write();
                                    }
                                    else
                                    {
                                        std::cout << "error_code " << ec.message() << std::endl;
                                    }
                                    
                                });
        memset(str_results, 0, 1);
    }

    tcp::socket socket_;
};

class server_report_results
{
  public:
    server_report_results(boost::asio::io_service &io_service, short port_number)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port_number)),
          socket_(io_service),
          port_number(port_number)
    {
        do_accept();
    }

  private:
    void do_accept()
    {
        std::cout << "wait for connection " << port_number << "\n";
        acceptor_.async_accept(socket_,
                                [this](boost::system::error_code ec) {
                                    if (!ec)
                                    {
                                        std::cout << "port " << port_number << " connection accepted" << "\n";
                                        std::make_shared<session_report_result>(std::move(socket_))->start();
                                    }

                                    do_accept();
                                });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    short port_number;
};

void report_results(short port_number)
{
    //initialize str_results
    memset(str_results, 0, 1);
    gMutex_send_results.lock();

    
    try
    {
        boost::asio::io_service io_service;
        server_report_results s(io_service, port_number);

        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

}
