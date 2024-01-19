#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UP 0x01     // 二進数で 0001
#define DOWN 0x02   // 二進数で 0010
#define LEFT 0x04   // 二進数で 0100
#define RIGHT 0x08  // 二進数で 1000

const char moves[4] = {'u', 'd', 'l', 'r'};

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

typedef struct Node {
    Point val;
    struct Node *next;
} Node;

Node *array_to_linked_list(Point *array, int length) {
    Node *head = NULL;
    Node *tail = NULL;
    for (int i = 0; i < length; i++) {
        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->val = array[i];
        new_node->next = NULL;
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }
    return head;
}

void free_linked_list(Node *head) {
    Node *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

typedef struct {
    Node *my_body;
    Node *enemy_body;
    Node *foods;
    int my_body_length;
    int enemy_body_length;
    char
        survivor;  // '2'はどちらも生存、'm'は自分だけ生存、'e'は敵だけ生存、'n'はどちらも生存していない。
    GameUpdate *up;
    GameUpdate *down;
    GameUpdate *left;
    GameUpdate *right;
} GameUpdate;

Node *deepCopyLinkedList(Node *head) {
    Node *new_head = NULL;
    Node *new_tail = NULL;
    while (head) {
        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->val = head->val;  // ポイントの値をコピー
        new_node->next = NULL;

        if (new_head == NULL) {
            new_head = new_node;
            new_tail = new_node;
        } else {
            new_tail->next = new_node;
            new_tail = new_node;
        }

        head = head->next;
    }
    return new_head;
}

// 注意！！
// この関数でのmyはmoveをする主体を指すので、
// myが敵,enemyが敵の敵=自分となる場合もある
GameUpdate performMove(Node *my_body, Node *enemy_body, int my_body_length,
                       int enemy_body_length, Node *foods, char move) {
    GameUpdate update;
    update.survivor = '2';
    update.my_body_length = my_body_length;
    update.enemy_body_length = enemy_body_length;

    // 各連結リストをディープコピー
    update.my_body = deepCopyLinkedList(my_body);
    update.enemy_body = enemy_body;
    update.foods = deepCopyLinkedList(foods);

    // 動きを実行する(頭を追加する)
    Node *new_my_head = (Node *)malloc(sizeof(Node));
    new_my_head->next = update.my_body;
    new_my_head->val.x = update.my_body->val.x;
    new_my_head->val.y = update.my_body->val.y;
    update.my_body = new_my_head;
    if (move == 'u')
        update.my_body->val.y += 1;
    else if (move == 'd')
        update.my_body->val.y -= 1;
    else if (move == 'l')
        update.my_body->val.x -= 1;
    else if (move == 'r')
        update.my_body->val.x += 1;

    // 盤面の外に出ていないか（削除予定！！！！）
    if (update.my_body->val.x < 0 || update.my_body->val.x > 10 ||
        update.my_body->val.y < 0 || update.my_body->val.y > 10) {
        printf("エラー！！！！！：%s方向に進むと盤面の外\n", move);
    }

    // フードを食べたかどうか判定
    Node *food = update.foods;
    while (food != NULL) {
        if (food->val.x == update.my_body->val.x &&
            food->val.y == update.my_body->val.y) {
            food->val.x = -1;
            food->val.y = -1;  // 存在しない座標を設定=食べたことにする
            update.my_body_length++;
            break;
        }
        food = food->next;
    }

    // フードを食べていない場合は尻尾を削除する
    if (food == NULL) {
        Node *del_body = update.my_body;
        while (del_body->next->next != NULL) {
            del_body = del_body->next;
        }
        free(del_body->next);
        del_body->next = NULL;
    }

    // 自分が体にぶつかっていないか(削除予定！！！)
    if (update.survivor == '2' || update.survivor == 'm') {
        // 敵にぶつかっていないか
        Node *current = update.enemy_body;
        while (current->next != NULL) {
            if (update.my_body->val.x == current->val.x &&
                update.my_body->val.y == current->val.y) {
                printf("エラー！！！：%s方向は相手の体", move);
                break;
            }
            current = current->next;
        }
        // 自分自身にぶつかっていないか
        current = update.my_body->next;
        while (current->next != NULL) {
            if (update.my_body->val.x == current->val.x &&
                update.my_body->val.y == current->val.y) {
                printf("エラー！！！：%s方向は自分自身の体", move);
                break;
            }
            current = current->next;
        }
    }

    return update;
}

char get_potential_moves(Node *my_body, Node *enemy_body, int my_body_length,
                         int enemy_body_length) {
    char directions = 0;
    int new_my_head[4][2];
    // 上
    new_my_head[0][0] = my_body->val.x;
    new_my_head[0][1] = my_body->val.y + 1;
    // 下
    new_my_head[1][0] = my_body->val.x;
    new_my_head[1][1] = my_body->val.y - 1;
    // 左
    new_my_head[2][0] = my_body->val.x - 1;
    new_my_head[2][1] = my_body->val.y;
    // 右
    new_my_head[3][0] = my_body->val.x + 1;
    new_my_head[3][1] = my_body->val.y;

    for (int i = 0; i < 4; i++) {
        char is_safe_direction = 1;
        if (new_my_head[i][0] < 0 || new_my_head[i][0] > 10 ||
            new_my_head[i][1] < 0 || new_my_head[i][0] > 10) {
            is_safe_direction = 0;
        } else {
            // 自分自身にぶつかっていないか
            Node *current = my_body->next;
            while (current->next != NULL) {
                if (new_my_head[i][0] == current->val.x &&
                    new_my_head[i][1] == current->val.y) {
                    is_safe_direction = 0;
                    break;
                }
                current = current->next;
            }

            // 敵の頭と衝突していないか
            if (new_my_head[i][0] == enemy_body->val.x &&
                new_my_head[i][1] == enemy_body->val.y) {
                // 自分のほうが短いとき
                if (enemy_body_length >= my_body_length) {
                    is_safe_direction = 0;
                }
            } else if (is_safe_direction == 1) {
                // 敵にぶつかっていないか(頭を除く)
                current = enemy_body->next;
                while (current->next != NULL) {
                    if (new_my_head[i][0] == current->val.x &&
                        new_my_head[i][1] == current->val.y) {
                        is_safe_direction = 0;
                        break;
                    }
                    current = current->next;
                }
            }
        }
        if (is_safe_direction == 0) {
            if (i == 0) {
                directions |= UP;
            } else if (i == 1) {
                directions |= DOWN;
            } else if (i == 2) {
                directions |= LEFT;
            } else if (i == 3) {
                directions |= RIGHT;
            }
        }
    }
    return directions;
}

int eval(GameUpdate state) {
    int score;
    switch (state.survivor) {
        case 'm':
            score = 3;
            break;
        case '2':
            score = 2;
            break;
        case 'e':
        case 'n':
        default:
            score = 0;
            break;
    }
    return score;
}

char move(GameData *data, Snake *my_snake, Snake *enemy_snake) {
    // すべての可能な動きのための配列を連結リストに変換
    Node *my_body = array_to_linked_list(my_snake->body, my_snake->body_length);
    Node *enemy_body =
        array_to_linked_list(enemy_snake->body, enemy_snake->body_length);
    Node *foods = array_to_linked_list(data->foods, data->food_count);
    int my_body_length = my_snake->body_length;
    int enemy_body_length = enemy_snake->body_length;

    char best_move = '\0';
    char survivor;

    GameUpdate result;
    result.my_body = my_body;
    result.enemy_body = enemy_body;
    result.foods = foods;
    result.my_body_length = my_body_length;
    result.enemy_body_length = enemy_body_length;
    result.up = NULL;
    result.down = NULL;
    result.left = NULL;
    result.right = NULL;

    // 仮定に基づいてゲームの現状を最も良くする動きを見つけえる
    int best_score = -1;

    char potential_moves =
        get_potential_moves(result.my_body, result.enemy_body,
                            result.my_body_length, result.enemy_body_length);
    if (potential_moves & UP) {
        result.up = performMove(result.my_body, result.enemy_body,
                                result.my_body_length, result.enemy_body_length,
                                result.foods, 'u');
        char enemy_potential_moves = get_potential_moves(
            result.up->enemy_body, result.up->my_body,
            result.up->enemy_body_length, result.up->my_body_length);
        if (enemy_potential_moves & UP) {
            result.up->up =
                performMove(result.up->enemy_body, result.up->my_body,
                            result.up->enemy_body_length,
                            result.up->my_body_length, result.up->foods, 'u');
            Node *temp = result.up->up->my_body;
            result.up->up->my_body = result.up->up->enemy_body;
            result.up->up->enemy_body = temp;
        }

        if (enemy_potential_moves & DOWN) {
            result.down = performMove(
                result.my_body, result.enemy_body, result.my_body_length,
                result.enemy_body_length, result.foods, 'd');
        }

        if (enemy_potential_moves & LEFT) {
            result.left = performMove(
                result.my_body, result.enemy_body, result.my_body_length,
                result.enemy_body_length, result.foods, 'l');
        }

        if (enemy_potential_moves & RIGHT) {
            result.right = performMove(
                result.my_body, result.enemy_body, result.my_body_length,
                result.enemy_body_length, result.foods, 'r');
        }
    }

    if (potential_moves & DOWN) {
        result.down = performMove(result.my_body, result.enemy_body,
                                  result.my_body_length,
                                  result.enemy_body_length, result.foods, 'd');
    }

    if (potential_moves & LEFT) {
        result.left = performMove(result.my_body, result.enemy_body,
                                  result.my_body_length,
                                  result.enemy_body_length, result.foods, 'l');
    }

    if (potential_moves & RIGHT) {
        result.right = performMove(result.my_body, result.enemy_body,
                                   result.my_body_length,
                                   result.enemy_body_length, result.foods, 'r');
    }

    if (score > best_score) {
        best_score = score;
        best_move = my_potential_move;
        survivor = result.survivor;
    }

    free_linked_list(my_body);
    free_linked_list(enemy_body);
    free_linked_list(foods);

    // もし有効な動きが見つからなかった場合はランダムな動きを返します。
    if (best_move == '\0') {
        srand(time(NULL));
        return moves[rand() % 4];
    }

    return best_move;
}