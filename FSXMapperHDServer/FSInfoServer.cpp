/* *************************************************************** 

	Multithreaded Server using Win32

	D.M.Kroeske
	
	1.0		NOV12	Initial release
	
******************************************************************/

#include "stdafx.h"


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>
#include <process.h>
#include "fsxinfo.h"
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")

#include "FSInfoServer.h"

#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "24042"

// local prototyping
void constructGPSjsonString(char *buf, int bufSize);
void constructStatusjsonString(char *buf, int bufSize);
RETVAL_ENUM dispatch(char *cmd, SOCKET socket);

/*****************************************************************************/
void constructGPSjsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXGPS_STRUCT *gpsinfo = fsxInfoGpsGet(); 

	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"gps\":{\n\r"
	"\"longitude\":\"%f\",\n\r"
	"\"latitude\":\"%f\",\n\r"
	"\"altitude\":\"%f\",\n\r"
	"\"magvar\":\"%f\",\n\r"
	"\"isActiveFlightPlan\":\"%f\",\n\r"
	"\"isActiveWayPoint\":\"%f\",\n\r"
	"\"isArrived\":\"%f\",\n\r"
	"\"isDirectToFlightpln\":\"%f\",\n\r"
	"\"groundSpeed\":\"%f\",\n\r"
	"\"groundTrueHeading\":\"%f\",\n\r"
	"\"groundMagneticTrack\":\"%f\",\n\r"
	"\"groundTrueTrack\":\"%f\",\n\r"
	"\"wpDistance\":\"%f\",\n\r"
	"\"wpBearing\":\"%f\"\n\r"
	"}\n\r"
	"}\n\r",
	gpsinfo->longitude, 
	gpsinfo->latitude, 
	gpsinfo->altitude, 
	gpsinfo->magvar,
	gpsinfo->isActiveFlightPlan,
	gpsinfo->isActiveWayPoint,
	gpsinfo->isArrived,
	gpsinfo->isDirectToFlightplan,
	gpsinfo->groundSpeed,
	gpsinfo->groundTrueHeading,
	gpsinfo->groundMagneticTrack,
	gpsinfo->groundTrueTrack,
	gpsinfo->wpDistance,
	gpsinfo->wpBearing
	);

	Log(LOG_DATA,buf);
}


/*****************************************************************************/
void constructStatusjsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXSTATUS_STRUCT *status = fsxInfoStatusGet(); 

	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"status\":{\n\r"
	"\"isRunning\":\"%d\",\n\r"
	"\"isPaused\":\"%d\",\n\r"
	"}\n\r"
	"}\n\r",
	status->isRunning, 
	status->isPaused
	);
	
	Log(LOG_DATA,buf);
}

/*****************************************************************************/
void constructAircraftStringDatajsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXAIRCRAFT_STRINGDATA_STRUCT *aircraftStringData = fsxAircraftStringDataGet(); 

	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"asd\":{\n\r"
	"\"atcType\":\"%s\",\n\r"
	"\"atcModel\":\"%s\",\n\r"
	"\"atcID\":\"%s\",\n\r"
	"\"atcAirline\":\"%s\",\n\r"
	"\"atcFlighNumber\":\"%s\",\n\r"
	"}\n\r"
	"}\n\r",
	aircraftStringData->atcType, 
	aircraftStringData->atcModel,
	aircraftStringData->atcID,
	aircraftStringData->atcAirline,
	aircraftStringData->atcFlightNumber
	);
	
	Log(LOG_DATA,buf);
}

/*****************************************************************************/
void constructAircraftPosAndSpeedjsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXAIRCRAFT_POS_AND_SPEED_STRUCT *aircraftPosAndSpeedData = fsxAircraftPosAndSpeedGet(); 

	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"apas\":{\n\r"
	"\"groundVelocity\":\"%f\",\n\r"
	"\"planeAltAboveGround\":\"%f\",\n\r"
	"\"planeAltitude\":\"%f\",\n\r"
	"\"groundAltitude\":\"%f\",\n\r"
	"\"simOnGround\":\"%f\",\n\r"
	"}\n\r"
	"}\n\r",
	aircraftPosAndSpeedData->groundVelocity, 
	aircraftPosAndSpeedData->planeAltAboveGround,
	aircraftPosAndSpeedData->planeAltitude,
	aircraftPosAndSpeedData->groundAltitude,
	aircraftPosAndSpeedData->simOnGround
	);
	
	Log(LOG_DATA,buf);
}

