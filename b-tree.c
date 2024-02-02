/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 743606
 * Aluno: William Giacometti Dutra de Oliveira
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
void ler_entrada(char* registro, Produto *novo);
Produto recuperar_registro(int rrn);
void gerarChave(Produto* p);
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice* iprimary, Indice* ibrand);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice ibrand);
int buscarIP(int rrn, char* pk); // busca no iprimary e retorna um RRN
void buscarIS(int rrn, Chave_is *chave); // busca no isecondary e retorna uma PK
int buscarComPrint(int rrn, char* pk); // printa os nos pelo que percorre

void listar(Indice iprimary, Indice ibrand);
void preorder(int rrn, int nivel);
void inorder(int rrn);
 /*******************************************************/

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
* de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_ibrand(Indice *ibrand);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/* Cria um no novo e distribui os nos entre o no do parametro e o novo */
node_Btree_ip *divide_noIP(int rrnIp, Chave_ip K, int filho_direito, Chave_ip *chave_promovida);
node_Btree_is *divide_noIS(int rrnIs, Chave_is K, int filho_direito, Chave_is *chave_promovida);

int insere_aux(int rrnIp, Chave_ip K, Chave_ip *chave_promovida);
int insere_aux_is(int rrnIs, Chave_is K, Chave_is *chave_promovida);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registroIP(Indice *iprimary, Produto p, int rrnArquivo);
void inserir_registroIS(Indice *ibrand, Produto p, int rrnArquivo);

