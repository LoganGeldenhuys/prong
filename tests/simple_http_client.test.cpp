#include "simple_http_client.h"
#include <iostream>
#include <sstream> // Include for std::ostringstream
#include <map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main() {
    // The target host, port, and URI
    std::string host = "jsonplaceholder.typicode.com"; // Changed to a host that returns JSON
    std::string port = "80";
    std::string target = "/posts/1"; // Targeting a specific JSON resource

    // Optional headers
    std::map<std::string, std::string> headers = {
        {"Accept", "application/json"},
        {"Connection", "close"}
    };

    // Make an asynchronous HTTP GET request with headers
    auto future_response_with_headers = simple_http_client::http_get(host, port, target, headers);
    std::cout << "Request with headers sent. Waiting for response...\n";
    std::string response_with_headers = future_response_with_headers.get();
    std::cout << "Response with headers received:\n" << response_with_headers << std::endl;

    // Make an asynchronous HTTP GET request without specifying headers
    auto future_response_without_headers = simple_http_client::http_get(host, port, target);
    std::cout << "\nRequest without headers sent. Waiting for response...\n";
    std::string response_without_headers = future_response_without_headers.get();
    std::cout << "Response without headers received:\n" << response_without_headers << std::endl;

    // Make an asynchronous HTTP GET JSON request
    auto future_json_response = simple_http_client::http_get_json(host, port, target, headers);
    std::cout << "\nJSON request sent. Waiting for response...\n";
    boost::property_tree::ptree json_response = future_json_response.get();
    std::cout << "JSON response received. Title: " << json_response.get<std::string>("title") << std::endl;

    // Make an asynchronous HTTP GET stream request
    auto response = simple_http_client::http_get_stream(std::cout, host, port, target, headers);
    std::cout << "\nStream request sent. Waiting for response...\n";
	//std::cout<< "\nResponse is:\n"<<response.get() <<std::endl;
	
	response.get();
	int i;

	std::cin >> i; 

    return 0;
}

