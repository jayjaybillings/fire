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
/**
 *
 */
class INetworkingTool {

public:

	virtual ~INetworkingTool() {
	}

	/**
	 * Execute HTTP GET to return the contents located at url.
	 *
	 * @param url The URL of the GET request.
	 * @param username The username. It is ignored if it is empty. It may not be null.
	 * @param password The password. It is ignored if it is empty. It may not be null.
	 * @return The contents at the URL or an error message if one took place.
	 */
	virtual std::string get(const std::string& url, const std::string& username,
			const std::string& password) = 0;

	/**
	 * Execute HTTP POST to transmit value at url.
	 *
	 * @param url The url that is used to post the value.
	 * @param username The username. It is ignored if it is empty. It may not be null.
	 * @param password The password. It is ignored if it is empty. It may not be null.
	 * @param value The value that is posted to the url.
	 * @return A std::string containing the error if one took place. Else returns an empty std::string.
	 */
	virtual std::string post(const std::string& url, const std::string& value,
			const std::string& username, const std::string& password) = 0;

};
}
}

#endif
