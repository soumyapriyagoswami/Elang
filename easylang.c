/* easylang.c
   Simple EasyLang interpreter in one file (C99)
   Build: gcc -std=c99 -O2 -o easylang easylang.c
   Run: ./easylang program.elang
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ---------- Lexical tokens ---------- */
typedef enum {
    T_EOF,
    T_IDENTIFIER,
    T_NUMBER,
    T_STRING,
    T_SET,      // set
    T_PRINT,    // print
    T_READ,     // read
    T_IF,       // if
    T_THEN,     // then
    T_END,      // end
    T_WHILE,    // while
    T_DO,       // do
    T_TO,       // to
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_MOD,
    T_LPAREN, T_RPAREN,
    // comparisons encoded as tokens:
    T_EQ, T_NEQ, T_GT, T_LT, T_LE, T_GE,
    T_AND,
    T_DOT,      // .
    T_NEWLINE,  // added for line breaks as statement terminators
    T_UNKNOWN
} TokenType;


typedef struct {
    TokenType type;
    char *text; // for identifiers/strings/numbers
} Token;

typedef struct {
    const char *src;
    size_t pos;
    int line;
} Lexer;

/* utility */
static char peekc(Lexer *lx) { return lx->src[lx->pos]; }
static char getc_l(Lexer *lx) {
    char c = lx->src[lx->pos];
    if (c != '\0') lx->pos++;
    if (c == '\n') lx->line++;
    return c;
}
static void skip_spaces(Lexer *lx) {
    char c;
    while ((c = peekc(lx)) != '\0') {
        if (isspace((unsigned char)c)) { getc_l(lx); continue; }
        if (c == '#') { while (peekc(lx) != '\0' && getc_l(lx) != '\n') {} continue; }
        break;
    }
}

/* returns a newly malloc'd string */
static char *substr_alloc(const char *s, size_t a, size_t b) {
    size_t len = (b > a) ? b - a : 0;
    char *r = malloc(len + 1);
    if (!r) { fprintf(stderr, "out of memory\n"); exit(1); }
    memcpy(r, s + a, len);
    r[len] = '\0';
    return r;
}

/* make token */
static Token make_token(TokenType t, const char *txt) {
    Token tk;
    tk.type = t;
    if (txt) tk.text = strdup(txt); else tk.text = NULL;
    return tk;
}

/* read string literal "..." */
static Token lex_string(Lexer *lx) {
    getc_l(lx); // skip opening quote
    size_t start = lx->pos;
    while (peekc(lx) != '\0' && peekc(lx) != '"') {
        if (peekc(lx) == '\\') { getc_l(lx); if (peekc(lx) != '\0') getc_l(lx); continue; }
        getc_l(lx);
    }
    size_t end = lx->pos;
    if (peekc(lx) == '"') getc_l(lx);
    char *s = substr_alloc(lx->src, start, end);
    Token t = make_token(T_STRING, s);
    free(s);
    return t;
}

/* read number or identifier */
static Token lex_ident_or_number(Lexer *lx) {
    size_t start = lx->pos;
    while (1) {
        char c = peekc(lx);
        if (isalnum((unsigned char)c) || c=='_') { getc_l(lx); continue; }
        if (c=='.') { getc_l(lx); continue; }
        break;
    }
    size_t end = lx->pos;
    char *s = substr_alloc(lx->src, start, end);
    int numeric = 1;
    int dots = 0;
    for (size_t i=0;i<strlen(s);++i) {
        if (s[i]=='.') { dots++; if (dots>1) numeric=0; continue; }
        if (!isdigit((unsigned char)s[i])) numeric=0;
    }
    if (numeric) { Token t = make_token(T_NUMBER, s); free(s); return t; }
    else {
        for (char *p=s; *p; ++p) *p = (char)tolower((unsigned char)*p);
        Token t = make_token(T_IDENTIFIER, s);
        free(s);
        return t;
    }
}

