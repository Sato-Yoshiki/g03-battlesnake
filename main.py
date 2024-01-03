#     //   ) )     //   ) )     //   ) )     //   / /     //   ) )     ___        ___   #
#    //           //___/ /     //   / /     //   / /     //___/ /    //   ) )   //   ) )#
#   //  ____     / ___ (      //   / /     //   / /     / ____ /    //   / /     __ / / #
#  //    / /    //   | |     //   / /     //   / /     //          //   / /         ) ) #
# ((____/ /    //    | |    ((___/ /     ((___/ /     //          ((___/ /    ((___/ /  #

import time
import typing
from ctypes import POINTER, Structure, byref, c_char, c_int, cdll


def info() -> typing.Dict:
    print("INFO")
    return {
        "apiversion": "1",
        "author": "g03",
        "color": "#ffe4e1",
        "head": "sand-worm",
        "tail": "block-bum",
    }


def start(game_state: typing.Dict):
    print("GAME START")


def end(game_state: typing.Dict):
    print("GAME OVER\n")


class Point(Structure):
    _fields_ = [("x", c_int), ("y", c_int)]


class Snake(Structure):
    _fields_ = [("body", POINTER(Point)), ("body_length", c_int), ("health", c_int)]


class GameData(Structure):
    _fields_ = [("foods", POINTER(Point)), ("food_count", c_int)]


# Cライブラリのロード
lib = cdll.LoadLibrary("./main.so")
lib.move.argtypes = (POINTER(GameData), POINTER(Snake), POINTER(Snake))
lib.move.restype = c_char


# PythonのデータをCの構造体に変換する関数
def convert_to_c_types(game_state):
    # foodの変換
    food_list = game_state["board"]["food"]
    FoodArrayType = Point * len(food_list)
    food_array = FoodArrayType(*[Point(f["x"], f["y"]) for f in food_list])

    # my_bodyの変換
    my_body_list = game_state["you"]["body"]
    BodyArrayType = Point * len(my_body_list)
    my_body_array = BodyArrayType(
        *[Point(part["x"], part["y"]) for part in my_body_list]
    )

    # enemy_bodyの変換
    enemy = game_state["board"]["snakes"][0]
    if (
        game_state["you"]["id"] == enemy["id"]
        and len(game_state["board"]["snakes"]) > 1
    ):
        enemy = game_state["board"]["snakes"][1]
    enemy_body_list = enemy["body"]
    EnemyBodyArrayType = Point * len(enemy_body_list)
    enemy_body_array = EnemyBodyArrayType(
        *[Point(part["x"], part["y"]) for part in enemy_body_list]
    )

    # GameDataのインスタンスを作成
    game_data = GameData(foods=food_array, food_count=len(food_list))

    # Snakeインスタンスの作成
    my_snake = Snake(
        body=my_body_array,
        body_length=len(my_body_list),
        health=game_state["you"]["health"],
    )

    # Enemy Snakeインスタンスの作成
    enemy_snake = Snake(
        body=enemy_body_array,
        body_length=len(enemy_body_list),
        health=enemy["health"],
    )

    return game_data, my_snake, enemy_snake


def move(game_state: typing.Dict) -> typing.Dict:
    # 11×11の盤面であることを前提としておりその確認は行わない
    game_data, my_snake, enemy_snake = convert_to_c_types(game_state)
    next_move = lib.move(byref(game_data), byref(my_snake), byref(enemy_snake))
    if next_move == b"u":
        next_move = "up"
    elif next_move == b"d":
        next_move = "down"
    elif next_move == b"r":
        next_move = "right"
    elif next_move == b"l":
        next_move = "left"
    else:
        print("Cの戻り値エラーが発生")
        next_move = "up"
    # デバッグ用遅延　本番では削除！！！！！！！！
    # time.sleep(0.48)
    return {"move": next_move}


if __name__ == "__main__":
    from server import run_server

    run_server({"info": info, "start": start, "move": move, "end": end})
