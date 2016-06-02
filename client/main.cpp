#include<iostream>
#include<winsock2.h>

#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

using namespace std;

bool WSAstart()
{
WSAData wsa;
WORD Version = MAKEWORD(2, 1); //2,2
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

class udp_client
{
private:
    struct sockaddr_in si_other;
    int udp_client, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

public:
    bool udp_conn();
    bool udp_send_recv_data();
    ~udp_client();
};

udp_client::~udp_client()
{
    shutdown(udp_client,2);
    closesocket(udp_client);
    WSACleanup();
}
bool udp_client::udp_conn()
{
    if(WSAstart())
    {
    //create socket
    if ( (udp_client=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        cout << "socket() failed with error code : "<<WSAGetLastError()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"UDP socket created"<<endl;
    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
    }
    return true;
}

bool udp_client::udp_send_recv_data()
{
    //start communication
    ZeroMemory(buf, sizeof(buf));
    ZeroMemory(message, sizeof(message));
    while(1)
    {

        cout <<"Enter message : "<<endl;
        gets(message);

        //send the message
        if (sendto(udp_client, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
        {
            cout <<"sendto() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }

        // receive some data
        if (recvfrom(udp_client, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
        {
            cout <<"recvfrom() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }
        cout<<"recieve message: "<<buf<<endl;
        ZeroMemory(buf, sizeof(buf));
        ZeroMemory(message, sizeof(message));

    }
}

class tcp_client
{
private:
    struct sockaddr_in client;
    int tcp_client, slen=sizeof(client);
    char buf[BUFLEN];
    char message[BUFLEN];

public:
    bool tcp_conn();
    bool conversation();
    ~tcp_client();
};

tcp_client::~tcp_client()
{
    shutdown(tcp_client,2);
    closesocket(tcp_client);
    WSACleanup();
}

bool tcp_client::tcp_conn()
{
    WSAstart();
    //create socket
    if ( (tcp_client=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
    {
        cout << "socket() failed with error code : "<<WSAGetLastError()<<endl;
        exit(EXIT_FAILURE);
    }
    else cout<<"TCP socket created"<<endl;

    //setup address structure
    memset((char *) &client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.S_un.S_addr = inet_addr(SERVER);

    if(connect(tcp_client, (SOCKADDR*)&client, sizeof(client)))
    {
        cout<<"Connected\n";
    }
    return true;
}


bool tcp_client::conversation()
{
    while(1)
    {
        cout << "Enter message: "<<endl;
        gets(message);

        // send
        if(send(tcp_client, message, MAX_PATH, 0)==SOCKET_ERROR)
        {
            cout <<"send() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }
        else cout<<"data send"<<endl;

        //recieve
        if(recv(tcp_client, buf, sizeof(buf), 0)==SOCKET_ERROR)
        {
            cout <<"recv() failed with error code : "<<WSAGetLastError()<<endl;
            exit(EXIT_FAILURE);
        }
        else cout<<buf<<endl;

        ZeroMemory(buf, sizeof(buf));
        ZeroMemory(message, sizeof(message));
        Sleep (100);
    }
 return true;
}

int main(void)
{
    int select;
    cout<<"Send by TCP - enter 1, by UDP - 2"<<endl;
    cin>>select;
    switch (select)
    {
    case 1:
    {
        tcp_client one;
        one.tcp_conn();
        one.conversation();
     break;
    }
     case 2:
    {
        udp_client two;
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
