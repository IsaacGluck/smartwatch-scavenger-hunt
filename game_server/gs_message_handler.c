/*
 * gs_message_handler - a function tables used for gameserver
 *
 * Handles message received from field agent or guide agent
 *
 * Kazuma Honjo, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** file-local global variables ****************/
static const int MESSAGE_LENGTH = 8192;
static const int CLUE_LENGTH = 140;

/**************** functions ****************/
static int fn_fa_location(char *rest_of_line);
static int fn_fa_claim(char *rest_of_line);
static int fn_fa_log(char *rest_of_line);
static int fn_ga_status(char *rest_of_line);
static int fn_ga_hint(char *rest_of_line);
static int fn_game_status(char *rest_of_line);
static int fn_gs_agent(char *rest_of_line);
static int fn_gs_clue(char *rest_of_line);
static int fn_gs_claimed(char *rest_of_line);
static int fn_gs_secret(char *rest_of_line);
static int fn_gs_response(char *rest_of_line);
static int fn_game_over(char *rest_of_line);
static int fn_team_record(char *rest_of_line);
static token_t *token_new();
static void token_next(token_t *token);
static void token_delete(token_t *token);

/**************** file local type ****************/
// struct for holding messages
static typedef struct token{
    char *left;              // left hand side of the equation
    char *right;             // right hand side of the equation
    char *rest_of_line;      // rest of the line
}token_t;

/**************** function dispatch table ****************/
static const struct {
    const char *command
    void (*func)(char *rest_of_line, );
} dispatch[] = {
    { "FA_LOCATION", fn_fa_location },
    { "FA_CLAIM", fn_fa_claim },
    { "FA_LOG", fn_fa_log },
    { "GA_STATUS", fn_ga_status },
    { "GA_HINT", fn_ga_hint },
    { "GAME_STATUS", fn_game_status },
    { "GS_AGENT", fn_gs_agent },
    { "GS_CLUE", fn_gs_clue },
    { "GS_CLAIMED", fn_gs_claimed },
    { "GS_SECRET", fn_gs_secret },
    { "GS_RESPONSE", fn_gs_response },
    { "GAME_OVER", fn_game_over },
    { "TEAM_RECORD", fn_team_record },
    { NULL, NULL }
};


/**************** fn_fa_location ****************/
static int fn_fa_location(char *rest_of_line){
    
}





static int fn_fa_claim(char *rest_of_line);
static int fn_fa_log(char *rest_of_line);
static int fn_ga_status(char *rest_of_line);
static int fn_ga_hint(char *rest_of_line);
static int fn_game_status(char *rest_of_line);
static int fn_gs_agent(char *rest_of_line);
static int fn_gs_clue(char *rest_of_line);
static int fn_gs_claimed(char *rest_of_line);
static int fn_gs_secret(char *rest_of_line);
static int fn_gs_response(char *rest_of_line);
static int fn_game_over(char *rest_of_line);
static int fn_team_record(char *rest_of_line);


static token_t *token_new(char *rest_of_line){
    if (rest_of_line == NULL){
        return NULL;
    }
    
    token_t *new_token = malloc(sizeof(token_t));
    if (new_token == NULL){
        fprintf(stderr, "token_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    // initialize parameters
    new_token->left = malloc(MESSAGE_LENGTH);
    new_token->right = malloc(MESSAGE_LENGTH);
    new_token->rest_of_line = malloc(MESSAGE_LENGTH);
    if (new_token->left == NULL || new_token->right == NULL || new_token->rest_of_line == NULL){
        fprintf(stderr, "token_new failed due to not being able to malloc memory\n");
        return NULL;
    }
    
    strcpy(new_token->rest_of_line, rest_of_line);
    return new_token;
}

static void token_next(token_t *token){
    // initialize parameters
    int i = 0;
    int j = 0;
    int state = 0; // 0: currently reading left hand side 1: right hand side
    
    while (i < strlen(token->rest_of_line)){
        if (token->rest_of_line+i == '|'){
            token->right+j = '\0';
            break;
        }
        else if (token->rest_of_line+i == '='){
            token->left+j = '\0';
            j = 0;
            state = 1;
        }
        else{
            if (state == 0){
                token->left+j = token->rest_of_line+i;
            }
            else {
                token->right+j = token->rest_of_line+i;
                if (i == strlen(token->rest_of_line)-1){
                    token->right+j+1 = '\0';
                }
            }
            j++;
        }
        i++;
    }
    if (i != strlen(token->rest_of_line)){
        token->rest_of_line = token->rest_of_line+i;
    }
    else{
        token->rest_of_line = '\0';
    }
}


static void token_delete(token_t *token){
    if (token == NULL){
        return;
    }
    if (token->left != NULL) free(token->left);
    if (token->right != NULL) free(token->right);
    if (token->rest_of_line != NULL) free(token->rest_of_line);
    free(new_token);
}
