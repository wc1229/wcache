cmd_/home/wc1229/pdlab/buffer/Module.symvers := sed 's/ko$$/o/' /home/wc1229/pdlab/buffer/modules.order | scripts/mod/modpost -m -a   -o /home/wc1229/pdlab/buffer/Module.symvers -e -i Module.symvers   -T -