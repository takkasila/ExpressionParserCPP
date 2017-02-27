#include <iostream>
#include <string>
#include <assert.h>
#include <stdio.h>

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

void expression();

void factor()
{
	// factor = constant | "(" , expression , ")";
	assert((sym == Number) || (sym == LParen));
	if (sym == Number)
		sym = getSymbol();
	else
	{
		sym = getSymbol();
		expression();
		assert(sym == RParen);
		sym = getSymbol();
	}
}

void preFactor()
{
	// preFactor = factor , { "^" , factor };
	factor();
	while (sym == Power)
	{
		sym = getSymbol();
		factor();
	}
}

void term()
{
	//term = preFactor, { ("*" | "/" | "%"), preFactor };
	preFactor();
	while (sym == Times || sym == Divide || sym == Mod)
	{
		sym = getSymbol();
		preFactor();
	}
}

void expression()
{
	//expression = ["+" | "-"], term, { ("+" | "-"), term };
	if (sym == Plus || sym == Minus)
		sym = getSymbol();
	term();
	while (sym == Plus || sym == Minus)
	{
		sym = getSymbol();
		term();
	}
}

int main()
{
	cout << "Power, Input expression: ";
	ch = getchar();

	sym = getSymbol();
	expression();

	assert(sym == EndLine);
	cout << "Succesfully parsed an expression." << endl;
	getchar();
	return 0;
}