static Token next_token(Lexer *lx) {
    char c;

    while ((c = peekc(lx)) != '\0') {
        if (c == ' ' || c == '\t') { getc_l(lx); continue; }

        if (c == '\r') {        // Windows line ending
            getc_l(lx);
            if (peekc(lx) == '\n') getc_l(lx);
            return make_token(T_NEWLINE, NULL);
        }

        if (c == '\n') {        // Unix line ending
            getc_l(lx);
            return make_token(T_NEWLINE, NULL);
        }

        if (c == '#') {         // comment
            while (peekc(lx) != '\0' && getc_l(lx) != '\n') {}
            continue;
        }

        break;
    }

    c = peekc(lx);
    if (c == '\0') return make_token(T_EOF, NULL);
    if (c == '"') return lex_string(lx);
    if (isalpha((unsigned char)c) || c=='_') {
        Token t = lex_ident_or_number(lx);
        if (t.type == T_IDENTIFIER && t.text) {
            // map keywords
            if (strcmp(t.text,"set")==0) { free(t.text); return make_token(T_SET,NULL); }
            if (strcmp(t.text,"print")==0) { free(t.text); return make_token(T_PRINT,NULL); }
            if (strcmp(t.text,"read")==0) { free(t.text); return make_token(T_READ,NULL); }
            if (strcmp(t.text,"if")==0) { free(t.text); return make_token(T_IF,NULL); }
            if (strcmp(t.text,"then")==0) { free(t.text); return make_token(T_THEN,NULL); }
            if (strcmp(t.text,"end")==0) { free(t.text); return make_token(T_END,NULL); }
            if (strcmp(t.text,"while")==0) { free(t.text); return make_token(T_WHILE,NULL); }
            if (strcmp(t.text,"do")==0) { free(t.text); return make_token(T_DO,NULL); }
            if (strcmp(t.text,"to")==0) { free(t.text); return make_token(T_TO,NULL); }
            if (strcmp(t.text,"and")==0) { free(t.text); return make_token(T_AND,NULL); }
        }
        return t;
    }

    if (isdigit((unsigned char)c) || c=='.') return lex_ident_or_number(lx);

    if (c == '.') { getc_l(lx); return make_token(T_DOT,NULL); }
    if (c == '(') { getc_l(lx); return make_token(T_LPAREN,NULL); }
    if (c == ')') { getc_l(lx); return make_token(T_RPAREN,NULL); }

    if (c == '+') { getc_l(lx); return make_token(T_PLUS,NULL); }
    if (c == '-') { getc_l(lx); return make_token(T_MINUS,NULL); }
    if (c == '*') { getc_l(lx); return make_token(T_MUL,NULL); }
    if (c == '/') { getc_l(lx); return make_token(T_DIV,NULL); }
    if (c == '%') { getc_l(lx); return make_token(T_MOD,NULL); }
    if (c == '<') {
        getc_l(lx);
        if (peekc(lx) == '=') {
            getc_l(lx);
            return make_token(T_LE, NULL);
        } else {
            return make_token(T_LT, NULL);
        }
    }
    if (c == '>') {
        getc_l(lx);
        if (peekc(lx) == '=') {
            getc_l(lx);
            return make_token(T_GE, NULL);
        } else {
            return make_token(T_GT, NULL);
        }
    }
    if (c == '=') {
        getc_l(lx);
        if (peekc(lx) == '=') {
            getc_l(lx);
            return make_token(T_EQ, NULL);
        } else {
            lx->pos--;
            getc_l(lx);
            return make_token(T_UNKNOWN, NULL);
        }
    }
    if (c == '!') {
        getc_l(lx);
        if (peekc(lx) == '=') {
            getc_l(lx);
            return make_token(T_NEQ, NULL);
        } else {
            lx->pos--;
            getc_l(lx);
            return make_token(T_UNKNOWN, NULL);
        }
    }

    getc_l(lx);
    return make_token(T_UNKNOWN,NULL);
}




/* ---------- AST and Parser ---------- */

typedef enum { VAL_NUM, VAL_STR, VAL_NONE } ValueType;
typedef struct { ValueType type; double num; char *str; } Value;

typedef enum {
    N_STMT_LIST, N_STMT_SET, N_STMT_PRINT, N_STMT_READ, N_STMT_IF, N_STMT_WHILE,
    N_EXPR_BINARY, N_EXPR_NUMBER, N_EXPR_STRING, N_EXPR_VAR
} NodeType;

typedef struct Node {
    NodeType type;
    char *name;
    struct Node *left;
    struct Node *right;
    struct Node *cond;
    struct Node *body;
    struct Node *else_body;
    struct Node *next;
    double number;
    char *string;
} Node;

static Node *node_alloc(NodeType t) { Node *n=calloc(1,sizeof(Node)); n->type=t; return n; }

