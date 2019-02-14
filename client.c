/*
	C ECHO client example using sockets
*/
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include <time.h>

#define NANOSECONDS_PER_SECOND (1000 * 1000 * 1000)

long long get_time() {

  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  long long  timestamp_nsec =(int64_t)spec.tv_sec * (int64_t)NANOSECONDS_PER_SECOND + (int64_t)spec.tv_nsec;
  return timestamp_nsec;
}

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("10.10.35.49");
	server.sin_family = AF_INET;
	server.sin_port = htons( 6666 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");

	sprintf(message,"Hello message from client, balabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabalabala");
	
	//keep communicating with server
	while(1)
	{
		//printf("Enter message : ");
		//scanf("%s" , message);
		
		//Send some data
		long long timeStart = get_time();
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server

		size_t revSize = recv(sock , server_reply , 2000 , 0);

		long long timeEnd = get_time();

		if( revSize < 0) {
			puts("recv failed");
			break;
		}
		
		server_reply[revSize] = 0;
		printf("Server reply: %s\n time used: %f ms \n", server_reply, (double)(timeEnd - timeStart)/(double)10000000);

		usleep(10);
	}
	
	close(sock);
	return 0;
}