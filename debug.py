#     //   ) )     //   ) )     //   ) )     //   / /     //   ) )     ___        ___   #
#    //           //___/ /     //   / /     //   / /     //___/ /    //   ) )   //   ) )#
#   //  ____     / ___ (      //   / /     //   / /     / ____ /    //   / /     __ / / #
#  //    / /    //   | |     //   / /     //   / /     //          //   / /         ) ) #
# ((____/ /    //    | |    ((___/ /     ((___/ /     //          ((___/ /    ((___/ /  #

import time
import typing
from ctypes import POINTER, Structure, byref, c_char, c_int, cdll

from getch import getch


def info() -> typing.Dict:
    print("INFO")
    return {
        "apiversion": "1",
        "author": "g03",
        "color": "#000000",
        "head": "tiger-king",
        "tail": "rbc-necktie",
    }


def start(game_state: typing.Dict):
    print("GAME START")


def end(game_state: typing.Dict):
    print("GAME OVER\n")


def move(game_state: typing.Dict) -> typing.Dict:
    next_move = "up"
    while True:
        key = ord(getch())
        if key == 119:
            next_move = "up"
            break
        elif key == 115:
            next_move = "down"
            break
        elif key == 97:
            next_move = "left"
            break
        elif key == 100:
            next_move = "right"
            break
    return {"move": next_move}


if __name__ == "__main__":
    from server import run_server

    run_server({"info": info, "start": start, "move": move, "end": end})