typedef struct { Lexer lx; Token cur; } Parser;
static void token_free(Token *t) { if(!t) return; if(t->text) free(t->text); t->text=NULL; }
static Token peek_token(Parser *p) { return p->cur; }
static void advance(Parser *p) { token_free(&p->cur); p->cur = next_token(&p->lx); }
static int accept(Parser *p, TokenType t) { if(peek_token(p).type==t){advance(p); return 1;} return 0;}
static void expect(Parser *p, TokenType t, const char *msg) { if(peek_token(p).type==t){advance(p);return;} fprintf(stderr,"Parse error: expected %s but found token %d\n",msg,peek_token(p).type); exit(1); }

static Node *parse_statement(Parser *p);
static Node *parse_statements(Parser *p);
static Node *parse_expression(Parser *p);
static Node *parse_term(Parser *p);
static Node *parse_factor(Parser *p);
static Node *parse_compare(Parser *p);

static void expect_stmt_terminator(Parser *p) {
    Token t = peek_token(p);
    if (t.type == T_DOT || t.type == T_NEWLINE) {
        advance(p);  // consume the terminator
    } else if (t.type == T_SET || t.type == T_PRINT || t.type == T_READ || t.type == T_IF || t.type == T_WHILE || t.type == T_END || t.type == T_EOF ||
               (t.type == T_IDENTIFIER && t.text && strcmp(t.text, "else") == 0)) {
        // allow statement to end implicitly
    } else {
        fprintf(stderr,
            "Parse error: expected '.' or newline after statement but found token %d ('%s')\n",
            t.type, t.text ? t.text : "");
        exit(1);
    }
}

static Node *parse_factor(Parser *p) {
    Token tk = peek_token(p);
    if (tk.type == T_NUMBER) {
        Node *n = node_alloc(N_EXPR_NUMBER);
        n->number = atof(tk.text);
        advance(p);
        return n;
    } else if (tk.type == T_STRING) {
        Node *n = node_alloc(N_EXPR_STRING);
        n->string = strdup(tk.text);
        advance(p);
        return n;
    } else if (tk.type == T_IDENTIFIER) {
        Node *n = node_alloc(N_EXPR_VAR);
        n->name = strdup(tk.text);
        advance(p);
        return n;
    } else if (tk.type == T_LPAREN) {
        advance(p);
        Node *n = parse_expression(p);
        expect(p, T_RPAREN, ")");
        return n;
    } else if (tk.type == T_MINUS) {
        advance(p);
        Node *right = parse_factor(p);
        Node *n = node_alloc(N_EXPR_BINARY);
        n->left = node_alloc(N_EXPR_NUMBER);
        n->left->number = 0;
        n->right = right;
        n->number = T_MINUS;
        return n;
    }
    fprintf(stderr, "Parse error: unexpected token in factor\n");
    exit(1);
}

