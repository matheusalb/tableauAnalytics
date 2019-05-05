#include "AnalyticTableaux.cpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#define AND 0
#define OR 1
#define NOT 2
#define IMPLIES 3

using namespace std;
int mainOperator(string expression, int* op);

string checkParentheses(string expression){
    int pOpen = 0;
    int pClose = 0;
    int indexUltOpen;
    int indexUltClose;
    
    for(int i = 0; i < expression.size(); i++){
        if(expression[i] == '('){
            pOpen++;
            indexUltOpen = i;
        }
        else if(expression[i] == ')'){
            pClose++;
            indexUltClose = i;
        }
    }
    while(pOpen > pClose){
       expression.erase(expression.begin());
       pOpen--;    
    }
    while(pOpen < pClose){
        expression.pop_back();
        pClose--;
    }

    return expression;
}
// Aplicar regras.
vector<Node*> applyRule(Node* node) {
    vector<Node*> insertedNodes;
    int mainOP;
    int sbOperator;
    string subExpL;
    string subExpR;
    string subExp;
    /*
     * pega operador principal;
     * pega o valor verdade;
     * aplica a regra
     */
    mainOP = mainOperator(node->getExpression(), &sbOperator);

    if(sbOperator != -1 && sbOperator != NOT){
        subExpL = node->getExpression().substr(0, mainOP - 1);
        subExpR = node->getExpression().substr(mainOP + 2);
        subExpL = checkParentheses(subExpL);
        subExpR = checkParentheses(subExpR);
    }
    else if(sbOperator == NOT){
        subExp = node->getExpression().substr(mainOP + 1);
        subExp = checkParentheses(subExp);
    }


    //atribuindo valores
    if(sbOperator == AND && node->getTruthValue() == 1){
        insertedNodes = node->insertFront(subExpL, 1, subExpR, 1);
    }
    else if(sbOperator == AND && node->getTruthValue() == 0){
        insertedNodes = node->insertSides(subExpL, 0, subExpR, 0);
    }
    else if(sbOperator == OR && node->getTruthValue() == 1){
        insertedNodes = node->insertSides(subExpL, 1, subExpR, 1);
    }
    else if(sbOperator == OR && node->getTruthValue() == 0){
        insertedNodes = node->insertFront(subExpL, 0, subExpR, 0);
    }
    else if(sbOperator == NOT && node->getTruthValue() == 0){
        insertedNodes = node->insertFront(subExp, 1);
    }
    else if(sbOperator == NOT && node->getTruthValue() == 1){
        insertedNodes = node->insertFront(subExp, 0);
    }
    else if(sbOperator == IMPLIES && node->getTruthValue() == 0){
        insertedNodes = node->insertFront(subExpL, 1, subExpR, 0);
    }
    else if(sbOperator == IMPLIES && node->getTruthValue() == 1){
        insertedNodes = node->insertSides(subExpL, 0, subExpR, 1);
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

int mainOperator(string expression, int* op){
    int pOpen = 0;
    int pClose = 0;
    int position;

    for(int i=0; i < expression.size();i++){
        if(expression[i] == '(')
            pOpen++;
        else if(expression[i] == ')')
            pClose++;
        else if( (expression[i] == '~' || expression[i] == 'v' || expression[i] == '&' || expression[i] == '>') && (pClose == pOpen - 1) ){
            position = i;
            if(expression[i] == '~')
                *op = NOT;
            else if(expression[i] == 'v')
                *op = OR;
            else if(expression[i] == '&')
                *op = AND;
            else if(expression[i] == '>')
                *op = IMPLIES;
            break;
        }
    }
    if(pOpen == 0 && pClose == 0){
        position = -1;
        *op = -1;
    }
    return position;
}

vector<string> separateSet(string set){
    vector<string> expressions;

    size_t position;
    int i = 0;

    do{
        position = set.find(",");
       // cout<< "position: "<<position<<endl;

        if(position != string::npos){

            expressions.push_back(set.substr(0, position));
            set = set.substr(position + 2);
           // cout << expressions[i] <<" "<< "A"<<endl;
            i++;
        }
        else{
            expressions.push_back(set.substr(0));
            //cout << expressions[i] <<" "<< "A"<<endl;;
        }
    }while(position != string::npos);

    return expressions;
}

bool verifyAlpha(vector<Node*> nodes, int* index){
    bool isAlpha = false;
    string expression;
    int op;

    for(int i = 0; i< nodes.size(); i++){
        expression = nodes[i]->getExpression();
        mainOperator(expression, &op);
        if(op == AND && nodes[i]->getTruthValue() == 1){
            isAlpha = true;
            *index = i;
            break;
        }
        else if(op == OR && nodes[i]->getTruthValue() == 0){
            isAlpha = true;
            *index = i;
            break;
        }
        else if(op == NOT){
            isAlpha = true;
            *index = i;
            break;
        }
        else if(op == IMPLIES && nodes[i]->getTruthValue() == 0){
            isAlpha = true;
            *index = i;
            break;
        }
    }
    return isAlpha;
}

int main() {

    int n;
    string text;
    string expression;
    string question;
    string N;
    string found_letter = "e";
    char spc = ' ';
    string set;
    int mainOP;
    ifstream inFile;
    ofstream outFile;
    
    outFile.open("Saida.out", ios::out);
    inFile.open("Entrada.in", ios::in);
    if (! outFile) {  
        cout << "Arquivo Saida.out nao pode ser aberto" << endl;    
        abort(); 
    }
    if (! inFile) {  
        cout << "Arquivo Entrada.in nao pode ser aberto" << endl;   
        abort();
    }

    getline(inFile, N);
    n = stoi(N);

    for(int problem = 0; problem < n; problem++){
        getline(inFile,text);
        text.pop_back();

        size_t found_index = text.find(found_letter);
       // cout<<"fi="<< found_index<<endl;
        expression = text.substr(0,found_index - 1);
        
        if(text[found_index + 2] == 'c'){
            size_t found_index = text.find("{");

            set = text.substr(found_index + 1);
            set.pop_back();
            set.pop_back();
            question = "consequencia logica?";
            //cout<< "expression:" << expression <<spc <<"expression2:"<< set <<spc<< "question:"<< question << endl;
        }
        else{ 
            question = text.substr(found_index + 2);//, text.size() - (found_index + 2) );
           // cout << question.size() <<endl;
         //   cout<< "expression:" << expression <<spc << "question:"<< question<< endl;    
        }

        if(question == "satisfativel?"){
            //cout<< "entrou Satisf"<<endl;
            Node tableau(expression, 1);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){

                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha)
                        insertedNodes = applyRule(appliableNodes[i]);
                    else
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
                //appliableNodes = tableau.getAppliableNodes();
            }
            
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Nao, nao e satisfativel."<< endl;
                //std::cout << "Tableau fechado." << std::endl;
            else
                outFile << "Sim, e satisfativel."<<endl;
            if(problem + 1 != n)
                outFile << endl;
            //tableau.printTree();
        
        
        }
        else if(question == "insatisfativel?"){
            //cout<< "entrou INSatisf"<<endl;
            Node tableau(expression, 1);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){

                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha)
                        insertedNodes = applyRule(appliableNodes[i]);
                    else
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
                //appliableNodes = tableau.getAppliableNodes();
            }
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Sim, e insatisfativel."<< endl;
            else
                outFile<< "Nao, nao e insatisfativel."<< endl;  
            if(problem + 1 != n)
                outFile << endl;
            //tableau.printTree();
        }
        else if(question == "tautologia?"){
            //cout<< "entrou TAUTOLOGIA"<<endl;
            Node tableau(expression, 0);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){

                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha)
                        insertedNodes = applyRule(appliableNodes[i]);
                    else
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
                //appliableNodes = tableau.getAppliableNodes();
            }
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Sim, e tautologia." << endl;
            else
                outFile << "Nao, nao e tautologia."<<endl;
            if(problem + 1 != n)
                outFile << endl;
                //std::cout << "Tableau fechado." << std::endl;

            //tableau.printTree();
        }
        else if(question == "refutavel?"){
            //cout<< "entrou REFUTAVEL"<<endl;
            Node tableau(expression, 0);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){

                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha)
                        insertedNodes = applyRule(appliableNodes[i]);
                    else
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
                //appliableNodes = tableau.getAppliableNodes();
            }
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile<<"Nao, nao e refutavel."<<endl;
            else
                outFile<<"Sim, e refutavel."<<endl;
            if(problem + 1 != n)
                outFile << endl;

            //tableau.printTree();
        }
        else if(question == "consequencia logica?"){
            //cout<< "entrou CL"<<endl;
            Node tableau(expression, 0);
            vector<string> expressions;
            expressions = separateSet(set);
            ;
            
            for(int i = 0; i < expressions.size(); i++){
                tableau.insertFront(expressions[i], 1);   
            }

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){

                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha)
                        insertedNodes = applyRule(appliableNodes[i]);
                    else
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
                //appliableNodes = tableau.getAppliableNodes();
            }

            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Sim, e consequencia logica."<< endl;
            else
                outFile<< "Nao, nao e consequencia logica."<< endl;  
            
            if(problem + 1 != n)
                outFile << endl;
        }

    }  
    inFile.close();
    outFile.close();

    return 0;
}