#include "utils.h"
#include "fonts/UbuntuMono-R.h"

#define WIDTH 500
#define HEIGHT 100

struct _color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
typedef struct _color Color;
const Color background_color = {.r = 0, .g = 0, .b = 0};

struct _bit_image{
    unsigned short w;
    unsigned short h;
    unsigned char* data;
};
typedef struct _bit_image BitImage;

struct _text
{
    short x;
    short y;
    unsigned short text_len; // num char in the text
    char* text;
    short letter_spacing;
    Color color;
};
typedef struct _text Text;

struct _textstrip
{
    char rolling;

    Text destination;
    Text line_num;
    Text dep_time;
};
typedef struct _textstrip TextStrip;


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

void draw_pixel(short x, short y, Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

int handle_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            cleanup_graphics();
            return 1;
        }
    }
}

int render()
{
    // Reset render target to the screen
    SDL_SetRenderTarget(renderer, NULL);

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, lowResTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Set the render target back to the texture
    SDL_SetRenderTarget(renderer, lowResTexture);

    return 0;
}
#endif


void draw_rectangle(short x1, short y1, short x2, short y2, Color color)
{
    for (; x1 < x2; x1++)
    {
        for (short y = y1; y < y2; y++)
            draw_pixel(x1, y, color);
    }   
}


void draw_bit_image(short x, short y, BitImage bit_image, Color color)
{
    unsigned int bit_index;
    for (unsigned short w = 0; w < bit_image.w; w++)
    {
        for (unsigned short h = 0; h < bit_image.h; h++)
        {
            bit_index = h;
            bit_index *= bit_image.w;
            if (get_bit(bit_image.data, bit_index + w))
                draw_pixel(x + w, y + h, color);
        }
    }
}


void set_bit_image_pixel(BitImage* image, unsigned short x, unsigned short y, unsigned char val)
{
    set_bit(image->data, x + image->w * y, val);
}

unsigned short get_text_drawn_width(Text* text)
{
    unsigned short width = 0;
    for (unsigned short i = 0; i < text->text_len - 1; i++)
    {
        if (text->text[i] != -61 )
            width += text->letter_spacing;
    }
    return width + letter_dimension;

}

void draw_text(Text* text)
{
    BitImage holder;
    holder.h = letter_dimension;
    holder.w = letter_dimension;

    short offset = 0;
    for (unsigned short i = 0; i < text->text_len; i++)
    {
        short letter_x = text->x + offset;
        short letter_index;
        if (text->text[i] == -61 ){
            i++;
            if (i < text->text_len){
                switch (text->text[i])
                {
                    case -124: // Ä
                        letter_index = (num_letters - 2) * letter_size;
                        break;
                    case -123: // Å
                        letter_index = (num_letters - 1) * letter_size;
                        break;
                    case -106: // Ö
                        letter_index = (num_letters - 4) * letter_size;
                        break;
                    case -92: // ä
                        letter_index = (num_letters - 2) * letter_size;
                        break;
                    case -91: // å
                        letter_index = (num_letters - 3) * letter_size;
                        break;
                    case -74: // ö
                        letter_index = (num_letters - 1) * letter_size;
                        break;
                    default:
                        break;
                }
            }
        }
        else
            letter_index = (text->text[i] - 32) * letter_size;

        if (letter_index >= 0 && letter_index < letter_size * num_letters)
        {
            holder.data = _letters + letter_index;
            if(letter_x + letter_dimension >= 0 && letter_x< WIDTH)
                draw_bit_image(letter_x, text->y, holder, text->color);
            offset += text->letter_spacing;
        }
    }
}

void clear_screen()
{
    draw_rectangle(0, 0, WIDTH, HEIGHT, background_color);
}

void clear_text_area(Text* text)
{
    const unsigned short text_w = get_text_drawn_width(text);

    draw_rectangle(
        text->x, 
        text->y, 
        text->x + text_w, 
        text->y + letter_dimension,
        background_color
    );
}


void draw_text_strip(TextStrip* strip)
{
    unsigned short i;
    Text* stip_texts[] = {&(strip->line_num), &(strip->destination), &(strip->dep_time)};
    if(strip->rolling)
    {
        short right_edge = -1;
        short left_edge = WIDTH;
        for (i = 0; i < 3; i++)
        {
            clear_text_area(stip_texts[i]);
            stip_texts[i]->x--;
            unsigned short text_w = get_text_drawn_width(stip_texts[i]);

            if (stip_texts[i]->x + text_w > right_edge)
                right_edge = stip_texts[i]->x + text_w > right_edge;

            if (stip_texts[i]->x < left_edge)
                left_edge = stip_texts[i]->x;
            
        }
        
        // Move back to the furthest right
        if (right_edge < 0)
        {
            for (i = 0; i < 3; i++)
                stip_texts[i]->x += (WIDTH - left_edge);
        }
    }

    for (i = 0; i < 3; i++)
        draw_text(stip_texts[i]);
}