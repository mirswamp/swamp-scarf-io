package dataStructures;

public class Method {
	private String id;
	private String name;
	private boolean isPrimary;
	
	public Method(String id, String name, boolean primary) {
		this.id = id;
		this.name = name;
		this.isPrimary = primary;
	}
	
	public String getID() {
		return id;
	}
	
	public String getName() {
		return name;
	}
	
	public boolean isPrimary() {
		return isPrimary;
	}

}
