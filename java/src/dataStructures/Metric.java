package dataStructures;

public class Metric {
	private Location location;
	private String className;
	private String methodName;
	private String metricType;
	private String metricValue;
	
	public void setLocation(Location l) {
		location = l;
	}
	
	public Location getLocation() {
		return location;
	}
	
	public void setClassName(String name) {
		className = name;
	}
	
	public String getClassName() {
		return className;
	}
	
	public void setMethodName(String name) {
		methodName = name;
	}
	
	public String getMethodName() {
		return methodName;
	}
	
	public void setMetricType(String type) {
		metricType = type;
	}
	public String getMetricType() {
		return metricType;
	}
	
	public void setMetricValue(String val) {
		metricValue = val;
	}
	
	public String getMetricValue() {
		return metricValue;
	}
	
	@Override 
	public String toString() {
		String result = "Location: " + location + "\n";
		result += "Class Name: " + className + "\n";
		result += "Method Name: " + methodName + "\n";
		result += "Metric Type: " + metricType + "\n";
		result += "Metric Value: " + metricValue + "\n";
		return result;
	}

}
