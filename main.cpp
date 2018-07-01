/* TODO(bkaylor): 
 * [ ] ALT + ENTER
 * [ ] ALT + F4
 * [ ] ALT + TAB should be smoother?
 * [ ] Audio device switching
 * [ ] Print score
 * [ ] Print fps
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#include "vec2.h"

typedef enum Player_State_Enum
{
    Idle,
    Burning
} Player_State;

typedef struct Laser_Struct
{
    int x0;
    int y0;
    int x1;
    int y1;
} Laser;

typedef struct Player_Struct
{
    vec2 position;
    vec2 facing;
    vec2 velocity;
    bool is_shooting;
    Laser laser;
    int score;
    Player_State state;
} Player;

typedef struct Goal_Struct
{
    vec2 position;
    vec2 facing;
    vec2 velocity;
} Goal;

typedef struct Sound_Struct
{
    char *path;
    Uint8 *buffer;
    Uint32 length;
} Sound;

typedef struct Audio_Struct
{
    SDL_AudioSpec spec;
    SDL_AudioDeviceID device_id;
    Sound *goal_sound;
} Audio;

typedef struct Game_State_Struct
{
    Player *player;
    Goal *goal;
    int width;
    int height;
    Audio *audio;
} Game_State;

char *player_bmp_path = "../assets/player.bmp";
char *player_burning_bmp_path = "../assets/player_burning.bmp";
char *goal_bmp_path = "../assets/goal.bmp";
char *background_bmp_path = "../assets/background.bmp";

SDL_Texture *player_texture;
SDL_Texture *player_burning_texture;
SDL_Texture *background_texture;
SDL_Texture *goal_texture;

void player_got_nugget(Player *, Goal *, Game_State *, Audio *);
void init_game_state(Game_State *game_state);
void init_player(Player *player);
void init_goal(Goal *goal);
void render(Game_State *game_state, SDL_Renderer *renderer);
bool update(const Uint8 *keyboard, Game_State *game_state);
void load_bmps();

void load_bmps(SDL_Renderer *renderer)
{
    SDL_Surface *player_bmp = SDL_LoadBMP(player_bmp_path); 
    SDL_Surface *player_burning_bmp = SDL_LoadBMP(player_burning_bmp_path); 
    SDL_Surface *goal_bmp = SDL_LoadBMP(goal_bmp_path); 
    SDL_Surface *background_bmp = SDL_LoadBMP(background_bmp_path); 

    player_texture = SDL_CreateTextureFromSurface(renderer, player_bmp);
    player_burning_texture = SDL_CreateTextureFromSurface(renderer, player_burning_bmp);
    goal_texture = SDL_CreateTextureFromSurface(renderer, goal_bmp);
    background_texture = SDL_CreateTextureFromSurface(renderer, background_bmp);

    SDL_FreeSurface(player_bmp);
    SDL_FreeSurface(player_burning_bmp);
    SDL_FreeSurface(goal_bmp);
    SDL_FreeSurface(background_bmp);
}

void init_and_load_sounds(Audio *audio)
{
    audio->goal_sound = (Sound *)calloc(1, sizeof(Sound));
    audio->goal_sound->path = "../assets/pickup.wav";

    SDL_LoadWAV(audio->goal_sound->path, 
                &audio->spec, 
                &audio->goal_sound->buffer,
                &audio->goal_sound->length);

    audio->device_id = SDL_OpenAudioDevice(NULL, 0, &audio->spec, NULL, 0);
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

    SDL_Rect goal_rect;
    goal_rect.x = game_state->goal->position.x - 50;
    goal_rect.y = game_state->goal->position.y - 50;
    goal_rect.w = 100;
    goal_rect.h = 100;

    SDL_Point center = {50, 50};

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    SDL_Texture *player_current_sprite; 

    switch (game_state->player->state)
    {
        case Burning:
              player_current_sprite = player_burning_texture;
              break;
        case Idle:
        default:
              player_current_sprite = player_texture;
    }

    // Draw goal.
    SDL_RenderCopyEx(renderer, goal_texture, NULL, &goal_rect, 
                     vec2_angle_degrees(game_state->goal->facing), &center, flip); 

    // Draw laser.
    Player *player = game_state->player;
    if (game_state->player->is_shooting)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, player->laser.x0, player->laser.y0,
                           player->laser.x1, player->laser.y1);
    }

    // Draw player.
    SDL_RenderCopyEx(renderer, player_current_sprite, NULL, &player_rect, 
                     vec2_angle_degrees(game_state->player->facing) - 90.0f, &center, flip); 

    SDL_RenderPresent(renderer);
}

bool update(const Uint8 *keyboard, Game_State *game_state, SDL_Renderer *renderer)
{
    Player *player = game_state->player;
    Goal *goal = game_state->goal;
    Audio *audio = game_state->audio;

    const float THRUST = 0.03f;

    SDL_GetRendererOutputSize(renderer, &game_state->width, &game_state->height);

    // Get acceleration vector.
    vec2 acceleration = vec2_normalize(player->facing);
    acceleration = vec2_scalar_multiply(player->facing, THRUST);

    player->state = Idle;

    if (keyboard[SDL_SCANCODE_UP])
    {
        player->velocity = vec2_add(player->velocity, acceleration); 
        player->state = Burning;
    }

    if (keyboard[SDL_SCANCODE_LEFT])
    {
        player->facing = vec2_rotate(player->facing, -0.0005f);
    }

    if (keyboard[SDL_SCANCODE_RIGHT])
    {
        player->facing = vec2_rotate(player->facing, 0.0005f);
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
        player->is_shooting = true;
    }
    else
    {
        player->is_shooting = false;
    }

    if (keyboard[SDL_SCANCODE_X])
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

    // Check collisions
    // Player is touching a nugget.
    if (fabs(player->position.x - goal->position.x) < 50.0f && 
        fabs(player->position.y - goal->position.y) < 50.0f)
    {
        player_got_nugget(player, goal, game_state, audio);
    }

    // Player is shooting a nugget.
    // TODO(bkaylor): Just store this on the goal struct ... ? 
    const SDL_Rect goal_rect = {
        (int)game_state->goal->position.x - 50,
        (int)game_state->goal->position.y - 50,
        100,
        100
    };

    player->laser.x0 = (int)player->position.x;
    player->laser.y0 = (int)player->position.y;

    vec2 player_laser_endpoint = vec2_add(player->position, vec2_scalar_multiply(player->facing, 400.0f));
    player->laser.x1 = (int)player_laser_endpoint.x;
    player->laser.y1 = (int)player_laser_endpoint.y;

    if (player->is_shooting && SDL_IntersectRectAndLine(&goal_rect, &player->laser.x0, &player->laser.y0,
                                                        &player->laser.x1, &player->laser.y1))
    {
        player_got_nugget(player, goal, game_state, audio);
    }

    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
        return true;
    }

    return false;
}

void player_got_nugget(Player *player, Goal *goal, Game_State *game_state, Audio *audio)
{
    player->score++;
    goal->position = vec2_make( (float)(rand() % game_state->width), (float)(rand() % game_state->height) );
    goal->velocity = vec2_make(0.0f, 0.0f);

    // Play sound.
    SDL_QueueAudio(audio->device_id, audio->goal_sound->buffer, audio->goal_sound->length);
    SDL_PauseAudioDevice(audio->device_id, 0);
    printf("Score: %d\n", player->score);
}

void init_game_state(Game_State *game_state)
{
    game_state->player = (Player *)calloc(1, sizeof(Player));
    init_player(game_state->player);
    
    game_state->goal = (Goal *)calloc(1, sizeof(Goal));
    init_goal(game_state->goal);

    game_state->audio = (Audio *)calloc(1, sizeof(Audio));
    init_and_load_sounds(game_state->audio);
}

void init_player(Player *player)
{
    player->position = vec2_make(0.0f, 0.0f);
    player->facing = vec2_make(1.0f, 0.0f);
    player->velocity = vec2_make(0.0f, 0.0f);
    player->is_shooting = false;
    player->laser = {
        0, 0,
        0, 0
    };
    player->score = 0;
}

void init_goal(Goal *goal)
{
    goal->position = vec2_make(100.0f, 0.0f);
    goal->facing = vec2_make(1.0f, 0.0f);
    goal->velocity = vec2_make(0.0f, 0.0f);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init video error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init audio error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_Window *window = SDL_CreateWindow("Hello, sailor!", 
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                          640, 480, 
                                          SDL_WINDOW_BORDERLESS | 
                                          SDL_WINDOW_FULLSCREEN | 
                                          SDL_WINDOW_INPUT_GRABBED);
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

    srand((unsigned int)time(NULL));

    Game_State game_state; 
    init_game_state(&game_state);

    load_bmps(renderer);

    const Uint8 *keyboard;

    SDL_Event event;
    bool quit = false;

    const float FPS_INTERVAL = 1.0f;
    Uint64 fps_start, fps_current, fps_frames = 0;

    fps_start = SDL_GetTicks();

    while (!quit)
    {
        // Input.
        SDL_PumpEvents();
        keyboard = SDL_GetKeyboardState(NULL);

        // Update.
        quit = update(keyboard, &game_state, renderer);
        
        // Render.
        render(&game_state, renderer);

        fps_frames++;

        if (fps_start < SDL_GetTicks() - FPS_INTERVAL * 1000)
        {
            fps_start = SDL_GetTicks();
            fps_current = fps_frames;
            fps_frames = 0;

            printf("%I64d fps\n", fps_current);
        }
    }

    // Cleanup.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
