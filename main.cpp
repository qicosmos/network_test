#include <iostream>
#include <thread>
#include <chrono>
#include "basic_server.hpp"

int main()
{
	try
	{
		boost::asio::io_service io_service;

		basic_server<1> s(io_service, 8090);

		std::thread t([&io_service] {io_service.run(); });
		t.detach();

		getchar();

		std::uint64_t last_succeed_count = 0;

		while (true)
		{
			auto curr_succeed_count = (std::uint64_t)g_succeed_count;
			std::cout << curr_succeed_count - last_succeed_count << std::endl;
			last_succeed_count = curr_succeed_count;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}