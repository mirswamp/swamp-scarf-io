#ifndef SCARFJSON_H
#define SCARFJSON_H

#include "ScarfCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif

#endif