/* Exibe o produto */
int exibir_registro(int rrn);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (TAM_STRING_INDICE +9);

	/* Indice primário */
	Indice iprimary;
	criar_iprimary(&iprimary);

	/* Indice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");
				}
			break;
		case 8: /* Libera toda memoria alocada dinamicamente (se ainda houver) e encerra */
			return 0;
		default: /* Exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);

	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}

void criar_iprimary(Indice *iprimary)
{
	if(nregistros > 0) {
		Produto p;
		for(int i = 0; i < nregistros; i++) {
			p = recuperar_registro(i);
			inserir_registroIP(iprimary, p, i);
		}
	}
	else {
		iprimary->raiz = -1;
	}
}

void criar_ibrand(Indice *ibrand)
{
	if(nregistros > 0) {
		Produto p;
		for(int i = 0; i < nregistros; i++) {
			p = recuperar_registro(i);
			inserir_registroIS(ibrand, p, i);
		}
	}
	else {
		ibrand->raiz = -1;
	}
}

void ler_entrada(char* registro, Produto *novo)
{
    scanf("%50[^\n]", novo->nome); getchar();
    scanf("%50[^\n]", novo->marca); getchar();
    scanf("%[^\n]", novo->data); getchar();
    scanf("%[^\n]", novo->ano); getchar();
    scanf("%[^\n]", novo->preco); getchar();
    scanf("%[^\n]", novo->desconto); getchar();
    scanf("%50[^\n]", novo->categoria); getchar();
    gerarChave(novo);

    char string_arquivo[TAM_REGISTRO + 1], *arroba;
    int i = 0, j;

    i = sprintf(string_arquivo, "%s@%s@%s@%s@%s@%s@%s@%s@", novo->pk, novo->nome, novo->marca, novo->data, novo->ano, novo->preco, novo->desconto, novo->categoria);

    if(i < TAM_REGISTRO) // preenche o string com # ate o fim do tamanho
        for(j = i; j < TAM_REGISTRO; j++)
            string_arquivo[j] = '#';
    string_arquivo[j] = '\0';

    strcpy(registro, string_arquivo);
}

void gerarChave(Produto* p)
{
    char newChave[TAM_PRIMARY_KEY];
    strncpy(newChave, p->nome, 2); // Copia os primeiros dois digitos de Nome para newChave
    strncpy(newChave+2, p->marca, 2); // Copia os primeiros dois digitos de marca a partir da posicao 2 de newChave
    strncpy(newChave+4, p->data, 2); // Copia os primeiros dois digitos de data a a partir da posicao 4 de newChave
    strncpy(newChave+6, p->data+3, 2);
    strncpy(newChave+8, p->ano, 2);
    strncpy(p->pk, newChave, TAM_PRIMARY_KEY); // Copia todos digitos
    p->pk[TAM_PRIMARY_KEY-1] = '\0';
}

void *criar_no(char ip)
{
	if(ip) {
		node_Btree_ip *tmp = NULL;
		tmp = (node_Btree_ip *) malloc(sizeof(node_Btree_ip));
		tmp->num_chaves = 0;
		tmp->chave = (Chave_ip *) malloc((ordem_ip-1) * sizeof(Chave_ip));
		tmp->desc = (int *) malloc(ordem_ip * sizeof(int));
		for(int i = 0; i < ordem_ip; i++)
			tmp->desc[i] = -1;
		tmp->folha = 'F';
		return tmp;
	}
	else {
		node_Btree_is *tmp = (node_Btree_is *) malloc(sizeof(node_Btree_is));
		tmp->num_chaves = 0;
		tmp->chave = (Chave_is *) malloc((ordem_is-1) * sizeof(Chave_is));
		tmp->desc = (int *) malloc(ordem_is * sizeof(int));
		for(int i = 0; i < ordem_is; i++)
			tmp->desc[i] = -1;
		tmp->folha = 'F';
		return tmp;
	}
}

int buscarIP(int rrn, char* pk) 
{
	node_Btree_ip *ip = read_btree(rrn, 1);
	int i = 0;

	while (i < ip->num_chaves && strcmp(pk, ip->chave[i].pk) > 0) {
		i++;
	}

	if (i < ip->num_chaves && strcmp(pk, ip->chave[i].pk) == 0) {
		// int k = ;
		// libera_no(ip, 1);
		return ip->chave[i].rrn;
	}
	
	if (ip->folha == 'F') {
		// libera_no(ip, 1);
		return -1;
	}
	else {
		// int k = ip->desc[i];
		// libera_no(ip, 1);
		return ip->desc[i];
	}
}

int buscarComPrint(int rrn, char* pk) 
{
	node_Btree_ip *ip = read_btree(rrn, 1);
	int i = 0;
	
	for(int k = 0; k < ip->num_chaves; k++) {
		if(!k)
			printf("%s", ip->chave[k].pk);
		else
			printf(", %s", ip->chave[k].pk);
	}
	printf("\n");

	while (i < ip->num_chaves) {
		if(strcmp(pk, ip->chave[i].pk) <= 0) {
			break;
		}
		i++;
	}

	if (i < ip->num_chaves && strcmp(pk, ip->chave[i].pk) == 0) {
		// int z = ;
		// libera_no(ip, 1);
		return ip->chave[i].rrn;
	}
	
	if (ip->folha == 'F') {
		// libera_no(ip, 1);
		return -1;
	}
	else {
		// int z = ip->desc[i];
		// libera_no(ip, 1);
		return buscarComPrint(ip->desc[i], pk);
	}
}

void buscarIS(int rrn, Chave_is *chave)
{
	node_Btree_is *is = read_btree(rrn, 0);
	int i = 0;

	for(int k = 0; k < is->num_chaves; k++) {
		if(!k)
			printf("%s", is->chave[k].string);
		else
			printf(", %s", is->chave[k].string);
	}
	printf("\n");

	while (i < is->num_chaves) {
		if(strcmp(chave->string, is->chave[i].string) <= 0)
			break;
		i++;
	}

	if (i < is->num_chaves && strcmp(chave->string, is->chave[i].string) == 0) {
		strcpy(chave->pk, is->chave[i].pk);
		// libera_no(is, 0);
		return;
	}
	
	if (is->folha == 'F') {
		// libera_no(is, 0);
		return;
	}
	else {
		// int z = is->desc[i];
		// libera_no(is, 0);
		buscarIS(is->desc[i], chave);
	}
}

Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
	p = strtok(temp,"@");
	strcpy(j.pk, p);
    p = strtok(NULL,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    return j;
}

void *read_btree(int rrn, char ip)
{
	if(rrn == -1)
		return NULL;
	
	if(ip) { // Indice primario
		char temp[tamanho_registro_ip + 1], *ptr;
		node_Btree_ip *node = criar_no(1);
    	strncpy(temp, ARQUIVO_IP + ((rrn)*tamanho_registro_ip), tamanho_registro_ip);
		temp[tamanho_registro_ip] = '\0';
		sscanf(temp, "%d%*c", &node->num_chaves); 
		ptr = &temp[3];
		
		for(int i = 0; i < node->num_chaves; i++) {
			strncpy(node->chave[i].pk, ptr, 10);
			ptr += 10;
			sscanf(ptr, "%d%*c", &node->chave[i].rrn);
			ptr += 4;
		}
		do {
			sscanf(ptr, "%c", &node->folha);	
			if(node->folha == '#')
				ptr += 14;
		} while (node->folha == '#'); // Ignora os caracteres # e obtem o caracter FOLHA
		ptr++;
		for(int i = 0; i <= ordem_ip; i++) {
			char tempRRN[4];
			strncpy(tempRRN, ptr, 3);
			tempRRN[3] = '\0';
			if(strcmp(tempRRN, "***\0") != 0)
				sscanf(tempRRN, "%d", &node->desc[i]);
			else
				node->desc[i] = -1;
			ptr += 3;
		}
		return node;
	}
	else { 	
		// Indice secundario
		char temp[tamanho_registro_is + 1], *ptr, *sharp;
		node_Btree_is *node = criar_no(0);
    	strncpy(temp, ARQUIVO_IS + ((rrn)*tamanho_registro_is), tamanho_registro_is);
		temp[tamanho_registro_is] = '\0'; 
		sscanf(temp, "%3d", &node->num_chaves); // Pega o numero de chaves a partir do node
		int cont = 3;
		for(int i = 0; i < node->num_chaves; i++) {
			ptr = &temp[3] + 111*i;
			strncpy(node->chave[i].pk, ptr, 10); 
			node->chave[i].pk[10] = '\0';
			ptr += 10;
			sscanf(ptr, "%[^#]", node->chave[i].string);
		}

		cont = strlen(temp) - ((ordem_is*3) + 1); // Comeca de tras pra frente, exclui as chaves
		node->folha = temp[cont]; 
		cont++;
		for(int i = 0; i <= ordem_is; i++) {
			char tempRRN[4];
			strncpy(tempRRN, temp + cont, 3);
			if(strcmp(tempRRN, "***") != 0)
				sscanf(tempRRN, "%d", &node->desc[i]);
			else
				node->desc[i] = -1;
			cont += 3;
		}
		return node;
	}
}

void write_btree(void *salvar, int rrn, char ip)
{
	if(ip) { // Indice primario
		node_Btree_ip *ip = salvar;
		char tmpIP[tamanho_registro_ip + 1], asteriscos[tamanho_registro_ip], 
				sharps[tamanho_registro_ip], 
						chaves[tamanho_registro_ip]; // Tamanho maximo de um "registro" do IP contando o '\0'
		sharps[0] = '\0';
		asteriscos[0] = '\0';

		for(int i = 0; i < ip->num_chaves; i++) {
			char *ptr = chaves + 14*i;
			sprintf(ptr, "%s%04d", ip->chave[i].pk, ip->chave[i].rrn);
		}

		if(strlen(chaves) < (ordem_ip - 1) * 14) // Preenche o string com # ate acabar
			for(int j = strlen(chaves); j < (ordem_ip - 1) * 14; j++) // strlen(p.pk) + 4 = tamanho da chave + 0000
				strcat(sharps, "#");

		if(ip->folha == 'F')
			for(int j = 0; j < ordem_ip; j++) // Preenche de asteriscos
				strcat(asteriscos, "***");
		else
			for(int j = 0; j < ordem_ip; j++) { // Preenche de asteriscos
				if(ip->desc[j] == -1 || j >= ip->num_chaves + 1)
					strcat(asteriscos, "***");
				else {
					char tmp[4];
					sprintf(tmp, "%03d", ip->desc[j]);
					strcat(asteriscos, tmp);
				}
			}

		sprintf(tmpIP, "%03d%s%s%c%s", ip->num_chaves, chaves, sharps, ip->folha, asteriscos);
		strncpy(ARQUIVO_IP + (tamanho_registro_ip * rrn), tmpIP, tamanho_registro_ip);
	}
	else {
		node_Btree_is *is = salvar;
		char tmpIS[tamanho_registro_is + 1], asteriscos[tamanho_registro_is + 1], 
						sharps[tamanho_registro_is + 1], chaves[tamanho_registro_is + 1], *ptr; 
								// Tamanho maximo de um "registro" do IS contando o \0
		sharps[0] = '\0';
		asteriscos[0] = '\0';
		
		sprintf(tmpIS, "%03d", is->num_chaves); 

		int i = 0, cont = 3;
		while(i < is->num_chaves) {
			strcat(tmpIS, is->chave[i].pk);
			strcat(tmpIS, is->chave[i].string);

			int j = 101 - strlen(is->chave[i].string); // 101 == 111 bytes - 10 bytes da chave
			while(j){
				strcat(tmpIS, "#");
				j--;
			}
			i++;
		}
		for(int k = is->num_chaves; k < ordem_is - 1; k++) {
			strcat(tmpIS, "###############################################################################################################");
		}
		char letra[2];
		letra[0] = is->folha;
		letra[1] = '\0';
		strcat(tmpIS, letra);

		if(is->folha == 'F')
			for(int j = 0; j < ordem_is; j++) // Preenche de asteriscos
				strcat(tmpIS, "***");
		else
			for(int j = 0; j < ordem_is; j++) { // Preenche de asteriscos
				if(is->desc[j] == -1 || j >= is->num_chaves + 1)
					strcat(tmpIS, "***");
				else {
					char tmp[4];
					sprintf(tmp, "%03d", is->desc[j]);
					strcat(tmpIS, tmp);
				}
			}
		strncpy(ARQUIVO_IS + (tamanho_registro_is * rrn), tmpIS, tamanho_registro_is);
	}	
}

void cadastrar(Indice* iprimary, Indice* ibrand)
{
	Produto p; // Produto com informacoes digitadas pelo usuario
	char s[TAM_REGISTRO]; // String formatada para guardar no arquivo
	ler_entrada(s, &p);
	int i;

	if(strlen(ARQUIVO) <= 0) { // Se o arquivo estiver vazio, a arvore nao tem raiz
		nregistros++;

		inserir_registroIP(iprimary, p, 0);
		inserir_registroIS(ibrand, p, 0);
		strcat(ARQUIVO, s);
	}
	else { 
	//arvore nao esta vazia
		if(buscarIP(iprimary->raiz, p.pk) != -1) { // Verifica se a chave ja nao esta incluida
			printf(ERRO_PK_REPETIDA, p.pk);
			return;
		}
		nregistros++;
		strcat(ARQUIVO, s);

		inserir_registroIP(iprimary, p, nregistros - 1);
		inserir_registroIS(ibrand, p, nregistros - 1);
	}
}

void inserir_registroIP(Indice *iprimary, Produto p, int rrnArquivo)
{
	if(nregistrosip == 0) {
		
		/* criando raiz do IP ============================================= */
		nregistrosip++;
		node_Btree_ip *no = criar_no(1); 

		strcpy(no->chave[0].pk, p.pk);
		no->chave[0].rrn = 0;
		no->num_chaves++;
		no->folha = 'F';
		write_btree(no, 0, 1);
		iprimary->raiz = 0;

		// libera_no(no, 1);
	} 
	else {
		Chave_ip K, chave_promovida;
		strcpy(K.pk, p.pk); // Inicializando K para a funcao insere_aux
		K.rrn = rrnArquivo;
		chave_promovida.pk[0] = '\0'; // Inicializando com nulo
		chave_promovida.rrn = -1;

		int filho_direito;
		
		filho_direito = insere_aux(iprimary->raiz, K, &chave_promovida);
		if(chave_promovida.pk[0] != '\0' && chave_promovida.rrn != -1) {
			node_Btree_ip *no = criar_no(1);
			no->folha = 'N';
			no->num_chaves = 1;

			strcpy(no->chave[0].pk, chave_promovida.pk);
			no->chave[0].rrn = chave_promovida.rrn;

			no->desc[0] = iprimary->raiz;
			no->desc[1] = filho_direito;

			iprimary->raiz = nregistrosip;
			write_btree(no, nregistrosip, 1);
			nregistrosip++;
			// libera_no(no, 1);
		}
	}
}

