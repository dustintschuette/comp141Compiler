//Dustin Schuette and Kevin Dejeu
//scanner 1.2



#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <sstream>
#include <stdlib.h>
#include <map>

using namespace std;
enum TType {TT_Iden, TT_Symbol, TT_Num, TT_Err, TT_Keyword};


class TToken{
    public: 
    string value;
    TType type;

    TToken(string value, TType type){
        this->value = "";
        this->value.append(value);
        this->type = type;
    }
    TToken(){
        value = "";
        type = TT_Err;

    }
    string getValue(){
    return value;
    }
    string getType(){
    return "" + type;
    }
    TType getTType(){
        return type;
    }


};
class tokenStream{
    public:
    vector<TToken> tokens;
    int currIndex = 0;
    int lastSymbol = 0;

    tokenStream(vector<TToken*> tokens){
        for(int i = 0; i < tokens.size();i ++){
            if(tokens.at(i)->getTType() != TT_Err){
                this->tokens.push_back (*tokens.at(i));
            }
           
            //cout << "Adding token: " << tokens.at(i)->getValue() << endl;
        }

        currIndex = 0;
        lastSymbol = 0;

    }

    string currentToken(){
        //cout << "currIndex: " << currIndex << " vector size: " << tokens.size() << endl;
        if(currIndex > tokens.size()-1){
          return tokens.at(tokens.size()-1).getValue();  
        }
        return tokens.at(currIndex).getValue();
    }

    void consumeToken(){
        lastSymbol = currIndex;
        if(currIndex == tokens.size() -1){

        }else{
          currIndex++;  
        }
        

    }

    TToken getLastSymbol(){
        return tokens.at(lastSymbol);
    }
    TType getCurrType(){
        return tokens.at(currIndex).getTType();
    }
    TToken getToken(){
        return tokens.at(currIndex);
    }
    int size(){
        return tokens.size();
    }
    TToken atIndex(int i){
        return tokens.at(i);
    }


};

class bTree{
    public:
    bTree* leftNode;
    bTree* middleNode;
    bTree* rightNode;
    TToken value;

    bTree(TToken value, bTree* left, bTree* right){
        this->value = value;
        leftNode = left;
        rightNode = right;
        middleNode = NULL;
    }
    bTree(TToken value){
        this->value = value;
        leftNode = NULL;
        rightNode = NULL;
        middleNode = NULL;
    }
    bTree(TToken value, bTree* left, bTree* middle,  bTree* right){
        this->value = value;
        leftNode = left;
        rightNode = right;
        middleNode = middle;
    }

    string getValue(){
        return value.getValue();
    }
};

//Prototyping

bTree* createTree(tokenStream* tokens);
bTree* parseExpression(tokenStream* tokens);
bTree* parseTerm(tokenStream* tokens);
bTree* parseFactor(tokenStream* tokens);
bTree* parsePiece(tokenStream* tokens);
bTree* parseElement(tokenStream* tokens);
void printTree(bTree tree, int indent);
string tokenToString(TToken* currTok);
bTree* parseStatement (tokenStream* tokens);
bTree* parseBaseStatment (tokenStream* tokens);
bTree* parseAssignment(tokenStream* tokens);
bTree* parseIfStatement(tokenStream* tokens);
bTree* parseWhileStatement(tokenStream* tokens);

void buildStack(bTree* tree, vector<TToken> &stack);
void evalTree(bTree* t, map<string, int> &m);
TToken evalMath(TToken op, TToken s2, TToken s3, map<string, int> &m);
int eval(vector<TToken> stack, map<string, int> &m);






/*statement ::=basestatement { ; basestatement }
basestatement ::=assignment | ifstatement | whilestatement | skip
assignmet ::=IDENTIFIER := expression
ifstatement ::=if expression then statement else statement endif
whilestatement ::=while expression do statement endwhile*/

bTree* createTree(tokenStream* tokens){
   return parseStatement(tokens);
}

bTree* parseStatement(tokenStream* tokens){
    bool flag =false;
    cout << "parsestatement called" << endl;
    bTree* t = parseBaseStatment(tokens);
    while(tokens->currentToken() == ";"){
        cout << "creating ; tree" << endl;
        flag = true;
        tokens->consumeToken();
        cout << "baseStatement started with token: " << tokens->currentToken() << endl;
        t = new bTree(tokens->getLastSymbol(), t, parseBaseStatment(tokens));
        //cout << "Looping" << tokens->currentToken() << endl;
        
    }
    if(flag){

    }
    
    return t;
}

