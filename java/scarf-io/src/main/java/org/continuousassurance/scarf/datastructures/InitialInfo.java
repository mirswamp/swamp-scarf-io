package org.continuousassurance.scarf.datastructures;

public class InitialInfo {

	private final String assessFramework;
	private final String assessFrameworkVersion; 
	private final String assessmentStartTimeStamp; 
	private final String buildFramework;
	private final String buildFrameworkVersion; 
	private final String buildRootDir;
	private final String packageName;
	private final String packageRootDir; 
	private final String packageVersion; 
	private final String parserFramework; 
	private final String parserFrameworkVersion; 
	private final String platformName;
	private final String toolName;
	private final String toolVersion;
	private final String uuid;

	public InitialInfo(String assessFramework, 
			String assessFrameworkVersion, 
			String assessmentStartTimeStamp, 
			String buildFramework,
			String buildFrameworkVersion, 
			String buildRootDir, 
			String packageName, 
			String packageRootDir, 
			String packageVersion, 
			String parserFramework, 
			String parserFrameworkVersion, 
			String platformName,
			String toolName, 
			String toolVersion, 
			String uuid) {
		this.assessFramework = assessFramework;
		this.assessFrameworkVersion = assessFrameworkVersion; 
		this.assessmentStartTimeStamp = assessmentStartTimeStamp;
		this.buildFramework = buildFramework;
		this.buildFrameworkVersion = buildFrameworkVersion; 
		this.buildRootDir = buildRootDir; 
		this.packageName = packageName; 
		this.packageRootDir = packageRootDir;
		this.packageVersion = packageVersion; 
		this.parserFramework = parserFramework; 
		this.parserFrameworkVersion = parserFrameworkVersion;
		this.platformName = platformName;
		this.toolName = toolName;
		this.toolVersion = toolVersion;
		this.uuid = uuid;
	}
	
	public String getAssessFramework() {
		return assessFramework;
	}

	public String getAssessFrameworkVersion() {
		return assessFrameworkVersion;
	}

	public String getAssessmentStartTimeStamp() {
		return assessmentStartTimeStamp;
	}

	public String getBuildFramework() {
		return buildFramework;
	}

	public String getBuildFrameworkVersion() {
		return buildFrameworkVersion;
	}

	public String getBuildRootDir() {
		return buildRootDir;
	}

	public String getPackageName() {
		return packageName;
	}

	public String getPackageRootDir() {
		return packageRootDir;
	}

	public String getPackageVersion() {
		return packageVersion;
	}

	public String getParserFramework() {
		return parserFramework;
	}

	public String getParserFrameworkVersion() {
		return parserFrameworkVersion;
	}

	public String getPlatformName() {
		return platformName;
	}

	public String getToolName() {
		return toolName;
	}
	
	public String getToolVersion() {
		return toolVersion;
	}
	
	public String getUUID() {
		return uuid;
	}
	
	@Override
	public String toString() {
		StringBuilder result = new StringBuilder("Tool name: " + toolName + "\n" +
		"Tool version: " + toolVersion + "\n" +
		"Tool UUID: " + uuid + "\n");
		return result.toString();
	}
}
