#include "maths.h"

typedef enum {
  kAdd,
  kNMathsPuzzles
} MathsPuzzle_t;

static uint16_t s_state = 0;

static MathsPuzzle_t s_puzzle;
static uint16_t s_sequence[4] = {0};

static int8_t s_choices[3] = {0};
static int8_t s_correct = 0;

void updateProcMaths(GContext* _ctx) {

  renderFloor(_ctx, 0);

  renderPlayer(_ctx);
  renderWalls(_ctx, true, true, true, true);
  renderClutter(_ctx);

  drawBitmap(_ctx, m_block, 5, 9);
  drawBitmap(_ctx, m_block, 13, 9);

  for (int _s = 0; _s < 3; ++_s) {
    renderHintNumber(_ctx, GRect((7 + _s*2)*SIZE, 9*SIZE, 16, 16), s_sequence[_s]);   // sequence

    GRect _b = GRect(16*SIZE - 2, (5 + (_s*4))*SIZE - 2, 2*SIZE + 4, 2*SIZE + 4);   // Choices
    renderFrame(_ctx, _b);
    renderHintNumber(_ctx, GRect(16*SIZE - 1, (5 + (_s*4))*SIZE - 1, 16, 16), s_choices[_s]);
  }

  if (getGameState() == kAwaitInput && getFrameCount() < ANIM_FPS/2) {
    drawBitmap(_ctx, m_arrow, 14, 4);
    drawBitmap(_ctx, m_arrow, 14, 8);
    drawBitmap(_ctx, m_arrow, 14, 12);
  }

}

bool tickMaths(bool _doInit) {
  if (_doInit == true) {
    s_state = 0;
    m_player.m_position = GPoint(0, SIZE*9);
    addCluter(3, 4, 5, 13); // Only left

    s_puzzle = rand() % kNMathsPuzzles; // Choose seq
    s_sequence[0] = 30 + rand()%50; // Choose starting
    uint16_t _mod = 5 + rand()%5; // Chose modifier
    for (int _i = 1; _i < 4; ++_i) {
      switch (s_puzzle) {
        case kAdd: s_sequence[_i] = s_sequence[_i - 1] + _mod; break; // Do seq
        default: break;
      }
    }

    s_choices[ 0 ] = -1;
    s_choices[ 1 ] = -1;
    s_choices[ 2 ] = -1;

    s_correct = rand() % 3;
    s_choices[ s_correct ] = s_sequence[3];

    // TODO move this to common, make the shuffler use it
    for (int _c = 0; _c < 3; ++_c) {
      int _c1 = _c + 1, _c2 = _c + 2;
      if (_c1 >= 3) _c1 -= 3;
      if (_c2 >= 3) _c2 -= 3;
      while (s_choices[_c] == -1 || s_choices[_c] == s_choices[_c1] || s_choices[_c] == s_choices[_c2]) {
        s_choices[_c] = s_sequence[3] - 10 + (rand()%20);
      }
    }

    return false;
  }

  if (s_state == 0) {
    enterRoom(&s_state);
  } else if (s_state == 1) { //get input
    setGameState(kAwaitInput);
    ++s_state;
  } else if (s_state == 2) { // check answer and move to chosen door
    if (getPlayerChoice() != s_correct) {
      if (m_dungeon.m_lives > 0) --m_dungeon.m_lives;
      else m_dungeon.m_rooms[ m_dungeon.m_level ][ m_dungeon.m_room + 1 ] = kDeath;
    }
    switch (getPlayerChoice()) {
      case 0: m_player.m_target = GPoint(SIZE*17, SIZE*5); break;
      case 1: m_player.m_target = GPoint(SIZE*17, SIZE*9); break;
      case 2: m_player.m_target = GPoint(SIZE*17, SIZE*13); break;
    }
    setGameState(kMovePlayer);
    ++s_state;
  } else if (s_state == 3) {
    setGameState(kFadeOut);
  }

  return false;
}