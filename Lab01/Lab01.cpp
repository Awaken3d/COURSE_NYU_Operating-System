/* File: Lab01.cpp
 * ---------------------------------
 * This program implements a linker.
 * 
 * By           : Jingxin Zhu
 * Last Modified: 06/11/2014
 * ---------------------------------
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <algorithm>

using namespace std;

struct DefList{
    int defcnt;   // number of defined symbols in definition list
    vector<string> defSymList;   // symbols of the form [a-Z][a-Z0-9]*
    vector<int> defAddrList;     // address
};

struct ProgramText{
    int codecnt;   // number of instructions in program text
    vector<string> typeList;    // instruction type of I, A, E,or R
    vector<string> instrList;   // 4-digit instruction
};

void readFile(char* argv[]);
bool parseDefList(vector<DefList>& defListVec); 
bool parseUseList(vector< vector<string> >& useListVec);
bool parseProgramText(int& programSum, vector<ProgramText>& progListVec);
void E_instruction(string token, int useIndex, vector<string>& useList);
void R_instruction(string token, int useIndex, int moduleSize, int baseAddr);
void A_instruction(string token, int useIndex); 
void I_instruction(string token, int useIndex); 
void printSymbolTable(vector<DefList>& defListVec, vector<int>& baseAddList);
void printWarning(vector< vector<string> >& useListVec, vector<DefList>& defListVec);
void printModuleWarning(vector<string>& useList, int moduleCt);
void printDefWarning(vector<DefList>& defListVec, const vector<ProgramText>& progListVec);
void printSyntaxErr(int index, int kind);

vector<string> tokenVec;      // all tokens 
vector<int> rowVec;           // row number of each token
vector<int> colVec;           // column number of each token 
map<string, int> defMap;      // <defSym, moduleCt>
map<string, int> defSymMap;   // <defSym, OccurCt>
map<string, int> symValueMap; //2 <symbol, value>
map<string, int> useCtMap;    // <useSym, OccurCt>
int tokenIndex = 0;  // index of token under process
int tokenNum;        // Number of total tokens in the file

int main (int argc, char* argv[]){

    if (argc != 2) {
        cout << "Unexpected input format." << endl;
        exit(0);
    }
    readFile(argv);

    vector<DefList> defListVec;
    vector< vector<string> > useListVec;
    vector<ProgramText> progListVec;

    /*       Pass One        */
    int programSum = 0; // sum of programs in all modules
    tokenNum = tokenVec.size(); // number of all tokens
    vector<int> baseAddList;
    baseAddList.push_back(programSum);
    while( tokenIndex < tokenNum ) {
        if( !parseDefList(defListVec) ) {
            return 1;
        }
        if ( !parseUseList(useListVec) ) {
            return 1;
        }	
        if ( !parseProgramText(programSum, progListVec) ) {
            return 1;
        }	
        tokenIndex++;
        baseAddList.push_back(programSum);
    }
    // print warning if symbol's address is too large
    printDefWarning(defListVec, progListVec);

    printSymbolTable(defListVec, baseAddList);

    /*       Pass Two       */
    int moduleCt = 0;   // counter for nudule number
    int moduleNum = defListVec.size();
    int useIndex = 0;
    cout << "Memory Map" << endl;
    while( moduleCt < moduleNum ) {
        vector<string> useList = useListVec[moduleCt]; 
        int size = useList.size();
        for (int i = 0; i < size; i++){
            map<string, int>::const_iterator iter = useCtMap.find( useList[i] );
            if ( iter == useCtMap.end()) {
                useCtMap.insert(make_pair(useList[i], 0));
            }
        }

        for (int i = 0; i < progListVec[moduleCt].codecnt; i++) {
            string type = progListVec[moduleCt].typeList[i];
            string instr = progListVec[moduleCt].instrList[i];
            if (type == "A")  	
                A_instruction(instr, useIndex);
            if (type == "I")  	
                I_instruction(instr, useIndex);
            if (type == "E")
                E_instruction(instr, useIndex, useList);
            if (type == "R")
                R_instruction(instr, useIndex, progListVec[moduleCt].codecnt, baseAddList[moduleCt]);
            useIndex++;
        }
        printModuleWarning(useList, moduleCt);
        moduleCt++;
    }
    // print warning if a symbol has been defined, but never used. 
    printWarning(useListVec, defListVec);

    return 0;
}


