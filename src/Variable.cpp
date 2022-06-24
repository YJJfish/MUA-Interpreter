#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <sstream>
#include "Variable.h"
#include "Tools.hpp"

//列表类型定义
//构造函数
List::List(int Capacity) : Size(0), Capacity(Capacity), Arr(new Variable* [Capacity]) {}
//析构函数
List::~List(void) {
	for (int i = 0; i < Size; i++)
		delete Arr[i];
	delete[]Arr;
}
//拷贝构造
List::List(const List& lst) : Size(lst.Size), Capacity(lst.Capacity), Arr(new Variable* [lst.Capacity]) {
	for (int i = 0; i < this->Size; i++)
		this->Arr[i] = new Variable(*(lst.Arr[i]));
}
//赋值
const List& List::operator=(const List& lst) {
	if (lst.Arr == this->Arr) return *this;
	if (this->Capacity >= lst.Size) {
		for (int i = 0; i < this->Size; i++)
			delete Arr[i];
	}
	else {
		this->~List();
		this->Capacity = lst.Capacity;
		this->Arr = new Variable * [this->Capacity];
	}
	this->Size = lst.Size;
	for (int i = 0; i < this->Size; i++)
		this->Arr[i] = new Variable(*(lst.Arr[i]));
	return *this;
}
//索引
Variable& List::operator[](int i)  const {
	if (i < 0) i += this->Size;
	if (0 <= i && i < this->Size)
		return *(this->Arr[i]);
	else throw logic_error("Index out of range");
}
//长度
size_t List::Length(void)  const {
	return this->Size;
}
//添加元素
List& List::Append(const Variable& v) {
	if (this->Size == this->Capacity) this->Expand();
	this->Arr[(this->Size)++] = new Variable(v);
	return *this;
}
//合并列表
List operator+(const List& v1, const List& v2) {
	List Rtn(v1.Capacity + v2.Capacity);
	for (int i = 0; i < v1.Size; i++) Rtn.Append(v1[i]);
	for (int i = 0; i < v2.Size; i++) Rtn.Append(v2[i]);
	return Rtn;
}
//从输入流中读入列表, 列表元素为字, 按空格划分
istream& operator>>(istream& in, List& lst) {
	string str;
	while (!in.eof()) {
		str = "";
		in >> str;
		if (str == "") break;
		lst.Append(Variable("\"" + str));
	}
	return in;
}
//输出列表到输出流
ostream& operator<<(ostream& out, const List& lst) {
	out << "[";
	for (int i = 0; i < lst.Size; i++) {
		if (i) out << " ";
		out << *(lst.Arr[i]);
	}
	out << "]";
	return out;
}
//比较运算符
Variable operator==(const List& v1, const List& v2) {
	if (v1.Size != v2.Size) return Variable(false);
	for (int i = 0; i < v1.Size; i++)
		if (!((v1[i] == v2[i]).Bool())) return  Variable(false);
	return Variable(true);
}
Variable operator!=(const List& v1, const List& v2) {
	return !(v1 == v2);
}
//扩容
void List::Expand(void) {
	this->Capacity *= 2;
	Variable** ptr = new Variable * [this->Capacity];
	for (int i = 0; i < this->Size; i++)
		ptr[i] = this->Arr[i];
	delete[](this->Arr);
	this->Arr = ptr;
}




