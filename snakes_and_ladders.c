#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX_LINE_LEN 1000
#define ARGS 3
#define BASE 10

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8, 30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number;    // Cell number 1-100
    int ladder_to; // ladder_to represents the jump of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there is one from this square
    // both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
int handle_error_snakes(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}

int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            handle_error_snakes(ALLOCATION_ERROR_MASSAGE, NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int fill_database_snakes(MarkovChain *markov_chain)
{
    Cell *cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])->data;
            add_node_to_frequency_list(from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell *)(from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node, markov_chain);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

static void free_func(const void *data)
{
    MarkovChain **ptr_chain = (MarkovChain **)data;
    free_markov_chain(ptr_chain);
}

static void *copy_func(const void *a)
{
    const Cell *data = (const Cell *)a;
    Cell *cell = malloc(sizeof(Cell));
    if (!cell)
    {
        return NULL;
    }
    cell->number = data->number;
    cell->ladder_to = data->ladder_to;
    cell->snake_to = data->snake_to;
    return cell;
}

static void print_func(const void *data)
{
    Cell *cell = (Cell *)data;
    if (cell->number == BOARD_SIZE)
    {
        printf("[%d]", cell->number);
        return;
    }
    else if (cell->ladder_to != EMPTY)
    {
        printf("[%d] -ladder to->", cell->number);
    }
    else if (cell->snake_to != EMPTY)
    {
        printf("[%d] -snake to->", cell->number);
    }
    else
    {
        printf("[%d] ->", cell->number);
    }
}

static bool check_is_last(const void *a)
{
    const Cell *cell = (const Cell *)a;
    return (cell->number == BOARD_SIZE);
}

static int comp_func(const void *a, const void *b)
{
    const Cell *cell_a = (const Cell *)a;
    const Cell *cell_b = (const Cell *)b;
    return cell_a->number - cell_b->number;
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

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    if (argc != ARGS)
    {
        printf(NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    int seed = strtol(argv[1], NULL, BASE);
    srand(seed);

    int walks_num;
    sscanf(argv[2], "%d", &walks_num);

    MarkovChain *chain = create_chain();
    if (!chain)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    if (fill_database_snakes(chain) == EXIT_FAILURE)
    {
        printf(ALLOCATION_ERROR_MASSAGE);
        free_markov_chain(&chain);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < walks_num; ++i)
    {
        printf("Random Walk %d: ", i + 1);
        generate_random_sequence(chain, chain->database->first->data, MAX_GENERATION_LENGTH);
        printf("\n");
    }
    free_markov_chain(&chain);
    return EXIT_SUCCESS;
}
