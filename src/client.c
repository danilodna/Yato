#include "../include/client.h"

int main(int argc, char** argv) {
	verify_args_client(argc, argv);

	// THREAD VARIABLES =============
	pthread_t t_recv, t_send;
	// ==============================

	// CLIENT VARIABLES =============
	int socket_fd;	// Socket File Descriptors to be used in the process conection
	struct sockaddr_in server_addr;
	struct hostent* server = gethostbyname(argv[1]);
	const int port_number = atoi(argv[2]); // Stores the port number on which the server accepts connections.
	// ==============================
	if (server == NULL)
  	error("ERROR, no such host\n");

	setup_client(port_number, &socket_fd, &server_addr, server);

	// Estabelishing connection with server
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  	error("ERROR connecting\n");

	// Creating thread to receive data from server.
	if(pthread_create(&t_recv, NULL, recv_func, &(socket_fd)) != 0){
		printf("Failure creating thread to receive data. Excecution terminated.");
		exit(EXIT_FAILURE);
	}

	// Creating thread to data data to the server.
	if(pthread_create(&t_send, NULL, send_func, &(socket_fd)) != 0){
		printf("Failure creating thread to send data. Excecution terminated.");
		exit(EXIT_FAILURE);
	}

	pthread_join(t_recv, NULL);
	pthread_join(t_send, NULL);

	// Extremely important to close the stream
	close(socket_fd);
	return 0;
}

// ==================================================================

void* recv_func(void* arg){
	int desc = *((int *) arg);
	int n;
	char buffer[MSG_SIZE]; // The server reads characters from the socket connection into this buffer.

	while(1){
		memset((char *) buffer, 0, sizeof(buffer));
		n = read(desc, buffer, sizeof(buffer));
		check_socket_io(n);

		if(buffer[0] == '-')
			pthread_exit(NULL);
		else
			fputs(buffer, stdout);
	}

	return NULL;
}

void* send_func(void* arg){
	int desc = *((int *) arg);
	int n;
	char buffer[MSG_SIZE];
	while(1){
		memset((void *) buffer, 0, (size_t) sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		n = write(desc, &buffer, sizeof(buffer));
		check_socket_io(n);
	}

	return NULL;
}

void setup_client(const int port_number, int* socket_fd, struct sockaddr_in* server_addr, struct hostent* server){
	// AF => Adress Family; INET => InterNET
	// SOCK_STREAM => TCP or SOCK_DGRAM => UDP
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socket_fd < 0)
		error("ERROR opening socket\n");

	memset((void *) server_addr, 0, sizeof(*server_addr));
	server_addr->sin_family = AF_INET;
	memmove((void *) &server_addr->sin_addr.s_addr, (const void *) server->h_addr, server->h_length);
	server_addr->sin_port = htons(port_number);

	/*
	struct hostent {
  	char    *h_name;									Official name of host
  	char    **h_aliases;							Alias list
  	int     h_addrtype;								Host address type
  	int     h_length;									Length of address
  	char    **h_addr_list;						List of addresses from name server
  	#define h_addr  h_addr_list[0]		Address, for backward compatiblity

	};
		h_name       Official name of the host.
		h_aliases    A zero  terminated  array  of  alternate names for the host.
		h_addrtype   The  type  of  address  being  returned; currently always AF_INET.
		h_length     The length, in bytes, of the address.
		h_addr_list  A pointer to a list of network addresses for the named host. Host addresses are returned in network byte order.
	*/
}
