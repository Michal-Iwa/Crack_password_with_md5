#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 7
#define NUMBER 101

pthread_mutex_t password_mutex;
pthread_cond_t password_found_cv;
int num_of_passwords_to_crack=0, how_many_left = 0, amount_cracked = 0;
int num_of_words_in_dict=0;
int count = 0;

struct base
{
        char password_md5[33];
        char login[65];
        char password[64];
        bool cracked;
        bool cracked2;
} to_crack[1025];
struct hash
{
        char word[64];
        char md5[33];
} * dictionary;

bool condition_password_not_found = true;

void sig_handler()
{
        printf("Znaleziono %d haseł", amount_cracked);
}

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
void *producer_dwuwyrazowy3(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char temp[64];
        char temp2[64];
        char tmp[64];
        char tmp2[64];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                strcpy(temp, dictionary[j].word);
                if (temp[0] > 96 && temp[0] < 123)
                        temp[0] = temp[0] - 32;
                for (int k = 0; k < num_of_words_in_dict && num_of_passwords_to_crack; k++)
                {
                        strcpy(temp2, dictionary[num_of_words_in_dict - 1-k].word);
                        if (temp2[0] > 96 && temp2[0] < 123)
                                temp2[0] = temp2[0] - 32;
                        for(int l=0;l<2;l++)
                        {
                                for(int m=0;m<3;m++)
                                {
                                        strcpy(tmp, temp);
                                        strcpy(tmp2, temp2);
                                        if(m==1 && l==0) {
                                               if (tmp2[0] > 64 && tmp2[0] < 91)
                                               {
                                                tmp2[0] = tmp2[0] + 32; 
                                                }
                                        }
                                        else if(m==1 && l==1) {
                                               if (tmp[0] > 64 && tmp[0] < 91)
                                                tmp[0] = tmp[0] + 32; 
                                        }
                                        else if(m==2 && l==0) {
                                                for (int i = 1; i < strlen(tmp2); i++)
                                                        if (tmp2[i] > 96 && tmp2[i] < 123)
                                                                tmp2[i] = tmp2[i] - 32; 
                                        }
                                        else if(m==2 && l==1) {
                                               for (int i = 1; i < strlen(tmp); i++)
                                                        if (tmp[i] > 96 && tmp[i] < 123)
                                                                tmp[i] = tmp[i] - 32; 
                                        }
                                        if (l==0) sprintf(word,"%s %s",tmp,tmp2);
                                        else if(l==1) sprintf(word,"%s %s",tmp2,tmp);
                                        bytes2md5(word, strlen(word), md5_word);
                                        strcat(md5_word, "\0");
                                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                                        {
                                                if (!to_crack[i].cracked)
                                                {
                                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                        {
                                                                pthread_mutex_lock(&password_mutex);
                                                                strcpy(to_crack[i].password, word);
                                                                to_crack[i].cracked = true;
                                                                to_crack[i].cracked2 = true;
                                                                how_many_left++;
                                                                amount_cracked++;
                                                                pthread_cond_signal(&password_found_cv);
                                                                pthread_mutex_unlock(&password_mutex);
                                                        }
                                                }
                                                pthread_cond_signal(&password_found_cv);
                                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                        }
                                        //nanosleep((const struct timespec[]){{0, 10000L}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}
void *producer_dwuwyrazowy2(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char temp[64];
        char temp2[64];
        char tmp[64];
        char tmp2[64];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                strcpy(temp, dictionary[j].word);
                for (int i = 0; i < strlen(temp); i++)
                {
                        if (temp[i] > 96 && temp[i] < 123)
                                temp[i] = temp[i] - 32;
                }
                for (int k = 0; k < num_of_words_in_dict && num_of_passwords_to_crack; k++)
                {
                        strcpy(temp2, dictionary[num_of_words_in_dict - 1-k].word);
                        for (int i = 0; i < strlen(temp2); i++)
                        {
                                if (temp2[i] > 96 && temp2[i] < 123)
                                        temp2[i] = temp2[i] - 32;
                        }
                        for(int l=0;l<2;l++)
                        {
                                for(int m=0;m<3;m++)
                                {
                                        strcpy(tmp, temp);
                                        strcpy(tmp2, temp2);
                                        if(m==1 && l==0) {
                                                for (int i = 0; i < strlen(tmp2); i++)
                                                        if (tmp2[i] > 64 && tmp2[i] < 91)
                                                                tmp2[i] = tmp2[i] + 32; 
                                        }
                                        else if(m==1 && l==1) {
                                               for (int i = 0; i < strlen(tmp); i++)
                                                        if (tmp[i] > 64 && tmp[i] < 91)
                                                                tmp[i] = tmp[i] + 32; 
                                        }
                                        else if(m==2 && l==0) {
                                                for (int i = 1; i < strlen(tmp2); i++)
                                                        if (tmp2[i] > 64 && tmp2[i] < 91)
                                                                tmp2[i] = tmp2[i] + 32; 
                                        }
                                        else if(m==2 && l==1) {
                                               for (int i = 1; i < strlen(tmp); i++)
                                                        if (tmp[i] > 64 && tmp[i] < 91)
                                                                tmp[i] = tmp[i] + 32; 
                                        }
                                        if (l==0) sprintf(word,"%s %s",tmp,tmp2);
                                        else if(l==1) sprintf(word,"%s %s",tmp2,tmp);
                                        bytes2md5(word, strlen(word), md5_word);
                                        strcat(md5_word, "\0");
                                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                                        {
                                                if (!to_crack[i].cracked)
                                                {
                                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                        {
                                                                pthread_mutex_lock(&password_mutex);
                                                                strcpy(to_crack[i].password, word);
                                                                to_crack[i].cracked = true;
                                                                to_crack[i].cracked2 = true;
                                                                how_many_left++;
                                                                amount_cracked++;
                                                                pthread_cond_signal(&password_found_cv);
                                                                pthread_mutex_unlock(&password_mutex);
                                                        }
                                                }
                                                pthread_cond_signal(&password_found_cv);
                                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                        }
                                        //nanosleep((const struct timespec[]){{0, 10000L}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}
void *producer_dwuwyrazowy1(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char temp[64];
        char temp2[64];
        char tmp[64];
        char tmp2[64];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                strcpy(temp, dictionary[j].word);
                for (int k = 0; k < num_of_words_in_dict && num_of_passwords_to_crack; k++)
                {
                        strcpy(temp2, dictionary[num_of_words_in_dict - 1-k].word);
                        for(int l=0;l<2;l++)
                        {
                                for(int m=0;m<3;m++)
                                {
                                        strcpy(tmp, temp);
                                        strcpy(tmp2, temp2);
                                        if(m==1 && l==0) {
                                               if (tmp2[0] > 96 && tmp2[0] < 123)tmp2[0] = tmp2[0] - 32; 
                                        }
                                        else if(m==1 && l==1) {
                                               if (tmp[0] > 96 && tmp[0] < 123) tmp[0] = tmp[0] - 32; 
                                        }
                                        else if(m==2 && l==0) {
                                                for (int i = 0; i < strlen(tmp2); i++)
                                                {
                                                        if (tmp2[i] > 96 && tmp2[i] < 123) tmp2[i] = tmp2[i] - 32; 
                                                }
                                        }
                                        else if(m==2 && l==1) {
                                               for (int i = 0; i < strlen(tmp); i++)
                                               {
                                                        if (tmp[i] > 96 && tmp[i] < 123) tmp[i] = tmp[i] - 32; 
                                               }
                                        }
                                        if (l==0) sprintf(word,"%s %s",tmp,tmp2);
                                        else if(l==1) sprintf(word,"%s %s",tmp2,tmp);
                                        bytes2md5(word, strlen(word), md5_word);
                                        strcat(md5_word, "\0");
                                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                                        {
                                                if (!to_crack[i].cracked)
                                                {
                                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                        {
                                                                pthread_mutex_lock(&password_mutex);
                                                                strcpy(to_crack[i].password, word);
                                                                to_crack[i].cracked = true;
                                                                to_crack[i].cracked2 = true;
                                                                how_many_left++;
                                                                amount_cracked++;
                                                                pthread_mutex_unlock(&password_mutex);
                                                        }
                                                }
                                                pthread_cond_signal(&password_found_cv);
                                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                        }
                                        //nanosleep((const struct timespec[]){{0, 10000L}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}
void *producer3(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char tmp[128];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                strcpy(tmp, dictionary[j].word);
                for (int i = 0; i < strlen(tmp); i++)
                {
                        if (tmp[i] > 96 && tmp[i] < 123)
                                tmp[i] = tmp[i] - 32;
                }
                for (int x = -1; x < NUMBER; x++)
                {
                        if (x < 0)
                                sprintf(word,"%s",tmp);
                        else
                        {
                                sprintf(word, "%s%d", tmp, x);
                        }
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }
                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int x = 0; x < NUMBER; x++)
                {
                        sprintf(word, "%d%s", x, tmp);
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }
                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int z = 0; z < NUMBER; z++)
                {
                        for (int x = 0; x < NUMBER; x++)
                        {
                                sprintf(word, "%d%s%d", x, tmp, z);
                                bytes2md5(word, strlen(word), md5_word);
                                strcat(md5_word, "\0");
                                for (int i = 0; i < num_of_passwords_to_crack; i++)
                                {
                                        if (!to_crack[i].cracked)
                                        {
                                                if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                {
                                                        pthread_mutex_lock(&password_mutex);
                                                        strcpy(to_crack[i].password, word);
                                                        to_crack[i].cracked = true;
                                                        to_crack[i].cracked2 = true;
                                                        how_many_left++;
                                                        amount_cracked++;
                                                        pthread_mutex_unlock(&password_mutex);
                                                }
                                        }
                                        pthread_cond_signal(&password_found_cv);
                                        //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}
void *producer2(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char tmp[128];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                printf("%s\n",dictionary[j].word);
                strcpy(tmp, dictionary[j].word);
                if (tmp[0] > 96 && tmp[0] < 123)
                        tmp[0] = tmp[0] - 32;
                for (int x = -1; x < NUMBER; x++)
                {
                        if (x < 0)
                                sprintf(word,"%s",tmp);
                        else
                        {
                                sprintf(word, "%s%d", tmp, x);
                        }
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }
                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int x = 0; x < NUMBER; x++)
                {
                        sprintf(word, "%d%s", x, tmp);
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }

                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int z = 0; z < NUMBER; z++)
                {
                        for (int x = 0; x < NUMBER; x++)
                        {
                                sprintf(word, "%d%s%d", x, tmp, z);
                                bytes2md5(word, strlen(word), md5_word);
                                strcat(md5_word, "\0");
                                for (int i = 0; i < num_of_passwords_to_crack; i++)
                                {
                                        if (!to_crack[i].cracked)
                                        {
                                                if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                {
                                                        pthread_mutex_lock(&password_mutex);
                                                        strcpy(to_crack[i].password, word);
                                                        to_crack[i].cracked = true;
                                                        to_crack[i].cracked2 = true;
                                                        how_many_left++;
                                                        amount_cracked++;
                                                        pthread_mutex_unlock(&password_mutex);
                                                }
                                        }
                                        pthread_cond_signal(&password_found_cv);
                                       // nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}
void *producer1(void *idp)
{
        long my_id = (long)idp;
        char word[256];
        char tmp[128];
        char md5_word[33];
        printf("Thread %ld started his work.\n",my_id);
        for (int j = 0; j < num_of_words_in_dict && num_of_passwords_to_crack; j++)
        {
                strcpy(tmp, dictionary[j].word);
                for (int x = -1; x < NUMBER; x++)
                {
                        if (x < 0)
                                sprintf(word,"%s",tmp);
                        else
                        {
                                sprintf(word, "%s%d", tmp, x);
                        }
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }
                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int x = 0; x < NUMBER; x++)
                {
                        sprintf(word, "%d%s", x, tmp);
                        bytes2md5(word, strlen(word), md5_word);
                        strcat(md5_word, "\0");
                        for (int i = 0; i < num_of_passwords_to_crack; i++)
                        {
                                if (!to_crack[i].cracked)
                                {
                                        if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                        {
                                                pthread_mutex_lock(&password_mutex);
                                                strcpy(to_crack[i].password, word);
                                                to_crack[i].cracked = true;
                                                to_crack[i].cracked2 = true;
                                                how_many_left++;
                                                amount_cracked++;
                                                pthread_mutex_unlock(&password_mutex);
                                        }
                                }
                                pthread_cond_signal(&password_found_cv);
                                //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                        }
                }
                for (int z = 0; z < NUMBER; z++)
                {
                        for (int x = 0; x < NUMBER; x++)
                        {
                                sprintf(word, "%d%s%d", x, tmp, z);
                                bytes2md5(word, strlen(word), md5_word);
                                strcat(md5_word, "\0");
                                for (int i = 0; i < num_of_passwords_to_crack; i++)
                                {
                                        if (!to_crack[i].cracked)
                                        {
                                                if (strcmp(md5_word, to_crack[i].password_md5) == 0)
                                                {
                                                        pthread_mutex_lock(&password_mutex);
                                                        strcpy(to_crack[i].password, word);
                                                        to_crack[i].cracked = true;
                                                        to_crack[i].cracked2 = true;
                                                        how_many_left++;
                                                        amount_cracked++;
                                                        
                                                        pthread_mutex_unlock(&password_mutex);
                                                }
                                        }
                                        pthread_cond_signal(&password_found_cv);
                                        //nanosleep((const struct timespec[]){{0, 1}}, NULL);
                                }
                        }
                }
        }
        count++;
        printf("Thread %ld ended his work.\n",my_id);
        pthread_cond_signal(&password_found_cv);
}

void *consumer(void *idp)
{
        long my_id = (long)idp;
        int j;
        //pthread_mutex_lock(&password_mutex);
        while (count < NUM_THREADS-1 || how_many_left) 
        {
                pthread_mutex_lock(&password_mutex);
                while (!how_many_left && count < NUM_THREADS-1)
                {
                        pthread_cond_wait(&password_found_cv, &password_mutex);
                }
                for (j = 0; to_crack[j].cracked2 == false && j< num_of_passwords_to_crack; j++)
                        ;
                if(to_crack[j].cracked2 == true) 
                {
                        printf("Password for %s is %s\n", to_crack[j].login, to_crack[j].password);
                        to_crack[j].cracked2 = false;
                        how_many_left--;
                }
                pthread_mutex_unlock(&password_mutex);
                //sleep(3);                    
                //nanosleep((const struct timespec[]){{0, 10000L}}, NULL);
        }
        //pthread_mutex_unlock(&password_mutex);
        printf("Znaleziono %d haseł. Koniec programu\n", amount_cracked);
}

int main()
{
        signal(SIGHUP, sig_handler);
        int SIZE = 127;
        char name_dictinary[128], name_login[128];
        FILE *dict, *login;
        char sign;
        int wrong = 0;
        do
        {
                printf("Write a name of file with potencial passwords: ");
                (void)scanf("%s", name_dictinary);
                strcat(name_dictinary, "\0");
                wrong++;
        } while ((dict = fopen(name_dictinary, "r")) == NULL && wrong < 10);
        if (wrong >= 10)
        {
                printf("Wrong filename too many times");
                exit(1);
        }
        wrong = 0;
        do
        {
                printf("Write a name of file with actuall passwords and logins: ");
                (void)scanf("%s", name_login);
                strcat(name_login, "\0");
                wrong++;
        } while ((login = fopen(name_login, "r")) == NULL && wrong < 10);
        if (wrong >= 10)
        {
                printf("Wrong filename too many times");
                exit(1);
        }
        struct hash *tmp;
        dictionary = calloc(sizeof(struct hash), SIZE);
        for (int i = 0;((sign = getc(dict)) != EOF); i++)
        {
                if (i == SIZE - 2)
                {
                        SIZE = SIZE * 2;
                        if (dictionary)
                        {
                                tmp = (struct hash *)realloc(dictionary, sizeof(struct hash) * (SIZE));
                                if (tmp)
                                        dictionary = tmp;
                        }
                }
                if(i==0) fseek(dict, -1, SEEK_CUR);
                (void)fscanf(dict, "%s", dictionary[i].word);
                num_of_words_in_dict++;
        }
        num_of_words_in_dict--;
        sign='0';
        fclose(dict);
        int skip = 0;
        for (int j = 0;(sign = getc(login)) != EOF; j++)
        {
                if(j==0) fseek(login, -1, SEEK_CUR);
                (void)fscanf(login, "%d", &skip);
                (void)fscanf(login, "%s", to_crack[j].password_md5);
                
                strcat(to_crack[j].password_md5, "\0");
                (void)fscanf(login, "%s", to_crack[j].login);
                while ((sign = getc(login)) != '\n');
                to_crack[j].cracked = false;
                to_crack[j].cracked2 = false;
                num_of_passwords_to_crack++;
        }
        fclose(login);
        printf("Starting threads\n");
        //Starting Thread work
        pthread_t threads[NUM_THREADS];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_mutex_init(&password_mutex, NULL);
        pthread_cond_init(&password_found_cv, NULL);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&threads[0], &attr, consumer, (void *)0);
        //nanosleep((const struct timespec[]){{0, 100}}, NULL);
        pthread_create(&threads[1], &attr, producer1, (void *)1);
        pthread_create(&threads[2], &attr, producer2, (void *)2);
        pthread_create(&threads[3], &attr, producer3, (void *)3);
        pthread_create(&threads[4], &attr, producer_dwuwyrazowy1, (void *)4);
        pthread_create(&threads[5], &attr, producer_dwuwyrazowy2, (void *)5);
        pthread_create(&threads[6], &attr, producer_dwuwyrazowy3, (void *)6);
        for (int k = 0; k < NUM_THREADS; k++)
        {
                pthread_join(threads[k], NULL);
        }
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&password_mutex);
        pthread_cond_destroy(&password_found_cv);
        pthread_exit(NULL);
        return (0);
        //const char *test="woman";
        //char md5[33]; // 32 characters + null terminator
        //bytes2md5(test, strlen(test), md5);
        //printf("%s ======================> %s\n", test, md5);
}
