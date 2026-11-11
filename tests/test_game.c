#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "game_internal.h"
#include "button_filter.h"
#include "ui_renderer.h"
static uint32_t now;
uint32_t app_now_ms(void) { return now; }
void tone(uint16_t hz,uint32_t ms) { (void)hz;(void)ms; }
static void line_case(unsigned n) {
 unsigned r,c;
 game_init(123);start_game();
 memset(game.board,0,sizeof(game.board));
 game.current_piece=0; game.mask=0x1111;game.piece_x=0;game.piece_y=16;
 for(r=20-n;r<20;r++)for(c=1;c<10;c++)game.board[r][c]=1;
 lock_piece();
 assert(game.lines==n);
 { const unsigned scores[]={0,100,300,500,800};assert(game.score==scores[n]); }
 for(c=0;c<10;c++)assert(game.board[0][c]==0);
}
int main(void) {
 unsigned i;ButtonFilter b={0};
 assert(button_filter_step(&b,1,1,0)==0);
 assert(button_filter_step(&b,0,1,10)==0);
 assert(button_filter_step(&b,1,1,20)==0);
 assert(button_filter_step(&b,1,1,49)==0);
 assert(button_filter_step(&b,1,1,50)==1);
 assert(button_filter_step(&b,1,1,349)==0);
 assert(button_filter_step(&b,1,1,350)==2);
 assert(button_filter_step(&b,0,1,450)==0);
 memset(&b,0,sizeof b);b.changed_at=0xfffffff0U;b.sample=1;
 assert(button_filter_step(&b,1,0,14U)==1);
 game_init(1);assert(game.state==STATE_WELCOME);
 handle_button(BTN_DROP);assert(game.state==STATE_MAIN_MENU);
 handle_button(BTN_DOWN);handle_button(BTN_DROP);assert(game.state==STATE_HELP);
 handle_button(BTN_PAUSE);handle_button(BTN_DROP);assert(game.state==STATE_DIFFICULTY);
 handle_button(BTN_DOWN);handle_button(BTN_DROP);assert(game.difficulty==DIFFICULTY_NORMAL);
 for(i=0;i<3;i++) {
  static const unsigned speeds[]={800,500,250};
  game.difficulty=(Difficulty_t)i;now=100;start_game();
  assert(gravity_ms()==speeds[i]);assert(game_wait_ms(100+speeds[i]-10)==10);
  assert(!game_tick(100+speeds[i]-1));assert(game_tick(100+speeds[i]));
  now=2000;handle_button(BTN_DROP);assert(game.last_fall_tick==2000);
 }
 game.current_piece=1;game.mask=0x66;game.piece_x=3;game.piece_y=0;
 assert(try_rotate());assert(game.mask==0x66&&game.piece_x==3&&game.piece_y==0);
 game.mask=0x00f0;game.current_piece=0;memset(game.board,0,sizeof game.board);
 for(i=0;i<4;i++)assert(try_rotate());assert(game.mask==0x00f0);
 while(try_move(-1,0)){};assert(!try_move(-1,0));
 for(i=1;i<=4;i++)line_case(i);
 line_case(1);game.lines=9;game.score=0;game.mask=0xf;game.piece_x=0;game.piece_y=19;
 memset(game.board,0,sizeof game.board);for(i=4;i<10;i++)game.board[19][i]=1;
 lock_piece();assert(game.lines==10&&game.score==100);
 memset(game.board,0,sizeof game.board);game.score=999;game.mask=0xf;game.piece_x=0;game.piece_y=19;
 for(i=4;i<10;i++)game.board[19][i]=1;
 lock_piece();assert(game.state==STATE_WIN);
 handle_button(BTN_DROP);assert(game.state==STATE_PLAYING&&game.score==0);
 handle_button(BTN_PAUSE);assert(game.state==STATE_PAUSED);assert(!game_tick(now+100000));
 now=3000;handle_button(BTN_PAUSE);assert(game.last_fall_tick==now);
 handle_button(BTN_PAUSE);assert(game.state==STATE_PAUSED);
 now=3100;handle_button(BTN_LEFT);assert(game.state==STATE_PLAYING && game.last_fall_tick==now);
 memset(game.board,0,sizeof game.board);game.board[0][4]=1;game.next_piece=1;
 game.mask=0xf;game.piece_x=0;game.piece_y=19;lock_piece();assert(game.state==STATE_GAME_OVER);
 handle_button(BTN_LEFT);assert(game.state==STATE_MAIN_MENU);
 game.state=STATE_WIN;handle_button(BTN_LEFT);assert(game.state==STATE_MAIN_MENU);
 game.last_fall_tick=0xfffffff0U;game.state=STATE_PLAYING;game.difficulty=DIFFICULTY_HARD;
 assert(!game_tick(233));assert(game_tick(234));
 { GameSnapshot_t f;memset(&f,0,sizeof f);
   for(i=0;i<8;i++){f.state=(GameState_t)i;assert(ui_render(&f)!=NULL);} }
 /* Deterministic long input stream exercises collision, retries and state changes. */
 { unsigned step,r,c; uint32_t rng=12345;
   game_init(1);start_game();
   for(step=0;step<100000;step++) {
     rng=rng*1664525U+1013904223U;now+=10;
     handle_button((uint16_t)(1U+(rng>>16)%6U));game_tick(now);
     assert(game.state<=STATE_GAME_OVER);assert(game.current_piece<7&&game.next_piece<7);
     for(r=0;r<20;r++)for(c=0;c<10;c++)assert(game.board[r][c]<=7);
     if(game.state==STATE_GAME_OVER||game.state==STATE_WIN)handle_button(BTN_DROP);
   }
 }
 puts("PASS: debounce/repeat/wrap, FSM, gravity/deadlines, O rotation, wall, line clears 1-4, scoring, WIN/Retry/GameOver, UI states");
 return 0;
}
