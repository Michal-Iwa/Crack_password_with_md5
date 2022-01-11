#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS  4

pthread_mutex_t password_mutex;
pthread_cond_t password_found_cv;
int num_of_passwords_to_crack,how_many_left=0;
int num_of_words_in_dict;
int count=0;

struct base{
        char password_md5[33];
        char login[65];
        char password[64];
        bool cracked;
        bool cracked2;
}to_crack[1025];
struct hash{
        char word[64];
        char md5[33];
}*dictionary;

bool condition_password_not_found=true;

void bytes2md5(const char *data, int len, char *md5buf)
{
  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, data, len);
        EVP_DigestFinal_ex(mdctx, md_value, &md_len);
        EVP_MD_CTX_free(mdctx);
        for (i = 0; i < md_len; i++)
        {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
        }
}
void *producer3(void *idp) 
{      
        long my_id = (long)idp;
        char word[64];
        char md5_word[33];
        for(int j=0;j<num_of_words_in_dict && num_of_passwords_to_crack;j++)
        {
                strcpy(word,dictionary[j].word);
                for(int i=0;i<strlen(word);i++)
                {
                        word[i]=word[i]-32;
                }
                bytes2md5(word, strlen(word), md5_word);
                strcat(md5_word,"\0");
                for(int i=0;i<num_of_passwords_to_crack;i++)
                {
                        if(!to_crack[i].cracked)
                        {
                                
                                if(strcmp(md5_word,to_crack[i].password_md5)==0)
                                {
                                        pthread_mutex_lock(&password_mutex);
                                        strcpy(to_crack[i].password, md5_word);
                                        to_crack[i].cracked = true;
                                        to_crack[i].cracked2=true;
                                        how_many_left++;
                                        pthread_mutex_unlock(&password_mutex);
                                }
                                
                        }
                        printf("watek%ld, %d\n",my_id,j);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                        nanosleep((const struct timespec[]){{0, 10}}, NULL);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                        
                }
        }
        count++;
        printf("xD3");
        pthread_exit (NULL);
}
void *producer2(void *idp) 
{      
        long my_id = (long)idp;
        char word[64];
        char md5_word[33];
        for(int j=0;j<num_of_words_in_dict && num_of_passwords_to_crack;j++)
        {
                strcpy(word,dictionary[j].word);
                word[0]=word[0]-32;
                bytes2md5(word, strlen(word), md5_word);
                strcat(md5_word,"\0");
                for(int i=0;i<num_of_passwords_to_crack;i++)
                {
                        if(!to_crack[i].cracked)
                        {
                                if(strcmp(md5_word,to_crack[i].password_md5)==0)
                                {
                                        pthread_mutex_lock(&password_mutex);
                                        strcpy(to_crack[i].password, md5_word);
                                        to_crack[i].cracked = true;
                                        to_crack[i].cracked2=true;
                                        how_many_left++;
                                        pthread_mutex_unlock(&password_mutex);
                                }
                                
                        }
                        printf("watek%ld, %d\n",my_id,j);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                        nanosleep((const struct timespec[]){{0, 10}}, NULL);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                }
                
        }
        count++;
        printf("xD2");
        pthread_exit (NULL);
}
void *producer1(void *idp) 
{      
        long my_id = (long)idp;
        for(int j=0;j<num_of_words_in_dict && num_of_passwords_to_crack;j++)
        {
                bytes2md5(dictionary[j].word, strlen(dictionary[j].word), dictionary[j].md5);
                strcat(dictionary[j].md5,"\0");
                for(int i=0;i<num_of_passwords_to_crack;i++)
                {
                        
                        if(!to_crack[i].cracked)
                        {
                                
                                if(strcmp(dictionary[j].md5,to_crack[i].password_md5)==0)
                                {
                                        pthread_mutex_lock(&password_mutex);
                                        strcpy(to_crack[i].password, dictionary[j].word);
                                        to_crack[i].cracked = true;
                                        to_crack[i].cracked2=true;
                                        how_many_left++;
                                        pthread_mutex_unlock(&password_mutex);
                                        
                                }
                                
                        }
                        printf("watek%ld, %d\n",my_id,j);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                        nanosleep((const struct timespec[]){{0, 10}}, NULL);
                        pthread_cond_signal(&password_found_cv);
                        printf("watek%ld, %d\n",my_id,j);
                }
        }
        count++;
        printf("xD1");
        pthread_exit (NULL);
}
// Nowy pomysł:
//stworzyć kilku konsumentów






