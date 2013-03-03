#include "stdafx.h"
#include <Windows.h>

#include "fsxinfo.h"


static FSXGPS_STRUCT gpsInfo;
static FSXSTATUS_STRUCT statusInfo;
static FSXAIRCRAFT_STRINGDATA_STRUCT aircraftStringDataInfo;
static FSXAIRCRAFT_POS_AND_SPEED_STRUCT aircraftPosAndSpeedInfo;
static FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT aircraftFlightInstrumentation;
static FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT aircraftEnvironmentData;


CRITICAL_SECTION writeLock;

/* ************************************************************************** */
void fxsInfoInitialize(void)
/*
 *
 */
{
	InitializeCriticalSection(&writeLock);
}

/* ************************************************************************** */
FSXGPS_STRUCT *fsxInfoGpsGet(void)
/*
 *
 */
{
	return &gpsInfo;
}

/* ************************************************************************** */
void fsxInfoGpsSet(FSXGPS_STRUCT *gps)
/*
 *
 */
{
	EnterCriticalSection(&writeLock);
	::gpsInfo = *gps;
	LeaveCriticalSection(&writeLock);
	
	//printf("\nLat=%f  Lon=%f  Alt=%f", ::gpsinfo.latitude, ::gpsinfo.longitude, ::gpsinfo.altitude);
}

/* ************************************************************************** */
FSXSTATUS_STRUCT *fsxInfoStatusGet(void)
/*
 *
 */
{
	return &statusInfo;
}

/* ************************************************************************** */
void fsxInfoStatusSet(FSXSTATUS_STRUCT *status)
/*
 *
 */
{
	EnterCriticalSection(&writeLock);
	::statusInfo = *status;
	LeaveCriticalSection(&writeLock);
}

/* ************************************************************************** */
FSXAIRCRAFT_STRINGDATA_STRUCT *fsxAircraftStringDataGet(void)
/*
 *
 */
{
	return &aircraftStringDataInfo;
}

/* ************************************************************************** */
void fsxAircraftStringDataSet(FSXAIRCRAFT_STRINGDATA_STRUCT *data)
/*
 *
 */
{
	EnterCriticalSection(&writeLock);
	::aircraftStringDataInfo = *data;
	LeaveCriticalSection(&writeLock);
}

FSXAIRCRAFT_POS_AND_SPEED_STRUCT *fsxAircraftPosAndSpeedGet(void)
{
	return &aircraftPosAndSpeedInfo;
}

void fsxAircraftPosAndSpeedSet(FSXAIRCRAFT_POS_AND_SPEED_STRUCT *data)
{
	EnterCriticalSection(&writeLock);
	::aircraftPosAndSpeedInfo = *data;
	LeaveCriticalSection(&writeLock);
}

// fsx aircraft flight instrumentation
FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *fsxAircraftFlightInstrumentationGet(void)
{
	return &aircraftFlightInstrumentation;
}

void fsxAircraftFlightInstrumentationSet(FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *data)
{
	EnterCriticalSection(&writeLock);
	::aircraftFlightInstrumentation = *data;
	LeaveCriticalSection(&writeLock);
}

// fsx aircraft environment data
FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *fsxAircraftEnvironmentDataGet(void)
{
	return &aircraftEnvironmentData;
}

void fsxAircraftEnvironmentDataSet(FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *data)
{
	EnterCriticalSection(&writeLock);
	::aircraftEnvironmentData = *data;
	LeaveCriticalSection(&writeLock);
}

