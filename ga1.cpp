#include <vector>
#include <stack> 
#include <math.h>
#include <stdio.h>
#include <random>
#include <time.h>
#include <string>

using namespace std;

#define MAX_INPUT 1

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

	int	sample_count;
		eval(int cnt);

	void	add(int v1, int v2, int result);
	void	sub(int v1, int v2, int result);
	void	mul(int v1, int v2, int result);
	void	div(int v1, int v2, int result);
	void	exp(int v1, int result);
	void 	vsqrt(int v1, int result);
	void 	vsqr(int v1, int result);
	void 	vsin(int v1, int result);
        void    expo(int v1, int result);
	void	cst(float v, int result);
        void    var(int idx, int result);


	float	error(int idx);

	void 	init_sample();
	fvector	inputs[MAX_INPUT];
	fvector	output;
        fvector  stack[32];
        int     sp;                             //stack pointer
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
    
    return("?????");
}

//---------------------------------------------------------------


#define VAR_COUNT 1

//---------------------------------------------------------------

class expression {
public:
    node    data[MAX_EXP];
    int     len; 
public:
            expression(int depth);
    void    build(int depth);
    node    get_rand_terminal_node();
    node    get_rand_arity0_node();
    node    get_rand_arity1_node();
    node    get_rand_arity2_node();
    
    void    get_arity_0(node n, eval *e, int result);
    void    get_arity_1(node n, eval *e, int result);
    void    get_arity_2(node n, eval *e, int v1, int v2, int result);

    
    void    dump();
    int     run(int idx);
    int     val(eval *e, int idx);


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
    
    this->build(depth);
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
}


//---------------------------------------------------------------

int expression::val(eval *e, int idx)
{
    node    current;

    while(idx >= 0) {    
        current = data[idx];
    
        idx--;
    
        if (current.arity == 0) {
                get_arity_0(current, e, e->sp);
                e->sp++;
        }
    
        if (current.arity == 1) {
                get_arity_1(current, e, e->sp);
        }
    
        if (current.arity == 2) {
                get_arity_2(current, e, e->sp, e->sp-1, e->sp-1);
                e->sp--;
        }
    }
}

//---------------------------------------------------------------

void expression::build(int depth)
{
    if (depth == 0) {           //terminal nodes
        node tmp = get_rand_arity0_node();
        data[len] = tmp;
        len++;
        return;
    }
    
    int arity = brand()%3;
    
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

//---------------------------------------------------------------

   eval::eval(int cnt)
{
	sample_count = cnt;
        sp = 0;
}

//---------------------------------------------------------------

float eval::error(int v1)
{
	float sum;
	
	sum = 0;
	for (int i = 0; i < sample_count; i++) {
		float e = (output.vec[i] - stack[v1].vec[i]);
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

        r = stack[result].vec;

	for (int i = 0; i < sample_count; i++) {
		*r++ = v;
	}
}

//---------------------------------------------------------------

void eval::var(int idx, int result)
{
        float *r;

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

        p1 = stack[v1].vec;
        r = stack[result].vec;

	for (int i = 0; i < sample_count; i++) {
		*r++ = sin(*p1++);
	}
}

//---------------------------------------------------------------

void eval::init_sample()
{
	int	i_count;


	for (i_count = 0; i_count < MAX_INPUT; i_count++) {
		for (int i = 0; i < sample_count; i++) {
			inputs[i_count].vec[i] = 10.0*RND;
		}
	}

	for (int i = 0; i < sample_count; i++) {
		output.vec[i] = sin(0.31415926*inputs[0].vec[i]);
	}
}


//---------------------------------------------------------------

int main()
{
    rx = time(NULL);

    expression e(5);
    e.dump();
    e.run(0);
    
    return 0;
}
