#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*
  Desafio Código da Ilha – Edição Free Fire
  Arquivo: FreeFire.c

  BLOCO 1 — NÍVEL NOVATO (Inventário básico)
    - struct Item (nome, tipo, quantidade)
    - Vetor estático MAX_ITENS=10
    - Menu: inserir, remover, listar
    - Sem busca/ordenação

  BLOCO 2 — NÍVEL AVENTUREIRO (Busca sequencial)
    - Mesmas operações do Novato
    - + Função de busca sequencial por nome (strcmp)

  BLOCO 3 — NÍVEL MESTRE (Ordenação e busca binária)
    - + Campo prioridade (1..5) em Item
    - + Enum de critérios e Insertion Sort (nome/tipo/prioridade)
    - + Busca binária por nome (requer ordenado por nome)
    - + Contador de comparações na ordenação
*/

#define MAX_ITENS 10
#define TAM_NOME  30
#define TAM_TIPO  20

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int  quantidade;
    int  prioridade; // usado apenas no Nível Mestre (Bloco 3)
} Item;

typedef enum {
    POR_NOME = 1,
    POR_TIPO = 2,
    POR_PRIORIDADE = 3
} CriterioOrdenacao;

/* -------- utilidades -------- */
static void strip_newline(char *s){ size_t n=strlen(s); if(n>0 && s[n-1]=='\n') s[n-1]='\0'; }
static void limparBuffer(void){ int c; while((c=getchar())!='\n' && c!=EOF){} }
static void lerLinha(char *dst, size_t tam, const char *prompt){
    if (prompt && *prompt) printf("%s", prompt);
    if (!fgets(dst, (int)tam, stdin)) { dst[0]='\0'; return; }
    strip_newline(dst);
}

/* -------- listagem -------- */
static void listarItens(const Item v[], int n, int nivel){
    printf("\n------ INVENTARIO ------\n");
    if (n==0){ printf("Mochila vazia.\n"); return; }
    if (nivel < 3){
        printf("%-28s | %-18s | %s\n", "Nome", "Tipo", "Quantidade");
        printf("-----------------------------------------------\n");
        for (int i=0;i<n;i++)
            printf("%-28s | %-18s | %d\n", v[i].nome, v[i].tipo, v[i].quantidade);
    } else {
        printf("%-28s | %-18s | %10s | %10s\n","Nome","Tipo","Quantidade","Prioridade");
        printf("--------------------------------------------------------------------------\n");
        for (int i=0;i<n;i++)
            printf("%-28s | %-18s | %10d | %10d\n", v[i].nome, v[i].tipo, v[i].quantidade, v[i].prioridade);
    }
}

/* -------- bloco 1: inserir/remover -------- */
static int buscarSequencialIndice(const Item v[], int n, const char *nome){
    for (int i=0;i<n;i++) if (strcmp(v[i].nome, nome)==0) return i;
    return -1;
}

static void inserirItem(Item v[], int *n, int nivel, bool *ordenadaPorNome){
    if (*n >= MAX_ITENS){ printf("\nMochila cheia.\n"); return; }

    Item novo = {0};
    lerLinha(novo.nome, TAM_NOME, "Nome: ");
    if (!strlen(novo.nome)){ printf("Nome invalido.\n"); return; }

    lerLinha(novo.tipo, TAM_TIPO, "Tipo (arma, municao, cura, ferramenta...): ");
    if (!strlen(novo.tipo)){ printf("Tipo invalido.\n"); return; }

    printf("Quantidade: ");
    if (scanf("%d",&novo.quantidade)!=1 || novo.quantidade<0){
        printf("Quantidade invalida.\n"); limparBuffer(); return;
    }
    limparBuffer();

    // BLOCO 3: prioridade só no Nível Mestre
    if (nivel==3){
        printf("Prioridade (1..5, 5=alta): ");
        if (scanf("%d",&novo.prioridade)!=1 || novo.prioridade<1 || novo.prioridade>5){
            printf("Prioridade invalida.\n"); limparBuffer(); return;
        }
        limparBuffer();
        if (ordenadaPorNome) *ordenadaPorNome = false;
    }

    v[*n] = novo; (*n)++;
    if (ordenadaPorNome) *ordenadaPorNome = false;
    printf("Item adicionado.\n");
}

