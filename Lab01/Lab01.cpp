/* File: Lab01.cpp
 * ----------------
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <algorithm>
//#include <stdlib.h>

using namespace std;

// function prototype declarations
bool isNumber(int tokenIndex);
bool isSymbol(int tokenIndex);
bool is_valid_symbol(string token);
bool isType(int tokenIndex);
void input_file();
bool parseDefList();
bool parseUseList();
bool parseProgramText();
void printSymbolTable();
void E_instruction(int type, vector<string> useList);
void R_instruction(int useIndex, int moduleCt);
void A_instruction(int useIndex); 
void I_instruction(int useIndex); 
int parseInstruction(string token);
string expressInstruction(int num);
string getOrder(int index); 
void printWarning();
void printModuleWarning(vector<string> useList, int moduleCt);
void printDefWarning();
bool is_too_long(int tokenIndex);

/*      Global Variables      */
vector<string> tokenVec;  // all tokens 
vector<int> lineList;    // line number of each token
vector<int> offsetList;  // store each offsets
vector<int> lengthList;   // Store the length of each line

vector<int> defCountList;
vector<string> defSymList;  // all def symbols in the file
vector<int> defAddList;
vector<string> useSymList; // all use symbols from the file
vector<int> codeCountList; 
vector<string> typeList;   // all IAER
vector<string> instruList; // all 1000

vector<string> symbolTable;  // for print
vector<int> baseAddList;
vector<string> defShowup;   // store all tokens that have been tested

vector< vector<string> > useListVector;

map<string, int> defMap;  // <defSym, moduleCt>
map<string, int> defSymMap; // <defSym, OccurCt>
map<string, int> symValueMap; //<symbol, value>
map<string, int> useCtMap;   // <useSym, OccurCt>
map<string, int> symModMap; // <TableSym, ModuleCt>
map<int,int> ModSizeMap;   // <ModuleCt, ModuleSize>
string str;
string defcount;
string endToken;

int lineNum = 0;
int offsetNum = 0;

int nonSpace;
int space;
int tokenIndex = 0;

int useCount;
int codeCount;
int tokenCt;  // The number of total tokens in the file
int sum = 0;

int moduleCt = 0;


int main (int argc, char* argv[]){
    ifstream infile (argv[1], ios::binary);
    string line;
    if (infile.is_open()) {

        //input_file();
        while ( getline(infile, line)) { 
            lineNum++;  // line number + 1 
            lengthList.push_back(line.length()); // store length of each line
            stringstream in(line);
            int offset = -1;
            for (string token; in >> token;){
                // find index of token in line
                offset = line.find(token, offset+1);
                tokenVec.push_back(token);  // Push tokens into tokenVec
                lineList.push_back(lineNum);
                offsetList.push_back(offset+1);
            }
        }
        // record end position of txt file
        int endLine = lineNum; 
        int endColumn = lengthList.back() + 1;	
        lineList.push_back(endLine);
        offsetList.push_back(endColumn);

        infile.close();
        tokenCt = tokenVec.size();

        /*       Pass One         */
        baseAddList.push_back(0);
        /* Pop up defcount */
        while( tokenIndex < tokenCt ) {
            if( !parseDefList() ) {
                return 1;
            }
            if ( !parseUseList() ) {
                return 1;
            }	
            if ( !parseProgramText() ) {
                return 1;
            }	
            tokenIndex++;
            moduleCt++;
            baseAddList.push_back(typeList.size());
        }

        printSymbolTable();


        /*       Pass Two         */
        int moduleNum = codeCountList.size();
        int moduleCt = 0;
        int useIndex = 0;
        //Module by module
        cout << "Memory Map" << endl;
        while( moduleCt < moduleNum ) {
            vector<string> useList = useListVector[moduleCt]; 
            int size = useList.size();
            for (int i=0; i<size; i++){
                map<string, int>::const_iterator iter = useCtMap.find( useList[i] );
                if ( iter == useCtMap.end()) {
                    useCtMap.insert(make_pair(useList[i], 0));
                }
            }
            for (int i=0; i< codeCountList[moduleCt]; i++) {
                string type = typeList[useIndex];
                if (type == "A")  	
                    A_instruction(useIndex);
                if (type == "I")  	
                    I_instruction(useIndex);
                if (type=="E")
                    E_instruction(useIndex, useList);
                if (type == "R")
                    R_instruction(useIndex, moduleCt);
                useIndex++;
            }
            printModuleWarning(useList, moduleCt);
            moduleCt++;
        }

        printWarning();



    } else {
        cout << "Unable to open file" << endl;
    }	
    return 0;
}  // "main" ends here


