# ascii-lines
draw ascii lines with and without antialiasing

A toy line drawing program. The non-antialiasing algorithm uses DDA with
int arithmetic. The antialiasing algorithm uses the Wu idea with minimal
floating point operations.

Use: ./draw_lines.bin <row1> <col1> <row2> <col2>
Or : ./draw_lines.bin -t