void inserir_registroIS(Indice *ibrand, Produto p, int rrnArquivo)
{
	if(nregistrosis == 0) {
		/* Criando raiz do IS ============================================= */
		nregistrosis++;
		node_Btree_is *noSecundario = criar_no(0); 

		strcpy(noSecundario->chave[0].pk, p.pk);
		sprintf(noSecundario->chave[0].string, "%s$%s", p.marca, p.nome); // Inicializar string
		noSecundario->num_chaves++;
		noSecundario->folha = 'F';
		
		write_btree(noSecundario, 0, 0);
		ibrand->raiz = 0;

		// libera_no(noSecundario, 0);
	} 
	else {
		Chave_is Ks, chave_promovida_is;
		strcpy(Ks.pk, p.pk); // Inicializando K para a funcao insere_aux
		sprintf(Ks.string, "%s$%s", p.marca, p.nome);
		chave_promovida_is.pk[0] = '\0'; // Inicializando com nulo
		chave_promovida_is.string[0] = '\0';
		
		int filho_direito = insere_aux_is(ibrand->raiz, Ks, &chave_promovida_is);

		if(chave_promovida_is.pk[0] != '\0' && chave_promovida_is.string[0] != '\0') {
			node_Btree_is *no = criar_no(0);
			no->folha = 'N';
			no->num_chaves = 1;

			strcpy(no->chave[0].pk, chave_promovida_is.pk);
			strcpy(no->chave[0].string, chave_promovida_is.string);

			no->desc[0] = ibrand->raiz;
			no->desc[1] = filho_direito;

			ibrand->raiz = nregistrosis;
			write_btree(no, nregistrosis, 0);
			nregistrosis++;

			// libera_no(no, 0);
		}
	}
}

