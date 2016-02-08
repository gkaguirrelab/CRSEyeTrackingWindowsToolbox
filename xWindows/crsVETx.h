#include <windows.h>
#include "VSGV8.h"


#define VXE_OK                          0
#define VXE_INTERNALERROR              -1
#define VXE_INVALIDET                  -2
#define VXE_INVALIDREGISTRATION        -3
#define VXE_FAILEDTOINITIALISEDEVICE   -4
#define VXE_NOSOURCE                   -5
#define VXE_NORESULTS                  -6
#define VXE_INVALIDREGION              -7
#define VXE_NOTCALIBRATED              -8
#define VXE_IOERROR                    -9
#define VXE_CANCELLEDBYUSER           -10
#define VXE_INCORRECTTYPE             -11
#define VXE_INVALIDPARAMETER          -12
#define VXE_PROPERTYISREADONLY        -13
#define VXE_FILENOTFOUND              -14
#define VXE_INVALIDPOINTER            -15

// GenericData TypeID codes
// The actual numbers here have no particular significance, except that they must be unique amongst themselves
#define GENERICDATA_INT32                 1
#define GENERICDATA_DOUBLE                2
#define GENERICDATA_BOOLEAN               3
#define GENERICDATA_TWODOUBLE             4
#define GENERICDATA_STRING                5
#define GENERICDATA_UTILTYWINDOWPOSITION 65
#define GENERICDATA_FIXATIONINFO         66
#define GENERICDATA_RESULTS              67
#define GENERICDATA_FORMATSPECIFICATION  68

typedef enum {dtVGA, dtVSG, dtUser}                                         TDeviceType;
typedef enum {vsUserSelect, vsCamera, vsFile, vsCameraI, vsHighSpeedCamera} TVideoSource;
typedef enum {fiSeparatedText, fiBinary, fiMAT}                             TFileFormat;
typedef enum {tsNone,     tsSquare, tsCircle, tsCross, tsChar, tsImage}     TTargetShape;
typedef enum {alHigh,     alMedium, alLow}                                  TAccuracyLevel;
typedef enum {psConstant, psFade}                                           TPersistenceStyle;
typedef enum {ptMotion,   ptMotionAndFixations, ptFixations}                TPersistenceType;
typedef enum {stMimic,    stCamera}                                         TScreenType;  // screen identifiers for CreateUtilityScreen etc
typedef enum {geptLatest, geptGetBuffered, geptGetAndRemoveBuffered}        TGetEyePositionType;

typedef enum {
  pidToolboxVersion,
  pidStimulusDevice,
  pidViewingDistanceMM,
  pidPupilScaleFactor,
  pidFixationPeriod,
  pidFixationRange,
  pidActiveRegion,
  pidUtilityWindowPosition,
  pidMimicPersistence,
  pidMimicPersistenceStyle,
  pidMimicPersistenceType,
  pidMimicTraceColour,
  pidMimicFixationColour,
  pidMimicBackgroundColour,
  pidCalibrated,
  pidPupilCalibrated,
  pidIsTracking,
  pidIsEyeDataAvailable,
  pidRegionsCount,
  pidResultsCount,
  pidVideoSourceFilename,
  pidFixationLocation,
  pidVideoSourceType,
  pidUIMonitor,         // for customizable multi-monitor support
  pidETIBLEDsEnabled,   // set to False to keep the ETIB's front panel LEDs turned off
  pidETIBXYOutputType

} TPropertyID;  // property identifiers for GetProperty and SetProperty

typedef enum
{
  cidRegionChanged,
  cidCollectResults,
  cidClearScreen,
  cidDrawTarget,
  cidFixate,
  cidInitialiseScreen
} TCallbackID;

typedef enum {btNone, btHBitmap, btFile} TBitmapType;

// Eyetracker Interface Box output type
typedef enum {
  otNone,      // "reserved"
  otMM100,     // mm position, 0.01V / mm  (max +-500mm)
  otMM1000,    // mm position, 0.001V / mm (max +-5000mm)
  otHelm10,    // Helmholtz,   0.1V / degree (max +-50 degrees)
  otFicks10    // Ficks,       0.1V / degree (max +-50 degrees)
} TXYOutputType;

#pragma pack(push, 1)

