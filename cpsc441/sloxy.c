/*
 *  CPSC 441 Assignment 1: Slow network proxy
 *  Created on: Jan. 26, 2018
 *  Created by: Duncan Lam
 *  
 *  Based on proxy.c provided in Tutorial
 *  Last edited: Jan. 26, 2018
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

int lstn_sock;

//Catches ctrl+c in terminal to cleanly close program
void catcher(int sig){
  close(lstn_sock);
  exit(0);
}

int main(){
  signal(SIGINT, catcher);
  
  //First, receive request from Web Browser
  //Initialize server address
  struct sockaddr_in addr_proxy;
  int serv_port = 6969;
  memset(&addr_proxy, 0, sizeof(addr_proxy));
  addr_proxy.sin_family = AF_INET;
  addr_proxy.sin_port = htons(serv_port);
  addr_proxy.sin_addr.s_addr = htonl(INADDR_ANY);

  //Create server listening socket
  lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (lstn_sock < 0) {
    printf("Error in socket() while creating lstn_sock.\n");
    exit(-1);
  }

  //Bind the socket to address and port
  int bind_status;
  bind_status = bind(lstn_sock, (struct sockaddr *) &addr_proxy, sizeof(struct sockaddr_in)); 
  if (bind_status < 0) {
    printf("Error while binding socket.\n");
    exit(-1);
  }

  //Listen on bound port number
  int lstn_status;
  lstn_status = listen(lstn_sock, 5);
  if (lstn_status < 0){
    printf("Error while listening.\n");
    exit(-1);
  }

  while(1){
    //Accept incoming browser connection requests
    int data_sock;
    data_sock = accept(lstn_sock, NULL, NULL);

    //Check successful connection
    if (data_sock < 0){
      printf("Error while accepting connection request.\n");
      exit(-1);
    }
    else{
      printf("\nConnection accepted from client.\n");
    }

    //Recieve HTTP message from web client
    char c_message_in[1024];
    char s_message_out[1024];
    int c_recv_status;
    c_recv_status = recv(data_sock, c_message_in, sizeof(c_message_in), 0);

    //Check successful receive
    if (c_recv_status < 0){
      printf("Error while receiving message.\n");
      exit(-1);
    }
    else{
      printf("Message received from client.\n");
    }

    //Preserve HTTP request
    strcpy(s_message_out, c_message_in);

    //Parse HTTP message
    char host[1024];
    char URL[1024];
    char PATH[1024];
    int i;

    //Find and parse GET request, retaining URL for later
    char *pathname = strtok(c_message_in, "\r\n");
    printf("Parsing HTTP request: %s\n", pathname);
    if (sscanf(pathname, "GET http://%s", URL) == 1){
      printf("From URL: %s\n\n", URL);
    }

    //Separate hostname from pathname and retain hostname
    for (i = 0; i < strlen(URL); i++){
      if (URL[i] == '/'){
	strncpy(host, URL, i);
	host[i] = '\0';
	break;
      }
    }

    //Retain pathname
    for(; i < strlen(URL); i++){
      strcat(PATH, &URL[i]);
      break;
    }

    //Echo host and path
    printf("Connection successful to:\n");
    printf("Host: %s\n", host);
    printf("Path: %s\n...\n", PATH);

    //Determine if destination is an html file
    bool is_html = false;
    if(strstr(PATH, ".html") != NULL){
      is_html = true;
      printf("!!! HTML file detected !!!\n\n");
    }

    //Create TCP socket for connecting to web server
    struct sockaddr_in addr_server;
    struct hostent *server;

    //Get web server's Address by host name
    server = gethostbyname(host);
    if (server == NULL){
      printf("Error in getting host by name from Web Server.\n");
    }
    else{
      printf("Web server = %s\n", server->h_name);
    }

    //Initialize socket for web server
    memset(&addr_server, 0, sizeof(addr_server));
    memset(&addr_server.sin_addr.s_addr, server->h_addr, server->h_length);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(80);

    //Create socket to connect to web server
    int web_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (web_sock < 0){
      printf("Error in creating web server socket.\n");
    }
    else{
      printf("Web server socket creation successful.\n");
    }

    //Connect to the web server's socket
    int connect_status;
    connect_status = connect(web_set, (struct sockaddr*) &addr_server, size
    
  }
  
  close(lstn_sock);
  return 0;
}