int insere_aux(int rrnIp, Chave_ip K, Chave_ip *chave_promovida) 
{
	node_Btree_ip *ip = read_btree(rrnIp, 1);
	
	if(ip->folha == 'F') { // Encontrou um no' folha
		if(ip->num_chaves < ordem_ip - 1) {
			int i = ip->num_chaves - 1;
			while (i >= 0 && strcmp(K.pk, ip->chave[i].pk) < 0) { // Faz um shift com os nos
				ip->chave[i + 1] = ip->chave[i];
				i--;
			}
			ip->chave[i + 1] = K;
			ip->num_chaves++;
			write_btree(ip, rrnIp, 1);
			// libera_no(ip, 1);

			chave_promovida->pk[0] = '\0'; // return NULL, NULL
			chave_promovida->rrn = -1;
			return -1;
		}
		else { // Chave esta cheia
			ip = divide_noIP(rrnIp, K, -1, chave_promovida);
			write_btree(ip, nregistrosip, 1);
			nregistrosip++;
			libera_no(ip, 1);
			return (nregistrosip - 1);
		}
	}
	else { // Primeiro no' nao eh folha
		int i = ip->num_chaves, filho_direito = -1;
		while(i > 0 && strcmp(K.pk, ip->chave[i - 1].pk) < 0) 
			i--;
		filho_direito = insere_aux(ip->desc[i], K, chave_promovida);

		if(chave_promovida->pk[0] != '\0' && chave_promovida->rrn != -1) {
			K = *chave_promovida;
			if(ip->num_chaves < ordem_ip - 1) {
				int j = ip->num_chaves - 1;
				while(j >= 0 && strcmp(K.pk, ip->chave[j].pk) < 0) {
					ip->chave[j+1] = ip->chave[j];
					ip->desc[j+2] = ip->desc[j+1];
					j--; 
				}
				ip->chave[j+1] = K;
				ip->desc[j+2] = filho_direito;
				ip->num_chaves++;

				write_btree(ip, rrnIp, 1);
				// libera_no(ip, 1);

				chave_promovida->pk[0] = '\0'; // return NULL, NULL
				chave_promovida->rrn = -1;
				return -1;
			}
			else {
				ip = divide_noIP(rrnIp, K, filho_direito, chave_promovida);
				
				//return rrnIp;

				//ip = divide_noIP(rrnIp, K, -1, chave_promovida);
				write_btree(ip, nregistrosip, 1);
				nregistrosip++;
				// libera_no(ip, 1);
				return (nregistrosip - 1);
			}
		}
		else {
			chave_promovida->pk[0] = '\0'; // return NULL, NULL
			chave_promovida->rrn = -1;
			return -1;
		}
	}
}

