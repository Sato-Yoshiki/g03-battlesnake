#     //   ) )     //   ) )     //   ) )     //   / /     //   ) )     ___        ___   #
#    //           //___/ /     //   / /     //   / /     //___/ /    //   ) )   //   ) )#
#   //  ____     / ___ (      //   / /     //   / /     / ____ /    //   / /     __ / / #
#  //    / /    //   | |     //   / /     //   / /     //          //   / /         ) ) #
# ((____/ /    //    | |    ((___/ /     ((___/ /     //          ((___/ /    ((___/ /  #

import random
import typing


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


# floodfill法（を応用した方法）による空白の数え上げ
# 尻尾にアクセスできるときは50以上の数字が返ってくるがその大小は特に意味はない
def floodfill(board, x, y, visited, my_tail):
    if x < 0 or x >= 11 or y < 0 or y >= 11 or visited[x][y] or board[x][y] != 0:
        if my_tail["x"] == x and my_tail["y"] == y:
            return 0
        else:
            return 0
    visited[x][y] = True
    count = 1

    count += floodfill(board, x + 1, y, visited, my_tail)
    if count >= 50:
        return count
    count += floodfill(board, x - 1, y, visited, my_tail)
    if count >= 50:
        return count
    count += floodfill(board, x, y + 1, visited, my_tail)
    if count >= 50:
        return count
    count += floodfill(board, x, y - 1, visited, my_tail)
    return count


