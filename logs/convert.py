import os


def main():
    # スクリプトのあるディレクトリの絶対パスを取得
    script_dir = os.path.dirname(os.path.abspath(__file__))
    log_file_path = os.path.join(script_dir, "out.log")
    output_path = os.path.join(script_dir, "output.js")
    with open(log_file_path, "r") as file:
        lines = file.readlines()
        # 最初の行と最後の行を除外
        data_lines = lines[1:-1]

    # JSONの配列を作るために、各行からなるストリングのリストを作成
    json_data_lines = [line.strip() for line in data_lines]

    with open(output_path, "w") as file:
        file.write("var gameLog = [\n")
        file.write(",\n".join(json_data_lines))
        file.write("\n];")


if __name__ == "__main__":
    main()