/*****************************************************************************/
void constructAircraftFlightInstrumentationjsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *aircraftFlightInstrumentation = fsxAircraftFlightInstrumentationGet();
	
	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"afi\":{\n\r"
	"\"airspeedTrue\":\"%f\",\n\r"
	"\"airspeedIndicated\":\"%f\",\n\r"
	"\"airspeedTrueCalibrate\":\"%f\",\n\r"
	"\"verticalSpeed\":\"%f\",\n\r"
	"}\n\r"
	"}\n\r",
	aircraftFlightInstrumentation->airspeedTrue,
	aircraftFlightInstrumentation->airspeedIndicated,
	aircraftFlightInstrumentation->airspeedTrueCalibrate,
	aircraftFlightInstrumentation->verticalSpeed
	);

	Log(LOG_DATA,buf);
}

/*****************************************************************************/
void constructAircraftEnvironmentjsonString(char *buf, int bufSize)
/*
 *
 */
{
	FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *aircraftEnvironmentData = fsxAircraftEnvironmentDataGet(); 
	
	sprintf_s(buf,bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"aed\":{\n\r"
	"\"temperature\":\"%f\",\n\r"
	"\"pressure\":\"%f\",\n\r"
	"\"windVelocity\":\"%f\",\n\r"
	"\"windDirection\":\"%f\",\n\r"
	"}\n\r"
	"}\n\r",
	aircraftEnvironmentData->ambientTemperature,
	aircraftEnvironmentData->ambientPressure,
	aircraftEnvironmentData->ambientWindVelocity,
	aircraftEnvironmentData->ambientWindDirection
	);

	Log(LOG_DATA,buf);
}

/*****************************************************************************/
void constructServerInfo(char *buf, int bufSize)
/*
 *
 */
{	
	sprintf_s(buf, bufSize,"{\n\r"
	"\"id\":\"fsxinfo\","
	"\"serverinfo\":{\n\r"
	"\"serverVersion\":\"%s\",\n\r"
	"}\n\r"
	"}\n\r",
	VERSION_INFO
	);

	Log(LOG_DATA,buf);
}