/* --------     Private Functions   -------- */

/* Return true if this token exists and stands for a valud number */
bool isNumber(int tokenIndex) {
    // Is there a token at given index?
    if (tokenIndex == tokenNum) {
        return false;
    }
    // Does this token stand for a valid number?
    string token = tokenVec[tokenIndex];
    int len = token.length();
    for (int i = 0; i < len; i++) {
        char ch = token[i];
        if ( ch < '0' || ch > '9') {
            return false;
        }
    }
    return true;
}

/* Return true if this token exists and in the form of [a-Z][a-Z0-9]* */
bool isSymbol(int tokenIndex) {
    if ( tokenIndex == tokenNum ) {
        return false;
    } 
    // Is this symbol in form of [a-Z][a-Z0-9]* ?
    string token = tokenVec[tokenIndex];
    char ch = token[0];
    if ( !isalpha(ch) ) {
        return false;
    }
    int len = token.length();
    for (int i = 1; i < len; i++) {
        ch = token[i];
        if ( !isalnum(ch) ) return false;
    }
    return true;		
}

/* Return true if this token exists and is an instruction type */
bool isType(int tokenIndex) {
    if (tokenIndex == tokenNum){
        return false;
    } 
    string token = tokenVec[tokenIndex];
    return (token == "I") || (token =="A") 
        || (token=="E") || (token=="R");
}

/* Print warning if a symbol's address is larger than 
 * the number of instructions in that def list
 */
void printDefWarning(vector<DefList>& defListVec, const vector<ProgramText>& progListVec) { 
    int nModule = defListVec.size();
    DefList* dl;
    vector<string> defShowUp;   // store symbols that have been tested so far
    for (int i = 0; i < nModule; i++) {
        dl = &defListVec[i];
        int num = dl->defcnt;
        // compare each address in the def list with maximum
        for (int j = 0; j < num; j++) {
            string token = dl->defSymList[j];
            int address = dl->defAddrList[j];
            if (find(defShowUp.begin(), defShowUp.end(), token) == defShowUp.end()) {
                if (address >= progListVec[i].codecnt) {
                    printf("Warning: Module %d: %s to big %d (max=%d) assume zero relative\n",
                            i+1, token.c_str(), address, progListVec[i].codecnt - 1);
                    dl->defAddrList[j] = 0;
                }
            defShowUp.push_back(token);
            }
        }
    }
}

void printSymbolTable(vector<DefList>& defListVec, vector<int>& baseAddList) {
    cout << "Symbol Table" << endl;
    DefList dl;
    vector<string> symShowUp;
    int nModule = defListVec.size();
    for (int i = 0; i < nModule; i++) {
        dl = defListVec[i];
        if ( dl.defcnt != 0 ) {
            int symsize = dl.defSymList.size();
            for (int j = 0; j < symsize; j++) {
                string token = dl.defSymList[j];
                int value = baseAddList[defMap[token]] + dl.defAddrList[j];
                symValueMap.insert(make_pair(token, value));
                if (find(symShowUp.begin(), symShowUp.end(), token) == symShowUp.end()) {
                    cout << token << "=" << value;
                    // if symbol is multiply defined, print warning and first value is used
                    if ( defSymMap[token] > 1) {
                        cout << " Error: This variable is multiple times defined; first value used";
                    }
                    cout <<endl;
                symShowUp.push_back(token);
                }
            }
        }
    }
    cout << endl;
}

/* Convert a 4-digit token into an instruction */ 
int parseInstr(string token) {
    int len = token.length();
    if ( len > 4 ) {
        // if instruction's length > 4, treat as 9999
        return 9999;
    }
    return atoi(token.c_str());
}

/* Convert an integer into a nDigits-digit string */
string numToString(int num, int nDigits) {
    string result;
    string s;
    stringstream ss;
    ss << num;
    ss >> s;
    int len = s.length();
    for (int i = 0; i < (nDigits- len); i++) {
        result += "0";
    }
    result += s;
    return result;
}

/* Convert an integer into an instruction, which is 4-digit string */
string getInstr(int number, int strLen) {
    return numToString(number, strLen);
}

string getOrder(int index, int strLen) {
    return numToString(index, strLen);
}

