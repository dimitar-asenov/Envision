package javaImportTool;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.Stack;

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
	private static String outputDir_ = null;
	private static Stack<PrintStream> out_ = new Stack<PrintStream>();
	private static boolean generateClipboardFormat_ = false;
	
	private List<Node> children_ = new LinkedList<Node>();
	
	// Getters and setters
	public Node parent() { return parent_; }
	public String tag() { return tag_; }
	public String name() { return name_; }
	public int numChildren() { return children_.size(); }
	
	public void setName(String name) { name_ = name; }
	public void setName(int name) { name_ = Integer.toString(name); }
	public void setDoubleValue(double val) { text_ = "D_" + Double.toString(val); }
	public void setStringValue(String val) { text_ = "S_" + StringEscapeUtils.escapeXml(val); }
	public void setLongValue(long val){ text_ = "I_" + Integer.toString((int)val); }
	//TODO: In Envision, implement long integers
	
	public boolean isPersistenceUnit()
	{
		return NodeDescriptors.isPersistenceUnit(this);
	}
	
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
	public void renderRootTree(String dir, String projectName, boolean generateClipboardFormat)
		throws ConversionException, FileNotFoundException, UnsupportedEncodingException
	{
		outputDir_ = dir;
		generateClipboardFormat_ = generateClipboardFormat;
		
		out_.push( new PrintStream(new File(outputDir_ + projectName), "UTF-8") );
		
		out_.peek().println("<!DOCTYPE EnvisionFilePersistence>");
		if (generateClipboardFormat_) out_.peek().println("<clipboard>");
		else
		{
			setName(symbol_);
			out_.peek().println("<model nextid=\"" + nextId_ + "\">");
		}
		renderTree("\t", false);
		if (generateClipboardFormat_) out_.peek().println("</clipboard>");
		else  out_.peek().println("</model>");
	}
	
	
	private void renderTree(String indentation, boolean considerPersistenceUnits)
		throws ConversionException, FileNotFoundException, UnsupportedEncodingException
	{	
		if (considerPersistenceUnits && isPersistenceUnit() && !generateClipboardFormat_)
		{
			// Create a new file for this persistence unit
			out_.peek().println(indentation + "<persistencenewunit name=\""
					+ name_ + "\">S_" + id_ + "</persistencenewunit>");
			
			out_.push( new PrintStream(new File(outputDir_ + id_), "UTF-8") );
			out_.peek().println("<!DOCTYPE EnvisionFilePersistence>");
			renderTree("", false);
			out_.pop().close();
		}
		else
		{
			// This file is not a persistence unit
			out_.peek().print(indentation + "<" + tag_);
			if (!generateClipboardFormat_) out_.peek().print(" id=\"" + id_ + "\"");
			out_.peek().print(" name=\"" + name_ + "\" partial=\"0\"");
			
			if (!children_.isEmpty() && text_ != null)
				throw new ConversionException("Invalid node condent. Node has both children and textual content.");
			
			if (text_ != null) out_.peek().println(">" + text_ + "</" + tag_ + ">");
			else if (children_.isEmpty()) out_.peek().println(" />");
			else
			{
				out_.peek().println(">");
				for(Node child : children_)
					child.renderTree(indentation + "\t", true);
				out_.peek().println(indentation + "</" + tag_ + ">");
			}
		}
	}
}
