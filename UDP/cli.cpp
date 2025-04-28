// Client side implementation of UDP client-server model 

#include <cstdlib>
#include <iostream>
#include <string>

#include <netdb.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 

using namespace std;

#define MAXLINE 1024 

#define USE_ERR 1
#define HOST_ERR 2
#define SOCK_ERR 3
#define BIND_ERR 4

// Driver code 
int main(int argc, char *argv[] )
{
  if (argc!=3)
    {
      cerr <<"USAGE: " << argv[0] << " <server-address> <port#>" << endl;
      return USE_ERR; 
    }

  // Create socket
  int sockfd; 
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
  { 
    cerr << "socket creation failed" << endl; 
    return SOCK_ERR; 
  } 

  // Fill in  server information 
  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;            // using IPv4
  servaddr.sin_port = htons(stoi(argv[2])); // specify port

  // set server addess.
  struct addrinfo *result;
  getaddrinfo(argv[1], NULL, NULL, &result); //DNS lookup
  if (!result)
    {
      cerr << "DNS lookup failed for:" << argv[1] << endl;
      return HOST_ERR; 
    }

  servaddr.sin_addr = ((struct sockaddr_in *) result->ai_addr)->sin_addr;

  // send message to server
  string msgForServer = "some message";
  sendto(sockfd, msgForServer.c_str(), msgForServer.length(), 
	 0, (const struct sockaddr *) &servaddr, 
	 sizeof(servaddr));
  
  //read response from server
  int n;                  // number of characters read
  socklen_t len;          // size of sender address
  char buffer[MAXLINE+1]; // holds response

  // do actual read
  n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
	       0, (struct sockaddr *) &servaddr, 
	       &len);

  // null terminate response from server
  buffer[n] = '\0';
  
  cout << "Server responded with:" << buffer << endl; 
  
  close(sockfd); //always a good idea to close socket when done with it. 
  
  return 0; 
}