// FIX GETLASTSYMBOL
bTree* parseBaseStatment(tokenStream* tokens){
    cout << "parseBaseStatementcalled" << endl;
    if(tokens->currentToken() == "if"){
        //cout << "getting token" << endl;
        return parseIfStatement(tokens);
    }else if(tokens->currentToken() == "while"){
        return parseWhileStatement(tokens);
    }else if(tokens->currentToken() == "skip"){
        cout << "entered skip" << endl;
        tokens->consumeToken();
        bTree* t = new bTree(tokens->getLastSymbol());
        return t;
    }else{
        bTree* t = parseAssignment(tokens);
        return t;
    }
}

bTree* parseWhileStatement(tokenStream* tokens){
    cout << "parseWhileStatementcalled"  << endl;
    tokens->consumeToken();
    bTree* t = parseExpression(tokens);
    if(tokens->currentToken() == "do"){
        tokens->consumeToken();
    }
    else{
        cout << "Error parsing While statement: expecting do got: " << tokens->currentToken() << endl;
        exit(1);
    }
    bTree* s = parseStatement(tokens);
    if(tokens->currentToken() == "endwhile"){
        tokens->consumeToken();
    }else{
        cout << "Error parsing While statement: expecting endwhile got: " << tokens->currentToken() << endl;
        exit(1);
    }
    TToken* z = new TToken("while", TT_Keyword);
    return new bTree (*z, t, s);
}

bTree* parseAssignment(tokenStream* tokens){
    cout << "parseAssignmentcalled" << endl;
    bTree* t = parseElement(tokens);
    if(t->value.getTType() != TT_Iden){
        cout << "expecting an identifier got: " + t->value.value << endl;
        exit(1);
    }
    if((tokens->currentToken() == ":=")){
        tokens->consumeToken();
        TToken z(":=", TT_Symbol);
        bTree* s = parseExpression(tokens);
        t = new bTree(z, t, s);
        return t;
    }
    else{
        cout << "Error parsing Assignment: expecting := got " << tokens->currentToken() << endl;
        exit(1);
    }
}
bTree* parseIfStatement(tokenStream* tokens){
    cout << "parseIfStatementcalled" << endl;
    tokens->consumeToken();
    bTree* t = parseExpression(tokens);
    if(tokens->currentToken() == "then"){
        tokens->consumeToken();

    }else{
        cout << "Error in parseIfStatement: expecting then, got: " << tokens->currentToken() << endl;
        exit(1);
    }
    bTree* s = parseStatement(tokens);
    if(tokens->currentToken() == "else"){
        tokens->consumeToken();
    }
    else{
        cout << "Error in parseIfStatement: expecting else, got: " << tokens->currentToken() << endl;
        exit(1);
    }
    bTree* r = parseStatement(tokens);
    if(tokens->currentToken() == "endif"){
        tokens->consumeToken();
        TToken* z = new TToken("if", TT_Keyword);
        return new bTree(*z, t, s, r);
    }else{
        cout << "Error in parseIfStatement: expecting endif, got: " << tokens->currentToken() << endl;
        exit(1);
    }
}

bTree* parseExpression(tokenStream* tokens){
    //cout << "parseExpression called" << endl;
    bTree* t = parseTerm(tokens);
    //cout << tokens->currentToken() << endl;
    while(tokens->currentToken() == "+"){
        //cout << "found + symbol" << endl;
        tokens->consumeToken();
        t = new bTree(tokens->getLastSymbol(), t, parseTerm(tokens));
        //cout << "Looping" << tokens->currentToken() << endl;
        
    }
    //cout << "exiting loop" << endl;
    
    return t;
}

bTree* parseTerm(tokenStream* tokens){
    //cout << "parseTerm called on " << tokens->currentToken() << endl;
    bTree* t = parseFactor(tokens);
    while(tokens->currentToken() == "-"){
        TToken op = tokens->getToken();
        tokens->consumeToken();
        t = new bTree(op, t, parseFactor(tokens));
    }
    return t;

}

bTree* parseFactor(tokenStream* tokens){
        //cout << "parseFactor called on " << tokens->currentToken() << endl;
        bTree* t = parsePiece(tokens);
        cout << "looking for / " << tokens->currentToken() << endl;
        
        while(tokens->currentToken() == "/"){
            TToken op = tokens->getToken();
            tokens->consumeToken();
            //cout << "looking for / : " << tokens->currentToken() << endl;
            t = new bTree(op, t, parsePiece(tokens));
            
        }
        return t;
}

