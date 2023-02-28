cmd_/home/wc1229/pdlab/sysfs/kobject/modules.order := {   echo /home/wc1229/pdlab/sysfs/kobject/kobject.ko; :; } | awk '!x[$$0]++' - > /home/wc1229/pdlab/sysfs/kobject/modules.order
