


#include <netdb.h>	// for gethostbyname(), socket()
#include <stdio.h>	// for printf(), perror()
#include <stdlib.h>	// for exit()
#include <unistd.h>	// for read(), write(), close()
#include <string.h>	// for strncpy()
#include <iostream>
#include <string>

using namespace std;

#define DEMO_PORT	54321


int main()
{

	char	peername[ 64 ] = { 0 };
	strncpy( peername, "localhost", 63 );
	//--------------------------------------
	// obtain the IP-address for the server
	//--------------------------------------
	struct hostent	*pp = gethostbyname( peername );
	if ( !pp ) { herror( "gethostbyname" ); exit(1); }
	//--------------------------------------------
	// construct a socket-address for this server
	//--------------------------------------------
	struct sockaddr_in	paddr = { 0 };
	socklen_t		palen = sizeof( paddr );
	paddr.sin_family	= AF_INET;
	paddr.sin_port		= htons( DEMO_PORT );
	paddr.sin_addr.s_addr	= *(int32_t*)pp->h_addr;


	//---------------------------------------------------------------
	// prompt the user for the input-message and accept the response
	//---------------------------------------------------------------
	char h;
	do {

	  char	msg[ 80 ] = { 0 };

		cout << "Escolha a ação desejada" << endl;
		cout << "1 - Data e Hora" << endl;
		cout << "2 - SO" << endl;
		cout << "3 - Consulta de Logs" << endl;
		cout << "4 - Abrir um arquivo (4 - nomedoarquivo.ext)" << endl;



		int	n = read( STDIN_FILENO, msg, sizeof( msg ) );

		//---------------------------------------------------
		// create a socket for communication with the server
		//---------------------------------------------------
		int	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( sock < 0 ) { perror( "socket" ); exit(1); }

		//----------------------------------------
		// establish a connection with the server
		//----------------------------------------
		if ( connect( sock, (sockaddr*)&paddr, palen ) < 0 )
			{ perror( "connect" ); exit(1); }

		//-------------------------------------------------------
		// write the message-characters to the socket one-by-one
		//-------------------------------------------------------
		for (int i = 0; i < n; i++)
			if ( write( sock, msg+i, 1 ) < 0 )
				{ perror( "write" ); break; }

		//----------------------------------------------
		// shut down the writing end of this connection
		//-----------------------------------------------
		fflush( fdopen( sock, "rw" ) );
		if ( shutdown( sock, SHUT_WR ) < 0 ) perror( "shutdown" );

		//-----------------------------------------
		// receive all the replies from the server
		//-----------------------------------------
		char	buf[ 500 ] = { 0 };
		int	len = sizeof( buf );
		for (int i = 0; i < len; i++)
			{
			int	rx = read( sock, buf+i, 1 );
			if ( rx < 0 ) { perror( "read" ); break; }
			if ( rx == 0 ) break;
			}

		//--------------------------------------------------
		// close the socket and release its file descriptor
		//--------------------------------------------------
		if ( close( sock ) < 0 ) perror( "close" );

		//-------------------------------------------------
		// display the characters received from the server
		//-------------------------------------------------
		puts( buf );

		cout << "Realizar outra operação?(y/n)\n";
		cin >> h;

	} while(h == 'y');

}
