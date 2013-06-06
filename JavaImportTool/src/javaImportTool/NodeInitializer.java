package javaImportTool;

public class NodeInitializer {

	public NodeInitializer(String nodeType, String[][] children)
	{
		nodeType_ = nodeType;
		children_ = children;
	}
	
	public NodeInitializer(String nodeType, long initialValue)
	{
		nodeType_ = nodeType;
		initialLongValue_ = new Long(initialValue);
	}
	
	public NodeInitializer(String nodeType, double initialValue)
	{
		nodeType_ = nodeType;
		initialDoubleValue_ = new Double(initialValue);
	}
	
	public NodeInitializer(String nodeType, String initialValue)
	{
		nodeType_ = nodeType;
		initialStringValue_ = initialValue;
	}
	
	public NodeInitializer(String nodeType, String baseNodeType, String[][] children)
	{
		nodeType_ = nodeType;
		baseNodeType_ = baseNodeType;
		children_ = children;
	}
	
	public void initialize(Node node)
	{
		if (baseNodeType_ != null) NodeDescriptors.initialize(node, baseNodeType_);
			
		if (children_ != null)
			for (String[] childAttributes : children_)
				node.add(new Node(node, childAttributes[0], childAttributes[1]));
		
		if (initialDoubleValue_ != null) node.setDoubleValue(initialDoubleValue_);
		if (initialLongValue_ != null) node.setLongValue(initialLongValue_);
		if (initialStringValue_ != null) node.setStringValue(initialStringValue_);
	}
	
	private String nodeType_;
	private String baseNodeType_;
	private String[][] children_;
	
	private Double initialDoubleValue_;
	private Long initialLongValue_;
	private String initialStringValue_;
	
	public String nodeType() { return nodeType_; }
}
