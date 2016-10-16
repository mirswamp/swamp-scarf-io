package dataStructures;

public class InstanceLocation {
	int startLine;
	int endLine;
	String xPath;
	
	public int getStartLine() {
		return startLine;
	}
	
	public void setStartLine(int ln) {
		startLine = ln;
	}
	
	public int getEndLine() {
		return endLine;
	}
	
	public void setEndLine(int ln) {
		endLine = ln;
	}
	
	public String getXPath() {
		return xPath;
	}
	
	public void setXPath(String xpath) {
		xPath = xpath;
	}
	
	@Override
	public String toString() {
		String result = "Start line: " + startLine + "\n";
		result += "End line: " + endLine + "\n";
		result += "XPath: " + xPath + "\n";
		return result;
	}
}
