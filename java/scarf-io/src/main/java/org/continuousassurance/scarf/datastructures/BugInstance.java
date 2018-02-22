package org.continuousassurance.scarf.datastructures;

import java.util.ArrayList;
import java.util.List;

public class BugInstance {
	private final int bugId;
	private final List<Integer> cweIds;
	private final List<Method> methods;
	private final List<Location> locations;
	private InstanceLocation instanceLoc;
	private String className;
	private String bugSeverity;
	private String bugRank;
	private String resolutionSuggestion;
	private String bugMessage;
	private String bugCode;
	private String bugGroup;
	private String assessmentReportFile;
	private String buildId;
	private BugTrace bugTrace;
	
	public BugInstance(final int id) {
		bugId = id;
		cweIds = new ArrayList<>();
		methods = new ArrayList<>();
		locations = new ArrayList<>();
	}
	
	public int getBugId() {
		return bugId;
	}
	
	public void addCWE(final int id) {
		cweIds.add(id);
	}
	
	public List<Integer> getCWEIDs() {
		return new ArrayList<Integer>(cweIds);
	}
	
	public void addMethod(final Method m) {
		methods.add(m);
	}
	
	public void addMethods (final List<Method> list) {
		methods.addAll(list);
	}
	
	public List<Method> getMethods() {
		return new ArrayList<Method>(methods);
	}
	
	public void addLocation(final Location l) {
		locations.add(l);
	}
	
	public void addLocations(final List<Location> list) {
		locations.addAll(list);
	}
	
	public List<Location> getLocations() {
		return new ArrayList<Location>(locations);
	}
	
	public void setInstanceLocation(final InstanceLocation i) {
		instanceLoc = i;
	}
	
	public InstanceLocation getInstanceLocation() {
		return instanceLoc;
	}
	
	public void setClassName(final String name) {
		className = name;
	}
	
	public String getClassName() {
		return className;
	}
	
	public void setBugSeverity(final String severity) {
		bugSeverity = severity;
	}
	
	public String getBugSeverity() {
		return bugSeverity;
	}
	
	public void setBugRank(final String rank) {
		bugRank = rank;
	}
	
	public String getBugRank() {
		return bugRank;
	}
	
	public void setResolutionSuggestion(final String sugg) {
		resolutionSuggestion = sugg;
	}
	
	public String getResolutionSuggestion() {
		return resolutionSuggestion;
	}
	
	public void setBugMessage(final String msg) {
		bugMessage = msg;
	}
	
	public String getBugMessage() {
		return bugMessage;
	}
	
	public void setBugCode(final String code) {
		bugCode = code;
	}
	
	public String getBugCode() {
		return bugCode;
	}
	
	public void setBugGroup(final String grp) {
		bugGroup = grp;
	}
	
	public String getBugGroup() {
		return bugGroup;
	}
	
	public void setAssessmentReportFile(final String file) {
		assessmentReportFile = file;
	}
	
	public String getAssessmentReportFile() {
		return assessmentReportFile;
	}
	
	public void setBuildId(final String id) {
		buildId = id;
	}
	
	public String getBuildId() {
		return buildId;
	}
	
	public void setBugTrace(final BugTrace b) {
		bugTrace = b;
	}
	
	public BugTrace getBugTrace() {
		return bugTrace;
	}
	
	@Override
	public String toString() {
		return "Bug id: " + bugId + "\n" + 
		"Class name: " + className + "\n" + 
		"Bug severity: " + bugSeverity + "\n" + 
		"Bug rank: " + bugRank + "\n" + 
		"Resolution suggestion: " + resolutionSuggestion + "\n" + 
		"Bug message: " + bugMessage + "\n" + 
		"Bug code: " + bugCode + "\n" + 
		"Bug group: " + bugGroup + "\n" + 
		"Assessment report file: " + assessmentReportFile + "\n" + 
		"Build ID: " + buildId + "\n" + 
		"CWE Ids: " + cweIds + "\n" +
		"Bug Trace: " + bugTrace.toString() + "\n";
	}
}

