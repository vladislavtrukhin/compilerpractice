#include <iostream>
#include <vector>
using namespace std;

//Token identifier
enum tokenEnums {
  T_EOF = -1,
  T_IDENTIFIER = -2,
  T_NUMBER = -3,
  T_RETURN = -4,
  T_SEMICOLON = -5,
  T_R_BRACKET = -6,
  T_L_BRACKET = -7,
  T_R_PAREN = -8,
  T_L_PAREN = -9,
};

//Class for storing token information
class Token {
  private:
    tokenEnums token;
    string id;
    double num;

  public:
    Token() {
      id = "";
      num = 0;
    }

    void print() { cout << "Token ID: " << token <<
    endl << "ID: " << id << endl << "Value: " << num << endl; }

    //Setters + getters
    void setToken(tokenEnums newToken) { token = newToken; }

    tokenEnums getToken() { return token; }

    void setId(string newId) { id = newId; }

    string getId() { return id; }

    void setNum(double newNum) { num = newNum; }

    double getNum() { return num; }
};

/*----------------------------------------------------------------
 *  LEXER
 */

//Get the next token
static Token gettok() {
  Token token;
  static int LastChar = ' ';

  //Go through file whitespace until char is reached
  while (isspace(LastChar)) LastChar = getchar();

  //String[A-Zz-a_]+
  if (isalpha(LastChar)) {
    string identifier;
    identifier = LastChar;
    while (isalnum((LastChar = getchar()))) identifier += LastChar;
    token.setId(identifier);
    if (identifier == "Return") token.setToken(T_RETURN);
    else token.setToken(T_IDENTIFIER);
    return token;
  }

  // Number: [0-9.]+
  if (isdigit(LastChar) || LastChar == '.') {
    string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');
    token.setNum(strtod(NumStr.c_str(), nullptr));
    token.setToken(T_NUMBER);
    return token;
  }
  if (LastChar == T_EOF) {
    token.setToken(T_EOF);
    token.setId("END OF FILE");
    LastChar = getchar();
    return token;
  }
  string strChar;
  strChar = LastChar;
  token.setId(strChar);

  //Additional Symbols
  if (LastChar == '{') {
    token.setToken(T_L_BRACKET);
    LastChar = getchar();
    return token;
  }
  if (LastChar == '(') {
    token.setToken(T_L_PAREN);
    LastChar = getchar();
    return token;
  }
  if (LastChar == ')') {
    token.setToken(T_R_PAREN);
    LastChar = getchar();
    return token;
  }
  if (LastChar == '}') {
    token.setToken(T_R_BRACKET);
    LastChar = getchar();
    return token;
  }
  if (LastChar == ';') {
    token.setToken(T_SEMICOLON);
    LastChar = getchar();
    return token;
  }

  //Return char if no token found
  LastChar = getchar();
  return token;
}

//Returns a vector of all the tokens in a file
static vector<Token> alltoks() {
  vector<Token> alltoks;
  Token x = gettok();
  alltoks.push_back(x);
  while (x.getToken() != T_EOF) {
    x = gettok();
    alltoks.push_back(x);
  }
  alltoks.push_back(x);
  return alltoks;
}

/*----------------------------------------------------------------
 *  AST Classes
 */

class AST {
  public:
    int id = 0;
};

class NumAST : public AST {
  public:
    int id = 5;
    double value;
};

class ExpressionAST : public AST {
  public:
    int id = 4;
    NumAST value;
};

class StatementAST : public AST {
  public:
    int id = 3;
    ExpressionAST exp;
};

class FunctionAST : public AST {
  public:
    int id = 2;
    string name;
    StatementAST body;
};

class ProgramAST : public AST {
  public:
    int id = 1;
    FunctionAST function;
};

/*----------------------------------------------------------------
 *  PARSER
 */

static Token curtok;

int accept(tokenEnums t) {
  if (curtok.getToken() == t) {
    curtok = gettok();
    return 1;
  }
  return 0;
}

int expect(tokenEnums t) {
  if (accept(t)) return 1;
  cout << "Error: unexpected symbol" << endl;
  return 0;
}

NumAST number() {
  NumAST num;
  num.value = curtok.getNum();
  return num;
}

ExpressionAST expression() {
  ExpressionAST exp;
  if (accept(T_NUMBER)) {
    exp.value = number();
  }
  return exp;
}

StatementAST statement() {
  StatementAST statement;
  if (accept(T_RETURN)) {
    statement.exp = expression();
    expect(T_SEMICOLON);
  }
  return statement;
}

FunctionAST function1() {
  FunctionAST function;
  if (accept(T_IDENTIFIER)) {
    curtok = gettok();
    function.name = curtok.getId();
    curtok = gettok();
    if (expect(T_L_PAREN) && expect(T_R_PAREN) && expect(T_L_BRACKET)) {
      function.body = statement();
      expect(T_R_BRACKET);
    }
  }
  return function;
}

ProgramAST program() {
  ProgramAST program;
  curtok = gettok();
  program.function = function1();
  expect(T_EOF);
  return program;
}

void printAST(AST root) {
  if (root.id == 0) {
    cout << "Oh no";
  }
  if (root.id == 1) {
    printAST(root.function);
  }
  if (root.id == 2) {
    printAST(root.statement);
    cout << dynamic_cast<FunctionAST*>root.name;
  }
  if (root.id == 3) {
    printAST(root.exp);
    cout << "Return ;";
  }
  if (root.id == 4) {
    printAST(root.value);
    cout << "Number ";
  }
  if (root.id == 5) {
    cout << dynamic_cast<NumAST*>(&root)->value;
  }
}

int main() {
  AST root = program();
  printAST(root);

}
