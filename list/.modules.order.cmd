cmd_/home/wc1229/pdlab/list/modules.order := {   echo /home/wc1229/pdlab/list/list.ko; :; } | awk '!x[$$0]++' - > /home/wc1229/pdlab/list/modules.order
