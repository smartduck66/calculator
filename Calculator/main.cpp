//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
// Commit initial : 10 décembre 2017
// Commit en cours : 1er janvier 2018 - Version modifiée du chapitre 7 - Avant exo 7 du Drill page 251
// Caractères spéciaux : [ ]   '\n'   {  }   ||

// Chemin pour VSCODE
// #include "../../std_lib_facilities.h"

// Chemin pour l'IDE XCODE
#include "std_lib_facilities.h"

// Définition de constantes symboliques pour clarifier le code
const char number='8';      // "a value" dans la classe Token
const char quit='q';        // Quitter
const char print=';';       // "Imprimer" le résultat
const string prompt="> ";   // Demande d'une saisie
const string result="= ";   // Indique que ce qui suit est un résultat
const char name='a';        // name token
const char let='L';         // declaration token
const string declkey="let"; // declaration keyword


class Variable {            // Cette classe sert de socle à la gestion des variables dans la calculette (ex : let x=2;)
public:
    string name;
    double value;
    
    Variable(string n, double v) :name(n), value(v) { }
    
};

// ------------------------------------------------------------------------------------
// Définition des 2 fonctions-membre de la classe Variable
// ------------------------------------------------------------------------------------

vector<Variable>var_table;

// The get_value() member function returns the value of the Variable named s
double get_value(string s){
    for (const Variable& v:var_table)
        if (v.name==s) return v.value;
    error("get: undefined variable ",s);
    return 1;
}

// The set_value() member function sets the Variable named s to d
double set_value(string s, double d){
    for (Variable& v:var_table)
        if (v.name==s){
            v.value=d;
            return 0;
        }
    error("set: undefined variable ",s);
    return 1;
}


//----------------------------------------------------------------------------------

class Token {               // Cette classe sert de socle à la gestion des "Tokens"
public:
    char kind;        // what kind of token
    double value;     // for numbers: a value
    string name;
    
    // Constructeurs
    Token(char ch):kind{ch}{}                           //initialize kind with ch
    Token(char ch, double val):kind {ch},value{val}{}   //initialize kind and value
    Token (char ch, string n):kind {ch},name{n}{}       //initialize kind and name
};

//----------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();             // make a Token_stream that reads from cin
    Token get();                // get a Token (get() is defined elsewhere)
    void putback(Token t);      // put a Token back
    void ignore(char c);        // discard characters up to and including a char
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

// The constructor just sets "full" to indicate that the buffer is empty:
Token_stream::Token_stream():full(false), buffer(0)    // no Token in buffer
{
}

// ------------------------------------------------------------------------------------
// Définition des 3 fonctions-membre de la classe Token_stream
// ------------------------------------------------------------------------------------
// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // copy t to buffer
    full = true;      // buffer is now full
}

//------------------------------------------------------------------------------
void Token_stream::ignore(char c)   // c represents the kind of Token
{
    // first look in buffer
    if (full&&c==buffer.kind){
        full=false;
        return;
    
    }
    full=false;
    
    // now search input
    char ch=0;
    while (cin>>ch)
        if (ch==c)return;
}

//------------------------------------------------------------------------------
Token Token_stream::get()
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full=false;
        return buffer;
    }
    
    char ch;
    cin >> ch;    // Saisie de l'expression : un CR stoppe le processus - Note that >> skips whitespace (space, newline, tab, etc.)
    
    switch (ch) {
        case print:
        case quit:
        case '(':
        case ')':
        case '{':
        case '}':
        case '+':
        case '-':
        case '*':
        case '/':
        case '!':
        case '%':
        case '=':
            return Token(ch);        // let each character represent itself
        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            cin.putback(ch);         // put digit back into the input stream
            double val;
            cin >> val;              // read a floating-point number
            return Token{number,val};   // La constante number définie est utilisée
        }
        default:
            if (isalpha(ch)){
                string s;
                s+=ch;
                while (cin.get(ch)&&(isalpha(ch)||isdigit(ch)))s+=ch;
                cin.putback(ch);
                if (s==declkey)
                    return Token{let}; //declaration keyword
                return Token{name,s};
                
            }
            
            error("Bad token !!");
            return 1;
    }
}



//------------------------------------------------------------------------------

