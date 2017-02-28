#include <iostream>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <math.h>

using namespace std;

char ch;
int val;
int sym;

enum Symbol
{
	Plus, Minus, Times, Divide, Mod, Power, LParen, RParen, Number, EndLine, Illegal
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
		sym = Illegal;
	}
	return sym;
}

int expression(Node *node);

int factor(Node *node)
{
	// factor = constant | "(" , expression , ")";
	int res = 0;
	assert((sym == Number) || (sym == LParen));
	if (sym == Number)
	{
		node->kind = sym;
		node->val = val;
		res = val;
		sym = getSymbol();
	}
	else
	{
		sym = getSymbol();
		res = expression(node);
		assert(sym == RParen);
		sym = getSymbol();
	}
	return res;
}

int preFactor(Node *node)
{
	// preFactor = factor , { "^" , factor };
	Node *leftNode = new Node();
	int res = 0;
	res = factor(leftNode);

	*node = *leftNode;
	Node *tempLeftNode = new Node();
	tempLeftNode->left = leftNode;
	while (sym == Power)
	{
		Node *midNode = new Node();
		midNode->kind = sym;
		Node *rightNode = new Node();
		sym = getSymbol();
		res = pow(res, factor(rightNode));

		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;
		tempLeftNode->left = midNode;
		*node = *midNode;
	}
	return res;
}

int term(Node *node)
{
	//term = preFactor, { ("*" | "/" | "%"), preFactor };
	Node *leftNode = new Node();
	int res = 0;
	res = preFactor(leftNode);

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
			res *= preFactor(rightNode);
			break;
		case Divide:
			midNode->kind = sym;
			sym = getSymbol();
			res /= preFactor(rightNode);
			break;
		case Mod:
			midNode->kind = sym;
			sym = getSymbol();
			res %= preFactor(rightNode);
			break;
		}
		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;
		tempLeftNode->left = midNode;
		*node = *midNode;
	}
	return res;
}

int expression(Node *node)
{
	//expression = ["+" | "-"], term, { ("+" | "-"), term };
	Node *leftNode = new Node();
	int positivity = 1;
	int res = 0;

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

	res = positivity * term(leftNode);
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
		res += positivity * term(rightNode);

		midNode->left = tempLeftNode->left;
		midNode->right = rightNode;

		tempLeftNode->left = midNode;
		*node = *midNode;
	}
	return res;
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
	}
	if (node->left != NULL)
		printExpressionTree(node->left, level + 1);
}

void symbolMap(Node *node)
{
	switch (node->kind)
	{
	case Plus:
		cout << "+";
		break;
	case Minus:
		cout << "-";
		break;
	case Times:
		cout << "*";
		break;
	case Divide:
		cout << "/";
		break;
	case Mod:
		cout << "%";
		break;
	case Power:
		cout << "^";
		break;
	case Number:
		cout << node->val;
		break;
	}
}

void prefix_traversal(Node *node)
{
	cout << "(";
	symbolMap(node);
	if (node->left != NULL)
		prefix_traversal(node->left);

	

	if (node->right != NULL)
		prefix_traversal(node->right);
	cout << ")";
}

void infix_traversal(Node *node)
{
	cout << "(";
	if (node->left != NULL)
		infix_traversal(node->left);

	symbolMap(node);

	if (node->right != NULL)
		infix_traversal(node->right);
	cout << ")";
}

void postfix_traversal(Node *node)
{
	cout << "(";
	if (node->left != NULL)
		postfix_traversal(node->left);
	if (node->right != NULL)
		postfix_traversal(node->right);
	symbolMap(node);
	cout << ")";
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
	printExpressionTree(root, 0);

	postfix_traversal(root);

	getchar();
	return 0;
}