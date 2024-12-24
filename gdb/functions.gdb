set $bpstart = $bpnum ? $bpnum : 0
set breakpoint pending on

define chain
  break $arg0

  set $distance = $bpnum - $bpstart

  commands $bpnum
    set $nextbp = $_hit_bpnum + 1
    set $chain_num = $_hit_bpnum - $bpstart
    printf "\nWe're at breakpoint number %d of the chain\n", $chain_num
    printf "Enabling breakpoint:  %d\n\n", $nextbp
    enable once $nextbp
  end

  # don't disable the first one
  if $distance > 1
    printf "Distabling:  %d\n", $bpnum
    disable $bpnum
  end
end

