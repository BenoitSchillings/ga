#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <random>
#include <time.h>
#include <string>

using namespace std;

#define EMAX_INPUT 15

//---------------------------------------------------------------

std::random_device rd;
//std::mt19937 e2(rd());
std::mt19937 e2(10);

std::uniform_real_distribution<> dist(-3, 3);
std::uniform_real_distribution<> dmult(0.95, 1.05);
std::uniform_real_distribution<> d02(0.05, 0.9);
std::uniform_real_distribution<> dsmall(-0.02, 0.02);

//---------------------------------------------------------------
#define RND dist(e2)
#define RNDM dmult(e2)
#define CMD d02(e2)
#define NOISE dsmall(e2)
//---------------------------------------------------------------
static unsigned long rx=123456789, ry=362436069, rz=521288629;
//---------------------------------------------------------------

unsigned long brand(void) {          //period 2^96-1
    unsigned long t;
    rx ^= rx << 16;
    rx ^= rx >> 5;
    rx ^= rx << 1;
    
    t = rx;
    rx = ry;
    ry = rz;
    rz = t ^ rx ^ ry;
    
    return rz;
}

//---------------------------------------------------------------

#define MAX_EXP 100
#define uchar unsigned char

//---------------------------------------------------------------

typedef struct  {
    uchar   op;
    uchar   arity;
    short   var;
    float   val;
} node;

//---------------------------------------------------------------

class fvector {
public:
    float   *vec;
    fvector();
    ~fvector();
};

//---------------------------------------------------------------

fvector::fvector()
{
    vec = new float[32];
}

//---------------------------------------------------------------

fvector::~fvector()
{
    delete vec;
}

//---------------------------------------------------------------

class eval {
public:
    
    int    sample_count;
    eval(int cnt);
    
    void    add(int v1, int v2, int result);
    void    sub(int v1, int v2, int result);
    void    mul(int v1, int v2, int result);
    void    div(int v1, int v2, int result);
    void    exp(int v1, int result);
    void    vsqrt(int v1, int result);
    void    vsqr(int v1, int result);
    void    vsin(int v1, int result);
    void    expo(int v1, int result);
    void    cst(float v, int result);
    void    var(int idx, int result);
    void    print_stack(int idx);
    
    
    float    error();
    
    void     init_sample();
    fvector    inputs[EMAX_INPUT];
    fvector    output;
    fvector  stack[32];
    int     sp;                             //stack pointer
    int     trace;
private:;
};

//---------------------------------------------------------------

#define VAR     1
#define CONST   2


#define PLUS    10
#define MINUS   11
#define MULT    12
#define DIV     13

#define SIN     100
#define SQ      101
#define SQRT    102
#define EXP     103
#define NOP     104

//---------------------------------------------------------------

string to_string(node n)
{
    if (n.op == VAR) {
        char c = 'x' + n.var;
        
        return(string(1, c));
    }
    
    if (n.op == CONST) {
        return(to_string(n.val));
    }
    
    if (n.op == PLUS) {
        return("+");
    }
    if (n.op == MINUS) {
        return("-");
    }
    if (n.op == MULT) {
        return("*");
    }
    if (n.op == DIV) {
        return("/");
    }
    
    if (n.op == SIN) {
        return("sin");
    }
    if (n.op == SQ) {
        return("sq");
    }
    if (n.op == SQRT) {
        return("sqrt");
    }
     if (n.op == NOP) {
        return("nop");
    }
   if (n.op == EXP) {
        return("exp");
    }
    printf("n.op = %d, n.arity = %d @ %d\n", n.op, n.arity);
    
    return("?????");
}

//---------------------------------------------------------------

void eval::print_stack(int idx)
{
    int     i;
    
    for (i = 0; i < sample_count; i++) {
        printf("%3.3f ", stack[idx].vec[i]);
    }
    printf("\n");
}

//---------------------------------------------------------------

#define VAR_COUNT 2

//---------------------------------------------------------------

class expression {
public:
    node    data[MAX_EXP];
    int     len;
    int     trace;
    int     max_depth;
    float   error;
public:
    expression(int depth);
    void    gen_expression();
    void    copy(expression *e);
private:
    void    build(int depth);
    node    get_rand_terminal_node();
    node    get_rand_arity0_node();
    node    get_rand_arity1_node();
    node    get_rand_arity2_node();
    