void printWarning(vector< vector<string> >& useListVec, vector<DefList>& defListVec) {
    cout << endl;
    // Put all non-repeated symbols that appear in def list into a new vector
    vector<string> defSymbols; 
    int nModule = defListVec.size();
    for (int i = 0; i < nModule; i++) {
        int nDef = defListVec[i].defcnt;
        for (int j = 0; j < nDef; j++) {
            string symbol = defListVec[i].defSymList[j]; 
            if ( find(defSymbols.begin(), defSymbols.end(), symbol) == defSymbols.end()) {
                defSymbols.push_back(symbol);
            }
        }
    }
    int nSymbol = defSymbols.size();
    for (int i = 0; i < nSymbol; i++ ) {
        bool found = false;
        string symbol = defSymbols[i];
        int useNum = useListVec.size();
        for (int j = 0; j < useNum; j++) {
            vector<string> useList = useListVec[j];   
            if (find(useList.begin(), useList.end(), symbol) 
                 != useList.end()){
                found = true;
                break;
            }
        }
        if ( !found ) {
            printf("Warning: Module %d: %s %s\n",
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
            printf("Warning: Module %d: %s %s\n",
                    moduleCt + 1, token.c_str(),
                    "appeared in the uselist but was not actually used");
        }
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

void E_instruction(string token, int useIndex, vector<string>& useList) {
    string instruction;
    string err = "";
    int len = token.length();
    if (len > 4) {
        instruction = "9999";
        err = " Error: Illegal opcode; treated as 9999";
    } else { 
        int instr = parseInstr(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;	
        // If the symbol is in the use list but not defined in the symbol table
        int size = useList.size();
        if (operand > size - 1) {
            instruction = getInstr(instr, 4);
            err = " Error: External address exceeds length of uselist; treated as immediate";
        } else { 
            string token = useList[operand];
            map<string, int>::const_iterator iter = defSymMap.find(token);
            operand = symValueMap[token];	
            if ( iter == defSymMap.end() ) {
                instruction = getInstr(opcode * 1000, 4);
                err = " Error: " + token + " is not defined; zero used";
                useCtMap[token] += 1;
            } else {
                useCtMap[token] += 1; 
                instruction = getInstr(opcode * 1000 + operand, 4);
            }
        }
    }
    cout << getOrder(useIndex, 3) << ": " << instruction
        << err << endl;
}

void A_instruction(string token, int useIndex) {
    string instruction;
    string err;
    if (token.length() > 4) {
        instruction = "9999";
        err = " Error: Illegal opcode; treated as 9999";
    } else { 
        int instr = parseInstr(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;
        if ( operand > 511 ) {
            instruction = getInstr(opcode * 1000, 4);
            err = " Error: Absolute address exceeds machine size; zero used";
        } else {
            instruction = getInstr(instr, 4);
            err = "";
        }
    }
    cout << getOrder(useIndex, 3) << ": " << instruction 
        << err << endl;
}

void I_instruction(string token, int useIndex) {
    if ( token.length() > 4 ) {
        cout << getOrder(useIndex, 3) << ": " << 9999 << 
            " Error: Illegal immediate value; treated as 9999" << endl;	
    } else {
        int instr = parseInstr(token);
        cout << getOrder(useIndex, 3) << ": " << getInstr(instr, 4) << endl;
    }
}

void R_instruction(string token, int useIndex, int moduleSize,int baseAddr) {
    string instruction;
    string err = "";
    if (token.length() > 4) {
        err = " Error: Illegal opcode; treated as 9999";
        instruction = "9999";
    } else {
        int instr = parseInstr(token);
        int opcode = instr / 1000;
        int operand = instr % 1000;
        if (operand > moduleSize) {
            instruction = getInstr(opcode * 1000 + baseAddr, 4);
            err = " Error: Relative address exceeds module size; zero used";
        } else {
            instruction = getInstr(instr + baseAddr, 4);
        }
    }
    cout << getOrder(useIndex, 3) << ": " << instruction 
        << err << endl;
}

bool parseDefList(vector<DefList>& defListVec) { 
    // test def counter: is defCnt a number? 
    if ( !isNumber(tokenIndex) ){
        printSyntaxErr(tokenIndex, 1);
        return false;
    }
    // test def counter: is defCnt < MAX_DEF_NUM (16)?
    int defCnt = atoi(tokenVec[tokenIndex].c_str());
    if (defCnt > 16) {
        printSyntaxErr(tokenIndex, 2); // TO_MANY_DEF_IN_MODULE: 
        return false;
    } 

    DefList deflist;
    deflist.defcnt = defCnt;

    // test all symbols and their addresses in this def list
    for (int i = 0; i < defCnt; i++) {		
        tokenIndex++;
        // test def symbol: is defsym a valid symbol?
        if( !isSymbol(tokenIndex) ) {
            printSyntaxErr(tokenIndex, 3);
            return false;
        } 
        // test def symbol: is defsym too long? (length > 16)
        if (tokenVec[tokenIndex].length() > 16) {
            printSyntaxErr(tokenIndex, 5);
            return false;
        }

        string token = tokenVec[tokenIndex];
        deflist.defSymList.push_back(token);

        // store the module number of each def symbol
        if (defMap.find(token) == defMap.end()) {
            int moduleCt = defListVec.size();
            defMap.insert(make_pair(token, moduleCt));
        }
        // store the times that one symbol occurs 
        defSymMap[token] += 1;

        tokenIndex++;
        // test address: is address a valid number? 
        if ( !isNumber(tokenIndex) ) {
            printSyntaxErr(tokenIndex, 1);
            return false;
        } 
        deflist.defAddrList.push_back(atoi(tokenVec[tokenIndex].c_str()));
    } // for-loop ends here
    defListVec.push_back(deflist);
    return true;
}

bool parseUseList(vector< vector<string> >& useListVec){
    vector<string> useList;
    tokenIndex++;
    // test uselist number: is useCnt a valid number? 
    if (!isNumber(tokenIndex)){
        printSyntaxErr(tokenIndex, 1);
        return false;
    }
    int useCnt = atoi(tokenVec[tokenIndex].c_str());
    if (useCnt > 16) {
        printSyntaxErr(tokenIndex, 4); // TO_MANY_USE_IN_MODULE
        return false;
    }
    for (int i = 0; i < useCnt; i++) {		
        tokenIndex++;
        if( !isSymbol(tokenIndex) ) {
            // test use symbol: is a valid symbol?
            printSyntaxErr(tokenIndex, 3); // SYM_EXPECTED
            return false;
        }
        if ( tokenVec[tokenIndex].length() > 16 ) {
            printSyntaxErr(tokenIndex, 5); // SYM_TOLONG
            return false;
        }
        useList.push_back(tokenVec[tokenIndex]);
    }
    useListVec.push_back( useList );
    return true;
}

bool parseProgramText(int& programSum, vector<ProgramText>& progListVec) {
    tokenIndex++;
    ProgramText pt;
    if (!isNumber(tokenIndex)){
        // test progCnt: is progCnt is a valid number?
        printSyntaxErr(tokenIndex, 1);
        return false;
    } 
    int progCt= atoi(tokenVec[tokenIndex].c_str());
    if ( progCt > (512 - programSum)) {
        printSyntaxErr(tokenIndex, 6);
        return false;
    }
    programSum += progCt;
    pt.codecnt = progCt;
    for (int i = 0; i < progCt; i++) { 
        tokenIndex++;
        // Miss token or Unexpected token
        if( !isType(tokenIndex) ) {
            printSyntaxErr(tokenIndex, 7); // ADDR_EXPECTED
            return false;
        }
        pt.typeList.push_back(tokenVec[tokenIndex]);
        tokenIndex++;	
        if ( !isNumber(tokenIndex)) {
            printSyntaxErr(tokenIndex, 1);
            return false;
        }
        pt.instrList.push_back(tokenVec[tokenIndex]);
    }	
    progListVec.push_back(pt);
    return true;
}

void printSyntaxErr(int index, int kind) {
    string err;
    if (kind == 1) {
        err = "NUM_EXPECTED";
    } else if (kind == 2) {
        err = "TO_MANY_DEF_IN_MODULE";
    } else if (kind == 3) {
        err = "SYM_EXPECTED";
    } else if (kind == 4) {
        err = "TO_MANY_USE_IN_MODULE";
    } else if (kind == 5) {
        err = "SYM_TOLONG";
    } else if (kind == 6) {
        err = "TO_MANY_INSTR";
    } else {
        err = "ADDR_EXPECTED";
    }
    printf("Parse Error line %d offset %d: %s\n",
        rowVec[index], colVec[index], err.c_str());
}
