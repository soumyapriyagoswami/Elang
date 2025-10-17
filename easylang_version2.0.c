/* easylang.c
   Enhanced EasyLang interpreter with fixed recursive return handling (C99)
   Build: gcc -std=c99 -O2 -lm -o easylang easylang.c
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
    T_EQ, T_NEQ, T_GT, T_LT, T_LE, T_GE,
    T_AND,
    T_DOT,      // .
    T_NEWLINE,  // line breaks
    T_FUNCTION, // function
    T_RETURN,   // return
    T_LBRACE,   // {
    T_RBRACE,   // }
    T_COMMA,    // ,
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

/* ---------- Utility Functions ---------- */
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

static char *substr_alloc(const char *s, size_t a, size_t b) {
    size_t len = (b > a) ? b - a : 0;
    char *r = malloc(len + 1);
    if (!r) { fprintf(stderr, "out of memory\n"); exit(1); }
    memcpy(r, s + a, len);
    r[len] = '\0';
    return r;
}

static Token make_token(TokenType t, const char *txt) {
    Token tk;
    tk.type = t;
    tk.text = txt ? strdup(txt) : NULL;
    return tk;
}

/* ---------- Lexer Functions ---------- */
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
    for (size_t i=0; i<strlen(s); ++i) {
        if (s[i]=='.') { dots++; if (dots>1) numeric=0; continue; }
        if (!isdigit((unsigned char)s[i]) && s[i] != '.') numeric=0;
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
        if (c == '\r') {
            getc_l(lx);
            if (peekc(lx) == '\n') getc_l(lx);
            return make_token(T_NEWLINE, NULL);
        }
        if (c == '\n') {
            getc_l(lx);
            return make_token(T_NEWLINE, NULL);
        }
        if (c == '#') {
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
            if (strcmp(t.text, "set") == 0) { free(t.text); return make_token(T_SET, NULL); }
            if (strcmp(t.text, "print") == 0) { free(t.text); return make_token(T_PRINT, NULL); }
            if (strcmp(t.text, "read") == 0) { free(t.text); return make_token(T_READ, NULL); }
            if (strcmp(t.text, "if") == 0) { free(t.text); return make_token(T_IF, NULL); }
            if (strcmp(t.text, "then") == 0) { free(t.text); return make_token(T_THEN, NULL); }
            if (strcmp(t.text, "end") == 0) { free(t.text); return make_token(T_END, NULL); }
            if (strcmp(t.text, "while") == 0) { free(t.text); return make_token(T_WHILE, NULL); }
            if (strcmp(t.text, "do") == 0) { free(t.text); return make_token(T_DO, NULL); }
            if (strcmp(t.text, "to") == 0) { free(t.text); return make_token(T_TO, NULL); }
            if (strcmp(t.text, "and") == 0) { free(t.text); return make_token(T_AND, NULL); }
            if (strcmp(t.text, "function") == 0) { free(t.text); return make_token(T_FUNCTION, NULL); }
            if (strcmp(t.text, "return") == 0) { free(t.text); return make_token(T_RETURN, NULL); }
        }
        return t;
    }
    if (isdigit((unsigned char)c) || c=='.') return lex_ident_or_number(lx);
    if (c == '.') { getc_l(lx); return make_token(T_DOT, NULL); }
    if (c == '(') { getc_l(lx); return make_token(T_LPAREN, NULL); }
    if (c == ')') { getc_l(lx); return make_token(T_RPAREN, NULL); }
    if (c == '{') { getc_l(lx); return make_token(T_LBRACE, NULL); }
    if (c == '}') { getc_l(lx); return make_token(T_RBRACE, NULL); }
    if (c == ',') { getc_l(lx); return make_token(T_COMMA, NULL); }
    if (c == '+') { getc_l(lx); return make_token(T_PLUS, NULL); }
    if (c == '-') { getc_l(lx); return make_token(T_MINUS, NULL); }
    if (c == '*') { getc_l(lx); return make_token(T_MUL, NULL); }
    if (c == '/') { getc_l(lx); return make_token(T_DIV, NULL); }
    if (c == '%') { getc_l(lx); return make_token(T_MOD, NULL); }
    if (c == '<') {
        getc_l(lx);
        if (peekc(lx) == '=') { getc_l(lx); return make_token(T_LE, NULL); }
        return make_token(T_LT, NULL);
    }
    if (c == '>') {
        getc_l(lx);
        if (peekc(lx) == '=') { getc_l(lx); return make_token(T_GE, NULL); }
        return make_token(T_GT, NULL);
    }
    if (c == '=') {
        getc_l(lx);
        if (peekc(lx) == '=') { getc_l(lx); return make_token(T_EQ, NULL); }
        lx->pos--; getc_l(lx); return make_token(T_UNKNOWN, NULL);
    }
    if (c == '!') {
        getc_l(lx);
        if (peekc(lx) == '=') { getc_l(lx); return make_token(T_NEQ, NULL); }
        lx->pos--; getc_l(lx); return make_token(T_UNKNOWN, NULL);
    }
    getc_l(lx);
    return make_token(T_UNKNOWN, NULL);
}

