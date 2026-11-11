/* Pure monochrome renderer: all eight FSM screens fit 128x64. */
#include <string.h>
#include "ui_renderer.h"
#define OLED_WIDTH   128U
#define OLED_HEIGHT   64U
#define OLED_I2C_ADDR (0x3CU << 1)

static uint8_t framebuffer[OLED_WIDTH * OLED_HEIGHT / 8U];

/* 5x7 font: A-Z (0-25), 0-9 (26-35), space (36), ':' (37),
 * '-' (38), '!' (39), '/' (40), '>' (41), '.' (42). */
static const uint8_t font5x7[43][5] = {
    {0x7e,0x11,0x11,0x11,0x7e}, /* A */
    {0x7f,0x49,0x49,0x49,0x36}, /* B */
    {0x3e,0x41,0x41,0x41,0x22}, /* C */
    {0x7f,0x41,0x41,0x22,0x1c}, /* D */
    {0x7f,0x49,0x49,0x49,0x41}, /* E */
    {0x7f,0x09,0x09,0x09,0x01}, /* F */
    {0x3e,0x41,0x49,0x49,0x7a}, /* G */
    {0x7f,0x08,0x08,0x08,0x7f}, /* H */
    {0x41,0x41,0x7f,0x41,0x41}, /* I */
    {0x20,0x40,0x41,0x3f,0x01}, /* J */
    {0x7f,0x08,0x14,0x22,0x41}, /* K */
    {0x7f,0x40,0x40,0x40,0x40}, /* L */
    {0x7f,0x02,0x0c,0x02,0x7f}, /* M */
    {0x7f,0x04,0x08,0x10,0x7f}, /* N */
    {0x3e,0x41,0x41,0x41,0x3e}, /* O */
    {0x7f,0x09,0x09,0x09,0x06}, /* P */
    {0x3e,0x41,0x51,0x21,0x5e}, /* Q */
    {0x7f,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7f,0x01,0x01}, /* T */
    {0x3f,0x40,0x40,0x40,0x3f}, /* U */
    {0x1f,0x20,0x40,0x20,0x1f}, /* V */
    {0x7f,0x20,0x18,0x20,0x7f}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x03,0x04,0x78,0x04,0x03}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}, /* Z */
    {0x3e,0x45,0x49,0x51,0x3e}, /* 0 */
    {0x00,0x42,0x7f,0x40,0x00}, /* 1 */
    {0x62,0x51,0x49,0x49,0x46}, /* 2 */
    {0x22,0x41,0x49,0x49,0x36}, /* 3 */
    {0x18,0x14,0x12,0x7f,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3c,0x4a,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1e}, /* 9 */
    {0,0,0,0,0},                /* space */
    {0,0x36,0x36,0,0},          /* : */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0,0,0x5f,0,0},             /* ! */
    {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x08,0x14,0x22,0x41,0},    /* > */
    {0,0x60,0x60,0,0}           /* . */
};

