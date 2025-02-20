//ARTIFICIAL INTELLIGENCE - PROGRAMMING PROJECT 1
//MEMBERS: DIONISIO, MUNCAL, GARCIA, TOLOSA, VERGARA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 4

//define the structure
typedef struct
{
    int board[N][N];
    int x, y; // Position of the blank tile
} State;

typedef struct Node
{
    int puzzle[N][N];
    int g; // Cost from the start node
    int h; // Heuristic (estimated cost to goal)
    int f; // Total cost (f = g + h)
    struct Node *parent;
    struct Node *next;
} Node;

//declare the goal state of the puzzle
int goal[N][N] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {8, 9, 10, 11},
    {12, 13, 14, 15}};

//used to define and handle movement directions
int dy[] = {1, -1, 0, 0};
int dx[] = {0, 0, 1, -1};
char *move[] = {"RIGHT", "LEFT", "DOWN", "UP"};
int nMoves = 0;
int node = 0;

void print_INPUT(State);
// IDS FUNCTIONS
int is_goal(State *);
void swap(int *, int *);
int do_move(State *, int);
int dls(State *, int, int, char *);
void ids(State *);
// A* FUNCTIONS
void astar_search(int[N][N]);
int calculate_solution_cost(Node *);
void reconstruct_path(Node *);
int calculate_manhattan_distance(int[N][N]);
void generate_successors(Node *, Node **, Node *);

int main()
{
    State initial_state;
    int initial_puzzle[N][N];

    int flag = 1, check;
    while (flag)
    {
        printf("Enter the initial state of the 15-puzzle (0 represents the blank tile):\n");
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                scanf("%d", &initial_state.board[i][j]);
                initial_puzzle[i][j] = initial_state.board[i][j];
                check = 1;
                if (initial_state.board[i][j] > 15 || initial_state.board[i][j] < 0)
                {
                    printf("\tInput should only consist of numbers from 0 - 15. Try Again\n\n");
                    check = 0;
                    i = j = N;
                }
                if (initial_state.board[i][j] == 0)
                {
                    initial_state.x = i;
                    initial_state.y = j;
                }
            }
        if (check)
        {
            for (int i = 0; i < N; i++)
                for (int j = 0 + i; j < N; j++)
                {
                    int temp = initial_puzzle[i][j];
                    int count = 0;
                    for (int k = 0; k < N; k++)
                        for (int l = 0; l < N; l++)
                            if (temp == initial_puzzle[k][l] && count == 0)
                                count++;
                            else if (temp == initial_puzzle[k][l] && count == 1)
                            {
                                printf("\tNo Duplicating Number. Try Again\n\n");
                                check = 0;
                                i = j = k = l = 3;
                            }
                }
            if (check)
                flag = 0;
        }

        fflush(stdin);
    }

    print_INPUT(initial_state);

    printf("\nSolving the 15-puzzle...\n\n");
    printf("ITERATIVE DEEPENING SEARCH:\n");

    // ITERATIVE DEEPENING SEARCH
    clock_t start = clock();
    ids(&initial_state);
    printf("\tSolution Cost: %d\n", nMoves); // prints how many moves in the solution
    printf("\tSearch Cost: %d\n", node);
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\tRunning time: %.4f seconds", elapsed_time);

    nMoves = node = 0;
    printf("\n\nA* SEARCH:");
    // A* SEARCH
    start = clock();
    astar_search(initial_puzzle);
    printf("\tSearch Cost: %d\n", node); // Print the search cost
    end = clock();
    elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\tRunning Time: %.4f seconds", elapsed_time); // Print the running time

    printf("\n\nClosing...\n\n");
    return 0;
}

void print_INPUT(State initial_state)
{
    printf("\nInitial State:\t\t\t\tGoal State:\n");
    printf("---------------------------------\t---------------------------------\n|");
    for (int i = 0; i < N; i++)
    {
        int flag = 1;
        for (int j = 0; j < N; j++)
        {
            printf("   %d\t|", initial_state.board[i][j]);
            if (j + 1 == N)
                printf("\t|");
        }
        for (int j = 0; j < N; j++)
            printf("   %d\t|", goal[i][j]);
        printf("\n---------------------------------\t---------------------------------\n");
        if (i + 1 != N)
            printf("|");
    }
}

//------------------------------------------------------------------------
//|                     ITERATIVE DEEPENING SEARCH                       |
//------------------------------------------------------------------------

// Function to check if a state is the goal state
int is_goal(State *state)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (state->board[i][j] != goal[i][j])
                return 0;

    return 1;
}

// Function to swap two elements in the board
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to perform a move and update the state
int do_move(State *state, int dir)
{
    int new_x = state->x + dx[dir];
    int new_y = state->y + dy[dir];

    if (new_x < 0 || new_x >= N || new_y < 0 || new_y >= N)
        return 0;

    swap(&state->board[state->x][state->y], &state->board[new_x][new_y]);
    state->x = new_x;
    state->y = new_y;
    return 1;
}

// Function to perform the depth-limited search
int dls(State *state, int depth, int max_depth, char *path)
{
    if (depth == max_depth)
    {
        if (is_goal(state))
        {
            printf("\tSolution path: %s->GOAL\n", path);
            return 1;
        }
        return 0;
    }

    for (int dir = 0; dir < 4; dir++)
    {
        State new_state = *state; // Create a new state to simulate a move.
        if (do_move(&new_state, dir))
        {
            // Iterate over possible moves (up, down, left, right).
            node++;
            char new_path[100]; // Create a new path by appending the move direction.
            strcpy(new_path, path);
            if (strlen(new_path) > 0)
                strcat(new_path, "->");
            strcat(new_path, move[dir]);

            // Recursively call dls with the new state and path.
            if (dls(&new_state, depth + 1, max_depth, new_path))
            {
                nMoves++; // increments the number of moves/directions used
                return 1;
            }
        }
    }
    return 0;
}

// Function to solve the 15-puzzle using Iterative Deepening Search
void ids(State *initial_state)
{
    int max_depth = 0;
    while (1)
    {
        if (dls(initial_state, 0, max_depth, "Start"))
            break;
        node = 0;
        max_depth++;
    }
    return;
}

//------------------------------------------------------------------------
//|                              A* SEARCH                               |
//------------------------------------------------------------------------

// Function to perform A* search
void astar_search(int puzzle[N][N])
{
    // Create the open and closed lists
    Node *open = NULL;
    Node *closed = NULL;

    // Create the start node
    Node *start_node = (Node *)malloc(sizeof(Node));
    // Copy the initial puzzle state
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            start_node->puzzle[i][j] = puzzle[i][j];

    start_node->g = 0;
    start_node->h = calculate_manhattan_distance(start_node->puzzle); // Calculate the Manhattan distance heuristic for the start node
    start_node->f = start_node->g + start_node->h;
    start_node->parent = NULL;

    // Add the start node to the open list
    open = start_node;

    while (open != NULL)
    {
        // Find the node with the lowest f value in the open list
        Node *current = open;
        for (Node *temp = open; temp != NULL; temp = temp->next)
            if (temp->f < current->f)
                current = temp;

        // Move the current node from open to closed
        if (current == open)
            open = open->next;

        else
        {
            Node *prev = open;
            while (prev->next != current)
                prev = prev->next;
            prev->next = current->next;
        }
        current->next = closed;
        closed = current;

        // Check if the current node is the goal node
        int is_goal = 1;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (current->puzzle[i][j] != goal[i][j])
                {
                    is_goal = 0;
                    break;
                }

        if (is_goal)
        {
            // Goal reached, reconstruct and print the path
            reconstruct_path(current);
            printf("GOAL\n");
            // Calculate and print the solution cost
            nMoves = calculate_solution_cost(current);
            printf("\tSolution Cost: %d\n", nMoves);

            return;
        }

        // Generate successors and update their f values
        generate_successors(current, &open, closed);
    }
    // No path found
    printf("No path found\n");
}

// Calculate the solution cost
int calculate_solution_cost(Node *goal_node)
{
    int cost = 0;
    Node *curr = goal_node;

    while (curr->parent != NULL)
    {
        cost++;
        curr = curr->parent;
    }
    return cost;
}

// Function to reconstruct and print the solution path
void reconstruct_path(Node *goal_node)
{
    Node *current = goal_node;
    Node *path = NULL;

    while (current != NULL)
    {
        Node *new_node = (Node *)malloc(sizeof(Node));
        // Copy the puzzle configuration from the current node to the new node
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                new_node->puzzle[i][j] = current->puzzle[i][j];
        // Set the parent of the new node to the current path
        new_node->parent = path;
        // Update the path to include the new node
        path = new_node;
        // Move to the parent of the current node in the original path
        current = current->parent;
    }

    printf("\n\tSolution Path: START->");
    Node *prev_node = NULL;

    while (path != NULL)
    {
        // Determine the move direction by comparing the empty space's position
        int empty_x, empty_y;
        int prev_empty_x, prev_empty_y;

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (path->puzzle[i][j] == 0)
                {
                    empty_x = i;
                    empty_y = j;
                }

        if (prev_node != NULL)
        {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    if (prev_node->puzzle[i][j] == 0)
                    {
                        prev_empty_x = i;
                        prev_empty_y = j;
                    }

            int tempdx = empty_x - prev_empty_x;
            int tempdy = empty_y - prev_empty_y;
            char *direction = NULL;

            for (int k = 0; k < 4; k++)
                if (tempdx == dx[k] && tempdy == dy[k])
                {
                    direction = move[k];
                    break;
                }

            if (direction != NULL)
                printf("%s", direction);
            if (path != NULL)
                printf("->");
        }

        prev_node = path;
        path = path->parent;
    }
}

// Function to calculate the Manhattan distance heuristic
int calculate_manhattan_distance(int puzzle[N][N])
{
    int distance = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            int value = puzzle[i][j];
            for (int x = 0; x < N; x++)
                for (int y = 0; y < N; y++)
                    if (goal[x][y] == value)
                        distance += abs(x - i) + abs(y - j);
        }

    return distance;
}

// Function to generate successors and update their f values
void generate_successors(Node *current, Node **open, Node *closed)
{
    // Find the empty space (0) in the puzzle
    int empty_x, empty_y;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (current->puzzle[i][j] == 0)
            {
                empty_x = i;
                empty_y = j;
                break;
            }

    // Define possible move directions (up, down, left, right)
    int dy[] = {1, -1, 0, 0};
    int dx[] = {0, 0, 1, -1};

    // Generate successor states by moving tiles into the empty space
    for (int k = 0; k < 4; k++)
    {
        int new_x = empty_x + dx[k];
        int new_y = empty_y + dy[k];

        if (new_x >= 0 && new_x < N && new_y >= 0 && new_y < N)
        {
            // Create a new node for the successor
            Node *successor = (Node *)malloc(sizeof(Node));
            // Copy the current puzzle state
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    successor->puzzle[i][j] = current->puzzle[i][j];

            // Swap the empty space and the tile
            successor->puzzle[empty_x][empty_y] = current->puzzle[new_x][new_y];
            successor->puzzle[new_x][new_y] = 0;

            // Update g, h, and f values for the successor
            successor->g = current->g + 1;
            successor->h = calculate_manhattan_distance(successor->puzzle);
            successor->f = successor->g + successor->h;
            successor->parent = current;

            // Check if the successor is not in the closed list or open list
            int in_closed = 0;
            for (Node *node = closed; node != NULL; node = node->next)
                if (node->f == successor->f)
                {
                    int match = 1;
                    for (int i = 0; i < N; i++)
                        for (int j = 0; j < N; j++)
                            if (node->puzzle[i][j] != successor->puzzle[i][j])
                            {
                                match = 0;
                                break;
                            }

                    if (match)
                    {
                        in_closed = 1;
                        break;
                    }
                }

            int in_open = 0;
            for (Node *node = *open; node != NULL; node = node->next)
                if (node->f == successor->f)
                {
                    int match = 1;
                    for (int i = 0; i < N; i++)
                        for (int j = 0; j < N; j++)
                            if (node->puzzle[i][j] != successor->puzzle[i][j])
                            {
                                match = 0;
                                break;
                            }
                    if (match)
                    {
                        in_open = 1;
                        break;
                    }
                }

            // If the successor is not in the closed or open list, add it to the open list
            if (!in_closed && !in_open)
            {
                successor->next = *open;
                *open = successor;
                node++; // increment to get the search cost
            }
        }
    }
}