//通用变量类型定义
//默认构造函数
Variable::Variable(void) : Type(NONE), Value(NULL), Context(NULL) {}
//构造函数1：Number
Variable::Variable(double v) : Type(NUMBER), Value((void*)new double(v)), Context(NULL) {}
Variable::Variable(int v) : Type(NUMBER), Value((void*)new double(v)), Context(NULL) {}
//构造函数2：Word & Number/Bool & Name. 当字符串首字符是"时解释为Word, 否则再尝试解释为Number或Bool, 最后解释为Name
Variable::Variable(const string& v): Context(NULL) {
	//当首字符不是"时，优先将字符串解释为NUMBER和BOOL类型
	if (v[0] == '\"') {
		this->Type = WORD;
		this->Value = (void*)new string(v.substr(1));
	}
	else {
		if (v == "true" || v == "false") {
			this->Type = BOOL;
			this->Value = (void*)new bool((v == "true") ? true : false);
			return;
		}
		try {
			double num = String2Float(v);
			this->Type = NUMBER;
			this->Value = (void*)new double(num);
			return;
		}
		catch (...) {}
		this->Type = NAME;
		this->Value = (void*)new string(v);
	}
}
//构造函数3：List
Variable::Variable(const List& v) : Type(LIST), Value((void*)new List(v)), Context(NULL) {}
//构造函数4：Bool
Variable::Variable(bool v) : Type(BOOL), Value((void*)new bool(v)), Context(NULL) {}
//构造函数5：Word
Variable::Variable(char v) : Type(WORD), Value((void*)new string(" ")), Context(NULL) {
	(*((string*)this->Value))[0] = v;
}
//析构函数
Variable::~Variable(void) {
	switch (Type) {
	case NUMBER:
		delete (double*)Value;
		break;
	case BOOL:
		delete (bool*)Value;
		break;
	case WORD:
	case NAME:
		delete (string*)Value;
		break;
	case LIST:
		delete (List*)Value;
		break;
	default:
		break;
	}
	if (Context) delete Context;
}
//拷贝构造
Variable::Variable(const Variable& v) : Type(v.Type), Value(NULL), Context((v.Context) ? new Trie(*(v.Context)) : NULL) {
	//直接调用对应变量类型的拷贝构造函数
	switch (this->Type) {
	case NUMBER:
		this->Value = (void*) new double(*(double*)(v.Value));
		break;
	case BOOL:
		this->Value = (void*) new bool(*(bool*)(v.Value));
		break;
	case WORD:
	case NAME:
		this->Value = (void*) new string(*(string*)(v.Value));
		break;
	case LIST:
		this->Value = (void*) new List(*(List*)(v.Value));
		break;
	default:
		this->Value = NULL;
		break;
	}
}
//赋值
const Variable& Variable::operator=(const Variable& v) {
	if (this->Value == v.Value) return *this;
	this->~Variable();
	this->Type = v.Type;
	//直接调用对应变量类型的拷贝构造函数
	switch (this->Type) {
	case NUMBER:
		this->Value = (void*) new double(*(double*)(v.Value));
		break;
	case BOOL:
		this->Value = (void*) new bool(*(bool*)(v.Value));
		break;
	case WORD:
	case NAME:
		this->Value = (void*) new string(*(string*)(v.Value));
		break;
	case LIST:
		this->Value = (void*) new List(*(List*)(v.Value));
		break;
	default:
		this->Value = NULL;
		break;
	}
	//拷贝闭包环境
	this->Context = (v.Context) ? new Trie(*(v.Context)) : NULL;
	return *this;
}
//设置闭包环境
void Variable::SetContext(Trie* t[], int Len) {
	if (Context) delete Context;
	Context = new Trie;
	for (int i = 0; i < Len; i++)
		Context->Join(*(t[i]));
}
//从闭包中得到变量
Variable Variable::GetVariable(string Name) {
	if (this->Context)
		return this->Context->GetValue(Name);
	else return None;
}
//类型转换
double Variable::Number(void) const {
	switch (this->Type) {
	case NUMBER:
		return *(double*)(this->Value);
		break;
	case BOOL:
		throw logic_error("Expected a number");
		break;
	case WORD:
		return String2Float(*(string*)(this->Value));
		break;
	case LIST:
		throw logic_error("Expected a number");
		break;
	case NAME:
		throw logic_error("Expected a number");
		break;
	default:
		throw logic_error("Expected a number");
		break;
	}
}
string Variable::Word(void) const {
	stringstream sstm;
	string Rtn;
	sstm << *this;
	getline(sstm, Rtn);
	return Rtn;
}
bool Variable::Bool(void) const {
	switch (this->Type) {
	case NUMBER:
		throw logic_error("Cannot convert to bool");
		break;
	case BOOL:
		return *(bool*)(this->Value);
		break;
	case WORD:
		if ((*(string*)(this->Value)) == "true") return true;
		if ((*(string*)(this->Value)) == "false") return false;
		throw logic_error("Cannot convert to bool");
		break;
	case LIST:
		throw logic_error("Cannot convert to bool");
		break;
	case NAME:
		throw logic_error("Cannot convert to bool");
		break;
	default:
		throw logic_error("Cannot convert to bool");
		break;
	}
}
List& Variable::GetList(void) const {
	if (this->Type != LIST)
		throw logic_error("Expected a list");
	return *(List*)this->Value;
}
//表或字的长度
size_t Variable::Length(void) const {
	switch (this->Type) {
	case NUMBER:
		throw logic_error("Expected a word or a list");
		break;
	case BOOL:
		throw logic_error("Expected a word or a list");
		break;
	case WORD:
		return (*(string*)this->Value).length();
		break;
	case LIST:
		return (*(List*)this->Value).Length();
		break;
	case NAME:
		throw logic_error("Expected a word or a list");
		break;
	default:
		throw logic_error("Expected a word or a list");
		break;
	}
}
//表或字的元素
Variable Variable::operator[](int i) {
	if (this->Type == LIST)
		return (*(List*)this->Value)[i];
	else if (this->Type == WORD || this->Type == NUMBER || this->Type == BOOL) {
		string String = this->Word();
		size_t Len = String.length();
		if (i < 0) i += Len;
		if (i < 0 || i >= Len)
			throw logic_error("Index out of range");
		return String[i];
	}
	else
		throw logic_error("Expected a list or a word");
}
//表的元素
Variable& Variable::operator[](int i) const{
	if (this->Type == LIST)
		return (*(List*)this->Value)[i];
	else
		throw logic_error("Expected a list or a word");
}
//子表/子字
Variable Variable::Range(int i, int j) const {
	if (this->Type == LIST) {
		size_t Len = (*(List*)this->Value).Length();
		if (i < 0) i += Len;
		if (i < 0) i = 0;
		else if (i > Len) i = Len;
		if (j < 0) j += Len;
		if (j < 0) j = 0;
		else if (j > Len) j = Len;
		List Rtn;
		for (int Index = i; Index < j; Index++)
			Rtn.Append((*(List*)this->Value)[Index]);
		return Rtn;
	}
	else if (this->Type == WORD || this->Type == NUMBER || this->Type == BOOL) {
		string String = this->Word();
		size_t Len = String.length();
		if (i < 0) i += Len;
		if (i < 0) i = 0;
		else if (i > Len) i = Len;
		if (j < 0) j += Len;
		if (j < 0) j = 0;
		else if (j > Len) j = Len;
		return String.substr(i, j - i);
	}
	else
		throw logic_error("Expected a list or a word");
}
//判断类型
enum Variable::TYPE Variable::VarType(void) const {
	return this->Type;
}
//判断是否是函数
bool Variable::IsFunction(void) const {
	return (this->Type == LIST) && ((*(List*)this->Value).Length() == 2) && ((*(List*)this->Value)[0].Type == LIST) && ((*(List*)this->Value)[1].Type == LIST);
}
//从输入流中读入数字或字
istream& operator>>(istream& in, Variable& v) {
	string str;
	in >> str;
	try {
		double num = String2Float(str);
		v = Variable(num);
	}
	catch (...) {
		v = Variable("\"" + str);
	}
	return in;
}
//输出变量到输出流
ostream& operator<<(ostream& out, const Variable& v) {
	switch (v.Type) {
	case Variable::NUMBER:
		out << *(double*)v.Value;
		break;
	case Variable::BOOL:
		out << ((*(bool*)v.Value) ? "true" : "false");
		break;
	case Variable::WORD:
	case Variable::NAME:
		out << *(string*)v.Value;
		break;
	case Variable::LIST:
		out << *(List*)v.Value;
		break;
	default:
		throw logic_error("Cannot print NONE variable");
		break;
	}
	return out;
}
//算数运算
Variable operator+(const Variable& v1, const Variable& v2) {
	return Variable(v1.Number() + v2.Number());
}
Variable operator-(const Variable& v1, const Variable& v2) {
	return Variable(v1.Number() - v2.Number());
}
Variable operator*(const Variable& v1, const Variable& v2) {
	return Variable(v1.Number() * v2.Number());
}
Variable operator/(const Variable& v1, const Variable& v2) {
	return Variable(v1.Number() / v2.Number());
}
Variable operator%(const Variable& v1, const Variable& v2) {
	double n1 = v1.Number(), n2 = v2.Number();
	return Variable(n1 - (int)(n1 / n2) * n2);
}
//逻辑运算
Variable operator&&(const Variable& v1, const Variable& v2) {
	if (v1.Bool()) return v2;
	else return Variable(false);
}
Variable operator||(const Variable& v1, const Variable& v2) {
	if (v1.Bool()) return Variable(true);
	else return v2;
}
Variable operator!(const Variable& v) {
	return Variable(!v.Bool());
}
//大小比较
Variable operator>(const Variable& v1, const Variable& v2) {
	try {
		return Variable(v1.Number() > v2.Number());
	}
	catch (...) {}
	return Variable(v1.Word() > v2.Word());
}
Variable operator<(const Variable& v1, const Variable& v2) {
	try {
		return Variable(v1.Number() < v2.Number());
	}
	catch (...) {};
		return Variable(v1.Word() < v2.Word());
}
Variable operator==(const Variable& v1, const Variable& v2) {
	try {
		return Variable(v1.Number() == v2.Number());
	}
	catch (...) {};
	return Variable(v1.Word() == v2.Word());
}
Variable operator!=(const Variable& v1, const Variable& v2) {
	return !(v1 == v2);
}


