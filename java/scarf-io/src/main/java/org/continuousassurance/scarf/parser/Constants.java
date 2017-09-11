package org.continuousassurance.scarf.parser;

public class Constants {
	// Analyzer report and attributes
	public static final String ANALYZER_REPORT = "AnalyzerReport";
	public static final String ANALYZER_REPORT_ATTR_ASSESSMENT_FRAMEWORK = "assess_fw";
	public static final String ANALYZER_REPORT_ATTR_ASSESSMENT_FRAMEWORK_VERISON = "assess_fw_version";
	public static final String ANALYZER_REPORT_ATTR_START_TIME = "assessment_start_ts";
	public static final String ANALYZER_REPORT_ATTR_BUILD_FRAMEWORK = "build_fw";
	public static final String ANALYZER_REPORT_ATTR_BUILD_FRAMEWORK_VERSION = "build_fw_version";
	public static final String ANALYZER_REPORT_ATTR_BUILD_ROOT_DIR = "build_root_dir";
	public static final String ANALYZER_REPORT_ATTR_PACKAGE_NAME = "package_name";   
	public static final String ANALYZER_REPORT_ATTR_PACKAGE_ROOT_DIR = "package_root_dir";
	public static final String ANALYZER_REPORT_ATTR_PACKAGE_VERSION = "package_version";
	public static final String ANALYZER_REPORT_ATTR_PARSER_FRAMEWORK = "parser_fw";
	public static final String ANALYZER_REPORT_ATTR_PARSER_FRAMEWORK_VERSION = "parser_fw_version"; 
	public static final String ANALYZER_REPORT_ATTR_PLATFORM_NAME = "platform_name";	
	public static final String ANALYZER_REPORT_ATTR_TOOL_NAME = "tool_name";
	public static final String ANALYZER_REPORT_ATTR_TOOL_VERSION = "tool_version";
	public static final String ANALYZER_REPORT_ATTR_UUID = "uuid";
	
	// Bug Instance
	public static final String BUG_INSTANCE = "BugInstance";
	public static final String BUG_INSTANCE_CLASS_NAME = "ClassName";
	public static final String BUG_INSTANCE_METHODS = "Methods";
	public static final String BUG_INSTANCE_BUG_LOCATIONS = "BugLocations";
	public static final String BUG_INSTANCE_BUG_GROUP = "BugGroup";
	public static final String BUG_INSTANCE_BUG_RANK = "BugRank";
	public static final String BUG_INSTANCE_BUG_SEVERITY = "BugSeverity";
	public static final String BUG_INSTANCE_CWE_ID = "CweId";
	public static final String BUG_INSTANCE_BUG_MESSAGE = "BugMessage";
	public static final String BUG_INSTANCE_RESOLUTION_SUGGESTION = "ResolutionSuggestion";
	public static final String BUG_INSTANCE_BUG_TRACE = "BugTrace";
	public static final String BUG_INSTANCE_BUG_CODE = "BugCode";
	
	// Bug Trace
	public static final String BUG_TRACE_BUILD_ID = "BuildId";
	public static final String BUG_TRACE_ASSESSMENT_REPORT_FILE = "AssessmentReportFile";
	public static final String BUG_TRACE_INSTANCE_LOCATION = "InstanceLocation";
	
	// Instance Location
	public static final String INSTANCE_LOCATION_XPATH = "Xpath";
	public static final String INSTANCE_LOCATION_LINE_NUM = "LineNum";
	
	// Line Num
	public static final String LINE_NUM_START = "Start";
	public static final String LINE_NUM_END = "End";
	
	// Method
	public static final String METHOD = "Method";
	
	// Location
	public static final String LOCATION = "Location";
	public static final String LOCATION_SOURCE_FILE = "SourceFile";
	public static final String LOCATION_START_LINE = "StartLine";
	public static final String LOCATION_END_LINE = "EndLine";
	public static final String LOCATION_START_COLUMN = "StartColumn";
	public static final String LOCATION_END_COLUMN = "EndColumn";
	public static final String LOCATION_EXPLANATION = "Explanation";

	// Metric
	public static final String METRIC = "Metric";
	public static final String METRIC_CLASS = "Class";
	public static final String METRIC_METHOD = "Method";
	public static final String METRIC_TYPE = "Type";
	public static final String METRIC_VALUE = "Value";
	
	// Bug summaries
	public static final String BUG_SUMMARY = "BugSummary";
	public static final String BUG_CATEGORY = "BugCategory";
	public static final String BUG_CATEGORY_COUNT = "count";
	public static final String BUG_CATEGORY_CODE = "code";
	public static final String BUG_CATEGORY_GROUP = "group";
	public static final String BUG_CATEGORY_BYTES = "bytes";
	
	// Metric summaries
	public static final String METRIC_SUMMARIES = "MetricSummaries";
	public static final String METRIC_SUMMARY = "MetricSummary";
	public static final String METRIC_SUMMARY_TYPE = "Type";
	public static final String METRIC_SUMMARY_COUNT = "Count";
	public static final String METRIC_SUMMARY_SUM = "Sum";
	public static final String METRIC_SUMMARY_SUM_OF_SQUARES = "SumOfSquares";
	public static final String METRIC_SUMMARY_MINIMUM = "Minimum";
	public static final String METRIC_SUMMARY_MAXIMUM = "Maximum";
	public static final String METRIC_SUMMARY_AVERAGE = "Average";
	public static final String METRIC_SUMMARY_STANDARD_DEVIATION = "StandardDeviation";
	
	// General
	public static final String ID = "id";
	public static final String PRIMARY = "primary";
}
