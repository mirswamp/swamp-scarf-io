#ifndef SCARFXML_H
#define SCARFXML_H

#include "ScarfCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif
