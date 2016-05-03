#pragma once
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class chat_client
{
public:
	chat_client(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		: io_service_(io_service),
		socket_(io_service)
	{
		do_connect(endpoint_iterator);
	}

private:
	void do_connect(tcp::resolver::iterator endpoint_iterator)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
			[this](boost::system::error_code ec, tcp::resolver::iterator)
		{
			if (!ec)
			{
				do_read();
				do_write();
			}
		});
	}

	void do_read()
	{
		boost::asio::async_read(socket_,
			boost::asio::buffer(data_, max_length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
		{
			do_write();
		});
	}

	void do_write()
	{
		boost::asio::async_write(socket_,
			boost::asio::buffer(data_, max_length),
			[this](boost::system::error_code ec, std::size_t /*length*/)
		{
			do_read();
		});
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	enum { max_length = 1 };
	char data_[max_length];
};