/* ---------- AST and Parser ---------- */
typedef enum { VAL_NUM, VAL_STR, VAL_NONE } ValueType;
typedef struct { ValueType type; double num; char *str; } Value;

typedef enum {
    N_STMT_LIST, N_STMT_SET, N_STMT_PRINT, N_STMT_READ, N_STMT_IF, N_STMT_WHILE,
    N_STMT_FUNCDEF, N_STMT_RETURN, N_EXPR_BINARY, N_EXPR_NUMBER, N_EXPR_STRING,
    N_EXPR_VAR, N_EXPR_CALL
} NodeType;

typedef struct Node {
    NodeType type;
    char *name; // for identifiers, function names
    struct Node *left, *right; // for binary ops
    struct Node *cond, *body, *else_body; // for if/while
    struct Node *next; // for statement lists
    double number; // for numbers, token types in binary
    char *string; // for strings
    char **params; // for function params
    int param_count; // number of params
    struct Node **args; // for function call args
    int arg_count; // number of args
} Node;

typedef struct FuncDef {
    char *name;
    char **params;
    int param_count;
    Node *body;
} FuncDef;

static Node *node_alloc(NodeType t) { Node *n = calloc(1, sizeof(Node)); n->type = t; return n; }

/* ---------- Symbol Table and Functions ---------- */
typedef struct Var {
    char *name;
    Value val;
    struct Var *next;
} Var;

typedef struct Scope {
    Var *vars;
    struct Scope *parent;
} Scope;

typedef struct {
    FuncDef **funcs;
    int func_count;
} FuncTable;

static Scope *global_scope = NULL;
static Scope *current_scope = NULL;
static FuncTable func_table = {NULL, 0};

static void push_scope() {
    Scope *s = malloc(sizeof(Scope));
    s->vars = NULL;
    s->parent = current_scope;
    current_scope = s;
    if (!global_scope) global_scope = s;
}

static void pop_scope() {
    if (!current_scope) return;
    Scope *s = current_scope;
    Var *v = s->vars;
    while (v) {
        Var *next = v->next;
        if (v->val.type == VAL_STR && v->val.str) free(v->val.str);
        free(v->name);
        free(v);
        v = next;
    }
    current_scope = s->parent;
    free(s);
}

static Var *var_get(const char *name) {
    for (Scope *s = current_scope; s; s = s->parent) {
        for (Var *v = s->vars; v; v = v->next) {
            if (strcmp(v->name, name) == 0) return v;
        }
    }
    // Also check global scope if we're in some scope
    if (current_scope && current_scope != global_scope) {
        for (Var *v = global_scope->vars; v; v = v->next) {
            if (strcmp(v->name, name) == 0) return v;
        }
    }
    return NULL;
}

