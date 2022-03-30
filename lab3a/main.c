#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HashFirst(char *key,int SizeOfTable){
    int sum = 0;
    int i;
    for (i = 0; i < strlen(key);i++){
        sum += key[i];
    }
    return sum % SizeOfTable;
}
int HashSecond(char *key ,int SizeOfTable){
    int sum = 0;
    int i;
    for (i = 0 ; i<strlen(key);i++){
        sum+=key[i];
    }
    return 3-sum%3;

}


typedef struct Item{
    char *str;
    int key1;
    char *key2;
    int  ind;
    struct KeySpace1 *p;

}Item;
typedef struct KeySpace1{
    int key;
    int par;
    Item *info;
    struct KeySpace1 *next;
}KeySpace1;

typedef struct KeySpace2{
    int busy;
    char *key;
    int  release;
    Item *info;
}KeySpace2;

typedef struct Table{
    KeySpace1 *ks1;
    KeySpace2 *ks2;
    int msize2;
    int csize1;
    int csize2;
}Table;

int FindVersionKS2(Table tab,char *k2){
    if (tab.csize2 == 0){
        return 1;
    }
    int i = 0,version = 1;
    for (i = 0 ;i < tab.msize2;i++){
        if (tab.ks2[i].busy > 0 && strcmp(tab.ks2[i].key,k2)==0){
            version++;
        }
    }

    return version;
}
int PushItemKS2(Table *p,Item *item){
    int count = FindVersionKS2(*p,item->key2);
    int index = HashFirst(item->key2,p->msize2);
    int step = 1;
    int begin = index;
    while (p->ks2[index].busy >0){
        index = (index + step ) % p->msize2;
        if (index == begin){
            const char *pr =  "Table is full\n";
            printf("%s\n",pr);
            return 0;

        }
    }
    p->ks2[index].busy = 1;
    p->ks2[index].info = item;
    p->ks2[index].release = count;
    p->ks2[index].key = (char*)malloc(strlen(item->key2));
    strcpy(p->ks2[index].key,item->key2);
    p->csize2++;
    return 1;

}


KeySpace1 *FindKS1(Table tab,int k1,char *k2){
    KeySpace1 *p = tab.ks1;
    while (p!= NULL){
        if (p->key == k1 ){
            return p;
        }
        p = p->next;
    }
    p = NULL;
    return p;
}