/******************************/
/*     Private Functions      */
/******************************/

/* isNumber(string token) if token is a pure number, return true; otherwise, return false */
bool isNumber(int tokenIndex) {
    if (tokenIndex == tokenCt) {
        return false;
    } else {
        int i = 0;
        string token = tokenVec[tokenIndex];
        int length = token.length();
        while ( i < length ){
            char ch = token[i];
            if ( ch < '0' || ch > '9') {
                return false;
            }
            i++;
        }
        return true;
    }
}

/* isSymbol: check wether token is leagal. */
bool isSymbol(int tokenIndex) {
    // Case 1: Miss token, no token left to be read
    if ( tokenIndex == tokenCt ) {
        // offset of 'endToken'
        return false;
    } else {
        // Case 2: Unexpected token, not of the form [a-Z][a-Z0-9]*
        string token = tokenVec[tokenIndex];
        if ( !is_valid_symbol(token) ){
            return false;	
        }	
    }
    return true;
}


/* is_valid_symbol(string token), in the form */
bool is_valid_symbol(string token){
    char ch = token[0];
    if ( !isalpha(ch) ) {
        return false;
    } else {
        int index = 1;
        int length = token.length();
        // check latter char isalnum()
        while (index < length) {
            char ch = token[index];
            if ( !isalnum(ch)) return false;
            index++;	
        }
        return true;		
    }
}

/* isType(int tokenIndex)
 * return false if miss token or unexpected token */
bool isType(int tokenIndex) {
    if (tokenIndex == tokenCt){
        return false;
    } else {
        string token = tokenVec[tokenIndex];
        return (token == "I") || (token =="A") 
            || (token=="E") || (token=="R");
    }
}

bool parseDefList() {
    /* if token is a pure number, then parse(S,R) */
    if (!isNumber(tokenIndex)){
        // !! error
        cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
            << offsetList[tokenIndex] <<	": NUM_EXPECTED"<< endl;
        return false;  // end of program	
    } else {
        string defCount = tokenVec[tokenIndex];
        defCountList.push_back(atoi(defCount.c_str()));
        int def = atoi(defCount.c_str());
        if (def > 16) {
            cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                << offsetList[tokenIndex] << ": TO_MANY_DEF_IN_MODULE"<< endl;
            return false;
        } else {
            for (int i=0; i<atoi(defCount.c_str()); i++) {		
                tokenIndex++;
                // Miss token or Unexpected token
                if( !isSymbol(tokenIndex) ) {
                    // !! error
                    cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                        << offsetList[tokenIndex] << ": SYM_EXPECTED"<< endl;
                    return false;
                } else {
                    if (is_too_long(tokenIndex)) return false;
                    // !!!! should test that symbol has existed
                    // put symbol into def symbol list
                    string token = tokenVec[tokenIndex];
                    defSymList.push_back(token);

                    // store the module number of each def symbol
                    map<string, int>::const_iterator iter = defMap.find(token);
                    if ( iter == defMap.end() ) {
                        defMap.insert(make_pair(token, moduleCt));
                    }

                    // store the times that one symbol occurs 
                    defSymMap[ token ] += 1;
                    if (defSymMap[ token ] == 1) {
                        symbolTable.push_back(token);
                        symModMap.insert(make_pair(token, moduleCt));
                    }

                    tokenIndex++;	
                    if ( !isNumber(tokenIndex)) {
                        // !! error
                        cout << "Parse Error line " << lineList[tokenIndex] <<
                            " offset " << offsetList[tokenIndex] << ": NUM_EXPECTED"<< endl;
                        return false;
                    } else {
                        defAddList.push_back( atoi(tokenVec[tokenIndex].c_str()) );
                    } // "isNumber" if-else ends here
                } // "isSymbol" if-else ends here
            } // for-loop ends here
        }
    } // "isNumber" if-else ends here
    return true;
}

