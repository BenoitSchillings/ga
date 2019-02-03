#include <vector>
#include <stack> 
#include <math.h>
#include <stdio.h>
#include <random>
#include <time.h>
using namespace std;

#define vf vector<float>
#define MAX_INPUT 3

//---------------------------------------------------------------

std::random_device rd;
std::mt19937 e2(rd());

std::uniform_real_distribution<> dist(-1, 1);

//---------------------------------------------------------------
#define RND dist(e2)
//---------------------------------------------------------------

#define	ADD	0
#define	SUB	1
#define	MUL	2
#define EXP	3

//---------------------------------------------------------------

static unsigned long rx=123456789, ry=362436069, rz=521288629;

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

class tree
{
public:;
   tree();
   ~tree();
    void print(int level);
    tree *pick_branch(int level);

    public:;
    char op;
    int  var;
    float val;

    tree* left, *right;
};

//---------------------------------------------------------------

class finder {
public:

	int	sample_count;
		finder(int cnt);
	vf	vec();
	void	add(const vf &v1, const vf &v2, vf &result);
	void	sub(const vf &v1, const vf &v2, vf &result);
	void	mul(const vf &v1, const vf &v2, vf &result);
	void	div(const vf &v1, const vf &v2, vf &result);
	void	expo(const vf &v1, vf &result);
	void 	vsqrt(const vf &v1, vf &result);
	void    vsqr(const vf &v1, vf &result);
	void 	vsin(const vf &v1, vf &result);
	void 	rand_opt(char *c, int *unarity, int *var, float *value);
	void 	rand_opt_terminal(char *c, int *unarity, int *var, float *value);
	tree 	*build_tree(int depth);
	vf 	eval(tree *t);

	void	cst(float v, vf &result);
	void	print(vf &vec);

	void 	init_sample();
	float	error(const vf &v);
	stack<vf> vstack;

	vf	inputs[MAX_INPUT];
	vf	output;
private:;
};

//---------------------------------------------------------------

   finder::finder(int cnt)
{
	sample_count = cnt;

	for (int i = 0; i < MAX_INPUT; i++) {
		inputs[i] = vf(sample_count);
		this->cst(0, inputs[i]);
	}
	output = vf(sample_count);
}

//---------------------------------------------------------------

void finder::init_sample()
{
	int	i_count;


	for (i_count = 0; i_count < MAX_INPUT; i_count++) {
		for (int i = 0; i < sample_count; i++) {
			inputs[i_count][i] = 4.0*RND;
		}
	}

	for (int i = 0; i < sample_count; i++) {
		output[i] = 0.31415+exp(-inputs[0][i]) + sin(sqrt(abs(0.1234*inputs[1][i]))) + inputs[2][i]/2.0;
	}
}

//---------------------------------------------------------------

float finder::error(const vf &vec)
{
	float sum;
	
	sum = 0;
	for (int i = 0; i < sample_count; i++) {
		float e = (output[i] - vec[i]);
		e = e * e;
		sum += e;
	}
	return sum;
}

//---------------------------------------------------------------

vf  finder::vec()
{
	return vf(sample_count);
}

//---------------------------------------------------------------

void finder::print(vf &vec)
{
	for (int i = 0; i < sample_count; i++) {
		printf("%3.3f ", vec[i]);
		if (i % 16 == 0) printf("\n");
	}
	printf("\n"); 
}

//---------------------------------------------------------------

void finder::add(const vf &v1, const vf &v2, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v1[i] + v2[i];
	}
}

//---------------------------------------------------------------

void finder::sub(const vf &v1, const vf &v2, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v1[i] - v2[i];
	}
}

//---------------------------------------------------------------

void finder::mul(const vf &v1, const vf &v2, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v1[i] * v2[i];
	}
}


//---------------------------------------------------------------

void finder::div(const vf &v1, const vf &v2, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v1[i] / v2[i];
	}
}

//---------------------------------------------------------------

void finder::expo(const vf &v1, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = pow(2.71828182, v1[i]);
	}
}

//---------------------------------------------------------------

