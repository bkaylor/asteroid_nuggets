#include <stdio.h>
#include <SDL.h>

typedef struct Player_Struct
{
    int x;
    int y;
    int facing;
    int direction;
    int speed;
} Player;

typedef struct Game_State_Struct
{
    Player *player;
} Game_State;

char *player_bmp_path = "../assets/player.bmp";
char *background_bmp_path = "../assets/background.bmp";

SDL_Texture *player_texture;
SDL_Texture *background_texture;

void init_game_state(Game_State *game_state);
void init_player(Player *player);
void render(Game_State *game_state, SDL_Renderer *renderer);
bool update(const Uint8 *keyboard, Game_State *game_state);
void load_bmps();

void load_bmps(SDL_Renderer *renderer)
{
    SDL_Surface *player_bmp = SDL_LoadBMP("../assets/player.bmp"); 
    SDL_Surface *background_bmp = SDL_LoadBMP("../assets/background.bmp"); 

    player_texture = SDL_CreateTextureFromSurface(renderer, player_bmp);
    background_texture = SDL_CreateTextureFromSurface(renderer, background_bmp);

    SDL_FreeSurface(player_bmp);
    SDL_FreeSurface(background_bmp);
}

void render(Game_State *game_state, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background_texture, NULL, NULL);

    SDL_Rect player_rect;
    player_rect.x = game_state->player->x;
    player_rect.y = game_state->player->y;
    player_rect.w = 100;
    player_rect.h = 100;

    SDL_RenderCopy(renderer, player_texture, NULL, &player_rect); 
    SDL_RenderPresent(renderer);
}

bool update(const Uint8 *keyboard, Game_State *game_state)
{
    if (keyboard[SDL_SCANCODE_UP])
    {
        game_state->player->y -= 3;
    }

    if (keyboard[SDL_SCANCODE_LEFT])
    {
        game_state->player->x -= 3;
    }

    if (keyboard[SDL_SCANCODE_RIGHT])
    {
        game_state->player->x += 3;
    }

    if (keyboard[SDL_SCANCODE_DOWN])
    {
        game_state->player->y += 3;
    }

    if (keyboard[SDL_SCANCODE_SPACE])
    {
    }

    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
        return true;
    }

    return false;
}

void init_game_state(Game_State *game_state)
{
    game_state->player = (Player *)calloc(1, sizeof(Player));
    init_player(game_state->player);
}

void init_player(Player *player)
{
    player->x = 0;
    player->y = 0;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_Window *window = SDL_CreateWindow("Hello, sailor!", 
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                          640, 480, 
                                          SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED);
    if (!window)
    {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 1;
    }

    Game_State game_state; 
    init_game_state(&game_state);

    load_bmps(renderer);

    const Uint8 *keyboard;

    SDL_Event event;
    bool quit = false;

    while (!quit)
    {
        // Input.
        SDL_PumpEvents();
        keyboard = SDL_GetKeyboardState(NULL);

        // Update.
        quit = update(keyboard, &game_state);
        
        // Render.
        render(&game_state, renderer);
    }

    // Cleanup.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
