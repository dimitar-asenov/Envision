package javaImportTool;

import java.util.HashMap;
import java.util.Map;

/**
 * The NodeDescriptors class contains a description of all Envision nodes and their possible child nodes.
 * 
 * This class is used to create all children of a node when that node is newly created. The initializers should not
 * include attributes which are not required.
 */
public class NodeDescriptors {
	
	public static void initialize(Node node)
	{
		String[][] children = initializerMap.get(node.tag());
		if (children != null)
			for (String[] childAttributes : children)
			{
				node.add(new Node(node, childAttributes[0], childAttributes[1]));
			}
	}

	private static Map<String, String[][]> initializerMap = new HashMap<String, String[][]> ();
	
	static {
		
		// Declarations
		initializerMap.put("Project", new String[][]{
				{"Text","name"},
				{"TypedListOfProject","projects"},
				{"TypedListOfModule","modules"},
				{"TypedListOfClass","classes"},
				{"TypedListOfMethod","methods"},
				{"StatementItemList","annotations"},
				{"TypedListOfField","fields"},
				{"Visibility","visibility"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("Module", new String[][]{
				{"Text","name"},
				{"TypedListOfModule","modules"},
				{"TypedListOfClass","classes"},
				{"TypedListOfMethod","methods"},
				{"StatementItemList","annotations"},
				{"TypedListOfField","fields"},
				{"Visibility","visibility"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("Class", new String[][]{
				{"Text","name"},
				{"TypedListOfExpression","baseClasses"},
				{"TypedListOfEnumerator","enumerators"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfField","fields"},
				{"TypedListOfClass","classes"},
				{"TypedListOfMethod","methods"},
				{"TypedListOfExpression","friends"},
				{"Visibility","visibility"},
				{"StatementItemList","annotations"},
				{"Integer","cKind"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("Method", new String[][]{
				{"Text","name"},
				{"StatementItemList","items"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfFormalArgument","arguments"},
				{"TypedListOfFormalResult","results"},
				{"Visibility","visibility"},
				{"StorageSpecifier","storageSpecifier"},
				{"StatementItemList","annotations"},
				{"Integer","mthKind"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("Field", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"},
				{"Visibility","visibility"},
				{"StatementItemList","annotations"},
				{"StorageSpecifier","storageSpecifier"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("VariableDeclaration", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"},
				{"Visibility","visibility"},
				{"StatementItemList","annotations"},
				{"StorageSpecifier","storageSpecifier"},
				{"TypedListOfDeclaration","subDeclarations"}});
		initializerMap.put("NameImport", new String[][]{
				{"Text","name"},
				{"Visibility","visibility"},
				{"StatementItemList","annotations"},
				{"TypedListOfDeclaration","subDeclarations"},
				{"ReferenceExpression","importedName"}});
		
		// Elements
		initializerMap.put("FormalTypeArgument", new String[][]{
				{"Text","name"}});
		initializerMap.put("FormalArgument", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"},
				{"Integer","directionInt"}});
		initializerMap.put("FormalResult", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"}});
		
		// Statements
		initializerMap.put("Block", new String[][]{
				{"StatementItemList","items"}});
		initializerMap.put("ExpressionStatement", new String[][]{
				{"Expression","expression"}});
		initializerMap.put("LoopStatement", new String[][]{
				{"StatementItemList","body"}});
		initializerMap.put("ForEachStatement", new String[][]{
				{"Text","varName"},
				{"Expression","collection"},
				{"StatementItemList","body"}});
		initializerMap.put("IfStatement", new String[][]{
				{"Expression","condition"},
				{"StatementItemList","thenBranch"},
				{"StatementItemList","elseBranch"}});
		initializerMap.put("ReturnStatement", new String[][]{
				{"TypedListOfExpression","values"}});
		initializerMap.put("SwitchCase", new String[][]{
				{"Expression","expr"},
				{"StatementItemList","statement"}});
		initializerMap.put("SwitchStatement", new String[][]{
				{"Expression","switchVar"},
				{"TypedListOfSwitchCase","cases"}});
		initializerMap.put("TryCatchFinallyStatement", new String[][]{
				{"StatementItemList","tryBody"},
				{"TypedListOfCatchClause","catchClauses"},
				{"StatementItemList","finallyBody"}});
		initializerMap.put("CatchClause", new String[][]{
				{"Expression","exceptionToCatch"},
				{"StatementItemList","body"}});
		
		// Expressions
		initializerMap.put("ReferenceExpression", new String[][]{
				{"OOReference","ref"},
				{"TypedListOfExpression","typeArguments"}});
		initializerMap.put("CommaExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"}});
		initializerMap.put("ThrowExpression", new String[][]{
				{"Expression","expr"}});
		initializerMap.put("VariableDeclarationExpression", new String[][]{
				{"VariableDeclaration","decl"}});
		initializerMap.put("BinaryOperation", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}});
		initializerMap.put("UnaryOperation", new String[][]{
				{"Expression","operand"},
				{"Integer","opr"}});
		initializerMap.put("NewExpression", new String[][]{
				{"Expression","newType"}});
		initializerMap.put("ArrayInitializer", new String[][]{
				{"TypedListOfExpression","values"}});
		initializerMap.put("AssignmentExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}});
		initializerMap.put("CastExpression", new String[][]{
				{"Expression","castType"},
				{"Expression","expr"},
				{"Integer","cKind"}});
		initializerMap.put("MethodCallExpression", new String[][]{
				{"ReferenceExpression","ref"},
				{"TypedListOfExpression","arguments"}});
		initializerMap.put("ConditionalExpression", new String[][]{
				{"Expression","condition"},
				{"Expression","trueExpression"},
				{"Expression","falseExpression"}});

		// Literals
		initializerMap.put("BooleanLiteral", new String[][]{
				{"Boolean","value"}});
		initializerMap.put("CharacterLiteral", new String[][]{
				{"Character","value"}});
		initializerMap.put("IntegerLiteral", new String[][]{
				{"Integer","value"}});
		initializerMap.put("FloatLiteral", new String[][]{
				{"Float","value"}});
		initializerMap.put("StringLiteral", new String[][]{
				{"Text","value"}});
		
		// Type expressions
		initializerMap.put("PrimitiveTypeExpression", new String[][]{
				{"Integer","val"}});
		initializerMap.put("ClassTypeExpression", new String[][]{
				{"ReferenceExpression","typeExpression"}});
		initializerMap.put("ArrayTypeExpression", new String[][]{
				{"Expression","typeExpression"}});
	}
}