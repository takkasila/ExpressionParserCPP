#include <iostream>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <math.h>

using namespace std;

char ch;
int val;
int sym;
char variable;

enum Symbol
{
	Plus, Minus, Times, Divide, Mod, Power, LParen, RParen, Number, EndLine, Illegal, Var
};

class Node
{
public:
	int kind;
	int val;
	Node *left;
	Node *right;

	Node()
	{
		kind = Illegal;
		val = 0;
		left = NULL;
		right = NULL;
	}

	Node & operator = (Node *other)
	{
		kind = other->kind;
		val = other->val;
		left = other->left;
		right = other->right;
	}

	Node *duplicate()
	{
		Node *newRoot = new Node();
		newRoot->kind = kind;
		newRoot->val = val;
		if (left != NULL)
			newRoot->left = left->duplicate();
		if (right != NULL)
			newRoot->right = right->duplicate();
		return newRoot;
	}

	void pow2()
	{
		Node *tempLeft = this->duplicate();
		Node *tempRight = this->duplicate();
		kind = Times;
		val = 0;
		left = tempLeft;
		right = tempRight;
	}

	void diff()
	{
		switch (kind)
		{
		case Plus:
		case Minus:
			left->diff();
			right->diff();
			break;
		case Times:
		{
			Node* copyLeft = left->duplicate();
			Node* copyLeftDiff = left->duplicate();
			copyLeftDiff->diff();
			Node* copyRight = right->duplicate();
			Node* copyRightDiff = right->duplicate();
			copyRightDiff->diff();

			Node* leftTerm = new Node();
			leftTerm->kind = Times;
			leftTerm->left = copyLeft;
			leftTerm->right = copyRightDiff;

			Node* rightTerm = new Node();
			rightTerm->kind = Times;
			rightTerm->left = copyRight;
			rightTerm->right = copyLeftDiff;

			kind = Plus;
			left = leftTerm;
			right = rightTerm;
			break;
		}
		case Divide:
		{
			Node* copyLeft = left->duplicate();
			Node* copyLeftDiff = left->duplicate();
			copyLeftDiff->diff();
			Node* copyRight = right->duplicate();
			Node* copyRightDiff = right->duplicate();
			copyRightDiff->diff();

			Node* leftTerm = new Node();
			leftTerm->kind = Times;
			leftTerm->left = copyRight;
			leftTerm->right = copyLeftDiff;

			Node* rightTerm = new Node();
			rightTerm->kind = Times;
			rightTerm->left = copyLeft;
			rightTerm->right = copyRightDiff;

			Node* midTerm = new Node();
			midTerm->kind = Minus;
			midTerm->left = leftTerm;
			midTerm->right = rightTerm;

			Node *bottomPow2 = right->duplicate();
			bottomPow2->pow2();

			kind = Divide;
			left = midTerm;
			right = bottomPow2;
			break;
		}
		case Power:
		{
			//x^y = y*x^(y-1)*x'
			//(y-1)
			Node *yTemp1 = right->duplicate();
			Node *one = new Node();
			one->kind = Number;
			one->val = 1;
			Node *termYMinus1 = new Node();
			termYMinus1->kind = Minus;
			termYMinus1->left = yTemp1;
			termYMinus1->right = one;
			//x^(y-1)
			Node *xTemp1 = left->duplicate();
			Node *termxPowYMinus = new Node();
			termxPowYMinus->kind = Power;
			termxPowYMinus->left = xTemp1;
			termxPowYMinus->right = termYMinus1;
			//y*x^(y-1)
			Node *yTemp2 = right->duplicate();
			Node *termYTimesXPow = new Node();
			termYTimesXPow->kind = Times;
			termYTimesXPow->left = yTemp2;
			termYTimesXPow->right = termxPowYMinus;
			//x'
			Node *xDiff = left->duplicate();
			xDiff->diff();
			//x^y = y*x^(y-1)*x'
			kind = Times;
			left = termYTimesXPow;
			right = xDiff;
			break;
		}
		case Number:
			val = 0;
			break;
		case Var:
			kind = Number;
			val = 1;
			break;
		}
	}

