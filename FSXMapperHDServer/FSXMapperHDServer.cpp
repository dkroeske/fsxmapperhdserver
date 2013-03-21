/* *************************************************************** 

	fsxmapserver, Defines the entry point for the console application.

	Queries MS SimConnect getting FSX data. Distributing this data
	via multithreated server

	D.M.Kroeske
	
	1.0		NOV12	Initial release
	
******************************************************************/

#include "stdafx.h"
#include "FSXMapperHDServer.h"
#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include "stdio.h"
#include "strsafe.h"

#include "SimConnect.h"		// Connection to SimConnect
#include "FSInfoServer.h"	// Server, clients can connect to
#include "fsxinfo.h"		// FSX data definition
#include "logger.h"

int quit = 0;
HANDLE hSimConnect = NULL;

static enum EVENT_ID {
	EVENT_SIM_START,
	EVENT_SIM_STOP,
	EVENT_SIM_PAUSED,
	EVENT_SIM_UNPAUSED,
};

static enum DATA_DEFINE_ID {
	DEFINITION_GPS,
	DEFINITION_AIRCRAFTSTRINGDATA,
	DEFINITION_AIRCRAFT_POS_AND_SPEED,
	DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,
	DEFINITION_AIRCRAFT_ENVIRONMENT_DATA,
};

static enum DATA_REQUEST_ID {
	REQUEST_GPS,
	REQUEST_AIRCRAFTSTRINGDATA,
	REQUEST_AIRCRAFT_POS_AND_SPEED,
	REQUEST_AIRCRAFT_FLIGHT_INSTRUMENTATION,
	REQUEST_AIRCRAFT_ENVIRONMENT_DATA,
};

struct StructVS
{
    char strings[1];
};