/*****************************************************************************/
RETVAL_ENUM dispatch(char *cmd, SOCKET socket)
/*
 * Dispatch commands
 */
{
	char *strValue;
	char *nextToken;
	char sendbuf[4096];
	int iSendResult;

	RETVAL_ENUM retval = OK;
  
	// Handle hi?
	if( strcmp(cmd, "hi?")  == 0 )
	{
		//sprintf_s(sendbuf, sizeof(sendbuf),"{\n\r"
		//"\"id\":\"fsxinfo\","
		//"\"serverinfo\":{\n\r"
		//"\"serverVersion\":\"%s\",\n\r"
		//"}\n\r"
		//"}\n\r",
		//VERSION_INFO
		//);
		constructServerInfo(sendbuf, sizeof(sendbuf));
		iSendResult = send(socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	} 

	// Handle gps
	if( strcmp(cmd, "gps?")  == 0 ||
		strcmp(cmd, "g?")  == 0  )
	{
		constructGPSjsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send(socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	}

	// Handle status
	if( strcmp(cmd, "status?")  == 0 ||
		strcmp(cmd, "s?")  == 0  )
	{
		constructStatusjsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send( socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	}  

	// Handle aircraftStringData
	if( strcmp(cmd, "aircraftStringData?")  == 0 ||
		strcmp(cmd, "asd?")  == 0  )
	{
		constructAircraftStringDatajsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send( socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	} 

	// Handle position and speed data
	if( strcmp(cmd, "aircraftPosAndSpeed?")  == 0 ||
		strcmp(cmd, "apas?")  == 0  )
	{
		constructAircraftPosAndSpeedjsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send( socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	}

	// Handle flight instrumentation
	if( strcmp(cmd, "aircraftFlightInstrumentation?")  == 0 ||
		strcmp(cmd, "afi?")  == 0  )
	{
		constructAircraftFlightInstrumentationjsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send( socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	}  

	// Handle aircraft environment data
	if( strcmp(cmd, "aircraftEnvironmentData?")  == 0 ||
		strcmp(cmd, "aed?")  == 0  )
	{
		constructAircraftEnvironmentjsonString(sendbuf, sizeof(sendbuf));
		iSendResult = send( socket, sendbuf, strlen(sendbuf), 0 );
		if( iSendResult == SOCKET_ERROR )
		{
			retval = NOK;
		}
	}  
  
	// Handle set (not used)
	if( strncmp(cmd, "set:", strlen("set:"))  == 0 ||
		strncmp(cmd, "s:", strlen("s:"))  == 0  )
	{
		// Split strings op ':'
		strValue = strtok_s(cmd,":",&nextToken);  // strValue = "set" of "s"
		strValue = strtok_s(NULL,":",&nextToken); // strValue = numerical value;
	} 

	return retval;
}


/*****************************************************************************/
DWORD WINAPI handleServerConnection(void *pParam)
/*
 * Thread, handling connection
 */
{
    SOCKET ClientSocket = (SOCKET)pParam;
	int iResult;
	RETVAL_ENUM retval = OK;	
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

	TCHAR logText[255];

	char cmdInBuffer[255] = {'\0'};
	UINT8 cmdInIndex = 0;
	
	do 
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			// Get char's, check for \n or \r and execute command
			for(int idx = 0; idx < iResult; idx++ )
			{
				char ch = recvbuf[idx];
				if( ch == '\n' || ch == '\r' )
				{
					// Add '\0' terminator
					cmdInBuffer[cmdInIndex] = '\0';
					retval = dispatch(cmdInBuffer, ClientSocket);
					cmdInIndex = 0;  
				}
				else
				{
					cmdInBuffer[cmdInIndex] = ch;
					cmdInIndex++;
				}
			}

			if( NOK == retval )
			{
				//printf("\nWSAGetLastError: %d", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();

				// Inform user
				wsprintf(logText, _T("handleServerConnection(...) failed with error: %d"), WSAGetLastError());
				Log(LOG_ERR,logText);
			}
		}
		else 
		{
			if (iResult == 0)
			{
				// Inform user connection is closing
				Log(LOG_INFO, _T("iOS device disconnects, bye!"));
			}
			else
			{
				closesocket(ClientSocket);
				WSACleanup();
				
				// Inform user
				wsprintf(logText, _T("recv failed with error: %d"), WSAGetLastError());
				Log(LOG_ERR,logText);

				retval = NOK;
			}
		}

	} while (iResult > 0 && OK == retval);

	return retval;
}

DWORD WINAPI setupServerSocketThread(void *pParams)
{
	int port = *((int *)pParams);
	setupServerSocket(port);
	return 0;
}

/* **************************************************************** */
void setupServerSocket(int portNumber)
/*
 * 
 */
{
    WSADATA wsaData;
    int iResult;
	RETVAL_ENUM retval = OK; 
	TCHAR logText[255];

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    // Initialize Winsock
    if(OK == retval)
	{
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0)
		{
			// Inform user
			wsprintf(logText, _T("WSAStartup(...) failed with error: %d"), iResult);
			Log(LOG_ERR,logText);

			retval = NOK;
		}
	}

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    if(OK == retval)
	{
		char strPortNumber[10];
		_itoa_s(portNumber,strPortNumber, 10);
		iResult = getaddrinfo(NULL, strPortNumber, &hints, &result);
		if ( iResult != 0 )
		{
			// Cleanup
			WSACleanup();
			
			// Inform user
			wsprintf(logText, _T("getaddrinfo(...) failed with error: %d"), iResult);
			Log(LOG_ERR,logText);
			
			retval = NOK;
		}
    }

    // Create a SOCKET for connecting to server
    if(OK == retval)
	{
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET)
		{
			// Cleanup
			freeaddrinfo(result);
			WSACleanup();

			// Inform user
			wsprintf(logText, _T("socket(...) failed with error: %d"), WSAGetLastError());
			Log(LOG_ERR,logText);

			retval = NOK;
		}
	}

    // Setup the TCP listening socket
	if(OK == retval)
	{
		iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			// Cleanup
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			
			// Inform user
			wsprintf(logText, _T("bind(...) failed with error: %d"), WSAGetLastError());
			Log(LOG_ERR,logText);
			
			retval = NOK;
		}
	}

    freeaddrinfo(result);

    if(OK == retval)
	{
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
		{
			//Clean up
			closesocket(ListenSocket);
			WSACleanup();

			// Inform user
			wsprintf(logText, _T("listen(...) failed with error: %d"), WSAGetLastError());
			Log(LOG_ERR,logText);

			retval = NOK;
		}
    }

	if(OK == retval)
	{
		// Get socket IP address
		hostent* localHost;
		localHost = gethostbyname("");
		char *localIP = inet_ntoa (*(struct in_addr *)*localHost->h_addr_list);
		
		// Inform user
		wsprintf(logText,_T("Info Server is running. This is OK\r\n\tServer IP = %s, PORT = %d"), localIP, portNumber);
		Log(LOG_INFO,logText);
	}
   
	// Accept a client socket
    while(OK == retval)
	{
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			// Cleanup
			closesocket(ListenSocket);
			WSACleanup();

			// Inform user
			wsprintf(logText, _T("Accept failed with error: %d"), WSAGetLastError());
			Log(LOG_ERR,logText);

			retval = NOK;
		}
		else
		{
			DWORD nThreadID;
            CreateThread(0,0, handleServerConnection, (void *)ClientSocket, 0, &nThreadID);

			// Get remote IP and inform user
			struct sockaddr_in  address = {0};
			socklen_t addressLenght = sizeof(address);
			int ipAddr = getpeername(ClientSocket, (struct sockaddr*) &address, &addressLenght);
			
			// Inform user
			char *str = inet_ntoa(address.sin_addr);
			wsprintf(logText, _T("iOS device with IP = %s is connected!"), str);
			Log(LOG_INFO,logText);
		}
    }

    // No longer need server socket
    closesocket(ListenSocket);
}
