#include "chibicc.h"
#define DEBUG_C "debug.c"

//  for debug needs
char *tokenkind2str(TokenKind kind)
{
    switch (kind)
    {
    case TK_IDENT:
        return "TK_IDENT";
    case TK_PUNCT:
        return "TK_PUNCT";
    case TK_KEYWORD:
        return "TK_KEYWORD";
    case TK_STR:
        return "TK_STR";
    case TK_NUM:
        return "TK_NUM";
    case TK_PP_NUM:
        return "TK_PP_NUM";
    case TK_EOF:
        return "TK_EOF";
    default:
        return "UNREACHABLE";
    }
}

// print all tokens received
void print_debug_tokens(char *currentfilename, char *function, Token *tok)
{

    fprintf(f, "=====================file : %s, function: %s\n", currentfilename, function);

    // for debug needs print all the tokens with values
    Token *t = tok;
    while (t->kind != TK_EOF)
    {
        if (t->len > 0)
        {
            char tokloc[t->len + 1];
            memset(tokloc, 0, sizeof(tokloc));
            char *ptokloc = &tokloc[0];
            strncpy(ptokloc, t->loc, t->len);
            fprintf(f, "token->kind: %s, token->len: %d, token->val: %ld, token->fval:%Lf \n", tokenkind2str(t->kind), t->len, t->val, t->fval);
            fprintf(f, "     token->str: %s, token->filename: %s, token->line_no: %d, token->at_bol:%d \n", t->str, t->filename, t->line_no, t->at_bol);
            fprintf(f, "     token->loc: %s \n", ptokloc);
        }
        // TK_EOF not sure that it helps to have this information in the log!
        //  else
        //  {
        //    fprintf(f, "token->kind: %s, token->len: %d, token->val: %ld, token->fval:%Lf \n", tokenkind2str(t->kind), t->len, t->val, t->fval);
        //    fprintf(f, "     token->str: %s, token->filename: %s, token->line_no: %d, token->at_bol:%d \n", t->str, t->filename, t->line_no, t->at_bol);
        //  }
        t = t->next;
    }
}