    void    get_arity_0(node n, eval *e, int result);
    void    get_arity_1(node n, eval *e, int result);
    void    get_arity_2(node n, eval *e, int v1, int v2, int result);
public:
    
    string  to_algebraic(short *idx);
    
    void    dump();
    int     run(int idx);
    int     val(eval *e, int idx);
    void    evaluate(eval *e);
    void    cross(expression *p1);
    void    mutate();
};

//---------------------------------------------------------------

void expression::copy(expression *e)
{
    int     i;
    
    for (int i = 0; i < len; i++) {
        e->data[i] = data[i];
    }
    e->len = len;
    e->max_depth = max_depth;
    e->error = error;
    e->trace = trace;
}

//---------------------------------------------------------------

void expression::dump()
{
    int     i;
    
    for (i = 0; i < len; i++) {
        printf("%d - %s\n", i, to_string(data[i]).c_str());
    }
}

//---------------------------------------------------------------

expression::expression(int depth)
{
    len = 0;
    
    trace = 0;
    max_depth = depth;
    this->gen_expression();
    error = 1e9;
}

//---------------------------------------------------------------

void expression::mutate()
{
    int     i;
    
    for (i = 0; i < len; i++) {
        if (data[i].op == CONST) {
            data[i].val *= RNDM;
            //data[i].val += RNDX;
        }
    }
}

//---------------------------------------------------------------

void expression::gen_expression()
{
    len = 0;
    this->build(max_depth);
}

//---------------------------------------------------------------

node expression::get_rand_arity0_node()
{
    node    tmp;
    
    
    int randv = brand() % 2;
    
    if (randv == 0) {
        tmp.op = VAR;
        tmp.var = brand() % VAR_COUNT;
        
        tmp.val = 0;
        tmp.arity = 0;
    }
    
    if (randv == 1) {
        tmp.op = CONST;
        tmp.var = 0;
        
        int rand1 = brand() % 32768;
        
        if (rand1 > 31000)
            tmp.val = 3.1415926535897932384626433;
        else if (rand1 < 30000)
            tmp.val = 1.0;
        else if (rand1 < 29000)
            tmp.val = 2.0;
        else if (rand1 < 28000)
            tmp.val = 2.718281828;
        else
        
            tmp.val = RND;
        
        tmp.arity = 0;
    }
    
    return tmp;
}

//---------------------------------------------------------------

node expression::get_rand_arity1_node()
{
    node    tmp;
    
    
    int randv = brand() % 4;
    
    tmp.op = 100 + randv;
    tmp.val = 0;
    tmp.arity = 1;
    return tmp;
}

//---------------------------------------------------------------

node expression::get_rand_arity2_node()
{
    node    tmp;
    
    
    int randv = brand() % 4;
    
    tmp.op = 10 + randv;
    tmp.val = 0;
    tmp.arity = 2;
    return tmp;
}

//---------------------------------------------------------------

string expression::to_algebraic(short *idx)
{
    node    current;
    
    current = data[*idx];
    
    (*idx)++;
    
    if (current.arity == 0) {
        return(to_string(current));
    }
    
    if (current.arity == 2) {
        string op = to_string(current);;
        string left = to_algebraic(idx);
        string right = to_algebraic(idx);
        return("(" + left + op + right + ")");
    }
    
    if (current.arity == 1) {
        string op = to_string(current);
        string left = to_algebraic(idx);
        return(op + "(" + left + ")");
    }
    return("ERROR");
}

//---------------------------------------------------------------

int expression::run(int idx)
{
    node    current;
    
    current = data[idx];
    printf("%d - %d -  %s\n", idx, current.arity, to_string(current).c_str());
    
    idx++;
    
    if (current.arity == 0) {
        printf("term %s\n", to_string(current).c_str());
        return (idx);
    }
    
    if (current.arity == 2) {
        printf("%s\n", to_string(current).c_str());
        int left = run(idx);
        int right = run(left);
        return(right);
    }
    
    if (current.arity == 1) {
        printf("%s\n", to_string(current).c_str());
        int left = run(idx);
        return(left);
    }
    printf("?????\n");
    printf("current.op = %d, current.arity = %d @ %d\n", current.op, current.arity, idx);
    return(-1);
}