//字典树，记录变量的值
//结点构造函数
Trie::Node::Node(void) : Value() {
	for (int i = 0; i < 63; i++)
		Child[i] = NULL;
}
Trie::Node::Node(const Variable& v) : Value(v) {
	for (int i = 0; i < 63; i++)
		Child[i] = NULL;
}
//结点拷贝构造函数
Trie::Node::Node(const Node& n): Value(n.Value) {
	for (int i = 0; i < 63; i++)
		if (n.Child[i] == NULL) this->Child[i] = NULL;
		else this->Child[i] = new Node(*n.Child[i]);
}
//结点析构函数
Trie::Node::~Node(void) {
	for (int i = 0; i < 63; i++)
		if (Child[i]) delete Child[i];
}
//合并以结点为根的子树
void Trie::Node::Join(const Node& n, bool Overwrite) {
	if (n.Value.VarType() != Variable::NONE && (Overwrite || this->Value.VarType() == Variable::NONE))
		this->Value = n.Value;
	for (int i = 0; i < 63; i++)
		if (n.Child[i]) {
			if (this->Child[i] == NULL) this->Child[i] = new Node(*n.Child[i]);
			else this->Child[i]->Join(*(n.Child[i]), Overwrite);
		}
}
//保存到外部文件
void Trie::Node::Save(string VarName, ostream& Fout) {
	if (this->Value.VarType() != Variable::NONE)
		Fout << "make \"" << VarName << " " << this->Value << endl;
	for (int i = 0; i < 63; i++)
		if (this->Child[i]) {
			string NextChar = " ";
			NextChar[0] = (i <= 9) ? i + '0' : (i <= 35) ? i - 10 + 'a' : (i <= 61) ? i - 36 + 'A' : '_';
			this->Child[i]->Save(VarName + NextChar, Fout);
		}
}
//构造函数
Trie::Trie(void) {}
//拷贝构造函数
Trie::Trie(const Trie& Tree): Root(Tree.Root) {}
//析构函数
Trie::~Trie(void) {}
//读取变量的值, 若变量不存在则返回NONE
Variable Trie::GetValue(string Name) {
	return this->SetValue(Name, None, 1);
}
//删除变量, 返回旧值的地址, 若变量不存在则返回NONE
Variable Trie::DeleteVariable(string Name) {
	return this->SetValue(Name, None, 2);
}
//Operation=0: 创建新变量, 若变量不存在则返回NONE; 若变量已存在, 则返回旧值
//Operation=1: 读取变量的值, 若变量不存在则返回NONE
//Operation=2: 删除变量, 返回旧值, 若变量不存在则返回NONE
Variable Trie::SetValue(string Name, const Variable& Value, int Operation) {
	Node* ptr = &Root;
	Variable Rtn;
	int Index;
	for (unsigned int i = 0; i < Name.length(); i++) {
		//确定下一个子结点的编号
		if (Name[i] >= '0' && Name[i] <= '9')
			Index = Name[i] - '0';
		else if (Name[i] >= 'a' && Name[i] <= 'z')
			Index = 10 + Name[i] - 'a';
		else if (Name[i] >= 'A' && Name[i] <= 'Z')
			Index = 10 + 26 + Name[i] - 'A';
		else if (Name[i] == '_')
			Index = 10 + 26 + 26;
		else throw logic_error("Invalid name");
		//如果字符串内的字符还没遍历完, 就到了叶节点
		if (ptr->Child[Index] == NULL)
			switch (Operation) {
			case 0:
				ptr = ptr->Child[Index] = new Node;
				break;
			case 1:
			case 2:
				return None;
			}
		else ptr = ptr->Child[Index];
	}
	//现在, ptr指向目标结点
	switch (Operation) {
	case 0:
		Rtn = ptr->Value;
		ptr->Value = Value;
		return Rtn;
		break;
	case 1:
		return ptr->Value;
	case 2:
		Rtn = ptr->Value;
		ptr->Value = Variable();
		return Rtn;
	}
}
//合并
void Trie::Join(const Trie& t) {
	this->Root.Join(t.Root);
}
//保存到外部文件
void Trie::Save(string FileName) {
	ofstream Fout(FileName);
	this->Root.Save("", Fout);
	Fout.close();
}