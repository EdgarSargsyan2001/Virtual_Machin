#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

int line = 0;
int TOKENIZING_SIZE_FOR_ROW = 10;
// creat simbole TABLE
std::map<std::string, int> TABLE;

void tokenizing(std::string &row, std::vector<std::string> &tokens);
bool declaration(std::vector<std::string> &tokens);
bool change_val_variable(std::vector<std::string> &tokens);
bool todo_arithmetic_inst(std::string &var, std::string &inst, std::string &op1, std::string &op2);
bool assignment(std::string &var, std::string &inst, std::string &op1, std::string &op2, std::string mess); // վերագրում
bool print_log(std::fstream &oFile, std::vector<std::string> &tokens);
bool include_var(std::vector<std::string> &tokens);

// smoll function
bool is_arithmetic_inst(char s);
int calculator(char inst, int op1, int op2);
bool is_digit(std::string &s);

// print
void print_vector(std::vector<std::string> &tokens);
void print_TABLE();
void error_message(std::string mess);

int main(int argc, char *argv[])
{

	// open source code
	std::fstream fs;
	// get file name from argv[1];
	fs.open(argv[1], std::ios::in);
	if (!fs.is_open())
	{
		std::cout << "ERROR: Can't open input file\n";
		return 0;
	}

	std::fstream oFile;
	// open write file for log
	oFile.open("process_logs.txt", std::ios::out);
	if (!oFile.is_open())
	{
		std::cout << "error: can't open output flie\n";
		return 0;
	}

	// create row
	std::string row;

	// get the row during loop iteration
	while (getline(fs, row))
	{
		line++; // incriment line

		if (row.empty() || row[0] == ';')
			continue;

		// creat vector for spliting row
		std::vector<std::string> tokens(TOKENIZING_SIZE_FOR_ROW, "!");
		tokenizing(row, tokens);
		// print_vector(tokens), std::cout << "\n";

		// ========== instruction execution =================

		// declaring variable
		if (tokens[0] == "decl")
		{
			if (!declaration(tokens))
				break;
		}
		// log variable
		else if (tokens[0] == "log")
		{
			if (!print_log(oFile, tokens))
				break;
		}
		else if (tokens[0] == "inc")
		{
			if (!include_var(tokens))
				break;
		}
		// change the value of a variable
		else if (TABLE.find(tokens[0]) != TABLE.end())
		{
			if (!change_val_variable(tokens))
				break;
		}
		else
		{
			error_message("varieble not found:)");
			break;
		}

		// print_TABLE(), std::cout << "\n";
	}

	fs.close();
	oFile.close();
}

void tokenizing(std::string &row, std::vector<std::string> &tokens)
{
	std::string word;
	int j = 0;

	for (int i = 0; i < row.length(); ++i)
	{
		if (row[i] != ' ')
		{
			if (row[i] == ';')
			{
				if (!word.empty())
					tokens[j++] = word;
				return;
			}
			if (is_arithmetic_inst(row[i]))
			{
				if (!word.empty())
				{
					tokens[j++] = word;
					word.clear();
				}
				// add arithmetic inst symbol
				tokens[j++] = (std::string(1, row[i]));
				continue;
			}
			word.push_back(row[i]);
		}
		else if (row[i] == ' ' || row[i] == '\0')
		{
			if (!word.empty())
				tokens[j++] = word;
			word.clear();
		}
	}
	if (!word.empty())
		tokens[j++] = word;
}

bool declaration(std::vector<std::string> &tokens)
{
	std::string var = tokens[1];
	// tokens[1] -> variable
	// tokens[2] -> havasar ' = '
	// tokens[3] -> operand1
	// tokens[4] -> arithmetic instruction
	// tokens[5] -> operand2

	if (var != "!")
	{
		if (TABLE.find(var) != TABLE.end())
		{ // if true -> var. has in symbol TABLE
			error_message("variable is already defined");
			return false;
		}

		if (tokens[2] != "!" && tokens[3] != "!")
		{
			if (!assignment(var, tokens[4], tokens[3], tokens[5], "variable name was not found"))
				return false;
		}
		// none init variable
		else
		{
			int a;
			TABLE[var] = a;
		}
	}
	else
	{
		error_message("error: variable name was not include");
		return false;
	}

	return true;
}