void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
	HRESULT hr;

	switch( pData->dwID )
	{		
		//
		case SIMCONNECT_RECV_ID_EVENT:
		{
			//Log(0, "SIMCONNECT_RECV_ID_EVENT");

			SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT *)pData;

			switch(evt->uEventID)
			{
				case EVENT_SIM_START:
				{
					// Start getting gps data on periodic interval
					hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_GPS, DEFINITION_GPS,
						SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

					// Start getting 'aircraft string data' only ones
					hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFTSTRINGDATA, DEFINITION_AIRCRAFTSTRINGDATA, 
						SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);

					// Start getting 'aircraft position and speed' on periodic interval
					hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFT_POS_AND_SPEED, DEFINITION_AIRCRAFT_POS_AND_SPEED, 
						SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

					// Start getting 'aircraft flight instrumentation' on periodic interval
					hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFT_FLIGHT_INSTRUMENTATION, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION, 
						SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

					// Start getting 'aircraft environment' on periodic interval
					hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFT_ENVIRONMENT_DATA, DEFINITION_AIRCRAFT_ENVIRONMENT_DATA, 
						SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

					FSXSTATUS_STRUCT *status = fsxInfoStatusGet();
					status->isRunning = true;
					fsxInfoStatusSet(status);

					Log(LOG_SIMSTATUS,"Simulation started");

					break;
				}

				case EVENT_SIM_STOP:
				{	
					FSXSTATUS_STRUCT *status = fsxInfoStatusGet();
					status->isRunning = false;
					fsxInfoStatusSet(status);
					Log(LOG_SIMSTATUS,"Simulation stopped");
					break;
				}

				case EVENT_SIM_PAUSED:
				{	
					FSXSTATUS_STRUCT *status = fsxInfoStatusGet();
					status->isPaused = true;
					fsxInfoStatusSet(status);
					Log(LOG_SIMSTATUS,"Simulation paused");
					break;
				}

				case EVENT_SIM_UNPAUSED:
				{	
					FSXSTATUS_STRUCT *status = fsxInfoStatusGet();
					status->isPaused = false;
					fsxInfoStatusSet(status);
					Log(LOG_SIMSTATUS,"Simulation is running");
					break;
				}

				default:
					break;
			}
			break;
		}

		case SIMCONNECT_RECV_ID_EVENT_FILENAME:
		{
			Log(LOG_INFO,"SIMCONNECT_RECV_ID_EVENT_FILENAME");
			SIMCONNECT_RECV_EVENT_FILENAME *evt = (SIMCONNECT_RECV_EVENT_FILENAME*)pData;
			switch(evt->uEventID)
			{
				case 0: //dummy!!
				default:
					break;
			}
			break;
		}
		
		//
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
		{
			SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *)pData;

			switch( pObjData->dwRequestID )
			{
				case REQUEST_GPS:
				{
					DWORD objectID = pObjData->dwObjectID;					
					fsxInfoGpsSet( (FSXGPS_STRUCT *)&pObjData->dwData);
					break;
				}

				//
				// According to SDK example 'VariableStrings.cpp'
				//
				case REQUEST_AIRCRAFTSTRINGDATA:
				{
					StructVS *pS =  (StructVS *)&pObjData->dwData;
					char *pszType;
					char *pszModel;
					char *pszID;
					char *pszAirline;
					char *pszFlightNumber;
					DWORD cbType;
					DWORD cbModel;
					DWORD cbID;
					DWORD cbAirline;
					DWORD cbFlightNumber;
					if( SUCCEEDED(SimConnect_RetrieveString(pData, cbData, &pS->strings, &pszType, &cbType)) &&
						SUCCEEDED(SimConnect_RetrieveString(pData, cbData, pszType+cbType, &pszModel, &cbModel)) &&
						SUCCEEDED(SimConnect_RetrieveString(pData, cbData, pszType+cbType+cbModel, &pszID, &cbID)) &&
						SUCCEEDED(SimConnect_RetrieveString(pData, cbData, pszType+cbType+cbModel+cbID, &pszAirline, &cbAirline)) &&
						SUCCEEDED(SimConnect_RetrieveString(pData, cbData, pszType+cbType+cbModel+cbID+cbAirline, &pszFlightNumber, &cbFlightNumber)) )
					{
						FSXAIRCRAFT_STRINGDATA_STRUCT *data = fsxAircraftStringDataGet(); 
						strcpy_s(data->atcType, pszType);
						strcpy_s(data->atcModel, pszModel);
						strcpy_s(data->atcID, pszID);
						strcpy_s(data->atcAirline, pszAirline);
						strcpy_s(data->atcFlightNumber, pszFlightNumber);
						fsxAircraftStringDataSet(data);
					}
					else
					{
						Log(LOG_INFO,"Error retrieving: REQUEST_AIRCRAFTSTRINGDATA");
					}
					break;
				}

				case REQUEST_AIRCRAFT_POS_AND_SPEED:
				{
					//printf("\n *** REQUEST_AIRCRAFT_POS_AND_SPEED_STRUCT ***\n");
					DWORD objectID = pObjData->dwObjectID;					
					fsxAircraftPosAndSpeedSet( (FSXAIRCRAFT_POS_AND_SPEED_STRUCT *)&pObjData->dwData);
					break;
				}

				// Aircraft flight instrumentation
				case REQUEST_AIRCRAFT_FLIGHT_INSTRUMENTATION:
				{
					DWORD objectID = pObjData->dwObjectID;					
					fsxAircraftFlightInstrumentationSet( (FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *)&pObjData->dwData);
					break;
				}

				// Aircraft environment data
				case REQUEST_AIRCRAFT_ENVIRONMENT_DATA:
				{
					DWORD objectID = pObjData->dwObjectID;					
					fsxAircraftEnvironmentDataSet( (FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *)&pObjData->dwData);
					break;
				}

				default:
				break;
		
			}
			break;
		}

		case SIMCONNECT_RECV_ID_QUIT:
		{
			//printf("\nSIMCONNECT_RECV_ID_QUIT");
			quit = 1;
			break;
		}

		default:
		{
			//printf("\nReceived:%d",pData->dwID);
			break;
		}
	}
}


