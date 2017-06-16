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
#include "server_https.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;

using namespace fire::util;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpsServer = SimpleWeb::Server<SimpleWeb::HTTPS>;

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

	AsioNetworkingTool<SimpleWeb::HTTP> asioTool("localhost", 8080);
    std::string json_string="{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

    auto response = asioTool.post("/json", json_string);
    BOOST_VERIFY(response.successful);
    BOOST_VERIFY(serverWasPinged);

    std::stringstream ss;
    ss << response.content.rdbuf();
    BOOST_VERIFY(ss.str() == "John Smith");

    // FIXME figure this out
//    BOOST_VERIFY(!asioTool.post("/badpath", json_string));

    server.stop();
    server_thread.join();
}

const std::string testServerKey = "-----BEGIN RSA PRIVATE KEY-----\n"
		"MIICXAIBAAKBgQCd9SiiUk+A0c+MQktXfAV1Pcvg6ltujndENrDNsWht9CYfv0Fh\n"
		"/OgEkNaHFj6aNAOE4K4nucqW3/Ex0d2+KygxaxcrUBY2wMatu16pZe4OZwnvkvdS\n"
		"l5bMe6okWne61vdK6EqKJH9D4PccG1z+E44mVr5jvEq1Q5Or0DXrPwrIOwIDAQAB\n"
		"AoGANdz/ujpvOfJR3/ZLUs3L3qrv9wZzn0Odj/lGWR4CClyTE4hYd12SkNwDC5We\n"
		"S18pRAqJ2KoWV1SOHvr9rndxAL/PGQne530F+SCG7C2T9pHovqdJX3goZ4Mr6L6R\n"
		"yM2zH9KEmDmtTloCCdu1f1n1CLn7WC9er2I0kMmNS2vAxeECQQDRP9TNrg5EUZsX\n"
		"Q9NAK33MMpiUyIjymilZPOvYPp79TdNiPh/0yy30rZEtLE/WLvyrrcmKeZ2H+lkV\n"
		"9ICaLyf9AkEAwT+pbwZ/rwbNvnN9zK7cO7mv+evP54UYTYEzEs3ZjbzsWPTJJYOD\n"
		"8j1PY66D0OjqLAyXUpdrNTUf+ta8JZyalwJBAJ03MFQ46k86YSRhuV8FTkcV3L4s\n"
		"zXg5i8eYljqrEhmPV7sAolHyNl0X63PdTz7FnuH+eOap6Uo5NqXgRp+RvqECQER1\n"
		"uh9z7O3jeiW7IljNpYWfDaF7EacSNn7Wg3Jo7GZTBGOrLEsW4TqCz8ofe+EZckPy\n"
		"nfULYVZzIIYrZnHonbcCQFMjG6j0IITk7VUFihSn83qWhYE0SZyvEM97dlrc4qUX\n"
		"PsnIyfBo4bYPaNr34d9N5MaVeEp+XnCOUR14IOcNSvk=\n"
		"-----END RSA PRIVATE KEY-----";
const std::string testServerCRT = "-----BEGIN CERTIFICATE-----\n"
		"MIIB+zCCAWQCCQDcaM69MWadDzANBgkqhkiG9w0BAQsFADBCMQswCQYDVQQGEwJY\n"
		"WDEVMBMGA1UEBwwMRGVmYXVsdCBDaXR5MRwwGgYDVQQKDBNEZWZhdWx0IENvbXBh\n"
		"bnkgTHRkMB4XDTE3MDYxMjE5NDkyN1oXDTE4MDYxMjE5NDkyN1owQjELMAkGA1UE\n"
		"BhMCWFgxFTATBgNVBAcMDERlZmF1bHQgQ2l0eTEcMBoGA1UECgwTRGVmYXVsdCBD\n"
		"b21wYW55IEx0ZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAnfUoolJPgNHP\n"
		"jEJLV3wFdT3L4Opbbo53RDawzbFobfQmH79BYfzoBJDWhxY+mjQDhOCuJ7nKlt/x\n"
		"MdHdvisoMWsXK1AWNsDGrbteqWXuDmcJ75L3UpeWzHuqJFp3utb3SuhKiiR/Q+D3\n"
		"HBtc/hOOJla+Y7xKtUOTq9A16z8KyDsCAwEAATANBgkqhkiG9w0BAQsFAAOBgQB7\n"
		"/hB/BLDGf01YY1kcLMVw28LTMf1JDVZooyVBBzCxwHIvMOLzDppYJayVU3IZfl2E\n"
		"mnE4Ji8jAW9gDXWnlXSGfchGU7fAI+ii6Kc4AsNlrZ+3TThatOAtWXz+1uBMip4Q\n"
		"JgalnTZ9W6yM6pX+gjfu8fi41uKrcDAzJX/iQsSb2A==\n"
		"-----END CERTIFICATE-----";

BOOST_AUTO_TEST_CASE(checkSimpleHTTPSPost) {

	std::ofstream serverKey("server.key");
	serverKey << testServerKey;
	std::ofstream serverCrt("server.crt");
	serverCrt << testServerCRT;
	serverKey.close();
	serverCrt.close();

	//HTTPS-server at port 8080 using 1 thread
	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	HttpsServer server("server.crt", "server.key");
	server.config.port = 8080;

	bool serverWasPinged = false;

	auto postResponse = [&](std::shared_ptr<HttpsServer::Response> response, std::shared_ptr<HttpsServer::Request> request) {
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

	AsioNetworkingTool<SimpleWeb::HTTPS> asioTool("localhost:8080", false);
    std::string json_string="{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

    auto response = asioTool.post("/json", json_string);
    BOOST_VERIFY(response.successful);
    BOOST_VERIFY(response.status_code == "200 OK");
    BOOST_VERIFY(serverWasPinged);

    std::stringstream ss;
    ss << response.content.rdbuf();

    BOOST_VERIFY(ss.str() == "John Smith");

    // FIXME figure this out
//    BOOST_VERIFY(!asioTool.post("/badpath", json_string));

    server.stop();
    server_thread.join();

    std::remove("server.key");
    std::remove("server.crt");

}