int insere_aux_is(int rrnIs, Chave_is K, Chave_is *chave_promovida)
{
	node_Btree_is *is = read_btree(rrnIs, 0);
	
	if(is->folha == 'F') { // Achou um no folha
		if(is->num_chaves < ordem_is - 1) {
			int i = is->num_chaves - 1;
			while (i >= 0 && strcmp(K.string, is->chave[i].string) < 0) { // faz um shift com os nos
				is->chave[i + 1] = is->chave[i];
				i--;
			}
			is->chave[i + 1] = K;
			is->num_chaves++;
			write_btree(is, rrnIs, 0);
			// libera_no(is, 0);

			chave_promovida->pk[0] = '\0'; // return NULL, NULL
			chave_promovida->string[0] = '\0';
			return -1;
		}
		else { // Chave esta cheia
			is = divide_noIS(rrnIs, K, -1, chave_promovida);
			write_btree(is, nregistrosis, 0);
			// libera_no(is, 0);
			nregistrosis++;
			return (nregistrosis - 1);
		}
	}
	else { // Primeiro noh nao eh folha
		int i = is->num_chaves, filho_direito = -1;
		while(i > 0 && strcmp(K.string, is->chave[i - 1].string) < 0) //
			i--;
		filho_direito = insere_aux_is(is->desc[i], K, chave_promovida);

		if(chave_promovida->pk[0] != '\0' && chave_promovida->string != '\0') {
			K = *chave_promovida;
			if(is->num_chaves < ordem_is - 1) {
				int j = is->num_chaves - 1;
				while(j >= 0 && strcmp(K.string, is->chave[j].string) < 0) {
					is->chave[j+1] = is->chave[j];
					is->desc[j+2] = is->desc[j+1];
					j--; 
				}
				is->chave[j+1] = K;
				is->desc[j+2] = filho_direito;
				is->num_chaves++;
				write_btree(is, rrnIs, 0);
				// libera_no(is, 0);

				chave_promovida->pk[0] = '\0'; // return NULL, NULL
				chave_promovida->string[0] = '\0';
				return -1;
			}
			else {
				is = divide_noIS(rrnIs, K, filho_direito, chave_promovida);

				write_btree(is, nregistrosis, 0);
				nregistrosis++;
				// libera_no(is, 0);
				return (nregistrosis - 1);
			}
		}
		else {
			chave_promovida->pk[0] = '\0'; // return NULL, NULL
			chave_promovida->string[0] = '\0';
			return -1;
		}
	}
}