static void var_set(const char *name, Value val) {
    // First, check if variable exists in CURRENT scope only
    Var *v = NULL;
    if (current_scope) {
        for (v = current_scope->vars; v; v = v->next) {
            if (strcmp(v->name, name) == 0) break;
        }
    } else {
        for (v = global_scope->vars; v; v = v->next) {
            if (strcmp(v->name, name) == 0) break;
        }
    }
    
    if (v) {
        // Found in current scope - overwrite it
        if (v->val.type == VAL_STR && v->val.str) free(v->val.str);
        v->val = val;
        return;
    }
    
    // Not found in current scope - create new variable in current scope
    v = malloc(sizeof(Var));
    v->name = strdup(name);
    v->val = val;
    
    if (current_scope) {
        v->next = current_scope->vars;
        current_scope->vars = v;
    } else {
        v->next = global_scope->vars;
        global_scope->vars = v;
    }
}

static FuncDef *func_get(const char *name) {
    for (int i = 0; i < func_table.func_count; i++) {
        if (strcmp(func_table.funcs[i]->name, name) == 0) return func_table.funcs[i];
    }
    return NULL;
}

static void func_set(const char *name, char **params, int param_count, Node *body) {
    if (func_get(name)) { fprintf(stderr, "Error: Function %s already defined\n", name); exit(1); }
    FuncDef *f = malloc(sizeof(FuncDef));
    f->name = strdup(name);
    f->params = params;
    f->param_count = param_count;
    f->body = body;
    func_table.funcs = realloc(func_table.funcs, (func_table.func_count + 1) * sizeof(FuncDef*));
    func_table.funcs[func_table.func_count++] = f;
}

/* ---------- Parser Functions ---------- */
typedef struct { Lexer lx; Token cur; } Parser;
static void token_free(Token *t) { if (!t) return; if (t->text) free(t->text); t->text = NULL; }
static Token peek_token(Parser *p) { return p->cur; }
static void advance(Parser *p) { token_free(&p->cur); p->cur = next_token(&p->lx); }
static int accept(Parser *p, TokenType t) { if (peek_token(p).type == t) { advance(p); return 1; } return 0; }
static void expect(Parser *p, TokenType t, const char *msg) {
    if (peek_token(p).type == t) { advance(p); return; }
    fprintf(stderr, "Parse error at line %d: expected %s but found token %d\n", p->lx.line, msg, peek_token(p).type);
    exit(1);
}

static void expect_stmt_terminator(Parser *p) {
    Token t = peek_token(p);
    if (t.type == T_DOT || t.type == T_NEWLINE) {
        advance(p);
    } else if (t.type == T_SET || t.type == T_PRINT || t.type == T_READ || t.type == T_IF || 
              t.type == T_WHILE || t.type == T_END || t.type == T_EOF || t.type == T_FUNCTION || 
              t.type == T_RETURN || t.type == T_RBRACE ||
              (t.type == T_IDENTIFIER && t.text && strcmp(t.text, "else") == 0)) {
        // Implicit termination
    } else {
        fprintf(stderr, "Parse error at line %d: expected '.' or newline but found token %d ('%s')\n",
                p->lx.line, t.type, t.text ? t.text : "");
        exit(1);
    }
}

static Node *parse_factor(Parser *p);
static Node *parse_term(Parser *p);
static Node *parse_expression(Parser *p);
static Node *parse_compare(Parser *p);
static Node *parse_statement(Parser *p);
static Node *parse_statements(Parser *p);

