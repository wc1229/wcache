cmd_/home/wc1229/pdlab/list/Module.symvers := sed 's/ko$$/o/' /home/wc1229/pdlab/list/modules.order | scripts/mod/modpost -m -a   -o /home/wc1229/pdlab/list/Module.symvers -e -i Module.symvers   -T -