static uint8_t glyph_index(char c)
{
    if(c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if(c >= 'A' && c <= 'Z') return (uint8_t)(c - 'A');
    if(c >= '0' && c <= '9') return (uint8_t)(26 + c - '0');
    switch (c) {
    case ':': return 37U;
    case '-': return 38U;
    case '!': return 39U;
    case '/': return 40U;
    case '>': return 41U;
    case '.': return 42U;
    default:  return 36U;
    }
}

static void pixel(uint8_t x,uint8_t y,uint8_t on)
{
    uint16_t index;
    uint8_t mask;
    if(x>=OLED_WIDTH || y>=OLED_HEIGHT) return;
    index=(uint16_t)x + (uint16_t)(y/8U)*OLED_WIDTH;
    mask=(uint8_t)(1U << (y%8U));
    if(on) framebuffer[index] |= mask;
    else   framebuffer[index] &= (uint8_t)~mask;
}

static void fill_rect(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t on)
{
    uint16_t xx,yy;
    for(yy=y;yy<(uint16_t)y+h && yy<OLED_HEIGHT;yy++)
        for(xx=x;xx<(uint16_t)x+w && xx<OLED_WIDTH;xx++)
            pixel((uint8_t)xx,(uint8_t)yy,on);
}

static void text_at(uint8_t x,uint8_t y,const char *text)
{
    uint8_t col,row;
    const uint8_t *glyph;
    while (*text && x+5U<OLED_WIDTH) {
        glyph=font5x7[glyph_index(*text++)];
        for(col=0; col<5U; col++)
            for(row=0; row<7U; row++)
                if(glyph[col] & (1U<<row)) pixel((uint8_t)(x+col),(uint8_t)(y+row),1U);
        x=(uint8_t)(x+6U);
    }
}

static void text_uint(uint8_t x,uint8_t y,uint32_t value)
{
    char buf[11];
    uint8_t n=0U,i;
    do {
        buf[n++]=(char)('0'+(value%10U));
        value/=10U;
    } while(value && n<10U);
    for(i=0; i<n/2U;i++) {
        char tmp=buf[i];buf[i]=buf[n-1U-i];buf[n-1U-i]=tmp;
    }
    buf[n]='\0';
    text_at(x,y,buf);
}

static const char *difficulty_name(Difficulty_t value)
{
    switch(value) {
    case DIFFICULTY_EASY: return "EASY";
    case DIFFICULTY_HARD: return "HARD";
    default: return "NORMAL";
    }
}

static void menu_item(uint8_t y,uint8_t selected,const char *name)
{
    text_at(5U,y,selected?">":" ");
    text_at(17U,y,name);
}

static void draw_game_board(const GameSnapshot_t *frame)
{
    uint8_t r,c;
    /* Landscape compromise: 3x3 pixel tiles -> full 10x20 board in 30x60. */
    for(r=0;r<TETRIS_ROWS;r++) for(c=0;c<TETRIS_COLS;c++) {
        if(frame->cells[r][c])
            fill_rect((uint8_t)(2U+c*3U),(uint8_t)(2U+r*3U),3U,3U,1U);
    }
    fill_rect(34U,0U,1U,64U,1U);
    /* Preview follows the engine ordering I,O,T,S,Z,J,L. */
    { static const uint16_t preview[7]={0x00f0,0x0066,0x0072,0x0036,0x0063,0x0071,0x0074};
      uint16_t mask=preview[frame->next_piece<7U?frame->next_piece:0U];
      text_at(95U,0U,"NEXT");
      for(r=0;r<4U;r++)for(c=0;c<4U;c++)
        if(mask&(1U<<(r*4U+c)))fill_rect(98U+c*3U,12U+r*3U,3U,3U,1U);
      text_at(90U,53U,"LV");text_uint(108U,53U,frame->level);
    }
    text_at(41U,0U,"TETRIS");
    text_at(41U,11U,"SCORE");
    text_uint(41U,20U,frame->score);
    text_at(41U,32U,"MODE");
    text_at(41U,41U,difficulty_name(frame->difficulty));
    text_at(41U,53U,"LNS:");
    text_uint(72U,53U,frame->lines);
}

const uint8_t *ui_render(const GameSnapshot_t *frame)
{
    memset(framebuffer,0,sizeof(framebuffer));
    switch(frame->state) {
    case STATE_WELCOME:
        text_at(45U,16U,"TETRIS");
        text_at(20U,36U,"PRESS JOY SW");
        break;
    case STATE_MAIN_MENU:
        text_at(34U,4U,"MAIN MENU");
        menu_item(23U,frame->menu_index==0U,"START GAME");
        menu_item(36U,frame->menu_index==1U,"HOW TO PLAY");
        break;
    case STATE_HELP:
        text_at(30U,2U,"CONTROLS");
        text_at(2U,14U,"L/R MOVE UP TURN");
        text_at(2U,25U,"DOWN SOFT DROP");
        text_at(2U,36U,"SW HARD DROP / OK");
        text_at(2U,47U,"PAUSE BACK / RESUME");
        break;
    case STATE_DIFFICULTY:
        text_at(25U,2U,"DIFFICULTY");
        menu_item(20U,frame->menu_index==0U,"EASY");
        menu_item(32U,frame->menu_index==1U,"NORMAL");
        menu_item(44U,frame->menu_index==2U,"HARD");
        break;
    case STATE_PLAYING:
        draw_game_board(frame);
        break;
    case STATE_PAUSED:
        text_at(45U,2U,"PAUSED");
        menu_item(20U,frame->menu_index==0U,"RESUME");
        menu_item(32U,frame->menu_index==1U,"RESTART");
        menu_item(44U,frame->menu_index==2U,"MAIN MENU");
        break;
    case STATE_WIN:
        text_at(39U,2U,"YOU WIN!");
        text_at(19U,17U,"SCORE:");
        text_uint(64U,17U,frame->score);
        menu_item(35U,frame->menu_index==0U,"RETRY");
        menu_item(47U,frame->menu_index==1U,"MAIN MENU");
        break;
    case STATE_GAME_OVER:
        text_at(34U,2U,"GAME OVER");
        text_at(19U,17U,"SCORE:");
        text_uint(64U,17U,frame->score);
        menu_item(35U,frame->menu_index==0U,"RETRY");
        menu_item(47U,frame->menu_index==1U,"MAIN MENU");
        break;
    default: break;
    }
    return framebuffer;
}
