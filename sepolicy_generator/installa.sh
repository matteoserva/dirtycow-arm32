#!/bin/bash
cp sepolicy_clean se2
while read p; do
  if [ ! -z "$p" ];then  
  echo ./sepolicy-inject --load se2 --save se2 "$p"
  ./sepolicy-inject --load se2 --save se2 "$p" | grep -v aggiu | grep -v inser
  fi

done <politiche.txt
