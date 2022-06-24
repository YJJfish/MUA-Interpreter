#pragma once
#include <iostream>
using namespace std;

#define None Variable()

//列表类型定义
class List;
//通用变量类型定义
class Variable;
//字典树变量类型定义
class Trie;


//列表类型定义
class List {
public:
	//构造函数
	List(int Capacity = 16);
	//析构函数
	~List(void);
	//拷贝构造
	List(const List& lst);
	//赋值
	const List& operator=(const List& lst);
	//索引
	Variable& operator[](int i) const;
	//长度
	size_t Length(void) const;
	//添加元素
	List& Append(const Variable& v);
	//合并列表
	friend List operator+(const List& v1, const List& v2);
	//输入运算符
	friend istream& operator>>(istream& in, List& lst);
	//输出运算符
	friend ostream& operator<<(ostream& out, const List& lst);
	//比较运算符
	friend Variable operator==(const List& v1, const List& v2);
	friend Variable operator!=(const List& v1, const List& v2);
private:
	Variable** Arr;
	int Size;
	int Capacity;

	//扩容
	void Expand(void);
};


//通用变量类型定义
class Variable {
public:
	//变量类型定义
	enum TYPE { NUMBER, WORD, LIST, BOOL, NAME, NONE };
	//默认构造函数, 变量类型为NONE
	Variable(void);
	//构造函数1：Number
	Variable(double v);
	Variable(int v);
	//构造函数2：Word & Number/Bool & Name. 当字符串首字符是"时解释为Word, 否则再尝试解释为Number或Bool, 最后解释为Name
	Variable(const string& v);
	//构造函数3：List
	Variable(const List& v);
	//构造函数4：Bool
	Variable(bool v);
	//构造函数5：Word
	Variable(char v);
	//析构函数
	~Variable(void);
	//拷贝构造
	Variable(const Variable& v);
	//赋值
	const Variable& operator=(const Variable& v);
	//设置闭包环境
	void SetContext(Trie* t[], int Len);
	//从闭包中得到变量
	Variable GetVariable(string Name);
	//类型转换
	double Number(void) const;
	string Word(void) const;
	bool Bool(void) const;
	List& GetList(void) const;
	//表或字的长度
	size_t Length(void) const;
	//表或字的元素
	Variable operator[](int i);
	//表的元素
	Variable& operator[](int i) const;
	//子表/子字
	Variable Range(int i, int j = 2147483647L) const;
	//判断类型
	enum TYPE VarType(void) const;
	//判断是否是函数
	bool IsFunction(void) const;
	//输入运算符
	friend istream& operator>>(istream& in, Variable& v);
	//输出运算符
	friend ostream& operator<<(ostream& out, const Variable& v);
	//算数运算
	friend Variable operator+(const Variable& v1, const Variable& v2);
	friend Variable operator-(const Variable& v1, const Variable& v2);
	friend Variable operator*(const Variable& v1, const Variable& v2);
	friend Variable operator/(const Variable& v1, const Variable& v2);
	friend Variable operator%(const Variable& v1, const Variable& v2);
	//逻辑运算
	friend Variable operator&&(const Variable& v1, const Variable& v2);
	friend Variable operator||(const Variable& v1, const Variable& v2);
	friend Variable operator!(const Variable& v);
	//大小比较
	friend Variable operator>(const Variable& v1, const Variable& v2);
	friend Variable operator<(const Variable& v1, const Variable& v2);
	friend Variable operator==(const Variable& v1, const Variable& v2);
	friend Variable operator!=(const Variable& v1, const Variable& v2);
private:
	TYPE Type;
	void* Value;
	Trie* Context;//若变量是函数内定义的函数, 则还要记录其环境
};


//字典树，记录变量的值
class Trie {
private:
	//结点类型定义
	class Node {
	public:
		//子结点
		Node* Child[10 + 26 + 26 + 1];
		//结点值
		Variable Value;
		//结点构造函数
		Node(void);
		Node(const Variable&);
		//结点拷贝构造函数
		Node(const Node&);
		//结点析构函数
		~Node(void);
		//合并以结点为根的子树
		void Join(const Node& n, bool Overwrite = true);
		//保存到外部文件
		void Save(string VarName, ostream& Fout);
	};
	Node Root;
public:
	//构造函数
	Trie(void);
	//拷贝构造函数
	Trie(const Trie&);
	//析构函数
	~Trie(void);
	//读取变量的值, 若变量不存在则返回NONE
	Variable GetValue(string Name);
	//删除变量, 返回旧值的地址, 若变量不存在则返回NONE
	Variable DeleteVariable(string Name);
	//Operation=0: 创建新变量, 若变量不存在则返回NONE; 若变量已存在, 则返回旧值
	//Operation=1: 读取变量的值, 若变量不存在则返回NONE
	//Operation=2: 删除变量, 返回旧值, 若变量不存在则返回NONE
	Variable SetValue(string Name, const Variable& Value, int Operation = 0);
	//合并
	void Join(const Trie& t);
	//保存到外部文件
	void Save(string FileName);
};