#ifndef SCARFXML_H
#define SCARFXML_H

/////////////////Summary structs ///////////////////////////
typedef struct BugSummary
{
    int count;
    int byteCount;
    char * code;
    char * group;
    struct BugSummary * next;
} BugSummary;


typedef struct BugSummaries
{
    char * code;
    struct BugSummary * codeSummary;
    struct BugSummaries * next;
} BugSummaries;


typedef struct MetricSummary
{
    double count;
    double sum;
    double min;
    double max;
    double sumOfSquares;
    double stdDeviation;
    double average;
    int valid;
    char * type;
    struct MetricSummary * next;
} MetricSummary;


/////////////////////////Initial Data Struct///////////////////////////////////////
typedef struct Initial
{
    char *assess_fw;
    char *assess_fw_version;
    char *assessment_start_ts;
    char *build_fw;
    char *build_fw_version;
    char *build_root_dir;
    char *package_name;
    char *package_root_dir;
    char *package_version;
    char *parser_fw;
    char *parser_fw_version;
    char *platform_name;
    char *tool_name;
    char *tool_version;
    char *uuid;
} Initial;


/////////////////////////Metric Structs////////////////////////////////////////////
typedef struct Metric
{
    int id;
    char *value;
    char *className;
    char *methodName;
    char *sourceFile;
    char *type;
} Metric;


///////////////////////////Bug Structs////////////////////////////////////////////
typedef struct Method
{
    int methodId;
    int primary;
    char *name;
} Method;

typedef struct Location
{
    int primary;
    int startLine;
    int endLine;
    int startColumn;
    int endColumn;
    int locationId;
    char *explanation;
    char *sourceFile;
} Location;

typedef struct LineNum
{
    int start;
    int end;
} LineNum;

typedef struct InstanceLocation
{
    LineNum lineNum;
    char *xPath;
} InstanceLocation;

typedef struct BugInstance
{
    int bugId;
    int *cweIds;
    int cweIdsCount;
    int cweIdsSize;
    int methodsCount;
    int methodsSize;
    int locationsCount;
    int locationsSize;
    char *className;
    char *bugSeverity;
    char *bugRank;
    char *resolutionSuggestion;
    char *bugMessage;
    char *bugCode;
    char *bugGroup;
    char *assessmentReportFile;
    char *buildId;
    InstanceLocation instanceLocation;
    Method *methods;
    Location *locations;
} BugInstance;


/*************** Common Scarf Types and Functions  *************/
typedef void *(*BugCallback)(BugInstance *bug, void *reference);
typedef void *(*BugSummaryCallback)(BugSummary *bugSum, void *reference);
typedef void *(*MetricCallback)(Metric *metr, void *reference);
typedef void *(*MetricSummaryCallback)(MetricSummary *metrSum, void *reference);
typedef void *(*InitialCallback)(Initial *initial, void *reference);
typedef void *(*FinalCallback)(void *killValue, void *reference);

void DeleteInitial(Initial *initial);
void DeleteMetric(Metric *metric);
void DeleteBug(BugInstance *bug);
BugInstance *CopyBug(BugInstance *bug);
Metric *CopyMetric(Metric *metr);
Initial *CopyInitial(Initial *init);
void DeleteBugSummary(BugSummary *bugSummary);
void DeleteMetricSummary(MetricSummary *metricSummary);

char * CheckStart(Initial * initial);
char * CheckBug(BugInstance * bug);
char * CheckMetric(Metric * metric);



/*************** ScarfXmlReader *************/
typedef struct ScarfXmlReader ScarfXmlReader;

ScarfXmlReader *NewScarfXmlReaderFromFilename(char *filename, char *encoding);
ScarfXmlReader *NewScarfXmlReaderFromString(char *str, char *encoding);
ScarfXmlReader *NewScarfXmlReaderFromFd(int fd, char *encoding);
ScarfXmlReader *NewScarfXmlReaderFromMemory(char *loc, int size, char *encoding);

void SetBugCallback(ScarfXmlReader *reader, BugCallback callback);
void SetMetricCallback(ScarfXmlReader *reader, MetricCallback callback);
void SetBugSummaryCallback(ScarfXmlReader *reader, BugSummaryCallback callback);
void SetMetricSummaryCallback(ScarfXmlReader *reader, MetricSummaryCallback callback);
void SetFinalCallback(ScarfXmlReader *reader, FinalCallback callback);
void SetInitialCallback(ScarfXmlReader *reader, InitialCallback callback);
void SetCallbackData(ScarfXmlReader *reader, void *callbackData);

BugCallback GetBugCallback(ScarfXmlReader *reader);
MetricCallback GetMetricCallback(ScarfXmlReader *reader);
BugSummaryCallback GetBugSummaryCallback(ScarfXmlReader *reader);
MetricSummaryCallback GetMetricSummaryCallback(ScarfXmlReader *reader);
FinalCallback GetFinalCallback(ScarfXmlReader *reader);
InitialCallback GetInitialCallback(ScarfXmlReader *reader);

void *GetCallbackData(ScarfXmlReader *reader);

void * Parse(ScarfXmlReader *hand);
int DeleteScarfXmlReader(ScarfXmlReader *reader);



