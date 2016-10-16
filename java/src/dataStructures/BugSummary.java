package dataStructures;

import java.util.ArrayList;
import java.util.List;

public class BugSummary {
	List<BugCategory> list;
	
	public BugSummary() {
		list = new ArrayList<>();
	}
	public void add(BugCategory c) {
		list.add(c);
	}
	
	public List<BugCategory> getBugCategories() {
		return list;
	}
	
	@Override
	public String toString() {
		StringBuffer sb = new StringBuffer();
		for (BugCategory c : list) {
			sb.append(c);
		}
		return sb.toString();
	}
}
