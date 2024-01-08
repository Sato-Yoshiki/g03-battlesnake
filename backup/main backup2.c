#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    char
        survivor;  // '2'はどちらも生存、'm'は自分だけ生存、'e'は敵だけ生存、'n'はどちらも生存していない。
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

GameUpdate performMove(Node *my_body, Node *enemy_body, Node *foods,
                       char my_move, char enemy_move) {
    GameUpdate update;
    update.survivor = '2';

    // 各連結リストをディープコピー
    update.my_body = deepCopyLinkedList(my_body);
    update.enemy_body = deepCopyLinkedList(enemy_body);
    update.foods = deepCopyLinkedList(foods);

    // 自分の動きを実行
    Node *new_my_head = (Node *)malloc(sizeof(Node));
    new_my_head->next = update.my_body;
    new_my_head->val.x = update.my_body->val.x;
    new_my_head->val.y = update.my_body->val.y;
    update.my_body = new_my_head;

    if (my_move == 'u')
        update.my_body->val.y += 1;
    else if (my_move == 'd')
        update.my_body->val.y -= 1;
    else if (my_move == 'l')
        update.my_body->val.x -= 1;
    else if (my_move == 'r')
        update.my_body->val.x += 1;

    if (update.my_body->val.x < 0 || update.my_body->val.x > 10 ||
        update.my_body->val.y < 0 || update.my_body->val.y > 10) {
        update.survivor = 'e';
    }

    Node *food = update.foods;
    while (food != NULL) {
        if (food->val.x == update.my_body->val.x &&
            food->val.y == update.my_body->val.y) {
            break;
        }
        food = food->next;
    }
    if (food == NULL) {
        Node *del_body = update.my_body;
        while (del_body->next->next != NULL) {
            del_body = del_body->next;
        }
        free(del_body->next);
        del_body->next = NULL;
    }

    // 敵の動きを実行
    Node *new_enemy_head = (Node *)malloc(sizeof(Node));
    new_enemy_head->next = update.enemy_body;
    new_enemy_head->val.x = update.enemy_body->val.x;
    new_enemy_head->val.y = update.enemy_body->val.y;
    update.enemy_body = new_enemy_head;

    if (enemy_move == 'u')
        update.enemy_body->val.y += 1;
    else if (enemy_move == 'd')
        update.enemy_body->val.y -= 1;
    else if (enemy_move == 'l')
        update.enemy_body->val.x -= 1;
    else if (enemy_move == 'r')
        update.enemy_body->val.x += 1;

    if (update.enemy_body->val.x < 0 || update.enemy_body->val.x > 10 ||
        update.enemy_body->val.y < 0 || update.enemy_body->val.y > 10) {
        if (update.survivor == '2') {
            update.survivor = 'm';
        } else {
            update.survivor = 'n';
        }
    }

    food = update.foods;
    while (food != NULL) {
        if (food->val.x == update.enemy_body->val.x &&
            food->val.y == update.enemy_body->val.y) {
            break;
        }
        food = food->next;
    }
    if (food == NULL) {
        Node *del_body = update.enemy_body;
        while (del_body->next->next != NULL) {
            del_body = del_body->next;
        }
        free(del_body->next);
        del_body->next = NULL;
    }

    // 自分が体にぶつかっていないか
    if (update.survivor == '2' || update.survivor == 'm') {
        Node *current = update.enemy_body;
        while (current != NULL) {
            printf("%d %d,", current->val.x,
                   current->val.y);  ////////////////////

            if (update.my_body->val.x == current->val.x &&
                update.my_body->val.y == current->val.y) {
                if (update.survivor == '2') {
                    update.survivor = 'e';
                } else {
                    update.survivor = 'n';
                }
                break;
            }
            current = current->next;
        }

        printf("\n");  /////////////

        current = update.my_body->next;
        while (current != NULL) {
            printf("X%d=%d Y%d=%d, ", update.my_body->val.x, current->val.x,
                   update.my_body->val.y, current->val.y);  ////////////////////

            if (update.my_body->val.x == current->val.x &&
                update.my_body->val.y == current->val.y) {
                if (update.survivor == '2') {
                    update.survivor = 'e';
                } else {
                    update.survivor = 'n';
                }
                break;
            }
            current = current->next;
        }
        printf("\n");  /////////////
    }
    // 相手が体にぶつかっていないか
    if (update.survivor == '2' || update.survivor == 'e') {
        Node *current = update.my_body;
        while (current != NULL) {
            if (update.enemy_body->val.x == current->val.x &&
                update.enemy_body->val.y == current->val.y) {
                if (update.survivor == '2') {
                    update.survivor = 'm';
                } else {
                    update.survivor = 'n';
                }
                break;
            }
            current = current->next;
        }
        current = update.enemy_body->next;
        while (current != NULL) {
            if (update.enemy_body->val.x == current->val.x &&
                update.enemy_body->val.y == current->val.y) {
                if (update.survivor == '2') {
                    update.survivor = 'm';
                } else {
                    update.survivor = 'n';
                }
                break;
            }
            current = current->next;
        }
    }

    return update;
}

char move(GameData *data, Snake *my_snake, Snake *enemy_snake) {
    // すべての可能な動きのための配列を連結リストに変換
    Node *my_body = array_to_linked_list(my_snake->body, my_snake->body_length);
    Node *enemy_body =
        array_to_linked_list(enemy_snake->body, enemy_snake->body_length);
    Node *foods = array_to_linked_list(data->foods, data->food_count);

    char best_move = '\0';
    char survivor;

    // 仮定に基づいてゲームの現状を最も良くする動きを見つけえる
    int best_score = -1;
    for (int i = 0; i < 4; i++) {
        char my_potential_move = moves[i];

        for (int j = 0; j < 4; j++) {
            char enemy_potential_move = moves[j];

            GameUpdate result =
                performMove(my_body, enemy_body, foods, my_potential_move,
                            enemy_potential_move);

            // スコアリングの論理を実装
            // - 'm' (自分だけが生存) の場合、高くスコア
            // - '2' (どちらも生存) の場合、次に良いスコア。
            // - 'n' または 'e' の場合、低いスコア。
            int score;
            switch (result.survivor) {
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

            if (score > best_score) {
                best_score = score;
                best_move = my_potential_move;
                survivor = result.survivor;
            }
            /*
            free_linked_list(result.my_body);
            free_linked_list(result.enemy_body);
            free_linked_list(result.foods);
            */
        }
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