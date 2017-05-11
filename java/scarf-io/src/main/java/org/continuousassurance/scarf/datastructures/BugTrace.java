package org.continuousassurance.scarf.datastructures;

public class BugTrace {
	private transient String buildId;
	private String assessmentReportFile;
	private InstanceLocation instanceLocation;
	
	public void setBuildID(final String id) {
		buildId = id;
	}
	
	public String getBuildID() {
		return buildId;
	}
	
	public void setAssessmentReportFile(final String filepath) {
		assessmentReportFile = filepath;
	}
	
	public String getAssessmentReportFile() {
		return assessmentReportFile;
	}
	
	public void setInstanceLocation(final InstanceLocation instanceLoc) {
		instanceLocation = instanceLoc;
	}
	
	public InstanceLocation getInstanceLocation() {
		return instanceLocation;
	}
	
	@Override
	public String toString() {
		StringBuilder result = new StringBuilder("Build ID: " + buildId + "\n" +
		"Assessment Report File: " + assessmentReportFile + "\n");
		if (instanceLocation != null) { result.append(instanceLocation.toString()); }
		return result.toString();
	}
	
}