bool change_val_variable(std::vector<std::string> &tokens)
{
	// tokens[0] -> variable  // already decl.
	// tokens[1] -> havasar ' = '
	// tokens[2] -> operand1
	// tokens[3] -> arithmetic instruction
	// tokens[4] -> operand2

	if (tokens[1] != "!" && tokens[2] != "!")
	{
		if (!assignment(tokens[0], tokens[3], tokens[2], tokens[4], "second variable not found"))
			return false;
	}
	else
	{
		error_message(" '=' or init. not found");
		return false;
	}

	return true;
}

bool todo_arithmetic_inst(std::string &var, std::string &inst, std::string &op1, std::string &op2)
{

	if (op1 == "!" || op2 == "!")
		error_message("operand don't found");

	if (is_digit(op1))
	{
		if (is_digit(op2))
		{
			TABLE[var] = calculator(inst[0], std::stoi(op1), std::stoi(op2));
		}
		else if (TABLE.find(op2) != TABLE.end())
		{
			TABLE[var] = calculator(inst[0], std::stoi(op1), TABLE[op2]);
		}
		else
		{
			error_message("operand isn't variable or isn't decl.");
			return false;
		}
	}
	else if (TABLE.find(op1) != TABLE.end())
	{
		if (is_digit(op2))
		{
			TABLE[var] = calculator(inst[0], TABLE[op1], std::stoi(op2));
		}
		else if (TABLE.find(op2) != TABLE.end())
		{
			TABLE[var] = calculator(inst[0], TABLE[op1], TABLE[op2]);
		}
		else
		{
			error_message("operand isn't variable or isn't decl.");
			return false;
		}
	}
	else
	{
		error_message("operand isn't variable or isn't decl.");
		return false;
	}
	return true;
}
bool assignment(std::string &var, std::string &inst, std::string &op1, std::string &op2, std::string mess)
{
	if (inst != "!")
	{
		if (!todo_arithmetic_inst(var, inst, op1, op2))
			return false;
	}
	else if (is_digit(op1))
	{
		TABLE[var] = std::stoi(op1);
	}
	else if (TABLE.find(op1) != TABLE.end())
	{
		TABLE[var] = TABLE[op1];
	}
	else
	{
		error_message(mess);
		return false;
	}

	return true;
}

bool print_log(std::fstream &oFile, std::vector<std::string> &tokens)
{
	// std::cout << tokens.size() << "\n";
	if (tokens[1] != "!" && tokens[2] == "!")
	{
		if (TABLE.find(tokens[1]) != TABLE.end())
		{
			std::cout << tokens[1] << " = " << TABLE[tokens[1]] << std::endl;
			oFile << tokens[1] << " = " << TABLE[tokens[1]] << "\n";
		}
		else
		{
			error_message("variable not found");
			return false;
		}
	}
	else
	{
		error_message("to many or few arguments");
		return false;
	}
	return true;
}
bool include_var(std::vector<std::string> &tokens)
{
	if (tokens[1] != "!" && tokens[2] == "!")
	{
		if (TABLE.find(tokens[1]) != TABLE.end())
		{
			std::cin >> TABLE[tokens[1]];
		}
		else
		{
			error_message("variable not found");
			return false;
		}
	}
	else
	{
		error_message("to many or few arguments");
		return false;
	}

	return true;
}

bool is_arithmetic_inst(char x)
{
	char str[] = "+-*/%=";
	int i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == x)
		{
			return true;
		}
		i++;
	}
	return false;
}
int calculator(char inst, int op1, int op2)
{
	if ('+' == inst)
		return op1 + op2;
	if ('-' == inst)
		return op1 - op2;
	if ('/' == inst)
		return op1 / op2;
	if ('*' == inst)
		return op1 * op2;
	if ('%' == inst)
		return op1 % op2;
	return 0;
}
void print_vector(std::vector<std::string> &tokens)
{
	std::cout << line << ": ";
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i] == "!")
			return;
		std::cout << tokens[i] << " ";
	}
	std::cout << std::endl;
}
void print_TABLE()
{
	auto it = TABLE.begin();
	while (it != TABLE.end())
	{
		std::cout << "key " << it->first << " val: " << it->second << "\n";
		++it;
	}
}
bool is_digit(std::string &s)
{
	for (int i = 0; i < s.length(); ++i)
	{
		if (s[i] < '0' || s[i] > '9')
			return false;
	}
	return true;
}
void error_message(std::string mess)
{
	std::cout << "line: " << line << ":  error: " << mess << '\n';
}
