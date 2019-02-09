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
std::mt19937 e2(rd());

std::uniform_real_distribution<> dist(-1, 1);

//---------------------------------------------------------------
#define RND dist(e2)
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

#define MAX_EXP 50
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
};

//---------------------------------------------------------------

fvector::fvector()
{
    vec = new float[32];
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
public:
    expression(int depth);
    void    gen_expression();

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
};

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

    if (depth > 30) {
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
    if (isnan(abs(sum))) sum = 1e9;
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
        output.vec[i] = 1.0+sin(X/2.0) + sin(Y);
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
    
    for (int i1 = 0; i1 < p1->len; i1++) {
        for (int i2 = 0; i2 < p2->len; i2++) {
            if (sts1[i1] == sts2[i2]) {
                int distance = abs(i1 - (p1->len>>1)) + abs(i2 - (p2->len>>1));
                //printf("%d %d, %d\n", i1, i2, distance);
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
    printf("%s\n", s.c_str());
}

//---------------------------------------------------------------


class population {
public:
    expression **vec;
    float      *evals;
    int        count;
    
            population(int p_count);
            ~population();
        
    void    evaluate(eval *e);    
};

//---------------------------------------------------------------

    population::population(int p_count)
{
    count = p_count;
    
    vec = (expression **)malloc(sizeof(expression*) * count);
    evals = (float *)malloc(sizeof(float) * count);
    
    
    for (int i = 0; i < count; i++) {
        printf("x %d\n", i);
        vec[i] = new expression(6);
    }
}


//---------------------------------------------------------------

    population::~population()
{
    for (int i = 0; i < count; i++) {
        delete vec[i];
    }
    free((char*)vec);
    free((char*)evals);
}


//---------------------------------------------------------------


void population::evaluate(eval *e)
{
    for (int i = 0; i < count; i++) {
        vec[i]->evaluate(e);
        float v = e->error();
        evals[i] = v;
    }
}


//---------------------------------------------------------------




//---------------------------------------------------------------

int main()
{
    rx = time(NULL);
    eval ev(5);
    ev.init_sample();
    printf("x1\n");
    population p(1000);
    printf("x2\n");
    return 0;
}
