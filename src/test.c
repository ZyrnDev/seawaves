#include <math.h>
#include <raylib.h>
#include <stdlib.h>

typedef float (*function_sampler_t)(float x);

typedef struct {
  Rectangle rect;
  int padding;
  Color color;
  function_sampler_t sampler;
} graph_t;

graph_t graph_create(int x, int y, int width, int height, int padding,
                     function_sampler_t sampler) {
  graph_t graph;

  graph.rect.x = x + padding;
  graph.rect.y = y + padding;
  graph.rect.width = width - 2 * padding;
  graph.rect.height = height - 2 * padding;
  graph.color = PURPLE;

  graph.sampler = sampler;

  return graph;
}

void graph_draw(graph_t *graph) {
  if (graph == NULL) {
    return;
  }
  // Draw a rectangle representing the graph
  DrawRectangleRec(graph->rect, graph->color);

  Vector2 last_point = {
      graph->rect.x,
      graph->sampler(0) * graph->rect.height + graph->rect.y,
  };
  for (int i = 1; i < graph->rect.width; i++) {
    float x = (float)i / graph->rect.width; // Normalize x to [0, 1]
    float y = graph->sampler(x);            // Sample the function
    Vector2 point = {
        x * graph->rect.width + graph->rect.x,
        y * graph->rect.height + graph->rect.y,
    };
    DrawLineV(last_point, point, BLACK);
    last_point = point;
  }
}

float test_sampler(float x) {
  return 0.5f + 0.5f * sinf(2 * PI * x * 4); // Example: sine wave
}

int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "seawaves");

  graph_t graph =
      graph_create(0, 0, screenWidth, screenHeight, 50, test_sampler);

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    graph_draw(&graph);

    // BeginMode2D(camera);

    // DrawCircle(0, 0, 50, RED);
    // DrawText("ZERO", 0, 0, 20, DARKGRAY);
    // DrawText("HALF", screenWidth / 2, screenHeight / 2, 20, DARKGRAY);
    // DrawText("FULL", screenWidth, screenHeight, 20, DARKGRAY);

    // EndMode2D();

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
