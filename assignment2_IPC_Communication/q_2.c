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
#define NUM_CONS 5

int pipefd[2]; //shared pipe between producer and consumer

int vowel_count[NUM_CONS] = {0};

char producer_buffer[BUFFER_SIZE];
int buffer_index = 0;


int is_vowel(char c){
	return ((c=='a') || (c=='e') || (c=='i') || (c=='o') || (c=='u'));
}

void clear_files(){
	
	char filename[20];
	sprintf(filename, "prod_0.txt");
	FILE *file = fopen(filename, "w");
	fclose(file);

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
			for(int i = 0; i<buffer_index; i++){
				fprintf(file, "%c\n", producer_buffer[i]);
			}
			fflush(file);
			fclose(file);
			buffer_index = 0;
			last_time = time(NULL);
			break;
		}
		
		char ch = 'a' + rand()%26;
		
		write(pipefd[1], &ch, 1);
		
		//printf("%c",ch);
		
		printf("Producer thread: %d produced %c\n", id, ch);
		
		producer_buffer[buffer_index] = ch;
		
		buffer_index++;
		
		int periodic_write_time = ((time(NULL))*1 - last_time);
		if(periodic_write_time>=60){
			char filename[20];
			sprintf(filename, "prod_%d.txt", id);
			FILE *file = fopen(filename, "a");
			for(int i = 0; i<buffer_index; i++){
				fprintf(file, "%c\n", producer_buffer[i]);
			}
			fflush(file);
			fclose(file);
			
			buffer_index = 0;
			
			last_time = time(NULL);
		}
	}
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
	pthread_t producers, consumers[NUM_CONS];
	int thread_id[1 + NUM_CONS];
	
	srand(time(NULL));
	
	//pipe for sharedbuffer
	pipe(pipefd);
	
	clear_files();
	
	pid = fork();
	
	if(pid == 0){
		//Producer process
		
		thread_id[0] = 0;
		pthread_create(&producers, NULL, producer, &thread_id[0]);
		
		pthread_join(producers, NULL);
		
	}
	else{
		//Consumer process
		
		for(int i = 0; i<NUM_CONS; i++){
			thread_id[i + 1] = i;
			pthread_create(&consumers[i], NULL, consumer, &thread_id[i + 1]);
		}
		
		for(int i = 0; i<NUM_CONS; i++){
			pthread_join(consumers[i], NULL);
		}
		
		
		//wait(NULL);
	}
	
	//Destroy mutexes and semaphores
	
	return 0;
}
