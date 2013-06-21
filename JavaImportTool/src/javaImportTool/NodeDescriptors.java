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
	
	public static void initialize(Node node, String initializeAsType)
	{
		NodeInitializer initializer = initializerMap.get(initializeAsType);
		if (initializer != null) initializer.initialize(node);
		else
		{
			assert initializeAsType.startsWith("TypedListOf");
		}
	}

	private static Map<String, NodeInitializer> initializerMap = new HashMap<String, NodeInitializer> ();
	
	private static void add(NodeInitializer ni)
	{
		initializerMap.put(ni.nodeType(), ni);
	}
	
	static {
		
		// Declarations
		add( new NodeInitializer("Declaration", new String[][] {
				{"Text","name"},
				{"Modifier","modifiers"},
				{"StatementItemList","annotations"},
				{"TypedListOfDeclaration","subDeclarations"}
		}));
		add( new NodeInitializer("-class-level-container", "Declaration", new String[][] {
				{"TypedListOfClass","classes"},
				{"TypedListOfMethod","methods"},
				{"TypedListOfField","fields"},
		}));
		add( new NodeInitializer("Project", "-class-level-container", new String[][]{
				{"TypedListOfProject","projects"},
				{"TypedListOfModule","modules"}
		}));
		
		add( new NodeInitializer("Module", "-class-level-container", new String[][]{
				{"TypedListOfModule","modules"}
		}));
		add( new NodeInitializer("Class", "-class-level-container", new String[][]{
				{"TypedListOfExpression","baseClasses"},
				{"TypedListOfEnumerator","enumerators"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfExpression","friends"},
				{"Integer","cKind"}
		}));
		add( new NodeInitializer("Method", "Declaration", new String[][]{
				{"StatementItemList","items"},
				{"TypedListOfFormalTypeArgument","typeArguments"},
				{"TypedListOfFormalArgument","arguments"},
				{"TypedListOfFormalResult","results"},
				{"TypedListOfMemberInitializer","memberInitializers"},
				{"Integer","mthKind"}
		}));
		add( new NodeInitializer("VariableDeclaration", "Declaration", new String[][]{
				{"Expression","typeExpression"}
		}));
		add( new NodeInitializer("Field", "VariableDeclaration", null));
		add( new NodeInitializer("NameImport", "Declaration", new String[][]{
				{"ReferenceExpression","importedName"}
		}));
		
		// Elements
		add( new NodeInitializer("FormalTypeArgument", new String[][]{
				{"Text","name"}
		}));
		add( new NodeInitializer("FormalArgument", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"},
				{"Integer","directionInt"}
		}));
		add( new NodeInitializer("FormalResult", new String[][]{
				{"Text","name"},
				{"Expression","typeExpression"}
		}));
		add( new NodeInitializer("Modifier", "Integer", null));
		
		// Statements
		add( new NodeInitializer("Block", new String[][]{
				{"StatementItemList","items"}
		}));
		add( new NodeInitializer("ExpressionStatement", new String[][]{
				{"Expression","expression"}
		}));
		add( new NodeInitializer("LoopStatement", new String[][]{
				{"StatementItemList","body"},
				{"Integer","lpKind"}
		}));
		add( new NodeInitializer("ForEachStatement", new String[][]{
				{"Text","varName"},
				{"Expression","collection"},
				{"StatementItemList","body"}
		}));
		add( new NodeInitializer("IfStatement", new String[][]{
				{"Expression","condition"},
				{"StatementItemList","thenBranch"},
				{"StatementItemList","elseBranch"}
		}));
		add( new NodeInitializer("ReturnStatement", new String[][]{
				{"TypedListOfExpression","values"}
		}));
		add( new NodeInitializer("SwitchCase", new String[][]{
				{"Expression","expr"},
				{"StatementItemList","statement"}
		}));
		add( new NodeInitializer("SwitchStatement", new String[][]{
				{"Expression","switchVar"},
				{"TypedListOfSwitchCase","cases"}
		}));
		add( new NodeInitializer("TryCatchFinallyStatement", new String[][]{
				{"StatementItemList","tryBody"},
				{"TypedListOfCatchClause","catchClauses"},
				{"StatementItemList","finallyBody"}
		}));
		add( new NodeInitializer("CatchClause", new String[][]{
				{"Expression","exceptionToCatch"},
				{"StatementItemList","body"}
		}));
		
		// Expressions
		add( new NodeInitializer("ReferenceExpression", new String[][]{
				{"OOReference","ref"},
				{"TypedListOfExpression","typeArguments"}
		}));
		add( new NodeInitializer("CommaExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"}
		}));
		add( new NodeInitializer("ThrowExpression", new String[][]{
				{"Expression","expr"}
		}));
		add( new NodeInitializer("VariableDeclarationExpression", new String[][]{
				{"VariableDeclaration","decl"}
		}));
		add( new NodeInitializer("BinaryOperation", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("UnaryOperation", new String[][]{
				{"Expression","operand"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("NewExpression", new String[][]{
				{"Expression","newType"}
		}));
		add( new NodeInitializer("ArrayInitializer", new String[][]{
				{"TypedListOfExpression","values"}
		}));
		add( new NodeInitializer("AssignmentExpression", new String[][]{
				{"Expression","left"},
				{"Expression","right"},
				{"Integer","opr"}
		}));
		add( new NodeInitializer("CastExpression", new String[][]{
				{"Expression","castType"},
				{"Expression","expr"},
				{"Integer","cKind"}
		}));
		add( new NodeInitializer("MethodCallExpression", new String[][]{
				{"Expression","callee"},
				{"TypedListOfExpression","arguments"}
		}));
		add( new NodeInitializer("ConditionalExpression", new String[][]{
				{"Expression","condition"},
				{"Expression","trueExpression"},
				{"Expression","falseExpression"}
		}));

		// Literals
		add( new NodeInitializer("BooleanLiteral", new String[][]{
				{"Boolean","value"}
		}));
		add( new NodeInitializer("CharacterLiteral", new String[][]{
				{"Character","value"}
		}));
		add( new NodeInitializer("IntegerLiteral", new String[][]{
				{"Integer","value"}
		}));
		add( new NodeInitializer("FloatLiteral", new String[][]{
				{"Float","value"}
		}));
		add( new NodeInitializer("StringLiteral", new String[][]{
				{"Text","value"}
		}));
		
		// Type expressions
		add( new NodeInitializer("PrimitiveTypeExpression", new String[][]{
				{"Integer","val"}
		}));
		add( new NodeInitializer("ClassTypeExpression", new String[][]{
				{"ReferenceExpression","typeExpression"}
		}));
		add( new NodeInitializer("ArrayTypeExpression", new String[][]{
				{"Expression","typeExpression"}
		}));
		
		// Primitive types
		add( new NodeInitializer("Boolean", 0));
		add( new NodeInitializer("Character", "x"));
		add( new NodeInitializer("Integer", 0));
		add( new NodeInitializer("Float", 0.0));
		add( new NodeInitializer("Text", ""));
	}
}