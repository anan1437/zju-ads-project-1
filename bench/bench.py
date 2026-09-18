# -*- coding: utf-8 -*-
# 评测程序：编译并加载各搜索树实现，做性能测试。
# 未实现、编译失败或加载失败的模块会被自动跳过，只执行能运行的部分。
# 用法：python bench/bench.py

import ctypes
import os
import random
import shutil
import subprocess
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   #项目根目录
SRC_DIR = os.path.join(ROOT, "src")
BUILD_DIR = os.path.join(ROOT, "build")
OUT_DIR = os.path.join(ROOT, "bench", "out")

IMPLS = ["bst", "avl", "splay"]                #三种实现，按目录名取
SIZES = list(range(1000, 10001, 1000))         #输入规模 1000 ~ 10000
SEED = 42                                #固定随机种子，保证结果可复现

SCENARIOS = [                                  #题目的三种测试方式
    ("增序插入-同序删除", "increase", "increase"),
    ("增序插入-逆序删除", "increase", "reverse"),
    ("随机插入-随机删除", "random",   "random"),
]

PROTOTYPES = {                                 #五个接口的类型签名
    "create":  (ctypes.c_void_p, []),
    "insert":  (ctypes.c_int,    [ctypes.c_void_p, ctypes.c_int]),
    "erase":   (ctypes.c_int,    [ctypes.c_void_p, ctypes.c_int]),
    "find":    (ctypes.c_int,    [ctypes.c_void_p, ctypes.c_int]),
    "destroy": (None,            [ctypes.c_void_p]),
}


def compile_impl(name):
    """把 src/<name>/<name>.c 编译成动态库，返回 (路径, 错误信息)。"""
    src = os.path.join(SRC_DIR, name, name + ".c")
    if not os.path.exists(src):
        return None, "源文件不存在"
    gcc = shutil.which("gcc")
    if not gcc:
        return None, "找不到 gcc"
    ext = ".dll" if os.name == "nt" else ".so"
    out = os.path.join(BUILD_DIR, name + ext)
    cmd = [gcc, "-std=c99", "-O2", "-Wall", "-Wextra", "-shared", "-fPIC", src, "-o", out]
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        lines = [l for l in proc.stderr.splitlines() if l.strip()]
        return None, lines[-1] if lines else "gcc 编译失败"
    return out, None


def load_impl(name, path):
    """加载动态库并取出五个函数，返回 (函数字典, 错误信息)。"""
    try:
        lib = ctypes.CDLL(path)
    except OSError as e:
        return None, "加载动态库失败: %s" % e
    funcs = {}
    for fn, (restype, argtypes) in PROTOTYPES.items():
        try:
            f = getattr(lib, "%s_%s" % (name, fn))
        except AttributeError:
            return None, "缺少函数 %s_%s" % (name, fn)
        f.restype = restype
        f.argtypes = argtypes
        funcs[fn] = f
    return funcs, None


def gen_orders(n, insert_mode, delete_mode, rng):
    """按场景生成插入序列和删除序列，键取自 1..n 的互不相同整数。"""
    base = list(range(1, n + 1))
    ins = base[:] if insert_mode == "increase" else rng.sample(base, n)
    if delete_mode == "increase":
        dele = sorted(ins)
    elif delete_mode == "reverse":
        dele = sorted(ins, reverse=True)
    else:
        dele = rng.sample(ins, n)
    return ins, dele


def run_case(funcs, ins, dele):
    """测一组插入+删除的耗时，返回 (插入秒, 删除秒)。"""
    t = funcs["create"]()
    if not t:
        raise RuntimeError("create 返回 NULL")
    start = time.perf_counter()
    for k in ins:
        if funcs["insert"](t, k) != 0:
            funcs["destroy"](t)
            raise RuntimeError("insert(%d) 失败" % k)
    t_insert = time.perf_counter() - start
    start = time.perf_counter()
    for k in dele:
        if funcs["erase"](t, k) != 0:
            funcs["destroy"](t)
            raise RuntimeError("erase(%d) 失败" % k)
    t_delete = time.perf_counter() - start
    funcs["destroy"](t)
    return t_insert, t_delete


def save_table(rows):
    """把运行时间表存成 CSV。"""
    path = os.path.join(OUT_DIR, "table.csv")
    with open(path, "w", encoding="utf-8-sig") as f:
        f.write("实现,场景,N,插入时间(ms),删除时间(ms)\n")
        for name, scene, n, ti, td in rows:
            f.write("%s,%s,%d,%.4f,%.4f\n" % (name, scene, n, ti * 1000, td * 1000))
    print("运行时间表已保存：%s" % path)


def plot_table(rows):
    """每个场景画一张时间-规模折线图，分插入/删除两个子图。"""
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("未安装 matplotlib，跳过画图")
        return
    plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei", "DejaVu Sans"]
    plt.rcParams["axes.unicode_minus"] = False
    names = sorted({r[0] for r in rows})
    for idx, (scene, _, _) in enumerate(SCENARIOS):
        scene_rows = [r for r in rows if r[1] == scene]
        if not scene_rows:
            continue
        fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))
        for ax, col, title in ((axes[0], 3, "插入"), (axes[1], 4, "删除")):
            for name in names:
                pairs = sorted((r[2], r[col] * 1000) for r in scene_rows if r[0] == name)
                ax.plot([p[0] for p in pairs], [p[1] for p in pairs], marker="o", label=name)
            ax.set_xlabel("输入规模 N")
            ax.set_ylabel("运行时间 (ms)")
            ax.set_title(scene + " - " + title)
            ax.grid(True, alpha=0.3)
            ax.legend()
        fig.tight_layout()
        path = os.path.join(OUT_DIR, "plot_%d.png" % (idx + 1))
        fig.savefig(path, dpi=150)
        plt.close(fig)
        print("时间图已保存：%s" % path)


def main():
    os.makedirs(BUILD_DIR, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    rng = random.Random(SEED)

    avail = {}
    for name in IMPLS:
        path, err = compile_impl(name)
        if path is None:
            print("[跳过] %-5s %s" % (name, err))
            continue
        funcs, err = load_impl(name, path)
        if funcs is None:
            print("[跳过] %-5s %s" % (name, err))
            continue
        avail[name] = funcs
        print("[可用] %s" % name)

    if not avail:
        print("没有可运行的实现")
        return 1

    rows = []
    for name, funcs in avail.items():
        for scene, imode, dmode in SCENARIOS:
            for n in SIZES:
                ins, dele = gen_orders(n, imode, dmode, rng)
                ti, td = run_case(funcs, ins, dele)
                rows.append((name, scene, n, ti, td))
                print("%-5s %-9s N=%-6d 插入 %9.3f ms  删除 %9.3f ms"
                      % (name, scene, n, ti * 1000, td * 1000))

    save_table(rows)
    plot_table(rows)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