//---------------------------------------------------------------

void expression::get_arity_0(node n, eval *e, int result)
{
    if (n.op == VAR) {
        e->var(n.var, result);
        return;
    }
    
    if (n.op == CONST) {
        e->cst(n.val, result);
        return;
    }
}

//---------------------------------------------------------------

void expression::get_arity_1(node n, eval *e, int result)
{
    if (n.op == SIN) {
        e->vsin(result, result);
        return;
    }
    
    if (n.op == SQRT) {
        e->vsqrt(result, result);
        return;
    }
    
    if (n.op == SQ) {
        e->vsqr(result, result);
        return;
    }
    
    if (n.op == EXP) {
        e->expo(result, result);
        return;
    }
    
    if (n.op == NOP) {
        return;
    }
    
}

//---------------------------------------------------------------

void expression::get_arity_2(node n, eval *e, int v1, int v2, int result)
{
    if (n.op == PLUS) {
        e->add(v1, v2, result);
        return;
    }
    
    if (n.op == MINUS) {
        e->sub(v1, v2, result);
        return;
    }
    
    if (n.op == MULT) {
        e->mul(v1, v2, result);
        return;
    }
    if (n.op == DIV) {
        e->div(v1, v2, result);
        return;
    }
    
}


//---------------------------------------------------------------

int expression::val(eval *e, int idx)
{
    node    current;
    
    while(idx >= 0) {
        if (e->sp>31) {
            printf("STACK OVERFLOW\n");
            exit(-1);
        }
        current = data[idx];
        
        idx--;
        
        if (current.arity == 0) {
            get_arity_0(current, e, e->sp);
            e->sp++;
        }
        
        if (current.arity == 1) {
            get_arity_1(current, e, e->sp - 1);
        }
        
        if (current.arity == 2) {
            get_arity_2(current, e, e->sp - 1, e->sp - 2, e->sp - 2);
            e->sp--;
        }
    }
    if (trace) printf("final sp is %d %d\n", e->sp, trace);
    return 0;
}

//---------------------------------------------------------------

void expression::evaluate(eval *e)
{
    e->sp = 0;
    
    val(e, len - 1);
}

//---------------------------------------------------------------

void expression::build(int depth)
{
    
    if (len >= MAX_EXP) {
        printf("LARGE EXP ??\n");
        exit(-1);
    }
    
    
    if (depth == 0) {           //terminal nodes
        node tmp = get_rand_arity0_node();
        data[len] = tmp;
        len++;
        return;
    }
    
    int arity = brand()%3;
    
    
    // kind of a hack to make expression at least 2 level deep
    
    
    while (arity == 0 && len < 3) {
        arity = brand()%3;
    }
    
    if (arity == 0) {
        node tmp = get_rand_arity0_node();
        data[len] = tmp;
        len++;
        return;
    }
    
    if (arity == 1) {
        node tmp = get_rand_arity1_node();
        data[len] = tmp;
        len++;
        build(depth - 1);
        return;
    }
    
    if (arity == 2) {
        node tmp = get_rand_arity2_node();
        data[len] = tmp;
        len++;
        build(depth - 1);
        build(depth - 1);
        return;
    }
    
    
    printf("ERROR in BUILD\n");
}

//---------------------------------------------------------------

eval::eval(int cnt)
{
    sample_count = cnt;
    sp = 0;
    trace = 0;
}

//---------------------------------------------------------------

float eval::error()
{
    float sum;
    
    sum = 0;
    for (int i = 0; i < sample_count; i++) {
        float e = (output.vec[i] - stack[0].vec[i]);
        e = e * e;
        sum += e;
    }
    sum = sum / (1.0*sample_count);
    sum = sqrt(sum);
    return sum;
}

//---------------------------------------------------------------


void eval::add(int v1, int v2, int result)
{
    float *p1;
    float *p2;
    float *r;
    
    p1 = stack[v1].vec;
    p2 = stack[v2].vec;
    r = stack[result].vec;
    
    if (trace) printf("add %d %d %d\n", v1, v2, result);
    for (int i = 0; i < sample_count; i++) {
        *r++ = *p1++ + *p2++;
    }
}

//---------------------------------------------------------------

