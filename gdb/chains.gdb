set breakpoint pending on
set $__bplist_len = 0
set $__bplist = (int*) 0
set $__batch_num = 1

define __resize_array
  if $__bplist_len < $arg0
    set $__bplist_newlen = $arg0 * 2
    printf "Resizing array from %d to %d\n", $__bplist_len, $__bplist_newlen
    set $__bplist_new = (int*) malloc($__bplist_newlen * sizeof(int))
    set $__bplist_index = 0
    while $__bplist_index < $__bplist_len
      set $__bplist_new[$__bplist_index] = $__bplist[$__bplist_index]
      set $__bplist_index = $__bplist_index + 1
    end
    set $__bplist_len = $__bplist_newlen
    
    # Fill with zeros
    while $__bplist_index < $__bplist_len
      set $__bplist_new[$__bplist_index] = 0
      set $__bplist_index = $__bplist_index + 1
    end
    set $__bplist = $__bplist_new
  end
end

define __enable_batch
  if $arg0 < 1
    printf "Invalid batch number: %d\n", $arg0
  end
  if $arg0 > ($__batch_num - 1)
    printf "Batch number doesn't exit yet (%d).\n", $arg0
  end
  if $arg0 < $__batch_num && $arg0 > 0
    printf "Enabling batch: %d\n", $arg0
    set $__bp_index = 0
    while $__bp_index != $__bplist_len
      if $__bplist[$__bp_index] == $arg0
        printf "  Enabling breakpoint: %d\n", $__bp_index
        enable once $__bp_index
      end
      set $__bp_index = $__bp_index + 1
    end
    printf "\n"
  end
end

define __chain_breakpoint
  printf "Chaining single breakpoint %d\n", $arg0

  __resize_array $arg0
  set $__bplist[$arg0] = $__batch_num

  commands $arg0
      set $__next_batch = $__bplist[$_hit_bpnum] + 1
      __enable_batch $__next_batch
  end

  # don't disable the first one
  if $__batch_num > 1
    printf "  Disabling breakpoint:  %d\n", $arg0
    disable $arg0
  end

  set $__batch_num = $__batch_num + 1
  printf "\n"
end

define __rchain_breakpoint
  printf "Chaining breakpoint range %d-%d:\n", $arg0, $arg1
  __resize_array $arg1
  set $__cur = $arg0
  while $__cur < $arg1
    printf "  Breakpoint[%d] = batch num #%d\n", $__cur, $__batch_num
    set $__bplist[$__cur] = $__batch_num

    # don't disable the first one
    if $__batch_num > 1 
      disable $__cur
    end

    commands $__cur
      set $__next_batch = $__bplist[$_hit_bpnum] + 1
      __enable_batch $__next_batch
    end

    set $__cur = $__cur + 1
  end

  printf "\n"
end

define chain_last
  __chain_breakpoint $bpnum
end

define chain
  break $arg0
  chain_last
end

define rchain
  set $__last_bpnum = $bpnum
  rbreak $arg0
  __rchain_breakpoint $__last_bpnum $bpnum

  set $__batch_num = $__batch_num + 1
end

# resets the chain to the current breakpoint
define chain_reset
  set $__batch_num = 1
  set $__bpstart = $bpnum ? $bpnum : 0
  if $__bpstart > 0
    # clear the `commands` of the last breakpoint
    commands $__bpstart
    end
  end
end


chain_reset
