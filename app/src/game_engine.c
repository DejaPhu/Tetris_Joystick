#include <string.h>
#include "game_internal.h"
#define TILE(r,c) ((uint16_t)(1U << ((r)*4U + (c))))
#define SHAPE(a,b,c,d) ((uint16_t)((a)|(b)|(c)|(d)))

/* 4x4 local masks: 7 tetrominoes in order I,O,T,S,Z,J,L. */
static const uint16_t piece_masks[7] = {
    SHAPE(TILE(1,0),TILE(1,1),TILE(1,2),TILE(1,3)),
    SHAPE(TILE(0,1),TILE(0,2),TILE(1,1),TILE(1,2)),
    SHAPE(TILE(0,1),TILE(1,0),TILE(1,1),TILE(1,2)),
    SHAPE(TILE(0,1),TILE(0,2),TILE(1,0),TILE(1,1)),
    SHAPE(TILE(0,0),TILE(0,1),TILE(1,1),TILE(1,2)),
    SHAPE(TILE(0,0),TILE(1,0),TILE(1,1),TILE(1,2)),
    SHAPE(TILE(0,2),TILE(1,0),TILE(1,1),TILE(1,2))
};

/* TV2: board[][] la luoi 10x20, 0 rong / 1..7 loai khoi.
 * mask la khoi 4x4 dang roi; toa do piece_x/y la goc mask tren board. */
Engine_t game;
static uint32_t rng_state;
static uint8_t random_piece(void)
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return (uint8_t)(rng_state % 7U);
}

uint32_t gravity_ms(void)
{
    if (game.difficulty == DIFFICULTY_EASY) return 800U;
    if (game.difficulty == DIFFICULTY_HARD) return 250U;
    return 500U;
}

uint8_t occupied(uint16_t mask, uint8_t row, uint8_t col)
{
    return (uint8_t)((mask >> (row * 4U + col)) & 1U);
}

/* Kiem tra truoc khi truy cap mang: toa do am/ngoai bien khong hop le. */
static uint8_t can_place(uint16_t mask, int8_t px, int8_t py)
{
    uint8_t r,c;
    for (r = 0; r < 4U; r++) for (c = 0; c < 4U; c++) {
        if (occupied(mask,r,c)) {
            int8_t x = (int8_t)(px + (int8_t)c);
            int8_t y = (int8_t)(py + (int8_t)r);
            if (x < 0 || x >= (int8_t)TETRIS_COLS ||
                y < 0 || y >= (int8_t)TETRIS_ROWS) return 0U;
            if (game.board[(uint8_t)y][(uint8_t)x]) return 0U;
        }
    }
    return 1U;
}

/* Xoay 90 do: o (r,c) cua mask 4x4 chuyen thanh (c,3-r). */
static uint16_t rotate_mask(uint16_t mask)
{
    uint16_t out = 0U;
    uint8_t r,c;
    for (r=0; r<4U; r++) for (c=0; c<4U; c++)
        if (occupied(mask,r,c)) out |= TILE(c,3U-r);
    return out;
}

uint8_t try_move(int8_t dx, int8_t dy)
{
    int8_t x = (int8_t)(game.piece_x + dx);
    int8_t y = (int8_t)(game.piece_y + dy);
    if (!can_place(game.mask,x,y)) return 0U;
    game.piece_x = x;
    game.piece_y = y;
    return 1U;
}

uint8_t try_rotate(void)
{
    /* O has no rotational displacement. Other pieces use bounded wall kicks. */
    uint16_t newmask;
    if (game.current_piece == 1U) return 1U;
    newmask = rotate_mask(game.mask);
    /* Simplified wall kick (not full SRS). */
    static const int8_t kicks[] = {0, -1, 1, -2, 2};
    uint8_t k;
    for(k=0; k<sizeof(kicks)/sizeof(kicks[0]); k++) {
        int8_t newx = (int8_t)(game.piece_x + kicks[k]);
        if(can_place(newmask,newx,game.piece_y)) {
            game.piece_x = newx;
            game.mask = newmask;
            return 1U;
        }
    }
    return 0U;
}

