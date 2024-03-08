#ifndef PRONG_SIMPLE_HTTP_CLIENT_H
#define PRONG_SIMPLE_HTTP_CLIENT_H

// This is a simple http client based on the sample http client provided at libs/beast/example/http/client/sync/http_client_sync.cpp in the Boost library v1.84
//
// The original had the following copyright:
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//


#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <future>
#include <map> // Include for using std::map

namespace Prong {

	namespace beast = boost::beast;     
	namespace http = beast::http;       
	namespace net = boost::asio;        
	using tcp = net::ip::tcp;     
	

	// Function signature with headers defaulted to an empty map
	std::future<std::string> httpRequest(
		const std::string& host, 
		const std::string& port, 
		const std::string& target, 
		const http::verb& verb,
		const std::string& payload,
		const std::map<std::string, std::string>& headers = {}, // Default to an empty map
		int version = 11) {
		return std::async(std::launch::async, [host, port, target, verb, payload, headers, version]() -> std::string {
			try {
				net::io_context ioc;
				tcp::resolver resolver(ioc);
				beast::tcp_stream stream(ioc);

				auto const results = resolver.resolve(host, port);
				stream.connect(results);

				http::request<http::string_body> req{verb, target, version};
				req.set(http::field::host, host);
				req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

				// set the payload for post and other methods that allow a body
				if (!payload.empty() && (verb == http::verb::post || verb == http::verb::put || verb == http::verb::patch)) {
					req.body() = payload;
					req.prepare_payload(); // this sets the content-length and content-type headers
				}

				// Iterate over the headers map and add each header to the request
				for(const auto& header : headers) {
					req.set(header.first, header.second);
				}

				std::cout << "Request Method: " << req.method_string().to_string() << std::endl;
				std::cout << "Request Target: " << req.target().to_string() << std::endl;
				std::cout << "Request Version: " << "HTTP/" << req.version() / 10 << "." << req.version() % 10 << std::endl;
				 // Diagnostic print for headers
				std::cout << "Headers: " << std::endl;
				for(auto const& field : req.base()) {
					std::cout << field.name_string() << ": " << field.value() << std::endl;
				}
				std::cout<<"With body:"<<req.body()<<std::endl;

				//add other request info
				http::write(stream, req);
				beast::flat_buffer buffer;
				http::response<http::dynamic_body> res;
				http::read(stream, buffer, res);

				beast::error_code ec;
				stream.socket().shutdown(tcp::socket::shutdown_both, ec);
				if(ec && ec != beast::errc::not_connected)
					throw beast::system_error{ec};
			
				std::cout<<"Printing response:"<<beast::buffers_to_string(res.body().data())<<std::endl;
				return beast::buffers_to_string(res.body().data());
			} catch(std::exception const& e) {
				return std::string("Error: ") + e.what();
			}
			});	
		}

