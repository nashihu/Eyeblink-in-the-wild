#include <iostream>
#include <fstream>//�ļ���ͷ�ļ�����д�ļ�ʱ��



int main()
{
	ofstream fin("D:\\data.txt");
	if (!fin)
	{
		cerr << "�ļ���ʧ��" << endl;
		return -1;
	}
	int a = 0, b = 0;//������ֵ
	char c;//���ܶ���
	//����int���Ͷ��룬����������ʱֹͣ��һ�ζ�ȡ��
	//Ȼ��","����c��Ȼ���ȡ�����������Ϊb
	fin << a;
	fin << " ";
	fin << b;
	return 0;

}