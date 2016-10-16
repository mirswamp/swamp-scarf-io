package dataStructures;

public class InitialInfo {
	private String toolName;
	private String toolVersion;
	private String uuid;
	
	public InitialInfo(String toolName, String toolVersion, String uuid) {
		this.toolName = toolName;
		this.toolVersion = toolVersion;
		this.uuid = uuid;
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
		String result = "Tool name: " + toolName + "\n";
		result += "Tool version: " + toolVersion + "\n";
		result += "Tool UUID: " + uuid + "\n";
		return result;
	}
}
