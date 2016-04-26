#pragma once
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

static std::uint64_t g_succeed_count = 0;

template<size_t N>
class session : public std::enable_shared_from_this<session<N>>
{
public:
	session(tcp::socket socket)
		: socket_(std::move(socket))
	{
	}

	void start()
	{
		do_read();
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length), boost::asio::transfer_at_least(N), [this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				//a successful request, can be used to statistic qps
				g_succeed_count++;
				do_read();
			}
			else
			{
				//log
				return;
			}
		});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				do_read();
			}
		});
	}

	tcp::socket socket_;
	enum { max_length = N };
	char data_[max_length];
};

class basic_server
{
public:
	basic_server(boost::asio::io_service& io_service, short port)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<session<1>>(std::move(socket_))->start();
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