void finder::cst(float v, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v;
	}
}

//---------------------------------------------------------------

void finder::vsqrt(const vf &v1, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = sqrt(fabs(v1[i]));
	}
}

//---------------------------------------------------------------

void finder::vsqr(const vf &v1, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = v1[i]*v1[i];
	}
}

//---------------------------------------------------------------

void finder::vsin(const vf &v1, vf &result)
{
	for (int i = 0; i < sample_count; i++) {
		result[i] = sin(v1[i]);
	}
}

//---------------------------------------------------------------

   tree::tree()
{
	op = 0;
	var = 0;
	val = 0;
	
	left = 0;
	right = 0;
}

//---------------------------------------------------------------

void destroy(tree *t)
{
        if (t == 0) return;

	if (t->left) destroy(t->left);
	if (t->right) destroy(t->right);

	delete t;
}

//---------------------------------------------------------------

  tree::~tree()
{
}

//---------------------------------------------------------------

void finder::rand_opt(char *c, int *unarity, int *var, float *value)
{
	int rnd = brand() % 14;

    switch(rnd) {
    
	case 0:
	case 1:
		*c= '+';
		*unarity = 2;
		break;	
	

	case 2:
	case 3:
		*c= '-';
		*unarity = 2;
		break;	
	
	case 4:
	case 5:
		*c= '*';
		*unarity = 2;
		break;	
	
	case 6:
	case 7:
		*c= '/';
		*unarity = 2;
		break;	
	
	case 8:
		*c= 'v';
		*unarity = 0;
		*var = brand() % MAX_INPUT;
		break;	
	
	case 9:
		*c= '1';
		*unarity = 1;
		break;	
	
	case 10:
		*c= '2';
		*unarity = 1;
		break;	
	

	case 11:
		*c= 'c';
		*unarity = 0;
		*value = RND;
		break;	
	
 	case 12:
		*c= '3';
		*unarity = 1;
		break;	
    case 13:
        *c = '^';
        *unarity = 1;
        break;
	}

}

//---------------------------------------------------------------

void finder::rand_opt_terminal(char *c, int *unarity, int *var, float *value)
{
	do {
		rand_opt(c, unarity, var, value);
	} while(*unarity != 0);
}

//---------------------------------------------------------------

int length(tree *t)
{
        if (t == NULL)
                return 0;

        return (length(t->left) + length(t->right) + 1);     
}

//---------------------------------------------------------------

void tree::print(int level)
{
        if (this == 0)
                return;

	level++;
	for (int i = 0; i < level; i++) {printf("  "); } 

	if (op == 'v') {
		printf("x%d\n", var);
	}
	else
	if (op == 'c') {
		printf("%f\n", val);
	}
	else {
		if (op == '1') {
			printf("sin\n");
		}
		else if (op == '2') {
			printf("sqrt\n");
		}
		else if (op == '3') {
			printf("sqr\n");
		}
		else {
			printf("%c\n", op);
		}
	}

	if (this->left) this->left->print(level);
	if (this->right) this->right->print(level);
}

//---------------------------------------------------------------

tree *clone(tree *t)
{

        if (t == 0)
                return 0;

        tree *nt = new tree();

        *nt = *t;


        nt->left = clone(t->left);
        nt->right = clone(t->right);


        return nt;
}

//---------------------------------------------------------------

tree *finder::build_tree(int depth)
{
	char	op;
	int	unarity;
	int	var;
	float	val;

	if (depth == 0) return 0;

	tree	*t = new tree();

	//printf("%d\n", depth);
	if (depth > 1) {
		rand_opt(&op, &unarity, &var, &val);
		//printf("op is %c\n", op);
		t->val = val;
		t->op = op;
		t->var = var;

		if (unarity == 1) {
			t->right = 0;
			t->left = build_tree(depth - 1);
		}
		if (unarity == 2) {
			t->right = build_tree(depth - 1);
			t->left = build_tree(depth - 1);
		}	
		if (unarity == 0) {
			t->right = 0;
			t->left = 0;
		}
	}
	if (depth == 1) {
		rand_opt_terminal(&op, &unarity, &var, &val);
		//printf("op terminal is %c\n", op);
		t->val = val;
		t->op = op;
		t->var = var;
		t->left = 0;
		t->right = 0;
	}
	return t;	
}