node_Btree_ip *divide_noIP(int rrnIp, Chave_ip K, int filho_direito, Chave_ip *chave_promovida) 
{
	node_Btree_ip *ip = read_btree(rrnIp, 1);

	int i = ip->num_chaves - 1, chave_alocada = 0;

	node_Btree_ip *novo = criar_no(1);
	novo->folha = ip->folha;
	novo->num_chaves = (ordem_ip - 1)/2;

	for(int j = novo->num_chaves - 1; j >= 0; j--) {
		if(!chave_alocada && strcmp(K.pk, ip->chave[i].pk) > 0) {
			strcpy(novo->chave[j].pk, K.pk);
			novo->chave[j].rrn = K.rrn;
			novo->desc[j+1] = filho_direito;
			chave_alocada = 1;
		}
		else {
			strcpy(novo->chave[j].pk, ip->chave[i].pk);
			novo->chave[j].rrn = ip->chave[i].rrn;
			novo->desc[j+1] = ip->desc[i+1];
			i--;
		}
	}

	if(!chave_alocada) { // A chave nova fica no node com as maiores chaves 
		while(i >= 0 && strcmp(K.pk, ip->chave[i].pk) < 0) {
			ip->chave[i+1] = ip->chave[i];
			ip->desc[i+2] = ip->desc[i+1];
			i--; 
		}
		ip->chave[i+1] = K;
		ip->desc[i+2] = filho_direito;
	}
	strcpy(chave_promovida->pk, ip->chave[(ordem_ip/2)].pk);
	chave_promovida->rrn = ip->chave[(ordem_ip/2)].rrn;
	novo->desc[0] = ip->desc[(ordem_ip/2) + 1];
	ip->num_chaves = (ordem_ip/2);
	write_btree(ip, rrnIp, 1);
	// libera_no(ip, 1);
	return novo;
}

