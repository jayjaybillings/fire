/**----------------------------------------------------------------------------
 Copyright (c) 2017-, UT-Battelle LLC
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of fire nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author(s): Alex McCaskey (mccaskeyaj <at> ornl <dot> gov)
 -----------------------------------------------------------------------------*/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AsioNetworkingTester

#include <boost/test/included/unit_test.hpp>
#include "AsioNetworkingTool.hpp"
#include "server_http.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;

using namespace fire::util;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

BOOST_AUTO_TEST_CASE(checkSimplePost) {

	HttpServer server;
	server.config.port = 8080;
	bool serverWasPinged = false;

	auto postResponse = [&](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        try {
        	using namespace boost::property_tree;
            ptree pt;
            read_json(request->content, pt);
            std::string name=pt.get<std::string>("firstName")+" "+pt.get<std::string>("lastName");
            *response << "HTTP/1.1 200 OK\r\n"
                      << "Content-Type: application/json\r\n"
                      << "Content-Length: " << name.length() << "\r\n\r\n"
                      << name;
            serverWasPinged = true;
        } catch(std::exception& e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << std::string(e.what()).length() << "\r\n\r\n" << e.what();
        }
    };

	server.resource["^/json$"]["POST"] = postResponse;

	std::thread server_thread([&server]() {
		server.start();
	});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	AsioNetworkingTool asioTool("localhost", 8080);
    std::string json_string="{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

    BOOST_VERIFY(asioTool.post("/json", json_string));
    BOOST_VERIFY(serverWasPinged);

    // FIXME figure this out
//    BOOST_VERIFY(!asioTool.post("/badpath", json_string));

    server.stop();
    server_thread.join();
}
