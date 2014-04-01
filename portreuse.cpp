#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
DWORD WINAPI ClientThread(LPVOID lpParam);
int main()
{
WORD wVersionRequested;
DWORD ret;
WSADATA wsaData;
BOOL val;
SOCKADDR_IN saddr;
SOCKADDR_IN scaddr;
int err;
SOCKET s;
SOCKET sc;
int caddsize;
HANDLE mt;
DWORD tid;

wVersionRequested = MAKEWORD( 2, 2 );
err = WSAStartup( wVersionRequested, &wsaData );
if ( err != 0 ) {
printf("error!WSAStartup failed!\n");
return -1;
}
saddr.sin_family = AF_INET;
/*
������ȻҲ���Խ���ַָ��ΪINADDR_ANY������Ҫ����Ӱ������Ӧ������£�Ӧ��ָ�������IP��
����127.0.0.1�������ķ���Ӧ�ã�Ȼ�����������ַ����ת�����Ϳ��Բ�Ӱ��Է�����Ӧ����
*/
saddr.sin_addr.s_addr = inet_addr("192.168.0.60"); 
saddr.sin_port = htons(23);
if((s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==SOCKET_ERROR)
{
printf("error!socket failed!\n");
return -1;
}
val = TRUE;
//SO_REUSEADDRѡ����ǿ���ʵ�ֶ˿��ذ󶨵�
if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val))!=0)
{
printf("error!setsockopt failed!\n");
return -1;
}
//���ָ����SO_EXCLUSIVEADDRUSE���Ͳ���󶨳ɹ���������Ȩ�޵Ĵ�����룻
//�������ͨ�������ö˿ڴﵽ���ص�Ŀ�ģ��Ϳ��Զ�̬�Ĳ��Ե�ǰ�Ѱ󶨵Ķ˿��ĸ����Գɹ�����˵���߱����©����Ȼ��̬���ö˿�ʹ�ø�����
//��ʵUDP�˿�һ�����������ذ����ã������Ҫ����TELNET����Ϊ���ӽ��й���
if(bind(s,(SOCKADDR *)&saddr,sizeof(saddr))==SOCKET_ERROR)
{
ret=GetLastError();
printf("error!bind failed!\n");
return -1;
}
listen(s,2); 
while(1)
{
caddsize = sizeof(scaddr);
//������������
sc = accept(s,(struct sockaddr *)&scaddr,&caddsize);
if(sc!=INVALID_SOCKET)
{
mt = CreateThread(NULL,0,ClientThread,(LPVOID)sc,0,&tid);
if(mt==NULL)
{
printf("Thread Creat Failed!\n");
break;
}
}
CloseHandle(mt);
}
closesocket(s);
WSACleanup();
return 0;
}
DWORD WINAPI ClientThread(LPVOID lpParam)
{
SOCKET ss = (SOCKET)lpParam;
SOCKET sc;
char buf[4096];
SOCKADDR_IN saddr;
long num;
DWORD val;
DWORD ret;
//��������ض˿�Ӧ�õĻ��������ڴ˴���һЩ�ж�
//������Լ��İ����Ϳ��Խ���һЩ���⴦�����ǵĻ�ͨ��127.0.0.1����ת��

saddr.sin_family = AF_INET;
saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
saddr.sin_port = htons(23);
if((sc=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==SOCKET_ERROR)
{
printf("error!socket failed!\n");
return -1;
}
val = 100;
if(setsockopt(sc,SOL_SOCKET,SO_RCVTIMEO,(char *)&val,sizeof(val))!=0)
{
ret = GetLastError();
return -1;
}
if(setsockopt(ss,SOL_SOCKET,SO_RCVTIMEO,(char *)&val,sizeof(val))!=0)
{
ret = GetLastError();
return -1;
}
if(connect(sc,(SOCKADDR *)&saddr,sizeof(saddr))!=0)
{
printf("error!socket connect failed!\n");
closesocket(sc);
closesocket(ss);
return -1;
}
while(1)
{
//����Ĵ�����Ҫ��ʵ��ͨ��127.0.0.1�����ַ�Ѱ�ת����������Ӧ���ϣ�����Ӧ��İ���ת����ȥ��
//�������̽���ݵĻ��������ٴ˴��������ݷ����ͼ�¼
//����ǹ�����TELNET���������������Ȩ�޵�½�û��Ļ������Է������½�û���Ȼ�����÷����ض��İ��Խٳֵ��û����ִ�С�
num = recv(ss,buf,4096,0);
if(num>0)
send(sc,buf,num,0);
else if(num==0)
break;
num = recv(sc,buf,4096,0);
if(num>0)
send(ss,buf,num,0);
else if(num==0)
break;
}
closesocket(ss);
closesocket(sc);
return 0 ;
} 

