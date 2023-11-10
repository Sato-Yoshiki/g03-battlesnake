# Welcome to
# __________         __    __  .__                               __
# \______   \_____ _/  |__/  |_|  |   ____   ______ ____ _____  |  | __ ____
#  |    |  _/\__  \\   __\   __\  | _/ __ \ /  ___//    \\__  \ |  |/ // __ \
#  |    |   \ / __ \|  |  |  | |  |_\  ___/ \___ \|   |  \/ __ \|    <\  ___/
#  |________/(______/__|  |__| |____/\_____>______>___|__(______/__|__\\_____>
#
# This file can be a nice home for your Battlesnake logic and helper functions.
#
# To get you started we've included code to prevent your Battlesnake from moving backwards.
# For more info see docs.battlesnake.com

import random
import typing


# info is called when you create your Battlesnake on play.battlesnake.com
# and controls your Battlesnake's appearance
# TIP: If you open your Battlesnake URL in a browser you should see this data
def info() -> typing.Dict:
    print("INFO")
    return {
        "apiversion": "1",
        "author": "g03",  # TODO: Your Battlesnake Username
        "color": "#ffe4e1",  # TODO: Choose color
        "head": "sand-worm",  # TODO: Choose head
        "tail": "block-bum",  # TODO: Choose tail
    }


# start is called when your Battlesnake begins a game
def start(game_state: typing.Dict):
    print("GAME START")


# end is called when your Battlesnake finishes a game
def end(game_state: typing.Dict):
    print("GAME OVER\n")


