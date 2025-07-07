#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define DEG_TO_RAD (M_PI / 180.0)
//gcc -o faceviewer faceviewer.c -lSDL2 -lm
typedef struct {
    float x, y, z;
} Point3D;

typedef struct {
    Point3D points[4];
} Face;

#define MAX_FACES 1024
Face faces[MAX_FACES];
int face_count = 0;

void load_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir ficheiro CSV");
        exit(1);
    }

    while (!feof(f) && face_count < MAX_FACES) {
        Face face;
        if (fscanf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                   &face.points[0].x, &face.points[0].y, &face.points[0].z,
                   &face.points[1].x, &face.points[1].y, &face.points[1].z,
                   &face.points[2].x, &face.points[2].y, &face.points[2].z,
                   &face.points[3].x, &face.points[3].y, &face.points[3].z) == 12) {
            faces[face_count++] = face;
        }
    }

    fclose(f);
}

Point3D rotate_y(Point3D p, float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    Point3D r;
    r.x = p.x * cosA - p.z * sinA;
    r.z = p.x * sinA + p.z * cosA;
    r.y = p.y;
    return r;
}

void project(Point3D p, int *x, int *y) {
    float scale = 300 / (p.z + 500);
    *x = (int)(p.x * scale + SCREEN_WIDTH / 2);
    *y = (int)(-p.y * scale + SCREEN_HEIGHT / 2);
}

void draw(SDL_Renderer *renderer, float angle) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Fundo amarelo
    SDL_RenderClear(renderer);

    for (int i = 0; i < face_count; i++) {
        int px[4], py[4];
        for (int j = 0; j < 4; j++) {
            Point3D r = rotate_y(faces[i].points[j], angle);
            project(r, &px[j], &py[j]);
        }

        // Linhas negras
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int j = 0; j < 4; j++) {
            SDL_RenderDrawLine(renderer, px[j], py[j], px[(j + 1) % 4], py[(j + 1) % 4]);
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s ficheiro.csv\n", argv[0]);
        return 1;
    }

    load_csv(argv[1]);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Faces 3D CSV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    float angle = 0;
    Uint32 last_tick = SDL_GetTicks();
    int running = 1;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
        }

        Uint32 now = SDL_GetTicks();
        if (now - last_tick >= 500) {
            angle += DEG_TO_RAD * 5;
            last_tick = now;
        }

        draw(renderer, angle);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

