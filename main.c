#include <limits.h>
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

int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }

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

typedef struct GameUpdate GameUpdate;
struct GameUpdate {
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
    GameUpdate *parent;
};

void printNodeList(Node *node) {
    while (node != NULL) {
        printf("(%d, %d)\n", node->val.x, node->val.y);
        node = node->next;
    }
}

void printGameUpdate(const GameUpdate *update) {
    if (update == NULL) {
        printf("GameUpdate is NULL\n");
        return;
    }

    // my_body の内容を表示
    printf("My Body:\n");
    printNodeList(update->my_body);

    // enemy_body の内容を表示
    printf("Enemy Body:\n");
    printNodeList(update->enemy_body);

    // foods の内容を表示
    printf("Foods:\n");
    printNodeList(update->foods);

    printf("My Body Length: %d\n", update->my_body_length);
    printf("Enemy Body Length: %d\n", update->enemy_body_length);

    // survivor の状態を表示
    switch (update->survivor) {
        case '2':
            printf("Both snakes are alive.\n");
            break;
        case 'm':
            printf("Only my snake is alive.\n");
            break;
        case 'e':
            printf("Only enemy snake is alive.\n");
            break;
        case 'n':
            printf("No snakes are alive.\n");
            break;
        default:
            printf("Unknown survivor state: %c\n", update->survivor);
    }
}

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
GameUpdate *performMove(Node *my_body, Node *enemy_body, int my_body_length,
                        int enemy_body_length, Node *foods, char move) {
    GameUpdate *update = (GameUpdate *)malloc(sizeof(GameUpdate));
    update->survivor = '2';
    update->my_body_length = my_body_length;
    update->enemy_body_length = enemy_body_length;

    // 各連結リストをディープコピー
    update->my_body = deepCopyLinkedList(my_body);
    update->enemy_body = enemy_body;
    update->foods = deepCopyLinkedList(foods);

    // 動きを実行する(頭を追加する)
    Node *new_my_head = (Node *)malloc(sizeof(Node));
    new_my_head->next = update->my_body;
    new_my_head->val.x = update->my_body->val.x;
    new_my_head->val.y = update->my_body->val.y;
    update->my_body = new_my_head;
    if (move == 'u')
        update->my_body->val.y += 1;
    else if (move == 'd')
        update->my_body->val.y -= 1;
    else if (move == 'l')
        update->my_body->val.x -= 1;
    else if (move == 'r')
        update->my_body->val.x += 1;

    // 盤面の外に出ていないか（削除予定！！！！）
    if (update->my_body->val.x < 0 || update->my_body->val.x > 10 ||
        update->my_body->val.y < 0 || update->my_body->val.y > 10) {
        printf("エラー！！！！！：%c方向に進むと盤面の外\n", move);
    }

    // フードを食べたかどうか判定
    Node *food = update->foods;
    while (food != NULL) {
        if (food->val.x == update->my_body->val.x &&
            food->val.y == update->my_body->val.y) {
            food->val.x = -1;
            food->val.y = -1;  // 存在しない座標を設定=食べたことにする
            update->my_body_length++;
            break;
        }
        food = food->next;
    }

    // フードを食べていない場合は尻尾を削除する
    if (food == NULL) {
        Node *del_body = update->my_body;
        while (del_body->next->next != NULL) {
            del_body = del_body->next;
        }
        free(del_body->next);
        del_body->next = NULL;
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
            new_my_head[i][1] < 0 || new_my_head[i][1] > 10) {
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
        if (is_safe_direction) {
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

int eval(GameUpdate *state, int depth) {
    ////////////////詳細未実装
    if (state->my_body->val.x == state->enemy_body->val.x &&
        state->my_body->val.y == state->enemy_body->val.y) {
        if (state->my_body_length > state->enemy_body_length) {
            state->survivor = 'm';
        } else if ((state->my_body_length < state->enemy_body_length)) {
            state->survivor = 'e';
        } else {
            state->survivor = 'n';
        }
    }
    // printf("state.survivor%c\n", state->survivor);
    int score = 0;
    switch (state->survivor) {
        case 'm':
            score = 1000;
            break;
        case '2':
            score = 0;
            break;
        case 'e':
            score -= 1000;
        case 'n':
            score -= 500;
        default:
            score = 0;
            break;
    }
    // 敵より長いほうがプラス
    score += (state->my_body_length - state->enemy_body_length) * 30;
    if (score > 0) {
        // 勝つのは早い方がいい=depthが浅いほどプラス=depthが深いほどマイナス
        score += depth;
    } else if (score < 0) {
        // 負けるのは後の方がいい=depthが深いほどプラス
        score -= depth;
    }
    return score;
}

char gameover(GameUpdate *state) {
    if (state->my_body->val.x == state->enemy_body->val.x &&
        state->my_body->val.y == state->enemy_body->val.y) {
        return 1;
    }
    return 0;
}

int alphabeta(GameUpdate *node, int depth, int alpha, int beta,
              int isMaximizingPlayer) {
    node->up = NULL;
    node->down = NULL;
    node->left = NULL;
    node->right = NULL;
    if (depth == 0 || gameover(node)) {
        return eval(node, depth);
    }

    if (isMaximizingPlayer) {
        int maxEval = INT_MIN;
        char potential_moves =
            get_potential_moves(node->my_body, node->enemy_body,
                                node->my_body_length, node->enemy_body_length);
        for (int i = 0; i < 4; i++) {
            // printf("===自分の行動:depth=%d,move=%d↓\n", depth, i);
            int eval = INT_MIN;
            if (i == 0 && (potential_moves & UP)) {
                node->up = performMove(
                    node->my_body, node->enemy_body, node->my_body_length,
                    node->enemy_body_length, node->foods, 'u');
                node->up->parent = node;
                eval = alphabeta(node->up, depth - 1, alpha, beta, 0);
            } else if (i == 1 && (potential_moves & DOWN)) {
                node->down = performMove(
                    node->my_body, node->enemy_body, node->my_body_length,
                    node->enemy_body_length, node->foods, 'd');
                node->down->parent = node;
                eval = alphabeta(node->down, depth - 1, alpha, beta, 0);
            } else if (i == 2 && (potential_moves & LEFT)) {
                node->left = performMove(
                    node->my_body, node->enemy_body, node->my_body_length,
                    node->enemy_body_length, node->foods, 'l');
                node->left->parent = node;
                eval = alphabeta(node->left, depth - 1, alpha, beta, 0);
            } else if (i == 3 && (potential_moves & RIGHT)) {
                node->right = performMove(
                    node->my_body, node->enemy_body, node->my_body_length,
                    node->enemy_body_length, node->foods, 'r');
                node->right->parent = node;
                eval = alphabeta(node->right, depth - 1, alpha, beta, 0);
            } else {
                // printf("!depth%d,move%d:行けない\n", depth, i);
                continue;
            }
            // printf("!depth%d,move%d:評価値 = %d\n", depth, i, eval);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                // printf("===アルファカット===\n");
                break;  // アルファカットオフ
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        // 敵も同じ条件(自分が動く前)で判断させる
        char potential_moves = get_potential_moves(
            node->parent->enemy_body, node->parent->my_body,
            node->parent->enemy_body_length, node->parent->my_body_length);
        for (int i = 0; i < 4; i++) {
            // printf("===敵の行動:depth=%d,move=%d↓\n", depth, i);
            int eval = INT_MAX;
            if (i == 0 && (potential_moves & UP)) {
                node->up = performMove(node->enemy_body, node->my_body,
                                       node->enemy_body_length,
                                       node->my_body_length, node->foods, 'u');
                node->up->parent = node;
                Node *temp = node->up->my_body;
                node->up->my_body = node->up->enemy_body;
                node->up->enemy_body = temp;
                int temp_length = node->up->my_body_length;
                node->up->my_body_length = node->up->enemy_body_length;
                node->up->enemy_body_length = temp_length;
                eval = alphabeta(node->up, depth - 1, alpha, beta, 1);
            } else if (i == 1 && (potential_moves & DOWN)) {
                node->down = performMove(
                    node->enemy_body, node->my_body, node->enemy_body_length,
                    node->my_body_length, node->foods, 'd');
                node->down->parent = node;
                Node *temp = node->down->my_body;
                node->down->my_body = node->down->enemy_body;
                node->down->enemy_body = temp;
                int temp_length = node->down->my_body_length;
                node->down->my_body_length = node->down->enemy_body_length;
                node->down->enemy_body_length = temp_length;
                eval = alphabeta(node->down, depth - 1, alpha, beta, 1);
            } else if (i == 2 && (potential_moves & LEFT)) {
                node->left = performMove(
                    node->enemy_body, node->my_body, node->enemy_body_length,
                    node->my_body_length, node->foods, 'l');
                node->left->parent = node;
                Node *temp = node->left->my_body;
                node->left->my_body = node->left->enemy_body;
                node->left->enemy_body = temp;
                int temp_length = node->left->my_body_length;
                node->left->my_body_length = node->left->enemy_body_length;
                node->left->enemy_body_length = temp_length;
                eval = alphabeta(node->left, depth - 1, alpha, beta, 1);
            } else if (i == 3 && (potential_moves & RIGHT)) {
                node->right = performMove(
                    node->enemy_body, node->my_body, node->enemy_body_length,
                    node->my_body_length, node->foods, 'r');
                node->right->parent = node;
                Node *temp = node->right->my_body;
                node->right->my_body = node->right->enemy_body;
                node->right->enemy_body = temp;
                int temp_length = node->right->my_body_length;
                node->right->my_body_length = node->right->enemy_body_length;
                node->right->enemy_body_length = temp_length;
                eval = alphabeta(node->right, depth - 1, alpha, beta, 1);
            } else {
                // printf("!depth%d,move%d:行けない\n", depth, i);
                continue;
            }
            // printf("!depth%d,move%d:評価値 = %d\n", depth, i, eval);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) {
                // printf("===ベータカット===\n");
                break;  // ベータカットオフ
            }
        }
        return minEval;
    }
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

    GameUpdate *result = (GameUpdate *)malloc(sizeof(GameUpdate));
    result->my_body = my_body;
    result->enemy_body = enemy_body;
    result->foods = foods;
    result->my_body_length = my_body_length;
    result->enemy_body_length = enemy_body_length;
    result->parent = NULL;

    // アルファベータ法
    int depth = 17;  // (注意！)奇数にする！！！
    int alpha = INT_MIN;
    int beta = INT_MAX;

    int maxEval = INT_MIN;
    char bestMove = '\0';

    char potential_moves = get_potential_moves(
        my_body, enemy_body, my_body_length, enemy_body_length);
    if (potential_moves == UP) {
        printf("上しか行けない\n");
        return 'u';
    } else if (potential_moves == DOWN) {
        printf("下しか行けない\n");
        return 'd';
    } else if (potential_moves == LEFT) {
        printf("左しか行けない\n");
        return 'l';
    } else if (potential_moves == RIGHT) {
        printf("右しか行けない\n");
        return 'r';
    }
    ///////テスト的に実装
    if (my_body_length + enemy_body_length < 20) {
        depth = 17;
    }
    for (int i = 0; i < 4; i++) {
        int eval = INT_MIN;
        printf("=====自分の行動move:%d↓=====\n", i);
        if (i == 0 && (potential_moves & UP)) {
            result->up = performMove(
                result->my_body, result->enemy_body, result->my_body_length,
                result->enemy_body_length, result->foods, 'u');
            result->up->parent = result;
            eval = alphabeta(result->up, depth, alpha, beta, 0);
        } else if (i == 1 && (potential_moves & DOWN)) {
            result->down = performMove(
                result->my_body, result->enemy_body, result->my_body_length,
                result->enemy_body_length, result->foods, 'd');
            result->down->parent = result;
            eval = alphabeta(result->down, depth, alpha, beta, 0);
        } else if (i == 2 && (potential_moves & LEFT)) {
            result->left = performMove(
                result->my_body, result->enemy_body, result->my_body_length,
                result->enemy_body_length, result->foods, 'l');
            result->left->parent = result;
            eval = alphabeta(result->left, depth, alpha, beta, 0);
        } else if (i == 3 && (potential_moves & RIGHT)) {
            result->right = performMove(
                result->my_body, result->enemy_body, result->my_body_length,
                result->enemy_body_length, result->foods, 'r');
            result->right->parent = result;
            eval = alphabeta(result->right, depth, alpha, beta, 0);
        } else {
            printf("=====!!!move%d:行けない\n", i);
            continue;
        }
        printf("=====!!!move%dの最終的な評価値 = %d\n", i, eval);
        if (eval > maxEval) {
            maxEval = eval;
            if (i == 0) {
                best_move = 'u';
            } else if (i == 1) {
                best_move = 'd';
            } else if (i == 2) {
                best_move = 'l';
            } else if (i == 3) {
                best_move = 'r';
            }
        }
        alpha = max(alpha, eval);
    }

    free_linked_list(my_body);
    free_linked_list(enemy_body);
    free_linked_list(foods);

    // もし有効な動きが見つからなかった場合はランダムな動きを返します。
    if (best_move == '\0') {
        srand(time(NULL));
        char next_move = moves[rand() % 4];
        printf("next_move:%c\n", next_move);
        return next_move;
    } else {
        printf("best_move:%c\n", best_move);
    }

    return best_move;
}