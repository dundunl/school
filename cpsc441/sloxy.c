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
  printf("listen socket closed.\n");
  exit(0);
}

//Converts integer to string
char* int2str(int i){
  
}

int main(){
  signal(SIGINT, catcher);
  signal(SIGSEGV, catcher);
  
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
      printf("-------------------------------------------");
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
    memset(PATH, '\0', sizeof(PATH));
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
    printf("Path: %s\n\n", PATH);

    //Determine if destination is an html file
    int is_html = 0;
    if(strstr(PATH, ".html") != NULL){
      is_html = 1;
      printf("!!! HTML file detected !!!\n\n");
    }
    
    //Create TCP socket for connecting to web server
    struct sockaddr_in addr_server;
    struct hostent *server;

    //Get web server's Address by host name
    server = gethostbyname(host);
    if (server == NULL){
      printf("Error in getting host by name from Web Server.\n");
      exit(-1);
    }
    else{
      printf("Web server = %s\n", server->h_name);
    }

    //Initialize socket for web server
    memset(&addr_server, 0, sizeof(addr_server));
    memcpy(&addr_server.sin_addr.s_addr, server->h_addr, server->h_length);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(80);
    
    //Determine size of web page (Content-Length)
    char *content_length;
    int content_len;
    int web_sock;
    int connect_status;
    
    if (is_html){
      char s_message_copy[1024];
      char s_message_probe[1024];
      char w_message_probe[10000];
      memset(s_message_probe, '\0', sizeof(s_message_probe));
      strcpy(s_message_copy, s_message_out);

      //Manually change GET to HEAD :^)
      s_message_probe[0] = 'H';
      s_message_copy[0] = 'E';
      s_message_copy[1] = 'A';
      s_message_copy[2] = 'D';
      strcat(s_message_probe, s_message_copy);

      //Create socket to connect to web server
      web_sock = socket(AF_INET, SOCK_STREAM, 0);
      if (web_sock < 0){
	printf("Error in creating web server socket.\n");
	exit(-1);
      }
      else{
	printf("Web server socket created.\n");
      }

      //Connect to the web server's socket
      connect_status = connect(web_sock, (struct sockaddr*) &addr_server, sizeof(addr_server));
      if(connect_status < 0){
	printf("Error in connecting to web server socket.\n");
	exit(-1);
      }
      else{
	printf("Established connection to web server.\n");
      }
    
      //Send HEAD request to web server
      int web_probe_status = send(web_sock, s_message_probe, sizeof(s_message_probe),0);
      if (web_probe_status < 0){
	printf("Error in HEAD request.\n");
	exit(-1);
      }
      else{
	printf("HEAD request successful\n");
      }

      //Receive HEAD response
      int web_probe_recv_status = recv(web_sock, w_message_probe, sizeof(w_message_probe), 0);
      if (web_probe_recv_status < 0){
      	printf("Error in recieving HEAD response.\n");
      	exit(-1);
      }
      else{
      	printf("HEAD response successful.\n");
      }

      //Parse out "Content-Length"
      content_length = strstr(w_message_probe, "Content-Length: ");
      content_length = strtok(content_length, "\r\n");
      sscanf(content_length, "Content-Length: %d", &content_len);
    }
    
    //Create socket to connect to web server
    web_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (web_sock < 0){
      printf("Error in creating web server socket.\n");
      exit(-1);
    }
    else{
      printf("Web server socket created.\n");
    }

    //Connect to the web server's socket
    connect_status = connect(web_sock, (struct sockaddr*) &addr_server, sizeof(addr_server));
    if(connect_status < 0){
      printf("Error in connecting to web server socket.\n");
      exit(-1);
    }
    else{
      printf("Established connection to web server.\n");
    }

    //Loop sending if html
    int send_range = 100;
    int low_bound = 0;
    int high_bound = 0 + send_range;
    char low_bnd_str[5];
    char high_bnd_str[5];
    char range_request[1024];
    char s_msg_out_rng[1024];
    do{
      strcpy(s_msg_out_rng, s_message_out);
      
      //Create Range Request header
      memset(range_request, '\0', sizeof(range_request));
      memset(low_bnd_str, '\0', sizeof(low_bnd_str));
      memset(high_bnd_str, '\0', sizeof(high_bnd_str));
      
      if(is_html){
	strcat(range_request, "\nRange: bytes=");
	strcat(range_request, itoa(low_bound));
	strcat(range_request, "-");
	if(high_bound > content_len){
	  strcat(range_request, itoa(content_len));
	}
	else{
	  strcat(range_request, itoa(high_bound));
	}

	//Append Range Request header to GET request
	strcat(s_msg_out_rng, range_request);
      }
      
      //Send HTTP request of the client to web server
      int web_send_status = send(web_sock, s_msg_out_rng, sizeof(s_msg_out_rng), 0);
      if (web_send_status < 0){
	printf("Error in sending HTTP request to web server.\n");
	exit(-1);
      }
      else{
	printf("HTTP request sent successfully to web server.\n");
	printf("\n\n%s\n\n",s_msg_out_rng);
      }

      //Recieve HTTP response from web server
      char w_message_in[10000];
      int web_recv_status = recv(web_sock, w_message_in, sizeof(w_message_in), 0);
      if (web_recv_status < 0){
	printf("Error receiving HTTP request from web server.\n");
	exit(-1);
      }
      else{
	printf("HTTP response received successfully from web server.\n");
	//printf("\n\n%s\n\n",w_message_in);
      }

      //Send data to client
      int c_send_status = send(data_sock, w_message_in, sizeof(w_message_in), 0);
      if (c_send_status < 0){
	printf("Error in sending data to client.\n");
	exit(-1);
      }
      else{
	printf("Data sent successfully to client\n");
      }

      low_bound += send_range;
      high_bound += send_range;
      if (low_bound > content_len){
	break;
      }
    }while(is_html)

    //Close socket connection with web server
    close(web_sock);
    //Close socket connection with client
    close(data_sock);
    printf("Transmission complete\n\n");
  }
  
  close(lstn_sock);
  return 0;
}

