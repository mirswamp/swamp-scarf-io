
### NAME
HashToScarf - A Python script for writing Scarf
### SYNOPSIS
```python
from HashToJSON import HashToJSON

outputFile = "/path/to/file"

writer = HashToJSON(outputFile, ERROR_LEVEL)
writer.addStartTag(INITIAL)
writer.addBugInstance(BUG)
writer.addMetric(METRIC)
writer.addSummary()
writer.addEndTag()

```
### DESCRIPTION
This module provides the ability to convert Python data structures into Scarf formatted documents. It is dependant on lxml, which is used as a tool for writing.

The writer is controlled primarily by the user through method calls which feed data in to be written. This data is to be structured as a dict containing all fields for a specific section. For details on how the data structures are formatted see below.

The user has the ability to set the error level to 0 (none), 1 (warnings), or 2 (exit).
If the error level is 1 or 2, the writer will check to ensure that the Scarf is valid and will print out any errors found.
Also if the error level is 2 and errors are found, then the program will exit to ensure the validity of results.
Errors consist of misformatted elements, required elements not found, wrong value type, and ordering violations.

### METHODS
#### new(FILE, ERRORLEVEL)
This is a class method used to instantiate the writer. FILE is any object with a write method or a string filename. Errorlevel is 0, 1, or 2 and sets the error level of the writer (default 2). Pretty print is set to false. 

#### getFile()
Returns the set file handle.

#### getErrorLevel()
Returns the current error level of the writer.

#### setErrorLevel(ERRORLEVEL)
Allows changing the error level of the writer to 0, 1, or 2. If ERRORLEVEL does not equal one of these values does nothing.

#### getPretty()
Returns the current value set to pretty print.

#### setPretty(PRETTYENABLE)
If PRETTYENABLE is a true value then enable pretty printing, else disable pretty printing.

#### addStartTag(INITIALDATA)
Writes a start tag to the file based on INITIALDATA. For details on valid [data structures](#data-structures) see below.

#### addBugInstance(BUGDATA) 
Writes a bug to the file based on BUGDATA. For details on valid data structures see below.

#### addMetric(METRICDATA)
Writes a metric to the file based on METRICDATA. For details on valid data structures see below.

#### addSummary()
Writes a summary to the file based on all bugs and metrics already written with this writer.

#### addEndTag()
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
