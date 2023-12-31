#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point *body;
    int body_length;
    int health;
} Snake;

typedef struct {
    Point *foods;
    int food_count;
} GameData;

char move(GameData *data, Snake *my_snake, Snake *enemy_snake) {
    printf("foodcount:%d,food1:(%d,%d)\n", data->food_count, data->foods[0].x,
           data->foods[0].y);
    return 'd';
}