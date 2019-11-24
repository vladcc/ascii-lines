# ascii-lines
draw ascii lines with, or without, really crude "antialiasing"

A toy line drawing program. Uses DDA with only int arithmetic and no hotpath
checks in the non-antialiasing version.

Use: ./draw_lines.bin <row1> <col1> <row2> <col2>
Or : ./draw_lines.bin -t

Code is messy, but the fun is at draw_line_int_dda() and draw_line_int_dda_aa()