void eval::sub(int v1, int v2, int result)
{
    float *p1;
    float *p2;
    float *r;
    
    p1 = stack[v1].vec;
    p2 = stack[v2].vec;
    r = stack[result].vec;
    if (trace) printf("sub %d %d %d\n", v1, v2, result);
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = *p1++ - *p2++;
    }
}

//---------------------------------------------------------------

void eval::mul(int v1, int v2, int result)
{
    float *p1;
    float *p2;
    float *r;
    
    p1 = stack[v1].vec;
    p2 = stack[v2].vec;
    r = stack[result].vec;
    if (trace) printf("mul %d %d %d\n", v1, v2, result);
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = *p1++ * *p2++;
    }
}



//---------------------------------------------------------------


void eval::div(int v1, int v2, int result)
{
    float *p1;
    float *p2;
    float *r;
    
    if (trace) printf("div %d %d %d\n", v1, v2, result);
    
    p1 = stack[v1].vec;
    p2 = stack[v2].vec;
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = *p1++ / *p2++;
    }
}



//---------------------------------------------------------------

void eval::expo(int v1, int result)
{
    float *p1;
    float *r;
    
    if (trace) printf("exp %d %d\n", v1, result);
    
    p1 = stack[v1].vec;
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = pow(2.71828182, *p1++);
    }
}

//---------------------------------------------------------------

void eval::cst(float v, int result)
{
    float *r;
    
    if (trace) printf("cst %f -> %d\n", v, result);
    
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = v;
    }
}

//---------------------------------------------------------------

void eval::var(int idx, int result)
{
    float *r;
    
    if (trace) printf("var %d\n", result);
    
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = inputs[idx].vec[i];
    }
}

//---------------------------------------------------------------

void eval::vsqr(int v1, int result)
{
    float *p1;
    float *r;
    
    if (trace) printf("vsqr %d %d\n", v1, result);
    
    p1 = stack[v1].vec;
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = *p1 * *p1;
        p1++;
    }
}

//---------------------------------------------------------------

void eval::vsqrt(int v1, int result)
{
    float *p1;
    float *r;
    
    if (trace) printf("vsqrt %d %d\n", v1, result);
    
    p1 = stack[v1].vec;
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = sqrt(abs(*p1++));
    }
}

//---------------------------------------------------------------

void eval::vsin(int v1, int result)
{
    float *p1;
    float *r;
    
    if (trace) printf("sin %d %d\n", v1, result);
    
    p1 = stack[v1].vec;
    r = stack[result].vec;
    
    for (int i = 0; i < sample_count; i++) {
        *r++ = sin(*p1++);
    }
}

//---------------------------------------------------------------

void eval::init_sample()
{
    int    i_count;
    
    
    for (i_count = 0; i_count < EMAX_INPUT; i_count++) {
        for (int i = 0; i < sample_count; i++) {
            inputs[i_count].vec[i] = 10.0*RND;
        }
    }
    
#define X inputs[0].vec[i]
#define Y inputs[1].vec[i]
    
    for (int i = 0; i < sample_count; i++) {
        output.vec[i] = NOISE + 1.0+sin(X/2.0) + sin(Y-X) + sin(X+Y);
    }
}


//---------------------------------------------------------------
// genetics


