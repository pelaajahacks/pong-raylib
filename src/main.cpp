#include <raylib.h>
#include <vector>



// Paddle
struct Paddle {
    Vector2 pos;
    Vector2 size;
    Color color;

    void draw() const {
        DrawRectangleV(pos, size, color);
    }
};

Vector2 paddleSize{50.0f, 200.0f};

float speed = 200.0f;
float aiSpeed = 100.0f;

std::vector<Paddle> paddles = {
  {{0, 0},paddleSize,WHITE},
  {{0, 0},paddleSize,WHITE},
};

void handleInputs(float Δt) {
  float moveSpeed = speed;
  if(IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)) {
    moveSpeed *= 2;
  }
  if(IsKeyDown(KEY_W)) {
    paddles[0].pos.y -= moveSpeed * Δt;
  }
  if(IsKeyDown(KEY_S)) {
    paddles[0].pos.y += moveSpeed * Δt;
  }

}

int main() {
    // Initialize window and OpenGL context
    InitWindow(1200, 800, "Ihan Paras pongi peli");

    // Main game loop
    while (!WindowShouldClose()) {
      float Δt = GetFrameTime();
      handleInputs(Δt);
      // Blank screen
      ClearBackground(BLACK);
      

      // Anchor the position before drawing paddle
      paddles[1].pos.x = GetScreenWidth() - paddles[1].size.x;
      BeginDrawing();
      for (const Paddle& r : paddles)
        r.draw();


      
      EndDrawing();
    }
    CloseWindow();
    return 0;
}
