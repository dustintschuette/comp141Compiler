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
    string getValue(){
    return value;
    }
    string getType(){
    return "" + type;
    }



};

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
	
	output.close();

}
