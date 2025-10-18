#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ITENS 10
#define TAM_NOME  30
#define TAM_TIPO  20

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int  quantidade;
    int  prioridade; // 1..5
} Item;

typedef enum {
    POR_NOME = 1,
    POR_TIPO = 2,
    POR_PRIORIDADE = 3
} CriterioOrdenacao;

/* -------- util -------- */
static void strip_newline(char *s){ size_t n=strlen(s); if(n>0&&s[n-1]=='\n') s[n-1]='\0'; }
static void limparBuffer(void){ int c; while((c=getchar())!='\n' && c!=EOF){} }
static void lerLinha(char *dest, size_t tam, const char *prompt){
    if (prompt && *prompt) printf("%s", prompt);
    if (!fgets(dest, (int)tam, stdin)) { dest[0]='\0'; return; }
    strip_newline(dest);
}

/* -------- listar -------- */
static void listarItens(const Item v[], int n){
    printf("\n------ INVENTARIO ------\n");
    if (n==0){ printf("Mochila vazia.\n"); return; }
    printf("%-28s | %-18s | %10s | %10s\n","Nome","Tipo","Quantidade","Prioridade");
    printf("--------------------------------------------------------------------------\n");
    for(int i=0;i<n;i++)
        printf("%-28s | %-18s | %10d | %10d\n", v[i].nome, v[i].tipo, v[i].quantidade, v[i].prioridade);
}

/* -------- inserir/remover -------- */
static int buscarSequencial(const Item v[], int n, const char *nome){
    for(int i=0;i<n;i++) if(strcmp(v[i].nome,nome)==0) return i;
    return -1;
}

static void inserirItem(Item v[], int *n, bool *ordenadaPorNome){
    if (*n>=MAX_ITENS){ printf("\nMochila cheia.\n"); return; }
    Item novo;
    lerLinha(novo.nome, TAM_NOME, "Nome: ");
    if (!strlen(novo.nome)){ printf("Nome invalido.\n"); return; }
    lerLinha(novo.tipo, TAM_TIPO, "Tipo (arma, municao, cura, ferramenta...): ");
    if (!strlen(novo.tipo)){ printf("Tipo invalido.\n"); return; }

    printf("Quantidade: ");
    if (scanf("%d",&novo.quantidade)!=1 || novo.quantidade<0){ printf("Quantidade invalida.\n"); limparBuffer(); return; }

    printf("Prioridade (1..5, 5=alta): ");
    if (scanf("%d",&novo.prioridade)!=1 || novo.prioridade<1 || novo.prioridade>5){ printf("Prioridade invalida.\n"); limparBuffer(); return; }
    limparBuffer();

    v[*n]=novo; (*n)++; *ordenadaPorNome=false;
    printf("Item adicionado.\n");
}

static void removerItem(Item v[], int *n, bool *ordenadaPorNome){
    if (*n==0){ printf("\nMochila vazia.\n"); return; }
    char alvo[TAM_NOME]; lerLinha(alvo, TAM_NOME, "Nome a remover: ");
    int idx = buscarSequencial(v,*n,alvo);
    if (idx==-1){ printf("Item nao encontrado.\n"); return; }
    for(int i=idx;i<*n-1;i++) v[i]=v[i+1];
    (*n)--; *ordenadaPorNome=false;
    printf("Item removido.\n");
}

/* -------- ordenacao -------- */
static int comparar(const Item *a, const Item *b, CriterioOrdenacao c){
    if (c==POR_NOME){
        int r = strcmp(a->nome,b->nome); if (r) return r;
        r = strcmp(a->tipo,b->tipo);     if (r) return r;
        return b->prioridade - a->prioridade; // maior primeiro
    } else if (c==POR_TIPO){
        int r = strcmp(a->tipo,b->tipo); if (r) return r;
        r = strcmp(a->nome,b->nome);     if (r) return r;
        return b->prioridade - a->prioridade;
    } else { // prioridade desc
        int r = b->prioridade - a->prioridade; if (r) return r;
        r = strcmp(a->nome,b->nome);           if (r) return r;
        return strcmp(a->tipo,b->tipo);
    }
}