bTree* parsePiece(tokenStream* tokens){
    //cout << "parsePiece called on " << tokens->currentToken() << endl;
    bTree* t = parseElement(tokens);
    while(tokens->currentToken() == "*"){
        TToken op = tokens->getToken();
        tokens->consumeToken();
        t = new bTree(op, t, parseElement(tokens));
    }
    return t;

}

bTree* parseElement(tokenStream* tokens){
    //cout << "parseElement called" << endl;
    if(tokens->getCurrType() == TT_Num){
        //cout << "getting token" << endl;
        bTree* t = new bTree(tokens->getToken());
        //cout << "Consuming token" << endl;
        tokens->consumeToken();
        //cout << "Consumed Token" << endl;
        return t;
    }else if(tokens->getCurrType() == TT_Iden){
        bTree* t = new bTree(tokens->getToken());
        tokens->consumeToken();
        return t;
    }else if(tokens->currentToken() == "("){
        cout << "entered paranthesis" << endl;
        tokens->consumeToken();
        bTree* t = parseExpression(tokens);
        if(tokens->currentToken() == ")"){
            tokens->consumeToken();
            cout << "ending paranthesis" << endl;
            
            return t;
        }else{
            cout << "Error: missing closing parenthesis" << endl;
            exit(1); 
        }
    }else if(tokens->currentToken() == ")"){
        cout << "Error: closing parenthesis wihtout opening";
        exit(1);
    }
    
    else{
        cout << "Error: Expected Element at " << tokens->currentToken() << endl;
        exit(1);
    }
}

void printTree(bTree *tree, int indent, ofstream &output){
    
    if(tree == NULL){
        return;
    }
    for(int i = 0; i < indent; i++){
        cout << "  ";
        output << "  ";
    }
    TToken * temp = &tree->value;
    cout <<  tree->getValue()  << ": " << tokenToString(temp)<< endl;
    output << tree->getValue()  << ": " << tokenToString(temp)<< endl;
    indent++;
    printTree(tree->leftNode, indent, output);
    printTree(tree->middleNode, indent, output);
    printTree(tree->rightNode, indent, output);
}


bool tokenIdentify(char tokenArray[], int size){
    string test = "";
    for (int i = 0; i < size; i++){
        test.append(1, tokenArray[i]);
    }
    regex symbol("\\+|\\*|\\(|\\)|\\-|\\/|\\\\|\\-|:=|;|:");
    //regex slash(R"\\\\let\\\\~");
    regex digit("[0-9]+");
    regex identifier("(_|[a-z]|[A-Z])(_|[a-z]|[A-Z]|[0-9])*");
    if(regex_match(test, symbol)){
        return true;
    }else if(regex_match(test, digit)){
        return true;
    }else if(regex_match(test, identifier)){
        return true;
    }else{
        return false;
    }


    

}


