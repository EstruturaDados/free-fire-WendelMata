#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ITENS 10
#define TAM_NOME  30
#define TAM_TIPO  20

// Item básico do Nível Novato
typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int  quantidade;
} Item;

// -------- utilidades de I/O --------
static void strip_newline(char *s){
    size_t n = strlen(s);
    if (n > 0 && s[n-1]=='\n') s[n-1]='\0';
}

static void limparBuffer(void){
    int c; while((c=getchar())!='\n' && c!=EOF){}
}

static void lerLinha(char *dest, size_t tam, const char *prompt){
    if (prompt && *prompt) printf("%s", prompt);
    if (!fgets(dest, (int)tam, stdin)) { dest[0]='\0'; return; }
    strip_newline(dest);
}

// -------- listar --------
static void listarItens(const Item v[], int n){
    printf("\n------ INVENTARIO ------\n");
    if (n==0){ printf("Mochila vazia.\n"); return; }
    printf("%-28s | %-18s | %s\n", "Nome", "Tipo", "Quantidade");
    printf("-----------------------------------------------\n");
    for (int i=0;i<n;i++){
        printf("%-28s | %-18s | %d\n", v[i].nome, v[i].tipo, v[i].quantidade);
    }
}

// -------- inserir --------
static void inserirItem(Item v[], int *n){
    if (*n >= MAX_ITENS){ printf("\nMochila cheia.\n"); return; }

    Item novo;
    lerLinha(novo.nome, TAM_NOME, "Nome: ");
    if (strlen(novo.nome)==0){ printf("Nome invalido.\n"); return; }

    lerLinha(novo.tipo, TAM_TIPO, "Tipo (arma, municao, cura, ferramenta...): ");
    if (strlen(novo.tipo)==0){ printf("Tipo invalido.\n"); return; }

    printf("Quantidade: ");
    if (scanf("%d", &novo.quantidade)!=1 || novo.quantidade<0){
        printf("Quantidade invalida.\n");
        limparBuffer();
        return;
    }
    limparBuffer();

    v[*n] = novo;
    (*n)++;
    printf("Item adicionado.\n");
}

// -------- remover por nome --------
static int buscarIndicePorNome(const Item v[], int n, const char *nome){
    for (int i=0;i<n;i++){
        if (strcmp(v[i].nome, nome)==0) return i;
    }
    return -1;
}

static void removerItem(Item v[], int *n){
    if (*n==0){ printf("\nMochila vazia.\n"); return; }
    char alvo[TAM_NOME];
    lerLinha(alvo, TAM_NOME, "Nome do item a remover: ");

    int idx = buscarIndicePorNome(v, *n, alvo);
    if (idx==-1){ printf("Item nao encontrado.\n"); return; }

    for (int i=idx;i<*n-1;i++) v[i]=v[i+1];
    (*n)--;
    printf("Item removido.\n");
}

// -------- menu --------
static void menu(void){
    printf("\n=== NIVEL NOVATO ===\n");
    printf("1. Adicionar item\n");
    printf("2. Remover item por nome\n");
    printf("3. Listar itens\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

int main(void){
    Item mochila[MAX_ITENS];
    int numItens=0;

    int op;
    do{
        menu();
        if (scanf("%d",&op)!=1){ limparBuffer(); printf("Entrada invalida.\n"); continue; }
        limparBuffer();

        switch(op){
            case 1: inserirItem(mochila, &numItens); listarItens(mochila, numItens); break;
            case 2: removerItem(mochila, &numItens); listarItens(mochila, numItens); break;
            case 3: listarItens(mochila, numItens); break;
            case 0: printf("Saindo.\n"); break;
            default: printf("Opcao invalida.\n");
        }
    }while(op!=0);

    return 0;
}
