#include "config.h"
#include "leds/leds.h"
#include "tetris.h"
#include "logger/logger.h"
#include "leds/animations.h"


// --- Internal state ---
static uint8_t cells[4] = {0,0,0,0};   // 0 empty, 1 R, 2 G, 3 B
static bool active = false;
static uint8_t activePos = 0;
static uint8_t activeColor = 1;
static uint16_t score = 0;
static bool gameOver = false;

enum TetrisPhase : uint8_t { RUNNING, GAMEOVER_BLINK, GAMEOVER_OFF };
static TetrisPhase phase = RUNNING;

static uint32_t gameOverBlinkLastToggleMs = 0;
static bool blinkOn = true;
static uint8_t blinkToggles = 0; // 6 toggles = 3 blinks (ON+OFF)*3


static uint32_t lastStep = 0;

static uint32_t cellColor(uint8_t c)
{
  switch (c) {
    case 1: return ledsColour(255, 0, 0);
    case 2: return ledsColour(0, 255, 0);
    case 3: return ledsColour(0, 0, 255);
    default:return ledsColour(0, 0, 0);
  }
}

static uint8_t randColorRGB()
{
  // returns 1..3
  return (uint8_t)random(1, 4);
}

void ledTetrisReset()
{
  for (uint8_t i = 0; i < 4; i++) cells[i] = 0;
  active = false;
  activePos = 0;
  activeColor = 1;
  score = 0;
  gameOver = false;

  phase = RUNNING;
  blinkOn = true;
  blinkToggles = 0;
  gameOverBlinkLastToggleMs = 0;

  lastStep = millis();
}

static void renderCells()
{
  // draw locked cells
  for (uint8_t i = 0; i < 4; i++) {
    ledsSetPixelPacked(i, cellColor(cells[i]));
  }
  // draw active piece on top
  if (active && activePos < 4) {
    ledsSetPixelPacked(activePos, cellColor(activeColor));
  }
  ledsShow();
}

static bool boardFull()
{
  for (uint8_t i = 0; i < 4; i++) {
    if (cells[i] == 0) return false;
  }
  return true;
}

LedTetrisState ledTetrisUpdate(uint16_t stepMs)
{
  LedTetrisState out{score, gameOver};

// Handle post-game behavior inside tetris.cpp
if (phase == GAMEOVER_BLINK) {
  uint32_t now = millis();
  if (gameOverBlinkLastToggleMs == 0) {
    gameOverBlinkLastToggleMs = now;
    blinkOn = true;
    blinkToggles = 0;
  }

  if (now - gameOverBlinkLastToggleMs >= 500) { // 0.5s toggle => 1Hz blink
    gameOverBlinkLastToggleMs += 500;
    blinkOn = !blinkOn;
    blinkToggles++;

    if (blinkToggles >= 7) { // 3 full blinks
      phase = GAMEOVER_OFF;
    }
  }

  if (phase == GAMEOVER_OFF) {
    ledsClear();
    ledsShow();

  } else {
    if (blinkOn) {
      renderCells();        // show final board
    } else {
      ledsClear();          // hide it
      ledsShow();

    }
  }

  out.gameOver = true;
  return out;
}

if (phase == GAMEOVER_OFF) {
  ledsClear();
  ledsShow();
  out.gameOver = true;
  return out;
}


  uint32_t now = millis();
  if (now - lastStep < stepMs) {
    renderCells();
    return out;
  }
  lastStep += stepMs;

  // Spawn a new active piece if none exists
  if (!active) {
    if (cells[0] != 0) {
      // LSB occupied: treat as game over condition (no spawn space)
      gameOver = true;
      phase = GAMEOVER_BLINK;
      gameOverBlinkLastToggleMs = 0;
      out.gameOver = true;
      renderCells();
      return out;
    }

    active = true;
    activePos = 0;
    activeColor = randColorRGB();
    renderCells();
    return out;
  }

  // Try to move toward MSB
  uint8_t nextPos = activePos + 1;

  bool canMove = (nextPos < 4) && (cells[nextPos] == 0);

  if (canMove) {
    activePos = nextPos;
    renderCells();
    return out;
  }

// We couldn't move to nextPos (either off end or occupied)
bool matched = false;

// Case A: stopped because next cell is occupied -> check the blocker in front
if (nextPos < 4 && cells[nextPos] != 0) {
  if (cells[nextPos] == activeColor) {
    cells[nextPos] = 0;     // remove the blocker
    score++;
    active = false;         // discard active (so both vanish)
    matched = true;
  }
}

// Case B: stopped because we hit the end (activePos == 3) -> check behind
else if (activePos > 0 && cells[activePos - 1] != 0) {
  if (cells[activePos - 1] == activeColor) {
    cells[activePos - 1] = 0;
    score++;
    active = false;
    matched = true;
  }
}

if (matched) {
  out.score = score;
  renderCells();
  return out;
}


  // No match: lock active into current position
  cells[activePos] = activeColor;
  active = false;

  // Game ends if board is full after locking
  if (boardFull()) {
    gameOver = true;
    phase = GAMEOVER_BLINK;
    gameOverBlinkLastToggleMs = 0;
    out.gameOver = true;
    logGame("Game over. Score: %u",out.score);


  }

  renderCells();
  return out;
}
