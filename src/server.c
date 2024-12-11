#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const char *HOST = "0.0.0.0";
const int PORT = 12345;

int make_server(int domain, const char *host, int port);

int main()
{
	int server_fd = make_server(AF_INET, HOST, PORT);
	if (server_fd < 0) {
		return 1;
	}

	close(server_fd);
	return 0;
}

int make_server(int protocol, const char *host, int port)
{
	int server_fd = socket(protocol, SOCK_STREAM, 0);
	if (server_fd < 0) {
		fprintf(stderr, "ERROR: socket: %s\n", strerror(errno));
		return -1;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt,
				   sizeof(opt)) < 0) {
		fprintf(stderr, "ERROR: setsockopt: %s\n", strerror(errno));
		return -1;
	}

	if (protocol == AF_INET) {
		struct sockaddr_in address;
		memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htonl(port);
		if (inet_pton(protocol, host, &address.sin_addr) == 0) {
			fprintf(stderr, "ERROR: inet_pton: %s\n", strerror(errno));
			return -1;
		}

		if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
			fprintf(stderr, "ERROR: bind: %s\n", strerror(errno));
			return -1;
		}
	} else if (protocol == AF_INET6) {
		struct sockaddr_in6 address;
		memset(&address, 0, sizeof(address));
		address.sin6_family = AF_INET6;
		address.sin6_port = htonl(port);
		if (inet_pton(protocol, host, &address.sin6_addr) == 0) {
			fprintf(stderr, "ERROR: inet_pton: %s\n", strerror(errno));
			return -1;
		}

		if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
			fprintf(stderr, "ERROR: bind: %s\n", strerror(errno));
			return -1;
		}
	} else {
		fprintf(stderr, "ERROR: Unknown protocol: %d\n", protocol);
		return -1;
	}
	printf("Opened server at %s:%d\n", host, port);
	return server_fd;
}
