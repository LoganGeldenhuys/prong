#ifndef PRONG_SIMPLE_HTTPS_CLIENT
#define PRONG_SIMPLE_HTTPS_CLIENT

// This is adapted from a Boost example HTTPS client by Vinnie Falco, see
// copyright below
//
//-----------------------------------------------------------------------------
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, synchronous
//
//------------------------------------------------------------------------------

#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <map>  // Include for using std::map
#include <string>
#include "root_certificates.h"

namespace Prong {

namespace ssl = boost::asio::ssl;
namespace fs = std::filesystem;
namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;     // from <boost/asio.hpp>
namespace ssl = net::ssl;        // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

void load_system_certificates(ssl::context& ctx) {
  std::string cert_path = "/etc/ssl/certs";  // Common path on Linux systems

  // Check if the directory exists and is indeed a directory
  if (!fs::exists(cert_path) || !fs::is_directory(cert_path)) {
    throw std::runtime_error(
        "Certificate directory not found or is not a directory.");
  }

  // Iterate over the files in the directory
  for (const fs::directory_entry& entry : fs::directory_iterator(cert_path)) {
    if (entry.is_regular_file()) {
      std::string cert_file_path = entry.path().string();

      // Open the certificate file
      std::ifstream cert_file(cert_file_path);
      if (!cert_file) {
        std::cerr << "Failed to open certificate file: " << cert_file_path
                  << std::endl;
        continue;  // Skip this file and continue with the next one
      }

      // Read the certificate
      std::string cert((std::istreambuf_iterator<char>(cert_file)),
                       std::istreambuf_iterator<char>());

      // Add the certificate to the SSL context
      try {
        ctx.add_certificate_authority(
            boost::asio::buffer(cert.data(), cert.size()));
      } catch (const std::exception& e) {
        // std::cerr << "Failed to load certificate: " << cert_file_path << ";
        // Error: " << e.what() << std::endl;
      }
    }
  }
}

// Function signature with headers defaulted to an empty map
std::string httpsRequest(const std::string& host,
                         const std::string& port,
                         const std::string& target,
                         const http::verb& verb,
                         const std::string& payload,
                         const std::map<std::string, std::string>& headers =
                             {},  // Default to an empty map
                         const int version = 11) {
  try {
    std::string my_host = host;

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx(ssl::context::tlsv13_client);

    // note this is insecure on any system except linux
    load_root_certificates(ctx);
    try {
      load_system_certificates(ctx);
    } catch (const std::exception& e) {
    }

    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_peer);

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream.native_handle(),
                                  static_cast<void*>(&my_host[0]))) {
      beast::error_code ec{static_cast<int>(::ERR_get_error()),
                           net::error::get_ssl_category()};
      throw beast::system_error{ec};
    }

    // Look up the domain name
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(stream).connect(results);

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{verb, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // set the payload for post and other methods that allow a body
    if (!payload.empty() &&
        (verb == http::verb::post || verb == http::verb::put ||
         verb == http::verb::patch)) {
      req.body() = payload;
      req.prepare_payload();  // this sets the content-length and content-type
                              // headers
    }

    // Iterate over the headers map and add each header to the request
    for (const auto& header : headers) {
      req.set(header.first, header.second);
    }

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Gracefully close the stream
    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == net::error::eof) {
      // Rationale:
      // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
      ec = {};
    }
    // if(ec)
    // throw beast::system_error{ec}; // I get cryptography but someone needs to
    // tell OpenAI

    // If we get here then the connection is closed gracefully
    return beast::buffers_to_string(res.body().data());
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::stringstream ss;
    ss << "Error: " << e.what() << std::endl;
    return ss.str();
  }
}

boost::property_tree::ptree httpsRequestJson(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    const http::verb& verb,
    const std::string& body,
    const std::map<std::string, std::string>& headers = {},
    int version = 11) {
  std::string response;
  try {
    // Reuse the http_get function to perform the HTTP GET request
    response = httpsRequest(host, port, target, verb, body, headers, version);

    // Parse the JSON string into a property tree
    std::stringstream ss;
    ss << response;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    return pt;
  } catch (std::exception const& e) {
    std::cerr << "HTTPS reponse:\n" << response << std::endl;
    // Handle parsing errors or other exceptions
    boost::property_tree::ptree error_ptree;
    error_ptree.put("error", e.what());
    return error_ptree;
  }
}

}  // namespace Prong

#endif
