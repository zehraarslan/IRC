#include "../inc/server.hpp"

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3) {
			std::cerr << "Usage: " << argv[0] << " <port>" << " <password>" << std::endl;
			return 1;
		}

		int i = 0;
		while(++i < 3)
		{
			int j = -1;
			while(argv[i][++j])
			{
				if (!isdigit(argv[i][j]))
				{
					std::cerr << "Usage: " << argv[0] << " <port>" << " <password> The password can only consist of numbers" << std::endl;
					return 1;
				}
			}
		}
		int port = atoi(argv[1]);
		if (port < 1024 || port > 65535)
		{
			std::cerr << "Port number must be between 1024 and 65535" << std::endl;
			return 1;
		}
		int password = atoi(argv[2]);
		if (password < 0 || password > 9999)
		{
			std::cerr << "Password must be between 0 and 9999" << std::endl;
			return 2;
		}
		Server server(port,password);
		server.start();		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
    return 0;
}