node_Btree_is *divide_noIS(int rrnIs, Chave_is K, int filho_direito, Chave_is *chave_promovida)
{
	node_Btree_is *is = read_btree(rrnIs, 0);

	int i = is->num_chaves - 1, chave_alocada = 0;

	node_Btree_is *novo = criar_no(0);
	novo->folha = is->folha;
	novo->num_chaves = (ordem_is - 1)/2;

	for(int j = novo->num_chaves - 1; j >= 0; j--) {
		if(!chave_alocada && strcmp(K.string, is->chave[i].string) > 0) {
			strcpy(novo->chave[j].pk, K.pk);
			strcpy(novo->chave[j].string, K.string);
			novo->desc[j+1] = filho_direito;
			chave_alocada = 1;
		}
		else {
			strcpy(novo->chave[j].pk, is->chave[i].pk);
			strcpy(novo->chave[j].string, is->chave[i].string);
			novo->desc[j+1] = is->desc[i+1];
			i--;
		}
	}

	if(!chave_alocada) { // A chave nova fica no node com as maiores chaves 
		while(i >= 0 && strcmp(K.string, is->chave[i].string) < 0) {
			is->chave[i+1] = is->chave[i];
			is->desc[i+2] = is->desc[i+1];
			i--; 
		}
		is->chave[i+1] = K;
		is->desc[i+2] = filho_direito;
	}
	strcpy(chave_promovida->pk, is->chave[(ordem_is/2)].pk);
	strcpy(chave_promovida->string, is->chave[(ordem_is/2)].string);
	novo->desc[0] = is->desc[(ordem_is/2) + 1];
	is->num_chaves = (ordem_is/2);
	write_btree(is, rrnIs, 0);
	// libera_no(is, 0);
	return novo;
}

void buscar(Indice iprimary, Indice ibrand) 
{
	Produto p;
	int argumento, rrn;
	Chave_is c;
	scanf("%d%*c", &argumento);

	switch(argumento) {
		case 1: // busca por codigo
			scanf("%10[^\n]", p.pk); getchar();
			printf(NOS_PERCORRIDOS_IP, p.pk);

			if(iprimary.raiz == -1) {
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}
			rrn = buscarComPrint(iprimary.raiz, p.pk);
			if(rrn != -1) {
				printf("\n");
				exibir_registro(rrn);
			}
			else {
				printf("\n");
				printf(REGISTRO_N_ENCONTRADO);
			}
		break;
		case 2:
    		scanf("%50[^\n]", p.marca); getchar();
			scanf("%50[^\n]", p.nome); getchar();
			sprintf(c.string, "%s$%s", p.marca, p.nome);
			printf(NOS_PERCORRIDOS_IS, c.string);
			c.pk[0] = '\0';
			if(ibrand.raiz == -1) {
				printf(REGISTRO_N_ENCONTRADO);
				return;
			}

			buscarIS(ibrand.raiz, &c);
			if(c.pk[0] != '\0') {
				printf("\n");
				rrn = buscarIP(iprimary.raiz, c.pk);
				exibir_registro(rrn);
			}
			else {
				printf("\n");
				printf(REGISTRO_N_ENCONTRADO);
			}
		break;
	}
}

void listar(Indice iprimary, Indice ibrand) 
{
	int argumento, rrn;
	scanf("%d%*c", &argumento);

	switch(argumento) {
		case 1: // Listar iprimary em pre-ordem
			if(nregistros < 1)
				printf(REGISTRO_N_ENCONTRADO);
			else 
				preorder(iprimary.raiz, 0);
		break;
		case 2: // Listar ibrand em-ordem
			if(nregistros < 1)
				printf(REGISTRO_N_ENCONTRADO);
			else 
				inorder(ibrand.raiz);
		break;
	}
}

