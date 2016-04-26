#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1 };

int main(int argc, char* argv[])
{
	try
	{
		int num = std::atoi(argv[1]);
		boost::asio::io_service io_service;

		tcp::socket s(io_service);
		tcp::resolver resolver(io_service);
		//boost::asio::connect(s, resolver.resolve({ "127.0.0.1", "8090" }));
		boost::asio::connect(s, resolver.resolve({ "192.168.2.183", "8090" }));

		char* request = new char[num];

		while (true)
		{
			boost::asio::write(s, boost::asio::buffer(request, num));
		}

		delete request;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
