
if type(Query.target) is IfStatement:
    parent = Query.target.parent
    parent.beginModification("If to while")
    
    condition = Query.target.condition
    thenBranch = Query.target.thenBranch
    
    Query.target.replaceChild(condition, Node.createNewNode("Expression", None))
    Query.target.replaceChild(thenBranch, Node.createNewNode("StatementItemList", None))

    whileNode = Node.createNewNode("LoopStatement", None)
    whileNode.condition = condition
    whileNode.body = thenBranch
    
    parent.replaceChild(Query.target, whileNode)
    parent.endModification()
    
else:
    print("Command only works on if statments")
