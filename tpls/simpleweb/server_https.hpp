/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Ole Christian Eidheim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef SERVER_HTTPS_HPP
#define	SERVER_HTTPS_HPP

#include "server_http.hpp"

#ifdef USE_STANDALONE_ASIO
#include <asio/ssl.hpp>
#else
#include <boost/asio/ssl.hpp>
#endif

#include <openssl/ssl.h>
#include <algorithm>

using namespace boost::system;

namespace SimpleWeb {
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;
    
    template<>
    class Server<HTTPS> : public ServerBase<HTTPS> {
        std::string session_id_context;
        bool set_session_id_context=false;
    public:
        DEPRECATED Server(unsigned short port, size_t thread_pool_size, const std::string& cert_file, const std::string& private_key_file,
                long timeout_request=5, long timeout_content=300,
                const std::string& verify_file=std::string()) : 
                Server(cert_file, private_key_file, verify_file) {
            config.port=port;
            config.thread_pool_size=thread_pool_size;
            config.timeout_request=timeout_request;
            config.timeout_content=timeout_content;
        }
        
        Server(const std::string& cert_file, const std::string& private_key_file, const std::string& verify_file=std::string()):
                ServerBase<HTTPS>::ServerBase(443), context(boost::asio::ssl::context::tlsv12) {
            context.use_certificate_chain_file(cert_file);
            context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
            
            if(verify_file.size()>0) {
                context.load_verify_file(verify_file);
                context.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert |
                                        boost::asio::ssl::verify_client_once);
                set_session_id_context=true;
            }
        }
        
        void start() {
            if(set_session_id_context) {
                // Creating session_id_context from address:port but reversed due to small SSL_MAX_SSL_SESSION_ID_LENGTH
                session_id_context=std::to_string(config.port)+':';
                session_id_context.append(config.address.rbegin(), config.address.rend());
                SSL_CTX_set_session_id_context(context.native_handle(), reinterpret_cast<const unsigned char*>(session_id_context.data()),
                                               std::min<size_t>(session_id_context.size(), SSL_MAX_SSL_SESSION_ID_LENGTH));
            }
            ServerBase::start();
        }

    protected:
        boost::asio::ssl::context context;
        
        void accept() {
            //Create new socket for this connection
            //Shared_ptr is used to pass temporary objects to the asynchronous functions
            auto socket=std::make_shared<HTTPS>(*io_service, context);

            acceptor->async_accept((*socket).lowest_layer(), [this, socket](const error_code& ec) {
                //Immediately start accepting a new connection (if io_service hasn't been stopped)
                if (ec != boost::asio::error::operation_aborted)
                    accept();

                
                if(!ec) {
                    boost::asio::ip::tcp::no_delay option(true);
                    socket->lowest_layer().set_option(option);
                    
                    //Set timeout on the following boost::asio::ssl::stream::async_handshake
                    auto timer=get_timeout_timer(socket, config.timeout_request);
                    socket->async_handshake(boost::asio::ssl::stream_base::server, [this, socket, timer]
                            (const error_code& ec) {
                        if(timer)
                            timer->cancel();
                        if(!ec)
                            read_request_and_content(socket);
                        else if(on_error)
                            on_error(std::shared_ptr<Request>(new Request(*socket)), ec);
                    });
                }
                else if(on_error)
                    on_error(std::shared_ptr<Request>(new Request(*socket)), ec);
            });
        }
    };
}


#endif	/* SERVER_HTTPS_HPP */