# move is called on every turn and returns your next move
# Valid moves are "up", "down", "left", or "right"
# See https://docs.battlesnake.com/api/example-move for available data
def move(game_state: typing.Dict) -> typing.Dict:
    is_move_safe = {"up": True, "down": True, "left": True, "right": True}

    # We've included code to prevent your Battlesnake from moving backwards
    my_head = game_state["you"]["body"][0]  # Coordinates of your head
    my_neck = game_state["you"]["body"][1]  # Coordinates of your "neck"

    if my_neck["x"] < my_head["x"]:  # Neck is left of head, don't move left
        is_move_safe["left"] = False

    elif my_neck["x"] > my_head["x"]:  # Neck is right of head, don't move right
        is_move_safe["right"] = False

    elif my_neck["y"] < my_head["y"]:  # Neck is below head, don't move down
        is_move_safe["down"] = False

    elif my_neck["y"] > my_head["y"]:  # Neck is above head, don't move up
        is_move_safe["up"] = False

    # TODO: Step 1 - Prevent your Battlesnake from moving out of bounds
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
    # TODO: Step 2 - Prevent your Battlesnake from colliding with itself
    my_body = game_state["you"]["body"]

    my_head_up = {"x": my_head["x"], "y": my_head["y"] + 1}
    my_head_down = {"x": my_head["x"], "y": my_head["y"] - 1}
    my_head_left = {"x": my_head["x"] - 1, "y": my_head["y"]}
    my_head_right = {"x": my_head["x"] + 1, "y": my_head["y"]}

    # 上の確認
    if is_move_safe["up"]:
        if my_head_up in my_body[:-1]:
            is_move_safe["up"] = False

    # 下方向の確認
    if is_move_safe["down"]:
        if my_head_down in my_body[:-1]:
            is_move_safe["down"] = False

    # 左方向の確認
    if is_move_safe["left"]:
        if my_head_left in my_body[:-1]:
            is_move_safe["left"] = False

    # 右方向の確認
    if is_move_safe["right"]:
        if my_head_right in my_body[:-1]:
            is_move_safe["right"] = False

    # Are there any safe moves left?
    safe_moves = []
    for move, isSafe in is_move_safe.items():
        if isSafe:
            safe_moves.append(move)

    if len(safe_moves) == 1:
        print(f"MOVE {game_state['turn']}: safe moves detected")
        return {"move": safe_moves[0]}

    # Choose a random move from the safe ones
    next_move = ""
    if len(safe_moves) != 0:
        unsafe_moves = []
        if "up" in safe_moves:
            my_head_up_left = {"x": my_head_up["x"] - 1, "y": my_head_up["y"]}
            if my_head_up_left["x"] < 0 or my_head_up_left in my_body[:-2]:
                my_head_up_right = {"x": my_head_up["x"] + 1, "y": my_head_up["y"]}
                if my_head_up_right["x"] >= 7 or my_head_up_right in my_body[:-2]:
                    my_head_up_up = {"x": my_head_up["x"], "y": my_head_up["y"] + 1}
                    if my_head_up_up["y"] >= 7 or my_head_up_up in my_body[:-2]:
                        unsafe_moves.append("up")
        if "down" in safe_moves:
            my_head_down_left = {"x": my_head_down["x"] - 1, "y": my_head_down["y"]}
            if my_head_down_left["x"] < 0 or my_head_down_left in my_body[:-2]:
                my_head_down_right = {
                    "x": my_head_down["x"] + 1,
                    "y": my_head_down["y"],
                }
                if my_head_down_right["x"] >= 7 or my_head_down_right in my_body[:-2]:
                    my_head_down_down = {
                        "x": my_head_down["x"],
                        "y": my_head_down["y"] - 1,
                    }
                    if my_head_down_down["y"] < 0 or my_head_down_down in my_body[:-2]:
                        unsafe_moves.append("down")
        if "right" in safe_moves:
            my_head_right_up = {"x": my_head_right["x"], "y": my_head_right["y"] + 1}
            if my_head_right_up["y"] >= 7 or my_head_right_up in my_body[:-2]:
                my_head_right_right = {
                    "x": my_head_right["x"] + 1,
                    "y": my_head_right["y"],
                }
                if my_head_right_right["x"] >= 7 or my_head_right_right in my_body[:-2]:
                    my_head_right_down = {
                        "x": my_head_right["x"],
                        "y": my_head_right["y"] - 1,
                    }
                    if (
                        my_head_right_down["y"] < 0
                        or my_head_right_down in my_body[:-2]
                    ):
                        unsafe_moves.append("right")
        if "left" in safe_moves:
            my_head_left_up = {"x": my_head_left["x"], "y": my_head_left["y"] + 1}
            if my_head_left_up["y"] >= 7 or my_head_left_up in my_body[:-2]:
                my_head_left_left = {
                    "x": my_head_left["x"] - 1,
                    "y": my_head_left["y"],
                }
                if my_head_left_left["x"] < 0 or my_head_left_left in my_body[:-2]:
                    my_head_left_down = {
                        "x": my_head_left["x"],
                        "y": my_head_left["y"] - 1,
                    }
                    if my_head_left_down["y"] < 0 or my_head_left_down in my_body[:-2]:
                        unsafe_moves.append("left")
        if len(unsafe_moves) >= 1:
            print("unsafe_moves:", unsafe_moves)
            next_safe_moves = []
            for move in safe_moves:
                if move not in unsafe_moves:
                    next_safe_moves.append(move)
            if len(next_safe_moves) == 1:
                print(f"MOVE {game_state['turn']}: {next_safe_moves[0]}")
                return {"move": next_safe_moves[0]}
            elif len(next_safe_moves) >= 2:
                safe_moves = next_safe_moves
            else:
                print("デバック用")
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
    first_next_moves = [
        ["up", "up", "up", "up", "up", "up", "right"],
        ["left", "up", "left", "left", "left", "right", "right"],
        ["left", "down", "up", "left", "right", "up", "right"],
        ["left", "down", "down", "left", "up", "up", "right"],
        ["left", "down", "left", "right", "down", "up", "right"],
        ["left", "left", "right", "right", "right", "down", "right"],
        ["left", "down", "down", "down", "down", "down", "down"],
    ]
    first_next_move = first_next_moves[my_head["x"]][my_head["y"]]
    if first_next_move in safe_moves:
        next_move = first_next_move

    if game_state["you"]["health"] >= 19:
        print(f"MOVE {game_state['turn']}: {next_move}")
        return {"move": next_move}
    else:
        if my_head["y"] == 0:
            if my_head["x"] == 0:
                if "up" in safe_moves:
                    next_move = "up"
            elif my_head["x"] == 6:
                if "left" in safe_moves:
                    next_move = "left"
            else:
                for food in game_state["board"]["food"]:
                    if my_head["x"] == food["x"] and "up" in safe_moves:
                        next_move = "up"
        elif my_head["y"] == 6:
            if my_head["x"] == 0:
                if "right" in safe_moves:
                    next_move = "right"
            elif my_head["x"] == 6:
                if "down" in safe_moves:
                    next_move = "down"
            else:
                for food in game_state["board"]["food"]:
                    if my_head["x"] == food["x"] and "down" in safe_moves:
                        next_move = "down"
        elif my_head["y"] != my_neck["y"]:
            if my_head["y"] < my_neck["y"]:
                if "down" in safe_moves:
                    next_move = "down"
            elif "up" in safe_moves:
                next_move = "up"
        print(f"MOVE {game_state['turn']}: {next_move}")
        return {"move": next_move}


# Start server when `python main.py` is run
if __name__ == "__main__":
    from server import run_server

    run_server({"info": info, "start": start, "move": move, "end": end})