bool parseUseList(){
    /* Deal with use list */
    /* if token is a pure number, then parse(S,R) */
    vector<string> useList;
    tokenIndex++;
    if (!isNumber(tokenIndex)){
        // !! error
        cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
            << offsetList[tokenIndex] <<	": NUM_EXPECTED"<< endl;
        return false;  // end of program	
    } else {
        string useCount= tokenVec[tokenIndex];
        int def = atoi(useCount.c_str());
        if (def > 16) {
            cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                << offsetList[tokenIndex] << ": TO_MANY_USE_IN_MODULE"<< endl;
            return false;
        } else {
            for (int i=0; i<atoi(useCount.c_str()); i++) {		
                tokenIndex++;
                // Miss token or Unexpected token
                if( !isSymbol(tokenIndex) ) {
                    // !! error
                    cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                        << offsetList[tokenIndex] << ": SYM_EXPECTED"<< endl;
                    return false;
                } else {
                    if (is_too_long(tokenIndex)) return false;
                    // !!!! should test that symbol has existed
                    // put symbol into def symbol list
                    useSymList.push_back(tokenVec[tokenIndex]);
                    useList.push_back(tokenVec[tokenIndex]);
                } // "isSymbol" if-else ends here
            } // for-loop ends here
        }
    } // "isNumber" if-else ends here
    useListVector.push_back( useList );
    return true;
}

bool parseProgramText() {
    /* Read in instruction list */
    tokenIndex++;
    if (!isNumber(tokenIndex)){
        // !! error
        cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
            << offsetList[tokenIndex] <<	": NUM_EXPECTED"<< endl;
        return false;  // end of program
    } else {
        string codeCount = tokenVec[tokenIndex];
        int def = atoi(codeCount.c_str());
        if (def > (512-sum)) {
            cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                << offsetList[tokenIndex] << ": TO_MANY_INSTR"<< endl;
            return false;
        } else {
            sum += def;
            codeCountList.push_back(atoi(codeCount.c_str()));
            ModSizeMap.insert(make_pair(moduleCt, atoi(codeCount.c_str())));
            for (int i=0; i< atoi(codeCount.c_str()); i++){
                tokenIndex++;
                // Miss token or Unexpected token
                if( !isType(tokenIndex) ) {
                    // !! error
                    cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                        << offsetList[tokenIndex] << ": ADDR_EXPECTED"<< endl;
                    return false;
                } else {
                    // !!!! should test that symbol has existed
                    // put symbol into def symbol list
                    typeList.push_back(tokenVec[tokenIndex]);
                    tokenIndex++;	
                    if ( !isNumber(tokenIndex)) {
                        // !! error
                        cout << "Parse Error line " << lineList[tokenIndex] << " offset " 
                            << offsetList[tokenIndex] << ": NUM_EXPECTED"<< endl;
                        return false;
                    } else {
                        instruList.push_back( (tokenVec[tokenIndex]) );
                    }
                }
            }	
        }
    }	
    return true;
}

void printSymbolTable() {
    // print warning for rule 3
    printDefWarning();

    cout << "Symbol Table" << endl;
    int size = symbolTable.size(); 
    // check whether it has been re-defined
    for (int i = 0; i< size; i++) {
        string token = symbolTable[i];
        int value = baseAddList[defMap[token]] + defAddList[i];
        symValueMap.insert(make_pair(token, value));
        if ( defSymMap[token] == 1) {
            cout << token << "=" << value << endl;
        } else {
            cout << token << "=" << value
                << " Error: This variable is multiple times defined; first value used"<< endl;
        }
    }
    cout << endl;
}

void printDefWarning() {
    int count = 0;
    int size = codeCountList.size();
    for (int i=0; i< size; i++) {
        int num = defCountList[i];
        // compare each address in the def list to code count	
        for (int j=0;j<num; j++) {
            string token = defSymList[count];
            int address = defAddList[count];
            if (find(defShowup.begin(), defShowup.end(), token)==defShowup.end()) {
                if (address >= codeCountList[i]) {
                    cout << "Warning: Module " << i+1 << ": " 
                        << token << " to big " << address << " (max=" << codeCountList[i] - 1
                        << ") assume zero relative" << endl;
                    defAddList[count] = 0;
                }
                defShowup.push_back(token);
            }
            count++;
        }
    }
}

int parseInstruction(string token) {
    int length = token.length();
    if ( length >4 ) {
        return 9999;
    } else {
        string result;
        for (int i=0; i<(4-length); i++) {
            result += "0";
        }
        result += token;
        return atoi(result.c_str());
    }
}

