// server.c - prints first message from client then exits

#include <sys/socket.h> // socket
#include <sys/types.h>  // socket
#include <netinet/in.h> // htons
#include <arpa/inet.h>  // inet_pton
#include <stdio.h>      // fprintf
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <unistd.h>     // write
#include <pthread.h>    // pthread_create

#define DOMAIN AF_INET6
#define PORT   htons(0xC271)
#define SIZE   1024
#define TRUE 1
#define FALSE 0

#define Snak '+'
    
int SnakNum, Snakx, Snaky;
typedef struct link {
	int x ,y ;
	struct link *next ;
} *link_P;

link_P append (link_P head, int x, int y ){
	link_P link_new = malloc(sizeof(struct link));
	link_new -> x = x ;
	link_new -> y = y ;
	link_new -> next = head ;
	return link_new ;
}

void pop (link_P head){
	link_P current = head ;
	link_P prev = NULL ;

	while(current -> next != NULL){
		prev = current ;
		current = current -> next ;

	}
	prev -> next = NULL ;
	free(current) ;

}

typedef struct {
    int conx;
    char* buff;
} client_data;

void *read_client_message(void *arg)
{
    client_data* data = (client_data*)arg;
    int conx = data->conx;
    char* buff = data->buff;
    //memset(buff, 0, SIZE);

    while (!sleep(1))
    {
        read(conx, buff, SIZE);
        
        
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    int sock = socket(DOMAIN, SOCK_STREAM, 0), opt = 1, conx ;
    socklen_t s = sizeof(struct sockaddr_in6) ;
    struct sockaddr_in6 addr ;
    char buff[SIZE] ;
    
    addr.sin6_family = DOMAIN ;
    addr.sin6_port   = PORT   ;
    addr.sin6_addr   = in6addr_any ;
    memset(buff, 0, SIZE) ;
    
    // make testing easier
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Server - setsockopt addr failed.\n") ;
        exit(-1) ;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Server - setsockopt port failed.\n") ;
        exit(-1) ;
    }
    // end testing ease
    
    if (bind(sock, (struct sockaddr *)&addr, s))
    {
        perror("Server - bindin failed.\n") ;
        exit(-1) ;
    }
    if (listen(sock, 1))
    {    
        perror("Server - listen failed.\n") ;
        exit(-1) ;
    }
    conx = accept(sock, (struct sockaddr *)&addr, &s) ;
    if (conx == -1)
    {
        perror("Server - accept failed.\n") ;
        exit(-1) ;
    }

    client_data data;
    data.conx = conx;
    data.buff = buff;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, read_client_message, &data) != 0)
    {
        perror("Server - thread creation failed.\n");
        exit(-1);
    }

    while (TRUE){        
            
            printf("%s\n", buff) ;
            sleep(1) ;
        
            
    
    }
    

}
