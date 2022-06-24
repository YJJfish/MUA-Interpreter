#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>
#include <math.h>
#include <random>
#include "Variable.h"

using namespace std;
Variable* InFunc = NULL;
//建立变量字典树, 设置常量
Trie* Tree[1024];
bool Return[1024];
int Last = 0;

string GetWord(istream& in, bool InList = false);
List ExtractList(istream& in);
Variable Eval(istream& in, bool Exec = true);

int main() {
	//设置常量
	Last = 0;
	Tree[0] = new Trie;
	Return[0] = false;
	Tree[0]->SetValue("pi", Variable(3.14159));
	while (!cin.eof() && !Return[0])
		Eval(cin);
}

string GetWord(istream& in, bool InList) {
	char ch;
	//过滤空白字符
	ch = in.get();
	while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') ch = in.get();
	if (InList) {
		if (ch == '[') return "[";	//如果是左方括号, 只返回左方括号
		else if (ch == ']') return "]";	//如果是右方括号, 只返回右方括号
		string str = "\"";
		while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '[' && ch != ']' && ch != EOF) {
			str.append(1, ch);
			ch = in.get();
		}
		if (ch != EOF) in.putback(ch);
		return str;
	}
	else {
		//判断读取到的字符类型
		if (ch == ':') return ":";		//如果是冒号, 只返回冒号
		else if (ch == '[') return "[";	//如果是左方括号, 只返回左方括号
		else if (ch == ']') return "]";	//如果是右方括号, 只返回右方括号
		else if (ch == '\"') {			//如果是双引号, 读取到空白字符为止, 都是当前这个字的字面量
			string str = "";
			while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != EOF) {
				str.append(1, ch);
				ch = in.get();
			}
			if (ch != EOF) in.putback(ch);
			return str;
		}
		else {							//否则, 读取到冒号或左右方括号或空白字符为止
			string str = "";
			while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '[' && ch != ']' && ch != ':' && ch != EOF) {
				str.append(1, ch);
				ch = in.get();
			}
			if (ch != EOF) in.putback(ch);
			return str;
		}
	}
}

//从输入流中提取表
List ExtractList(istream& in) {
	string str;
	List lst;
	if (GetWord(in, true) != "[") throw logic_error("A list must begin with \"[");
	while ((str = GetWord(in, true)) != "]") {
		if (str == "") throw logic_error("Expected a right square bracket \"]\".");
		else if (str == "[") {//嵌套表
			in.putback('[');
			lst.Append(Variable(ExtractList(in)));
		}
		else {
			Variable value = Variable(str.substr(1));
			if (value.VarType() == Variable::TYPE::NUMBER || value.VarType() == Variable::TYPE::BOOL)
				lst.Append(value);
			else
				lst.Append(Variable(str));
		}
	}
	return lst;
}


