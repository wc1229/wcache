cmd_/home/wc1229/pdlab/buffer/modules.order := {   echo /home/wc1229/pdlab/buffer/buffer.ko; :; } | awk '!x[$$0]++' - > /home/wc1229/pdlab/buffer/modules.order
