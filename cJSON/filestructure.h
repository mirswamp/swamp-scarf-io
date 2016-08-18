#ifndef GRANDFATHER_H
#define GRANDFATHER_H


/////////////////////////Summary Data Struct///////////////////////////////////////


typedef struct BugSummary{
    char * code;
    char * group;
    int count;
    int bytes;
    struct BugSummary * next;
} BugSummary;


typedef struct BugSummaries {
    char * code;
    struct BugSummary * codeSummary;
    struct BugSummaries * next;
} BugSummaries;


typedef struct MetricSummary{
    char * type;
    int valid;
    double max;
    double min;
    double sum;
    double sumOfSquares;
    double stdDeviation;
    double average;
    double count;
    struct MetricSummary * next;
} MetricSummary;


/////////////////////////Initial Data Struct///////////////////////////////////////
typedef struct Initial{
    char *tool_name;
    char *tool_version;
    char *uuid;
} Initial;


/////////////////////////Metric Structs////////////////////////////////////////////
typedef struct Metric{
    int id;
    char *value;
    char *className;
    char *methodName;
    char *sourceFile;
    char *type;
} Metric;


///////////////////////////Bug Structs////////////////////////////////////////////
typedef struct Method{
    int methodId;
    int primary;
    char *name;
    struct Method *next;
} Method;

typedef struct Methods{
    int count;
    int size;
    Method *methods;
} Methods;

typedef struct Location{
    int primary;
    int startLine;
    int endLine;
    int startColumn;
    int endColumn;
    int locationId;
    char *explanation;
    char *sourceFile;
} Location;

typedef struct BugLocations{
    int count;
    int size;
    Location *locations;
} BugLocations;

typedef struct {
    int start;
    int end;
} LineNum;


typedef struct {
    LineNum lineNum;
    char *xPath;
} InstanceLocation;


typedef struct CweIds{
    int *cweids;
    int count;
    int size;
} CweIds;


typedef struct {
    int bugId;
    CweIds *cweIds;
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
    Methods *methods;
    BugLocations *bugLocations;
} BugInstance;



#endif