/*
  TDataFormatSpecification = packed record
    Format: TFileFormat;
    IncludeHeader: Boolean;
    IncludeCalibrationData: Boolean;
    TimeStamp: Boolean;
    Tracked: Boolean;
    Calibrated: Boolean;
    ScreenPositionXmm: Boolean;
    ScreenPositionYmm: Boolean;
    FicksLongitude: Boolean;
    FicksLatitude: Boolean;
    HelmholtzAzimuth: Boolean;
    HelmholtzElevation: Boolean;
    PupilDiameter: Boolean;
    Region: Boolean;
    Fixation: Boolean;
    DigitalIO: Boolean;
    Dropped: Boolean;
    SeparatorChar: Char;      // for fiSeparatedText only
    NumericOnly: Boolean;     // for fiSeparatedText only
  end;
*/
// for CommaDelimited, set Format to fiSeparatedText and NumericOnly to False
// for NumericCSV, set Format to fiSeparatedText and NumericOnly to True
typedef struct
{
  DWORD       TypeID;  // TypeID must be GENERICDATA_FORMATSPECIFICATION
  UCHAR       Format;
  UCHAR       IncludeHeader;
  UCHAR       IncludeCalibrationData;
  UCHAR       TimeStamp;
  UCHAR       Tracked;
  UCHAR       Calibrated;
  UCHAR       ScreenPositionXmm;
  UCHAR       ScreenPositionYmm;
  UCHAR       FicksLongitude;
  UCHAR       FicksLatitude;
  UCHAR       HelmholtzAzimuth;
  UCHAR       HelmholtzElevation;
  UCHAR       PupilDiameter;
  UCHAR       Region;
  UCHAR       Fixation;
  UCHAR       DigitalIO;
  UCHAR       Dropped;
  char        SeparatorChar;   // for fiSeparatedText only
  UCHAR       NumericOnly;     // for fiSeparatedText only
} TDataFormatSpecification;

/*
  TetCalibrationParams = packed record
    NumberOfXpoints : Longint;
    NumberOfYpoints : Longint;
    TargetSize      : Double;
    TargetColour    : vsgTRIVAL;
    BackgroundColour: vsgTRIVAL;
    ScaleFactor     : Longint;
    FixationDuration: Longint;
    TargetShape     : TTargetShape;
    AccuracyLevel   : TAccuracyLevel;
    ImageFilename   : array[0..259] of Char;
  end;
*/
typedef struct
{
  long            NumberOfXpoints;
  long            NumberOfYpoints;
  double          TargetSize;
  VSGTRIVAL       TargetColour;
  VSGTRIVAL       BackgroundColour;
  long            ScaleFactor;
  long            FixationDuration;
  char            TargetShape;
  char            AccuracyLevel;
  char            ImageFilename[260];
} TetCalibrationParams;

/*
TetStandardResults = packed record
  TimeStamp          : Int64;
  Tracked            : Boolean;
  Calibrated         : Boolean;
  Fixation           : Boolean;
  Dropped            : Boolean;
  PupilWidth         : Single;
  ScreenPositionXmm  : Single;
  ScreenPositionYmm  : Single;
  Ficks_Longitude    : Single;
  Ficks_Latitude     : Single;
  Helmholtz_Elevation: Single;
  Helmholtz_Azimoth  : Single;
  Region             : Longint;
  DigitalIO          : Longint;
end;
*/
typedef struct
{
  LONGLONG TimeStamp;
  UCHAR    Tracked;
  UCHAR    Calibrated;
  UCHAR    Fixation;
  UCHAR    Dropped;
  float    PupilWidth;
  float    ScreenPositionXmm;
  float    ScreenPositionYmm;
  float    Ficks_Longitude;
  float    Ficks_Latitude;
  float    Helmholtz_Elevation;
  float    Helmholtz_Azimuth;
  long     Region;
  long     DigitalIO;
} TetStandardResults;

/*
TetResultsData = packed record
  TypeID             : DWORD;   // Should be set to GENERICDATA_RESULTS
  TimeStamp          : Int64;
  Tracked            : Boolean;
  Calibrated         : Boolean;
  Fixation           : Boolean;
  Dropped            : Boolean;
  PupilWidth         : Single;
  ScreenPositionXmm  : Single;
  ScreenPositionYmm  : Single;
  Ficks_Longitude    : Single;
  Ficks_Latitude     : Single;
  Helmholtz_Elevation: Single;
  Helmholtz_Azimuth  : Single;
  Region             : Longint;
  DigitalIO          : Longint;
  TriggerADC         : Smallint;
end;
*/
typedef struct
{
  DWORD    TypeID;
  LONGLONG TimeStamp;
  UCHAR    Tracked;
  UCHAR    Calibrated;
  UCHAR    Fixation;
  UCHAR    Dropped;
  float    PupilWidth;
  float    ScreenPositionXmm;
  float    ScreenPositionYmm;
  float    Ficks_Longitude;
  float    Ficks_Latitude;
  float    Helmholtz_Elevation;
  float    Helmholtz_Azimuth;
  long     Region;
  long     DigitalIO;
  short    TriggerADC;
} TetResultsData;

/*
  TBitmapType = (btNone, btHBitmap, btFile);
  TBitmapInfo = packed record
    ScaleToStimulusDevice: Boolean;
    BitmapType: TBitmapType;
    reserved: Word;
    case Byte of
      0: (Handle: HBitmap);
      1: (Filename: PChar);
  end;
*/
typedef struct
{
  UCHAR       ScaleToStimulusDevice;
  UCHAR       BitmapType;
  WORD        reserved;
  union {
    HBITMAP  Handle;
    char    *Filename;
  };
} TBitmapInfo;

typedef struct
{
  DWORD TypeID;
} TGenericData;

typedef struct
{
  DWORD TypeID; // Should be set to GENERICDATA_INT32
  long  Value;
} TInt32Data;

typedef struct
{
  DWORD  TypeID; // Should be set to GENERICDATA_DOUBLE
  double Value;
} TDoubleData;

typedef struct
{
  DWORD TypeID; // Should be set to GENERICDATA_BOOLEAN
  UCHAR  Value;
} TBooleanData;

typedef struct
{
  DWORD TypeID; // Should be set to GENERICDATA_PCHAR
  long  Size;   // size of buffer, in bytes
  char *Buffer;
} TStringData;

typedef struct
{
    DWORD  TypeID;   // Should be set to GENERICDATA_TWODOUBLE
    double Value1;
    double Value2;
} TTwoDoubleData;

typedef struct
{
  DWORD       TypeID;   // Should be set to GENERICDATA_UTILTYWINDOWPOSITION
  UCHAR       WindowID;
  UCHAR       r1;
  WORD        r2;
  long        Left;
  long        Top;
  long        Width;
  long        Height;
} TUtilityWindowPositionData;

typedef struct
{
  DWORD  TypeID;   // Should be set to GENERICDATA_FIXATIONINFO
  UCHAR  Fixation;
  UCHAR  r1;
  WORD   r2;
  double Xposition;
  double Yposition;
} TFixationInfoData;

#pragma pack(pop)

typedef void (__stdcall *TGenericCallbackFunction)(void *p, void *d);


void * _export __stdcall crsvet_CreateEyetracker(                void *reserved);
long   _export __stdcall crsvet_DestroyEyetracker(               void *et);
long   _export __stdcall crsvet_SelectVideoSource(               void *et, UCHAR Source, char *Filename);
long   _export __stdcall crsvet_ClearResultsBuffer(              void *et);
long   _export __stdcall crsvet_StartTracking(                   void *et);
long   _export __stdcall crsvet_StopTracking(                    void *et);
long   _export __stdcall crsvet_Calibrate(                       void *et, TetCalibrationParams *parameters);
long   _export __stdcall crsvet_LoadDefaultCalibrationSettings(  void *et, TetCalibrationParams *parameters);

long   _export __stdcall crsvet_GetEyePosition(                  void *et, TetStandardResults   *Data,      UCHAR Control);
long   _export __stdcall crsvet_StartRecordingToFile(            void *et, char                 *Filename);
long   _export __stdcall crsvet_StopRecording(                   void *et);

long   _export __stdcall crsvet_AddRegion(                       void *et, double LeftMM, double TopMM, double RightMM, double BottomMM);
long   _export __stdcall crsvet_ClearAllRegions(                 void *et);

long   _export __stdcall crsvet_DeleteRegion(                    void *et, long  RegionIndex);
long   _export __stdcall crsvet_LoadCalibrationFile(             void *et, char *Filename);
long   _export __stdcall crsvet_SaveCalibrationFile(             void *et, char *Filename);

long   _export __stdcall crsvet_CreateUtilityScreen(             void *et, UCHAR ScreenType, void *ParentHandle);
long   _export __stdcall crsvet_DestroyUtilityScreen(            void *et, UCHAR ScreenType);

long   _export __stdcall crsvet_SetDeviceParameters(             void *et, UCHAR Device, long ViewingDistance_mm, long Width_mm, long Height_mm);
long   _export __stdcall crsvet_SaveResults(                     void *et, char *Filename, TDataFormatSpecification *Spec);

long   _export __stdcall crsvet_SetMimicWindowBitmap(            void *et, TBitmapInfo *Source);
long   _export __stdcall crsvet_GetMimicWindowBitmap(            void *et, TBitmapInfo *Dest);

long   _export __stdcall crsvet_SetProperty(                     void *et, UCHAR PropertyID, void *Value);
long   _export __stdcall crsvet_GetProperty(                     void *et, UCHAR PropertyID, void *Value);

long   _export __stdcall crsvet_SetCallbackFunction(             void *et, UCHAR CallbackID, TGenericCallbackFunction *CallbackFunction, void *p);

long   _export __stdcall crsvet_GetEyePositionData(              void *et, void *Data, UCHAR Control);

void   _export __stdcall crsvga_Set_DisplayMonitor(              long Value);
