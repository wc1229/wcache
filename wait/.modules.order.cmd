cmd_/home/wc1229/pdlab/wait/modules.order := {   echo /home/wc1229/pdlab/wait/wait.ko; :; } | awk '!x[$$0]++' - > /home/wc1229/pdlab/wait/modules.order
