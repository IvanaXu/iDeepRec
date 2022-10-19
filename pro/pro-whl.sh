
#
sh /pro/run-whl.sh|tee /pro/log/run-whl-epoch1-$1.log
sh /pro/run-whl.sh|tee /pro/log/run-whl-epoch2-$1.log
sh /pro/run-whl.sh|tee /pro/log/run-whl-epoch3-$1.log
sh /pro/run-whl.sh|tee /pro/log/run-whl-epoch4-$1.log
sh /pro/run-whl.sh|tee /pro/log/run-whl-epoch5-$1.log

# 
python /pro/eval/score.py
