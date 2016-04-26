#include <iostream>
#include <thread>
#include <chrono>
#include "basic_server.hpp"

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		std::unique_ptr<base> ptr = nullptr;
		int num = std::atoi(argv[1]);
		if (num == 1)
			 ptr = std::make_unique<basic_server<1>>(io_service, 8090);
		else if(num == 2)
			ptr = std::make_unique<basic_server<2>>(io_service, 8090);
		else if (num == 3)
			ptr = std::make_unique<basic_server<4>>(io_service, 8090);
		else if (num == 4)
			ptr = std::make_unique<basic_server<8>>(io_service, 8090);
		else
			ptr = std::make_unique<basic_server<500>>(io_service, 8090);

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