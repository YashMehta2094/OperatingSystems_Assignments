#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 5000000
#define NUM_PRODS 5
#define NUM_CONS 5

int pipefd[2]; //shared pipe between producer and consumer

int vowel_count[NUM_CONS] = {0};

int producers_completed = 0;

char producer_buffers[NUM_PRODS][BUFFER_SIZE];
int buffer_index[NUM_PRODS] = {0};

pthread_mutex_t buffer_mutex;

int is_vowel(char c){
	return ((c=='a') || (c=='e') || (c=='i') || (c=='o') || (c=='u'));
}

void clear_files(){
	for(int i = 0; i<NUM_PRODS; i++){
		char filename[20];
		sprintf(filename, "prod_%d.txt", i);
		FILE *file = fopen(filename, "w");
		fclose(file);
	}
	for(int i = 0; i<NUM_CONS; i++){
		char filename[20];
		sprintf(filename, "cons_%d.txt", i);
		FILE *file = fopen(filename, "w");
		fclose(file);
	}
}

void* producer(void* thread_id){
	printf("Producer process begins\n");
	int id = *((int*)thread_id);
	int last_time = time(NULL);
	int start_time = time(NULL);
	close(pipefd[0]);
	
	while(1){
		int start_time_diff = ((time(NULL))*1 - start_time); 
		if(start_time_diff>=300){
			//printf("Entered prod print\n");
			char filename[20];
			sprintf(filename, "prod_%d.txt", id);
			FILE *file = fopen(filename, "a");
			for(int i = 0; i<buffer_index[id]; i++){
				fprintf(file, "%c\n", producer_buffers[id][i]);
			}
			fflush(file);
			fclose(file);
			buffer_index[id] = 0;
			last_time = time(NULL);
			break;
		}
		
		char ch = 'a' + rand()%26;
		
		pthread_mutex_lock(&buffer_mutex);
		
		write(pipefd[1], &ch, 1);
		
		//printf("%c",ch);
		
		pthread_mutex_unlock(&buffer_mutex);
		
		printf("Producer thread: %d produced %c\n", id, ch);
		
		producer_buffers[id][buffer_index[id]] = ch;
		
		buffer_index[id]++;
		
		int periodic_write_time = ((time(NULL))*1 - last_time);
		if(periodic_write_time>=60){
			char filename[20];
			sprintf(filename, "prod_%d.txt", id);
			FILE *file = fopen(filename, "a");
			for(int i = 0; i<buffer_index[id]; i++){
				fprintf(file, "%c\n", producer_buffers[id][i]);
			}
			fflush(file);
			fclose(file);
			
			buffer_index[id] = 0;
			
			last_time = time(NULL);
		}
	}
	producers_completed++;
	printf("Producer process ends\n");
}

void* consumer(void* thread_id){
	printf("Consumer process begins\n");
	int id = *((int*)thread_id);
	int last_time = time(NULL);
	char ch;
	close(pipefd[1]);
	
	while(read(pipefd[0], &ch, 1) > 0){
		
		printf("Consumer thread: %d consumed %c\n", id, ch);
		
		if(is_vowel(ch)){
			vowel_count[id]++;
		}
		
		/*if(producers_completed == 5){
			printf("Entered cons print\n");
			char filename[20];
			sprintf(filename, "cons_%d.txt", id);
			FILE *file = fopen(filename, "a");
			fprintf(file, "vowel count: %d\n", vowel_count[id]);
			fflush(file);
			fclose(file);
			last_time = time(NULL);
			break;
		}*/
	
		//sleep(60);
		int periodic_write_time = ((time(NULL))*1 - last_time);
		if(periodic_write_time>=60){
			//printf("Entered cons print\n");
			char filename[20];
			sprintf(filename, "cons_%d.txt", id);
			FILE *file = fopen(filename, "a");
			fprintf(file, "vowel count: %d\n", vowel_count[id]);
			fflush(file);
			fclose(file);
			last_time = time(NULL);
		}
	}
	char filename[20];
	sprintf(filename, "cons_%d.txt", id);
	FILE *file = fopen(filename, "a");
	fprintf(file, "vowel count: %d\n", vowel_count[id]);
	fflush(file);
	fclose(file);
	last_time = time(NULL);
	printf("Consumer process ends\n");
	//return NULL;
	
}

int main(){
	
	setbuf(stdout, NULL);
	
	pid_t pid;
	pthread_t producers[NUM_PRODS], consumers[NUM_CONS];
	int thread_id[NUM_PRODS + NUM_CONS];
	
	srand(time(NULL));
	
	//pipe for sharedbuffer
	pipe(pipefd);
	
	clear_files();
	
	pthread_mutex_init(&buffer_mutex, NULL);
	
	pid = fork();
	
	if(pid == 0){
		//Producer process
		for(int i = 0; i<NUM_PRODS; i++){
			thread_id[i] = i;
			pthread_create(&producers[i], NULL, producer, &thread_id[i]);
		}
		
		for(int i = 0; i<NUM_PRODS; i++){
			pthread_join(producers[i], NULL);
		}
		
	}
	else{
		//Consumer process
		
		for(int i = 0; i<NUM_CONS; i++){
			thread_id[i + NUM_PRODS] = i;
			pthread_create(&consumers[i], NULL, consumer, &thread_id[i + NUM_PRODS]);
		}
		
		for(int i = 0; i<NUM_CONS; i++){
			pthread_join(consumers[i], NULL);
		}
		
		
		//wait(NULL);
	}
	
	//Destroy mutexes and semaphores
	pthread_mutex_destroy(&buffer_mutex);
	
	return 0;
}