static Node *parse_term(Parser *p) {
    Node *left = parse_factor(p);
    while (1) {
        Token tk = peek_token(p);
        if (tk.type == T_MUL) {
            advance(p);
            Node *right = parse_factor(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = T_MUL;
            left = n;
        } else if (tk.type == T_DIV) {
            advance(p);
            Node *right = parse_factor(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = T_DIV;
            left = n;
        } else if (tk.type == T_MOD) {
            advance(p);
            Node *right = parse_factor(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = T_MOD;
            left = n;
        } else {
            break;
        }
    }
    return left;
}

static Node *parse_expression(Parser *p) {
    Node *left = parse_term(p);
    while (1) {
        Token tk = peek_token(p);
        if (tk.type == T_PLUS) {
            advance(p);
            Node *right = parse_term(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = T_PLUS;
            left = n;
        } else if (tk.type == T_MINUS) {
            advance(p);
            Node *right = parse_term(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = T_MINUS;
            left = n;
        } else {
            break;
        }
    }
    return left;
}

static Node *parse_statements(Parser *p) {
    Node *head = NULL;
    Node **tail = &head;

    while (1) {
        Token t = peek_token(p);
        if (t.type == T_EOF || t.type == T_END || t.type == T_THEN || t.type == T_DO ||
            (t.type == T_IDENTIFIER && t.text && strcmp(t.text, "else") == 0)) break;

        // Skip empty lines
        while (t.type == T_NEWLINE) {
            advance(p);
            t = peek_token(p);
        }

        Node *stmt = parse_statement(p);
        if (!stmt) break;

        *tail = stmt;
        tail = &stmt->next;
    }

    Node *list = node_alloc(N_STMT_LIST);
    list->body = head;
    return list;
}



static Node *parse_compare(Parser *p) {
    Node *node = parse_expression(p);
    Token tk = peek_token(p);

    TokenType comp = T_UNKNOWN;
    if (tk.type == T_LT) { comp = T_LT; advance(p); }
    else if (tk.type == T_LE) { comp = T_LE; advance(p); }
    else if (tk.type == T_GT) { comp = T_GT; advance(p); }
    else if (tk.type == T_GE) { comp = T_GE; advance(p); }
    else if (tk.type == T_EQ) { comp = T_EQ; advance(p); }
    else if (tk.type == T_NEQ) { comp = T_NEQ; advance(p); }

    if (comp != T_UNKNOWN) {
        Node *right = parse_expression(p);
        Node *bin = node_alloc(N_EXPR_BINARY);
        bin->left = node;
        bin->right = right;
        bin->number = comp;
        node = bin;
    }

    while (peek_token(p).type == T_AND) {
        advance(p);
        Node *right = parse_compare(p);
        Node *bin = node_alloc(N_EXPR_BINARY);
        bin->left = node;
        bin->right = right;
        bin->number = T_AND;
        node = bin;
    }

    return node;
}


static Node *parse_statement(Parser *p) {
    // skip leading newlines
    while (peek_token(p).type == T_NEWLINE) advance(p);

    Token tk = peek_token(p);

    if (tk.type == T_SET) {
        advance(p);
        if (peek_token(p).type != T_IDENTIFIER) {
            fprintf(stderr,"Parse error: expected identifier after 'set'\n"); exit(1);
        }
        char *name = strdup(peek_token(p).text);
        advance(p);

        if (peek_token(p).type == T_TO) advance(p);
        else { fprintf(stderr,"Parse error: expected 'to'\n"); exit(1); }

        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_SET);
        n->name = name;
        n->body = expr;
        return n;
    }

    else if (tk.type == T_PRINT) {
        advance(p);
        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_PRINT);
        n->body = expr;
        return n;
    }

    else if (tk.type == T_READ) {
        advance(p);
        if (peek_token(p).type != T_IDENTIFIER) {
            fprintf(stderr,"Parse error: expected identifier after 'read'\n"); exit(1);
        }
        char *name = strdup(peek_token(p).text);
        advance(p);
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_READ);
        n->name = name;
        return n;
    }

    else if (tk.type == T_IF) {
        advance(p);
        Node *cond = parse_compare(p);
        if (peek_token(p).type == T_THEN) advance(p);

        Node *stmts = parse_statements(p);
        Node *else_body = NULL;
        Token t = peek_token(p);
        if (t.type == T_IDENTIFIER && t.text && strcmp(t.text, "else") == 0) {
            advance(p);
            else_body = parse_statements(p);
        }
        expect(p, T_END, "'end' to close if");
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_IF);
        n->cond = cond;
        n->body = stmts;
        n->else_body = else_body;
        return n;
    }

    else if (tk.type == T_WHILE) {
        advance(p);
        Node *cond = parse_compare(p);
        if (peek_token(p).type == T_DO) advance(p);

        Node *stmts = parse_statements(p);
        expect(p, T_END, "'end' to close while");
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_WHILE);
        n->cond = cond;
        n->body = stmts;
        return n;
    }

    else if (tk.type == T_DOT) { advance(p); return NULL; }
    else if (tk.type == T_EOF) return NULL;

    else {
        // bare expression statement
        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);

        Node *n = node_alloc(N_STMT_PRINT);
        n->body = expr;
        return n;
    }
}


/* ---------- Evaluation ---------- */
typedef struct Var { char *name; Value val; struct Var *next; } Var;
static Var *vars=NULL;
static Var *var_get(const char *name){
    for(Var *v=vars;v;v=v->next) if(strcmp(v->name,name)==0) return v;
    return NULL;
}
static void var_set(const char *name, Value val){
    Var *v=var_get(name);
    if(v){
        if(v->val.type==VAL_STR && v->val.str) free(v->val.str);
        v->val = val;
        return;
    }
    Var *n=malloc(sizeof(Var));
    n->name=strdup(name);
    n->val=val;
    n->next=vars;
    vars=n;
}
static Value eval_expr(Node *n);