/*************** ScarfXmlWriter *************/
typedef struct ScarfXmlWriter ScarfXmlWriter;

ScarfXmlWriter * NewScarfXmlWriterFromFd(int fd, char *encoding);
ScarfXmlWriter * NewScarfXmlWriterFromFile(FILE * handle, char *encoding);
ScarfXmlWriter * NewScarfXmlWriterFromFilename(char * filename, char *encoding);
ScarfXmlWriter * NewScarfXmlWriterFromString(char * str, size_t * size, char *encoding);
void CloseScarfXmlWriter (ScarfXmlWriter * writerInfo);
int SetPretty ( ScarfXmlWriter * writerInfo, int pretty_level );
int GetErrorLevel(ScarfXmlWriter * writerInfo);
int SetErrorLevel(ScarfXmlWriter * writerInfo, int errorLevel);
void SetIndent(ScarfXmlWriter * writerInfo, int tabSpace);
int AddBug(ScarfXmlWriter * writerInfo, BugInstance * bug);
int AddMetric(ScarfXmlWriter *  writerInfo, Metric * metric);
int AddStartTag(ScarfXmlWriter * writerInfo, Initial * initial);
int AddEndTag(ScarfXmlWriter * writerInfo);
int AddSummary(ScarfXmlWriter * writerInfo);



/*************** ScarfJsonReader *************/
typedef struct ScarfJSONReader ScarfJSONReader;

ScarfJSONReader * NewScarfJSONReaderFromFilename(char * filename);
ScarfJSONReader * NewScarfJSONReaderFromFd(int fd);
ScarfJSONReader * NewScarfJSONReaderFromFile(FILE * file);
ScarfJSONReader * NewScarfJSONReaderFromString(char * str, size_t * size);
void DeleteScarfJSONReader(ScarfJSONReader * reader);
void ScarfJSONReaderSetUTF8(ScarfJSONReader * reader, int value);
void ScarfJSONReaderSetBugCallback(ScarfJSONReader * reader, BugCallback callback);
void ScarfJSONReaderSetMetricCallback(ScarfJSONReader * reader, MetricCallback callback);
void ScarfJSONReaderSetBugSummaryCallback(ScarfJSONReader * reader, BugSummaryCallback callback);
void ScarfJSONReaderSetMetricSummaryCallback(ScarfJSONReader * reader, MetricSummaryCallback callback);
void ScarfJSONReaderSetFinalCallback(ScarfJSONReader * reader, FinalCallback callback);
void ScarfJSONReaderSetInitialCallback(ScarfJSONReader * reader, InitialCallback callback);
void ScarfJSONReaderSetCallbackData(ScarfJSONReader * reader, void * callbackData);
int ScarfJSONReaderGetUTF8(ScarfJSONReader * reader);
BugCallback ScarfJSONReaderGetBugCallback(ScarfJSONReader * reader);
MetricCallback ScarfJSONReaderGetMetricCallback(ScarfJSONReader * reader);
BugSummaryCallback ScarfJSONReaderGetBugSummaryCallback(ScarfJSONReader * reader);
MetricSummaryCallback ScarfJSONReaderGetMetricSummaryCallback(ScarfJSONReader * reader);
FinalCallback ScarfJSONReaderGetFinalCallback(ScarfJSONReader * reader);
InitialCallback ScarfJSONReaderGetInitialCallback(ScarfJSONReader * reader);
void * ScarfJSONReaderGetCallbackData(ScarfJSONReader * reader);
void * ScarfJSONReaderParse(ScarfJSONReader * hand);



/*************** ScarfJsonWriter *************/
typedef struct ScarfJSONWriter ScarfJSONWriter;

ScarfJSONWriter * NewScarfJSONWriterFromFile(FILE * file);
ScarfJSONWriter * NewScarfJSONWriterFromFilename(char * filename);
ScarfJSONWriter * NewScarfJSONWriterFromString(char * str, size_t *size);
void DeleteScarfJSONWriter (ScarfJSONWriter * writerInfo);
void ScarfJSONWriterSetPretty ( ScarfJSONWriter * writerInfo, int pretty_level );
void ScarfJSONWriterSetUTF8 (ScarfJSONWriter * writerInfo, int utf8);
int ScarfJSONWriterGetPretty (ScarfJSONWriter * writerInfo);
int ScarfJSONWriterGetUTF8 (ScarfJSONWriter * writerInfo);
int ScarfJSONWriterGetErrorLevel(ScarfJSONWriter * writerInfo);
int ScarfJSONWriterSetErrorLevel(ScarfJSONWriter * writerInfo, int errorLevel);
int ScarfJSONWriterAddBug(ScarfJSONWriter * writerInfo, BugInstance * bug);
int ScarfJSONWriterAddMetric(ScarfJSONWriter *  writerInfo, Metric * metric);
int ScarfJSONWriterAddStartTag(ScarfJSONWriter * writerInfo, Initial * initial);
int ScarfJSONWriterAddEndTag(ScarfJSONWriter * writerInfo);
int ScarfJSONWriterAddSummary(ScarfJSONWriter * writerInfo);


#endif
