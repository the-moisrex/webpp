import gdb

class PrintPinsCommand(gdb.Command):
    def __init__(self):
        super(PrintPinsCommand, self).__init__("print_pins", gdb.COMMAND_USER)

    def byte_to_escaped(self, b):
        c = chr(b)
        if 32 <= b <= 126 and c != '\\':
            return c
        elif c == '\\':
            return '\\\\'
        else:
            return "\\x{:02x}".format(b)

    def bytes_to_ascii(self, byte_seq):
        # convert bytes to ascii, non-printable replaced with '.'
        return "".join(chr(b) if 32 <= b <= 126 else '.' for b in byte_seq)

    def invoke(self, arg, from_tty):
        try:
            starter_pin = gdb.parse_and_eval("starter_pin")
            rep_pin = gdb.parse_and_eval("rep_pin")
            cp2_pin = gdb.parse_and_eval("cp2_pin")
            cp1_pin = gdb.parse_and_eval("cp1_pin")

            starter_pos = int(starter_pin["pos"])
            rep_pos = int(rep_pin["pos"])
            cp2_pos = int(cp2_pin["pos"])
            cp1_pos = int(cp1_pin["pos"])

            # Use length 3 bytes, adjust if you have lengths
            pin_info = [
                ("S", starter_pos, starter_pos + 3, starter_pin),
                ("R", rep_pos, rep_pos + 3, rep_pin),
                ("2", cp2_pos, cp2_pos + 3, cp2_pin),
                ("1", cp1_pos, cp1_pos + 3, cp1_pin),
            ]
        except Exception as e:
            print("Error reading pins:", e)
            return

        buffer_ptr = starter_pin["pos"]
        start_addr = int(buffer_ptr)
        max_length = 20
        inferior = gdb.selected_inferior()

        try:
            buffer_bytes = inferior.read_memory(buffer_ptr, max_length).tobytes()
        except gdb.MemoryError:
            print("Failed to read memory at", buffer_ptr)
            return

        buffer_len = len(buffer_bytes)

        output = ""
        for i in range(buffer_len):
            byte_addr = start_addr + i

            open_brackets = sum(1 for _, start, _, _ in pin_info if start == byte_addr)
            close_brackets = sum(1 for _, _, end, _ in pin_info if end - 1 == byte_addr)

            output += '[' * open_brackets
            output += self.byte_to_escaped(buffer_bytes[i])
            output += ']' * close_brackets

        # Carets and labels stacked vertically for overlapping start pins
        max_stack = max(
            sum(1 for _, start, _, _ in pin_info if start == start_addr + i)
            for i in range(buffer_len)
        ) if buffer_len > 0 else 0
        pointer_lines = [[' '] * buffer_len for _ in range(max_stack)]
        label_lines = [[' '] * buffer_len for _ in range(max_stack)]

        placed_count = [0] * buffer_len

        for label, start, _, pin_val in pin_info:
            offset = start - start_addr
            if 0 <= offset < buffer_len:
                stack_pos = placed_count[offset]
                pointer_lines[stack_pos][offset] = '^'
                label_lines[stack_pos][offset] = label
                placed_count[offset] += 1

        print(output)
        for line in pointer_lines:
            print("".join(line))
        for line in label_lines:
            print("".join(line))

        # Print pins' values and ASCII
        print("\nPins values and ASCII:")
        for label, start, end, pin_val in pin_info:
            start_off = start - start_addr
            end_off = end - start_addr
            # Clamp offsets to valid buffer range
            if start_off < 0 or start_off >= buffer_len:
                ascii_repr = "<out of range>"
            else:
                end_off_clamped = min(end_off, buffer_len)
                ascii_repr = self.bytes_to_ascii(buffer_bytes[start_off:end_off_clamped])
            print(f"{label}: {pin_val}  ASCII: '{ascii_repr}'")

PrintPinsCommand()