def move(game_state: typing.Dict) -> typing.Dict:
    # 消去法による安全な方向の絞り込み
    is_move_safe = {"up": True, "down": True, "left": True, "right": True}

    my_head = game_state["you"]["body"][0]
    my_neck = game_state["you"]["body"][1]
    my_tail = game_state["you"]["body"][-1]

    # 首がある方向には進めない
    if my_neck["x"] < my_head["x"]:
        is_move_safe["left"] = False

    elif my_neck["x"] > my_head["x"]:
        is_move_safe["right"] = False

    elif my_neck["y"] < my_head["y"]:
        is_move_safe["down"] = False

    elif my_neck["y"] > my_head["y"]:
        is_move_safe["up"] = False

    # 盤面外に出るのを防ぐ
    board_width = game_state["board"]["width"]
    board_height = game_state["board"]["height"]

    if my_head["x"] == 0:
        is_move_safe["left"] = False
    elif my_head["x"] == board_width - 1:
        is_move_safe["right"] = False
    if my_head["y"] == 0:
        is_move_safe["down"] = False
    elif my_head["y"] == board_height - 1:
        is_move_safe["up"] = False

    # 自分の体にぶつかる（一手先）のを防ぐ
    my_body = game_state["you"]["body"]

    my_head_up = {"x": my_head["x"], "y": my_head["y"] + 1}
    my_head_down = {"x": my_head["x"], "y": my_head["y"] - 1}
    my_head_left = {"x": my_head["x"] - 1, "y": my_head["y"]}
    my_head_right = {"x": my_head["x"] + 1, "y": my_head["y"]}

    # 頭の上のマスが自分の体（尻尾を除く）ではないか
    if is_move_safe["up"]:
        if my_head_up in my_body[:-1]:
            is_move_safe["up"] = False

    # 頭の下のマスが自分の体（尻尾を除く）ではないか
    if is_move_safe["down"]:
        if my_head_down in my_body[:-1]:
            is_move_safe["down"] = False

    # 頭の左のマスが自分の体（尻尾を除く）ではないか
    if is_move_safe["left"]:
        if my_head_left in my_body[:-1]:
            is_move_safe["left"] = False

    # 頭の右のマスが自分の体（尻尾を除く）ではないか
    if is_move_safe["right"]:
        if my_head_right in my_body[:-1]:
            is_move_safe["right"] = False

    safe_moves = []
    for move, isSafe in is_move_safe.items():
        if isSafe:
            safe_moves.append(move)

    # この段階で一方向に絞り込まれた場合はその方向へ進む
    if len(safe_moves) == 1:
        # print(f"MOVE {game_state['turn']}: safe moves [{safe_moves[0]}] detected")
        return {"move": safe_moves[0]}

    next_move = ""
    if len(safe_moves) != 0:
        # floodfill法で自分が自分に囲まれるのを防ぐ
        counts_right = 0  # 右方向から行ける空白のマスの数
        counts_left = 0  # 左方向から行ける空白のマスの数
        counts_up = 0  # 上方向から行ける空白のマスの数
        counts_down = 0  # 下方向から行ける空白のマスの数
        unsafe_moves = []
        board = [[0] * 11 for _ in range(11)]
        for i, body in enumerate(my_body):
            board[body["x"]][body["y"]] = i + 1
        if "right" in safe_moves:
            visited = [[False] * 11 for _ in range(11)]
            counts_right = floodfill(
                board, my_head["x"] + 1, my_head["y"], visited, my_tail
            )
            if counts_right < 50:
                unsafe_moves.append("right")
            elif counts_right == 50 and game_state["you"]["health"] >= 40:
                # print(f"MOVE {game_state['turn']}: tail is right")
                return {"move": "right"}
        if "left" in safe_moves:
            visited = [[False] * 11 for _ in range(11)]
            counts_left = floodfill(
                board, my_head["x"] - 1, my_head["y"], visited, my_tail
            )
            if counts_left < 50:
                unsafe_moves.append("left")
            elif counts_left == 50 and game_state["you"]["health"] >= 40:
                # print(f"MOVE {game_state['turn']}: tail is left")
                return {"move": "left"}
        if "up" in safe_moves:
            visited = [[False] * 11 for _ in range(11)]
            counts_up = floodfill(
                board, my_head["x"], my_head["y"] + 1, visited, my_tail
            )
            if counts_up < 50:
                unsafe_moves.append("up")
            elif counts_up == 50 and game_state["you"]["health"] >= 40:
                # print(f"MOVE {game_state['turn']}: tail is up")
                return {"move": "up"}
        if "down" in safe_moves:
            visited = [[False] * 11 for _ in range(11)]
            counts_down = floodfill(
                board, my_head["x"], my_head["y"] - 1, visited, my_tail
            )
            if counts_down < 50:
                unsafe_moves.append("down")
            elif counts_down == 50 and game_state["you"]["health"] >= 40:
                # print(f"MOVE {game_state['turn']}: tail is down")
                return {"move": "down"}
        not_unsafe_moves = []
        for move in safe_moves:
            if move not in unsafe_moves:
                not_unsafe_moves.append(move)
        if len(not_unsafe_moves) == 1:
            # print(f"MOVE {game_state['turn']}: {not_unsafe_moves[0]}")
            return {"move": not_unsafe_moves[0]}
        elif len(not_unsafe_moves) >= 2:
            safe_moves = not_unsafe_moves
        if len(safe_moves) == 2:
            # 左右しかないとき
            if "left" in safe_moves and "right" in safe_moves:
                # 下向き
                if my_head["y"] < my_neck["y"]:
                    next_move = "right"
                # 上向き
                else:
                    next_move = "left"
            # 上下しかないとき
            if "up" in safe_moves and "down" in safe_moves:
                # 左向き
                if my_head["x"] < my_neck["x"]:
                    next_move = "down"
                # 右向き
                else:
                    next_move = "up"
        if next_move == "":
            next_move = random.choice(safe_moves)

    # 餌が頭のすぐ隣に存在する時その方向へ進む
    if my_head_up in game_state["board"]["food"]:
        if "up" in safe_moves:
            next_move = "up"
    elif my_head_down in game_state["board"]["food"]:
        if "down" in safe_moves:
            next_move = "down"
    elif my_head_right in game_state["board"]["food"]:
        if "right" in safe_moves:
            next_move = "right"
    elif my_head_left in game_state["board"]["food"]:
        if "left" in safe_moves:
            next_move = "left"
    # 外側一周にいて、餌が内側にあればその方向へ向かう
    elif my_head["y"] == 0:
        if my_head["x"] == 0:
            if "up" in safe_moves:
                next_move = "up"
        elif my_head["x"] == 10:
            if "left" in safe_moves:
                next_move = "left"
        else:
            for food in game_state["board"]["food"]:
                if my_head["x"] == food["x"] and "up" in safe_moves:
                    next_move = "up"
    elif my_head["y"] == 10:
        if my_head["x"] == 0:
            if "right" in safe_moves:
                next_move = "right"
        elif my_head["x"] == 10:
            if "down" in safe_moves:
                next_move = "down"
        else:
            for food in game_state["board"]["food"]:
                if my_head["x"] == food["x"] and "down" in safe_moves:
                    next_move = "down"
    elif my_head["x"] == 0:
        for food in game_state["board"]["food"]:
            if my_head["y"] == food["y"] and "right" in safe_moves:
                next_move = "right"
    elif my_head["x"] == 10:
        for food in game_state["board"]["food"]:
            if my_head["y"] == food["y"] and "left" in safe_moves:
                next_move = "left"

    # 向いている方向（餌のある方向）に進み続ける
    elif my_head["y"] != my_neck["y"]:
        if my_head["y"] < my_neck["y"]:
            if "down" in safe_moves:
                next_move = "down"
        elif "up" in safe_moves:
            next_move = "up"
    else:
        if my_head["x"] < my_neck["x"]:
            if "left" in safe_moves:
                next_move = "left"
        elif "right" in safe_moves:
            next_move = "right"
    # print(f"MOVE {game_state['turn']}: {next_move}")
    return {"move": next_move}


if __name__ == "__main__":
    from server import run_server

    run_server({"info": info, "start": start, "move": move, "end": end})
