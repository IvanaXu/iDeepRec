import json
import pandas as pd

N = 5
result = []
for n in range(1, N+1):
    _r = "{}"
    with open(f"../log/run-whl-epoch{n}.log", "r") as f:
        for i in f:
            i = i.strip("\n")
            if "score" in i:
                _r = i
    result.append(
        json.loads(_r.replace("'", '"'))
    )
result = [
    float(_r.get("score", "864000.0"))
    for _r in result
]
print(pd.DataFrame(result, columns=["score"]).describe())
