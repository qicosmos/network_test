#pragma once
#include "connection.hpp"
#include "http_connection.hpp"
#include "io_service_pool.hpp"
using namespace cinatra;

class multi_server : private boost::noncopyable
{
public:

	multi_server(short port, size_t size) : io_service_pool_(size), 
		acceptor_(io_service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
	{
		do_accept();
	}

	void run()
	{
		thd_ = std::make_shared<std::thread>([this] {io_service_pool_.run(); });
		//io_service_pool_.run();
		//std::thread t([this] {io_service_pool_.run(); });
		//t.detach();
	}

	void register_handler(const std::string& uri, const request_handler& handler)
	{
		dispatcher_.register_handler(uri, handler);
	}

private:
	void do_accept()
	{
		conn_.reset(new http_connection(io_service_pool_.get_io_service(), dispatcher_));
		acceptor_.async_accept(conn_->socket(), [this](boost::system::error_code ec)
		{
			if (!ec)
			{
				conn_->start();
			}

			do_accept();
		});
	}

	io_service_pool io_service_pool_;
	tcp::acceptor acceptor_;
	std::shared_ptr<http_connection> conn_;
	std::shared_ptr<std::thread> thd_;

	dispatcher dispatcher_;
};

