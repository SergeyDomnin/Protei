#include <iostream>
#include <winsock2.h>
#include <vector>
#include <algorithm>

#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

using namespace std;

void input_calculate(char *buf);
bool WSAstart()
{
WSAData wsa;
WORD Version = MAKEWORD(2, 1);//2,2
WSAStartup(Version, &wsa);

if(WSAStartup(Version, &wsa) != 0)
    {
    cout << "Can't load WSA library." << endl;
    return false;
    }
else
    {
    cout << "WSA library load complite" << endl;
    return true;
    }
}

class udp_server
{
private:
    SOCKET udp_connect;
    struct sockaddr_in udpp_server, si_other;
    int slen , recv_len;

public:
     char buf[BUFLEN];
      bool udp_conn();
      bool udp_send_recv_data();
      ~udp_server();
};

udp_server::~udp_server()
{
    shutdown(udp_connect,2);
    closesocket(udp_connect);
    WSACleanup();

}
bool udp_server::udp_conn()
{
    if(WSAstart())
    {
    slen = sizeof(si_other) ;
    //Create a socket
    if((udp_connect = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
         cout << "Could not create socket : " <<WSAGetLastError()<<endl;
    }
     cout << "UDP socket created."<<endl;

    //Prepare the sockaddr_in structure
    memset((char *) &si_other, 0, sizeof(si_other));
    udpp_server.sin_family = AF_INET;
    udpp_server.sin_addr.s_addr = INADDR_ANY;
    udpp_server.sin_port = htons( PORT );

    //Bind
    if( bind(udp_connect ,(struct sockaddr *)&udpp_server , sizeof(udpp_server)) == SOCKET_ERROR)
    {
         cout << "Bind failed with error code : "<<WSAGetLastError()<<endl;
         exit(EXIT_FAILURE);
    }
    cout<<"Bind done"<<endl;
    }
    return true;
}

bool udp_server::udp_send_recv_data()
{
    ZeroMemory(buf, sizeof(buf));

    while(true)
    {
        cout <<"Waiting for data..."<<endl;

        //clear the buffer

        //try to receive some data
        if ((recv_len = recvfrom(udp_connect, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            cout <<"recvfrom() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }

        cout <<"Data:  "<<buf<<endl;
        input_calculate(buf);

        //now reply the client
        if (sendto(udp_connect, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
        {
            cout <<"sendto() failed with error code :  "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }

        ZeroMemory(buf, sizeof(buf));
        Sleep (100);
    }
    return true;
}

class tcp_server
{
private:
    SOCKET tcp_connect;
    SOCKET tcp_listen;
    struct sockaddr_in tcpp_server;
    int slen , recv_len;

public:
      char buf[BUFLEN];
      bool tcp_conn();
      bool tcp_send_recv_data();
      ~tcp_server();
};

tcp_server::~tcp_server()
{
    shutdown(tcp_listen,0);
    shutdown(tcp_connect,2);
    closesocket(tcp_listen);
    closesocket(tcp_connect);
    WSACleanup();
}

bool tcp_server::tcp_conn()
{
    WSAstart();

    //Create a socket
    if((tcp_listen = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET ||
            (tcp_connect = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET )
    {
         cout << "Could not create socket : " <<WSAGetLastError()<<endl;
    }
     cout << "TCP socket created."<<endl;

    //Prepare the sockaddr_in structure
    memset((char *) & tcpp_server, 0, sizeof( tcpp_server));
    tcpp_server.sin_family = AF_INET;
    tcpp_server.sin_port = htons( PORT );
    tcpp_server.sin_addr.s_addr = INADDR_ANY;

    //Bind
    if( bind(tcp_listen ,(struct sockaddr *)&tcpp_server , sizeof(tcpp_server)) == SOCKET_ERROR)
    {
         cout << "Bind failed with error code : "<<WSAGetLastError()<<endl;
         exit(EXIT_FAILURE);
    }
    cout<<"Bind done"<<endl;

    listen(tcp_listen, SOMAXCONN);

    if((tcp_connect = accept(tcp_listen, NULL, NULL)) != SOCKET_ERROR) cout << "Client accepted: " << endl;

    return true;
}

bool tcp_server::tcp_send_recv_data()
{

    while(true)
    {
        //recieve
        if (recv(tcp_connect, buf, BUFLEN, 0)== SOCKET_ERROR)
        {
            cout <<"recvfrom() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }


        if ( send(tcp_connect, buf, BUFLEN, 0)== SOCKET_ERROR)
        {
            cout <<"sendto() failed with error code :  "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }

        cout <<"Data:  "<<buf<<endl;
        input_calculate(buf);

        ZeroMemory(buf, sizeof(buf));


    }
    return true;
}


void  input_calculate(char *buf)
{
    vector <int> vec;

    int sum=0;
    char *s = new char[BUFLEN + 1];
    strcpy(s, buf);
    char * ptrEnd;
    char * pch = strtok(s, " ");
    while (pch != NULL)
        {
            int b = strtod(pch, &ptrEnd);
            if (ptrEnd[0] == '\0' && (b>=0 && b<=9)) vec.push_back(b);
            pch = strtok(NULL, " ,");
        }
    if(!vec.empty())
    {
    sort(vec.begin(), vec.end(), greater<int>());
    cout << "numbers:";
    for (vector<int>::iterator it = vec.begin() ; it != vec.end(); ++it)
    cout << ' ' << *it;
    cout << '\n';
    auto minmax = std::minmax_element(vec.begin(), vec.end());
    cout <<"min= "<<*minmax.first << " " <<"max= "<<*minmax.second <<endl;
    cout <<"summa= "<<accumulate(vec.begin(),vec.end(),sum)<<endl;
    }
    delete s;
    vec.clear();
}

int main()
{
    int select;
    cout<<"Recieve by TCP - enter 1, by UDP - 2"<<endl;
    cin>>select;
    switch (select)
    {
    case 1:
        {
        tcp_server one;
        one.tcp_conn();
        one.tcp_send_recv_data();
        break;
        }
     case 2:
        {
        udp_server two;
        two.udp_conn();
        two.udp_send_recv_data();
        break;
        }
    default:
        cout<<"Error, bad input, quitting\n";
        break;
    }

    return 0;
}