void cross(expression *p1, expression *p2)
{
    int     idx1;
    
    int     st = 0;
    
    node    n1;
    
    int     sts1[MAX_EXP];
    int     sts2[MAX_EXP];
    
    idx1 = p1->len - 1;
    
    while(idx1 >= 0) {
        n1 = p1->data[idx1];
        
        if (n1.arity == 0) {
            st += 1;
        }
        
        if (n1.arity == 1) {
            st = st;
        }
        
        if (n1.arity == 2) {
            st -= 1;
        }
        sts1[idx1] = st;
        //printf("stack1 %d %d %d\n", idx1, st, n1.arity);
        idx1--;
    }
    
    idx1 = p2->len - 1;
    st = 0;
    
    
    while(idx1 >= 0) {
        n1 = p2->data[idx1];
        
        if (n1.arity == 0) {
            st += 1;
        }
        
        if (n1.arity == 1) {
            st = st;
        }
        
        if (n1.arity == 2) {
            st -= 1;
        }
        sts2[idx1] = st;
        //printf("stack2 %d %d %d\n", idx1, st, n1.arity);
        idx1--;
    }
    
    
    int best_distance = 32768;
    int best_i1 = -1;
    int best_i2 = -1;
    
    
    for (int trys = 0; trys < 200; trys++) {
        int cross1 = brand()%p1->len;
        int cross2 = brand()%p2->len;
        
        if (sts1[cross1] == sts2[cross2]) {
            best_i1 = cross1;
            best_i2 = cross2;
            goto out;
        }
    }
    
    out:;
    
    if (best_i1 < 0)
    for (int i1 = 0; i1 < p1->len; i1++) {
        for (int i2 = 0; i2 < p2->len; i2++) {
            if (sts1[i1] == sts2[i2]) {
                //int distance = abs(i1 - (p1->len>>1)) + abs(i2 - (p2->len>>1));
                //printf("%d %d, %d\n", i1, i2, distance);
                
                int distance = brand() % 100;
                
                if (distance < best_distance) {
                    best_distance = distance;
                    best_i1 = i1;
                    best_i2 = i2;
                }
            }
        }
    }
    //printf("len %d len %d\n", p1->len, p2->len);
    //printf("best %d %d\n", best_i1, best_i2);
    if (best_i1 != -1 && best_i2 != -1) {
        p2->len = best_i2;
        
        while(best_i1 < p1->len) {
            //printf("copy %d %d\n", best_i1, best_i2);
            p2->data[best_i2++] = p1->data[best_i1++];
            p2->len++;
        }
    }
}


//---------------------------------------------------------------

void print(expression *p)
{
    short pos = 0;
    
    string s = p->to_algebraic(&pos);
    printf("%4.4f %s\n", p->error, s.c_str());
}

//---------------------------------------------------------------
#define PSIZE 1200


class population {
public:
    expression **vec;
    int        count;
    
    population();
    ~population();
    
    void    evaluate(eval *e);
    void    sort_population();
    void    crossing(float elite);
    void    display_population();
    void    new_gen(float ratio);
    void    mutate(float ratio);
    void    mutate_top(int n);
    void    kill_twins();
    void    extinction(int n);
private:
    void    shellsort(expression **p, int num);
};

//---------------------------------------------------------------

void population::crossing(float elite)
{
    int         e_border = count * elite;
    expression  tmp(0);
    
    for (int idx = e_border+1; idx < (count); idx++) {
        int     parent1 = brand() % (e_border);
        int     parent2 = brand() % (e_border);
        vec[parent2]->copy(&tmp);
        cross(vec[parent1], &tmp);
        
        if (tmp.len < 23)
            tmp.copy(vec[idx]);
        else {
            if (brand() % 3 == 0)
                vec[parent1]->copy(vec[idx]);
            vec[idx]->mutate();
        }
    }
}

//---------------------------------------------------------------

void    population::extinction(int n)
{
    for (int i = n; i < count; i++)
        vec[i]->gen_expression();
}

//---------------------------------------------------------------

void population::mutate(float ratio)
{
    int     new_count = count * ratio;
    
    for (int idx = 0; idx < new_count; idx++) {
        vec[1 + (brand()%(count-1))]->mutate();
    }
}

//---------------------------------------------------------------

void population::mutate_top(int n)
{
    for (int idx = 0; idx < n; idx++) {
        int target = 1+brand() % (count-1);
        
        vec[0]->copy(vec[target]);
        vec[target]->mutate();
    }
}

//---------------------------------------------------------------

void population::new_gen(float ratio)
{
    int     new_count = count * ratio;
    
    for (int idx = 1; idx < new_count; idx++) {
        vec[count - idx]->gen_expression();
    }
}


//---------------------------------------------------------------

population::population()
{
    count = PSIZE;
    
    vec = (expression **)malloc(sizeof(expression*) * count);
    
    
    for (int i = 0; i < count; i++) {
        vec[i] = new expression(5);
    }
}


//---------------------------------------------------------------

population::~population()
{
    for (int i = 0; i < count; i++) {
        delete vec[i];
    }
    free((char*)vec);
}


//---------------------------------------------------------------

void population::kill_twins()
{
    for (int i = 1; i < count; i++) {
        float e0 = vec[i]->error;
        float e1 = vec[i-1]->error;
        
        if (fabs(e0 - e1) < 0.001) {
            vec[i]->gen_expression();
        }
    }
}

//---------------------------------------------------------------


