#pragma once
#include <string>
#include <map>
#include <functional>
#include "reply.hpp"
#include "http_request.hpp"

class reply;
struct http_request;

typedef std::function<void(const http_request& req, reply& rep)> request_handler;

class dispatcher
{
public:
	dispatcher() {}
	dispatcher(const dispatcher&) = delete;
	dispatcher& operator=(const dispatcher&) = delete;

	void register_handler(const std::string& uri, const request_handler& handler)
	{
		handlers_.emplace(uri, handler);
	}

	void dispatch(const http_request& req, reply& rep)
	{
		/* get rid of the part being after the question mark */
		size_t pos = req.uri.find_last_of('?');
		std::string uri_processed;

		if (pos != std::string::npos)
		{
			uri_processed = std::string(req.uri.begin(), req.uri.begin() + pos);
		}
		else 
		{
			uri_processed = req.uri;
		}

		auto it = handlers_.find(uri_processed);
		if (it == handlers_.end()) 
		{
			rep = reply::stock_reply(status_type::not_found);
			return;
		}

		(it->second)(req, rep);
	}

private:
	/// The handlers for all incoming requests.
	std::map<std::string, request_handler> handlers_;
};
