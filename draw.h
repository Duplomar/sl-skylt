#include "utils.h"
#include "Ubuntu-B.h"

#define WIDTH 300
#define HEIGHT 100

struct _color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
typedef struct _color Color;

struct _bit_image{
    unsigned short w;
    unsigned short h;
    unsigned char* data;
};
typedef struct _bit_image BitImage;

struct _text
{
    unsigned short x;
    unsigned short y;
    unsigned short text_len;
    char* text;
};
typedef struct _text Text;


#define LOCALRUN
#ifdef LOCALRUN
#include <SDL2/SDL.h>
SDL_Window *win;
SDL_Renderer *renderer;
SDL_Texture *lowResTexture;

void cleanup_graphics()
{
    SDL_DestroyTexture(lowResTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int init_graphics(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow("Simple Graphics", 100, 100, 1000, 500, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        cleanup_graphics();
        return 1;
    }

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cleanup_graphics();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // Create a low-resolution texture (50x100)
    lowResTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    if (lowResTexture == NULL) {
        cleanup_graphics();
        return 1;
    }
    SDL_SetRenderTarget(renderer, lowResTexture);
    return 0;        

}

void draw_pixel(unsigned short x, unsigned short y, Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

int render()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            cleanup_graphics();
            return 1;
        }
    }

    // Reset render target to the screen
    SDL_SetRenderTarget(renderer, NULL);

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, lowResTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Set the render target back to the texture
    SDL_SetRenderTarget(renderer, lowResTexture);

    SDL_Delay(10);
    return 0;
}
#endif


void fill_rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, Color color)
{
    for (; x1 < x2; x1++)
    {
        for (unsigned short y = y1; y < y2; y++)
            draw_pixel(x1, y, color);
    }   
}


void draw_bit_image(unsigned short x, unsigned short y, BitImage bit_image, Color color)
{
    for (unsigned short w = 0; w < bit_image.w; w++)
    {
        for (unsigned short h = 0; h < bit_image.h; h++)
        {
            if (get_bit(bit_image.data, h * bit_image.w + w))
                draw_pixel(x + w, y + h, color);
        }
    }
}


void set_bit_image_pixel(BitImage* image, unsigned short x, unsigned short y, unsigned char val)
{
    set_bit(image->data, x + image->w * y, val);
}


BitImage test_image(unsigned short size)
{
    BitImage returner;
    returner.h = size;
    returner.w = size;
    returner.data = (unsigned char*) malloc((size * size)/8 + 1);
    memset(returner.data, 0, (size * size)/8 + 1);
    for (unsigned short i = 0; i < size; i++)
        set_bit_image_pixel(&returner, size/2, i , 1);
    
    return returner;
}


void draw_text(Text text, unsigned short letter_spacing, Color color)
{
    BitImage holder;
    holder.h = letter_dimension;
    holder.w = letter_dimension;
    for (unsigned short i = 0; i < text.text_len; i++)
    {
        holder.data = _letters + (text.text[i] - 32) * letter_size;
        draw_bit_image(text.x + i * letter_spacing, text.y, holder, color);
    }
    printf("\n");
    
}


void move_banner(char offset)
{

}