char * FindInfoKS1(Table tab, int k1,char *k2){
    char *info = NULL;
    KeySpace1 *p = FindKS1(tab,k1,k2);
    if (p != NULL){
        info = (char *)malloc(strlen(tab.ks1->info->str));
        info = strcpy(info,tab.ks1->info->str);
    }
    else{ const char *pr =  "Item not found!\n";
        printf("%s\n",pr);
        return info;

    }
    return info;
}
int getInt(int *a)
{
    int n;
    do{
        n = scanf_s("%d", a, sizeof(int));
        if (n < 0)
            return 0;

        if (n == 0){
            printf("%s\n", "Error! Repeat input");
            scanf_s("%*c", 0);
        }
    } while (n == 0);
    return 1;
}
char *getStr(){
    char *s, *buf = (char *)calloc(sizeof (char), 256);
    int flag;
    char *error;
    error = "";
    do{
        flag = 1;
        puts(error);
        error = "Please enter string without spaces";
        if (!scanf("%s%*[ ]", buf))
            flag = 0;
        if (getchar() != '\n')
            flag = 0;
        fflush(stdin);
    } while (!flag);
    s = (char *)calloc(sizeof (char), strlen(buf) + 1);
    strcpy(s, buf);
    free(buf);
    buf = NULL;
    return s;
}
void CreateTable(Table *p,int SizeKS2){
    p->csize1 = 0;
    p->csize2 = 0;
    p->msize2 = SizeKS2;
    p->ks1 = NULL;
    p->ks2 = (KeySpace2 *)malloc(SizeKS2*sizeof(KeySpace2));
    int i = 0;
    for (i = 0; i < SizeKS2;i++){
        p->ks2[i].busy = 0;
    }
}
int PushItemKS1(Table *q,Item *item){
    const char *pr =  "KeySpace1 is full!\n";
    KeySpace1 *curr = (KeySpace1 *)malloc(sizeof(KeySpace1));
    if (!curr){
        printf("%s\n",pr);
        return 0;
    }
    if (q->ks1 == NULL){
        curr->par = 0;
    }
    else {
        curr->par = q->ks1->key;
    }
    curr->info = item;
    curr->key = item->key1;
    curr->next = q->ks1;
    q->ks1 = curr;
    return 1;
}
int insert(Table *p,Item *item){
    const char *pr = "KeySpace2 is full!\n";
    if ((p->csize2 == p->msize2)){
        printf("%s\n",pr);
        return 0;
    }

    else {
        KeySpace1 *curr = FindKS1(*p,item->key1,item->key2);
        if (curr == NULL){
            PushItemKS1(p,item);
            PushItemKS2(p,item);
        }
        else {
            const char *pr = "Item already exists in space 1\n";
            printf("%s\n",pr);
        }


        return 1;
    }

}
int DeleteFromKS1(Table *p,int key1,char *key2){


    const char *pr = "KeySpace1 is empty!\n";
    if (p->ks1 == NULL){
        printf("%s\n",pr);
        return 0;
    }
    KeySpace1 *curr = FindKS1(*p,key1,key2);
    if (curr == NULL){
        const char *pr = "Item not found!\n";
        printf("%s\n",pr);
    }
    else {
        if (curr == p->ks1){
            p->ks1 = curr->next;
            free(curr);
            return 1;
        }
        KeySpace1 **change;
        KeySpace1 *iter = p->ks1;
        while (iter->next != curr){
            iter = iter->next;
        }

        change = &iter;
        (*change)->next = curr->next;

        free(curr);
        return 1;


    }

}
int DeleteFromKS2(Table *p,int key1,char key2){
    int i = 0;
    for (i = 0; i < p->msize2; i++){
        if (p->ks2[i].busy > 0){
            if (p->ks2[i].info->key1 == key1){
                p->ks2[i].busy = 0 ;

            }


        }
    }
    return -1;
}
int SpecialDeleteFromKS1(Table *p,int key1){

    const char *pr = "KeySpace1 is empty!\n";
    if (p->ks1 == NULL){
        printf("%s\n",pr);
        return 0;
    }
    KeySpace1 *curr = p->ks1;
    KeySpace1 *ans =  NULL;
    while (curr!=NULL){
        if (curr->par == key1){
            curr->par = 0;
        }
        if (curr->key == key1){
            ans = curr;
        }
        curr = curr->next;
    }

    if (ans == NULL){
        const char *pr = "Item not found!\n";

        printf("%s\n",pr);
        return 0;
    }
    DeleteFromKS2(p,ans->key,ans->info->key2);
    if (ans == p->ks1){
        p->ks1 = ans->next;
        free(ans);
        return 1;
    }

    KeySpace1 **change;
    KeySpace1 *iter = p->ks1;
    while (iter->next != ans){
        iter = iter->next;
    }

    change = &iter;
    (*change)->next = ans->next;


    return 1;

}
int  SpecialFindForKS1(Table p,int a, int b){
    const char *pr = "KeySpace1 is empty!\n";
    if (p.ks1 == NULL){
        printf("%s\n",pr);
        return 0;
    }

    KeySpace1 *curr = p.ks1;
    int cnt = 0;
    while (curr!=NULL){
        if (curr->par >a && curr->par <b){
            cnt++;
        }
        curr  = curr->next;
    }
    if (cnt == 0){
        const char *pr = "Not found special item in this range\n";
        printf("%s\n",pr);
        return 0;
    }
    Table SpecialTable;
    CreateTable(&SpecialTable,cnt);
    KeySpace1 *iter = p.ks1;
    while (iter!=NULL){
        if (iter->par >a && iter->par <b) {
            insert(&SpecialTable,iter->info);
            SpecialTable.ks1->par = iter->par;
        }
        iter  = iter->next;

    }
    output(SpecialTable);
    return 1;
}
int DeleteFromTable(Table *p,int key1 ,char *key2){
    if(DeleteFromKS1(p,key1,key2) &&  DeleteFromKS2(p,key1,key2)){
        return 1;
    }

    return 0;
}
int  output(Table p){
    const char *tab = "Content of the Table:\n";
    printf("%s\n",tab);
    const char *msg = "Content of the KS1:\n";
    printf("%s\n",msg);
    const char *msg1 = "Key 1";
    printf("%10s",msg1);
    const char *msg4 = "Key 2";
    const char *msg2 = "ParentKey";
    printf("%10s",msg2);
    printf("%10s",msg4);
    const char *msg3 = "Info ";
    printf("%10s\n",msg3);
    while (p.ks1!=NULL){
        printf("%10d",p.ks1->key);
        printf("%10d",p.ks1->par);
        printf("%10s",p.ks1->info->key2);
        printf("%10s\n",p.ks1->info->str);
        p.ks1 = p.ks1->next;
    }
    printf("\n");
    const char *msg5 = "Content of the KS2:\n";
    printf("%s\n",msg5);
    printf("%10s",msg4);

    const char *msg8 = "Version";
    printf("%10s",msg8);
    printf("%10s",msg1);
    printf("%10s\n",msg3);
    int i = 0;
    for (i = 0; i<p.msize2;i++){
        if (p.ks2[i].busy > 0){
            printf("%10s",p.ks2[i].key);
            printf("%10d",p.ks2[i].release);
            printf("%10d",p.ks2[i].info->key1);
            printf("%10s\n",p.ks2[i].info->str);

        }
    }
    return 1;
}
int SpecialFindForKS2(Table p,char *k){
    int cnt = FindVersionKS2(p,k);
    Table ans;
    CreateTable(&ans,cnt);
    int j = 0;
    if (p.csize2 == 0){
        const char *pr = "Key space is empty!\n";
        printf("%s\n",pr);
        return 0;
    }
    for (j = 0; j < p.msize2;j++){
        if (p.ks2[j].busy > 0){
        if (strcmp(p.ks2[j].key,k) ==0){
            printf("%s %d %s\n",p.ks2[j].key,p.ks2[j].release,p.ks2[j].info->str);
        }}
    }
    return 1;
}
int FindByVersion(Table p,char *key,int version){
    for (int i = 0; i <p.msize2; i++){
        if(p.ks2->busy > 0){
            if (strcmp(p.ks2[i].key,key) == 0 && p.ks2[i].release == version){
                printf("\n Info :");
                printf("%s",p.ks2[i].info->str);
                return 1;
            }
        }
    }
    return 0;
}


