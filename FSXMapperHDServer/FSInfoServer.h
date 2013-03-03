#ifndef FSINFOSERVER_INC
#define FSINFOSERVER_INC

void ServerThread(void *arg);

DWORD WINAPI handleServerConnection(void *pParam);
void setupServerSocket(int portNumber);
DWORD WINAPI setupServerSocketThread(void *pParams);

#endif