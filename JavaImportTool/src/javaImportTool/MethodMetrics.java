package javaImportTool;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.ChildListPropertyDescriptor;
import org.eclipse.jdt.core.dom.ChildPropertyDescriptor;
import org.eclipse.jdt.core.dom.MethodDeclaration;

public class MethodMetrics {
	
	static class NodeMetrics
	{
		public int count;
		public int nesting;
		public NodeMetrics(int c, int n) {count = c; nesting = n;}
	}
	
	static void printMethodData(MethodDeclaration method, Node envisionNode)
	{
		String nodeText = method.toString();
		int signatureBegin = nodeText.indexOf("*/");
		if (signatureBegin < 0) signatureBegin = 0;
		else signatureBegin += 3;
		int signatureEnd = nodeText.indexOf('\n', signatureBegin);
		if (signatureEnd < 0 || signatureEnd < signatureBegin + 3)
			throw new RuntimeException("Invalid method: " + nodeText);
		if (nodeText.charAt(signatureEnd-1) != ';') // skip interfaces
		{
			String body = nodeText.substring(signatureEnd+1);
			int bodyLength = body.length() - body.replace("\n", "").length() - 1;
			
			System.out.print("loc=\t"+ bodyLength + "\t" );
			System.out.print("args=\t" + method.parameters().size() + "\t" );
			
			// Various other counters
			showMetric(method, "locals", false, new int[]{ASTNode.VARIABLE_DECLARATION_STATEMENT,
					ASTNode.VARIABLE_DECLARATION_EXPRESSION});
			showMetric(method, "null", false, new int[]{ASTNode.NULL_LITERAL});
			showMetric(method, "assert", false, new int[]{ASTNode.ASSERT_STATEMENT});
			showMetric(method, "return", false, new int[]{ASTNode.RETURN_STATEMENT});
			showMetric(method, "this", false, new int[]{ASTNode.THIS_EXPRESSION});
			showMetric(method, "throw", false, new int[]{ASTNode.THROW_STATEMENT});
			showMetric(method, "cast", true, new int[]{ASTNode.CAST_EXPRESSION});
			showMetric(method, "new", true, new int[]{ASTNode.CLASS_INSTANCE_CREATION, ASTNode.ARRAY_CREATION});
			showMetric(method, "if", true, new int[]{ASTNode.IF_STATEMENT});
			showMetric(method, "loop", true, new int[]{ASTNode.FOR_STATEMENT, ASTNode.WHILE_STATEMENT,
					ASTNode.DO_STATEMENT, ASTNode.ENHANCED_FOR_STATEMENT});
			showMetric(method, "try", true, new int[]{ASTNode.TRY_STATEMENT});
			showMetric(method, "catch", true, new int[] {ASTNode.CATCH_CLAUSE});
			showMetric(method, "type_decl", true, new int[] {ASTNode.TYPE_DECLARATION_STATEMENT});
			showMetric(method, "block", true, new int[] {ASTNode.BLOCK});
			showMetric(method, "switch", true, new int[] {ASTNode.SWITCH_STATEMENT});
			showMetric(method, "case", false, new int[] {ASTNode.SWITCH_CASE});
			showMetric(method, "super", false, new int[] {ASTNode.SUPER_METHOD_INVOCATION,
					ASTNode.SUPER_CONSTRUCTOR_INVOCATION});
			showMetric(method, "anonymous", false, new int[] {ASTNode.ANONYMOUS_CLASS_DECLARATION});
			
			// Print placeholders that will be used in later stages
			System.out.print("use\tcomment\t");
					
			// Print path
			Node pathNode = envisionNode.parent();
			String path = "\t >>>\t ";
			while (pathNode != null)
			{	
				if (pathNode.symbol() != null && !pathNode.symbol().isEmpty())
					path = "." + pathNode.symbol() +  path;
				pathNode = pathNode.parent();
			}
			System.out.print(path);
			
			// Print signature
			String signature = nodeText.substring(signatureBegin, signatureEnd-1);
			int throwsIndex = signature.indexOf(") throws");
			if (throwsIndex >= 0) signature = signature.substring(0,throwsIndex+1);
			System.out.println(signature);
		}		
	}
	
	static void showMetric(ASTNode startingNode, String name, boolean includeNesting, int[] nodeTypes)
	{
		NodeMetrics metrics = computeMetrics(startingNode, nodeTypes);
		if (includeNesting)
			System.out.print(name + "=\t" + metrics.count + "\t" + metrics.nesting + "\t");
		else
			System.out.print(name + "=\t" + metrics.count + "\t");
	}
	
	static NodeMetrics computeMetrics(ASTNode startingNode, int[] nodeTypes)
	{
		int count = 0;
		int nesting = 0;
		
		for (ASTNode child : childrenOfASTNode(startingNode))
			if (child != null)
			{
				NodeMetrics m = computeMetrics(child, nodeTypes);
				count += m.count;
				nesting = Math.max(nesting, m.nesting);
			}
		
		for(int type : nodeTypes)
			if (startingNode.getNodeType() == type)
			{
				count++;
				nesting++;
			}

		return new NodeMetrics(count, nesting);
	}
	
	static List<ASTNode> childrenOfASTNode(ASTNode node)
	{
		List<ASTNode> result = new ArrayList<ASTNode>();
		for ( Object property : node.structuralPropertiesForType())
		{
			if (property instanceof ChildPropertyDescriptor)
				result.add((ASTNode)node.getStructuralProperty((ChildPropertyDescriptor) property ));
			else if (property instanceof ChildListPropertyDescriptor)
				result.addAll((List<ASTNode>)node.getStructuralProperty((ChildListPropertyDescriptor) property ));
		}
		
		return result;
	}
}
