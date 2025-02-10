#include "markov_chain.h"
#include <string.h>

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"
#define MAX_LINE 1000
#define MIN_ARGC 4
#define MAX_ARGC 5
#define MAX_TWEET 20

#define MAX_WORD_TO_READ -1
#define DEC_FORMAT 10

static void print_func(const void *a)
{
    const char *data = (char *)a;
    printf("%s", data);
}

static int comp_func(const void *a, const void *b)
{
    const char *data_a = (const char *)a;
    const char *data_b = (const char *)b;
    return strcmp(data_a, data_b);
}

static void *copy_func(const void *a)
{
    const char *data = (const char *)a;
    char *word = malloc(strlen(data) + 1);
    if (!word)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    strcpy(word, data);
    return word;
}

static bool check_is_last(const void *a)
{
    const char *word = (const char *)a;
    return (word[strlen(word) - 1] == '.');
}

static void free_func(const void *data)
{
    MarkovChain **ptr_chain = (MarkovChain **)data;
    free_markov_chain(ptr_chain);
}

static int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
    char line[MAX_LINE];
    int words_count = 0;
    while (fgets(line, MAX_LINE, fp) != NULL)
    {
        char *token = strtok(line, DELIMITERS);
        Node *prev = NULL;
        while (token)
        {
            Node *new_node = add_to_database(markov_chain, token);
            if (!new_node)
            {
                printf(ALLOCATION_ERROR_MASSAGE);
                return EXIT_FAILURE;
            }
            words_count++;
            if (prev != NULL)
            {
                add_node_to_frequency_list(prev->data, new_node->data, markov_chain);
            }
            prev = new_node;
            if (words_to_read != MAX_WORD_TO_READ && words_count == words_to_read)
            {
                fclose(fp);
                return EXIT_SUCCESS;
            }
            token = strtok(NULL, DELIMITERS);
        }
    }
    fclose(fp);
    return EXIT_SUCCESS;
}

static MarkovChain *create_chain(void)
{
    MarkovChain *chain = malloc(sizeof(MarkovChain));
    if (!chain)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    chain->database = malloc(sizeof(LinkedList));
    if (!chain->database)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        free(chain);
        chain = NULL;
        return NULL;
    }

    chain->database->first = NULL;
    chain->database->last = NULL;
    chain->database->size = 0;
    chain->comp_func = comp_func;
    chain->copy_func = copy_func;
    chain->free_data = free_func;
    chain->is_last = check_is_last;
    chain->print_func = print_func;
    return chain;
}

int main(int argc, char *argv[])
{
    // checks input
    if (argc != MAX_ARGC && argc != MIN_ARGC)
    {
        printf("%s", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    FILE *fp = fopen(argv[3], "r");
    if (fp == NULL)
    {
        printf("%s", FILE_PATH_ERROR);
        fclose(fp);
        return EXIT_FAILURE;
    }
    int seed, num_of_tweets;
    sscanf(argv[1], "%d", &seed);
    srand(seed);
    sscanf(argv[2], "%d", &num_of_tweets);
    int words_to_read = MAX_WORD_TO_READ;
    if (argc == MAX_ARGC)
    {
        sscanf(argv[4], "%d", &words_to_read);
    }
    MarkovChain *chain = create_chain();
    if (!chain)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    int result = fill_database(fp, words_to_read, chain);
    if (result == EXIT_FAILURE)
    {
        free_markov_chain(&chain);
        printf(ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    for (int i = 1; i <= num_of_tweets; ++i)
    {
        printf("Tweet %d: ", i);
        generate_random_sequence(chain, NULL, MAX_TWEET);
        printf("\n");
    }
    free_markov_chain(&chain);
    return EXIT_SUCCESS;
}