static void eval_stmt(Node *n){
    if(!n) return;
    switch(n->type){
        case N_STMT_LIST: { Node *c=n->body; while(c){eval_stmt(c); c=c->next;} break; }
        case N_STMT_SET: { Value v=eval_expr(n->body); var_set(n->name,v); break; }
        case N_STMT_PRINT: { Value v=eval_expr(n->body);
            if(v.type==VAL_NUM) printf("%g\n",v.num);
            else if(v.type==VAL_STR) printf("%s\n",v.str);
            break;
        }
        case N_STMT_READ: { 
            double val;
            if (scanf("%lf", &val) != 1) { fprintf(stderr, "Input error\n"); exit(1); }
            var_set(n->name, (Value){VAL_NUM, val, NULL});
            break;
        }
        case N_STMT_IF: { Value v=eval_expr(n->cond); 
            if(v.num!=0.0) eval_stmt(n->body); 
            else if (n->else_body) eval_stmt(n->else_body); 
            break; 
        }
        case N_STMT_WHILE: { while(eval_expr(n->cond).num!=0.0) eval_stmt(n->body); break; }
        default: break;
    }
}

static Value eval_expr(Node *n){
    if(!n) return (Value){VAL_NONE,0,NULL};
    switch(n->type){
        case N_EXPR_NUMBER: return (Value){VAL_NUM,n->number,NULL};
        case N_EXPR_STRING: return (Value){VAL_STR,0,n->string};
        case N_EXPR_VAR: { Var *v=var_get(n->name); if(v) return v->val; return (Value){VAL_NONE,0,NULL}; }
        case N_EXPR_BINARY: {
            Value l=eval_expr(n->left);
            Value r=eval_expr(n->right);
            switch((TokenType)(int)n->number){
                case T_PLUS: {
                    if (l.type == VAL_STR || r.type == VAL_STR) {
                        char lbuf[64], rbuf[64];
                        const char *lstr = l.type == VAL_STR ? l.str : (sprintf(lbuf, "%g", l.num), lbuf);
                        const char *rstr = r.type == VAL_STR ? r.str : (sprintf(rbuf, "%g", r.num), rbuf);
                        size_t len = strlen(lstr) + strlen(rstr) + 1;
                        char *res = malloc(len);
                        if (!res) exit(1);
                        strcpy(res, lstr);
                        strcat(res, rstr);
                        return (Value){VAL_STR, 0, res};
                    } else {
                        return (Value){VAL_NUM,l.num+r.num,NULL};
                    }
                }
                case T_MINUS: return (Value){VAL_NUM,l.num-r.num,NULL};
                case T_MUL: return (Value){VAL_NUM,l.num*r.num,NULL};
                case T_DIV: return (Value){VAL_NUM,l.num/r.num,NULL};
                case T_MOD: return (Value){VAL_NUM,fmod(l.num, r.num),NULL};
                case T_EQ: return (Value){VAL_NUM,l.num==r.num?1:0,NULL};
                case T_NEQ: return (Value){VAL_NUM,l.num!=r.num?1:0,NULL};
                case T_GT: return (Value){VAL_NUM,l.num>r.num?1:0,NULL};
                case T_LT: return (Value){VAL_NUM,l.num<r.num?1:0,NULL};
                case T_LE: return (Value){VAL_NUM,l.num<=r.num?1:0,NULL};
                case T_GE: return (Value){VAL_NUM,l.num>=r.num?1:0,NULL};
                case T_AND: return (Value){VAL_NUM, (l.num != 0.0 && r.num != 0.0) ? 1.0 : 0.0, NULL};
                default: return (Value){VAL_NONE,0,NULL};
            }
        }
        default: return (Value){VAL_NONE,0,NULL};
    }
}

/* ---------- Main ---------- */
int main(int argc, char **argv){
    if(argc<2){ fprintf(stderr,"Usage: %s file.elang\n",argv[0]); return 1; }
    FILE *f=fopen(argv[1],"rb"); if(!f){ perror("fopen"); return 1; }
    fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET);
    char *src=malloc(sz+1); fread(src,1,sz,f); src[sz]=0; fclose(f);
    Parser p={.lx={.src=src,.pos=0,.line=1}};
    advance(&p); // initialize first token
    Node *ast=parse_statements(&p);
    eval_stmt(ast);
    free(src);
    return 0;
}