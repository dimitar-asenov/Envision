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

public class NodeInitializer {

	public NodeInitializer(String nodeType, String[][] children)
	{
		nodeType_ = nodeType;
		children_ = children;
	}
	
	public NodeInitializer(String nodeType, boolean isPersistenceUnit, String[][] children)
	{
		nodeType_ = nodeType;
		children_ = children;
		isPersistenceUnit_ = isPersistenceUnit;
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
	
	public NodeInitializer(String nodeType, boolean isPersistenceUnit, String baseNodeType, String[][] children)
	{
		nodeType_ = nodeType;
		baseNodeType_ = baseNodeType;
		children_ = children;
		isPersistenceUnit_ = isPersistenceUnit;
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
	
	private boolean isPersistenceUnit_ = false;
	
	public String nodeType() { return nodeType_; }
	public boolean isPersistenceUnit() { return isPersistenceUnit_; }
}
