package org.continuousassurance.scarf.datastructures;

public class Method {
	private final String id;
	private final String name;
	private final boolean is_primary;
	
	public Method(String id, String name, boolean primary) {
		this.id = id;
		this.name = name;
		this.is_primary = primary;
	}
	
	public String getID() {
		return id;
	}
	
	public String getName() {
		return name;
	}
	
	public boolean isPrimary() {
		return is_primary;
	}

}
