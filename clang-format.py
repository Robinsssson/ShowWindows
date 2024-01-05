# This Python file uses the following encoding: utf-8

# if __name__ == "__main__":
#     pass

import os
import subprocess
import threading

# 指定根目录和clang-format路径
root_dir = ".\\"
clang_format_path = "clang-format"

# 定义文件类型列表
file_types = [".cpp", ".h"]


# 定义执行clang-format的函数
def run_clang_format(file_list):
    for file_path in file_list:
        print(f"正在格式化文件：{file_path}")
        subprocess.call([clang_format_path, "-i", file_path, "--style=file"])
        print(f"文件格式化完成：{file_path}")
    # os.system("clang-format -i %s -style=File" % (" ".join(file_list)))


# 遍历文件夹并收集满足要求的文件
def traverse_and_collect(root_dir):
    file_list = []
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            file_path = os.path.join(root, file)
            if file_path.endswith(tuple(file_types)):
                file_list.append(file_path)
                if len(file_list) == 10:
                    thread = threading.Thread(
                        target=run_clang_format, args=(file_list,)
                    )
                    thread.start()
                    file_list = []
    if file_list:
        thread = threading.Thread(target=run_clang_format, args=(file_list,))
        thread.start()


# 创建线程用于遍历根目录并收集文件
traverse_thread = threading.Thread(target=traverse_and_collect, args=(root_dir,))
traverse_thread.start()

# 等待遍历线程完成
traverse_thread.join()
