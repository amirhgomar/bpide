#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
void ensureLastLineVisible(int currentLine, int &scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines);
int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("SDL Text Editor",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont(R"(C:\Windows\Fonts\consola.ttf)", 24); // Replace with the path to your .ttf font
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    std::vector<std::string> lines = {""}; // Holds multiple lines of text
    int currentLine = 0; // Track the current line being edited
    int cursorPos = 1; // Track the cursor position within the current line
    int scrollOffset = 0; // Keeps track of scrolling
    const int LINE_HEIGHT = TTF_FontHeight(font); // Height of each line

    // Timer for cursor blinking
    Uint32 lastCursorToggle = SDL_GetTicks();
    bool cursorVisible = true;
    const Uint32 CURSOR_BLINK_INTERVAL = 500; // 500 ms for blinking

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        // Handle cursor blinking
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastCursorToggle + CURSOR_BLINK_INTERVAL) {
            cursorVisible = !cursorVisible;
            lastCursorToggle = currentTime;
        }

        // Event loop
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEWHEEL) {
                // Handle scroll
                if (e.wheel.y > 0) { // Scroll up
                    scrollOffset = std::max(0, scrollOffset - LINE_HEIGHT);
                } else if (e.wheel.y < 0) { // Scroll down
                    scrollOffset += LINE_HEIGHT;
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    // Ensure cursorPos is within the valid range
                    if (cursorPos > 1 && cursorPos <= lines[currentLine].size()) {
                        // Remove character before cursor
                        lines[currentLine].erase(cursorPos - 1, 1);
                        cursorPos--;
                    } else if (currentLine > 0) {
                        // Merge with previous line
                        cursorPos = lines[currentLine - 1].size();
                        lines[currentLine - 1] += lines[currentLine].substr(1, lines[currentLine].size());
                        lines.erase(lines.begin() + currentLine);
                        currentLine--;
                    }

                    // Ensure there's always at least one line
                    if (lines.empty()) {
                        lines.push_back("");
                        currentLine = 0;
                        cursorPos = 0;
                    }
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (cursorPos <= lines[currentLine].size()) {
                        std::string remainder = lines[currentLine].substr(cursorPos);
                        lines[currentLine] = lines[currentLine].substr(0, cursorPos);
                        lines.insert(lines.begin() + currentLine + 1, remainder);
                        currentLine++;
                        cursorPos = 0;
                        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                    }
                }
                else if (e.key.keysym.sym == SDLK_TAB) {
                    // Add spaces for tab
                    lines[currentLine].insert(cursorPos, "    ");
                    cursorPos += 4;
                } else if (e.key.keysym.sym == SDLK_LEFT) {
                    // Move cursor left
                    if (cursorPos > 1) {
                        cursorPos--;
                    } else if (currentLine > 0) {
                        currentLine--;
                        cursorPos = lines[currentLine].size();
                    }
                } else if (e.key.keysym.sym == SDLK_RIGHT) {
                    // Move cursor right
                    if (cursorPos < lines[currentLine].size()) {
                        cursorPos++;
                    } else if (currentLine < lines.size() - 1) {
                        currentLine++;
                        cursorPos = 0;
                    }
                } else if (e.key.keysym.sym == SDLK_UP) {
                    // Move cursor up
                    if (currentLine > 0) {
                        currentLine--;
                        cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                    }
                } else if (e.key.keysym.sym == SDLK_DOWN) {
                    if (currentLine < lines.size() - 1) {
                        currentLine++;
                        cursorPos = std::min(cursorPos, (int)lines[currentLine].size());
                        ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                    }
                }
            } else if (e.type == SDL_TEXTINPUT) {
                if (e.text.text) {
                    lines[currentLine].insert(cursorPos, e.text.text);
                    cursorPos += strlen(e.text.text);
                    ensureLastLineVisible(currentLine, scrollOffset, SCREEN_HEIGHT, LINE_HEIGHT, lines.size());
                }
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render text
        SDL_Color textColor = {0, 0, 0, 255};
        int y = -scrollOffset; // Start rendering based on the scroll offset

        for (size_t i = 0; i < lines.size(); ++i) {
            if (y + LINE_HEIGHT > 0 && y < SCREEN_HEIGHT) { // Render only visible lines
                if (lines[i].empty()) {
                    lines[i] = " "; // Show cursor on the current line
                }
                SDL_Surface* textSurface = TTF_RenderText_Blended(font, lines[i].c_str(), textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                int textWidth = textSurface->w;
                int textHeight = textSurface->h;
                SDL_Rect renderQuad = {10, y, textWidth, textHeight};

                SDL_FreeSurface(textSurface);

                SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
                SDL_DestroyTexture(textTexture);

                // Render cursor if this is the current line
                if (i == currentLine) {
                    int cursorX = 0;
                    if (cursorPos > 0) {
                        TTF_SizeText(font, lines[i].substr(0, cursorPos).c_str(), &cursorX, nullptr);
                    }
                    cursorX += 10; // Add padding for the left margin
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawLine(renderer, cursorX, y, cursorX, y + LINE_HEIGHT);
                }
            }
            y += LINE_HEIGHT; // Move to the next line
        }

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}


// Ensure the current line is visible when adding new lines or moving the cursor
void ensureLastLineVisible(int currentLine, int &scrollOffset, int SCREEN_HEIGHT, int LINE_HEIGHT, int totalLines) {
    int cursorY = currentLine * LINE_HEIGHT - scrollOffset;
    if (cursorY < 0) {
        // Scroll up
        scrollOffset = currentLine * LINE_HEIGHT;
    } else if (cursorY + LINE_HEIGHT > SCREEN_HEIGHT) {
        // Scroll down
        scrollOffset = (currentLine + 1) * LINE_HEIGHT - SCREEN_HEIGHT;
    }

    // Ensure last line is always visible
    int contentHeight = totalLines * LINE_HEIGHT;
    if (contentHeight > SCREEN_HEIGHT) {
        scrollOffset = std::min(scrollOffset, contentHeight - SCREEN_HEIGHT);
    } else {
        scrollOffset = 0; // No scrolling needed if content fits
    }
}