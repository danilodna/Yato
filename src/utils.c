#include "../include/utils.h"

// Functions for receiving and sending data through network ================================
void send_card(const int32_t sock_fd, int32_t value) {
	int32_t n;
	n = write(sock_fd, &value, sizeof(int32_t));
	check_socket_io(n);
}

int32_t recv_card(const int32_t sock_fd) {
	int32_t n, ret_val;
	n = read(sock_fd, &ret_val, sizeof(int32_t));
	check_socket_io(n);

	//	Return the integer send by the server
	return ret_val;
}


// Verify args function ========================================

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_socket_io(const int n) {
	if(n < 0)
		error("ERROR reading or writing from socket");
}

void verify_args_client(int argc, char* argv[]) {
	if(argc != 3){
		printf("Wrong arguments, the correct form is:\n");
		printf("./server localhost port_number\n");
		exit(EXIT_FAILURE);
	}
}

void verify_args_server(int argc, char* argv[]) {
	if(argc != 2){
		printf("Wrong arguments, the correct form is:\n");
		printf("./server port_number\n");
		exit(EXIT_FAILURE);
	}
}