	void optimize()
	{
		if (left == NULL || right == NULL)
			return;
		if (left != NULL)
			left->optimize();
		if (right != NULL)
			right->optimize();
		//0*f
		if (kind == Times && ((left->kind == Number && left->val == 0) || (right->kind == Number && right->val == 0)))
		{
			kind = Number;
			val = 0;
			left = NULL;
			right = NULL;
		}
		//0+f
		else if (kind == Plus && left->kind == Number && left->val == 0 )
		{
			kind = right->kind;
			val = right->val;
			if (right->kind != Number && right->kind != Var)
			{
				left = right->left;
				right = right->right;
			}
			else
			{
				left = NULL;
				right = NULL;
			}
		}
		//f+0
		else if (kind == Plus && right->kind == Number && right->val == 0)
		{
			kind = left->kind;
			val = left->val;
			if (left->kind != Number && left->kind != Var)
			{
				right = left->right;
				left = left->left;
			}
			else
			{
				left = NULL;
				right = NULL;
			}
		}
		//1*f
		else if (kind == Times && left->kind == Number && left->val == 1)
		{
			kind = right->kind;
			val = right->val;
			if (right->kind != Number && right->kind != Var)
			{
				left = right->left;
				right = right->right;
			}
			else
			{
				left = NULL;
				right = NULL;
			}
		}
		//f*1
		else if (kind == Times && right->kind == Number && right->val == 1)
		{
			kind = left->kind;
			val = left->val;
			if (left->kind != Number && left->kind != Var)
			{
				right = left->right;
				left = left->left;
			}
			else
			{
				left = NULL;
				right = NULL;
			}
		}
		//f-f
		//f+f
		else if ((left->kind == Number || left->kind == Var) && (right->kind == Number || right->kind == Var)
			&& left->val == right->val
			&& left->kind == right->kind)
		{
			switch (kind)
			{
			case Plus:
			{
				Node *two = new Node();
				two->kind = Number;
				two->val = 2;
				left = two;
				kind = Times;
				break;
			}
			case Minus:
			{
				kind = Number;
				val = 0;
				left = NULL;
				right = NULL;
				break;
			}
			}
		}
		//f^1
		else if (kind == Power && right->kind == Number && right->val == 1)
		{
			kind = left->kind;
			val = left->val;
			if (left->kind != Number && left->kind != Var)
			{
				right = left->right;
				left = left->left;
			}
			else
			{
				left = NULL;
				right = NULL;
			}
		}
		//f^0
		else if (kind == Power && right->kind == Number && right->val == 0)
		{
			kind = Number;
			val = 1;
			left = NULL;
			right = NULL;
		}
	}
};

void getNumber()
{
	// digit, { digit };
	val = 0;
	while (('0' <= ch) && (ch <= '9'))
	{
		// Char to int
		val = val * 10 + ch - '0';
		ch = getchar();
	}
}

int getSymbol()
{
	int sym;
	while ((ch != EOF) && (ch <= ' ') && (ch != '\n'))
		ch = getchar();
	switch (ch)
	{
	case '\n':
		sym = EndLine;
		break;
	case '+':
		sym = Plus;
		ch = getchar();
		break;
	case '-':
		sym = Minus;
		ch = getchar();
		break;
	case '*':
		sym = Times;
		ch = getchar();
		break;
	case '/':
		sym = Divide;
		ch = getchar();
		break;
	case '%':
		sym = Mod;
		ch = getchar();
		break;
	case '^':
		sym = Power;
		ch = getchar();
		break;
	case '(':
		sym = LParen;
		ch = getchar();
		break;
	case ')':
		sym = RParen;
		ch = getchar();
		break;
		// digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		sym = Number;
		getNumber();
		break;
	default:
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			sym = Var;
			variable = ch;
			ch = getchar();
		}
		else
			sym = Illegal;
	}
	return sym;
}

void expression(Node *node);

