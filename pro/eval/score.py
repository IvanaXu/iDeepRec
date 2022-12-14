import sys
import json
import numpy as np

M = sys.argv[1]
N = 5 # 5 

result = []
for n in range(1, N+1):
    _r = "{}"
    with open(f"/pro/log/run-whl-epoch{n}-{M}.log", "r") as f:
        for i in f:
            i = i.strip("\n")
            if "score" in i:
                _r = i
    result.append(
        json.loads(_r.replace("'", '"'))
    )

result = np.array([
    float(_r.get("score", "864000.0"))
    for _r in result
])

print(f"\nEND result CNT: {len(result)} AVG: {np.mean(result):.4f}, MAX: {np.max(result):.4f}, MIN: {np.min(result):.4f}")

result = [_r for _r in result if _r != 864000.0]
if result:
    print("DEL 864000.0")
    print(f"END result CNT: {len(result)} AVG: {np.mean(result):.4f}, MAX: {np.max(result):.4f}, MIN: {np.min(result):.4f}")
