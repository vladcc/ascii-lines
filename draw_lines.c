#include <stdio.h>
#include <string.h>

#define ROWS 23
#define COLS 79
#define EMPTY ' '
static char fbuff[ROWS*COLS] = {0};

typedef struct frame {
    char * buff;
    int rows;
    int cols;
} frame;
static frame static_frame = {.buff = fbuff, .rows = ROWS, .cols = COLS};

typedef struct point {
    int row;
    int col;
} point;

void frame_set_point(frame * frm, point * pt, char ch)
{
    frm->buff[pt->row*frm->cols+pt->col] = ch;
}

char frame_get_point(const frame * frm, const point * pt)
{
    return frm->buff[pt->row*frm->cols+pt->col];
}

void init_frame(frame * frm)
{
    memset(frm->buff, EMPTY, frm->rows*frm->cols);
}

void print_frame(frame * frm)
{
    char * buff = frm->buff;
    int rows = frm->rows;
    int cols = frm->cols;
    
    for (int i = -2; i < cols; ++i)
        putchar('-');
    putchar('\n');
        
    int the_row;
    for (int i = 0; i < rows; ++i)
    {
        putchar('|');
        the_row = i*cols;
        for (int j = 0; j < cols; ++j)
            putchar(buff[the_row + j]);
        putchar('|');
        putchar('\n');
    }
    
    for (int i = -2; i < cols; ++i)
        putchar('-');
    putchar('\n');
}


#define STRONG_DOT '#'
#define SCALE_UP(val)   ((val) << 10) // * 1024
#define SCALE_DOWN(val) ((val) >> 10) // / 1024
void draw_line_int_dda(frame * frm, point * pt_a, point * pt_b)
{    
    frame_set_point(frm, pt_a, STRONG_DOT);
    
    int drow = pt_b->row - pt_a->row;
    int dcol = pt_b->col - pt_a->col;
    
    int abs_drow = drow * ((drow > 0) - (drow < 0));
    int abs_dcol = dcol * ((dcol > 0) - (dcol < 0));
    
    int steps = (abs_drow > abs_dcol) ? abs_drow : abs_dcol;
    
    if (steps)
    {
        int inc_drow = SCALE_UP(drow)/steps;
        int inc_dcol = SCALE_UP(dcol)/steps;
      
        int start_row = pt_a->row;
        int start_col = pt_a->col;
   
        point next_pt;
        for (int i = 1; i < steps; ++i)
        {
            next_pt.row = start_row + SCALE_DOWN(inc_drow*i);
            next_pt.col = start_col + SCALE_DOWN(inc_dcol*i);
            frame_set_point(frm, &next_pt, STRONG_DOT);
        }
        frame_set_point(frm, pt_b, STRONG_DOT);
    }   
}

#define WEAK_DOT ':'
static void anti_alias(frame * frm, const point * pt, const point * directions)
{
#define in_range(row, col)\
((unsigned)(row) < rows && (unsigned)(col) < cols)

    int rows = frm->rows;
    int cols = frm->cols;
    int dir_row = directions->row;
    int dir_col = directions->col;
    
    point aal = *pt;
    
    aal.row += dir_row;
    aal.col += dir_col;
    if (in_range(aal.row, aal.col)
        && EMPTY == frame_get_point(frm, &aal))
       frame_set_point(frm, &aal, WEAK_DOT);
    
    aal.row -= (dir_row << 1); // * 2
    aal.col -= (dir_col << 1); // * 2
    if (in_range(aal.row, aal.col)
        && EMPTY == frame_get_point(frm, &aal))
       frame_set_point(frm, &aal, WEAK_DOT);
       
#undef in_range
}

void draw_line_int_dda_aa(frame * frm, point * pt_a, point * pt_b)
{    
    frame_set_point(frm, pt_a, STRONG_DOT); 
        
    int drow = pt_b->row - pt_a->row;
    int dcol = pt_b->col - pt_a->col;
    
    int drow_sign = ((drow > 0) - (drow < 0));
    int dcol_sign = ((dcol > 0) - (dcol < 0));
    point directions = {.row = drow_sign, .col = dcol_sign};
    
    int abs_drow = drow * drow_sign;
    int abs_dcol = dcol * dcol_sign;
    
    int steps = (abs_drow > abs_dcol) ? abs_drow : abs_dcol;
    
    if (steps)
    {
        int inc_drow = SCALE_UP(drow)/steps;
        int inc_dcol = SCALE_UP(dcol)/steps;
      
        int start_row = pt_a->row;
        int start_col = pt_a->col;
        
        point next_pt;
        for (int i = 1; i < steps; ++i)
        {
            next_pt.row = start_row + SCALE_DOWN(inc_drow*i);
            next_pt.col = start_col + SCALE_DOWN(inc_dcol*i);
            frame_set_point(frm, &next_pt, STRONG_DOT);
            anti_alias(frm, &next_pt, &directions);
        }
        frame_set_point(frm, pt_b, STRONG_DOT); 
    }
}

