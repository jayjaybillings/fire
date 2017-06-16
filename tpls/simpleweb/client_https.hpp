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
#ifndef CLIENT_HTTPS_HPP
#define	CLIENT_HTTPS_HPP

#include "client_http.hpp"

#ifdef USE_STANDALONE_ASIO
#include <asio/ssl.hpp>
#else
#include <boost/asio/ssl.hpp>
#endif

namespace SimpleWeb {
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;
using namespace boost::system;

template<>
class Client<HTTPS> : public ClientBase<HTTPS> {
public:
	Client(const std::string& server_port_path, bool verify_certificate = true,
			const std::string& cert_file = std::string(),
			const std::string& private_key_file = std::string(),
			const std::string& verify_file = std::string()) :
			ClientBase<HTTPS>::ClientBase(server_port_path, 443), context(
					boost::asio::ssl::context::tlsv12) {
		if (cert_file.size() > 0 && private_key_file.size() > 0) {
			context.use_certificate_chain_file(cert_file);
			context.use_private_key_file(private_key_file,
					boost::asio::ssl::context::pem);
		}

		if (verify_certificate)
			context.set_verify_callback(
					boost::asio::ssl::rfc2818_verification(host));

		if (verify_file.size() > 0)
			context.load_verify_file(verify_file);
		else
			context.set_default_verify_paths();

		if (verify_file.size() > 0 || verify_certificate)
			context.set_verify_mode(boost::asio::ssl::verify_peer);
		else
			context.set_verify_mode(boost::asio::ssl::verify_none);

		socket = std::unique_ptr<HTTPS>(new HTTPS(io_service, context));
		SSL_set_tlsext_host_name(socket->native_handle(), host.c_str());
	}

protected:
	boost::asio::ssl::context context;

	void connect() {
		if (!socket->lowest_layer().is_open()) {
			std::unique_ptr<boost::asio::ip::tcp::resolver::query> query;
			if (config.proxy_server.empty()) {
				query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(
						new boost::asio::ip::tcp::resolver::query(host,
								std::to_string(port)));
			} else {
				auto proxy_host_port = parse_host_port(config.proxy_server,
						8080);
				query = std::unique_ptr<boost::asio::ip::tcp::resolver::query>(
						new boost::asio::ip::tcp::resolver::query(
								proxy_host_port.first,
								std::to_string(proxy_host_port.second)));
			}
			auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(
					io_service);
			resolver->async_resolve(*query,
					[this, resolver] (const error_code &ec, boost::asio::ip::tcp::resolver::iterator it) {
						if(!ec) {
							auto timer=get_timeout_timer(config.timeout_connect);
							boost::asio::async_connect(socket->lowest_layer(), it, [this, resolver, timer] (const error_code &ec, boost::asio::ip::tcp::resolver::iterator /*it*/) {
										if(timer) timer->cancel();
										if(!ec) {
											boost::asio::ip::tcp::no_delay option(true);
											this->socket->lowest_layer().set_option(option);
										}
										else {
											close();
											throw system_error(ec);
										}
									});
						}
						else {
							close();
							throw system_error(ec);
						}
					});
			io_service.reset();
			io_service.run();

			if (!config.proxy_server.empty()) {
				auto write_buffer = std::make_shared<boost::asio::streambuf>();
				std::ostream write_stream(write_buffer.get());
				auto host_port = host + ':' + std::to_string(port);
				write_stream << "CONNECT " + host_port + " HTTP/1.1\r\n"
						<< "Host: " << host_port << "\r\n\r\n";
				auto timer = get_timeout_timer();
				boost::asio::async_write(socket->next_layer(), *write_buffer,
						[this, write_buffer, timer](const error_code &ec, size_t /*bytes_transferred*/) {
							if(timer)
							timer->cancel();
							if(ec) {
								close();
								throw system_error(ec);
							}
						});
				io_service.reset();
				io_service.run();

				std::shared_ptr<Response> response(new Response());
				timer = get_timeout_timer();
				boost::asio::async_read_until(socket->next_layer(),
						response->content_buffer, "\r\n\r\n",
						[this, response, timer](const error_code& ec, size_t /*bytes_transferred*/) {
							if(timer)
							timer->cancel();
							if(ec) {
								close();
								throw system_error(ec);
							}
						});
				io_service.reset();
				io_service.run();
				parse_response_header(response);
				if (response->status_code.empty()
						|| response->status_code.compare(0, 3, "200") != 0) {
					close();
					throw make_error_code(errc::permission_denied);
				}
			}

			auto timer = get_timeout_timer();
			this->socket->async_handshake(boost::asio::ssl::stream_base::client,
					[this, timer](const error_code& ec) {
						if(timer)
						timer->cancel();
						if(ec) {
							close();
							throw system_error(ec);
						}
					});
			io_service.reset();
			io_service.run();
		}
	}
};
}

#endif	/* CLIENT_HTTPS_HPP */
