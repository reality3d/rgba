#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <SDL/SDL.h>

int main(void)
{
    const SDL_VideoInfo* info = NULL;

    /* Flags we will pass into SDL_SetVideoMode. */
    int flags = 0;

    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
        /* Failed, exit. */
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
        return -1;
    }
    flags = (SDL_OPENGL | SDL_FULLSCREEN);
    SDL_SetVideoMode(1024, 768, 24, flags);
    SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
}