int dialog(const char *msgs[],int Nmsgs){
    char *errmsg = "";
    int rc;
    int i, n;
    do {
        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";
        for(i = 0; i < Nmsgs; ++i)
            puts(msgs[i]);
        puts("Make your choice: --> ");

        n = getInt(&rc);
        if (n==0) rc = 0;

    }while(rc < 0 || rc >= Nmsgs);
    return rc;
}
int D_Add(Table *p){
    int k, rc, n;
    char *errmsg = "";
    Item *item = (Item *)malloc(sizeof(Item));
    do {
        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter key 1: -->");

        n = getInt(&item->key1);

    }while(n==0);
    printf("Enter key 2: -->");
    item->key2 = getStr();
    if (item->key2 == NULL)return 0;
    printf("Enter info:");
    item->str = getStr();
    if (item->str == NULL)return 0;
    if (insert(p,item)){
        return 1;
    }
    else {
        return 0;
    }
}
int D_Find(Table *p){
    int k,rc,n;
    int key1;
    char *errmsg = "";
    do {
        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter key 1: -->");

        n = getInt(&key1);

    }while(n==0);
    printf("Enter key 2: -->");
    char *key2 = NULL;
    key2 = getStr();
    if (key2 == NULL)return 0;
    if (FindInfoKS1(*p,key1,key2)==NULL){
        return 0;
    }
    else {
        char *info =(char*)malloc(strlen(FindInfoKS1(*p,key1,key2)));
        strcpy(info,FindInfoKS1(*p,key1,key2));
        printf("Information :%10s\n",info);
        return 2;
    }
}
int D_Delete(Table *p){
    int k,rc,n;char *errmsg = "";
    int key1;
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter key 1: -->");

        n = getInt(&key1);

    }while(n==0);
    printf("Enter key 2: -->\n");
    char *key2 = NULL;
    key2 = getStr();
    if (key2 == NULL)return 0;
    if (DeleteFromTable(p,key1,key2)){
        return 3;
    }
    else{
        return 0;
    }
}
int D_Show (Table *p){
    if (output(*p)){
        return 4;
    }
    else {
        return 0;
    }
}
int D_SpecialFindForKS1(Table *p){
    int a,b,n;
    char *errmsg = "";
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Low board: -->");

        n = getInt(&a);

    }while(n==0);
    errmsg = "";
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("High board: -->");

        n = getInt(&b);

    }while(n==0);

    if (SpecialFindForKS1(*p,a,b)){
        return 5;
    }else{
        return 0;
    }
}
int D_SpecialDeleteForKS1(Table *p){
    int k,rc,n;char *errmsg = "";
    int key1;
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter key 1: -->");

        n = getInt(&key1);

    }while(n==0);
    if (SpecialDeleteFromKS1(p,key1)){
        return 6;
    }
    else{
        return 0;
    }
}
int D_SpecialFindForKS2(Table *p){
    printf("Enter key 2: -->\n");
    char *key2 = NULL;
    key2 = getStr();
    if (key2 == NULL)return 0;

    if(SpecialFindForKS2(*p,key2)){
        return 7;
    }
    else {
        return 0;
    }

}
int D_FindByTheVersion(Table *p){
    printf("Enter key 2: -->\n");
    char *key2 = NULL;
    char *errmsg = "";
    int n;
    key2 = getStr();
    if (key2 == NULL)return 0;
    int version;
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter version: -->");

        n = getInt(&version);

    }while(n==0);

    if (FindByVersion(*p,key2,version)){
        return 8;
    }
    else {
        return 0;
    }


}
int D_FindByTheFirsKey(Table p){
    char *errmsg = "";
    int n;
    int key1;
    do {

        puts(errmsg);
        errmsg = "You are wrong. Repeate, please!";

        puts("Enter key1: -->");

        n = getInt(&key1);

    }while(n==0);

    if (FindKS1(p,key1,"")){
        KeySpace1 *q = FindKS1(p,key1,"");
        printf("Info :%s\n",q->info->str);
        return 9;
    }
    else {
        return 0;
    }

}


int (*fptr[])(Table *) = {NULL, D_Add, D_Find, D_Delete, D_Show,D_SpecialFindForKS1,D_SpecialDeleteForKS1,D_SpecialFindForKS2,
                          D_FindByTheVersion, D_FindByTheFirsKey};

int main()
{   const char *msgs[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show","5. SpecialFindForKS1","6. SpecialDeleteForKS1","7. SpecialFindForKS2","8. FindByTheVersion","9. FindByTheFirsKey"};
    const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

    Table table;
    CreateTable(&table,10);
    int rc;
    while (rc = dialog(msgs,NMsgs)){
        if (!fptr[rc](&table)){
            break;
        }
    }


    return 0;
}