static Node *parse_func_def(Parser *p) {
    advance(p); // consume T_FUNCTION
    if (peek_token(p).type != T_IDENTIFIER) {
        fprintf(stderr, "Parse error at line %d: expected identifier after 'function'\n", p->lx.line);
        exit(1);
    }
    char *name = strdup(peek_token(p).text);
    advance(p);
    expect(p, T_LPAREN, "(");
    char **params = NULL;
    int param_count = 0;
    if (peek_token(p).type != T_RPAREN) {
        params = malloc(16 * sizeof(char*));
        params[param_count++] = strdup(peek_token(p).text);
        advance(p);
        while (peek_token(p).type == T_COMMA) {
            advance(p);
            if (peek_token(p).type != T_IDENTIFIER) {
                fprintf(stderr, "Parse error at line %d: expected parameter name\n", p->lx.line);
                exit(1);
            }
            params[param_count++] = strdup(peek_token(p).text);
            advance(p);
        }
    }
    expect(p, T_RPAREN, ")");
    expect(p, T_LBRACE, "{");
    Node *body = parse_statements(p);
    expect(p, T_RBRACE, "}");
    Node *n = node_alloc(N_STMT_FUNCDEF);
    n->name = name;
    n->params = params;
    n->param_count = param_count;
    n->body = body;
    return n;
}

static Node *parse_return_stmt(Parser *p) {
    advance(p); // consume T_RETURN
    Node *expr = (peek_token(p).type == T_DOT || peek_token(p).type == T_NEWLINE || 
                  peek_token(p).type == T_RBRACE) ? NULL : parse_expression(p);
    expect_stmt_terminator(p);
    Node *n = node_alloc(N_STMT_RETURN);
    n->body = expr;
    return n;
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
        char *name = strdup(tk.text);
        advance(p);
        if (peek_token(p).type == T_LPAREN) { // Function call
            advance(p);
            Node **args = NULL;
            int arg_count = 0;
            if (peek_token(p).type != T_RPAREN) {
                args = malloc(16 * sizeof(Node*));
                args[arg_count++] = parse_expression(p);
                while (peek_token(p).type == T_COMMA) {
                    advance(p);
                    args[arg_count++] = parse_expression(p);
                }
            }
            expect(p, T_RPAREN, ")");
            Node *n = node_alloc(N_EXPR_CALL);
            n->name = name;
            n->args = args;
            n->arg_count = arg_count;
            return n;
        } else { // Variable
            Node *n = node_alloc(N_EXPR_VAR);
            n->name = name;
            return n;
        }
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
    fprintf(stderr, "Parse error at line %d: unexpected token in factor\n", p->lx.line);
    exit(1);
}

