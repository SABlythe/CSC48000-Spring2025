// Server side implementation of UDP client-server model 
#include <cstdlib> 
#include <iostream>

#include <unistd.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 

using namespace std;

#define MAXLINE 1024 

#define USE_ERR 1
#define SOCK_ERR 2
#define BIND_ERR 3

int main(int argc, char *argv[])
{ 
  if (argc!=2)
    {
      cerr << "USAGE: " << argv[0] << " <port#>" << endl;
      return USE_ERR;
    }
  

  struct sockaddr_in servaddr, cliaddr; 
  
  // Create socket file descriptor 
  int sockfd;
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
      cerr << "socket creation failed" << endl;
      return SOCK_ERR;
    } 
	
  // Filling server information 
  servaddr.sin_family = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = INADDR_ANY; 
  servaddr.sin_port = htons(stoi(argv[1])); 
  
  // Bind the socket with the server address 
  //   note that the leading :: in front of the bind call orevents some bind() conflicts with a newer
  //   std::bind and the "global" bind function(s).
  if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) <0) 
    { 
      cerr << "bind failed" << endl; 
      return BIND_ERR; 
    } 
	
  int clientCount=1; // server responds with the cilent number, initially 1.

  //server ... so keep processing client requests until ... forever !!
  while(true)
    {
      // read whatever comes in on socket from a client. 
      char buffer[MAXLINE+1];
      socklen_t len; //length of client address
      int n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
		       0, ( struct sockaddr *) &cliaddr, 
		       &len);
      
      buffer[n] = '\0'; // terminate received buffer as if it is a C string

      cout << "Client (" << inet_ntoa(cliaddr.sin_addr)
	   << ") sent:" << buffer << endl;

      // build and return reply for client
      string reply = "You were client #" + to_string(clientCount++);  
      sendto(sockfd, reply.c_str(), reply.length(), 
	     0, (const struct sockaddr *) &cliaddr, 
	     len); 
    }	
  return 0; 
}

