#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define NUM_THREADS  2

pthread_mutex_t password_mutex;
pthread_cond_t password_found_cv;
int num_of_passwords_to_crack;
int num_of_words_in_dict;

struct base{
        char password[33];
        char login[65];
        bool cracked;
}to_crack[1025];
struct hash{
        char word[64];
        char md5[33];
}*dictionary;

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
                                if(strcmp(dictionary[j].md5,to_crack[i].password)==0)
                                {
                                     printf("%s\n%s\n%s",dictionary[j].md5,to_crack[i].password,dictionary[j].word);
                                }
                        }       
                }
        }
        pthread_exit (NULL);
}
void *consumer(void *idp) 
{
        long my_id = (long)idp;
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
                printf("%s\n",dictionary[i].word);
        }
        num_of_words_in_dict=i;
        fclose(dict);
        int skip=0;
        for(i=0;(sign = getc(login)) != EOF;i++)
        {
                fseek(login, -1, SEEK_CUR);
                (void)fscanf(login, "%d", &skip);
                (void)fscanf(login, "%s", to_crack[i].password);
                strcat(to_crack[i].password,"\0");
                (void)fscanf(login, "%s", to_crack[i].login);
                while((sign = getc(login)) != '\n');
                printf("%s\n",to_crack[i].password);
                printf("%s\n",to_crack[i].login);
                to_crack[i].cracked=false;
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
        for (i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
        }
        pthread_attr_destroy(&attr);
        pthread_exit (NULL);
        //const char *test="woman";
        //char md5[33]; // 32 characters + null terminator                        
        //bytes2md5(test, strlen(test), md5);
        //printf("%s ======================> %s\n", test, md5);
}