void *consumer(void *idp) 
{
        long my_id = (long)idp;
        int j;
        int crackedplusone=1;
        while(count<NUM_THREADS-1 || how_many_left)//muszę tutaj dodać warunek jak wszystkie wątki skończą szukać to wyjść
        {
                pthread_mutex_lock(&password_mutex);
                while(!how_many_left){
                        printf("czekam");
                pthread_cond_wait(&password_found_cv, &password_mutex);
                }
                for(j=0;to_crack[j].cracked2==false;j++);
                printf("Password for %s is %s\n",to_crack[j].login,to_crack[j].password);
                to_crack[j].cracked2=false;
                how_many_left--;
                pthread_mutex_unlock(&password_mutex);
                //nanosleep((const struct timespec[]){{0, 10}}, NULL);
        }
        printf("done");
        pthread_exit (NULL);
}

int main()
{
        int SIZE=128;
        char name_dictinary[128],name_login[128];
        FILE *dict,*login;
        char sign;
        int wrong=0;
        do
        {
        printf("Write a name of file with potencial passwords: ");
        (void)scanf("%s",name_dictinary);
        strcat(name_dictinary,"\0");
        wrong++;
        }while( (dict = fopen(name_dictinary, "r") ) == NULL && wrong<10);
        if (wrong>=10) {
                printf("Wrong filename too many times");
                exit(1);
        }
        wrong=0;
        do
        {
        printf("Write a name of file with actuall passwords and logins: ");
        (void)scanf("%s",name_login);
        strcat(name_login,"\0");
        wrong++;
        }while( (login = fopen(name_login, "r") ) == NULL && wrong<10);
        if (wrong>=10) {
                printf("Wrong filename too many times");
                exit(1);
        }
        struct hash *tmp;
        dictionary=calloc(sizeof(struct hash),SIZE);
        int i=0;
        for(i=0;(sign = getc(dict)) != EOF;i++)
        {
                if(i==SIZE-1){
                        SIZE=SIZE*2;
                        if (dictionary) {
				tmp = (struct hash*)realloc(dictionary, sizeof(struct hash) * (SIZE));
				if (tmp) dictionary = tmp;
			}
                }
                fseek(dict, -1, SEEK_CUR);
                (void)fscanf(dict, "%s", dictionary[i].word);
        }
        num_of_words_in_dict=i;
        fclose(dict);
        int skip=0;
        for(i=0;(sign = getc(login)) != EOF && (sign = getc(login)) != EOF;i++)
        {
                fseek(login, -2, SEEK_CUR);
                (void)fscanf(login, "%d", &skip);
                (void)fscanf(login, "%s", to_crack[i].password_md5);
                strcat(to_crack[i].password_md5,"\0");
                (void)fscanf(login, "%s", to_crack[i].login);
                while((sign = getc(login)) != '\n');
                to_crack[i].cracked=false;
                to_crack[i].cracked2=false;
        }
        num_of_passwords_to_crack=i;
        fclose(login);
        //Starting Thread work
        pthread_t threads[NUM_THREADS];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_mutex_init(&password_mutex, NULL);
        pthread_cond_init (&password_found_cv, NULL);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&threads[0], &attr, consumer, (void *)0);
        pthread_create(&threads[1], &attr, producer1, (void *)1);
        pthread_create(&threads[2], &attr, producer2, (void *)2);
        pthread_create(&threads[3], &attr, producer3, (void *)3);
        for (i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
        }
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&password_mutex);
        pthread_cond_destroy(&password_found_cv);
        pthread_exit (NULL);
        return(0);
        //const char *test="woman";
        //char md5[33]; // 32 characters + null terminator                        
        //bytes2md5(test, strlen(test), md5);
        //printf("%s ======================> %s\n", test, md5);
}

