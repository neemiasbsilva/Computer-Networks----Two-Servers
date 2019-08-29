#include <stdio.h>	// for printf(), perror()
#include <stdlib.h>	// for exit()
#include <unistd.h>	// for read(), write(), close()
#include <netinet/in.h>	// for socket(), bind(), listen(),
#include <iostream>
#include <string>
#include <dirent.h>
#include <cstdlib>
#include <string.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <fstream>

using namespace std;

#define DEMO_PORT	54000


string datahora(){
  time_t timer;
  struct tm *horario;

  time(&timer);
  horario = localtime(&timer);

  int dia = horario->tm_mday;
  int mes = horario->tm_mon + 1;
  int ano = horario->tm_year + 1900;

  int hora = horario->tm_hour;
  int min  = horario->tm_min;
  int sec  = horario->tm_sec;

  string retorno = "";
  retorno = to_string(dia) + "/" + to_string(mes) + "/" +to_string(ano) + "  " +
  to_string(hora) + ":" + to_string(min) + ":" + to_string(sec);

  return retorno;
}


string ler(string arquivo){

	ifstream leitura;
	leitura.open(arquivo);
	string enviar;
	char mostra[100000];


	while(leitura.getline(mostra,100000)){
		enviar = enviar + mostra + "\n";
	}
	leitura.close();

  return enviar;
}


void criarlog(string nome){
	string conteudo = datahora();
	ofstream escreve;

  escreve.open("logsserver2.txt",ofstream::app);

  escreve << conteudo << endl << "O Arquivo: " << nome << " foi consultado" << endl << endl;

  escreve.close();
}

string listaArquivos (string nomeDir, string nomeArq) {
    DIR *dir = 0;
    struct dirent *entrada = 0;
    unsigned char isDir = 0x4;
    unsigned char isFile = 0x8;

    dir = opendir (nomeDir.c_str());

    if (dir == 0) {
        std::cerr << "Nao foi possivel abrir diretorio." << std::endl;
        exit (1);
    }

    //Iterar sobre o diretorio
    while (entrada = readdir (dir)){
      if (entrada->d_type == isFile){

        if(entrada->d_name == nomeArq){
          closedir (dir);
          criarlog(nomeArq);
          nomeArq = "server2/" + nomeArq;
          return "code: 200\n " + ler(nomeArq);
        }
      }
    }

    closedir (dir);
    return "code: 404\n O Arquivo não existe";
}

void criarlog(int m){
	string conteudo = datahora();
	ofstream escreve;

  escreve.open("logs.txt",ofstream::app);

	switch (m) {
		case 1:
			escreve << conteudo << endl << "Data e Hora Consultada" << endl << endl;
			break;

		case 2:
		  escreve << conteudo << endl << "SO solicitado" << endl << endl;
			break;
		}


  escreve.close();
}


int main( int argc, char **argv )
{
	//--------------------------------------------------
	// create a socket-address structure for our socket
	//--------------------------------------------------
	struct sockaddr_in	haddr = { 0 };
	socklen_t		halen = sizeof( haddr );
	haddr.sin_family	= AF_INET;
	haddr.sin_port		= htons( DEMO_PORT );
	haddr.sin_addr.s_addr	= htonl( INADDR_ANY );

	//---------------------------------------------
	// create the listening socket for this server
	//---------------------------------------------
	int	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( sock < 0 ) { perror( "socket" ); exit(1); }

	//-----------------------------------------------------
	// bind this listening socket to our fixed port-number
	//-----------------------------------------------------
	if ( bind( sock, (sockaddr*)&haddr, halen ) < 0 )
		{ perror( "bind" ); exit(1); }

	//-----------------------------------------------------
	// create a connection queue for this listening socket
	//-----------------------------------------------------
	if ( listen( sock, 5 ) < 0 )
		{ perror( "listen" ); exit(1); }

	//------------------------------------------------
	// enter the main look to process client requests
	//------------------------------------------------
	do	{


		printf( "\nserver is listening on port %u \n", DEMO_PORT );

		//--------------------------------------------------
		// accept the next connection-request from a client
		//--------------------------------------------------
		struct sockaddr_in	paddr = { 0 };
		socklen_t		palen = sizeof( paddr );
		int	conn = accept( sock, (sockaddr*)&paddr, &palen );
		if ( conn < 0 ) { perror( "accept" ); exit(1); }
		//--------------------------------------------------
		// we can now receive characters sent by the client
		//--------------------------------------------------
		char	buf[ 5000 ] = {0};
		int	len = sizeof( buf );
		int	rx = 0;
		while ( rx < len ){
			int	inch = 0;
			int	i = read( conn, &inch, 1 );
			if ( i < 0 ) { perror( "read" ); exit(1); }
			if ( i == 0 ) break;
			buf[ rx ] = inch;
			++rx;
		}
		string receber = "";

		for(int i = 0; i < rx;i++){
			receber = receber +  buf[i];
		}

    //Verificar nos arquivos se existe algum com o mesmo nome e retornar seu conteúdo

    string enviar = listaArquivos("server2",receber);



		//Transfere para o buffer o conteudo da string enviar

		for(int i = 0; i < enviar.length();i++){
			buf[i] = enviar[i];
		}

		int Tamanho = enviar.length();
		rx = Tamanho;

		//----------------------------------------------
		// send the modified message back to the client
		//----------------------------------------------
		int	tx = write( conn, buf, rx );
		if ( tx < 0 ) { perror( "write" ); exit(1); }

		//----------------------------------------------------
		// flush the output buffers and close this connection
		//----------------------------------------------------
		fflush( fdopen( conn, "rw" ) );
		close( conn );

		printf( "data was received, processed, and returned\n" );
		}
	while( 1 );
}
