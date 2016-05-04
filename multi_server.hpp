#pragma once
#include "connection.hpp"
#include "io_service_pool.hpp"

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
		io_service_pool_.run();
		//std::thread t([this] {io_service_pool_.run(); });
		//t.detach();
	}

private:
	void do_accept()
	{
		conn_.reset(new connection(io_service_pool_.get_io_service()));
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
	std::shared_ptr<connection> conn_;
};

