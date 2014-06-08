/* File: Lab01.cpp
 * ---------------------------
 * By           : Jingxin Zhu
 * Last Modified: 06/07/2014
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

void readFile(char* argv[]);
bool isNumber(int tokenIndex);
bool isSymbol(int tokenIndex);
bool isType(int tokenIndex);
bool parseDefList(int module);
bool parseUseList();
bool parseProgramText(int moduleCt);
void printSymbolTable();
void E_instruction(int type, vector<string>& useList);
void R_instruction(int useIndex, int moduleCt);
void A_instruction(int useIndex); 
void I_instruction(int useIndex); 
int parseInstruction(string token);
string getInstr(int num);
string getOrder(int index); 
void printWarning();
void printModuleWarning(vector<string>& useList, int moduleCt);
void printDefWarning();
bool is_too_long(int tokenIndex);

vector<string> tokenVec;  // all tokens 
vector<int> rowVec;    // line number of each token
vector<int> colVec;  // store each cols
vector<int> defCntList;
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

int tokenIndex = 0;
int tokenNum;  // The number of total tokens in the file
int sum = 0;

int main (int argc, char* argv[]){

    if (argc != 2) {
        cout << "Unexpected input format." << endl;
        exit(0);
    }
    readFile(argv);
    tokenNum = tokenVec.size();
    int moduleCt = 0;

    /*       Pass One         */
    baseAddList.push_back(0);
    /* Pop up defcount */
    while( tokenIndex < tokenNum ) {
        if( !parseDefList(moduleCt) ) {
            return 1;
        }
        if ( !parseUseList() ) {
            return 1;
        }	
        if ( !parseProgramText(moduleCt) ) {
            return 1;
        }	
        tokenIndex++;
        moduleCt++;
        baseAddList.push_back(typeList.size());
    }
    printSymbolTable();

    /*       Pass Two         */
    int moduleNum = codeCountList.size();
    moduleCt = 0;
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

    return 0;
}  // "main" ends here


/******************************/
/*     Private Functions      */
/******************************/