void addToken(string currToken, vector<TToken*> &tokens){
    //fix :=
    regex symbol("\\+|\\*|\\(|\\)|\\-|\\/|\\\\|\\-|:=|;|:");
    regex digit("[0-9]+");
    regex identifier("(_|[a-z]|[A-Z])(_|[a-z]|[A-Z]|[0-9])*");
    string keyWords[8] = {"if", "then", "else", "endif", "while", "do","endwhile", "skip" };


    cout << "currToken: " << currToken << endl;
            bool isKey = false;
            for(int i = 0; i < 8; i ++){
                if(currToken == keyWords[i]){
                    isKey = true;
                }
        
            }
            if(isKey == true){
                cout << ": keyword" << endl;
                tokens.push_back(new TToken(currToken, TT_Keyword));
            }
            //special case for :
            else if(currToken == ":" || currToken == "::"){
                tokens.push_back(new TToken(currToken, TT_Err));
            }
            else if(regex_match(currToken, digit)){
                cout << ": number" << endl;
                tokens.push_back(new TToken(currToken, TT_Num));
            }else if(regex_match(currToken, symbol)){
                cout << ": symbol" << endl;
                tokens.push_back(new TToken(currToken, TT_Symbol));
            }else if(regex_match(currToken, identifier)){
                cout << ": identifier" << endl;
                tokens.push_back(new TToken(currToken, TT_Iden));
            }else{
                int tokenL = currToken.length();
                char tokenArray[tokenL];
                strcpy(tokenArray, currToken.c_str());

                for(int i = 1; i < tokenL; i++){
                    if(!tokenIdentify(tokenArray, i)){
                        if(i == 1){
                            // look at this
                            string token2 = "";
                            //cout << endl <<  "error added " << currToken << endl;
                            tokens.push_back(new TToken(currToken, TT_Err));
                            //break;
                            //code if we want the token to be read after an incorrect token
                            //for(int j = 1; j < tokenL; j++){
                            //    token2 += tokenArray[j];
                            //}
                            addToken(token2, tokens);
                        }else{
                            if(tokenArray[0] == ':' && tokenArray[1] != '='){
                                cout << "special case :" << endl;
                                tokens.push_back(new TToken(currToken, TT_Err));
                            }else{
                                //cout << "splitting token, token 1: ";
                                string token2 = "";
                                for(int j = 0; j < i-1; j++){
                                    token2.append(1, tokenArray[j]);
                                }
                                cout << "adding token value: " << token2 << endl;
                                //cout << token2 << endl;
                                addToken(token2, tokens);
                                string token3 = "";
                                for(int j = i-1; j < tokenL; j++){
                                    token3.append(1, tokenArray[j]);
                                }
                                cout << "adding token value: " << token2 << endl;
                                //cout << "token 2: " << token3 << endl;
                                addToken(token3, tokens);
                            }
                            
                        }
                        return;
                    }
                }
                if(tokenL == 1 && !tokenIdentify(tokenArray, 1)){
                    //cout << "Error on char : " << currToken << endl;
                    tokens.push_back(new TToken(currToken, TT_Err));
                }else if(!tokenIdentify(tokenArray, tokenL) && tokenL > 1){
                    //last character is the problem
                    string token5 = "";
                    for(int i = 0; i < tokenL-1; i++){
                       token5.append(1, tokenArray[i]); 
                    }
                    cout << "token5 : " << token5 << endl;
                    string token6 = "";
                    token6.append(1, tokenArray[tokenL-1]);
                    addToken(token5, tokens);
                    addToken(token6, tokens);
                    return;
                    
                }else{

                }

            }
            //cout << endl;
            

}

string tokenToString(TToken* currTok){
TType temp = currTok -> type;
	if( temp ==  TT_Iden){
	    return "identifier";

    }else if(temp == TT_Symbol){
        return "Symbol";
    }else if(temp == TT_Num){
        return "Num";
    }else if(temp == TT_Keyword){
        return "Keyword";
    }else{
        return "error";}

}
//uses preorder traversal to build a stack
void buildStack(bTree* tree, vector<TToken> &stack){
    if(tree == NULL){return;}
    stack.push_back(tree->value);
    buildStack(tree->leftNode, stack);
    buildStack(tree->rightNode, stack);

}
//recieves a built stack and evaulates it
int eval(vector<TToken> stack, map<string, int> &m){
    while(stack.size() > 2){
        TToken s1, s2, s3, result;
        int index = 0;

        for(int i = stack.size()-1; i > 0; i--){
            if(stack.at(i).getTType() == TT_Symbol){
                //cout << "Symbol found: " << stack.at(i).getValue() << " at index: " << i << endl;
                index = i;
                break;
            }
        }

        s1 = stack.at(index);
        cout << "s1 is: " << stack.at(index).getValue() << endl;
        stack.erase(stack.begin() + index);
        s2 = stack.at(index);
        cout << "s2 is: " << stack.at(index).getValue() << endl;
        stack.erase(stack.begin() + index);
        s3 = stack.at(index);
        cout << "s3 is: " << stack.at(index).getValue() << endl;
        result  = evalMath(s1, s2, s3, m);
        stack.at(index) = result;
    }
    cout << "result: " + stack.at(0).getValue() << endl;
    return atoi(stack.at(0).getValue().c_str());
}
//helper function to evaluate simple math on two tokens and an operand
TToken evalMath(TToken op, TToken s2, TToken s3, map<string, int> &m){
    int left, right;
    if(s2.getTType() == TT_Iden){
        if(m.find(s2.getValue()) != m.end()){
            left = m.find(s2.getValue())->second;
        }else{
            cout << "Undeclared identifier used: " + s2.getValue() << endl;
            exit(1);
        }
    }else{
        left = atoi(s2.getValue().c_str());
    }

    if(s3.getTType() == TT_Iden){
        if(m.find(s3.getValue()) != m.end()){
            right = m.find(s3.getValue())->second;
        }else{
            cout << "Undeclared identifier used: " + s3.getValue() << endl;
            exit(1);
        }
    }else{
        right= atoi(s3.getValue().c_str());
    }
    
    string ops = op.getValue();
    cout << "Evaluating: " << left << " " + ops + " " << right << endl;
    if(ops == "+"){
        TToken z( (to_string(left + right)), TT_Num);
        return z;
    }else if(ops == "-"){
        TToken z((to_string(left - right)), TT_Num);
        return z;
    }else if(ops == "*"){
        TToken z((to_string(left * right)), TT_Num);
        return z;
    }else if(ops == "/"){
        if(right == 0){
                    cout << "Error dividing by 0. Exiting ...." << endl;
                    exit(1);
        }
        TToken z( (to_string(left / right)), TT_Num);
        return z;
    }else{
        cout << "Error in MathEval, expected math operation got: " << ops << endl;
        exit(1);
    }
}

