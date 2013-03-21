#ifndef LOGGER_INC
#define LOGGER_INC

typedef enum
{
	LOG_ERR = 0,
	LOG_INFO,
	LOG_SIMSTATUS,
	LOG_DATA
} LOGINFO_ENUM;

typedef enum
{
	LOG_VERBOSE_OFF = 0,
	LOG_VERBOSE_ON
} LOGVERBOSEMODE_ENUM;

void Log(LOGINFO_ENUM level, LPCTSTR text);
void LoggerInit(HWND tbDestination);
void LoggerVerboseMode(LOGVERBOSEMODE_ENUM mode);
LOGVERBOSEMODE_ENUM LoggerGetCurrentVerboseMode(void);
void LoggerToggleVerboseMode();

#endif