package dataStructures;

public class BugCategory {
	private int count;
	private int byteCount;
	private String bugCode;
	private String bugGroup;
	
	public BugCategory(int cnt, int bytes, String code, String group) {
		count = cnt;
		byteCount = bytes;
		bugCode = code;
		bugGroup = group;
	}
	
	public int getCount() {
		return count;
	}
	
	public int getByteCount() {
		return byteCount;
	}
	
	public String getBugCode() {
		return bugCode;
	}
	
	public String getBugGroup() {
		return bugGroup;
	}
	
	@Override
	public String toString() {
		String result = "Count: " + count + "\n";
		result += "Byte Count: " + byteCount + "\n";
		result += "Bug Code: " + bugCode + "\n";
		result += "Bug Group: " + bugGroup + "\n";
		return result;
	}
}