void factor(Node *node)
{
	// factor = constant | "(" , expression , ")" | Var;
	assert(sym == Number || sym == LParen || sym == Var);
	if (sym == Number)
	{
		node->kind = sym;
		node->val = val;
		sym = getSymbol();
	}
	else if (sym == LParen)
	{
		sym = getSymbol();
		expression(node);
		assert(sym == RParen);
		sym = getSymbol();
	}
	else
	{
		node->kind = sym;
		node->val = variable;
		sym = getSymbol();
	}
}

void preFactor(Node *node)
{
	// preFactor = factor , { "^" , factor };
	Node *leftNode = new Node();
	factor(leftNode);

	*node = *leftNode;
	Node *tempLeftNode = new Node();
	tempLeftNode->left = leftNode;
	while (sym == Power)
	{
		Node *midNode = new Node();
		midNode->kind = sym;
		Node *rightNode = new Node();
		sym = getSymbol();
		factor(rightNode);

		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;
		tempLeftNode->left = midNode;
		*node = *midNode;
	}
}

void term(Node *node)
{
	//term = preFactor, { ("*" | "/" | "%"), preFactor };
	Node *leftNode = new Node();
	preFactor(leftNode);

	*node = *leftNode;
	Node *tempLeftNode = new Node();
	tempLeftNode->left = leftNode;
	while (sym == Times || sym == Divide || sym == Mod)
	{
		Node *midNode = new Node();
		Node *rightNode = new Node();
		switch (sym)
		{
		case Times:
			midNode->kind = sym;
			sym = getSymbol();
			preFactor(rightNode);
			break;
		case Divide:
			midNode->kind = sym;
			sym = getSymbol();
			preFactor(rightNode);
			break;
		case Mod:
			midNode->kind = sym;
			sym = getSymbol();
			preFactor(rightNode);
			break;
		}
		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;
		tempLeftNode->left = midNode;
		*node = *midNode;
	}
}

void expression(Node *node)
{
	//expression = ["+" | "-"], term, { ("+" | "-"), term };
	Node *leftNode = new Node();
	int positivity = 1;

	switch (sym)
	{
	case Plus:
		positivity = 1;
		sym = getSymbol();
		break;
	case Minus:
		positivity = -1;
		sym = getSymbol();
		break;
	}

	term(leftNode);
	*node = *leftNode;
	Node *tempLeftNode = new Node();
	tempLeftNode->left = leftNode;
	while (sym == Plus || sym == Minus)
	{
		switch (sym)
		{
		case Plus:
			positivity = 1;
			break;
		case Minus:
			positivity = -1;
			break;
		}
		Node *midNode = new Node();
		midNode->kind = sym;

		Node *rightNode = new Node();
		sym = getSymbol();
		term(rightNode);

		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;

		tempLeftNode->left = midNode;
		*node = *midNode;
	}
}

void printExpressionTree(Node *node, int level)
{
	if (node->kind == Illegal)
		return;

	if (node->right != NULL)
		printExpressionTree(node->right, level + 1);

	for (int i = 0; i < level; i++)
		cout << "    ";
	switch (node->kind)
	{
	case Plus:
		cout << "+" << endl;
		break;
	case Minus:
		cout << "-" << endl;
		break;
	case Times:
		cout << "*" << endl;
		break;
	case Divide:
		cout << "/" << endl;
		break;
	case Mod:
		cout << "%" << endl;
		break;
	case Power:
		cout << "^" << endl;
		break;
	case Number:
		cout << node->val << endl;
		break;
	case Var:
		cout << (char) node->val << endl;
		break;
	}
	if (node->left != NULL)
		printExpressionTree(node->left, level + 1);
}

int main()
{
	Node *root = new Node();
	cout << "Input expression: ";
	ch = getchar();

	sym = getSymbol();
	expression(root);

	assert(sym == EndLine);
	cout << "Succesfully parsed an expression." << endl;
	cout << "Expression Tree: " << endl;
	printExpressionTree(root, 0);
	root->diff();
	cout << "Diff Expression Tree: " << endl;
	printExpressionTree(root, 0);
	cout << "Optimized Expression Tree: " << endl;
	root->optimize();
	printExpressionTree(root, 0);
	getchar();
	return 0;
}
