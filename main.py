#     //   ) )     //   ) )     //   ) )     //   / /     //   ) )     ___        ___   #
#    //           //___/ /     //   / /     //   / /     //___/ /    //   ) )   //   ) )#
#   //  ____     / ___ (      //   / /     //   / /     / ____ /    //   / /     __ / / #
#  //    / /    //   | |     //   / /     //   / /     //          //   / /         ) ) #
# ((____/ /    //    | |    ((___/ /     ((___/ /     //          ((___/ /    ((___/ /  #

import threading
import time
import typing
import main2
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


def call_lib_move(game_data, my_snake, enemy_snake, result_container, depth):
    result = lib.move(byref(game_data), byref(my_snake), byref(enemy_snake), depth)
    result_container.append(result)


# 11×11の盤面であることを前提としておりその確認は行わない
def move(game_state: typing.Dict) -> typing.Dict:
    print(f"==========MOVE {game_state['turn']}: 処理開始==========")
    before_timeout = game_state["you"]["latency"]
    print(f"前回latency{before_timeout}")
    # PythonのデータをCの構造体に変換
    game_data, my_snake, enemy_snake = convert_to_c_types(game_state)

    # depth設定（注意：設定できるのは奇数のみ）
    # 対戦環境では15は安定17は場合による
    depth = 15

    move_result = []
    move_thread = threading.Thread(
        target=call_lib_move,
        args=(game_data, my_snake, enemy_snake, move_result, depth),
    )
    move_thread.start()

    # 並行してPythonでも簡単な処理を行う
    # start = time.perf_counter()
    Python_result = main2.move(game_state)
    next_move = Python_result["move"]
    # print(time.perf_counter() - start)

    # タイムアウトを秒で設定
    TIMEOUT_SECONDS = 0.49
    move_thread.join(TIMEOUT_SECONDS)

    if not move_result:
        print("タイムアウト: Cの関数が時間内に完了しませんでした。")
    else:
        next_move = move_result[0]
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
            next_move = "\0"
    if next_move == "\0":
        print("エラー:次の動きがCでもPythonでも取得できませんでした")
        return {"move": next_move}  # 意図的にヌル文字を送ることで向いている方向へ進ませる
    print(f"MOVE {game_state['turn']}: {next_move}")
    return {"move": next_move}


if __name__ == "__main__":
    from server import run_server

    run_server({"info": info, "start": start, "move": move, "end": end})
