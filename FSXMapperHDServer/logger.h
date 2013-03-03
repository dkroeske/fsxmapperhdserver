#ifndef LOGGER_INC
#define LOGGER_INC

typedef enum
{
	LOG_ERR = 0,
	LOG_INFO,
	LOG_SIMSTATUS,
	LOG_DATA
} LOGINFO_ENUM;

void Log(LOGINFO_ENUM level, LPCTSTR text);
void LoggerInit(HWND tbDestination);

#endif