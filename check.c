#include "check.h"

int check_id_length(char* str){
    int length = 0;
    char* name = str;
    while(*name != '\0'){
        length++;
        name++;
    }

    return length;
}

void* is_unsigned_int(void* value){
    long result = 0;
    char* str = (char*)value;
    while(*str != '\0' && result <= 4294967295){
        result = result*10 + (*str-'0');
        str++;        
    }

    if(result > 4294967295) return NULL;
    else {
        uint32_t* value = malloc(sizeof(uint32_t));
        uint32_t temp = result;
        *value = temp;
        return value;
    }

}

bool analyseQ1(Q_Node* root){

    //check debit account name
    if(check_id_length(root->node.q1.debit_account->name) > 30){
        printf("Account name must be less or equal to 30 characters.\
        Wrong account name:\n%s\n", root->node.q1.debit_account->name);
        return false;
    }


    //check debit account value
    void* check_debit_value = is_unsigned_int(root->node.q1.debit_value->value);
    if(!check_debit_value){
        printf("Debit account %s value is out of bound.\n",
        root->node.q1.debit_account->name);
        return false;
    } else {
        root->node.q1.debit_value->value = check_debit_value;
    }


    //check credit account name
    if(check_id_length(root->node.q1.credit_account->name) > 30){
        printf("Account name must be less or equal to 30 characters.\
        Wrong account name:\n%s\n", root->node.q1.credit_account->name);
        return false;
    }

    //check credit account value
    void* check_credit_value = is_unsigned_int(root->node.q1.credit_value->value);
    if(!check_credit_value){
        printf("Credit account %s value is out of bound.\n",
        root->node.q1.credit_account->name);
        return false;
    } else {
        root->node.q1.credit_value->value = check_credit_value;
    }


    if(check_id_length(root->node.q1.table_name->name) > 30){
        printf("File name must be less or equal to 30 characters.\
        Wrong file name:\n%s\n", root->node.q1.table_name->name);
        return false;
    }

    return true;
}
bool analyseQ3(Q_Node* root){
    //check if the table name is less than or equal to 30 letters

    if(check_id_length(root->node.q3.table_name->name) > 30){
        printf("File name must be less or equal to 30 characters.\
        Wrong file name:\n%s\n", root->node.q3.table_name->name);
        return false;
    } else return true;
}

bool analyseQ(Q_Node* root){
    int nodeType = root->tag;

    switch (nodeType)
    {
    case 0:
        return analyseQ1(root);
    case 2:
        return analyseQ3(root);   
    default:
        return true;   
    }

}