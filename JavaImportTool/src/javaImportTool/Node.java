package javaImportTool;

import java.io.PrintStream;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

import org.apache.commons.lang3.StringEscapeUtils;

/**
 * The Node class represents an Envision model node that will be saved to the generated XML output.
 * 
 * This single class is used to model all corresponding nodes in Envision. Therefore it can represent both leaf nodes
 * and composite nodes such as lists or CompositeNode.
 */
public class Node {
	
	// These are all the properties a node might have
	private Node parent_;
	private String tag_;
	private String name_;
	private long id_;
	private String symbol_ = null; // only set this if this node defines a symbol (e.g. class or method definition)
	private String text_ = null; // only set this if this is a terminal node and contains text (e.g. a literal)
	
	// Each new node is assigned a unique id
	private static long nextId_ = 0;
	
	// These are used when writing the nodes to a stream
	private static PrintStream out_ = null;
	private static boolean generateClipboardFormat_ = false;
	
	private List<Node> children_ = new LinkedList<Node>();
	
	// Getters and setters
	public Node parent() { return parent_; }
	public String tag() { return tag_; }
	public String name() { return name_; }
	public int numChildren() { return children_.size(); }
	
	public void setName(String name) { name_ = name; }
	public void setDoubleValue(double val) { text_ = "D_" + Double.toString(val); }
	public void setStringValue(String val) { text_ = "S_" + StringEscapeUtils.escapeXml(val); }
	public void setLongValue(long val){ text_ = "I_" + Integer.toString((int)val); }
	//TODO: In Envision, implement long integers
	
	public void setSymbol(String symbol) throws ConversionException
	{
		symbol_ = symbol;
		child("name").setStringValue(symbol);
	}
	
	// Constructors
	public Node(Node parent, String tag)
	{
		parent_ = parent;
		tag_ = tag;
		id_ = nextId_++;
		NodeDescriptors.initialize(this, tag_);
	}
	
	public Node(Node parent, String tag, String name)
	{
		this(parent, tag);
		name_ = name;
	}
	
	public Node(Node parent, String tag, int name)
	{
		this(parent, tag, Integer.toString(name));
	}
	
	// Other methods
	public Node add(Node n)
	{
		n.parent_ = this;
		children_.add(n);
		return n;
	}
	
	public Node child(String name) throws ConversionException
	{
		for (Node c : children_)
		{
			if (c.name_.equals(name)) return c;
		}
		
		throw new ConversionException("Could not find child '" + name + "' in a node of type '" + tag_ + "'");
	}
	
	public Node setChild(String name, Node newNode) throws ConversionException
	{
		ListIterator<Node> it = children_.listIterator();
		while(it.hasNext())
		{
			Node n = it.next();
			if (n.name_.equals(name))
			{
				it.set(newNode);
				newNode.parent_ = this;
				newNode.setName(name);
				return newNode;
			}
		}
		
		throw new ConversionException("Could not find child '" + name + "' in a node of type '" + tag_ + "'");
	}
	
	public Node getModuleContext(String restOfQualifiedName) throws ConversionException
	{
		return getModuleContext(restOfQualifiedName.split("\\."));
	}
	
	public Node getModuleContext(String[] restOfQualifiedName) throws ConversionException
	{
		if (restOfQualifiedName.length == 0 )
			return this;
		
		String[] remaining = new String[restOfQualifiedName.length-1];
		for (int i = 0; i <remaining.length; ++i) remaining[i] = restOfQualifiedName[i+1];
		
		for (Node c : child("modules").children_)
		{
			if (c.symbol_ != null)
			{
				if (c.symbol_.equals(restOfQualifiedName[0]))
					return c.getModuleContext(remaining);
			}
		}
		
		return addSymbolNodeInList("modules", "Module", restOfQualifiedName[0]).getModuleContext(remaining);
	}
	
	public Node addSymbolNodeInList(String listName, String tag, String symbolName) throws ConversionException
	{
		for(Node child : children_)
		{
			if (child.name_.equals(listName))
			{
				Node newNode = child.add(new Node(child, tag, child.children_.size()));
				newNode.setSymbol(symbolName);
				return newNode;
			}
		}
		
		throw new ConversionException("Symbol list '" + listName + "' is unknown in node of type '" + tag + "'");
	}
	
	// Save as XML to a stream in either file or clipboard format
	public void renderTree(PrintStream out, boolean generateClipboardFormat) throws ConversionException
	{
		generateClipboardFormat_ = generateClipboardFormat;
		out_ = out;
		
		out_.println("<!DOCTYPE EnvisionFilePersistence>");
		if (generateClipboardFormat_) out_.println("<clipboard>");
		else
		{
			setName(symbol_);
			out_.println("<model nextid=\"" + nextId_ + "\">");
		}
		renderTree("\t");
		if (generateClipboardFormat_) out_.println("</clipboard>");
		else  out_.println("</model>");
	}
	
	private void renderTree(String indentation) throws ConversionException
	{	
		// The print below omits the closing angular brace since it might not be needed.
		out_.print(indentation + "<" + tag_);
		if (!generateClipboardFormat_) out_.print(" id=\"" + id_ + "\"");
		out_.print(" name=\"" + name_ + "\" partial=\"0\"");
		
		if (!children_.isEmpty() && text_ != null)
			throw new ConversionException("Invalid node condent. Node has both children and textual content.");
		
		if (text_ != null) out_.println(">" + text_ + "</" + tag_ + ">");
		else if (children_.isEmpty()) out_.println(" />");
		else
		{
			out_.println(">");
			for(Node child : children_)
				child.renderTree(indentation + "\t");
			out_.println(indentation + "</" + tag_ + ">");
		}
	}
}
