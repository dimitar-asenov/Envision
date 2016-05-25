package envision.java.importtool;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.ChildListPropertyDescriptor;
import org.eclipse.jdt.core.dom.ChildPropertyDescriptor;
import org.eclipse.jdt.core.dom.MethodDeclaration;
import org.eclipse.jdt.core.dom.PrimitiveType;
import org.eclipse.jdt.core.dom.SingleVariableDeclaration;

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
			
			// Print the answers to all questions from the user stury on comprehension of method structure
			System.out.print(question1(method));
			System.out.print(question2(method));
			System.out.print(question3(method));
			System.out.print(question4(method));
			System.out.print(question5(method));
			System.out.print(question6(method));
			System.out.print(question7(method));
			System.out.print(question8(method));
			System.out.print(question9(method));
			System.out.print(question10(method));
			System.out.print(question11(method));
			System.out.print(question12(method));
			System.out.print(question13(method));
			System.out.print(question14(method));
			System.out.print(question15(method));
			System.out.print('\t');
			
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
	
	static void showMetric(ASTNode node, String name, boolean includeNesting, int[] nodeTypes)
	{
		NodeMetrics metrics = computeMetrics(node, nodeTypes);
		if (includeNesting)
			System.out.print(name + "=\t" + metrics.count + "\t" + metrics.nesting + "\t");
		else
			System.out.print(name + "=\t" + metrics.count + "\t");
	}
	
	static NodeMetrics computeMetrics(ASTNode node, int[] nodeTypes)
	{
		return analyzeMethod(node, nodeTypes, 0, null);
	}
	
	static NodeMetrics analyzeMethod(ASTNode node, int[] nodeTypes, int nestingLevel,
			MethodAnalyzer analyzer)
	{
		int count = 0;
		int nesting = 0;
		
		for (ASTNode child : childrenOfASTNode(node))
			if (child != null)
			{
				NodeMetrics m = analyzeMethod(child, nodeTypes, nestingLevel+1, analyzer);
				count += m.count;
				nesting = Math.max(nesting, m.nesting);
			}
		
		for(int type : nodeTypes)
			if (node.getNodeType() == type)
			{
				count++;
				nesting++;
				if (analyzer != null) analyzer.onMatchingNode(node, nestingLevel);
			}

		return new NodeMetrics(count, nesting);
	}
	
	static List<ASTNode> childrenOfASTNode(ASTNode node)
	{
		List<ASTNode> result = new ArrayList<>();
		for ( Object property : node.structuralPropertiesForType())
		{
			if (property instanceof ChildPropertyDescriptor)
				result.add((ASTNode)node.getStructuralProperty((ChildPropertyDescriptor) property ));
			else if (property instanceof ChildListPropertyDescriptor)
				result.addAll((List<ASTNode>)node.getStructuralProperty((ChildListPropertyDescriptor) property ));
		}
		
		return result;
	}
	
	static String question1(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.THROW_STATEMENT});
		return "\tq01=" + (metrics.count > 0 ? "yes" : "no");
	}
	
	static String question2(MethodDeclaration method)
	{
		AllVarsInitializedDetector detector = new AllVarsInitializedDetector();
		analyzeMethod(method, new int[]{ASTNode.VARIABLE_DECLARATION_FRAGMENT}, 0, detector);
		return "\tq02=" + (detector.allInitialized ? "yes" : "no");
	}
	
	static String question3(MethodDeclaration method)
	{
		SubtractionDetector detector = new SubtractionDetector();
		analyzeMethod(method, new int[]{ASTNode.INFIX_EXPRESSION, ASTNode.PREFIX_EXPRESSION,
				ASTNode.POSTFIX_EXPRESSION, ASTNode.ASSIGNMENT, ASTNode.NUMBER_LITERAL}, 0, detector);
		return "\tq03=" + (detector.found ? "yes" : "no");
	}
	
	static String question4(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.THIS_EXPRESSION});
		return "\tq04=" + (metrics.count > 0 ? "yes" : "no");
	}
	
	static String question5(MethodDeclaration method)
	{
		StringVariableDetector detector = new StringVariableDetector();
		analyzeMethod(method, new int[]{ASTNode.VARIABLE_DECLARATION_EXPRESSION,
				ASTNode.VARIABLE_DECLARATION_STATEMENT}, 0, detector);
		return "\tq05=" + (detector.found ? "yes" : "no");
	}
	
	static String question6(MethodDeclaration method)
	{
		ElseBranchDetector detector = new ElseBranchDetector();
		analyzeMethod(method, new int[]{ASTNode.IF_STATEMENT}, 0, detector);
		return "\tq06=" + (detector.found ? "yes" : "no");
	}
	
	static String question7(MethodDeclaration method)
	{
		if (method.parameters().size() >= 2) {
			SingleVariableDeclaration param = (SingleVariableDeclaration) method.parameters().get(1);
			if (param.getExtraDimensions() == 0 && param.getType().isPrimitiveType()) {
				PrimitiveType pt = (PrimitiveType) param.getType();
				if (pt.getPrimitiveTypeCode() == PrimitiveType.INT)
					return "\tq07=yes"; 
			}
		}
		return "\tq07=no";
	}
	
	static String question8(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.FOR_STATEMENT, ASTNode.WHILE_STATEMENT,
				ASTNode.DO_STATEMENT, ASTNode.ENHANCED_FOR_STATEMENT});
		return "\tq08=" + (metrics.nesting > 1 ? "yes" : "no");
	}
	
	static String question9(MethodDeclaration method)
	{
		return "\tq09=" + (method.parameters().size() == 3 ? "yes" : "no");
	}
	
	static String question10(MethodDeclaration method)
	{
		LoopWith2CallsDetector detector = new LoopWith2CallsDetector();
		analyzeMethod(method, new int[]{ASTNode.FOR_STATEMENT, ASTNode.WHILE_STATEMENT,
				ASTNode.DO_STATEMENT, ASTNode.ENHANCED_FOR_STATEMENT}, 0, detector);
		return "\tq10=" + (detector.found ? "yes" : "no");
	}
	
	static String question11(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.CLASS_INSTANCE_CREATION, ASTNode.ARRAY_CREATION});
		return "\tq11=" + (metrics.count > 0 ? "yes" : "no");
	}
	
	static String question12(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.CATCH_CLAUSE});
		return "\tq12=" + (metrics.count > 0 ? "yes" : "no");
	}
	
	static String question13(MethodDeclaration method)
	{
		TopLevelLoopAfterIfDetector detector = new TopLevelLoopAfterIfDetector();
		analyzeMethod(method, new int[]{ASTNode.IF_STATEMENT, ASTNode.FOR_STATEMENT, ASTNode.WHILE_STATEMENT,
				ASTNode.DO_STATEMENT, ASTNode.ENHANCED_FOR_STATEMENT}, 0, detector);
		return "\tq13=" + (detector.found ? "yes" : "no");
	}
	
	static String question14(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.RETURN_STATEMENT});
		if (metrics.count == 0) return "\tq14=no";
		if (metrics.count > 1) return "\tq14=yes";
		return "\tq14=maybe";
	}
	
	static String question15(MethodDeclaration method)
	{
		NodeMetrics metrics = computeMetrics(method, new int[]{ASTNode.CAST_EXPRESSION});
		return "\tq15=" + (metrics.count > 0 ? "yes" : "no");
	}
}