void evalTree(bTree* t, map<string, int> &m){
        if(t->value.value == ";"){
            //more than one basestatement
            //go left and evaluate
            evalTree(t->leftNode, m);
            evalTree(t->rightNode, m);
        }
        if(t->value.value == ":="){
            //assignments should be first if used
            //cout << "Assignment called on " << t->leftNode->value.value << endl;
            vector<TToken> s;
            buildStack(t->rightNode, s);
            int val = eval(s, m);
            //check to see if identifier is already in the map
            if(m.find(t->leftNode->value.value) != m.end()){
                m.find(t->leftNode->value.value)->second = val;
            }else{
                m.insert(pair<string, int>(t->leftNode->value.value, val));
            }
            
        }
        if(t->value.value == "if"){
            //check to see if condition is met
            vector<TToken> stack;
            buildStack(t->leftNode, stack);
            int val = eval(stack, m);
            bool condition = val > 0;
            if(condition){
                cout << "eval: " + val << endl;
                evalTree(t->middleNode, m);
            }else{
                evalTree(t->rightNode, m);
            }
        }
        if(t->value.value == "while"){
            vector<TToken> stack;
            buildStack(t->leftNode, stack);
            int val = eval(stack, m);

            while(val > 0){

                cout << "entered while loop " + val << endl;
                evalTree(t->rightNode, m);
                stack.clear();

                buildStack(t->leftNode, stack);
                val = eval(stack, m);
            }
        }
    }

int main(int argc, char* argv[]){
    vector<TToken*> tokens;
    vector<string> tokenList;
    if(argc < 3){
        return 0;
    }

    //cout << argv[1] <<  " " <<  argv[2];

    string targetOutput = argv[2];
    string targetInput = argv[1];

    ofstream output;
    output.open(targetOutput);
    if(!output){
        cout << "Error opening output file" << endl;
        return 1;

    }
    ifstream input{targetInput};
    if(!input){
        cout << "Error opening input file" << endl;
        return 1;
    }

    string line;
    string currToken;
    int lineCount = 1;
    int placeHolder = 0;
    while (getline(input, line)) {
        
        output << "line " << lineCount << ": " << line << endl;
        lineCount++;
        //call the parser to seperate tokens by whitespace 
        stringstream inStream (line);
        while(inStream >> currToken){
            //cout << "currToken : " << currToken << endl;
            addToken(currToken, tokens);
        }

        //write tokens to output one token per line
        for(int i = placeHolder; i < tokens.size(); i++){
	        string temp = tokens.at(i)->getValue();
            cout << "Writing: " << temp << endl;
	        //output << temp << " " << tokenToString(tokens.at(i)) <<endl;
            output.write( temp.data(), temp.length());
            output << " " << tokenToString(tokens.at(i)) << endl;
            placeHolder++;
	    }

    }
    
	//cout << tokenList.size() << endl;


    //expression ::= term {+ term}
    //term ::= factor {- factor}
    //factor ::= piece {/ factor}
    //piece ::= element {* element}
    //element ::= (expression) | Number | Identifier
    cout << endl << "Starting tree" << endl;
    tokenStream* tokenStr = new tokenStream(tokens);
    cout << "tokenStream created" << endl;
    bTree* t = createTree(tokenStr);
    cout << "Tree created" << endl;
    printTree(t, 0, output);
    
    //evaluate the tree

    //vector<TToken> stack;
    //buildStack(t, stack);
    //cout << "stack built:" << endl;
    map<string, int> m;
    evalTree(t, m);
    map<string, int>::iterator it;
    output << "Output: " << endl;
    for (it = m.begin(); it != m.end(); it++) {
        cout << it->first + " := " << it->second << endl;
        output << it->first + " := " << it->second << endl;
    }
    
	
	output.close();

}
