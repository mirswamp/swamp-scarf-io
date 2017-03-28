package dataStructures;

public class Location {
	private String id;
	private boolean primary;
	private int startLine;
	private int endLine;
	private int startColumn;
	private int endColumn;
	private String explanation;
	private String sourceFile;
	
	public Location(String id, boolean isPrimary) {
		this.id = id;
		primary = isPrimary;
		startLine = -1;
		endLine = -1;
		startColumn = 0;
		endColumn = 0;
		explanation = "";
		sourceFile = "";
	}
	
	public String getID() {
		return id;
	}
	
	public boolean isPrimary() {
		return primary;
	}
	
	public void setStartLine(int ln) {
		startLine = ln;
	}
	
	public int getStartLine() {
		return startLine;
	}
	
	public void setEndLine(int ln) {
		endLine = ln;
	}
	
	public int getEndLine() {
		return endLine;
	}
	
	public void setStartColumn(int col) {
		startColumn = col;
	}
	
	public int getStartColumn() {
		return startColumn;
	}
	
	public void setEndColumn(int col) {
		endColumn = col;
	}
	
	public int getEndColumn() {
		return endColumn;
	}
	
	public void setExplanation(String exp) {
		explanation = exp;
	}
	
	public String getExplanation() {
		return explanation;
	}
	
	public void setSourceFile(String filepath) {
		sourceFile = filepath;
	}
	
	public String getSourceFile() {
		return sourceFile;
	}
	
	
	
}