typedef void (draw_line)(frame *, point *, point *);
void draw_start_center(frame * frm, int divide, draw_line fdraw)
{
    int rows = frm->rows;
    int cols = frm->cols;
    
    point start_ = {.row = 0, .col = 0}, * start = &start_;
    point center_ = {.row = rows/2, .col = cols/2}, * center = &center_;
    
    int repeat = divide;
    int inccol = cols/repeat;
    int incrow = rows/repeat;
    
    init_frame(frm);
    
    for (int i = 0; i < repeat; ++i)
    {
        fdraw(frm, start, center);
        start_.col += inccol;
        
        if (start_.col >= cols)
            start_.col = cols-1;
    }
    
    for (int i = 0; i < repeat; ++i)
    {
        fdraw(frm, start, center);
        start_.row += incrow;
        
        if (start_.row >= rows)
            start_.row = rows-1;
    }

    for (int i = 0; i < repeat; ++i)
    {
        fdraw(frm, start, center);
        start_.col -= inccol;
        
        if (start_.col < 0)
            start_.col = 0;
    }

    for (int i = 0; i < repeat; ++i)
    {
        fdraw(frm, start, center);
        start_.row -= incrow;
        
        if (start_.row < 0)
            start_.row = 0;
    } 
}

/*
void draw_center_start(frame * frm, int divide)
{
    int rows = frm->rows;
    int cols = frm->cols;
    
    point start_ = {.row = 0, .col = 0}, * start = &start_;
    point center_ = {.row = rows/2, .col = cols/2}, * center = &center_;
    
    int repeat = divide;
    int inccol = cols/repeat;
    int incrow = rows/repeat;
    
    init_frame(frm);

    for (int i = 0; i < repeat; ++i)
    {
        draw_line_int_dda(frm, center, start);
        start_.col += inccol;
        
        if (start_.col >= cols)
            start_.col = cols-1;
    }
    
    for (int i = 0; i < repeat; ++i)
    {
        draw_line_int_dda(frm, center, start);
        start_.row += incrow;
        
        if (start_.row >= rows)
            start_.row = rows-1;
    }

    for (int i = 0; i < repeat; ++i)
    {
        draw_line_int_dda(frm, center, start);
        start_.col -= inccol;
        
        if (start_.col < 0)
            start_.col = 0;
    }

    for (int i = 0; i < repeat; ++i)
    {
        draw_line_int_dda(frm, center, start);
        start_.row -= incrow;
        
        if (start_.row < 0)
            start_.row = 0;
    } 
}
*/

void draw_slopes(frame * frm, draw_line fdraw)
{
    point start = {.row = 0, .col = 0};
    
    int rows = frm->rows;
    int cols = frm->cols;
    int first = 0;
    
    point dest = {.row = 22, .col = first};
    for (int i = first; i < cols; i += 15)
    {
        fdraw(frm, &start, &dest);
        dest.col = i;
    }
    
    dest.col = cols-1;
    dest.row = 0;
    for (int i = first; i < rows; i += 7)
    {
        fdraw(frm, &start, &dest);
        dest.row = i;
    }
}

void test_draw(frame * frm, int divide)
{
    init_frame(frm);
    draw_start_center(frm, divide, draw_line_int_dda);
    print_frame(frm);
    
    init_frame(frm);
    draw_start_center(frm, divide, draw_line_int_dda_aa);
    print_frame(frm);
    
    init_frame(frm);
    draw_slopes(frm, draw_line_int_dda);
    print_frame(frm);
    
    init_frame(frm);
    draw_slopes(frm, draw_line_int_dda_aa);
    print_frame(frm);
}

int main(int argc, char * argv[])
{

    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
    {
        test_draw(&static_frame, 2);
        return 0;
    }
    
    if (argc != 5)
    {
        printf("Use: %s <row1> <col1> <row2> <col2>\n", *argv);
        printf("Or : %s -t\n", *argv);
        return -1;
    }
    
    point pta = {.row = -1, .col = -1}, ptb = {.row = -1, .col = -1};
    sscanf(argv[1], "%d", &pta.row);
    sscanf(argv[2], "%d", &pta.col);
    sscanf(argv[3], "%d", &ptb.row);
    sscanf(argv[4], "%d", &ptb.col);
    
    if (((unsigned)pta.row >= ROWS || (unsigned)ptb.row >= ROWS)
        || ((unsigned)pta.col >= COLS || (unsigned)ptb.col >= COLS))
    {
        printf("error: row 0-%d, col 0-%d\n", ROWS-1, COLS-1);
        return -1;
    }
    
    printf("frame r%d c%d\n", ROWS, COLS);
    
    frame * the_frame = &static_frame;
    init_frame(the_frame);

    draw_line_int_dda(the_frame, &pta, &ptb);
    
    print_frame(the_frame);
    return 0;
}
