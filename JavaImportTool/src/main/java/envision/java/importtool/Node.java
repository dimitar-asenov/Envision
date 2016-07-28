/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/
package envision.java.importtool;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.Stack;
import java.util.UUID;

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
	private UUID id_;
	private String symbol_ = null; // only set this if this node defines a symbol (e.g. class or method definition)
	private String text_ = null; // only set this if this is a terminal node and contains text (e.g. a literal)
	private SizeEstimator.Size estimatedSize_ = null;
	
	// These are used when writing the nodes to a stream
	private static Stack<String> outputDir_ = new Stack<>();
	private static Stack<PrintStream> out_ = new Stack<>();
	private static OutputFormat format_ = OutputFormat.XML;
	private static String suffix = null;
	
	/**
	 * If true, sort children by label when writing encoding.
	 */
	private final boolean SORT_BY_LABEL = true;
	
	public static boolean FORCE_SINGLE_PERSISTENT_UNIT = false;
	
	/**
	 * This comparator is used to sort children lists by label.
	 * This is to ensure consistency between all methods used to produce Envision encodings.
	 */
	private static Comparator<Node> labelComparator = (Node n1, Node n2) -> {
		try {
			int l1 = Integer.parseInt(n1.name_);
			int l2 = Integer.parseInt(n2.name_);
			return l1 - l2;
		} catch (NumberFormatException e) {
			return n1.name_.compareTo(n2.name_);
		}
	};
	
	private List<Node> children_ = new LinkedList<>();
	
	// Getters and setters
	public Node parent() { return parent_; }
	public String tag() { return tag_; }
	public String name() { return name_; }
	public String symbol() { return symbol_; }
	public int numChildren() { return children_.size(); }
	public List<Node> children() { return children_;}
	public SizeEstimator.Size estimatedSize() throws ConversionException
	{
		if (estimatedSize_ == null) estimatedSize_ = SizeEstimator.estimateSize(this);
		return estimatedSize_;
	}
	
	public void setName(String name) { name_ = name; }
	public void setName(int name) { name_ = Integer.toString(name); }
	public void setDoubleValue(double val) { text_ = "D_" + Double.toString(val); }
	public void setStringValue(String val) { text_ = "S_" + val; }
	public void setLongValue(long val){ text_ = "I_" + Integer.toString((int)val); }
	//TODO: In Envision, implement long integers
	
	public boolean hasValue() { return text_ != null;}
	public String valueAsText() { return text_; }
	
	public boolean isPersistenceUnit()
	{
		return !FORCE_SINGLE_PERSISTENT_UNIT && NodeDescriptors.isPersistenceUnit(this);
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
		id_ = UUID.randomUUID();
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
	
	public Node childOrNull(String name)
	{
		for (Node c : children_)
			if (c.name_.equals(name)) return c;
		
		return null;
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
			if (c.symbol_ != null && c.symbol_.equals(restOfQualifiedName[0])) {
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
	
	public enum OutputFormat {XML, CLIPBOARD, SIMPLE};
	
	// Save as XML to a stream in either file or clipboard format
	public void renderRootTree(String dir, String projectName, OutputFormat format)
		throws ConversionException, FileNotFoundException, UnsupportedEncodingException
	{
		outputDir_.push(dir);
		format_ = format;
		suffix = (format_ == OutputFormat.XML) ? ".xml" : "";
		
		out_.push( new PrintStream(new File(outputDir_.peek() + projectName + suffix), "UTF-8") );
		
		if (format_ == OutputFormat.XML || format_ == OutputFormat.CLIPBOARD)
			out_.peek().println("<!DOCTYPE EnvisionFilePersistence>");
		
		if (format_ == OutputFormat.CLIPBOARD) out_.peek().println("<clipboard>");
		else setName(symbol_);
		renderTree("", false);
		if (format_ == OutputFormat.CLIPBOARD) out_.peek().println("</clipboard>");
	}
	
	
	private void renderTree(String indentation, boolean considerPersistenceUnits)
		throws ConversionException, FileNotFoundException, UnsupportedEncodingException
	{
		if (SORT_BY_LABEL)
			java.util.Collections.sort(children_, labelComparator);
		
		if (considerPersistenceUnits && isPersistenceUnit() && format_ != OutputFormat.CLIPBOARD)
		{
			String relativeDirectoryPath = relativeDirectoryPathForPersistenceUnit();
			String relativeFilePath = relativeDirectoryPath + symbol() + " {" + id_ + "}" + suffix;
			String fullDirectoryPath = outputDir_.peek() + relativeDirectoryPath;
			String fullFilePath = outputDir_.peek() + relativeFilePath;
					
			// Create a new file for this persistence unit
			if (format_ == OutputFormat.XML)
			{
				assert false; // "This code is actually invalid at this point, due to parent id handling");
				out_.peek().print(indentation + "<persistencenewunit name=\"" + name_ + "\">S_{" + id_ + "}");
				out_.peek().println("</persistencenewunit>");
			}
			else if (format_ == OutputFormat.SIMPLE)
			{
				out_.peek().print(indentation + name_ + " persistencenewunit {" + id_ +"}");
				out_.peek().print(parent_ == null ? " {00000000-0000-0000-0000-000000000000}": " {" + parent_.id_ + "}");
				out_.peek().println(". S_" + relativeFilePath );
			}
			
			(new File(fullDirectoryPath)).mkdirs(); // Make the directory if it doesn't exist.
			out_.push( new PrintStream(new File(fullFilePath), "UTF-8") );
			outputDir_.push(fullDirectoryPath);
			if (format_ == OutputFormat.XML) out_.peek().println("<!DOCTYPE EnvisionFilePersistence>");
			renderTree("", false);
			outputDir_.pop();
			out_.pop().close();
		}
		else
		{
			if (!children_.isEmpty() && text_ != null)
				throw new ConversionException("Invalid node content. Node has both children and textual content.");
			
			// This file is not a persistence unit
			if (format_ == OutputFormat.XML || format_ == OutputFormat.CLIPBOARD)
			{
				out_.peek().print(indentation + "<" + tag_);
				if (format_ == OutputFormat.XML)
				{
					out_.peek().print(" id=\"{" + id_ + "}\" parentId=\"");
					assert parent_ != null || !considerPersistenceUnits;
					out_.peek().print(parent_ == null ? "{00000000-0000-0000-0000-000000000000}\"": " {" + parent_.id_ + "}\"");
				}
				out_.peek().print(" name=\"" + name_ + "\"");
				
				if (text_ != null) out_.peek().println(">" + StringEscapeUtils.escapeXml(text_) + "</" + tag_ + ">");
				else if (children_.isEmpty()) out_.peek().println(" />");
				else
				{
					out_.peek().println(">");
					for(Node child : children_)
						child.renderTree(indentation + "\t", true);
					out_.peek().println(indentation + "</" + tag_ + ">");
				}
			}
			else if (format_ == OutputFormat.SIMPLE)
			{
				// Do not output empty lists
				/* Actually do because deleting the single element of a list confuses the merge.
				 * There is a note on this in Envision/misc/version-control.
				if (children_.isEmpty() && (tag_.startsWith("TypedListOf") || tag_.endsWith("List")))
					return;
				*/
				out_.peek().print(indentation + name_ + " " + tag_ + " {" + id_+"}");
				assert parent_ != null || !considerPersistenceUnits;
				out_.peek().print(parent_ == null ? " {00000000-0000-0000-0000-000000000000}": " {" + parent_.id_ + "}");
				if (text_ != null) out_.peek().print(". " + escape(text_));
				out_.peek().println();
				for (Node child : children_)
					child.renderTree(indentation + "\t", true);
			}
		}
	}
	
	String relativeDirectoryPathForPersistenceUnit()
	{
		Node ancestor = parent_;
		while (ancestor != null && ancestor.symbol() == null)
			ancestor = ancestor.parent_;
		
		if (ancestor != null && ancestor.parent_ != null)
			return ancestor.symbol() + "/";
		
		return "";
	}
	
	String escape(String s)
	{
		s = s.replace("\\", "\\\\");
		s = s.replace("\r", "\\r");
		s = s.replace("\n", "\\n");
		return s;
	}
}