Variable Eval(istream& in, bool Exec) {
	string cmd;
	cmd = GetWord(in);
	if (cmd == "") return None;
	else if (cmd == "make") {//make <name> <value>：将value绑定到name上，绑定后的名字位于当前命名空间，返回value
		string name = Eval(in).Word();
		Variable value = Eval(in);
		Tree[Last]->SetValue(name, value);
		return value;
	}
	else if (cmd == "thing" || cmd == ":") {//thing <name>：返回word所绑定的值; :<name>：与thing相同
		string name = Eval(in, false).Word();
		Variable value = Tree[Last]->GetValue(name);//先访问本地
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(name);//再访问闭包环境
		if (value.VarType() == Variable::TYPE::NONE)
			value = Tree[0]->GetValue(name);//再访问全局
		return value;
	}
	else if (cmd == "print") {//print <value>：输出value，返回这个value
		Variable value = Eval(in);
		string String = value.Word();
		if (value.VarType() == Variable::LIST) String = String.substr(1, String.length() - 2);
		cout << String << endl;
		return value;
	}
	else if (cmd == "read") {//read：返回一个从标准输入读取的数字或字
		Variable value;
		string Remaining, NextLine;
		getline(cin, Remaining);
		getline(cin, NextLine);
		cin.putback('\n');
		for (int i = Remaining.length() - 1; i >= 0; i--)
			cin.putback(Remaining[i]);
		stringstream(NextLine) >> value;
		return value;
	}
	else if (cmd == "add") {//add <number> <number>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 + number2;
	}
	else if (cmd == "sub") {//sub <number> <number>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 - number2;
	}
	else if (cmd == "mul") {//mul <number> <number>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 * number2;
	}
	else if (cmd == "div") {//div <number> <number>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 / number2;
	}
	else if (cmd == "mod") {//mod <number> <number>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 % number2;
	}
	else if (cmd[0] == '[') {//表的字面量
		in.putback('[');
		Variable Rtn = ExtractList(in);
		if (Rtn.IsFunction() && Last)//如果定义了一个函数, 且当前非主函数, 则形成闭包, 需要记录环境
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "erase") {//erase <name>： 清除word所绑定的值，返回原绑定的值
		string name = Eval(in).Word();
		return Tree[Last]->DeleteVariable(name);
	}
	else if (cmd == "isname") {//isname <word>：返回word是否是一个名字，true/false
		string word = Eval(in).Word();
		Variable value = Tree[Last]->GetValue(word);//先访问本地
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(word);//再访问闭包环境
		if (value.VarType() == Variable::TYPE::NONE)
			value = Tree[0]->GetValue(word);//再访问全局
		return value.VarType() != Variable::TYPE::NONE;
	}
	else if (cmd == "run") {//run <list>：运行list中的代码，返回list中执行的最后一个op的返回值
		Variable list = Eval(in);
		if (list.VarType() != Variable::TYPE::LIST)
			throw logic_error("Expected a list variable.");
		string line = list.Word();
		stringstream sstm(line.substr(1, line.length() - 2));
		Variable Rtn = List();
		while (!sstm.eof() && !Return[Last])
			Rtn = Eval(sstm);
		return Rtn;
	}
	else if (cmd == "eq") {//eq <number|word> <number|word>
		string word1 = Eval(in).Word();
		string word2 = Eval(in).Word();
		return word1 == word2;
	}
	else if (cmd == "gt") {//gt <number|word> <number|word>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 > number2;
	}
	else if (cmd == "lt") {//lt <number|word> <number|word>
		Variable number1 = Eval(in);
		Variable number2 = Eval(in);
		return number1 < number2;
	}
	else if (cmd == "and") {//and <bool> <bool>
		Variable bool1 = Eval(in);
		Variable bool2 = Eval(in);
		return (bool1 && bool2).Bool();
	}
	else if (cmd == "or") {//or <bool> <bool>
		Variable bool1 = Eval(in);
		Variable bool2 = Eval(in);
		return (bool1 || bool2).Bool();
	}
	else if (cmd == "not") {//not <bool>
		Variable bool1 = Eval(in);
		return !bool1;
	}
	else if (cmd == "if") {//if <bool> <list1> <list2>：如果bool为真，则执行list1，否则执行list2
		Variable bool1 = Eval(in);
		Variable list1 = Eval(in);
		Variable list2 = Eval(in);
		if (list1.VarType() != Variable::TYPE::LIST || list2.VarType() != Variable::TYPE::LIST)
			throw logic_error("Expected a list variable.");
		string line;
		if (bool1.Bool())
			line = list1.Word();
		else
			line = list2.Word();
		stringstream sstm(line.substr(1, line.length() - 2));
		Variable Rtn = List();
		while (!sstm.eof() && !Return[Last])
			Rtn = Eval(sstm);
		return Rtn;
	}
	else if (cmd == "isnumber") {//isnumber <value>：返回value是否是数字
		try {
			Eval(in).Number();
			return Variable(true);
		}catch(...){}
		return Variable(false);
	}
	else if (cmd == "isword") {//isword <value>：返回value是否是字
		return (Eval(in).VarType() == Variable::TYPE::WORD);
	}
	else if (cmd == "islist") {//islist <value>：返回value是否是表
		return (Eval(in).VarType() == Variable::TYPE::LIST);
	}
	else if (cmd == "isbool") {//isbool <value>：返回value是否是布尔量
		try {
			Eval(in).Bool();
			return Variable(true);
		}
		catch (...) {}
		return Variable(false);
	}
	else if (cmd == "isempty") {//isempty <word|list>: 返回word/list是否是空字/空列表
		return (Eval(in).Length() == 0);
	}
	else if (cmd == "return") {//return <value>：停止执行函数，设定value为返回给调用者的值
		Variable Rtn = Eval(in);
		Return[Last] = true;
		return Rtn;
	}
	else if (cmd == "export") {//export <name>：将本地make的变量<name>输出到全局，返回它的值
		string name = Eval(in).Word();
		Variable value = Tree[Last]->GetValue(name);//先访问本地
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(name);//再访问闭包环境
		Tree[0]->SetValue(name, value);
		return value;
	}
	else if (cmd == "readlist") {//readlist：返回一个从标准输入读取的一行，构成一个表，行中每个以空格分隔的部分是list的一个元素，元素的类型为字
		List lst;
		string Remaining, NextLine;
		getline(cin, Remaining);
		getline(cin, NextLine);
		for (int i = 0; i < Remaining.length(); i++)
			cin.putback(Remaining[i]);
		cin.putback('\n');
		stringstream(NextLine) >> lst;
		//只可能是单层的表, 因此不需要检测是否为函数
		return lst;
	}
	else if (cmd == "word") {//word <word> <word|number|bool>：将两个word合并为一个word，第二个值可以是word、number或bool
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		if (value1.VarType() != Variable::WORD || (value2.VarType() != Variable::WORD && value2.VarType() != Variable::NUMBER && value2.VarType() != Variable::BOOL))
			throw logic_error("Invalid variable type");
		return  value1.Word() + value2.Word();
	}
	else if (cmd == "sentence") {//sentence <value1> <value2>：将value1和value2合并成一个表，两个值的元素并列，value1的在value2的前面
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		List lst;
		if (value1.VarType() == Variable::LIST)
			lst = value1.GetList();
		else lst.Append(value1);
		if (value2.VarType() == Variable::LIST)
			lst = lst + value2.GetList();
		else lst.Append(value2);
		Variable Rtn = lst;
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "list") {//list <value1> <value2>：将两个值合并为一个表，如果值为表，则不打开这个表
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		Variable Rtn = List().Append(value1).Append(value2);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "join") {//join <list> <value>：将value作为list的最后一个元素加入到list中（如果value是表，则整个value成为表的最后一个元素）
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		if (value1.VarType() != Variable::LIST)
			throw logic_error("The first operand must be a list");
		value1.GetList().Append(value2);
		if (value1.IsFunction() && Last)
			value1.SetContext(Tree + 1, Last);
		return value1;
	}
	else if (cmd == "first") {//first <word|list>：返回word的第一个字符，或list的第一个元素
		Variable Rtn = Eval(in)[0];
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "last") {//last <word|list>：返回word的最后一个字符，list的最后一个元素
		Variable Rtn = Eval(in)[-1];
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "butfirst") {//butfirst <word|list>：返回除第一个元素外剩下的表，或除第一个字符外剩下的字
		Variable Rtn = Eval(in).Range(1);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "butlast") {//butlast <word|list>：返回除最后一个元素外剩下的表，或除最后一个字符外剩下的字
		Variable Rtn = Eval(in).Range(0, -1);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "random") {//random <number>：返回[0,number)的一个随机数
		return (double)rand() / (RAND_MAX + 1) * Eval(in);
	}
	else if (cmd == "int") {//int <number>：floor the int
		return int(Eval(in).Number());
	}
	else if (cmd == "sqrt") {//sqrt <number>：返回number的平方根
		return sqrt(Eval(in).Number());
	}
	else if (cmd == "save") {//save <word>：以源码形式保存当前命名空间在word文件中，返回文件名
		Variable FileName = Eval(in);
		Tree[Last]->Save(FileName.Word());
		return FileName;
	}
	else if (cmd == "load") {//load <word>：从word文件中装载内容，加入当前命名空间，返回true
		ifstream Fin(Eval(in).Word());
		if (Fin.fail()) return false;
		while (!Fin.eof() && !Return[Last])
			Eval(Fin);
		Fin.close();
		return true;
	}
	else if (cmd == "erall") {//erall：清除当前命名空间的全部内容，返回true
		delete Tree[Last];
		Tree[Last] = new Trie;
		return true;
	}
	else {					//其他字面量或名字
		Variable Rtn(cmd);
		if (Rtn.VarType() == Variable::NAME && Exec) {
			Variable Func = Tree[Last]->GetValue(Rtn.Word());//先访问本地
			if (Func.VarType() == Variable::TYPE::NONE && InFunc)
				Func = InFunc->GetVariable(Rtn.Word());//再访问闭包环境
			if (Func.VarType() == Variable::TYPE::NONE)
				Func = Tree[0]->GetValue(Rtn.Word());//再访问全局
			if (!Func.IsFunction())
				throw logic_error("Only function can be called");
			//cout << ">>>Function body: " << Func.Word() << endl;
			//创建变量树
			Trie* FuncVarTable = new Trie;
			//设置参数
			for (int i = 0; i < Func[0].Length(); i++)
				FuncVarTable->SetValue(Func[0][i].Word(), Eval(in));
			Tree[++Last] = FuncVarTable;
			Return[Last] = false;
			//设置闭包环境
			Variable* TempInFunc = InFunc;
			InFunc = &Func;
			string FuncContent = Func[1].Word();
			stringstream sstm(FuncContent.substr(1, FuncContent.length() - 2));
			Variable Rtn;
			while (!sstm.eof() && !Return[Last])
				Rtn = Eval(sstm);
			//还原闭包闭包环境
			InFunc = TempInFunc;
			//删除变量树
			delete Tree[Last--];
			return Rtn;
		}
		else return Rtn;
	}
}