DWORD WINAPI setupSimConnect(void *pParams)
{
	HRESULT hr;

	if( SUCCEEDED(SimConnect_Open(&hSimConnect, "fsxmaphd", NULL, 0, 0, 0)))
	{		
		// Avionics Information
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS POSITION LAT", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS POSITION LON", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS POSITION ALT", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS MAGVAR", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS ACTIVE FLIGHT PLAN", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS ACTIVE WAY POINT", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS ARRIVED", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS DIRECTTO FLIGHTPLAN", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS GROUND SPEED", "Meters per second");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS GROUND TRUE HEADING", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS GROUND MAGNETIC TRACK", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS GROUND TRUE TRACK", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP DISTANCE", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP BEARING", "Radians");		
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP TRUE BEARING", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP CROSS TRK", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP DESIRED TRACK", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP TRUE REQ HDG", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP VERTICAL SPEED", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP TRACK ANGLE ERROR", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS ETE", "Seconds");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS ETA", "Seconds");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP NEXT LAT", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP NEXT LON", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP NEXT ALT", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP PREV VALID", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP PREV LAT", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP PREV LON", "Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP PREV ALT", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP ETE", "Seconds");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS WP ETA", "Seconds");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS COURSE TO STEER", "Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS FLIGHT PLAN WP INDEX", "Number");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS FLIGHT PLAN WP COUNT", "Number");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS ACTIVE WP LOCKED", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS APPROACH LOADED", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS IS APPROACH ACTIVE", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH MODE", "Enum");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH WP TYPE", "Enum");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH IS WP RUNWAY", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH SEGMENT TYPE", "Enum");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH APPROACH INDEX", "Number");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH APPROACH TYPE", "Enum");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH TRANSITION INDEX", "Number");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH IS FINAL", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH IS MISSED", "Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH TIMEZONE DEVIATION", "Seconds");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH WP INDEX", "Number");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_GPS, "GPS APPROACH WP COUNT", "Number");

		// Aircraft string data
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFTSTRINGDATA, "ATC TYPE", NULL, SIMCONNECT_DATATYPE_STRINGV);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFTSTRINGDATA, "ATC MODEL", NULL, SIMCONNECT_DATATYPE_STRINGV);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFTSTRINGDATA, "ATC ID", NULL, SIMCONNECT_DATATYPE_STRINGV);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFTSTRINGDATA, "ATC AIRLINE", NULL, SIMCONNECT_DATATYPE_STRINGV);
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFTSTRINGDATA, "ATC FLIGHT NUMBER", NULL, SIMCONNECT_DATATYPE_STRINGV);

		// Aircraft position and speed data
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_POS_AND_SPEED, "GROUND VELOCITY", "Knots");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_POS_AND_SPEED, "PLANE ALT ABOVE GROUND", "Feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_POS_AND_SPEED, "PLANE ALTITUDE", "Feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_POS_AND_SPEED, "GROUND ALTITUDE", "Meters");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_POS_AND_SPEED, "SIM ON GROUND", "Bool");

		// Aircraft Flight Instrumentation data
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"AIRSPEED TRUE","Knots");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"AIRSPEED INDICATED","Knots");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"AIRSPEED TRUE CALIBRATE","Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"AIRSPEED BARBER POLE","Knots");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"AIRSPEED MACH","Mach");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"VERTICAL SPEED","Feet per second");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"MACH MAX OPERATE","Mach");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"STALL WARNING","Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"OVERSPEED WARNING","Bool");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"BARBER POLE MACH","Mach");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"INDICATED ALTITUDE","Feet");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"KOHLSMAN SETTING MB","Millibars");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"KOHLSMAN SETTING HG","Inches of Mercury, inHg");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"ATTITUDE INDICATOR PITCH DEGREES","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"ATTITUDE INDICATOR BANK DEGREES","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"ATTITUDE BARS POSITION","Percent Over 100");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"ATTITUDE CAGE","Bool"); 
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"WISKEY COMPASS INDICATION DEGREES","Degrees");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"PLANE HEADING DEGREES GYRO","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"HEADING INDICATOR","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"GYRO DRIFT ERROR","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"DELTA HEADING RATE","Radians per second");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"TURN COORDINATOR BALL","Position 128 (-127 to 127)");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"ANGLE OF ATTACK INDICATOR","Radians");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_FLIGHT_INSTRUMENTATION,"RADIO HEIGHT","Feet");

		// Aircraft Environment Data
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_ENVIRONMENT_DATA,"AMBIENT TEMPERATURE","Celsius");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_ENVIRONMENT_DATA,"AMBIENT PRESSURE","inHg");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_ENVIRONMENT_DATA,"AMBIENT WIND VELOCITY","Knots");
		hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_ENVIRONMENT_DATA,"AMBIENT WIND DIRECTION","Degrees");

		// Sim Events	
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_STOP, "SimStop");
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_PAUSED, "Paused");
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_UNPAUSED, "Unpaused");

		while( 0 == quit )
		{
			SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
			Sleep(1);
		}
		Log(LOG_ERR,"SimConnect has stopped!!, Please restart.");
		hr = SimConnect_Close(hSimConnect);
	}

	return 0;
}

