#include "ChildWindowPaint.h"

#ifdef DUI_BUILD_FOR_SDL

#include "SDL3/SDL.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Define the graphics type enumeration
typedef enum {
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_LINE,
    SHAPE_COUNT
} ShapeType;

/** Draw a circle
 * @param renderer SDL renderer pointer
 * @param center The circle center coordinates
 * @param radius Circle radius
 * @param fill Whether to fill (1 = fill, 0 = outline)
 */
static void RenderCircle(SDL_Renderer* renderer, const SDL_FPoint* center, float radius, int fill)
{
    const int segments = 36; // Number of circle segments; more segments makes it smoother
    SDL_FPoint vertices[segments + 1];

    // Generate circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159265f * (float)i / (float)segments;
        vertices[i].x = center->x + radius * cosf(angle);
        vertices[i].y = center->y + radius * sinf(angle);
    }

    if (fill) {
        // Fill the circle: draw a triangle fan
        for (int i = 0; i < segments; i++) {
            SDL_FPoint tri[3] = { *center, vertices[i], vertices[i + 1] };
            SDL_RenderLines(renderer, tri, 3);
        }
    }
    else {
        // Outline the circle: draw lines
        SDL_RenderLines(renderer, vertices, segments);
    }
}

/** Draw random patterns
 * @param renderer SDL renderer pointer
 * @param windowWidth Window width
 * @param windowHeight Window height
 * @param shapeCount The number of shapes to generate
 */
static void DrawRandomPatterns(SDL_Renderer* renderer, int windowWidth, int windowHeight, int shapeCount) {
    // Initialize the random number generator (ensure different results on each run)
    static int randomInitialized = 0;
    if (!randomInitialized) {
        srand((unsigned int)time(NULL));
        randomInitialized = 1;
    }

    // Set the renderer drawing color to black and clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw random shapes in a loop
    for (int i = 0; i < shapeCount; i++) {
        // 1. Generate a random color (RGBA)
        Uint8 r = (Uint8)(rand() % 256);
        Uint8 g = (Uint8)(rand() % 256);
        Uint8 b = (Uint8)(rand() % 256);
        Uint8 a = (Uint8)(180 + rand() % 76); // Alpha 180-255, ensuring visibility
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // 2. Randomly select the shape type
        ShapeType shape = (ShapeType)(rand() % SHAPE_COUNT);

        // 3. Random line width (1-10 pixels) - adjusted for the SDL3.4 interface
        int lineWidth = 1 + rand() % 10;

        // 4. Draw according to the shape type
        switch (shape) {
        case SHAPE_RECTANGLE: {
            // Random rectangle position and size
            int x = rand() % windowWidth;
            int y = rand() % windowHeight;
            int w = 10 + rand() % 100;  // Width 10-109
            int h = 10 + rand() % 100;  // Height 10-109

            // Ensure the shape is within the window
            if (x + w > windowWidth) w = windowWidth - x;
            if (y + h > windowHeight) h = windowHeight - y;

            SDL_FRect rect = { (float)x, (float)y, (float)w, (float)h };

            // Randomly choose fill or outline
            if (rand() % 2 == 0) {
                SDL_RenderFillRect(renderer, &rect);  // Fill the rectangle (still supported in SDL3.4)
            }
            else {
                SDL_RenderRect(renderer, &rect); // Outline the rectangle (still supported in SDL3.4)
            }
            break;
        }

        case SHAPE_CIRCLE: {
            // Random circle position and radius
            int centerX = rand() % windowWidth;
            int centerY = rand() % windowHeight;
            int radius = 5 + rand() % 50;  // Radius 5-54

            // Ensure the circle is within the window
            if (centerX - radius < 0) centerX = radius;
            if (centerX + radius > windowWidth) centerX = windowWidth - radius;
            if (centerY - radius < 0) centerY = radius;
            if (centerY + radius > windowHeight) centerY = windowHeight - radius;

            SDL_FPoint center = { (float)centerX, (float)centerY };

            // Call the adapted circle drawing function
            RenderCircle(renderer, &center, (float)radius, rand() % 2);
            break;
        }

        case SHAPE_LINE: {
            // Random line start and end points
            SDL_FPoint start = { (float)(rand() % windowWidth), (float)(rand() % windowHeight) };
            SDL_FPoint end = { (float)(rand() % windowWidth), (float)(rand() % windowHeight) };
            SDL_FPoint points[2] = { start , end };
            SDL_RenderLines(renderer, points, 2);  // Draw the line
            break;
        }

        default:
            break;
        }
    }

    // Update the screen display
    SDL_RenderPresent(renderer);
}

struct ChildWindowPaint::TImpl
{
};

ChildWindowPaint::ChildWindowPaint(ui::ChildWindow* pChildWindow) :
    m_pChildWindow(pChildWindow),
    m_impl(nullptr)
{
}

ChildWindowPaint::~ChildWindowPaint()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void ChildWindowPaint::PaintChildWindow(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool bPaintFps)
{
    ASSERT(nativeMsg.uMsg == SDL_EVENT_WINDOW_EXPOSED);
    if (nativeMsg.uMsg != SDL_EVENT_WINDOW_EXPOSED) {
        return;
    }
    SDL_Window* sdlWindow = (SDL_Window*)nativeMsg.wParam;
    ASSERT(sdlWindow != nullptr);
    if (sdlWindow == nullptr) {
        return;
    }
    SDL_Renderer* renderer = SDL_GetRenderer(sdlWindow);
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(sdlWindow, nullptr);
    }
    ASSERT(renderer != nullptr);
    if (renderer != nullptr) {
        if (bPaintFps && (m_pChildWindow != nullptr)) {
            ui::UiRect childWndRect;
            m_pChildWindow->GetChildWindowRect(childWndRect);
            DrawRandomPatterns(renderer, childWndRect.Width(), childWndRect.Height(), 60);
        }
        else {
            // Show a solid color
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Set blue
            SDL_RenderClear(renderer);   // Clear the screen
            SDL_RenderPresent(renderer); // Display
        }
    }
}

#endif
