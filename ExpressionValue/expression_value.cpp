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

int expression();

int factor()
{
	// factor = constant | "(" , expression , ")";
	int res = 0;
	assert((sym == Number) || (sym == LParen));
	if (sym == Number)
	{
		res = val;
		sym = getSymbol();
	}
	else
	{
		sym = getSymbol();
		res = expression();
		assert(sym == RParen);
		sym = getSymbol();
	}
	return res;
}

int preFactor()
{
	// preFactor = factor , { "^" , factor };
	int res = 0;
	res = factor();
	while (sym == Power)
	{
		sym = getSymbol();
		res = pow(res, factor());
	}
	return res;
}

int term()
{
	//term = preFactor, { ("*" | "/" | "%"), preFactor };
	int res = 0;
	res = preFactor();

	while (sym == Times || sym == Divide || sym == Mod)
	{
		switch (sym)
		{
			case Times:
				sym = getSymbol();	
				res *= preFactor();
				break;
			case Divide:
				sym = getSymbol();
				res /= preFactor();
				break;
			case Mod:
				sym = getSymbol();
				res %= preFactor();
				break;
		}
	}
	return res;
}

int expression()
{
	//expression = ["+" | "-"], term, { ("+" | "-"), term };
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

	res = positivity * term();

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
		sym = getSymbol();
		res += positivity * term();
	}
	return res;
}

int main()
{
	cout << "Input expression: ";
	ch = getchar();

	sym = getSymbol();
	int res = expression();

	assert(sym == EndLine);
	cout << "Succesfully parsed an expression." << endl;
	cout << "Result: " << res;
	getchar();
	return 0;
}