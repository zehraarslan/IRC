#include "../inc/server.hpp"
#include <cerrno>
#include <errno.h>
#include "../inc/numeric.hpp"
#include <stdlib.h>



template <typename T>
std::string IntToString(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

struct PollFdFinder
{
    int clientSocket;
    PollFdFinder(int socket) : clientSocket(socket) {}

    bool operator()(const struct pollfd &pollFd)
    {
        return pollFd.fd == clientSocket;
    }
};

Server::Server(int port,int password) : port(port)  , serverSocket(0) 
{
	this->Password = password;
}

void Server::polError(int pollSocket)
{
    if (pollSocket == -1)
    {
        std::cerr << "Error: Failed to poll sockets." << std::endl;
        exit(1);
    }
}

void Server::start()
{
    serverSocket = createSocket();
    bindSocket(serverSocket);
    listenForConnections(serverSocket);
    setHostName();
    std::cout << "Waiting for connections on port " << port << std::endl;

    setupPolling();

    while (true)
    {
        polError(pollSockets());
        handlePollEvents();
    }
}

void Server::setupPolling()
{
    pollFds.resize(1);
    pollFds[0].fd = serverSocket;
    pollFds[0].events = POLLIN;
}

int Server::pollSockets()
{
    return poll(&pollFds[0], pollFds.size(), -1);
}

void Server::handlePollEvents()
{
    int numReadySockets = pollSockets();

    if (numReadySockets == -1)
    {
        std::cerr << "Error: Failed to poll sockets." << std::endl;
        exit(1);
    }

    for (int i = 0; i <(int)pollFds.size(); i++)
    {
        if (numReadySockets <= 0)
            break;

        if (pollFds[i].revents & POLLIN)
        {
            if (pollFds[i].fd == serverSocket)
            {
                handleNewConnection(serverSocket);
            }
            else
            {
                handleClientSocket(pollFds[i].fd);
            }

            numReadySockets--;
        }
    }
}

int Server::createSocket()
{
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        std::cerr << "Error: Failed to create socket." << std::endl;
        exit(1);
    }
    return socketFd;
}

void Server::bindSocket(int socket)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(socket, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error: Failed to bind socket." << std::endl;
        exit(1);
    }
}

void Server::listenForConnections(int socket)
{
    if (listen(socket, SOMAXCONN) == -1)
    {
        std::cerr << "Error: Failed to listen on socket." << std::endl;
        exit(1);
    }
}

void Server::handleNewConnection(int serverSocket)
{
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressSize);

    if (clientSocket == -1)
    {
		std::cerr << "Error: Failed to accept connection." << std::endl;
        return;
    }

    std::string clientAddressStr = getClientAddressString(clientAddress);
    int clientPort = getClientPort(clientAddress);
    std::cout << "Connected to client: " << clientAddressStr << " (Port: " << clientPort << ")" << std::endl;

    addClient(clientSocket, clientAddressStr);
    addClientToPolling(clientSocket);
}

std::string Server::getClientAddressString(const sockaddr_in &clientAddress)
{
    return inet_ntoa(clientAddress.sin_addr);
}

int Server::getClientPort(const sockaddr_in &clientAddress)
{
    return ntohs(clientAddress.sin_port);
}

void Server::addClient(int clientSocket, const std::string &clientAddress)
{
    clients.insert(std::make_pair(clientSocket, clientAddress));
}

void Server::addClientToPolling(int clientSocket)
{
    struct pollfd newClientPollFd;
    newClientPollFd.fd = clientSocket;
    newClientPollFd.events = POLLIN;
    std::string undefined = std::string("undefined") +  IntToString(newClientPollFd.fd);
    users.createUser(newClientPollFd.fd,undefined,undefined,undefined,undefined);
	pollFds.push_back(newClientPollFd);
}

void Server::handleClientSocket(int clientSocket)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int numBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (numBytes <= 0)
    {
        closeClientSocket(clientSocket);
        return;
    }

    std::string message(buffer);

    // Tam bir satır alınana kadar devam edin
    while (message.find("\n") == std::string::npos )
    {
        memset(buffer, 0, sizeof(buffer));
        numBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (numBytes <= 0 )
        {
            closeClientSocket(clientSocket);
            return;
        }
        message += buffer;
    }

    message = trim(message);
    if(message.length() == 0)
    {
        std::cout << " ERROR:" << " Message lenght" <<  message.length() << "\n";
        return ;
    }
    printReceivedMessage(clientSocket, message);
}

void Server::printReceivedMessage(int clientSocket, const std::string &message)
{
    std::cout << "Received message from client " << clientSocket << ": " << message << std::endl;
    std::string capLs = getMessage(message,*this,clientSocket) = message;
    if(capLs.size() > 5)
    {
        sender(clientSocket, capLs.c_str());
    }
}


