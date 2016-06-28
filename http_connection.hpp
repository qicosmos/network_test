#pragma once
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "http_parser.hpp"
#include "reply.hpp"
#include "dispatcher.hpp"

std::string str = "HTTP/1.0 200 OK\r\n"
"Content-Length: 4\r\n"
"Content-Type: text/html\r\n"
"Connection: Keep-Alive\r\n\r\n"
"TEST";
using boost::asio::ip::tcp;
namespace cinatra {
	class http_connection : public std::enable_shared_from_this<http_connection>, private boost::noncopyable
	{
	public:

		http_connection(boost::asio::io_service& io_service, dispatcher& dispatcher) : socket_(io_service), timer_(io_service), dispatcher_(dispatcher)
		{
		}

		~http_connection()
		{
		}

		void start()
		{
			do_read();
		}

		tcp::socket& socket()
		{
			return socket_;
		}
	private:
		void do_read()
		{
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(read_buffer_),
				[this, self](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					http_parser::result_type result;
					std::tie(result, std::ignore) = request_parser_.parse(request_, read_buffer_.begin(), read_buffer_.end());

					if (result == http_parser::good)
					{
						dispatcher_.dispatch(request_, reply_);
						do_write();
						request_parser_.reset();
						//do_read();
					}
					else if (result == http_parser::bad)
					{
						reply_ = reply::stock_reply(status_type::bad_request);
						do_write();
					}
					else
					{
						do_read();
					}
				}
				else if (ec != boost::asio::error::operation_aborted)
				{
					close();
					request_parser_.reset();
				}
			});
		}

		void handle_ready_to_go(const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			do_write();
		}

		void do_write()
		{
			boost::system::error_code ec;
			//boost::asio::write(socket_, boost::asio::buffer(str), ec);
			boost::asio::write(socket_, reply_.to_buffers(), ec);
			if (!ec)
			{
				g_succeed_count++;
				return;
			}

			//g_succeed_count++;
			//auto self(this->shared_from_this());
			//boost::asio::async_write(socket_, reply_.to_buffers(),
			//	[this, self](boost::system::error_code ec, std::size_t)
			//{
			//	if (!ec)
			//	{
			//		// Initiate graceful connection closure.
			//		//boost::system::error_code ignored_ec;
			//		//socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			//		
			//		return;
			//	}

			//	if (ec != boost::asio::error::operation_aborted)
			//	{
			//		close();
			//		//connection_manager_.stop(shared_from_this());
			//	}
			//});
		}

		void reset_timer()
		{
			if (timeout_milli_ == 0)
				return;

			auto self(this->shared_from_this());
			timer_.expires_from_now(boost::posix_time::milliseconds(timeout_milli_));
			timer_.async_wait([this, self](const boost::system::error_code& ec)
			{
				if (!socket_.is_open())
				{
					return;
				}

				if (ec)
				{
					//SPD_LOG_INFO(ec.message().c_str());
					return;
				}

				//SPD_LOG_INFO("connection timeout");

				close();
			});
		}

		void cancel_timer()
		{
			if (timeout_milli_ == 0)
				return;

			timer_.cancel();
		}

		void close()
		{
			boost::system::error_code ignored_ec;
			socket_.close(ignored_ec);
		}
	private:
		tcp::socket socket_;
		std::array<char, 8192> read_buffer_;
		boost::asio::deadline_timer timer_;
		std::size_t timeout_milli_;

		http_parser request_parser_;
		http_request request_;
		reply reply_;
		dispatcher& dispatcher_;
	};
}