string expressInstruction(int number) {
    string num;
    stringstream ss;
    ss << number;
    ss >> num;
    int length = num.length();
    string result;
    for ( int i=0; i<(4-length);i++){
        result += "0";
    }
    result += num;
    return result;
}


string getOrder(int index) {
    string result;
    string s;
    stringstream ss;
    ss << index;
    ss >> s;
    int length = s.length();
    for (int i=0; i<(3-length); i++) {
        result += "0";
    }
    result += s;
    return result;
}

void E_instruction(int useIndex, vector<string> useList) {
    string token = instruList[useIndex];
    int length = token.length();
    if (length > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instruction = parseInstruction(token);
        int opcode = instruction / 1000;
        int operand = instruction % 1000;	
        // If the symbol is in the use list but not defined in the symbol table
        int size = useList.size();
        if (operand > size-1 ) {
            cout << getOrder(useIndex) << ": " << expressInstruction(instruction) <<  
                " Error: External address exceeds length of uselist; treated as immediate" << endl;
        } else { 
            string token = useList[operand];
            map<string, int>::const_iterator iter = defSymMap.find(token);
            operand = symValueMap[token];	
            if ( iter == defSymMap.end() ) {
                cout << getOrder(useIndex) << ": "<< expressInstruction(opcode * 1000) 
                    << " Error: " << token << " is not defined; zero used" << endl;
                //symbolTable.push_back(token);
                useCtMap[token] += 1;
            } else {
                useCtMap[token] += 1; 
                //cout << "number:  " << useCtMap[token] << endl;
                cout << getOrder(useIndex) << ": " 
                    << expressInstruction(opcode * 1000 + operand) << endl;
            }
        }
    }
}

void A_instruction(int useIndex) {
    string token = instruList[useIndex];
    int length = token.length();
    if (length > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instruction = parseInstruction(token);
        int opcode = instruction / 1000;
        int operand = instruction % 1000;
        if ( operand > 511 ) {
            cout << getOrder(useIndex) << ": " << expressInstruction(opcode * 1000) << 
                " Error: Absolute address exceeds machine size; zero used" << endl;
        } else {
            cout << getOrder(useIndex) << ": " << expressInstruction(instruction) << endl;
        }
    }
}

void I_instruction(int useIndex) {
    string token = instruList[useIndex];
    if ( token.length() >4 ) {
        cout << getOrder(useIndex) << ": " << 9999 << 
            " Error: Illegal immediate value; treated as 9999" << endl;	
    } else {
        int instruction = parseInstruction(token);
        cout << getOrder(useIndex) << ": " << expressInstruction(instruction) << endl;
    }
}

void R_instruction(int useIndex, int moduleCt){
    string token = instruList[useIndex];
    int length = token.length();
    if (length > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instruction = parseInstruction(token);
        int opcode = instruction / 1000;
        int operand = instruction % 1000;
        if (operand > ModSizeMap[moduleCt]) {
            cout << getOrder(useIndex) << ": "
                << expressInstruction(opcode * 1000 + baseAddList[moduleCt]) 
                << " Error: Relative address exceeds module size; zero used" <<	endl;
        } else {
            cout << getOrder(useIndex) << ": " 
                << expressInstruction(instruction + baseAddList[moduleCt]) << endl;
        }
    }
}

void printWarning() {
    cout << endl;
    int size = symbolTable.size();
    for (int i=0; i< size; i++ ) {
        // if does not find
        string symbol = symbolTable[i];
        if (find(useSymList.begin(), useSymList.end(), symbol) == useSymList.end()){
            cout << "Warning: Module " << defMap[symbol] + 1 << ": " << symbol <<
                " was defined but never used" << endl;	
        }
    }	
}

void printModuleWarning(vector<string> useList, int moduleCt) {
    int size = useList.size();
    for (int i=0; i<size; i++) {
        string token = useList[i];
        if ( useCtMap[token] == 0) {
            cout << "Warning: Module " << moduleCt + 1 << ": " << token 
                << " appeared in the uselist but was not actually used"	<< endl;
        }
    }
}

bool is_too_long(int tokenIndex) {
    string token = tokenVec[tokenIndex];
    if (token.length()>16) {
        cout << "Parse Error line " << lineList[tokenIndex] << " offset "
            << offsetList[tokenIndex] << ": SYM_TOLONG"<< endl;
        return true;
    } else {
        return false;
    }
}