static Node *parse_term(Parser *p) {
    Node *left = parse_factor(p);
    while (1) {
        Token tk = peek_token(p);
        if (tk.type == T_MUL || tk.type == T_DIV || tk.type == T_MOD) {
            advance(p);
            Node *right = parse_factor(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = tk.type;
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
        if (tk.type == T_PLUS || tk.type == T_MINUS) {
            advance(p);
            Node *right = parse_term(p);
            Node *n = node_alloc(N_EXPR_BINARY);
            n->left = left;
            n->right = right;
            n->number = tk.type;
            left = n;
        } else {
            break;
        }
    }
    return left;
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

static Node *parse_statements(Parser *p) {
    Node *head = NULL;
    Node **tail = &head;
    while (1) {
        Token t = peek_token(p);
        if (t.type == T_EOF || t.type == T_END || t.type == T_THEN || t.type == T_DO ||
            t.type == T_RBRACE || (t.type == T_IDENTIFIER && t.text && strcmp(t.text, "else") == 0)) break;
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

static Node *parse_statement(Parser *p) {
    while (peek_token(p).type == T_NEWLINE) advance(p);
    Token tk = peek_token(p);
    if (tk.type == T_SET) {
        advance(p);
        if (peek_token(p).type != T_IDENTIFIER) {
            fprintf(stderr, "Parse error at line %d: expected identifier after 'set'\n", p->lx.line);
            exit(1);
        }
        char *name = strdup(peek_token(p).text);
        advance(p);
        expect(p, T_TO, "to");
        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);
        Node *n = node_alloc(N_STMT_SET);
        n->name = name;
        n->body = expr;
        return n;
    } else if (tk.type == T_PRINT) {
        advance(p);
        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);
        Node *n = node_alloc(N_STMT_PRINT);
        n->body = expr;
        return n;
    } else if (tk.type == T_READ) {
        advance(p);
        if (peek_token(p).type != T_IDENTIFIER) {
            fprintf(stderr, "Parse error at line %d: expected identifier after 'read'\n", p->lx.line);
            exit(1);
        }
        char *name = strdup(peek_token(p).text);
        advance(p);
        expect_stmt_terminator(p);
        Node *n = node_alloc(N_STMT_READ);
        n->name = name;
        return n;
    } else if (tk.type == T_IF) {
        advance(p);
        Node *cond = parse_compare(p);
        expect(p, T_THEN, "then");
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
    } else if (tk.type == T_WHILE) {
        advance(p);
        Node *cond = parse_compare(p);
        expect(p, T_DO, "do");
        Node *stmts = parse_statements(p);
        expect(p, T_END, "'end' to close while");
        expect_stmt_terminator(p);
        Node *n = node_alloc(N_STMT_WHILE);
        n->cond = cond;
        n->body = stmts;
        return n;
    } else if (tk.type == T_FUNCTION) {
        return parse_func_def(p);
    } else if (tk.type == T_RETURN) {
        return parse_return_stmt(p);
    } else if (tk.type == T_DOT) {
        advance(p);
        return NULL;
    } else if (tk.type == T_EOF) {
        return NULL;
    } else {
        Node *expr = parse_expression(p);
        expect_stmt_terminator(p);
        Node *n = node_alloc(N_STMT_PRINT);
        n->body = expr;
        return n;
    }
}

/* ---------- Evaluation ---------- */
static Value value_dup(const Value *v) {
    Value nv = *v;
    if (v->type == VAL_STR && v->str) nv.str = strdup(v->str);
    return nv;
}

static void value_free(Value *v) {
    if (v->type == VAL_STR && v->str) free(v->str);
    v->type = VAL_NONE;
    v->num = 0;
    v->str = NULL;
}

static Value eval_expr(Node *n);
static Value eval_stmt(Node *n, int *returned, Value *return_val) {
    if (!n) return (Value){VAL_NONE, 0, NULL};
    if (*returned) return value_dup(return_val);
    switch (n->type) {
        case N_STMT_LIST: {
            Node *c = n->body;
            Value last_result = (Value){VAL_NONE, 0, NULL};
            while (c) {
                value_free(&last_result);
                last_result = eval_stmt(c, returned, return_val);
                if (*returned) {
                    Value rv = value_dup(return_val);
                    value_free(&last_result);
                    return rv;
                }
                c = c->next;
            }
            return last_result;
        }
        case N_STMT_SET: {
            Value v = eval_expr(n->body);
            var_set(n->name, v);
            return v;
        }
        case N_STMT_PRINT: {
            Value v = eval_expr(n->body);
            if (v.type == VAL_NUM) printf("%g\n", v.num);
            else if (v.type == VAL_STR) printf("%s\n", v.str ? v.str : "");
            value_free(&v);
            return (Value){VAL_NONE, 0, NULL};
        }
        case N_STMT_READ: {
            char buf[256];
            if (!fgets(buf, sizeof(buf), stdin)) {
                fprintf(stderr, "Input error\n");
                exit(1);
            }
            buf[strcspn(buf, "\n")] = 0;
            char *endptr;
            double val = strtod(buf, &endptr);
            if (endptr == buf || *endptr != '\0') {
                Value strv = {VAL_STR, 0, strdup(buf)};
                var_set(n->name, strv);
                return value_dup(&strv);
            } else {
                Value numv = {VAL_NUM, val, NULL};
                var_set(n->name, numv);
                return numv;
            }
        }
        case N_STMT_IF: {
            Value condv = eval_expr(n->cond);
            Value res = (Value){VAL_NONE, 0, NULL};
            if (condv.type != VAL_NUM) {
                fprintf(stderr, "Error: Condition must be numeric\n");
                exit(1);
            }
            if (condv.num != 0.0) {
                res = eval_stmt(n->body, returned, return_val);
            } else if (n->else_body) {
                res = eval_stmt(n->else_body, returned, return_val);
            }
            value_free(&condv);
            return res;
        }
        case N_STMT_WHILE: {
            Value res = (Value){VAL_NONE, 0, NULL};
            Value condv;
            while (1) {
                condv = eval_expr(n->cond);
                if (condv.type != VAL_NUM || condv.num == 0.0 || *returned) {
                    value_free(&condv);
                    break;
                }
                value_free(&condv);
                value_free(&res);
                res = eval_stmt(n->body, returned, return_val);
                if (*returned) {
                    Value rv = value_dup(return_val);
                    value_free(&res);
                    return rv;
                }
            }
            return res;
        }
        case N_STMT_FUNCDEF: {
            func_set(n->name, n->params, n->param_count, n->body);
            return (Value){VAL_NONE, 0, NULL};
        }
        case N_STMT_RETURN: {
            Value ret_val;
            if (n->body) {
                ret_val = eval_expr(n->body);
            } else {
                ret_val = (Value){VAL_NUM, 0.0, NULL};
            }
            *returned = 1;
            *return_val = value_dup(&ret_val);
            value_free(&ret_val);
            return value_dup(return_val);
        }
        default: return (Value){VAL_NONE, 0, NULL};
    }
}

static Value eval_expr(Node *n) {
    if (!n) return (Value){VAL_NONE, 0, NULL};
    switch (n->type) {
        case N_EXPR_NUMBER: return (Value){VAL_NUM, n->number, NULL};
        case N_EXPR_STRING: return (Value){VAL_STR, 0, strdup(n->string ? n->string : "")};
        case N_EXPR_VAR: {
            Var *v = var_get(n->name);
            if (!v) {
                fprintf(stderr, "Error: Undefined variable %s\n", n->name);
                exit(1);
            }
            return value_dup(&v->val);
        }
        case N_EXPR_CALL: {
            FuncDef *f = func_get(n->name);
            if (!f) { fprintf(stderr, "Error: Undefined function %s\n", n->name); exit(1); }
            if (f->param_count != n->arg_count) {
                fprintf(stderr, "Error: Function %s expects %d args, got %d\n", 
                        n->name, f->param_count, n->arg_count);
                exit(1);
            }
            
            // CRITICAL FIX: Evaluate ALL arguments BEFORE pushing new scope
            Value *arg_values = malloc(f->param_count * sizeof(Value));
            for (int i = 0; i < f->param_count; i++) {
                arg_values[i] = eval_expr(n->args[i]);
                // Evaluate in current scope before creating new one
            }
            
            // Create new scope for function call
            push_scope();
            
            // Set parameters in the new scope
            for (int i = 0; i < f->param_count; i++) {
                var_set(f->params[i], arg_values[i]);
                value_free(&arg_values[i]); // Clean up temporary value
            }
            free(arg_values);
            
            // Execute function body
            int func_returned = 0;
            Value func_return_value = {VAL_NONE, 0, NULL};
            Value func_result = eval_stmt(f->body, &func_returned, &func_return_value);
            
            // Clean up the function's scope
            pop_scope();
            
            // Return the appropriate value
            Value final_result;
            if (func_returned) {
                final_result = value_dup(&func_return_value);
                value_free(&func_result);
            } else {
                final_result = func_result; // func_result is already a copy
            }
            
            // Clean up
            value_free(&func_return_value);
            return final_result;
        }

        case N_EXPR_BINARY: {
            Value l = eval_expr(n->left);
            Value r = eval_expr(n->right);
            if (n->number == T_PLUS && (l.type == VAL_STR || r.type == VAL_STR)) {
                char lbuf[64], rbuf[64];
                const char *lstr = l.type == VAL_STR ? (l.str ? l.str : "") : (sprintf(lbuf, "%g", l.num), lbuf);
                const char *rstr = r.type == VAL_STR ? (r.str ? r.str : "") : (sprintf(rbuf, "%g", r.num), rbuf);
                size_t len = strlen(lstr) + strlen(rstr) + 1;
                char *res = malloc(len);
                strcpy(res, lstr);
                strcat(res, rstr);
                value_free(&l);
                value_free(&r);
                return (Value){VAL_STR, 0, res};
            }
            if (l.type != VAL_NUM || r.type != VAL_NUM) {
                fprintf(stderr, "Error: Numeric operation on non-numeric types\n");
                value_free(&l);
                value_free(&r);
                exit(1);
            }
            Value result;
            switch ((TokenType)(int)n->number) {
                case T_PLUS: result = (Value){VAL_NUM, l.num + r.num, NULL}; break;
                case T_MINUS: result = (Value){VAL_NUM, l.num - r.num, NULL}; break;
                case T_MUL: result = (Value){VAL_NUM, l.num * r.num, NULL}; break;
                case T_DIV: {
                    if (r.num == 0) { fprintf(stderr, "Error: Division by zero\n"); exit(1); }
                    result = (Value){VAL_NUM, l.num / r.num, NULL};
                    break;
                }
                case T_MOD: result = (Value){VAL_NUM, fmod(l.num, r.num), NULL}; break;
                case T_EQ: result = (Value){VAL_NUM, l.num == r.num ? 1 : 0, NULL}; break;
                case T_NEQ: result = (Value){VAL_NUM, l.num != r.num ? 1 : 0, NULL}; break;
                case T_GT: result = (Value){VAL_NUM, l.num > r.num ? 1 : 0, NULL}; break;
                case T_LT: result = (Value){VAL_NUM, l.num < r.num ? 1 : 0, NULL}; break;
                case T_LE: result = (Value){VAL_NUM, l.num <= r.num ? 1 : 0, NULL}; break;
                case T_GE: result = (Value){VAL_NUM, l.num >= r.num ? 1 : 0, NULL}; break;
                case T_AND: result = (Value){VAL_NUM, (l.num != 0.0 && r.num != 0.0) ? 1.0 : 0.0, NULL}; break;
                default: result = (Value){VAL_NONE, 0, NULL}; break;
            }
            value_free(&l);
            value_free(&r);
            return result;
        }
        default: return (Value){VAL_NONE, 0, NULL};
    }
}

/* ---------- Memory Cleanup ---------- */
static void free_node(Node *n) {
    if (!n) return;
    if (n->name) free(n->name);
    if (n->string) free(n->string);
    if (n->params) {
        for (int i = 0; i < n->param_count; i++) free(n->params[i]);
        free(n->params);
    }
    if (n->args) {
        for (int i = 0; i < n->arg_count; i++) free_node(n->args[i]);
        free(n->args);
    }
    free_node(n->left);
    free_node(n->right);
    free_node(n->cond);
    free_node(n->body);
    free_node(n->else_body);
    free_node(n->next);
    free(n);
}

static void free_func_table() {
    for (int i = 0; i < func_table.func_count; i++) {
        FuncDef *f = func_table.funcs[i];
        free(f->name);
        for (int j = 0; j < f->param_count; j++) free(f->params[j]);
        free(f->params);
        free_node(f->body);
        free(f);
    }
    free(func_table.funcs);
}

/* ---------- Main ---------- */
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s file.elang\n", argv[0]); return 1; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    Parser p = {.lx = {.src = src, .pos = 0, .line = 1}};
    push_scope();
    advance(&p);
    Node *ast = parse_statements(&p);
    int returned = 0;
    Value return_val = (Value){VAL_NONE, 0, NULL};
    Value main_res = eval_stmt(ast, &returned, &return_val);
    value_free(&main_res);
    value_free(&return_val);
    free_node(ast);
    free_func_table();
    while (current_scope) pop_scope();
    free(src);
    return 0;
}