#include <raylib.h>
#include <vector>
#include <cmath>
#include <cstdint>
#include <string>
#include <algorithm> // std::clamp


// Paddle
struct Paddle {
    Vector2 size;
    Color color;
    bool isAI;
    int8_t score = 0;
    Vector2 pos = {0,0};
    Paddle(Vector2 s, Color c, bool ai, Vector2 p)
        : size(s), color(c), isAI(ai), pos(p) {}

    void draw() const {
        DrawRectangleV(pos, size, color);
    }
};

struct Ball {
    Vector2 pos,vel;
    Vector2 size;
    Color color;

    void draw() const {
        DrawRectangleV(pos, size, color);
    }
    void update(float w, float h, float dt, std::vector<Paddle>& paddles) {
      pos.x += vel.x * dt;
      pos.y += vel.y * dt;

      if (pos.y < 0) {
        pos.y = 0;
        vel.y = -vel.y;
      }

      if (pos.y + size.y > h) {
        pos.y = h - size.y;
        vel.y = -vel.y;
      }
      
      // scoring (left / right)
      if (pos.x + size.x < 0) {
          paddles[1].score++;
          reset();
          return;
      }

      if (pos.x > w) {
          paddles[0].score++;;
          reset();
          return;
      }

      // paddle collisions
      for (const Paddle& p : paddles) {
          // only bounce if moving toward the paddle
          if (checkCollision(p)) {
              if (vel.x < 0 && pos.x < p.pos.x + p.size.x) {      // moving left
                  pos.x = p.pos.x + p.size.x;
                  vel.x = fabsf(vel.x);
              } else if (vel.x > 0 && pos.x + size.x > p.pos.x) { // moving right
                  pos.x = p.pos.x - size.x;
                  vel.x = -fabsf(vel.x);
              }
          }
      }
    }
    bool checkCollision(const Paddle& p) {
        return pos.x < p.pos.x + p.size.x &&
               pos.x + size.x > p.pos.x &&
               pos.y < p.pos.y + p.size.y &&
               pos.y + size.y > p.pos.y;
    }
    
    void reset() {
        pos = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        vel.x = -vel.x; // optional: send ball toward scorer
    }

};

Vector2 paddleSize{50.0f, 200.0f};
Vector2 ballSize{50.0f, 50.0f};

Vector2 ballVel{500.0f, 500.0f};

float speed = 500.0f;
float aiSpeed = 400.0f;

float paddlePadding = 50.0f;

std::vector<Paddle> paddles = {
    Paddle(paddleSize, BLUE, false, {0,0}),
    Paddle(paddleSize, RED, true, {0,0})
};

std::vector<Ball> balls = {
  {{600,0},ballVel,ballSize,BLUE}
};


void handleInputs(float Δt) {
    float dy = 0.0f;

    // --- player 0 (W/S + Left Shift) ---
    float moveSpeed0 = speed;
    if (IsKeyDown(KEY_LEFT_SHIFT) || (paddles[1].isAI && IsKeyDown(KEY_RIGHT_SHIFT))) {
        moveSpeed0 *= 2;
    }

    if (IsKeyDown(KEY_W)) dy -= moveSpeed0 * Δt;
    if (IsKeyDown(KEY_S)) dy += moveSpeed0 * Δt;

    if (IsGamepadAvailable(0)) {
        float stickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        const float deadzone = 0.15f;
        if (fabsf(stickY) > deadzone) {
            dy += stickY * moveSpeed0 * Δt;
        }
    }

    paddles[0].pos.y += dy;

    // clamp player 0
    if (paddles[0].pos.y < 0) paddles[0].pos.y = 0;
    if (paddles[0].pos.y + paddles[0].size.y > GetScreenHeight())
        paddles[0].pos.y = GetScreenHeight() - paddles[0].size.y;

    // --- player 1 (Up/Down + Right Shift) ---
    if (!paddles[1].isAI) {
        float dy2 = 0.0f;
        float moveSpeed1 = speed;

        if (IsKeyDown(KEY_RIGHT_SHIFT)) {
            moveSpeed1 *= 2;
        }

        if (IsKeyDown(KEY_UP)) dy2 -= moveSpeed1 * Δt;
        if (IsKeyDown(KEY_DOWN)) dy2 += moveSpeed1 * Δt;

        paddles[1].pos.y += dy2;

        // clamp player 1
        if (paddles[1].pos.y < 0) paddles[1].pos.y = 0;
        if (paddles[1].pos.y + paddles[1].size.y > GetScreenHeight())
            paddles[1].pos.y = GetScreenHeight() - paddles[1].size.y;
    }
}

// En kyl jaksais ite tehä tätä kaikkee matikka, kiitti gpt
void moveAI(Paddle& ai, const std::vector<Ball>& balls, float dt, float speed, float screenWidth, float screenHeight) {
    Ball* target = nullptr;

    // find the closest ball moving toward the AI
    for (const Ball& b : balls) {
        bool movingToward = (ai.pos.x < screenWidth / 2) ? b.vel.x < 0 : b.vel.x > 0;
        if (movingToward) {
            if (!target || fabsf(b.pos.x - ai.pos.x) < fabsf(target->pos.x - ai.pos.x)) {
                target = const_cast<Ball*>(&b); // just to get a pointer
            }
        }
    }

    if (!target) return; // no threatening ball

    float paddleCenter = ai.pos.y + ai.size.y * 0.5f;

    if (target->pos.y < paddleCenter) {
        ai.pos.y -= speed * dt;
    } else if (target->pos.y > paddleCenter) {
        ai.pos.y += speed * dt;
    }

    // clamp inside screen
    if (ai.pos.y < 0) ai.pos.y = 0;
    if (ai.pos.y + ai.size.y > screenHeight)
        ai.pos.y = screenHeight - ai.size.y;
}

void drawDivider(int screenWidth, int screenHeight, int segmentHeight = 20, int gap = 10, Color color = WHITE) {
    int x = screenWidth / 2; // middle of screen

    for (int y = 0; y < screenHeight; y += segmentHeight + gap) {
        DrawRectangle(x - 2, y, 4, segmentHeight, color); // 4 px wide vertical segment
    }
}

void drawElements(int w, int h, float Δt) {
      drawDivider(w, h);
      for (Paddle& r : paddles)
        r.draw();
      for (Ball& r : balls) {
        r.update(w, h, Δt, paddles);
        r.draw();
        
      }

      // move AI paddles
      for (Paddle& p : paddles) {
          if (p.isAI) {  // assuming you have a flag to know which paddles are AI
              moveAI(p, balls, Δt, aiSpeed, w, h);
          }
      }

      // Draw the score then
      DrawText(std::to_string(paddles[0].score).c_str(), 100, 20, 60, WHITE);
      DrawText(std::to_string(paddles[1].score).c_str(), w-150, 20, 60, WHITE);

}


int main() {
    // Initialize window and OpenGL context
    InitWindow(1200, 800, "Ihan Paras pongi peli");

    // Main game loop
    while (!WindowShouldClose()) {
      float w = (float)GetScreenWidth();
      float h = (float)GetScreenHeight();
      float Δt = GetFrameTime();
      handleInputs(Δt);
      // Blank screen
      ClearBackground(BLACK);
      
      // Anchor the position before drawing paddle
      paddles[1].pos.x = w - paddles[1].size.x-paddlePadding;
      BeginDrawing();
      drawElements(w, h, Δt);
      EndDrawing();
    }
    CloseWindow();
    return 0;
}