void population::evaluate(eval *e)
{
    for (int i = 0; i < count; i++) {
        vec[i]->evaluate(e);
        float v = e->error();
        //v = i/100.0;
        if (!isnormal(v))
            v = 1e10;
        
        //printf("%4.4f\n", v);
        vec[i]->error = v * (100.0 + vec[i]->len)/100.0;
    }
}


//---------------------------------------------------------------

int cmp(const void* p1, const void* p2)
{
    expression *i1, *i2;
    
    i1 = *((expression**)p1);
    i2 = *((expression**)p2);
    
    float v1 = i1->error;
    float v2 = i2->error;
    
    return(v1 < v2);
}


//---------------------------------------------------------------

void population::shellsort(expression **p, int num)
{
    int i, j, k;
    expression *tmp;
    
    for (i = num / 2; i > 0; i = i / 2)
    {
        for (j = i; j < num; j++)
        {
            for(k = j - i; k >= 0; k = k - i)
            {
                if (p[k+i]->error >= p[k]->error)
                    break;
                else
                {
                    tmp = p[k];
                    p[k] = p[k+i];
                    p[k+i] = tmp;
                }
            }
        }
    }
}

//---------------------------------------------------------------

void population::sort_population()
{
    shellsort(vec, count);
}

//---------------------------------------------------------------

void population::display_population()
{
    int i;
    
    for (i = 0; i < 1;i++) {
        print(vec[i]);
    }
    
}

//---------------------------------------------------------------

void gotoxy(int x,int y)
{
        printf("%c[%d;%df",0x1B,y,x);
}

//---------------------------------------------------------------

void cls()
{
    printf("\033[2J\033[1;1H");
}

//---------------------------------------------------------------

void swap_pop(population *p1, population *p2)
{
    int         i;
    
    for (int i = 0; i < 10; i++) {
        int i1 = 1+brand()%(p1->count-1);
        int i2 = 1+brand()%(p2->count-1);
        p1->vec[i1]->copy(p2->vec[i2]);
    }
}

//---------------------------------------------------------------

#define NP  18          //number of populations

float tryx(float newg)
{
    rx = time(NULL);
    //rx = 15;
    eval ev(15);
    ev.init_sample();
    population  pops[NP];
    
    
    for (int i = 0; i < 111111; i++) {
        for (int pn = 0; pn < NP; pn++) {
            pops[pn].evaluate(&ev);
            pops[pn].sort_population();
            pops[pn].kill_twins();
            pops[pn].new_gen(0.1);
            pops[pn].crossing(0.4);    // see https://docs.google.com/spreadsheets/d/1TJWPCB-mIWW9WyfOPP8lo4F2v1aETUWIP_Ils68MWtE/edit#gid=0
            pops[pn].mutate(0.2);
            pops[pn].mutate_top(10);
                
        }
        
        if (i % 10 == 0) {
            cls(); gotoxy(1,1);
            for (int i = 0; i < NP; i++) {
                pops[i].evaluate(&ev);
                pops[i].sort_population();
                pops[i].display_population();
                //printf("\n");
            }
            
        }
        
        int     p1;
        int     p2;
        
        p1 = brand() % NP/2;
        p2 = brand() % NP/2;
        
        swap_pop(&pops[p1], &pops[p2]);
        
        p1 = NP/2 + brand() % NP/2;
        p2 = NP/2 + brand() % NP/2;
        swap_pop(&pops[p1], &pops[p2]);

        if (i % 140 == 0) {
            p1 = brand() % NP/2;
            p2 = brand() % NP/2 + NP/2;
            swap_pop(&pops[p1], &pops[p2]);
            swap_pop(&pops[p2], &pops[p1]);

        }
        
        if (i % 300 == 0) {
            int side = brand()%2;
            
            if (side == 0) {
                for (int k = 0; k < (NP/2); k++)
                    pops[k].extinction(0);
            }
            if (side == 1) {
                for (int k = NP/2; k < NP; k++)
                    pops[k].extinction(0);
            
            }
        }
    }
    return pops[0].vec[0]->error;
}

//---------------------------------------------------------------

int main()
{
    for (int tt = 0; tt < 1; tt++) {
        float cmd = CMD;
        float t = tryx(cmd);
        printf("%f, %f\n", cmd, t);
    }
    return 0;
}
