#!/bin/bash
if [ ! -d "./logs/history/" ]; then
  mkdir "./logs/history/"
fi
./battlesnake_Linux/battlesnake play -W 11 -H 11 -n 'Python1' --url http://localhost:8000 -n 'Python2' --url http://localhost:8001 -o ./logs/history/out.log -v -g duel
#./battlesnake_Linux/battlesnake play -W 11 -H 11 -n 'Python1' --url https://g03.pbl-b.repl.co -n 'Python2' --url https://g03.pbl-b.repl.co -g duel