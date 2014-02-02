/*
2011.2.04005.11 Diego da Silva da Rocha
2011.2.04035.11 Luan Henrique Moreira Azevedo
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include </usr/include/semaphore.h>

#define BUFF_SIZE  5
#define MM_THREADS 3
#define SC_THREADS 2
#define CF_THREADS 2

typedef struct {
	char *nome;
	int ordem;
	double **a;
	double **b;
	double **c;
	double *v;
	double e;
} s_t;

typedef struct {
    s_t *buf[BUFF_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    sem_t mutex;
} sbuf_t;

sbuf_t shared[5];

void destruir(s_t *s)
{
	if(s != NULL)
	{
		free(s->nome);
		free(s->a);
		free(s->b);
		free(s->c);
		free(s->v);
		free(s);
	}
}

void escreveMatrixArquivo(FILE *f, double **m, int ordem)
{
	for(int i = 0; i < ordem; i++)
  {
  	for(int j = 0; j < ordem; j++)
  	{
  		fprintf(f, "%lf ", m[i][j]);
  	}
  	fputs("\n", f);
  }
  fputs("--------------------------------\n", f);
}

void *la(void *arg)
{
	FILE *f;
	char nome[255];
	f = fopen("bin/entrada.in", "r");
	for(int i=0; i<25; i++)
	{
		s_t *s;
		FILE *fp;
		int size;
		//Cria dinamicamente s
		s = (s_t*) malloc(sizeof(s_t));
		
		//Le o nome do arquivo
		fgets(nome, 255, f);
		
		//Salva o nome do arquivo em s
		size = strlen(nome);
		s->nome = (char*) malloc(sizeof(char) * size);
		strncpy(s->nome, nome, size-1);
		
		//Abre o arquivo de entrada
		fp = fopen(s->nome, "r");

		//Lê a ordem das matrizes
		fscanf(fp, "%d", &s->ordem);
		
		//Cria as matrizes e vetores
		s->a = (double **) malloc(sizeof(double*) * s->ordem);
		s->b = (double **) malloc(sizeof(double*) * s->ordem);
		s->c = (double **) malloc(sizeof(double*) * s->ordem);
		s->v = (double*) malloc(sizeof(double) * s->ordem);
		for(int i=0; i< s->ordem; i++)
		{
			s->a[i] = (double*) malloc(sizeof(double) * s->ordem);
			s->b[i] = (double*) malloc(sizeof(double) * s->ordem);
			s->c[i] = (double*) malloc(sizeof(double) * s->ordem);
			memset(s->a[i], s->ordem, 0);
			memset(s->b[i], s->ordem, 0);
			memset(s->c[i], s->ordem, 0);
		}
		
		//Lê a primeira matriz
		for(int i=0; i < s->ordem; i++)
		{
			for(int j=0; j < s->ordem; j++)
			{
				fscanf(fp, "%lf", &s->a[i][j]);
			}
		}
		
		//Lê a segunda matriz
		for(int i=0; i < s->ordem; i++)
		{
			for(int j=0; j < s->ordem; j++)
			{
				fscanf(fp, "%lf", &s->b[i][j]);
			}
		}
		
		//Fecha o arquivo de entrada
		fclose(fp);
		
		//Semáforos de compartilhamento com MM
    sem_wait(&shared[0].empty);
    sem_wait(&shared[0].mutex);
    shared[0].buf[shared[0].in] = s;
    shared[0].in = (shared[0].in+1)%BUFF_SIZE;
    sem_post(&shared[0].mutex);
    sem_post(&shared[0].full);
	}
	fclose(f);
	return NULL;
}

void *mm(void *arg)
{
    s_t *item;
    while(1) 
    {
    	//Semáforos de compartilhamento com LA
      sem_wait(&shared[0].full);
      sem_wait(&shared[0].mutex);
      item = shared[0].buf[shared[0].out];
      shared[0].out = (shared[0].out+1)%BUFF_SIZE;
      sem_post(&shared[0].mutex);
      sem_post(&shared[0].empty);
   	
   		//Processa
			for(int i = 0; i < item->ordem; i++)
			{
				for(int j = 0; j < item->ordem; j++)
				{
				  item->c[i][j] = 0;
				  for(int k = 0; k< item->ordem; k++)
				  {
				      item->c[i][j] += item->a[i][k] * item->b[k][j];
				  }
				}
			}

			//Semáforos de compartilhamento com SC
      sem_wait(&shared[1].empty);
      sem_wait(&shared[1].mutex);
      shared[1].buf[shared[1].in] = item;
      shared[1].in = (shared[1].in+1)%BUFF_SIZE;
      sem_post(&shared[1].mutex);
      sem_post(&shared[1].full);
    }
    return NULL;
}

void *sc(void *arg)
{
    s_t *item;
    while(1) 
    {
    
    	//Semáforos de compartilhamento com MM
      sem_wait(&shared[1].full);
      sem_wait(&shared[1].mutex);
      item = shared[1].buf[shared[1].out];
      shared[1].out = (shared[1].out+1)%BUFF_SIZE;
      sem_post(&shared[1].mutex);
      sem_post(&shared[1].empty);

			//Processa
			for(int i=0; i < item->ordem; i++)
			{
				item->v[i] = 0;
				for(int j=0; j < item->ordem; j++)
				{
					item->v[i] += item->c[j][i];
				}
			}

			//Semáforos de compartilhamento com CF
      sem_wait(&shared[2].empty);
      sem_wait(&shared[2].mutex);
      shared[2].buf[shared[2].in] = item;
      shared[2].in = (shared[2].in+1)%BUFF_SIZE;
      sem_post(&shared[2].mutex);
      sem_post(&shared[2].full);
    }
    return NULL;
}

void *cf(void *arg)
{
    s_t *item;
    while(1) 
    {
    	//Semáforos de compartilhamento com SC
      sem_wait(&shared[2].full);
      sem_wait(&shared[2].mutex);
      item = shared[2].buf[shared[2].out];
      shared[2].out = (shared[2].out+1)%BUFF_SIZE;
      sem_post(&shared[2].mutex);
      sem_post(&shared[2].empty);
			
			//Processa
			item->e = 0;
			for(int i=0; i < item->ordem; i++)
			{
				item->e += item->v[i];
			}

			//Semáforos de compartilhamento com EA
      sem_wait(&shared[3].empty);
      sem_wait(&shared[3].mutex);
      shared[3].buf[shared[3].in] = item;
      shared[3].in = (shared[3].in+1)%BUFF_SIZE;
      sem_post(&shared[3].mutex);
      sem_post(&shared[3].full);
    }
    return NULL;
}

void *ea(void *arg)
{
    s_t *item;
    FILE *f;
    f = fopen("bin/saida.out", "w");
    for (int i=0; i < 25; i++) 
    {
		  //Semáforos de compartilhamento com CF
		  sem_wait(&shared[3].full);
		  sem_wait(&shared[3].mutex);
		  item = shared[3].buf[shared[3].out];
		  shared[3].out = (shared[3].out+1)%BUFF_SIZE;
		  sem_post(&shared[3].mutex);
		  sem_post(&shared[3].empty);
		  
		  //Escreve no arquivo de saida
		  fprintf(f, "%s - %d\n--------------------------------\n", item->nome, item->ordem);
		  escreveMatrixArquivo(f, item->a, item->ordem);
		  escreveMatrixArquivo(f, item->b, item->ordem);
		  escreveMatrixArquivo(f, item->c, item->ordem);
		  for(int i=0; i < item->ordem; i++)
		  {
		  		fprintf(f, "%lf\n", item->v[i]);
		  }
		  fputs("--------------------------------\n", f);
		  fprintf(f, "%lf\n================================\n", item->e);
		  
		  destruir(item);
    }
    //Fecha arquivo
    fclose(f);
    
    //Encerra execução
    return NULL;
}

int main()
{
  pthread_t idLa, idMm, idSc, idCf, idEa;

	//Inicia os semáforos
  sem_init(&shared[0].full, 0, 0);
  sem_init(&shared[0].empty, 0, BUFF_SIZE);
  sem_init(&shared[0].mutex, 0, 1);
  sem_init(&shared[1].full, 0, 0);
  sem_init(&shared[1].empty, 0, BUFF_SIZE);
  sem_init(&shared[1].mutex, 0, 1);
  sem_init(&shared[2].full, 0, 0);
  sem_init(&shared[2].empty, 0, BUFF_SIZE);
  sem_init(&shared[2].mutex, 0, 1);
  sem_init(&shared[3].full, 0, 0);
  sem_init(&shared[3].empty, 0, BUFF_SIZE);
  sem_init(&shared[3].mutex, 0, 1);
  
  //Cria a thread LA
  pthread_create(&idLa, NULL, la, NULL);
  
  //Cria as threads MM
  for(int i=0; i < MM_THREADS; i++)
  {
		pthread_create(&idMm, NULL, mm, NULL);
	}
	
	//Cria as threads SC
	for(int i=0; i < SC_THREADS; i++)
	{
		pthread_create(&idSc, NULL, sc, NULL);
	}
	
	//Cria as threads CF
	for(int i=0; i < CF_THREADS; i++)
	{
		pthread_create(&idCf, NULL, cf, NULL);
	}
	
	//Cria a thread EA
	pthread_create(&idEa, NULL, ea, NULL);
 
 	//Espera a thread EA terminar 
  pthread_join(idEa, NULL);
  
  //Encerra a execução
	return EXIT_SUCCESS;
}
