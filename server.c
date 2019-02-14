#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>
#include <unistd.h>
#include <fcntl.h>

struct serve_thread_param {
	int client_sock;
};

static void serve_thread_fun(void *args);
static void daemonize();

int main(int argc , char *argv[])
{
	daemonize();

	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 6666 );

	//Bind
	if ( bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);


	while (1) {
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");

		struct serve_thread_param *param = (struct serve_thread_param*)malloc(sizeof(struct serve_thread_param));
		param->client_sock = client_sock;

		pthread_t tid = 0; 
		int rc = pthread_create(&tid, NULL, serve_thread_fun, param);

		if (rc < 0) {
			printf("Failed to create rev thread: %d\n", rc);
			return 1;
		}
	}

	return 0;
}

static void serve_thread_fun(void *args) {
	printf("start serving thread...\n");

	struct serve_thread_param *param = (struct serve_thread_param*)args;
	int client_sock = param->client_sock;
	free(param);

    int read_size;
	char client_message[2000];
	//Receive a message from client
	while ( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
	{
		//Send the message back to client
		printf("received: %s\n", client_message);
		write(client_sock , client_message , read_size);
	}

	if (read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if (read_size == -1)
	{
		perror("recv failed");
	}

	printf("serving thread ended\n");
}



static void daemonize()
{
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
         /* Log the failure */
        exit(EXIT_FAILURE);
    }


    pid = fork();

    if (pid < 0)   {
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
            /* Log the failure */
       exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Redirect to empty */
    int fd = open("/dev/null", O_RDWR);
    dup2(fd,STDIN_FILENO);
    dup2(fd,STDOUT_FILENO);
    dup2(fd,STDERR_FILENO);
}
