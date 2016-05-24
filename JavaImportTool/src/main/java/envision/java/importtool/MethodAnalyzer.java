package envision.java.importtool;

import java.util.List;

import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.Assignment;
import org.eclipse.jdt.core.dom.IfStatement;
import org.eclipse.jdt.core.dom.InfixExpression;
import org.eclipse.jdt.core.dom.NumberLiteral;
import org.eclipse.jdt.core.dom.PostfixExpression;
import org.eclipse.jdt.core.dom.PrefixExpression;
import org.eclipse.jdt.core.dom.SimpleName;
import org.eclipse.jdt.core.dom.SimpleType;
import org.eclipse.jdt.core.dom.VariableDeclarationExpression;
import org.eclipse.jdt.core.dom.VariableDeclarationStatement;
import org.eclipse.jdt.core.dom.VariableDeclarationFragment;;

public interface MethodAnalyzer {

	public void onMatchingNode(ASTNode node, int nestingLevel);
}

class StringVariableDetector implements MethodAnalyzer {

	public boolean found = false;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		if (node instanceof VariableDeclarationExpression) {
			VariableDeclarationExpression exp = (VariableDeclarationExpression) node;
			if (exp.getType().isSimpleType() && hasAtleastOneDimensionlessFragment(exp.fragments())) {
				SimpleType t = (SimpleType) exp.getType();
				if ( ((SimpleName) t.getName()).getIdentifier().equals("String") )
					found = true;
			}
		}
		else {
			VariableDeclarationStatement st = (VariableDeclarationStatement) node;
			if (st.getType().isSimpleType() && hasAtleastOneDimensionlessFragment(st.fragments())) {
				SimpleType t = (SimpleType) st.getType();
				if (t.getName().isSimpleName())
					if ( ((SimpleName) t.getName()).getIdentifier().equals("String") )
						found = true;
			}
		}
	}
	
	private boolean hasAtleastOneDimensionlessFragment(List<VariableDeclarationFragment> fragments)
	{
		for (VariableDeclarationFragment fragment: fragments) {
			if (fragment.getExtraDimensions() == 0) return true;
		}
		
		return false;
	}
}

class AllVarsInitializedDetector implements MethodAnalyzer {

	public boolean allInitialized = true;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		VariableDeclarationFragment fragment = (VariableDeclarationFragment) node;
		if (fragment.getInitializer() == null)
			allInitialized = false;
	}
}

class SubtractionDetector implements MethodAnalyzer {

	public boolean found = false;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		if (node.getNodeType() == ASTNode.INFIX_EXPRESSION) {
			InfixExpression e = (InfixExpression) node;
			if (e.getOperator() == InfixExpression.Operator.MINUS)
				found = true;
		}
		else if (node.getNodeType() == ASTNode.PREFIX_EXPRESSION) {
			PrefixExpression e = (PrefixExpression) node;
			if (e.getOperator() == PrefixExpression.Operator.DECREMENT
				|| e.getOperator() == PrefixExpression.Operator.MINUS)
				found = true;
		}
		else if (node.getNodeType() == ASTNode.POSTFIX_EXPRESSION) {
			PostfixExpression e = (PostfixExpression) node;
			if (e.getOperator() == PostfixExpression.Operator.DECREMENT)
				found = true;
		}
		else if (node.getNodeType() == ASTNode.ASSIGNMENT) {
			Assignment a = (Assignment) node;
			if (a.getOperator() == Assignment.Operator.MINUS_ASSIGN)
				found = true;
		}
		else {
			NumberLiteral lit = (NumberLiteral) node;
			if (lit.getToken().trim().startsWith("-"))
				found = true;
		}
	}
}

class ElseBranchDetector implements MethodAnalyzer {

	public boolean found = false;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		IfStatement ifs = (IfStatement) node;
		if (ifs.getElseStatement() != null)
			found = true;
	}
}

class LoopWith2CallsDetector implements MethodAnalyzer {

	public boolean found = false;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		MethodMetrics.NodeMetrics metrics = MethodMetrics.computeMetrics(node, new int[]{
				ASTNode.CLASS_INSTANCE_CREATION, ASTNode.METHOD_INVOCATION, ASTNode.SUPER_METHOD_INVOCATION});
		if (metrics.count >= 2)
			found = true;
	}
}

class TopLevelLoopAfterIfDetector implements MethodAnalyzer {

	public boolean found = false;
	private ASTNode foundIf = null;
	
	public void onMatchingNode(ASTNode node, int nestingLevel)
	{
		if (foundIf == null && node.getNodeType() == ASTNode.IF_STATEMENT) foundIf = node;
		
		if (foundIf != null && nestingLevel == 2 && node.getNodeType() != ASTNode.IF_STATEMENT
				&& !isAncestorOfIf(node))
			found = true;
	}
	
	private boolean isAncestorOfIf(ASTNode node) {
		ASTNode ifAncestor = foundIf;
		while (ifAncestor != null) {
			if (ifAncestor == node) return true;
			ifAncestor = ifAncestor.getParent();
		}
		
		return false;
	}
}