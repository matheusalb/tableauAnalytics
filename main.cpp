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

string checkParentheses(string expression){ //função verifica e corrige a quantidade de paratenses numa expressão
    int pOpen = 0; //numero de paratenses abertos
    int pClose = 0; //numero de parenteses fechado
    int indexUltOpen; //indice do ultimo paresentese aberto da string
    int indexUltClose; //indice do ultimo paresentese fechado da string
    
    for(int i = 0; i < expression.size(); i++){ //varre a string contando o numero de parenteses
        if(expression[i] == '('){
            pOpen++;
            indexUltOpen = i;
        }
        else if(expression[i] == ')'){
            pClose++;
            indexUltClose = i;
        }
    }
    while(pOpen > pClose){ //caso tenha mais parenteses abertos do que fechado
       expression.erase(expression.begin());
       pOpen--;    
    }
    while(pOpen < pClose){ //caso tenha mais parenteses fechados do que aberto
        expression.pop_back();
        pClose--;
    }

    return expression; //retorna expressão corrigida
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
    mainOP = mainOperator(node->getExpression(), &sbOperator); //Retorna a posição do operador principal da expressão, e qual é o operador
                                                                            
    if(sbOperator != -1 && sbOperator != NOT){ //para operadores binários, separa-se em duas sub-expressões
        subExpL = node->getExpression().substr(0, mainOP - 1);
        subExpR = node->getExpression().substr(mainOP + 2);
        subExpL = checkParentheses(subExpL);
        subExpR = checkParentheses(subExpR);
    }
    else if(sbOperator == NOT){ //pega a expressão negada
        subExp = node->getExpression().substr(mainOP + 1);
        subExp = checkParentheses(subExp);
    }


    /*atribuindo valores
    verificando cada operador e o valor verdade associado*/
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

//verifica se há contradição entre os nós
void checkContradictions(vector<Node*> insertedNodes) {
    
    for (int i = 0; i < insertedNodes.size(); i++) {
        if(insertedNodes[i]->checkContradiction())
            insertedNodes[i]->markContradiction();
    }
}

//encontra o operador principal da expressão e sua posição
int mainOperator(string expression, int* op){
    int pOpen = 0;
    int pClose = 0;
    int position;

    /*Usando o fato em que ao encontrar um operador e o numero de parenteses fechado == numero de parenteses aberto -1,
    então esse operador é o operador principal*/
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
    if(pOpen == 0 && pClose == 0){//caso o elemento seja atomico
        position = -1;
        *op = -1;
    }
    return position;
}

//separa as proposições individualmente de um conjunto de proposições
vector<string> separateSet(string set){
    vector<string> expressions;

    size_t position;
    int i = 0;

    //separa e salva as proposições
    do{
        position = set.find(",");

        if(position != string::npos){

            expressions.push_back(set.substr(0, position));
            set = set.substr(position + 2);
            i++;
        }
        else{
            expressions.push_back(set.substr(0));
        }
    }while(position != string::npos);

    return expressions;
}

/*verifica qual proposição, junto com o seu valor verdade, é do tipo alfa.
  retornando o indice do vetor no qual esse nó é do tipo alfa, e um booleano dizendo se existe uma proposição do tipo alfa*/
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
    
    //abertura arquivo
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
    //leitura número de casos
    getline(inFile, N);
    n = stoi(N);

    for(int problem = 0; problem < n; problem++){
        getline(inFile,text); //leitura do problema
        text.pop_back();

        //Separa do que foi lido, as proposições e o problema
        size_t found_index = text.find(found_letter);
        expression = text.substr(0,found_index - 1); //separa a expressão
        
        if(text[found_index + 2] == 'c'){ //caso seja do formato "e consequencia logica"
            size_t found_index = text.find("{"); //pega o conjunto de sentenças
            set = text.substr(found_index + 1);
            set.pop_back();
            set.pop_back();
            question = "consequencia logica?"; //atribui a pergunta
        }
        else{ //caso nao seja do formato de consequencia logica
            question = text.substr(found_index + 2); //pega a pergunta
        }

        //verifica a pergunta
        if(question == "satisfativel?"){
            /*para verificar se uma proposição é satisfatível
            perguntamos ao tableau se existe um mundo possível no qual ela é verdadeira
            se existir, então ela é satisfatível*/
            Node tableau(expression, 1);
            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){
                
                //dá-se uma preferência para a aplicação de regras do tipo alfa
                for(int i = 0; i < appliableNodes.size(); i++){
                    isAlpha = false;
                    for(int j = 0; j < appliableNodes.size(); j++){
                        isAlpha = verifyAlpha(appliableNodes, &index);
                    } 
                    if(!isAlpha) //caso nao seja alfa, aplica-se a regra no nó disponível
                        insertedNodes = applyRule(appliableNodes[i]);
                    else //caso seja alfa, aplica-se a regra nele
                        insertedNodes = applyRule(appliableNodes[index]);

                    checkContradictions(insertedNodes);
                    appliableNodes = tableau.getAppliableNodes();
                }
            }
            
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Nao, nao e satisfativel."<< endl;
            else
                outFile << "Sim, e satisfativel."<<endl;
            if(problem + 1 != n)
                outFile << endl;
        }
        else if(question == "insatisfativel?"){
            /*para verificar se uma proposição é insatisfatível
            perguntamos ao tableau se existe um mundo possível no qual ela é verdadeira,
            se não existir, então ela é insatisfativel*/
            Node tableau(expression, 1);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){
                //dá-se uma preferência para a aplicação de regras do tipo alfa
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
        }
        else if(question == "tautologia?"){
            /*para verificar se uma proposição é tautologia
            perguntamos ao tableau se existe um mundo possível no qual ela é falsa,
            se não existir, então ela é tautologia*/
            Node tableau(expression, 0);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){
                //dá-se uma preferência para a aplicação de regras do tipo alfa
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
            }
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Sim, e tautologia." << endl;
            else
                outFile << "Nao, nao e tautologia."<<endl;
            if(problem + 1 != n)
                outFile << endl;
        }
        else if(question == "refutavel?"){
            /*para verificar se uma proposição é refutável
            perguntamos ao tableau se existe um mundo possível no qual ela é falsa,
            se existir, então ela é refutável*/
            Node tableau(expression, 0);

            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            vector<Node*> insertedNodes;
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){
                //dá-se uma preferência para a aplicação de regras do tipo alfa
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
            }
            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile<<"Nao, nao e refutavel."<<endl;
            else
                outFile<<"Sim, e refutavel."<<endl;
            if(problem + 1 != n)
                outFile << endl;

        }
        else if(question == "consequencia logica?"){
            /*para verificar se uma proposição é consequencia logica de um conjunto de proposições
            perguntamos ao tableau se existe um mundo possível no qual ela a proposição é falsa, e
            as proposições do conjunto são verdadeiras se não existir, então ela é consequencia logica do conjunto*/
            Node tableau(expression, 0);
            vector<string> expressions;
            expressions = separateSet(set);//separa as proposições do conjunto
            vector<Node*> insertedNodes;

            for(int i = 0; i < expressions.size(); i++){
                insertedNodes = tableau.insertFront(expressions[i], 1); //coloca elas no tableau, depois verifica se existe contradição
                checkContradictions(insertedNodes);
            }
            vector<Node*> appliableNodes = tableau.getAppliableNodes();
            int index;
            bool isAlpha = false;
            while(!tableau.isClosed() && !appliableNodes.empty()){
                //dá-se uma preferência para a aplicação de regras do tipo alfa
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
            }

            outFile << "Problema #"<<problem + 1<<endl;
            if(tableau.isClosed())
                outFile << "Sim, e consequencia logica."<< endl;
            else
                outFile<< "Nao, nao e consequencia logica."<< endl;  
            
            if(problem + 1 != n)
                outFile << endl;

            //tableau.printTree();
        }

    }  
    inFile.close();
    outFile.close();

    return 0;
}