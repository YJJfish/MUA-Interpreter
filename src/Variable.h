#pragma once
#include <iostream>
using namespace std;

#define None Variable()

//�б����Ͷ���
class List;
//ͨ�ñ������Ͷ���
class Variable;
//�ֵ����������Ͷ���
class Trie;


//�б����Ͷ���
class List {
public:
	//���캯��
	List(int Capacity = 16);
	//��������
	~List(void);
	//��������
	List(const List& lst);
	//��ֵ
	const List& operator=(const List& lst);
	//����
	Variable& operator[](int i) const;
	//����
	size_t Length(void) const;
	//���Ԫ��
	List& Append(const Variable& v);
	//�ϲ��б�
	friend List operator+(const List& v1, const List& v2);
	//���������
	friend istream& operator>>(istream& in, List& lst);
	//��������
	friend ostream& operator<<(ostream& out, const List& lst);
	//�Ƚ������
	friend Variable operator==(const List& v1, const List& v2);
	friend Variable operator!=(const List& v1, const List& v2);
private:
	Variable** Arr;
	int Size;
	int Capacity;

	//����
	void Expand(void);
};


//ͨ�ñ������Ͷ���
class Variable {
public:
	//�������Ͷ���
	enum TYPE { NUMBER, WORD, LIST, BOOL, NAME, NONE };
	//Ĭ�Ϲ��캯��, ��������ΪNONE
	Variable(void);
	//���캯��1��Number
	Variable(double v);
	Variable(int v);
	//���캯��2��Word & Number/Bool & Name. ���ַ������ַ���"ʱ����ΪWord, �����ٳ��Խ���ΪNumber��Bool, ������ΪName
	Variable(const string& v);
	//���캯��3��List
	Variable(const List& v);
	//���캯��4��Bool
	Variable(bool v);
	//���캯��5��Word
	Variable(char v);
	//��������
	~Variable(void);
	//��������
	Variable(const Variable& v);
	//��ֵ
	const Variable& operator=(const Variable& v);
	//���ñհ�����
	void SetContext(Trie* t[], int Len);
	//�ӱհ��еõ�����
	Variable GetVariable(string Name);
	//����ת��
	double Number(void) const;
	string Word(void) const;
	bool Bool(void) const;
	List& GetList(void) const;
	//����ֵĳ���
	size_t Length(void) const;
	//����ֵ�Ԫ��
	Variable operator[](int i);
	//���Ԫ��
	Variable& operator[](int i) const;
	//�ӱ�/����
	Variable Range(int i, int j = 2147483647L) const;
	//�ж�����
	enum TYPE VarType(void) const;
	//�ж��Ƿ��Ǻ���
	bool IsFunction(void) const;
	//���������
	friend istream& operator>>(istream& in, Variable& v);
	//��������
	friend ostream& operator<<(ostream& out, const Variable& v);
	//��������
	friend Variable operator+(const Variable& v1, const Variable& v2);
	friend Variable operator-(const Variable& v1, const Variable& v2);
	friend Variable operator*(const Variable& v1, const Variable& v2);
	friend Variable operator/(const Variable& v1, const Variable& v2);
	friend Variable operator%(const Variable& v1, const Variable& v2);
	//�߼�����
	friend Variable operator&&(const Variable& v1, const Variable& v2);
	friend Variable operator||(const Variable& v1, const Variable& v2);
	friend Variable operator!(const Variable& v);
	//��С�Ƚ�
	friend Variable operator>(const Variable& v1, const Variable& v2);
	friend Variable operator<(const Variable& v1, const Variable& v2);
	friend Variable operator==(const Variable& v1, const Variable& v2);
	friend Variable operator!=(const Variable& v1, const Variable& v2);
private:
	TYPE Type;
	void* Value;
	Trie* Context;//�������Ǻ����ڶ���ĺ���, ��Ҫ��¼�价��
};


//�ֵ�������¼������ֵ
class Trie {
private:
	//������Ͷ���
	class Node {
	public:
		//�ӽ��
		Node* Child[10 + 26 + 26 + 1];
		//���ֵ
		Variable Value;
		//��㹹�캯��
		Node(void);
		Node(const Variable&);
		//��㿽�����캯��
		Node(const Node&);
		//�����������
		~Node(void);
		//�ϲ��Խ��Ϊ��������
		void Join(const Node& n, bool Overwrite = true);
		//���浽�ⲿ�ļ�
		void Save(string VarName, ostream& Fout);
	};
	Node Root;
public:
	//���캯��
	Trie(void);
	//�������캯��
	Trie(const Trie&);
	//��������
	~Trie(void);
	//��ȡ������ֵ, �������������򷵻�NONE
	Variable GetValue(string Name);
	//ɾ������, ���ؾ�ֵ�ĵ�ַ, �������������򷵻�NONE
	Variable DeleteVariable(string Name);
	//Operation=0: �����±���, �������������򷵻�NONE; �������Ѵ���, �򷵻ؾ�ֵ
	//Operation=1: ��ȡ������ֵ, �������������򷵻�NONE
	//Operation=2: ɾ������, ���ؾ�ֵ, �������������򷵻�NONE
	Variable SetValue(string Name, const Variable& Value, int Operation = 0);
	//�ϲ�
	void Join(const Trie& t);
	//���浽�ⲿ�ļ�
	void Save(string FileName);
};