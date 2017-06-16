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
#ifndef INETWORKINGTOOL_H_
#define INETWORKINGTOOL_H_

namespace fire {
namespace util {

class PostResponse {
public:

	PostResponse(std::istream& str) :
			successful(false), status_code("400 Error"), contentLength(0), contentType(
					""), content(str) {
	}
	bool successful;
	std::string status_code;
	int contentLength;
	std::string contentType;
	std::istream& content;
};
/**
 * The INetworkingTool interface provides methods that enable
 * typical HTTP GET and POST behavior.
 *
 */
class INetworkingTool {

public:

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
					std::string, std::string>()) = 0;

	/**
	 * Issue an HTTP Post command at the given relative path with
	 * the provided message. Clients can provide a map of header key values to modify the
	 * POST request.
	 *
	 * @param relativePath The path relative to the hostname/port provided to this NetworkingTool
	 * @param message The message to post
	 * @param header The map of additional HTTP POST header information
	 * @return postResponse Struct containing state of post response
	 *
	 */
	virtual PostResponse post(const std::string& relativePath,
			const std::string& message,
			const std::map<std::string, std::string>& header = std::map<
					std::string, std::string>()) = 0;

	/**
	 * virtual destructor
	 */
	virtual ~INetworkingTool() {}
protected:

	/**
	 * The host name that this NetworkingTool interacts with
	 */
	std::string hostName;

	/**
	 * The port on the host to establish connection with.
	 */
	int port;

};
}
}

#endif