	std::future<std::string> httpStreamRequest(
		std::ostream& os, 
		const std::string& host, 
		const std::string& port, 
		const std::string& target, 
		const http::verb& verb,
		const std::string& payload,
		const std::map<std::string, std::string>& headers = {}, // Default to an empty map
		int version = 11) {
		return std::async(std::launch::async, [&os, host, port, target, verb, payload, headers, version]() -> std::string {
			try {
				net::io_context ioc;
				tcp::resolver resolver(ioc);
				beast::tcp_stream stream(ioc);

				auto const results = resolver.resolve(host, port);
				stream.connect(results);

				http::request<http::string_body> req{verb, target, version};
				req.set(http::field::host, host);
				req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

				// set the payload for post and other methods that allow a body
				if (!payload.empty() && (verb == http::verb::post || verb == http::verb::put || verb == http::verb::patch)) {
					req.body() = payload;
					req.prepare_payload(); // this sets the content-length and content-type headers
				}

				// Iterate over the headers map and add each header to the request
				for(const auto& header : headers) {
					req.set(header.first, header.second);
				}

				http::write(stream, req);
				beast::flat_buffer buffer;

				http::parser<false, http::dynamic_body> parser;

				bool touched = false;

				std::function<void(std::uint64_t, boost::beast::string_view, boost::system::error_code&)> on_chunk_header = [&os, &touched](std::uint64_t size, boost::beast::string_view extensions, boost::system::error_code& ec) {
					touched = true;
					if (ec) {
						std::cout << "Error during chunk header parsing: " << ec.message() << std::flush;
					} else {
						std::cout << "Chunk header: size=" << size;
						if (!extensions.empty()) {
							std::cout << ", extensions=" << extensions;
						}
						std::cout << std::flush;
					}
				};

				// Callback for chunk body
				std::function<std::size_t(std::uint64_t, boost::beast::string_view, boost::system::error_code)> on_chunk_body = [&os, &touched](std::uint64_t remain, boost::beast::string_view body, boost::system::error_code ec) -> std::size_t{
					touched = true;
					if (ec) {
						std::cout << "Error during chunk body parsing: " << ec.message() << std::flush;
						return 0; // Indicate that no octets were consumed due to the error
					} else {
						std::size_t consumed = body.size(); // Assume we can process the entire chunk at once
						std::cout << "Chunk body: remain=" << remain << ", body=\"" << body << "\"" << std::flush;
						return consumed; // Return the number of octets consumed
					}
				};

				std::cout<<"Touched is:"<<touched<<std::flush;

				// Before calling http::read, set the callbacks on the parser
				parser.on_chunk_header(on_chunk_header);
				parser.on_chunk_body(on_chunk_body);

				http::read(stream, buffer, parser);

				beast::error_code ec;
				stream.socket().shutdown(tcp::socket::shutdown_both, ec);
				if(ec && ec != beast::errc::not_connected)
					throw beast::system_error{ec};

				return beast::buffers_to_string(parser.get().body().data());
			} catch(std::exception const& e) {
				return std::string("Error: ") + e.what();
			}
		});	
	}

	boost::property_tree::ptree httpRequestJson(
		const std::string& host, 
		const std::string& port, 
		const std::string& target, 
		const http::verb& verb,
		const std::string& body,
		const std::map<std::string, std::string>& headers = {}, 
		int version = 11) {
		try{
				// Reuse the http_get function to perform the HTTP GET request
				auto future_response = httpRequest(host, port, target, verb, body, headers, version);
				std::string response = future_response.get(); // Get the response string from the future
				
				std::cout<<"Got response:\n"<<response<<std::endl;

				// Parse the JSON string into a property tree
				std::stringstream ss;
				ss << response;
				boost::property_tree::ptree pt;
				boost::property_tree::read_json(ss, pt);

				return pt;
			} catch (std::exception const& e) {
				// Handle parsing errors or other exceptions
				boost::property_tree::ptree error_ptree;
				error_ptree.put("error", e.what());
				return error_ptree;
			}
}
	std::future<boost::property_tree::ptree> httpRequestJsonStream(
		std::ostream& os, 
		const std::string& host, 
		const std::string& port, 
		const std::string& target, 
		const http::verb& verb,
		const std::string& body,
		const std::map<std::string, std::string>& headers = {}, 
		int version = 11) {
		return std::async(std::launch::async, [&os, host, port, target, verb, body, headers, version]() -> boost::property_tree::ptree {
			try {
				// Reuse the http_get function to perform the HTTP GET request
				auto future_response = httpStreamRequest(os, host, port, target, verb, body, headers, version);
				std::string response = future_response.get(); // Get the response string from the future

				// Parse the JSON string into a property tree
				std::stringstream ss;
				ss << response;
				boost::property_tree::ptree pt;
				boost::property_tree::read_json(ss, pt);

				return pt;
			} catch (std::exception const& e) {
				// Handle parsing errors or other exceptions
				boost::property_tree::ptree error_ptree;
				error_ptree.put("error", e.what());
				return error_ptree;
			}
		});
}

};


#endif // SIMPLE_HTTP_CLIENT_H

