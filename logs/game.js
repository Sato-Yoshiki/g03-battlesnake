// 初期設定など
var currentTurn = 0;
var autoPlayInterval;
var boardSize = 11;

function drawBoard(turnData) {
    var board = document.getElementById('game-board');
    board.innerHTML = ''; // Clear the board

    for (var y = 0; y < boardSize; y++) {
        for (var x = 0; x < boardSize; x++) {
            var cell = document.createElement('div');
            cell.classList.add('cell');

            var invertedY = boardSize - 1 - y; // Y座標を逆にする

            turnData.board.food.forEach(function (food) {
                if (food.x === x && food.y === invertedY) {
                    cell.classList.add('food');
                }
            });

            turnData.board.snakes.forEach(function (snake, index) {
                snake.body.forEach(function (bodyPart) {
                    if (bodyPart.x === x && bodyPart.y === invertedY) {
                        if (index === 0) {
                            cell.classList.add('snake1');
                        } else {
                            cell.classList.add('snake2');
                        }
                    }
                });
            });

            board.appendChild(cell);
        }
    }
}

function updateSnakeInfo(turnData) {
    var snakeInfoDiv = document.getElementById('snake-info');
    snakeInfoDiv.innerHTML = ''; // Clear the info

    turnData.board.snakes.forEach(function (snake, index) {
        var snakeInfo = document.createElement('p');
        var color = index === 0 ? '#f81900' : 'blue';
        snakeInfo.style.color = color;
        snakeInfo.textContent = 'Snake ' + (index + 1) + ': ' + snake.name + ', Health: ' + snake.health;
        snakeInfoDiv.appendChild(snakeInfo);
    });
}

function changeTurn(turn) {
    currentTurn = turn;
    if (currentTurn < 0) currentTurn = 0;
    if (currentTurn >= gameLog.length) currentTurn = gameLog.length - 1;
    drawBoard(gameLog[currentTurn]);
    updateSnakeInfo(gameLog[currentTurn]); // Add this line
}

function nextTurn() {
    changeTurn(currentTurn + 1);
}

function prevTurn() {
    changeTurn(currentTurn - 1);
}

function firstTurn() {
    changeTurn(0);
}

function lastTurn() {
    changeTurn(gameLog.length - 1);
}

function autoPlay() {
    if (autoPlayInterval) {
        clearInterval(autoPlayInterval);
        autoPlayInterval = null;
    } else {
        autoPlayInterval = setInterval(function () {
            if (currentTurn < gameLog.length - 1) {
                nextTurn();
            } else {
                clearInterval(autoPlayInterval); // 最後のターンに達したら停止
                autoPlayInterval = null; // インターバルIDをクリア
            }
        }, 40);
    }
}

// 最初のターンを描画
drawBoard(gameLog[currentTurn]);