static void spawn_next(void)
{
    game.last_fall_tick = app_now_ms(); /* Every new piece gets a full fall interval. */
    game.current_piece = game.next_piece;
    game.next_piece = random_piece();
    game.mask = piece_masks[game.current_piece];
    game.piece_x = 3;
    game.piece_y = 0;
    if (!can_place(game.mask,game.piece_x,game.piece_y)) {
        game.state = STATE_GAME_OVER;
        game.menu_index = 0U;
        tone(350U, 220U);
    }
}

void start_game(void)
{
    memset(game.board,0,sizeof(game.board));
    game.score = 0U;
    game.lines = 0U;
    game.menu_index = 0U;
    game.state = STATE_PLAYING;
    game.last_fall_tick = app_now_ms();
    game.next_piece = random_piece();
    spawn_next();
}

/* Khoa khoi -> don hang -> tinh diem -> kiem tra WIN -> sinh khoi tiep.
 * Thu tu nay giup WIN khong bi GAME_OVER ghi de trong cung luot. */
void lock_piece(void)
{
    uint8_t r,c,y,src,dst;
    uint8_t cleared = 0U;
    static const uint16_t score_table[5] = {0,100,300,500,800};
    for (r=0; r<4U; r++) for(c=0; c<4U; c++) {
        if (occupied(game.mask,r,c)) {
            int8_t x = (int8_t)(game.piece_x+c);
            int8_t yy = (int8_t)(game.piece_y+r);
            if (x>=0 && x<(int8_t)TETRIS_COLS &&
                yy>=0 && yy<(int8_t)TETRIS_ROWS)
                game.board[(uint8_t)yy][(uint8_t)x] = (uint8_t)(game.current_piece+1U);
        }
    }
    /* Compact all surviving rows from bottom to top. */
    /* Doc tu duoi len, chi copy hang chua day xuong dst.
     * Sau cung xoa cac hang trong o phia tren, khong bo sot hai hang day lien nhau. */
    dst = TETRIS_ROWS;
    for (src=TETRIS_ROWS; src>0U;) {
        uint8_t full = 1U;
        src--;
        for (c=0; c<TETRIS_COLS; c++) {
            if(game.board[src][c] == 0U) {full=0U;break;}
        }
        if(full) {cleared++;continue;}
        dst--;
        if(dst != src) memcpy(game.board[dst],game.board[src],TETRIS_COLS);
    }
    for(y=0; y<dst; y++) memset(game.board[y],0,TETRIS_COLS);
    if (cleared) {
        /* Score uses the level before clearing; the next piece uses the new level. */
        game.score += (uint32_t)score_table[cleared] * (game.lines/10U + 1U);
        game.lines += cleared;
        tone(1500U, 110U);
    }
    /* WIN wins priority over GAME_OVER if both would happen on this turn. */
    if(game.score >= TETRIS_WIN_SCORE) {
        game.state = STATE_WIN;
        game.menu_index = 0U;
        tone(2200U, 200U);
        return;
    }
    spawn_next();
}


/* Seed once; keep RNG progressing across retries. Zero is invalid for xorshift. */
void game_init(uint32_t seed) {
    memset(&game, 0, sizeof(game));
    rng_state = seed ? seed : 0xA341316CUL;
    game.state = STATE_WELCOME;
    game.difficulty = DIFFICULTY_NORMAL;
}
/* Khong dung delay de roi: deadline doc lap voi toc do nhan input/ve OLED. */
uint8_t game_tick(uint32_t now) {
    if (game.state != STATE_PLAYING) return 0U;
    if ((uint32_t)(now - game.last_fall_tick) < gravity_ms()) return 0U;
    /* Advance the deadline, not now: avoid cumulative 20-ms quantization drift. */
    game.last_fall_tick += gravity_ms();
    if (!try_move(0,1)) lock_piece();
    return 1U;
}
uint32_t game_wait_ms(uint32_t now) {
    uint32_t elapsed, remaining;
    if (game.state != STATE_PLAYING) return TETRIS_GAME_PERIOD_MS;
    elapsed = now - game.last_fall_tick;
    if (elapsed >= gravity_ms()) return 1U;
    remaining = gravity_ms() - elapsed;
    return remaining < TETRIS_GAME_PERIOD_MS ? remaining : TETRIS_GAME_PERIOD_MS;
}
