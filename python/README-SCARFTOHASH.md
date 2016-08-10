### NAME
ScarfToHash - A python script for parsing SCARF
### SYNOPSIS
```python
inputFile = "/my/file/location";

callbacks = {"InitialCallback": INITIALCALLBACK, "MetricCallback": METRICCALLBACK, "BugCallback": BUGCALLBACK, "MetricSummaryCallback": METRICSUMCALLBACK, "BugSummaryCallback": BUGSUMCALLBACK, "CallbackData": DATACALLBACK }

test_reader = ScarfToHash(inputFile, callbacks)

test_reader.parse()
```
### DESCRIPTION
This module provides the ability to convert SCARF files into Python data structures. It is dependant on xml.etree.ElementTree, which is used to handle parsing of the XML document.

The parser is controlled primarily by the callbacks the user sets prior to calling the parse method. A callback will be called once the parser is finished parsing a section of the document. These sections are the beginning AnalyzerReport tag, an entire BugInstance or Metric, a complete BugSummary or MetricSummary and the end AnalyzerReport tag.

All Callbacks except the FinishCallback receive as parameters a dict containing information on their section of parsed data and the data stored in the CallbackData key of the callback dict. Upon reaching the end of an Analyzer Report the FinishCallback  will be called with just the CallbackData key.

While the parser does do minor checks to ensure the inputted file is a SCARF file, if an invalid SCARF file is passed in, most behavior will be undefined.
### METHODS
#### ScarfToHash(FILE, CALLBACKS)
This is a class method used to instatiate the parser. FILE can be an open handle, or a string scalar containing the filename of a file. CALLBACKS is a dict containing specified callback functions for parsed data to be sent to. For additional information on CALLBACKS see below.

#### parse()
This method initiates the parsing of the set file. If parsing fails an exit(1) call will be thrown, otherwise will return 0 on completion.


### CALLBACKS
The main purpose of this module is to interpret data from xml.etree.ElementTree and assemble them into usable Python data structures. When parsing, the module will call the pre-defined callbacks upon completion of parsing an object of their respective type. If defined, all callbacks receive the data contained in the optional key "CallbackData" as a parameter. For details on the structure of each individual Python data structure see below.

#### InitialCallback(INITIALDATA[, CALLBACKDATA])
This is called just after the AnalyzerReport start tag is parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### MetricCallback(METRICDATA[, CALLBACKDATA])
This is called every time a single Metric completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### BugCallback(BUGDATA[, CALLBACKDATA])
This is called every time a single BugInstance completes parsing. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### BugSummaryCallback(BUGSUMMARYDATA[, CALLBACKDATA])
This is called after all BugSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### MetricSummaryCallback(METRICSUMMARYDATA[, CALLBACKDATA])
This is called once all MetricSummaries have been parsed. Any non-zero return value will terminate parsing and skip to FinishCallback.

#### FinishCallback([CALLBACKDATA])
This is called after reaching an AnalayzerReport end tag.


### DATA STRUCTURES

The following are the data structures used in the callbacks listed above. If a keys value is not defined in the SCARF file, then the corresponding key will not exist in the data structures.

#### INITIALDATA
```
{
    'uuid' => UUIDVALUE,
    'tool_name' => TOOLNAMEVALUE,
    'tool_verison' => TOOLVERSIONVALUE 
}
```

#### BUGDATA
```
{
    'AssessmentReportFile' => ASSESSREPORTVALUE,
    'BuildId' => BUILDIDVALUE,
    'BugCode' => CODEVALUE,
    'BugId' => BUGIDVALUE,
    'BugRank' => BUGRANKVALUE,
    'CweIds' => [
        CWEIDVALUE, CWEIDVALUE, CWEIDVALUE
        ]
    'Methods' => [
        {                                    
            'MethodId' => METHODIDVALUE,               
            'name' => METHODNAMEVALUE,             
            'primary' => PRIMARYVALUE
        },
        {                                    
            'MethodId' => METHODIDVALUE,               
            'name' => METHODNAMEVALUE,             
            'primary' => PRIMARYVALUE                   
        }
    ],
    'ClassName' => CLASSVALUE,
    'BugSeverity' => SEVERITYVALUE,
    'BugGroup' => GROUPVALUE,
    'BugLocations' => [
        {                                    
            'EndLine' => ENDLINEVALUE,                
            'LocationId' => LOCIDVALUE,             
            'StartLine' => STARTLINEVALUE,              
            'primary' => PRIMARYVALUE,                  
            'SourceFile' => SOURCEVALUE         
        },
        {                                    
            'primary' => PRIMARYVALUE,                  
            'EndColumn' => ENDCOLUMNVALUE,              
            'EndLine' => ENDLINEVALUE,              
            'Explanation' => EXPLANATIONVALUE,    
            'StartLine' => STARTLINEVALUE,            
            'LocationId' => LOCIDVALUE,             
            'SourceFile' => SOURCEVALUE,        
            'StartColumn' => STARTCOLUMNVALUE             
        }
    ],
    'InstanceLocation' => {                  
        'Xpath' => XPATHVALUE
        'LineNum' => { 
            'Start' = STARTVALUE
            'End' = ENDVALUE
        }
    }
    'BugMessage' => BUGMESSAGEVALUE
    'ResolutionSuggestion' => RESOLUTIONSUGGESTIONVALUE
}
```

#### METRICDATA
```
{
    'Value' => VALUE,          
    'Type' => TYPEVALUE,       
    'Method' => METHODVALUE,   
    'Class' => CLASSVALUE,     
    'SourceFile' => SOURCEVALUE,
    'MetricId' => METRICIDVALUE
}
```

#### BUGSUMMARYDATA
```
{
CODEVALUE => {
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    }
CODEVALUE => {
    GROUPVALUE => {
        'bytes' => BYTESVALUE,
        'count' => COUNTVALUE
        }
    }
}
```

#### METRICSUMMARYDATA
```
{
'MetricSummaries' => [{
    'Type' => TYPEVALUE,
    'Maximum' => MAXVALUE,
    'Sum' => SUMVALUE,
    'Average' => AVERAGEVALUE,
    'StandardDeviation' => STDDEVIATIONVALUE,
    'Count' => COUNTVALUE,
    'Minimum' => MINIMUMVALUE,
    'SumOfSquares' => SUMOFSQVALUE
    },
    {
    'Type' => TYPEVALUE,
    'Maximum' => MAXVALUE,
    'Sum' => SUMVALUE,
    'Average' => AVERAGEVALUE,
    'StandardDeviation' => STDDEVIATIONVALUE,
    'Count' => COUNTVALUE,
    'Minimum' => MINVALUE,
    'SumOfSquares' => SUMOFSQVALUE
    }
}
```