//---------------------------------------------------------------

int tweak_constants(tree *t)
{
        if (t == 0) return 0;

        if (t->op == 'c') {
                t->val += RND / 10.0;
                return 1;        
        }
        return (tweak_constants(t->left) + tweak_constants(t->right));
}


//---------------------------------------------------------------

vf finder::eval(tree *t)
{
	if (t->op == 'c') {
		vf v = vec();
		cst(t->val, v);
		return v;
	}
	if (t->op == 'v') {
		return inputs[t->var];
	}

	if (t->op == '+') {
		vf left = eval(t->left);
		vf right = eval(t->right);
		vf out = vec();
		add(left, right, out);
		return out;
	}

	if (t->op == '-') {
		vf left = eval(t->left);
		vf right = eval(t->right);
		vf out = vec();
		sub(left, right, out);
		return out;
	}
	if (t->op == '*') {
		vf left = eval(t->left);
		vf right = eval(t->right);
		vf out = vec();
		mul(left, right, out);
		return out;
	}
	if (t->op == '/') {
		vf left = eval(t->left);
		vf right = eval(t->right);
		vf out = vec();
		div(left, right, out);
		return out;
	}
	if (t->op == '^') {
		vf left = eval(t->left);
		vf out = vec();
		expo(left, out);
		return out;
	}
	if (t->op == '1') {
		vf left = eval(t->left);
		vf out = vec();
		vsin(left, out);
		return out;
	}
	if (t->op == '2') {
		vf left = eval(t->left);
		vf out = vec();
		vsqrt(left, out);
		return out;
	}
	if (t->op == '3') {
		vf left = eval(t->left);
		vf out = vec();
		vsqr(left, out);
		return out;
	}

	printf("bad op %c\n", t->op);
}


//---------------------------------------------------------------
#define POP 45000

//---------------------------------------------------------------

class individual {
public:
        individual();
        tree    *a_tree;
        float   error;
};

//---------------------------------------------------------------

individual::individual()
{
        a_tree = 0;
        error = 0;
}

//---------------------------------------------------------------

class population {
public:
        individual pop[POP];
        finder     *a_finder;

        population(finder *f);
        void eval_population();
        void build_population();
        void sort_population();
        void display_population();
        void extinction();

        void cross(float ratio, float up);
        void newborn(float up);
        void kill_twins();

};

//---------------------------------------------------------------

population::population(finder *f)
{
        a_finder = f;
}

int sz()
{
        return 1 + brand()%5;
}

//---------------------------------------------------------------

void population::build_population()
{
        for (int i = 0; i < POP; i++) {
                pop[i].a_tree = a_finder->build_tree(sz());
        }
}

//---------------------------------------------------------------

int cmp(const void* p1, const void* p2)
{
        individual *i1, *i2;

        i1 = (individual*)p1;
        i2 = (individual*)p2;

        return(i1->error > i2->error);
}

//---------------------------------------------------------------

void population::eval_population()
{
        for (int i = 0; i < POP; i++) {
                vf vec = a_finder->eval(pop[i].a_tree);
		float error = a_finder->error(vec);
                pop[i].error = error;
        }
}

//---------------------------------------------------------------

void population::sort_population()
{
        qsort(pop, POP, sizeof(individual), cmp);
}

//---------------------------------------------------------------

void population::display_population()
{
        for (int i = 0; i < 1; i++) {
                printf("%d %f %d\n", i, pop[i].error, length(pop[i].a_tree));
                pop[i].a_tree->print(0);
        }
}

//---------------------------------------------------------------

tree *tree::pick_branch(int level)
{
        level--;

        if (level == 0) {
                return this;
        }

        if (left == 0 && right == 0) {
                return this;
        }

        int r = brand();

        r = r % 2;

        if (r == 1 && left) {
                return left->pick_branch(level);
        }
        if (right == 0)
                return this;
        else
                return right->pick_branch(level);
        
}

