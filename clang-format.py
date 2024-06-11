# This Python file uses the following encoding: utf-8

import os
import subprocess
import threading
from queue import Queue

# 指定根目录和clang-format路径
root_dir = "."
clang_format_path = "clang-format"

# 定义文件类型列表
file_types = [".cpp", ".h"]
jump_dirs = ["build"]

# 定义文件队列
file_queue = Queue()

# 定义执行clang-format的函数
def run_clang_format():
    while True:
        file_path = file_queue.get()
        if file_path is None:
            break
        try:
            subprocess.call([clang_format_path, "-i", file_path, "--style=file"])
            print(f"正在格式化文件：{file_path}")
        except Exception as e:
            print(f"格式化文件 {file_path} 时出错: {e}")
        file_queue.task_done()

# 遍历文件夹并收集满足要求的文件
def traverse_and_collect(root_dir):
    for root, dirs, files in os.walk(root_dir):
        # 跳过 jump_dirs 中的目录
        dirs[:] = [d for d in dirs if d not in jump_dirs]
        for file in files:
            if file.endswith(tuple(file_types)):
                file_queue.put(os.path.join(root, file))

# 创建并启动worker线程
num_worker_threads = 4
threads = []
for _ in range(num_worker_threads):
    t = threading.Thread(target=run_clang_format)
    t.start()
    threads.append(t)

# 遍历目录并收集文件
traverse_and_collect(root_dir)

# 等待所有文件都被处理
file_queue.join()

# 停止worker线程
for _ in range(num_worker_threads):
    file_queue.put(None)
for t in threads:
    t.join()

print("所有文件格式化完成")