void Server::closeClientSocket(int clientSocket)
{
    std::cout << "Client " << clientSocket << " disconnected." << std::endl;	
    close(clientSocket);

    std::map<int, std::string>::iterator it = clients.find(clientSocket);
    if (it != clients.end())
    {
        clients.erase(it);
    }
    std::vector<struct pollfd>::iterator pollIt = std::find_if(pollFds.begin(), pollFds.end(), PollFdFinder(clientSocket));
    if (pollIt != pollFds.end())
    {
        pollFds.erase(pollIt);
    }
}

void Server::setHostName()
{
    char hostname[2048];
    int error = gethostname(hostname, 2048);
    if(error != 0)
    {
        std::cerr << "Error: Failed to get hostname."<< std::strerror(errno) << std::endl;
        exit(1);
    }
    hostName = hostname;
}

//*********************************************
void Server::channelChangeUserInfoPush(int fd, std::string channelName)
{
    std::string all_users = "";
    int own_fd = this->channels.getChannelOwner(channelName);
    all_users += this->users.getNickname(own_fd) + " ";
	std::vector<int> channelUsers = this->channels.getChannelUsers(channelName);
    for (int i = 0; i < (int)channelUsers.size(); i++)
    {
        if (own_fd != channelUsers[i])
            all_users += this->users.getNickname(channelUsers[i]) + " ";
    }
	all_users = trim(all_users);
    for (int i = 0; i < (int)channelUsers.size(); i++)
    {
        fd = channelUsers[i];
        std::string _353 = RPL_NAMEREPLY( this->users.getHostname(fd), this->users.getNickname(fd), this->users.getNickname(fd), "#" + channelName, "@" + all_users) + "\r\n";
        std::string _356 = RPL_ENDOFNAMES(this->users.getHostname(fd), this->users.getNickname(fd), this->users.getNickname(fd), "#" + channelName) + "\r\n";
        sender(fd, _353.c_str());
        sender(fd, _356.c_str());
    }
}

void Server::channelAllChangeUserInfoPush(int fd)
{
	std::vector<std::string> userChannels = this->channels.findChannelsByUser(fd);

	for (size_t i = 0; i < userChannels.size(); i++)
	{
		channelChangeUserInfoPush(fd, userChannels[i]);
		std::cout << "channel name" << userChannels[i] << " ";
	}	
}

void Server::channelChangeUserInfoPush(int fd, std::string channelName, std::string message)
{
    std::string all_users = "";
    int own_fd = this->channels.getChannelOwner(channelName);
    all_users += this->users.getNickname(own_fd) + " ";
	std::vector<int> channelUsers = this->channels.getChannelUsers(channelName);

    for (int i = 0; i < (int)channelUsers.size(); i++)
    {
        fd = channelUsers[i];
        sender(fd, message);
    }
}

void Server::channelAllChangeUserInfoPush(int fd, std::string message)
{
	std::vector<std::string> userChannels = this->channels.findChannelsByUser(fd);

	for (size_t i = 0; i < userChannels.size(); i++)
	{
		channelChangeUserInfoPush(fd, userChannels[i], message);
		std::cout << "channel name" << userChannels[i] << " ";
	}
}

void Server::quitAll(int fd)
{
	std::vector<std::string> userChannels = this->channels.findChannelsByUser(fd);

	for (int i = 0; i < (int)userChannels.size() ; i++)
	{
		std::vector<int> users = this->channels.getChannelUsers(userChannels[i]);
		if (userChannels[i].size() == 1)
		{
			this->channels.removeChannelAllInfo(userChannels[i]);
			std::string server_msg = this->users.getPrefix(fd) + " PART #" + userChannels[i] + " " + "\r\n";
            sender(fd, server_msg);
			continue;
		}
		if (this->channels.getChannelOwner(userChannels[i]) == fd)
		{
			int temp_fd = users[fd == users[0]];
			this->channels.updateChannelOwner(userChannels[i] , users[fd == users[0]]);
			this->concatenateStrings(-1,users,(this->users.getPrefix(fd) + " MODE #" + userChannels[i] + " +o " + this->users.getNickname(temp_fd) + "\r\n").c_str(),NULL);	
		}

		this->channels.removeChannelUser(userChannels[i], fd);
		this->concatenateStrings(-1,users,(this->users.getPrefix(fd) + " PART #" + userChannels[i] + " " + " " + "\r\n").c_str(),NULL);	
		this->channelChangeUserInfoPush(fd, userChannels[i]);
	}
	this->users.removeUserAllInfo(fd);
}

void Server::concatenateStrings(int fd ,std::vector<int> users,const char* arg1, ...) {
    std::string result;
    va_list args;
    va_start(args, arg1);

    const char* currentArg = arg1;
    while (currentArg != NULL) {
        result += currentArg;
        currentArg = va_arg(args, const char*);
    }

    va_end(args);
	for (int i = 0; i < (int)users.size(); i++)
	{ 
		if(fd == -1)
			sender(users[i], result);
	}
}
