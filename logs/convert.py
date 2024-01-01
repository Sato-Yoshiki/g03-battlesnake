import os
import shutil
from datetime import datetime


def main():
    # スクリプトのあるディレクトリの絶対パスを取得
    script_dir = os.path.dirname(os.path.abspath(__file__))
    log_file_path = os.path.join(script_dir, "history", "out.log")
    output_path = os.path.join(script_dir, "history", "output.js")
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

    # ファイルをコピー
    today_folder_name = datetime.now().strftime("%Y%m%d")
    today_folder_path = os.path.join(script_dir, "history", today_folder_name)
    if not os.path.exists(today_folder_path):
        os.makedirs(today_folder_path)
    file_index = int(len(os.listdir(today_folder_path)) / 2 + 1)
    copy_log_filename = f"out{file_index}.log"
    copy_js_filename = f"output{file_index}.js"
    copy_log_path = os.path.join(today_folder_path, copy_log_filename)
    shutil.copy2(log_file_path, copy_log_path)
    copy_js_path = os.path.join(today_folder_path, copy_js_filename)
    shutil.copy2(output_path, copy_js_path)


if __name__ == "__main__":
    main()
