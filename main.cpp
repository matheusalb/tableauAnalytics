#include "bib/AnalyticTableaux.cpp"


// Aplicar regras.
vector<Node*> applyRule(Node* node) {
    vector<Node*> insertedNodes;

    /*
     * pega operador principal;
     * pega o valor verdade;
     * aplica a regra
     */
    if(node->getExpression() == "((~Q) > (~P))" && node->getTruthValue() == 1) {
        insertedNodes = node->insertSides("(~Q)", 0, "(~P)", 1);
    }

    else if(node->getExpression() == "(~Q)" && node->getTruthValue() == 0) {
        insertedNodes = node->insertFront("Q", 1);
    }

    else if(node->getExpression() == "(~P)" && node->getTruthValue() == 1) {
        insertedNodes = node->insertFront("P", 0);
    }

    node->markApplied();

    return insertedNodes;
}

void checkContradictions(vector<Node*> insertedNodes) {
    for (int i = 0; i < insertedNodes.size(); i++) {
        if(insertedNodes[i]->checkContradiction())
            insertedNodes[i]->markContradiction();
    }
}


int main() {

    //criar o primeiro nó.
    Node tableau = Node("((~Q) > (~P))", 1);
    tableau.insertFront("P", 1);
    tableau.insertFront("Q", 0);

    vector<Node*> appliableNodes = tableau.getAppliableNodes();
    while(!tableau.isClosed() && !appliableNodes.empty()) {

        /*for(Node* node : appliableNodes) {

        }*/
        for(int i = 0; i < appliableNodes.size(); i++) {
            vector<Node*> insertedNodes = applyRule(appliableNodes[i]);
            checkContradictions(insertedNodes);
        }

        appliableNodes = tableau.getAppliableNodes();
    }

    if(tableau.isClosed())
        std::cout << "Tableau fechado." << std::endl;

    tableau.printTree();

    return 0;
}