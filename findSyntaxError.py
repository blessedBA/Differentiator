# language: python
# Запуск: python3 check_brackets.py .
# поиск недостающих скобок онлайн без смс и регистраций (by Copilot)
import sys, os

pairs = {'(':')','{':'}','[':']'}
openers = set(pairs.keys())
closers = set(pairs.values())

def check_file(path):
    stack = []
    with open(path, 'r', errors='ignore') as f:
        for lineno, line in enumerate(f, 1):
            for col, ch in enumerate(line, 1):
                if ch in openers:
                    stack.append((ch, lineno, col))
                elif ch in closers:
                    if not stack:
                        print(f"UNMATCHED closing '{ch}' at {path}:{lineno}:{col}")
                        return False
                    last, lno, lcol = stack.pop()
                    if pairs[last] != ch:
                        print(f"MISMATCH {path}:{lineno}:{col}: expected '{pairs[last]}', got '{ch}' (opened at {path}:{lno}:{lcol})")
                        return False
    if stack:
        ch, lno, lcol = stack[-1]
        print(f"UNMATCHED opening '{ch}' at {path}:{lno}:{lcol}")
        return False
    return True

def walk(paths):
    ok = True
    for base in paths:
        for root, _, files in os.walk(base):
            for name in files:
                if name.endswith(('.cpp','.c','.h','.hpp','.cc','.cxx')):
                    path = os.path.join(root, name)
                    if not check_file(path):
                        ok = False
    return ok

if __name__ == '__main__':
    paths = sys.argv[1:] or ['.']
    good = walk(paths)
    if not good:
        sys.exit(2)
    print("All brackets matched.")
