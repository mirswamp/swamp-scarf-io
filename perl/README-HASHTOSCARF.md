
### NAME
ScarfToHash - A perl module for parsing Scarf
### SYNOPSIS
```perl
use ScarfToHash;

my $output = "/path/to/file";

my $test_writer = new HashToScarf($output, 0);

$test_writer->addStartTag($initial_details);
$test_writer->addBugInstance($bug);
$test_writer->addMetric($metric);
$test_writer->addSummary;
$test_writer->addEndTag;

```
### DESCRIPTION
This module provides the ability to convert Perl data structures into Scarf formatted documents. It is dependant on XML::Writer, which is used as a tool for writing.

The writer is controlled primarily by the user through method calls which feed data in to be written.

The user has the ability to set the error level to 0 (none), 1 (warnings), or 2 (exit).
If the error level is 1 or 2, the writer will check to ensure that the Scarf is valid and will print out any errors found.
Also if the error level is 2 and errors are found, then the program will exit to ensure the validity of results.

### METHODS
#### new(HANDLE [, ERRORLEVEL, PRETTYENABLE])
This is a class method used to instatiate the writer. Handle is an object blessed into IO::Handle or one of its subclasses, or a string to a file path. Errorlevel is 0, 1, or 2 and sets the error level of the writer (default 2). Prettyenable will enable or disable pretty printing of output with a true or false value respectively (default true).

#### getWriter
Returns the XML::Writer used for writing.

#### getHandle
Returns the set handle.

#### getErrorLevel
Returns the current error level of the writer.

#### setErrorLevel(ERRORLEVEL)
Allows changing the error level of the writer to 0, 1, or 2. If ERRORLEVEL does not equal one of these values does nothing.

#### getPretty 
Returns the current value set to pretty print.

#### setPretty(PRETTYENABLE)
If PRETTYENABLE is a true value then enable pretty printing, else disable pretty printing.

#### addStartTag(INITIALDATA)
Writes a start tag to the file based on INITIALDATA. For details on valid [data structures](#data-structures) see below.

#### addBugInstance(BUGDATA) 
Writes a bug to the file based on BUGDATA. For details on valid data structures see below.

#### addMetric(METRICDATA)
Writes a metric to the file based on METRICDATA. For details on valid data structures see below.

#### addSummary
Writes a summary to the file based on all bugs and metrics already written with this writer.

#### addEndTag
Writes an end tag to the file.


### DATA STRUCTURES


The following are the data structures used in the methods listed above. Undefined keys will not be written. 


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
