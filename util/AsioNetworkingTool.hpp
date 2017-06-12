/*******************************************************************************
 * Copyright (c) 2017, UT-Battelle, LLC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in
 *  the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *  contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *******************************************************************************/
#ifndef ASIONETWORKINGTOOL_H_
#define ASIONETWORKINGTOOL_H_

#include "INetworkingTool.hpp"
#include "client_https.hpp"
#include <boost/algorithm/string.hpp>
#include <thread>
#include <chrono>

namespace fire {

namespace util {

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

/**
 * The AsioNetworkignTool is a realization of the INetworkingTool
 * interface that uses the BOOST Asio library to post/get
 * http requests.
 */
template<typename PROTOCOL>
class AsioNetworkingTool: public INetworkingTool {

	/**
	 * Convenience name for SimpleWeb Client
	 */
	using WebClient = SimpleWeb::Client<PROTOCOL>;

protected:

	/**
	 * Reference to the asio client we will use
	 */
	std::shared_ptr<WebClient> client;

	std::string responseMessage;
	std::string status_code;

public:
	/**
	 * The constructor
	 */
	AsioNetworkingTool(std::string host, int p) {
		static_assert(std::is_same<PROTOCOL, SimpleWeb::HTTP>::value, "This constructor can only be used with HTTP clients");
		client = std::make_shared<WebClient>(host + ":" + std::to_string(p));
	}

	AsioNetworkingTool(std::string host_and_port, bool verifyCert = true) {
		static_assert(std::is_same<PROTOCOL, SimpleWeb::HTTPS>::value, "This constructor can only be used with HTTPS clients");
		client = std::make_shared<WebClient>(host_and_port, verifyCert);
	}

	/**
	 * The destructor
	 */
	virtual ~AsioNetworkingTool() {
		client->close();
	}

	/**
	 * Return the last received status code.
	 *
	 * @return code The status code as a string
	 */
	virtual std::string getLastStatusCode() {
		return status_code;
	}

	virtual std::string getLastRequestMessage() {
		return responseMessage;
	}

	/**
	 * Issue an HTTP GET Command at the given relative path.
	 * Clients can provide a map of header key values to modify the
	 * GET request.
	 *
	 * @param relativePath The path relative to the hostname/port provided to this NetworkingTool
	 * @return The contents at the URL or an error message if one took place.
	 */
	virtual std::string get(const std::string& relativePath,
			const std::map<std::string, std::string>& header = std::map<
					std::string, std::string>()) {
		return "";
	}

	/**
	 * Issue an HTTP Post command at the given relative path with
	 * the provided message. Clients can provide a map of header key values to modify the
	 * POST request.
	 *
	 * @param relativePath The path relative to the hostname/port provided to this NetworkingTool
	 * @param message The message to post
	 * @param header The map of additional HTTP POST header information
	 * @return success Boolean indicating if post was successful
	 *
	 */
	virtual bool post(const std::string& relativePath,
			const std::string& message,
			const std::map<std::string, std::string>& header = std::map<
					std::string, std::string>()) {
		auto response = client->request("POST", relativePath, message, header);
		status_code = response->status_code;
		auto buf = response->content.rdbuf();
		responseMessage = std::string(std::istreambuf_iterator<char>(buf),
	               std::istreambuf_iterator<char>());
		return boost::contains(status_code, "200 OK");
	}

	/**
	 *
	 * @param bytes_to_encode
	 * @param in_len
	 * @return
	 */
	std::string base64_encode(char const* bytes_to_encode, int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
						+ ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
						+ ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
					+ ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
					+ ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';
		}

		return ret;
	}


};

}
}
#endif
