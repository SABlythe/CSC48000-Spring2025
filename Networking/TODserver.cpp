#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include <ctime>
#include <iostream>
#include <string>

using namespace std;


// various error codes that correspond to socket problems
#define SUCCESS 0
#define USAGE_ERROR 1
#define SOCK_ERROR 2
#define BIND_ERROR 3
#define LISTEN_ERROR 4

#define MAX_WAITING 25

// forward definition of functions we will code. 
int doServer(int);
void doWork(int, struct sockaddr_in *);

int main(int argc, char *argv[])
{
  // check to make sure program has been invoked properly
  if (argc!=2)
    {
      cout << "Usage: " << argv[0] << " <port number>" << endl;
      exit (USAGE_ERROR);
    }

  return doServer(stoi(argv[1]));
}

int doServer(int onPort)
{
  // the listening socket
  int listen_sock=socket(AF_INET,SOCK_STREAM,0);

  //If the socket() function fails we exit
  if(listen_sock<0)
    {
      cout << "Could not create listening socket!" << endl;
      return SOCK_ERROR;
    }

  //A sockaddr_in structure specifies the address of the socket
  //for TCP/IP sockets.
  struct sockaddr_in local_addr; // server address in this context

  // Fill in local (server) half of socket info
  local_addr.sin_family=AF_INET;         //IPv4 Address family
  local_addr.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
  local_addr.sin_port=htons(onPort);    //port to use
  
  // bind (i.e. "fill in") our socket info to server socket
  if(bind(listen_sock,(sockaddr*)&local_addr,sizeof(local_addr))!=0)
    {
      cout << "Binding failed - this could be caused by:" << endl
           << "  * an invalid port (no access, or already in use?)" << endl
           << "  * an invalid local address (did you use the wildcard?)" 
           << endl;
      return BIND_ERROR;
    }

  //listen for a client connection on 
  if(listen(listen_sock,MAX_WAITING)!=0)
    {
      cout << "Listen error" << endl;
      return LISTEN_ERROR;
    }

    
  while(true)//keep handling connections forever
    {
      int connected_sock;      // socket for the actual connected client
      struct sockaddr_in from; // holds client address data
      unsigned int from_len;   // holds size of client address data
      
      // how big is the connected socket structure? 
      from_len=sizeof(from);
      
      // wait for the listening socket to get an attempted
      //   client connection
      connected_sock=accept(listen_sock,
                            (struct sockaddr*)&from,
                            &from_len);
      
      // get and process attempted client connection
      doWork(connected_sock, &from);
  }
  // if we get here, things worked just fine. But we should never get here!
  return SUCCESS;
  
}

int requestNumber=0;

void doWork(int connsock, struct sockaddr_in *client_addr)
{
  requestNumber++;
  
  // what is being requested?
  string command="";

  char readBuffer[81];
  int charsRead = read(connsock, readBuffer, 80);
  readBuffer[charsRead]='\0';
  


  if (readBuffer[charsRead-1]=='\n')
    {
      readBuffer[charsRead-2]='\0';
      command += readBuffer;      
      cout << "Got a good command:" << command << ":" << endl;
    }
  else
    {
      cout << "Command not newline terminated:" << command << ":" << endl;
    }

  if (command=="time")
    {
      // what this server generates ...
      string buffer;
      
      // build response string
      time_t currTime = time(nullptr);
      buffer = "The time on this server is :"; 
      buffer += ctime( &currTime ); 
      
      //we simply send this string to the client
      char *cbuff=(char *)buffer.c_str();
      
      int needed=buffer.length();
      while(needed)  // as long as writing is not yet completed, 
	{ 
	  // keep writing more of the buffer
	  int n=write(connsock, cbuff, needed);
	  needed-=n;
	  cbuff+=n;
	}
    }
  else if (command=="count")
    {
      string buffer;
      
      // build response string
      buffer = "Your request number is :";
      buffer += requestNumber; // think about how to fix this!
      buffer += "\r\n"; 
      
      //we simply send this string to the client
      char *cbuff=(char *)buffer.c_str();
      
      int needed=buffer.length();
      while(needed)  // as long as writing is not yet completed, 
	{ 
	  // keep writing more of the buffer
	  int n=write(connsock, cbuff, needed);
	  needed-=n;
	  cbuff+=n;
	}
    }
  else // error command
    {
    }
  // make a local log of who connected ...
  cout << "Connection from " << inet_ntoa(client_addr->sin_addr) << endl;
  
  //close (disconnect) the client socket
  close(connsock);
}
  
