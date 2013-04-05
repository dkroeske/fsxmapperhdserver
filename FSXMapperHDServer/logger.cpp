#include "stdafx.h"
#include "logger.h"
//#include "stdio.h"
#include <string.h>
#include <tchar.h>

HWND tbLogger = NULL;
LOGVERBOSEMODE_ENUM verboseMode = LOG_VERBOSE_OFF;

void appendLogText(LPCTSTR newText);

void LoggerInit(HWND tbDestination)
{
	tbLogger = tbDestination;
	verboseMode = LOG_VERBOSE_OFF;
}

void LoggerToggleVerboseMode()
{
	if( LOG_VERBOSE_OFF == verboseMode )
	{
		LoggerVerboseMode(LOG_VERBOSE_ON);
	}
	else
	{
		LoggerVerboseMode(LOG_VERBOSE_OFF);
	}
}

void LoggerVerboseMode(LOGVERBOSEMODE_ENUM mode)
{
	verboseMode = mode;
}

LOGVERBOSEMODE_ENUM LoggerGetCurrentVerboseMode(void)
{
	return verboseMode;
}

void Log(LOGINFO_ENUM level, LPCTSTR text)
{
	TCHAR logText[4096];

	switch(level)
	{
		case LOG_ERR:
			wsprintf(logText,_T("<error>\t%s\r\n"), text);
			appendLogText(logText);
			break;
		case LOG_INFO:
			wsprintf(logText,_T("<info>\t%s\r\n"), text);
			appendLogText(logText);
			break;
		case LOG_SIMSTATUS:
			wsprintf(logText,_T("<status>\t%s\r\n"), text);
			appendLogText(logText);
			break;
		case LOG_DATA:
			if( LOG_VERBOSE_ON == verboseMode )
			{
				wsprintf(logText,_T("<data:>\t%s\r\n"), text);
				appendLogText(logText);
			}
			break;
		default:
			wsprintf(logText,_T("<???>\t%s\r\n"), text);
			appendLogText(logText);
			break;
	}
}

// Append text to info already in the editbox.
void appendLogText(LPCTSTR newText)
{
  if(tbLogger != NULL)
  {
	  int ndx = GetWindowTextLength(tbLogger);
	  SetFocus(tbLogger);
	  SendMessage(tbLogger, EM_SETSEL, (WPARAM)ndx,(LPARAM)ndx);
	  SendMessage(tbLogger, EM_REPLACESEL,0,(LPARAM)newText);
  }
}