
struct SymbolData {
    char *symbol;   /* Symbol name */
    int val;        /* Symbol value */
    int type;       /* Type of the symbol (e.x., DATA_SYMBOL) */
    bool isEntry;   /* Flag to indicate if the symbol is an entry point */
};

struct SymbolNode {
    struct SymbolData *data;
    struct SymbolNode *next;
};

struct MacroData {
    char *name;
    char *val;
};

struct MacroNode {
    struct MacroData *data;
    struct MacroNode *next;
};

/* external variables:*/
extern struct SymbolNode *symbolTable;
extern struct SymbolNode *externalSymbolsTable;

extern int IC;
extern int DC;

/* functions:*/
void clearTable(void *head, bool isSymbolTable);

void insertMacro(struct MacroNode *head, struct MacroData *data);

struct MacroNode *searchMacro(struct MacroNode *head, char *macroname);

void insertSymbol(struct SymbolNode *head, struct SymbolData *data);

struct SymbolNode *searchSymbol(struct SymbolNode *head, char *symbol);

bool hasEntry(struct SymbolNode *head);


