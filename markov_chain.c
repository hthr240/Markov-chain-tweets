#include "markov_chain.h"
#define MAX_TWEET 20

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    if (!(markov_chain->database) || !(markov_chain->database->first))
    {
        return NULL;
    }

    Node *index_node = markov_chain->database->first;

    while (index_node)
    {
        if (markov_chain->comp_func(index_node->data->data, data_ptr) == EXIT_SUCCESS)
        {
            return index_node;
        }
        index_node = index_node->next;
    }
    return NULL;
}

Node *add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    // checks if data in database
    Node *check_node = get_node_from_database(markov_chain, data_ptr);
    if (check_node)
    {
        return check_node;
    }
    // allocate new markovNode
    MarkovNode *new_markov_node = (MarkovNode *)malloc(sizeof(MarkovNode));
    if (!new_markov_node)
    {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    new_markov_node->data = markov_chain->copy_func(data_ptr);
    new_markov_node->fre_list_size = 0;
    new_markov_node->frequency_list = (MarkovNodeFrequency **)malloc(sizeof(MarkovNodeFrequency));
    if (!(new_markov_node->frequency_list))
    {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    int allocate_result = add(markov_chain->database, new_markov_node);
    if (allocate_result == EXIT_FAILURE)
    {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node, MarkovChain *markov_chain)
{
    // checks if second node inside first node freq list
    for (int i = 0; i < first_node->fre_list_size; ++i)
    {
        if (markov_chain->comp_func(first_node->frequency_list[i]->markov_node->data, second_node->data) == EXIT_SUCCESS)
        {
            first_node->frequency_list[i]->frequency++;
            return EXIT_SUCCESS;
        }
    }

    MarkovNodeFrequency *new_markov_freq_node;
    new_markov_freq_node = (MarkovNodeFrequency *)(malloc(sizeof(MarkovNodeFrequency)));
    if (!new_markov_freq_node)
    {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    new_markov_freq_node->frequency = 1;
    new_markov_freq_node->markov_node = second_node;
    first_node->fre_list_size++;

    MarkovNodeFrequency **new_freq_list = realloc(first_node->frequency_list, sizeof(MarkovNodeFrequency) * first_node->fre_list_size);
    if (!new_freq_list)
    {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    else
    {
        new_freq_list[(first_node->fre_list_size) - 1] = new_markov_freq_node;
        first_node->frequency_list = new_freq_list;
        return EXIT_SUCCESS;
    }
}

void free_markov_chain(MarkovChain **ptr_chain)
{
    Node *index_node = (*ptr_chain)->database->first;
    while (index_node)
    {
        for (int i = 0; i < index_node->data->fre_list_size; ++i)
        {
            free(index_node->data->frequency_list[i]);
        }
        // free index_node fields
        free(index_node->data->frequency_list);
        free(index_node->data->data);
        free(index_node->data);

        // get the pointer to next node and stores it
        Node *next_node = index_node->next;
        free(index_node);
        index_node = next_node;
    }

    free((*ptr_chain)->database);
    free(*ptr_chain);
    *ptr_chain = NULL;
}

MarkovNode *get_first_random_node(MarkovChain *markov_chain)
{
    MarkovNode *markov_node = NULL;
    bool check = true;

    while (check)
    {
        int max_num = markov_chain->database->size;
        int rand_num = get_random_number(max_num);

        Node *cur_word = markov_chain->database->first;

        int index = 0;
        while (index < rand_num)
        {
            cur_word = cur_word->next;
            index++;
        }
        if (markov_chain->is_last(cur_word->data->data))
        {
            check = true;
        }
        else
        {
            check = false;
            markov_node = cur_word->data;
        }
    }

    return markov_node;
}

MarkovNode *get_next_random_node(MarkovNode *cur_markov_node)
{
    if (!cur_markov_node->fre_list_size)
    {
        return EXIT_SUCCESS;
    }
    // get total freq
    int total = 0;
    for (int i = 0; i < cur_markov_node->fre_list_size; ++i)
    {
        total += (int)cur_markov_node->frequency_list[i]->frequency;
    }
    int rand_num = get_random_number(total);
    // choosing randomly according to freq
    for (int i = 0; i < (int)cur_markov_node->fre_list_size; ++i)
    {
        if (rand_num < (int)cur_markov_node->frequency_list[i]->frequency)
        {
            return cur_markov_node->frequency_list[i]->markov_node;
        }
        rand_num -= (int)cur_markov_node->frequency_list[i]->frequency;
    }
    MarkovNode *next_rand_node = cur_markov_node->frequency_list[get_random_number(cur_markov_node->fre_list_size)]->markov_node;
    return next_rand_node;
}

void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *first_node, int max_length)
{
    if (!first_node)
    {
        first_node = get_first_random_node(markov_chain);
    }

    markov_chain->print_func(first_node->data);
    printf(" ");

    for (int i = 0; i < max_length - 1; ++i)
    {
        MarkovNode *next_node = get_next_random_node(first_node);
        markov_chain->print_func(next_node->data);
        if (markov_chain->is_last(next_node->data))
        {
            break;
        }
        printf(" ");
        first_node = next_node;
    }
}
