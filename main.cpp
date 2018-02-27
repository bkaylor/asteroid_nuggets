#include <stdio.h>
#include <math.h>
#include <SDL.h>

#include "vec2.h"

typedef struct Player_Struct
{
    vec2 position;
    vec2 facing;
    vec2 velocity;
} Player;

typedef struct Game_State_Struct
{
    Player *player;
    int width;
    int height;
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
    player_rect.x = game_state->player->position.x - 50;
    player_rect.y = game_state->player->position.y - 50;
    player_rect.w = 100;
    player_rect.h = 100;

    SDL_Point center = {50, 50};

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    SDL_RenderCopyEx(renderer, player_texture, NULL, &player_rect, 
                     vec2_angle_degrees(game_state->player->facing) - 90.0f, &center, flip); 
    SDL_RenderPresent(renderer);
}

bool update(const Uint8 *keyboard, Game_State *game_state, SDL_Renderer *renderer)
{
    Player *player = game_state->player;
    const float THRUST = 0.05f;

    SDL_GetRendererOutputSize(renderer, &game_state->width, &game_state->height);

    // Get acceleration vector.
    vec2 acceleration = vec2_normalize(player->facing);
    acceleration = vec2_scalar_multiply(player->facing, THRUST);

    if (keyboard[SDL_SCANCODE_UP])
    {
        player->velocity = vec2_add(player->velocity, acceleration); 
    }

    if (keyboard[SDL_SCANCODE_LEFT])
    {
        player->facing = vec2_rotate(player->facing, -0.001f);
    }

    if (keyboard[SDL_SCANCODE_RIGHT])
    {
        player->facing = vec2_rotate(player->facing, 0.001f);
    }

    if (keyboard[SDL_SCANCODE_DOWN])
    {
        player->velocity = vec2_subtract(player->velocity, acceleration); 
    }

    if (keyboard[SDL_SCANCODE_S])
    {
        player->velocity = vec2_make(0.0f, 0.0f);
    }

    if (keyboard[SDL_SCANCODE_SPACE])
    {
        player->position = vec2_make(0.0f, 0.0f);
        player->facing = vec2_make(1.0f, 0.0f);
        player->velocity = vec2_make(0.0f, 0.0f);
    }

    player->position = vec2_add(player->position, player->velocity);

    // Do screen wrap stuff.
    if (player->position.x < 0)
    {
        player->position.x = game_state->width;
    }

    if (player->position.y < 0)
    {
        player->position.y = game_state->height;
    }

    if (player->position.x > game_state->width)
    {
        player->position.x = 0;
    }

    if (player->position.y > game_state->height)
    {
        player->position.y = 0;
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
    player->position = vec2_make(0.0f, 0.0f);
    player->facing = vec2_make(1.0f, 0.0f);
    player->velocity = vec2_make(0.0f, 0.0f);
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

    printf("Starting sdl_asteroids!\n");

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
        quit = update(keyboard, &game_state, renderer);
        
        // Render.
        render(&game_state, renderer);
    }

    // Cleanup.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