static void insertionSort(Item v[], int n, CriterioOrdenacao c, long *comparacoes){
    if (comparacoes) *comparacoes=0;
    for(int i=1;i<n;i++){
        Item key=v[i]; int j=i-1;
        while(j>=0){
            if (comparacoes) (*comparacoes)++;
            if (comparar(&v[j], &key, c) > 0){ v[j+1]=v[j]; j--; }
            else break;
        }
        v[j+1]=key;
    }
}

static void menuOrdenacao(Item v[], int n, bool *ordenadaPorNome){
    if (n<=1){ printf("\nNada a ordenar.\n"); return; }
    printf("\n-- Ordenacao --\n1. Nome (A->Z)\n2. Tipo (A->Z)\n3. Prioridade (5->1)\nEscolha: ");
    int op; if (scanf("%d",&op)!=1){ limparBuffer(); printf("Opcao invalida.\n"); return; }
    limparBuffer();
    CriterioOrdenacao c = (op==1)?POR_NOME:(op==2)?POR_TIPO:(op==3)?POR_PRIORIDADE:0;
    if (!c){ printf("Opcao invalida.\n"); return; }
    long comps=0; insertionSort(v,n,c,&comps); *ordenadaPorNome = (c==POR_NOME);
    printf("Itens ordenados. Comparacoes: %ld\n", comps);
}

/* -------- buscas -------- */
static int buscaBinariaPorNome(const Item v[], int n, const char *nome){
    int i=0, f=n-1;
    while(i<=f){
        int m=(i+f)/2; int r=strcmp(v[m].nome,nome);
        if (r==0) return m;
        if (r<0) i=m+1; else f=m-1;
    }
    return -1;
}

/* -------- menu -------- */
static void menu(bool ordenada){
    printf("\n=== NIVEL MESTRE (ordenadaPorNome=%s) ===\n", ordenada?"true":"false");
    printf("1. Adicionar item\n2. Remover item por nome\n3. Listar itens\n4. Ordenar (nome/tipo/prioridade)\n5. Buscar sequencial por nome\n6. Buscar BINARIA por nome\n0. Sair\nEscolha: ");
}

/* -------- main -------- */
int main(void){
    Item mochila[MAX_ITENS]; int numItens=0; bool ordenadaPorNome=false;
    int op;
    do{
        menu(ordenadaPorNome);
        if (scanf("%d",&op)!=1){ limparBuffer(); printf("Entrada invalida.\n"); continue; }
        limparBuffer();
        switch(op){
            case 1: inserirItem(mochila,&numItens,&ordenadaPorNome); listarItens(mochila,numItens); break;
            case 2: removerItem(mochila,&numItens,&ordenadaPorNome); listarItens(mochila,numItens); break;
            case 3: listarItens(mochila,numItens); break;
            case 4: menuOrdenacao(mochila,numItens,&ordenadaPorNome); listarItens(mochila,numItens); break;
            case 5: {
                if (numItens==0){ printf("\nMochila vazia.\n"); break; }
                char alvo[TAM_NOME]; lerLinha(alvo,TAM_NOME,"Nome a buscar (sequencial): ");
                int i = buscarSequencial(mochila,numItens,alvo);
                if (i>=0) printf("Encontrado: %s | %s | qtd=%d | prioridade=%d\n", mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
                else printf("Item nao encontrado.\n");
            } break;
            case 6: {
                if (!ordenadaPorNome){ printf("\nOrdene por nome antes da busca binaria.\n"); break; }
                if (numItens==0){ printf("\nMochila vazia.\n"); break; }
                char alvo[TAM_NOME]; lerLinha(alvo,TAM_NOME,"Nome a buscar (binaria): ");
                int i = buscaBinariaPorNome(mochila,numItens,alvo);
                if (i>=0) printf("Encontrado: %s | %s | qtd=%d | prioridade=%d\n", mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
                else printf("Item nao encontrado.\n");
            } break;
            case 0: printf("Saindo.\n"); break;
            default: printf("Opcao invalida.\n");
        }
    }while(op!=0);
    return 0;
}
