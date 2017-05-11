package org.continuousassurance.scarf.datastructures;

import java.util.ArrayList;
import java.util.List;

public class BugSummary {
	private final List<BugCategory> list;
	
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
		StringBuilder sb = new StringBuilder();
		for (BugCategory c : list) {
			sb.append(c);
		}
		return sb.toString();
	}
}