int testSimConnectAndClose()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "fsxmap", NULL, 0, 0, 0)))
    {
        hr = SimConnect_Close(hSimConnect);
		return OK;
    }
	return NOK;
}

const char g_szClassName[] = "myWindowClass";

#define IDC_MAIN_EDIT	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		{
			HFONT hfDefault;
			HWND hEdit;

			hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL , 
				0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(hEdit == NULL)
				MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		}
		break;
		case WM_SIZE:
		{
			HWND hEdit;
			RECT rcClient;

			GetClientRect(hwnd, &rcClient);

			hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
			SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		}
		break;
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDM_SHOWALLDATA:
					
					// Toggle verbose mode
					LoggerToggleVerboseMode();

					// Update menu
					HMENU menuItem = GetMenu(hwnd);
					LOGVERBOSEMODE_ENUM lvm = LoggerGetCurrentVerboseMode();
					if( LOG_VERBOSE_ON == lvm )
					{
						CheckMenuItem(menuItem, IDM_SHOWALLDATA, MF_CHECKED);
					}
					else
					{
						CheckMenuItem(menuItem, IDM_SHOWALLDATA, MF_UNCHECKED);
					}

					//PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
//	wc.lpszMenuName  = NULL;
	wc.lpszMenuName  = MAKEINTRESOURCE(IDC_FSXMAPPERHDSERVER);
//	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = g_szClassName;
//	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm		 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		"FSX MapperHD Info Server",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 480, 320,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Init the logging
	HWND hEdit;
	hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
	LoggerInit(hEdit);
	Log(LOG_INFO,"FSX MapperHD Server says welcome.\r\n\tCheck http://www.d68.nl/fsxmapperhd for more info.\r\n\r\n\t\"Any landing you can walk away from is a good one!\"\r\n");

	//Init the fsx info module
	fxsInfoInitialize();
	if( OK == testSimConnectAndClose() )
	{
		// Spinoff the thread that gets fsx data
		DWORD nSimConnectThreadID;
		CreateThread(0, 0, setupSimConnect, NULL, 0, &nSimConnectThreadID);
		Log(LOG_INFO,"FSX Connect is responding, This is OK");

		//// Setup server. This call is blocking and spins of multiple connections
		DWORD nSocketServerThreadID;
		int portNr = DEFAULT_PORTNUMBER;
		CreateThread(0, 0, setupServerSocketThread, &portNr, 0, &nSocketServerThreadID);
	}
	else
	{
		Log(LOG_ERR,"Can not connect to FSX!!. Is your flightsim running?");
	}

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

