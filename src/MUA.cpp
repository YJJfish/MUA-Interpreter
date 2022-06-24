#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>
#include <math.h>
#include <random>
#include "Variable.h"

using namespace std;
Variable* InFunc = NULL;
//���������ֵ���, ���ó���
Trie* Tree[1024];
bool Return[1024];
int Last = 0;

string GetWord(istream& in, bool InList = false);
List ExtractList(istream& in);
Variable Eval(istream& in, bool Exec = true);

int main() {
	//���ó���
	Last = 0;
	Tree[0] = new Trie;
	Return[0] = false;
	Tree[0]->SetValue("pi", Variable(3.14159));
	while (!cin.eof() && !Return[0])
		Eval(cin);
}

string GetWord(istream& in, bool InList) {
	char ch;
	//���˿հ��ַ�
	ch = in.get();
	while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') ch = in.get();
	if (InList) {
		if (ch == '[') return "[";	//�����������, ֻ����������
		else if (ch == ']') return "]";	//������ҷ�����, ֻ�����ҷ�����
		string str = "\"";
		while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '[' && ch != ']' && ch != EOF) {
			str.append(1, ch);
			ch = in.get();
		}
		if (ch != EOF) in.putback(ch);
		return str;
	}
	else {
		//�ж϶�ȡ�����ַ�����
		if (ch == ':') return ":";		//�����ð��, ֻ����ð��
		else if (ch == '[') return "[";	//�����������, ֻ����������
		else if (ch == ']') return "]";	//������ҷ�����, ֻ�����ҷ�����
		else if (ch == '\"') {			//�����˫����, ��ȡ���հ��ַ�Ϊֹ, ���ǵ�ǰ����ֵ�������
			string str = "";
			while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != EOF) {
				str.append(1, ch);
				ch = in.get();
			}
			if (ch != EOF) in.putback(ch);
			return str;
		}
		else {							//����, ��ȡ��ð�Ż����ҷ����Ż�հ��ַ�Ϊֹ
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

//������������ȡ��
List ExtractList(istream& in) {
	string str;
	List lst;
	if (GetWord(in, true) != "[") throw logic_error("A list must begin with \"[");
	while ((str = GetWord(in, true)) != "]") {
		if (str == "") throw logic_error("Expected a right square bracket \"]\".");
		else if (str == "[") {//Ƕ�ױ�
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
	else if (cmd == "make") {//make <name> <value>����value�󶨵�name�ϣ��󶨺������λ�ڵ�ǰ�����ռ䣬����value
		string name = Eval(in).Word();
		Variable value = Eval(in);
		Tree[Last]->SetValue(name, value);
		return value;
	}
	else if (cmd == "thing" || cmd == ":") {//thing <name>������word���󶨵�ֵ; :<name>����thing��ͬ
		string name = Eval(in, false).Word();
		Variable value = Tree[Last]->GetValue(name);//�ȷ��ʱ���
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(name);//�ٷ��ʱհ�����
		if (value.VarType() == Variable::TYPE::NONE)
			value = Tree[0]->GetValue(name);//�ٷ���ȫ��
		return value;
	}
	else if (cmd == "print") {//print <value>�����value���������value
		Variable value = Eval(in);
		string String = value.Word();
		if (value.VarType() == Variable::LIST) String = String.substr(1, String.length() - 2);
		cout << String << endl;
		return value;
	}
	else if (cmd == "read") {//read������һ���ӱ�׼�����ȡ�����ֻ���
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
	else if (cmd[0] == '[') {//���������
		in.putback('[');
		Variable Rtn = ExtractList(in);
		if (Rtn.IsFunction() && Last)//���������һ������, �ҵ�ǰ��������, ���γɱհ�, ��Ҫ��¼����
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "erase") {//erase <name>�� ���word���󶨵�ֵ������ԭ�󶨵�ֵ
		string name = Eval(in).Word();
		return Tree[Last]->DeleteVariable(name);
	}
	else if (cmd == "isname") {//isname <word>������word�Ƿ���һ�����֣�true/false
		string word = Eval(in).Word();
		Variable value = Tree[Last]->GetValue(word);//�ȷ��ʱ���
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(word);//�ٷ��ʱհ�����
		if (value.VarType() == Variable::TYPE::NONE)
			value = Tree[0]->GetValue(word);//�ٷ���ȫ��
		return value.VarType() != Variable::TYPE::NONE;
	}
	else if (cmd == "run") {//run <list>������list�еĴ��룬����list��ִ�е����һ��op�ķ���ֵ
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
	else if (cmd == "if") {//if <bool> <list1> <list2>�����boolΪ�棬��ִ��list1������ִ��list2
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
	else if (cmd == "isnumber") {//isnumber <value>������value�Ƿ�������
		try {
			Eval(in).Number();
			return Variable(true);
		}catch(...){}
		return Variable(false);
	}
	else if (cmd == "isword") {//isword <value>������value�Ƿ�����
		return (Eval(in).VarType() == Variable::TYPE::WORD);
	}
	else if (cmd == "islist") {//islist <value>������value�Ƿ��Ǳ�
		return (Eval(in).VarType() == Variable::TYPE::LIST);
	}
	else if (cmd == "isbool") {//isbool <value>������value�Ƿ��ǲ�����
		try {
			Eval(in).Bool();
			return Variable(true);
		}
		catch (...) {}
		return Variable(false);
	}
	else if (cmd == "isempty") {//isempty <word|list>: ����word/list�Ƿ��ǿ���/���б�
		return (Eval(in).Length() == 0);
	}
	else if (cmd == "return") {//return <value>��ִֹͣ�к������趨valueΪ���ظ������ߵ�ֵ
		Variable Rtn = Eval(in);
		Return[Last] = true;
		return Rtn;
	}
	else if (cmd == "export") {//export <name>��������make�ı���<name>�����ȫ�֣���������ֵ
		string name = Eval(in).Word();
		Variable value = Tree[Last]->GetValue(name);//�ȷ��ʱ���
		if (value.VarType() == Variable::TYPE::NONE && InFunc)
			value = InFunc->GetVariable(name);//�ٷ��ʱհ�����
		Tree[0]->SetValue(name, value);
		return value;
	}
	else if (cmd == "readlist") {//readlist������һ���ӱ�׼�����ȡ��һ�У�����һ��������ÿ���Կո�ָ��Ĳ�����list��һ��Ԫ�أ�Ԫ�ص�����Ϊ��
		List lst;
		string Remaining, NextLine;
		getline(cin, Remaining);
		getline(cin, NextLine);
		for (int i = 0; i < Remaining.length(); i++)
			cin.putback(Remaining[i]);
		cin.putback('\n');
		stringstream(NextLine) >> lst;
		//ֻ�����ǵ���ı�, ��˲���Ҫ����Ƿ�Ϊ����
		return lst;
	}
	else if (cmd == "word") {//word <word> <word|number|bool>��������word�ϲ�Ϊһ��word���ڶ���ֵ������word��number��bool
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		if (value1.VarType() != Variable::WORD || (value2.VarType() != Variable::WORD && value2.VarType() != Variable::NUMBER && value2.VarType() != Variable::BOOL))
			throw logic_error("Invalid variable type");
		return  value1.Word() + value2.Word();
	}
	else if (cmd == "sentence") {//sentence <value1> <value2>����value1��value2�ϲ���һ��������ֵ��Ԫ�ز��У�value1����value2��ǰ��
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
	else if (cmd == "list") {//list <value1> <value2>��������ֵ�ϲ�Ϊһ�������ֵΪ���򲻴������
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		Variable Rtn = List().Append(value1).Append(value2);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "join") {//join <list> <value>����value��Ϊlist�����һ��Ԫ�ؼ��뵽list�У����value�Ǳ�������value��Ϊ������һ��Ԫ�أ�
		Variable value1 = Eval(in);
		Variable value2 = Eval(in);
		if (value1.VarType() != Variable::LIST)
			throw logic_error("The first operand must be a list");
		value1.GetList().Append(value2);
		if (value1.IsFunction() && Last)
			value1.SetContext(Tree + 1, Last);
		return value1;
	}
	else if (cmd == "first") {//first <word|list>������word�ĵ�һ���ַ�����list�ĵ�һ��Ԫ��
		Variable Rtn = Eval(in)[0];
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "last") {//last <word|list>������word�����һ���ַ���list�����һ��Ԫ��
		Variable Rtn = Eval(in)[-1];
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "butfirst") {//butfirst <word|list>�����س���һ��Ԫ����ʣ�µı������һ���ַ���ʣ�µ���
		Variable Rtn = Eval(in).Range(1);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "butlast") {//butlast <word|list>�����س����һ��Ԫ����ʣ�µı�������һ���ַ���ʣ�µ���
		Variable Rtn = Eval(in).Range(0, -1);
		if (Rtn.IsFunction() && Last)
			Rtn.SetContext(Tree + 1, Last);
		return Rtn;
	}
	else if (cmd == "random") {//random <number>������[0,number)��һ�������
		return (double)rand() / (RAND_MAX + 1) * Eval(in);
	}
	else if (cmd == "int") {//int <number>��floor the int
		return int(Eval(in).Number());
	}
	else if (cmd == "sqrt") {//sqrt <number>������number��ƽ����
		return sqrt(Eval(in).Number());
	}
	else if (cmd == "save") {//save <word>����Դ����ʽ���浱ǰ�����ռ���word�ļ��У������ļ���
		Variable FileName = Eval(in);
		Tree[Last]->Save(FileName.Word());
		return FileName;
	}
	else if (cmd == "load") {//load <word>����word�ļ���װ�����ݣ����뵱ǰ�����ռ䣬����true
		ifstream Fin(Eval(in).Word());
		if (Fin.fail()) return false;
		while (!Fin.eof() && !Return[Last])
			Eval(Fin);
		Fin.close();
		return true;
	}
	else if (cmd == "erall") {//erall�������ǰ�����ռ��ȫ�����ݣ�����true
		delete Tree[Last];
		Tree[Last] = new Trie;
		return true;
	}
	else {					//����������������
		Variable Rtn(cmd);
		if (Rtn.VarType() == Variable::NAME && Exec) {
			Variable Func = Tree[Last]->GetValue(Rtn.Word());//�ȷ��ʱ���
			if (Func.VarType() == Variable::TYPE::NONE && InFunc)
				Func = InFunc->GetVariable(Rtn.Word());//�ٷ��ʱհ�����
			if (Func.VarType() == Variable::TYPE::NONE)
				Func = Tree[0]->GetValue(Rtn.Word());//�ٷ���ȫ��
			if (!Func.IsFunction())
				throw logic_error("Only function can be called");
			//cout << ">>>Function body: " << Func.Word() << endl;
			//����������
			Trie* FuncVarTable = new Trie;
			//���ò���
			for (int i = 0; i < Func[0].Length(); i++)
				FuncVarTable->SetValue(Func[0][i].Word(), Eval(in));
			Tree[++Last] = FuncVarTable;
			Return[Last] = false;
			//���ñհ�����
			Variable* TempInFunc = InFunc;
			InFunc = &Func;
			string FuncContent = Func[1].Word();
			stringstream sstm(FuncContent.substr(1, FuncContent.length() - 2));
			Variable Rtn;
			while (!sstm.eof() && !Return[Last])
				Rtn = Eval(sstm);
			//��ԭ�հ��հ�����
			InFunc = TempInFunc;
			//ɾ��������
			delete Tree[Last--];
			return Rtn;
		}
		else return Rtn;
	}
}
