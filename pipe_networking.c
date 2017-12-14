#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Perofrms the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  char privateFIFO_name[1024];
  char client_conf[1024];

  //initially remove it in case it already exists
  remove("wkp");

  //creates well known pipe (upstream: client -> server)
  mkfifo("wkp", 0666);

  //server receives client’s message and removes wkp
  int up = open("wkp", O_RDONLY);
  read(up, privateFIFO_name, sizeof(privateFIFO_name));
  printf("server recieved this message from the client: %s\n", privateFIFO_name);
  remove("wkp");

  //server opens private FIFO (downstream: server -> client) and sends client a confirmation message
  int down = open(privateFIFO_name, O_WRONLY);
  write(down, "client messaged recieved by server", sizeof("client messaged recieved by server"));

  //server recieves final confirmation from the client
  read(up, client_conf, sizeof(client_conf));
  printf("last message from client to server: %s\n", client_conf);
  
  *to_client = down;
  return up;
}


/*=========================
  client_handshake
  args: int * to_server

  Perofrms the server side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char server_conf[1024];

  //initially remove it in case it already exists
  remove("private");

  //creates private FIFO
  mkfifo("private", 0666);

  //client connects to WKP (upsteam: client -> server)
  int up = open("wkp", O_WRONLY);

  //client sends the private FIFO name
  write(up, "private", sizeof("private"));

  //client opens private FIFO (downstream: client -> server)
  int down = open("private", O_RDONLY);

  //recieves server's confirmation and removes private FIFO
  read(down, server_conf, sizeof(server_conf));
  printf("client recieved this message from the server: %s\n", server_conf);
  remove("private");

  //client sends a confirmation message to server
  write(up, "server message recieved by client", sizeof("server message recieved by client"));

  *to_server = up;
  return down;
}
