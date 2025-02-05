#include "Server.h"

int main()
{
	Server server;
	server.Init();
	server.Launch("127.0.0.1", 8888);
	server.Run();
	return 0;
}