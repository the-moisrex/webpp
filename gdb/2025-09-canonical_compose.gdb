# canonical_compose.gdb
define show_hole_buffer
  # Usage: show_hole_buffer <hole_var> <reducer_var>

  set $hole = $arg0
  set $reducer = $arg1

  # hole pointers
  set $beginp = $hole.beginp
  set $endp = $hole.endp

  # buffer pointers
  set $buf_start = $reducer.beg
  set $buf_end   = $reducer.endptr

  # offset + size
  set $offset = $beginp - $buf_start
  set $size   = $endp - $beginp

  printf "Buffer size: %d units\n", $buf_end - $buf_start
  printf "Hole: offset=%d size=%d\n\n", $offset, $size

  set $ptr = $buf_start
  printf "Buffer with hole:\n"

  while $ptr < $buf_end
    if $ptr == $beginp
      printf "["
    end

    set $val = *(unsigned char*)$ptr
    if $val >= 32 && $val <= 126
      printf "%c", $val
    else
      printf "."
    end

    set $ptr = $ptr + 1

    if $ptr == $endp
      printf "]"
    end
  end

  printf "\n"
end