//---------------------------------------------------------------


tree *build_child(tree *t1, tree *t2)
{
        tree    *t;
       

        t = clone(t1);

        int l1 = brand() % 5;
        int l2 = brand() % 4;

        tree *pt1 = t->pick_branch(l1);
        tree *pt2 = t2->pick_branch(l1);

        
        destroy(pt1->left);

        destroy(pt1->right);
        pt1->left = clone(pt2->left);
        pt1->right = clone(pt2->right);
        pt1->op = pt2->op;
        pt1->var = pt2->var;
        pt1->val = pt2->val;

        return t;
}

//---------------------------------------------------------------

void    population::cross(float ratio, float up)
{
        int     elite = POP * ratio;
        int     kids = POP * up;
        int     victime;

        for (victime = elite; victime < kids; victime++) {
                destroy(pop[victime].a_tree);
                pop[victime].a_tree = 0;
        }

        for (victime = elite; victime < kids; victime++) {

                tree *t;

                do {
                        int     parent1 = brand() % (elite);
                        int     parent2 = 130 + brand() % (elite-130);
                        
                        t = build_child(pop[parent1].a_tree, pop[parent2].a_tree);
                        if (length(t) > 24) {
                                destroy(t);
                                t = 0;
                        }
                } while(t == 0);


                pop[victime].a_tree = t;
        }
}

//---------------------------------------------------------------

void    population::newborn(float up)
{
        int     kids = POP * up;

        for (int new_p = kids + 1; new_p < POP; new_p++) {
                destroy(pop[new_p].a_tree);
                pop[new_p].a_tree = a_finder->build_tree(sz());
        }
}

//---------------------------------------------------------------

void    population::extinction()
{
         for (int new_p = 1; new_p < POP; new_p++) {
                destroy(pop[new_p].a_tree);
                pop[new_p].a_tree = a_finder->build_tree(sz());
        }
}
//---------------------------------------------------------------

void    population::kill_twins()
{
         for (int i = 1; i < POP; i++) {
                float v1 = pop[i].error;
                float v2 = pop[i-1].error;
                float delta = abs(v1-v2);
                if (delta < 0.001) {   //twins
                        if (brand() % 5 < 2) {  
                                destroy(pop[i].a_tree);
                                pop[i].a_tree = a_finder->build_tree(sz());
                        }
                        else {

                                int cnt = tweak_constants(pop[i].a_tree);
                                //if (cnt == 0) {
                                  //      pop[i].a_tree->destroy();
                                    //    pop[i].a_tree = a_finder->build_tree(4);
                                //}
                        }
                }
        }
}


//---------------------------------------------------------------


int main()
{
	finder	f(20);

	srand (time(NULL));
	rx = time(NULL);

	f.init_sample();
	tree	*t;
	float	best_error = 1e30;
	int	cnt = 0;

        population p(&f);


        for (int k = 0; k < 200; k++) {
        p.build_population();
        p.eval_population();
        p.sort_population();

        for (int gen = 1; gen < 100110; gen++) {
 
                p.newborn(0.8);
                p.cross(0.4, 0.8);
                p.eval_population();
                p.sort_population();
                p.kill_twins();
                if (gen % 82 == 0) {
                        p.eval_population();
                        p.sort_population();
                        p.extinction();
                }
                p.eval_population();
                p.sort_population();
                if (gen % 2 == 0) p.display_population();

        }
        p.display_population();
	    //printf("%f\n", cross);
        printf("%f\n", p.pop[0].error);
        }
        exit(-1);

	for (long tmp = 0; tmp < 100000000; tmp++) {
		cnt++;
		t = f.build_tree(6);

		vf vec = f.eval(t);


		float error = f.error(vec);

		if (error < best_error) {
			printf("%d - error %f\n", cnt, error);
			t->print(0);
			best_error = error;
		}

		destroy(t);
	}


	return 0;
}


//---------------------------------------------------------------

