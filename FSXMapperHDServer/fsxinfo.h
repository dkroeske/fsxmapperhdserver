#ifndef FSXINFO_INCLUDE
#define FSXINFO_INCLUDE

#define VERSION_INFO	"VERSION: 0.9 (MARCH 2013)"
#define DEFAULT_PORTNUMBER	8100

// FSX GPS information
typedef struct
{
    double latitude;
    double longitude;
	double altitude;
	double magvar;
	double isActiveFlightPlan;
	double isActiveWayPoint;
	double isArrived;
	double isDirectToFlightplan;
	double groundSpeed;
	double groundTrueHeading;
	double groundMagneticTrack;
	double groundTrueTrack;
	double wpDistance;
	double wpBearing;
	double wpTrueBearing; 
	double wpCrossTrack;
	double wpDesiredTrack;
	double wpTrueReqHDG;
	double wpVerticalSpeed;
	double wpTrackAngleError;
	double ETE;
	double ETA;
	double wpNextLat;
	double wpNextLon;
	double wpNextAlt;
	double wpPrevValid;
	double wpPrevLat;
	double wpPrevLon;
	double wpPrevAlt;
	double wpETE;
	double wpETA;
	double wpPrevCourseToSteer;
	double wpFlightPlanWPIndex;
	double wpFlightPlanWPCount;
	double isWPActive;
	double isApprochLoaded;
	double isApprochActive;
	double isApprochMode;
	double isApprochWPType;
	double ApproachIsWPRunway;
	double ApproachSegmentType;
	double ApproachApproachIndex;
	double ApproachApproachType;
	double ApproachTransitionIndex;
	double ApproachIsFinal;
	double ApprochIsMissed;
	double ApprochTimezoneDeviation;
	double ApprochWPIndex;
	double ApprochWPCount;
} FSXGPS_STRUCT;

typedef struct
{
	char atcType[30];
	char atcModel[10];
	char atcID[10];
	char atcAirline[50];
	char atcFlightNumber[6];
} FSXAIRCRAFT_STRINGDATA_STRUCT;


typedef struct
{
	double groundVelocity;
	double planeAltAboveGround;
	double planeAltitude;
	double groundAltitude;
	double simOnGround;
} FSXAIRCRAFT_POS_AND_SPEED_STRUCT;

typedef struct
{
	double airspeedTrue;
	double airspeedIndicated;
	double airspeedTrueCalibrate;
	double airspeedBarberPole;
	double airspeedMach;
	double verticalSpeed;
	double machMaxOperate;
	double stallWarning;
	double overspeedWarning;
	double barberPoleMach;
	double indicatedAltitude;
	double kohlsmanSetting_mb;
	double kohlsmanSetting_hg;
	double attitudeIndicatorPitchDegrees;
	double attitudeIndicatorBankDegrees;
	double attitudeBarsPosition;
	double attitudeCage; 
	double wiskeyCompassIndicationDegrees;
	double planeHeadingDegreesGyro;
	double headingIndicator;
	double gyroDriftError;
	double deltaHeadingRate;
	double turnCoordinatorBall;
	double angleOfAttackIndicator;
	double radioHeight;
} FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT;

typedef struct
{
	double ambientTemperature;
	double ambientPressure;
	double ambientWindVelocity;
	double ambientWindDirection;
} FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT;


// FSX sim status
typedef struct
{
	bool isRunning;
	bool isPaused;
} FSXSTATUS_STRUCT;

void fxsInfoInitialize(void);

// fsx info
FSXGPS_STRUCT *fsxInfoGpsGet(void);
void fsxInfoGpsSet(FSXGPS_STRUCT *gps);

// fsx status
FSXSTATUS_STRUCT *fsxInfoStatusGet(void);
void fsxInfoStatusSet(FSXSTATUS_STRUCT *status);

// fsx Aircraft string data
FSXAIRCRAFT_STRINGDATA_STRUCT *fsxAircraftStringDataGet(void);
void fsxAircraftStringDataSet(FSXAIRCRAFT_STRINGDATA_STRUCT *data);

// fsx position and speed data
FSXAIRCRAFT_POS_AND_SPEED_STRUCT *fsxAircraftPosAndSpeedGet(void);
void fsxAircraftPosAndSpeedSet(FSXAIRCRAFT_POS_AND_SPEED_STRUCT *data);

// fsx aircraft flight instrumentation
FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *fsxAircraftFlightInstrumentationGet(void);
void fsxAircraftFlightInstrumentationSet(FSXAIRCRAFT_FLIGHT_INSTRUMENTATION_STRUCT *data);

// fsx aircraft environment data
FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *fsxAircraftEnvironmentDataGet(void);
void fsxAircraftEnvironmentDataSet(FSXAIRCRAFT_ENVIRONMENT_DATA_STRUCT *data);

#endif