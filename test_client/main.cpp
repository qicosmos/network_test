#include <iostream>
#include <boost/asio.hpp>
#include "basic_client.hpp"

using boost::asio::ip::tcp;

enum { max_length = 1 };

void test_client()
{
	try
	{
		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve({ "192.168.2.183", "8090" });
		chat_client c(io_service, endpoint_iterator);
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

int main(int argc, char* argv[])
{
	test_client();

	return 0;
}
