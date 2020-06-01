#pragma once
#include "intermediary.h"

#define NETWORK_ERROR(err) out("NETWORK ERROR: " << err);

typedef struct Server;
typedef struct Client;

struct Network_Message
{
	int size;
	char* data;
};

struct Server_Connection
{
	uint status;

	char ip[32];
	SOCKET connect_socket;
};

struct Server
{
	char name[64];

	uint num_clients, max_clients;
	Server_Connection clients[8];

	SOCKET listen_socket;
};

// prepares the server for recieving connections
void init_server(Server* server, const char* ip, const char* port, int max_clients = 8)
{
	WSADATA wsa_data;
	addrinfo* result = NULL;

	// Initialize Winsock
	int res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res != 0)
	{
		NETWORK_ERROR("WSAStartup failed | Error code: " << res);
		return;
	}

	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resoooolve server address and port
	if ((res = getaddrinfo(NULL, port, &hints, &result)) != 0)
	{
		NETWORK_ERROR("getaddrinfo failed | Error code: " << WSAGetLastError());
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to the server
	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		NETWORK_ERROR("socket creation failed | Error code: " << WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	res = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		NETWORK_ERROR("bind() failed | Error code : " << WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listen_socket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	res = listen(listen_socket, max_clients);
	if (res == SOCKET_ERROR)
	{
		NETWORK_ERROR("listen() failed | Error code: " << WSAGetLastError());
		closesocket(listen_socket);
		WSACleanup();
		return;
	}

	// FIONBIO sets blocking mode for a socket. mode = 0 for blocking, mode != 0 for non-blocking
	u_long blocking_mode = 1; // non-blocking
	Sleep(1);
	//res = ioctlsocket(listen_socket, FIONBIO, &blocking_mode);

	*server = {};
	server->max_clients = max_clients;
	server->listen_socket = listen_socket;
}

// accepts connection requests & removes disconnected clients
int server_update_connections(Server* server)
{
	SOCKET test_socket = INVALID_SOCKET;

	for (uint i = 0; i < server->max_clients; ++i)
	{
		if (server->clients[i].status != 0) continue;

		test_socket = accept(server->listen_socket, NULL, NULL);
		if (test_socket == INVALID_SOCKET) continue;

		//u_long mode = 1;
		//ioctlsocket(test_socket, FIONBIO, &mode);

		server->clients[i].status = 1;
		server->clients[i].connect_socket = test_socket;
		server->num_clients += 1;

		out("new connection established! " << i); return 1;
	}

	return server->num_clients;
}

Network_Message server_recieve(Server server, uint id, uint max_size = 256)
{
	Network_Message ret = {};
	ret.data = (char*)malloc(max_size);
	ret.size = recv(server.clients[id].connect_socket, ret.data, max_size, 0);

	return ret;
}

//sends msg to the client at clients[id]
int server_send(Server server, uint client_id, char* msg, uint size)
{
	return send(server.clients[client_id].connect_socket, msg, size, 0);
}

struct Client
{
	SOCKET connect_socket;
};

// connects to a server at the given ip, if succsesful sends client info
void init_client(Client* client, const char* ip, const char* port)
{
	int res = 0;
	addrinfo* result = NULL;

	print("Connecting to %s - ", ip);

	// Initialize Winsock
	WSADATA wsa_data = {};
	res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res != 0)
	{
		NETWORK_ERROR("WSAStartup failed | Error code: " << res);
		return;
	}

	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resoooolve server address and port
	if ((res = getaddrinfo(ip, port, &hints, &result)) != 0)
	{
		NETWORK_ERROR("getaddrinfo failed | Error code: " << WSAGetLastError());
		WSACleanup();
	}

	SOCKET connect_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		NETWORK_ERROR("socket creation failed | Error code: " << WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	// Connect to server.
	res = connect(connect_socket, result->ai_addr, (int)result->ai_addrlen);
	if (res == SOCKET_ERROR || connect_socket == INVALID_SOCKET)
	{
		closesocket(connect_socket);
		NETWORK_ERROR("could not connect to " << ip);
		return;
	} else out("connected successfuly!");

	freeaddrinfo(result);

	*client = {};
	client->connect_socket = connect_socket;
}

Network_Message client_receive(Client client, uint max_size = 256)
{
	Network_Message ret = {};
	ret.data = (char*)malloc(max_size);
	ret.size = recv(client.connect_socket, ret.data, max_size, 0);

	return ret;
}

int client_send(Client client, char* msg, uint size)
{
	return send(client.connect_socket, msg, size, 0);
}

int test_client()
{
	char client_msg[256] = {};


	Client client = {};
	init_client(&client, "192.168.1.3", "42069");
	std::cout << client_receive(client).data;
	//client_send(client, client_msg, sizeof(client_msg));

	stop; return 0;
}
int test_server()
{
	char client_msg[] = "Hello Client!";

	Server server = {};
	init_server(&server, "192.168.1.3", "42069", 8);

	while (!server_update_connections(&server));

	stop;

	while (1)
	{
		for (int i = 0; i < server.max_clients; ++i)
		{
			if (server.clients[i].status != 0 && server.clients[i].connect_socket != INVALID_SOCKET)
			{
				//out("recieving from " << i);
				//Network_Message msg = server_recieve(server, i);
				//out("size = " << msg.size << '\n' << msg.data);

				std::cout << server_send(server, i, client_msg, sizeof(client_msg));
			}
		}

		Sleep(64);
	}

	stop; return 0;
}