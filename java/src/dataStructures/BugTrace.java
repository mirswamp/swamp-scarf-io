package dataStructures;

public class BugTrace {
	private String buildId;
	private String assessmentReportFile;
	private InstanceLocation instanceLocation;
	
	public void setBuildID(String id) {
		buildId = id;
		assessmentReportFile = "";
		instanceLocation = null;
	}
	
	public String getBuildID() {
		return buildId;
	}
	
	public void setAssessmentReportFile(String filepath) {
		assessmentReportFile = filepath;
	}
	
	public String getAssessmentReportFile() {
		return assessmentReportFile;
	}
	
	public void setInstanceLocation(InstanceLocation instanceLoc) {
		instanceLocation = instanceLoc;
	}
	
	public InstanceLocation getInstanceLocation() {
		return instanceLocation;
	}
	
	@Override
	public String toString() {
		String result = "Build ID: " + buildId + "\n";
		result += "Assessment Report File: " + assessmentReportFile + "\n";
		if (instanceLocation != null) { result += instanceLocation.toString(); }
		return result;
	}
	
}
