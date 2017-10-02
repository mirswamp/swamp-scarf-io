#ifndef SCARFCOMMON_H
#define SCARFCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif
