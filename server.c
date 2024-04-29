// server.c - prints first message from client then exits

#include <sys/socket.h> // socket
#include <sys/types.h>  // socket
#include <netinet/in.h> // htons
#include <arpa/inet.h>  // inet_pton
#include <stdio.h>      // fprintf
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <unistd.h>     // write
#include <pthread.h>    // pthread_create

#define DOMAIN AF_INET6
#define PORT   htons(0xC271)
#define SIZE   1024
#define TRUE 1
#define FALSE 0
// gameplay
#define HIGH 23
#define WIDE 80

#define SNAK '&'
#define SNEK 'O'

#define REDO 'r'
#define QUIT 'q'

#define FORE 'w'
#define BACK 's'
#define LEFT 'a'
#define RITE 'd'
    
int SnakNum, Snakx, Snaky;

typedef struct SnakeSegment {
    int x;
    int y;
    struct SnakeSegment* next;
} SnakeSegment;

typedef struct {
    int food_x;
    int food_y;
    SnakeSegment* head;
    int score;
} GameState ;


void generateFoodPosition(GameState* state) {
    state->food_x = rand() % WIDE;
    state->food_y = rand() % HIGH;
}

void initGameState(GameState* state) {
    // Initialize food position
    generateFoodPosition(state);

    // Initialize snake (empty)
    state->head = malloc(sizeof(SnakeSegment));
    state->head->x = WIDE / 2;
    state->head->y = HIGH / 2;

    // Initialize score
    state->score = 0;
}

void render(GameState* state) {

    char board[HIGH][WIDE];
    for (int y = 0; y < HIGH; y++) {
        for (int x = 0; x < WIDE; x++) {
            if (y == 0){
                board[y][x] = '-';
            }
            else if (y == HIGH - 1){
                board[y][x] = '-';
            }
            else if (x == 0){
                board[y][x] = '|';
            }
            else if (x == WIDE - 1){
                board[y][x] = '|';
            }
            else{
                board[y][x] = ' ';
            }
        }
        
    }
    board[state->food_y][state->food_x] = SNAK;
    
    SnakeSegment* current = state->head;
    while (current != NULL) {
        board[current->y][current->x] = SNEK;
        current = current->next;
        
    }
    for (int y = 0; y < HIGH; y++) {
        for (int x = 0; x < WIDE; x++) {
            printf("%c", board[y][x]);
        }
        printf("%d\n", y);
    }
    printf("finished rendering");
    fflush(stdout);
}

void addToSnakeHead(GameState* state, int x, int y) {
    printf("Adding to head");
    fflush(stdout);
    SnakeSegment* new_head = (SnakeSegment*)malloc(sizeof(SnakeSegment));
    new_head->x = x;
    new_head->y = y;
    new_head->next = state->head;
    state->head = new_head;
    printf("finished adding to head");
    fflush(stdout);
}

void removeFromSnakeTail(GameState* state) {
    if (state->head == NULL) return;

    SnakeSegment* current = state->head;
    SnakeSegment* prev = NULL;

    // Traverse the linked list to find the last element
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    // Free the last segment and update the tail pointer
    if (prev != NULL) {
        prev->next = NULL;
        free(current);
    } else {
        free(state->head);
        state->head = NULL;
    }
}

void updateGameState(GameState* state, char* direction) {
    // Move the snake based on user input
    printf("start update");
    fflush(stdout);
    switch (direction[0]) {
        case 'w': // Up
            addToSnakeHead(state, state->head->x, state->head->y - 1);
            break;
        case 's': // Down
            addToSnakeHead(state, state->head->x, state->head->y + 1);
            break;
        case 'a': // Left
            addToSnakeHead(state, state->head->x - 1, state->head->y);
            break;
        case 'd': // Right
            addToSnakeHead(state, state->head->x + 1, state->head->y);
            break;
        default:
            // Invalid input or no input
            return;
    }
    printf("finsihed moving");
    fflush(stdout);
    // Check for collision with food
    if (state->head->x == state->food_x && state->head->y == state->food_y) {
        // If the snake's head is at the same position as the food,
        // increase the score, generate a new food position, and
        // add a new segment to the snake's head
        state->score++;
        printf("before food");
        fflush(stdout);
        generateFoodPosition(state);
        printf("after food");
        fflush(stdout);
    } else {
        // If no collision with food, remove the last segment from the snake's tail
        printf("before remove");
        fflush(stdout);
        removeFromSnakeTail(state);
        printf("after remove");
        fflush(stdout);
    }

    // Update other game state (e.g., check for collisions with walls or itself)
    // This part would depend on the specific rules of the game.
    printf("finished updating");
    fflush(stdout);
}



typedef struct {
    int conx;
    char* buff;
} client_data;

void *read_client_message(void *arg)
{
    client_data* data = (client_data*)arg;
    int conx = data->conx;
    char* buff = data->buff;
    //memset(buff, 0, SIZE);

    while (!sleep(1))
    {
        read(conx, buff, SIZE);
        
        
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    int sock = socket(DOMAIN, SOCK_STREAM, 0), opt = 1, conx ;
    socklen_t s = sizeof(struct sockaddr_in6) ;
    struct sockaddr_in6 addr ;
    char buff[SIZE] ;
    
    addr.sin6_family = DOMAIN ;
    addr.sin6_port   = PORT   ;
    addr.sin6_addr   = in6addr_any ;
    memset(buff, 0, SIZE) ;
    
    // make testing easier
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Server - setsockopt addr failed.\n") ;
        exit(-1) ;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Server - setsockopt port failed.\n") ;
        exit(-1) ;
    }
    // end testing ease
    
    if (bind(sock, (struct sockaddr *)&addr, s))
    {
        perror("Server - bindin failed.\n") ;
        exit(-1) ;
    }
    if (listen(sock, 1))
    {    
        perror("Server - listen failed.\n") ;
        exit(-1) ;
    }
    conx = accept(sock, (struct sockaddr *)&addr, &s) ;
    if (conx == -1)
    {
        perror("Server - accept failed.\n") ;
        exit(-1) ;
    }

    client_data data;
    data.conx = conx;
    data.buff = buff;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, read_client_message, &data) != 0)
    {
        perror("Server - thread creation failed.\n");
        exit(-1);
    }

    srand(time(NULL));

    // Initialize game state
    GameState state;
    initGameState(&state);

    // Generate initial food position
    generateFoodPosition(&state);

    while (TRUE){        
        
        render(&state);
        // Update game state based on user input
        updateGameState(&state, buff);

        // Render the game (not implemented in this example)

        // Example: Print snake head position, food position, and score
        printf("Snake Head: (%d, %d)\n", state.head->x, state.head->y);
        
        printf("Food Position: (%d, %d)\n", state.food_x, state.food_y);
        printf("Score: %d\n", state.score);
        fflush(stdout);
        sleep(1) ;
        
            
    
    }
    

}