bool isNumber(int tokenIndex) {
    if (tokenIndex == tokenNum) {
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

bool isSymbol(int tokenIndex) {
    if ( tokenIndex == tokenNum ) {
        // Case 1: Miss token, no token left to be read
        return false;
    } else {
        // Case 2: Unexpected token, not of the form [a-Z][a-Z0-9]*
        string token = tokenVec[tokenIndex];
        char ch = token[0];
        if ( !isalpha(ch) ) {
            return false;
        } else {
            int len = token.length();
            // check latter char isalnum()
            for (int i = 0; i < len; i++) {
                ch = token[i];
                if ( !isalnum(ch)) return false;
            }
            return true;		
        }
    }
    return true;
}

/* return false if miss token or unexpected token */
bool isType(int tokenIndex) {
    if (tokenIndex == tokenNum){
        return false;
    } else {
        string token = tokenVec[tokenIndex];
        return (token == "I") || (token =="A") 
            || (token=="E") || (token=="R");
    }
}

bool parseDefList(int module) {
    /* if token is a pure number, then parse(S,R) */
    if (!isNumber(tokenIndex)){
        printf("Parse Error line %d offset %d : NUM_EXPECTED\n",
                rowVec[tokenIndex], colVec[tokenIndex]);
        return false;
    } else {
        int defCnt = atoi(tokenVec[tokenIndex].c_str());
        defCntList.push_back(defCnt);
        if (defCnt > 16) {
            printf("Parse Error line %d offset %d : TO_MANY_DEF_IN_MODULE\n",
                rowVec[tokenIndex], colVec[tokenIndex]);
            return false;
        } else {
            for (int i = 0; i < defCnt; i++) {		
                tokenIndex++;
                // Miss token or Unexpected token
                if( !isSymbol(tokenIndex) ) {
                    printf("Parse Error line %d offset %d : SYM_EXPECTED\n",
                        rowVec[tokenIndex], colVec[tokenIndex]);
                    return false;
                } else {
                    if (is_too_long(tokenIndex)) return false;
                    string token = tokenVec[tokenIndex];
                    defSymList.push_back(token);

                    // store the module number of each def symbol
                    map<string, int>::const_iterator iter = defMap.find(token);
                    if ( iter == defMap.end() ) {
                        defMap.insert(make_pair(token, module));
                    }

                    // store the times that one symbol occurs 
                    defSymMap[ token ] += 1;
                    if (defSymMap[ token ] == 1) {
                        symbolTable.push_back(token);
                        symModMap.insert(make_pair(token, module));
                    }

                    tokenIndex++;	
                    if ( !isNumber(tokenIndex)) {
                        printf("Parse Error line %d offset %d : NUM_EXPECTED\n",
                            rowVec[tokenIndex], colVec[tokenIndex]);
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
        cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
            << colVec[tokenIndex] <<	": NUM_EXPECTED"<< endl;
        return false;  // end of program	
    } else {
        string useCount= tokenVec[tokenIndex];
        int def = atoi(useCount.c_str());
        if (def > 16) {
            cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
                << colVec[tokenIndex] << ": TO_MANY_USE_IN_MODULE"<< endl;
            return false;
        } else {
            for (int i=0; i<atoi(useCount.c_str()); i++) {		
                tokenIndex++;
                // Miss token or Unexpected token
                if( !isSymbol(tokenIndex) ) {
                    // !! error
                    cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
                        << colVec[tokenIndex] << ": SYM_EXPECTED"<< endl;
                    return false;
                } else {
                    if ( is_too_long(tokenIndex) ) return false;
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

bool parseProgramText(int moduleCt) {
    /* Read in instruction list */
    tokenIndex++;
    if (!isNumber(tokenIndex)){
        // !! error
        cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
            << colVec[tokenIndex] <<	": NUM_EXPECTED"<< endl;
        return false;  // end of program
    } else {
        string codeCount = tokenVec[tokenIndex];
        int def = atoi(codeCount.c_str());
        if (def > (512-sum)) {
            cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
                << colVec[tokenIndex] << ": TO_MANY_INSTR"<< endl;
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
                    cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
                        << colVec[tokenIndex] << ": ADDR_EXPECTED"<< endl;
                    return false;
                } else {
                    // !!!! should test that symbol has existed
                    // put symbol into def symbol list
                    typeList.push_back(tokenVec[tokenIndex]);
                    tokenIndex++;	
                    if ( !isNumber(tokenIndex)) {
                        // !! error
                        cout << "Parse Error line " << rowVec[tokenIndex] << " offset " 
                            << colVec[tokenIndex] << ": NUM_EXPECTED"<< endl;
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
        int num = defCntList[i];
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
    int len = token.length();
    if ( len > 4 ) {
        // if instruction's length > 4, treat as 9999
        return 9999;
    } else {
        string result;
        for (int i = 0; i < (4 - len); i++) {
            result += "0";
        }
        result += token;
        return atoi(result.c_str());
    }
}

string getInstr(int number) {
    string num;
    stringstream ss;
    ss << number;
    ss >> num;
    int length = num.length();
    string result;
    for (int i = 0; i < (4 - length);i++){
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
    for (int i = 0; i < (3 - length); i++) {
        result += "0";
    }
    result += s;
    return result;
}

void E_instruction(int useIndex, vector<string>& useList) {
    string token = instruList[useIndex];
    int length = token.length();
    if (length > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instr = parseInstruction(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;	
        // If the symbol is in the use list but not defined in the symbol table
        int size = useList.size();
        if (operand > size-1 ) {
            cout << getOrder(useIndex) << ": " << getInstr(instr) <<  
                " Error: External address exceeds length of uselist; treated as immediate" << endl;
        } else { 
            string token = useList[operand];
            map<string, int>::const_iterator iter = defSymMap.find(token);
            operand = symValueMap[token];	
            if ( iter == defSymMap.end() ) {
                cout << getOrder(useIndex) << ": "<< getInstr(opcode * 1000) 
                    << " Error: " << token << " is not defined; zero used" << endl;
                //symbolTable.push_back(token);
                useCtMap[token] += 1;
            } else {
                useCtMap[token] += 1; 
                cout << getOrder(useIndex) << ": " 
                    << getInstr(opcode * 1000 + operand) << endl;
            }
        }
    }
}

void A_instruction(int useIndex) {
    string token = instruList[useIndex];
    if (token.length() > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instr = parseInstruction(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;
        if ( operand > 511 ) {
            cout << getOrder(useIndex) << ": " << getInstr(opcode * 1000) << 
                " Error: Absolute address exceeds machine size; zero used" << endl;
        } else {
            cout << getOrder(useIndex) << ": " << getInstr(instr) << endl;
        }
    }
}

void I_instruction(int useIndex) {
    string token = instruList[useIndex];
    if ( token.length() > 4 ) {
        cout << getOrder(useIndex) << ": " << 9999 << 
            " Error: Illegal immediate value; treated as 9999" << endl;	
    } else {
        int instr = parseInstruction(token);
        cout << getOrder(useIndex) << ": " << getInstr(instr) << endl;
    }
}

void R_instruction(int useIndex, int moduleCt){
    string token = instruList[useIndex];
    if (token.length() > 4) {
        cout << getOrder(useIndex) << ": " << 9999 
            << " Error: Illegal opcode; treated as 9999" << endl;
    } else { 
        int instr = parseInstruction(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;
        if (operand > ModSizeMap[moduleCt]) {
            cout << getOrder(useIndex) << ": "
                << getInstr(opcode * 1000 + baseAddList[moduleCt]) 
                << " Error: Relative address exceeds module size; zero used" <<	endl;
        } else {
            cout << getOrder(useIndex) << ": " 
                << getInstr(instr + baseAddList[moduleCt]) << endl;
        }
    }
}

void printWarning() {
    cout << endl;
    int size = symbolTable.size();
    for (int i=0; i< size; i++ ) {
        // if does not find
        string symbol = symbolTable[i];
        if (find(useSymList.begin(), useSymList.end(), symbol) 
                == useSymList.end()){
            printf("Warning: Module %d : %s %s\n",
                    defMap[symbol] + 1, symbol.c_str(), 
                    "was defined but never used");
        }
    }	
}

void printModuleWarning(vector<string>& useList, int moduleCt) {
    int size = useList.size();
    for (int i=0; i<size; i++) {
        string token = useList[i];
        if ( useCtMap[token] == 0) {
            printf("Warning: Module %d : %s %s\n",
                    moduleCt + 1, token.c_str(),
                    "appeared in the uselist but was not actually used");
        }
    }
}

bool is_too_long(int tokenIndex) {
    string token = tokenVec[tokenIndex];
    if (token.length() > 16) {
        printf("Parse Error line %d offset %d: %s\n",
                rowVec[tokenIndex], colVec[tokenIndex], ": SYM_TOLONG");
        return true;
    } else {
        return false;
    }
}

void readFile(char* argv[]) {
    ifstream infile (argv[1], ios::binary);
    if (infile.is_open()) {
        vector<int> lenList;   // Store the length of each line
        string line;
        int row = 0;
        while ( getline(infile, line)) { 
            row++;
            lenList.push_back(line.length());
            stringstream input(line);
            int col = -1;
            for (string token; input >> token;){
                col = line.find(token, col+1);
                tokenVec.push_back(token);  
                rowVec.push_back(row);
                colVec.push_back(col+1);
            }
        }
        // record end position of input file
        int endRow = row; 
        int endCol = lenList.back() + 1;	
        rowVec.push_back(endRow);
        colVec.push_back(endCol);
        infile.close();
    } else {
        cout << "Error opening file." << endl;
    }
}