static void removerItem(Item v[], int *n, bool *ordenadaPorNome){
    if (*n==0){ printf("\nMochila vazia.\n"); return; }
    char alvo[TAM_NOME]; lerLinha(alvo, TAM_NOME, "Nome do item a remover: ");
    int idx = buscarSequencialIndice(v, *n, alvo);
    if (idx==-1){ printf("Item nao encontrado.\n"); return; }
    for (int i=idx;i<*n-1;i++) v[i]=v[i+1];
    (*n)--; if (ordenadaPorNome) *ordenadaPorNome=false;
    printf("Item removido.\n");
}

/* -------- bloco 2: busca sequencial -------- */
static void buscarItemSequencial(const Item v[], int n){
    if (n==0){ printf("\nMochila vazia.\n"); return; }
    char alvo[TAM_NOME]; lerLinha(alvo, TAM_NOME, "Nome do item a buscar: ");
    bool achou=false;
    for (int i=0;i<n;i++){
        if (strcmp(v[i].nome, alvo)==0){
            printf("\nItem encontrado:\nNome: %s\nTipo: %s\nQuantidade: %d\n",
                   v[i].nome, v[i].tipo, v[i].quantidade);
            achou=true; break;
        }
    }
    if (!achou) printf("\nItem '%s' nao encontrado.\n", alvo);
}

/* -------- bloco 3: ordenação + busca binária -------- */
static int comparar(const Item *a, const Item *b, CriterioOrdenacao c){
    if (c==POR_NOME){
        int r=strcmp(a->nome,b->nome); if (r) return r;
        r=strcmp(a->tipo,b->tipo);     if (r) return r;
        return b->prioridade - a->prioridade; // maior primeiro
    } else if (c==POR_TIPO){
        int r=strcmp(a->tipo,b->tipo); if (r) return r;
        r=strcmp(a->nome,b->nome);     if (r) return r;
        return b->prioridade - a->prioridade;
    } else {
        int r=b->prioridade - a->prioridade; if (r) return r;
        r=strcmp(a->nome,b->nome);           if (r) return r;
        return strcmp(a->tipo,b->tipo);
    }
}

static void insertionSort(Item v[], int n, CriterioOrdenacao c, long *comparacoes){
    if (comparacoes) *comparacoes=0;
    for (int i=1;i<n;i++){
        Item key=v[i]; int j=i-1;
        while (j>=0){
            if (comparacoes) (*comparacoes)++;
            if (comparar(&v[j], &key, c) > 0){ v[j+1]=v[j]; j--; }
            else break;
        }
        v[j+1]=key;
    }
}

static void ordenarMenu(Item v[], int n, bool *ordenadaPorNome){
    if (n<=1){ printf("\nNada a ordenar.\n"); return; }
    printf("\n-- Ordenacao --\n1. Nome (A->Z)\n2. Tipo (A->Z)\n3. Prioridade (5->1)\nEscolha: ");
    int op; if (scanf("%d",&op)!=1){ limparBuffer(); printf("Opcao invalida.\n"); return; }
    limparBuffer();
    CriterioOrdenacao c = (op==1)?POR_NOME:(op==2)?POR_TIPO:(op==3)?POR_PRIORIDADE:0;
    if (!c){ printf("Opcao invalida.\n"); return; }
    long comps=0; insertionSort(v,n,c,&comps);
    *ordenadaPorNome = (c==POR_NOME);
    printf("Itens ordenados. Comparacoes: %ld\n", comps);
}

static int buscaBinariaPorNome(const Item v[], int n, const char *nome){
    int i=0,f=n-1;
    while(i<=f){
        int m=(i+f)/2; int r=strcmp(v[m].nome,nome);
        if (r==0) return m;
        if (r<0) i=m+1; else f=m-1;
    }
    return -1;
}

