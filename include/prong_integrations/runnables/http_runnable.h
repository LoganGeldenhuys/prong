#ifndef PRONG_HTTP_RUNNABLE_H
#define PRONG_HTTP_RUNNABLE_H

#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "../utils/simple_https_client.h"
#include "../utils/simple_http_client.h"
#include "../../prong/runnable.h"

namespace Prong {

	template <class Input, class Output>
    class HTTPRunnable: public Runnable<Input, Output> {
    protected:
		virtual std::string getBaseUrl_() const = 0; 
		virtual std::string getPort_() const = 0;
		virtual std::string getBasePath_() const = 0;
		virtual boost::beast::http::verb getHTTPVerb_() const = 0;
		virtual std::string getRequestBody_(const Input& input) const = 0;
		virtual std::map<std::string, std::string> getHeaders_() const = 0;
		virtual int getVersion_() const = 0;
		virtual Output parseResponse_(const boost::property_tree::ptree& pt) const = 0;

		//calls non-streaming methods by default, override if streaming behavior should be different
		virtual std::string getBaseUrl_Stream() const {
			return getBaseUrl_();
		}
		virtual std::string getPort_Stream() const {
			return getPort_(); 
		}
		virtual std::string getBasePath_Stream() const {
			return getBasePath_();
		}
		virtual boost::beast::http::verb getHTTPVerb_Stream() const {
			return getHTTPVerb_(); 
		}
		virtual std::string getRequestBody_Stream(const Input& input) const {
			return getRequestBody_(input); 
		}
		virtual std::map<std::string, std::string> getHeaders_Stream() const {
			return getHeaders_(); 
		}
		virtual int getVersion_Stream() const {
			return getVersion_(); 
		}

	public:
		//Simply prints the JSON, override if streaming behavior should be different
		virtual std::string parseResponse_Stream(const boost::property_tree::ptree& pt) const {
			std::stringstream ss;
			boost::property_tree::write_json(ss, pt, false); 
			return ss.str();
		}

		 // Implementing the call method
		virtual Output operator()(const Input& input) const override {
			// Use http_request_json to make an asynchronous request to the OpenAI API
			boost::property_tree::ptree responsePTree;

			if(getPort_()=="443"){
			responsePTree = httpsRequestJson(getBaseUrl_(), getPort_(), getBasePath_(), getHTTPVerb_(), getRequestBody_(input), getHeaders_(), getVersion_());
			}else{
			responsePTree = httpRequestJson(getBaseUrl_(), getPort_(), getBasePath_(), getHTTPVerb_(), getRequestBody_(input), getHeaders_(), getVersion_());
			}
			// Parse the response and construct a Output object with the assistant's reply
			try{
			return parseResponse_(responsePTree);
			} catch (const std::exception& er)	{
				std::cerr<<"Parser failed to parse response:"<<std::endl;
				boost::property_tree::write_json(std::cerr,responsePTree);
				std::cerr<<"\nWith error:\n"<<er.what()<<std::endl;
				throw er;
			}
		}

		 // Implementing the call method
		virtual Output operator()(std::ostream& os, const Input& input) const override {
			//TODO finish implementing and testing streaming

			// Use http_request_json to make an asynchronous request to the OpenAI API
			auto futureResponse = httpRequestJsonStream(os, getBaseUrl_Stream(), getPort_Stream(), getBasePath_Stream(), getHTTPVerb_Stream(), getRequestBody_Stream(input), getHeaders_Stream());
			
			//TODO add parsing callback to httpRequestJsonStream

			// Wait for the future to be ready and get the response
			auto responsePTree = futureResponse.get();

			// Parse the response and construct a Output object with the assistant's reply
			return Message("user","hey");
		}

    };

}

#endif // PRONG_HTTP_RUNNABLE_H