void preorder(int rrn, int nivel)
{
	node_Btree_ip *no = read_btree(rrn, 1);

	if(no->folha == 'F') {
		int i = 0;
		while(i < no->num_chaves) {
			if(!i)
				printf("%d - %s", nivel + 1, no->chave[i].pk);
			else
				printf(", %s", no->chave[i].pk);
			i++;
		}
		printf("\n");
	}
	else {
		int i = 0;
		while(i < no->num_chaves) {
			if(!i)
				printf("%d - %s", nivel + 1, no->chave[i].pk);
			else
				printf(", %s", no->chave[i].pk);
			i++;
		}
		printf("\n");
		i = 0;
		while(i < (no->num_chaves + 1)) {
			preorder(no->desc[i], nivel + 1);
			i++;
		}
	}
	// libera_no(no, 1);
}

void inorder(int rrn)
{
	node_Btree_is *no = read_btree(rrn, 0);

	if(no->folha == 'F') {
		int i = 0;
		while(i < no->num_chaves) {
			char marca[TAM_MARCA + 1], nome[TAM_NOME + 1];
			sscanf(no->chave[i].string, "%[^$]", marca);
			sscanf(no->chave[i].string + strlen(marca) + 1, "%[^#]", nome);
			
			printf("%s", marca);
			if(strlen(marca) < TAM_MARCA - 1)
				for(int j = strlen(marca); j < TAM_MARCA - 1; j++)
					printf("-");
			printf(" %s", nome);
			if(strlen(nome) < TAM_NOME - 1)
				for(int j = strlen(nome); j < TAM_NOME - 1; j++)
					printf("-");
			printf("\n");
			i++;
		}
	}
	else {
		int i = 0;
		while(i <= no->num_chaves) {
			inorder(no->desc[i]);
			if(i < no->num_chaves) {
				char marca[TAM_MARCA + 1], nome[TAM_NOME + 1];
				sscanf(no->chave[i].string, "%[^$]", marca);
				sscanf(no->chave[i].string + strlen(marca) + 1, "%[^#]", nome);
				
				printf("%s", marca);
				if(strlen(marca) < TAM_MARCA - 1)
					for(int j = strlen(marca); j < TAM_MARCA - 1; j++)
						printf("-");
				printf(" %s", nome);
				if(strlen(nome) < TAM_NOME - 1)
					for(int j = strlen(nome); j < TAM_NOME - 1; j++)
						printf("-");
				printf("\n");
			}
			i++;
		}
	}
	// libera_no(no, 0);
}

int alterar(Indice iprimary) 
{
	char desconto[TAM_DESCONTO], pk[TAM_PRIMARY_KEY];
	scanf("%10[^\n]", pk); getchar();
	
	if(iprimary.raiz == -1) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

    int rrn = buscarIP(iprimary.raiz, pk), desc;

    if(rrn != -1) {
        while(1) {
            scanf("%[^\n]", desconto); getchar();
            sscanf(desconto, "%d", &desc);
            if(desc >= 0 && desc <= 100)
                break;
            else
                printf(CAMPO_INVALIDO);
        }
        char tmp[TAM_REGISTRO + 1];

        int cont = 6, i = 0;
        float precoNovo = -1;
        Produto p = recuperar_registro(rrn);

        i = sprintf(tmp, "%s@%s@%s@%s@%s@%s@%s@%s@", p.pk, p.nome, p.marca, p.data, p.ano, p.preco, desconto, p.categoria);
		int j;
    	if(i < TAM_REGISTRO) // Preenche o string com # ate acabar
        	for(j = i; j < TAM_REGISTRO; j++)
            	tmp[j] = '#';

        strncpy(ARQUIVO + (TAM_REGISTRO*rrn), tmp, TAM_REGISTRO);
        return 1;
    }
    else {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }

}

void libera_no(void *node, char ip)
{
	if(ip) {
		node_Btree_ip *no = (node_Btree_ip *) node;
		free(no->chave);
		free(no->desc);
		free(no);
	}
	else {
		node_Btree_is *no = (node_Btree_is *) node;
		free(no->chave);
		free(no->desc);
		free(no);
	}	
}