/* -------- menus -------- */
static void menuNivel(int nivel, bool ordenada){
    if (nivel==1){
        printf("\n=== NIVEL NOVATO ===\n");
        printf("1. Adicionar item\n2. Remover item por nome\n3. Listar itens\n0. Sair\nEscolha: ");
    } else if (nivel==2){
        printf("\n=== NIVEL AVENTUREIRO ===\n");
        printf("1. Adicionar item\n2. Remover item por nome\n3. Listar itens\n4. Buscar item por nome\n0. Sair\nEscolha: ");
    } else {
        printf("\n=== NIVEL MESTRE (ordenadaPorNome=%s) ===\n", ordenada?"true":"false");
        printf("1. Adicionar item\n2. Remover item por nome\n3. Listar itens\n4. Ordenar (nome/tipo/prioridade)\n5. Buscar sequencial por nome\n6. Buscar BINARIA por nome\n0. Sair\nEscolha: ");
    }
}

int main(void){
    Item mochila[MAX_ITENS]; int numItens=0; bool ordenadaPorNome=false;

    printf("Selecione o nivel (1=Novato, 2=Aventureiro, 3=Mestre): ");
    int nivel=1; if (scanf("%d",&nivel)!=1 || nivel<1 || nivel>3){ limparBuffer(); nivel=1; }
    limparBuffer();

    int op;
    do{
        menuNivel(nivel, ordenadaPorNome);
        if (scanf("%d",&op)!=1){ limparBuffer(); printf("Entrada invalida.\n"); continue; }
        limparBuffer();

        if (nivel==1){
            switch(op){
                case 1: inserirItem(mochila,&numItens,1,&ordenadaPorNome); listarItens(mochila,numItens,1); break;
                case 2: removerItem(mochila,&numItens,&ordenadaPorNome);   listarItens(mochila,numItens,1); break;
                case 3: listarItens(mochila,numItens,1); break;
                case 0: printf("Saindo.\n"); break;
                default: printf("Opcao invalida.\n");
            }
        } else if (nivel==2){
            switch(op){
                case 1: inserirItem(mochila,&numItens,2,&ordenadaPorNome); listarItens(mochila,numItens,2); break;
                case 2: removerItem(mochila,&numItens,&ordenadaPorNome);   listarItens(mochila,numItens,2); break;
                case 3: listarItens(mochila,numItens,2); break;
                case 4: buscarItemSequencial(mochila,numItens); break;
                case 0: printf("Saindo.\n"); break;
                default: printf("Opcao invalida.\n");
            }
        } else { // nivel 3
            switch(op){
                case 1: inserirItem(mochila,&numItens,3,&ordenadaPorNome); listarItens(mochila,numItens,3); break;
                case 2: removerItem(mochila,&numItens,&ordenadaPorNome);   listarItens(mochila,numItens,3); break;
                case 3: listarItens(mochila,numItens,3); break;
                case 4: ordenarMenu(mochila,numItens,&ordenadaPorNome);    listarItens(mochila,numItens,3); break;
                case 5: buscarItemSequencial(mochila,numItens); break;
                case 6: {
                    if (!ordenadaPorNome){ printf("\nOrdene por nome antes da busca binaria.\n"); break; }
                    if (numItens==0){ printf("\nMochila vazia.\n"); break; }
                    char alvo[TAM_NOME]; lerLinha(alvo,TAM_NOME,"Nome a buscar (binaria): ");
                    int i = buscaBinariaPorNome(mochila,numItens,alvo);
                    if (i>=0) printf("Encontrado: %s | %s | qtd=%d | prioridade=%d\n",
                                     mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
                    else printf("Item nao encontrado.\n");
                } break;
                case 0: printf("Saindo.\n"); break;
                default: printf("Opcao invalida.\n");
            }
        }
    } while(op!=0);

    return 0;
}