Token_stream ts;        // Instanciation de la classe Token_stream : provides get() and putback()

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers, parentheses et accolades
double primary()
{
    Token t = ts.get();
    switch (t.kind) {
        case '(':    // handle '(' expression ')'
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')') error("')' expected");
            return d;
        }
        
        case '{':    // handle '{' expression '}'
        {
            double d = expression();
            t = ts.get();
            if (t.kind != '}') error("'}' expected");
            return d;
        }
        
        case '-':            // Rajout de la gestion des nombres négatifs...
            return -primary();
            
        case '+':            // ... et du "+" éventuellement rajouté devant le 1er nombre saisi
            return primary();
            
        case number:            // we use '8' to represent a number
            return t.value;  // return the number's value
            
        case name:
            return get_value(t.name);
        
        default:
            error("primary expected");
            return 1;
    }
}

//------------------------------------------------------------------------------

// deal with *, /, !, %
double term()
{
    double left = primary();
    Token t = ts.get();        // get the next token from token stream
    
    while(true) {
        switch (t.kind) {
            case '*':
                left *= primary();
                t = ts.get();
                break;
            
            case '/':
            {
                double d = primary();
                if (d == 0) error("divide by zero");
                left /= d;
                t = ts.get();
                break;
            }
            
            case '%':
            {
                double d=primary();
                if (d==0) error ("%: divide by zero");
                left=fmod(left,d); // Appel de la librairie "float modulo" de la librairie cmath
                t = ts.get();
                break;
            }
                
            case '!':
            {
                // On calcule la factorielle via une boucle simple inverse sur la variable left qui contient l'entier à traiter (ex : 4!)
                // On ne sait pas le faire sur un réel, donc on convertit en int
                int factorielle=static_cast<int>(left);
                int resultat_fact=factorielle;
                
                if (factorielle==0)
                    resultat_fact=1;
                else
                    for (int i=factorielle-1;i>=1;i--)resultat_fact *=i;
                
                left = resultat_fact;
                t = ts.get();
                break;
            }
            
            default:
                ts.putback(t);     // put t back into the token stream
                return left;
        }
    }
}

//------------------------------------------------------------------------------

// deal with +, -
double expression()
{
    double left = term();      // read and evaluate a Term
    Token t = ts.get();        // get the next token from token stream
    
    while(true) {
        switch(t.kind) {
             
            case '+':
                left += term();    // evaluate Term and add
                t = ts.get();
                break;
            case '-':
                left -= term();    // evaluate Term and subtract
                t = ts.get();
                break;
            default:
                ts.putback(t);     // put t back into the token stream
                return left;       // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------
void clean_up_mess()
{
    ts.ignore(print);
}

//------------------------------------------------------------------------------
bool is_declared(string var)   // is var already in var_table ?
{
    for (const Variable& v:var_table)
        if (v.name==var) return true;
    return false;
    
}

//------------------------------------------------------------------------------
double define_name(string var,double val)   // add (var,val) to var_table
{
    if (is_declared(var)) error(var," declared twice");
    var_table.push_back(Variable{var,val});
    return val;
    
}


//------------------------------------------------------------------------------
double declaration()
// assume we have seen "let"
// handle : name = expression
// declare a variable called "name" with the initial value "expression"
{
    Token t=ts.get();
    if (t.kind!=name) error("name expected in declaration");
    string var_name=t.name;
    
    Token t2=ts.get();
    if (t2.kind!='=') error("= missing in declaration of ",var_name);
    
    double d=expression();
    define_name(var_name,d);
    return d;
    
}

//------------------------------------------------------------------------------
double statement()    // handle expressions and declarations
{
    Token t=ts.get();
    switch (t.kind){
        case let:
            return declaration();
        default:
            ts.putback(t);
            return expression();
    }
}

//------------------------------------------------------------------------------
void calculate()    // expression evaluation loop
{
    while (cin) {
    try {
        cout <<prompt;
        Token t = ts.get();
        while(t.kind==print)t=ts.get();    // eat ';'
        if (t.kind == quit)return;
        ts.putback(t);
        cout << result << statement() << '\n';
        
        // Debug : vérification des variables déclarées
        // for (Variable& v:var_table)
        //    cout << "Variable "<<v.name<<" = "<<v.value<<"\n";
        
        
    }
    catch (exception& e) {
        cerr << "error: " << e.what() << '\n';
        clean_up_mess();
        }
    }
}
    
//------------------------------------------------------------------------------
int main()
try
{
    //predefined names:
    define_name("pi",3.1415926535);
    define_name("e",2.7182818284);
    define_name("k",1000);
    
    cout << "Saisis une expression :\n";
    calculate();
    keep_window_open();
    return 0;
    
}
catch (runtime_error& e) {
    cerr << "error: " << e.what() << '\n';
    keep_window_open("@");
    return 1;
}
catch (...) {
    cerr << "Oops: unknown exception!\n";
    return 2;
}
    
//------------------------------------------------------------------------------
