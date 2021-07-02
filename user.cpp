#include<stdlib.h>
#include <conio.h>
#include<iostream>
#include<fstream>
#include<string.h>
using namespace std;

const char* USER_FILE = "user.dat";
class User {
	public:
		char usr[10]; // �û��� 
		char pwd[10]; // ���� 
		User *next;
		User() {};
		User(char usr[10], char pwd[10]) {
			strcpy(this->usr, usr);
			strcpy(this->pwd, pwd);
			this->next = NULL; 
		};
		bool login(char *usr, char *pwd) {
			return this->currentUser(usr) && strcmp(this->pwd, pwd) == 0;
		};
		bool currentUser(char *usr) {
			return strcmp(this->usr, usr) == 0;
		}
};

User* loginForm();
/** �ͷ��û����� */
void releaseUser(User* user);
/** �����û���Ϣ */
User* loadUser();
void saveUser(User *user);
/** ע�� */
void regist();
/** ��¼ */
User* login();

void inputPwd(char *p) {
	while(*p=getch()) {
		if(*p == 0x0d) {
			*p = '\0'; //������Ļس���ת���ɿո�
			break;
		}
		printf("*");    //�������������"*"����ʾ
		p++;
	}
	cout << endl;
}

User* loginForm() {
	char choice, temp[10];
	User *current = NULL;
	cout << "       ϵͳ��¼" << endl;
	cout << "    ע��(R)" << endl;
	cout << "    �û�(L)" << endl;
	do {
		cout << "> ";
		cin >> choice;
		gets(temp);
		if ((choice == 'R') || (choice == 'r')) {
			regist();
		} else if ((choice == 'L') || (choice == 'l')){
			current = login();
		} else {
			cout << "ѡ�����������ѡ��" << endl;
		}
	} while (current == NULL);
	return current;
}

User* login() {
	char usr[10], pwd[10];
	cout << "�������û�����";
	cin >> usr;
	cout << "���������룺";
	inputPwd(pwd);
	
	User *user = loadUser(), *temp = user;
	bool flag = false;
	while (temp != NULL) {
		if (temp->login(usr, pwd)) {
			flag = true;
			break;
		}
		temp = temp->next;
	}
	releaseUser(user);
	if (!flag) {
		cout << "����¼ʧ�ܣ�������û�������" << endl;
	} else {
		cout << "��¼�ɹ�" << endl;
	}
	return flag ? new User(usr, pwd) : NULL;
}

void regist() {
	char usr[10], pwd[10], repwd[10];
	cout << "�������û�����";
	cin >> usr;
	cout << "���������룺";
	inputPwd(pwd);
	cout << "��ȷ�����룺";
	inputPwd(repwd);
	
	if (strcmp(repwd, pwd) != 0) {
		cout << "������������벻һ�£�ע��ʧ��" << endl;
		return;
	}
	
	User *record = loadUser(), *temp = record;
	while (temp != NULL) {
		if (temp->currentUser(usr)) {
			cout << "����ʧ�ܣ��û��Ѵ���" << endl;
			releaseUser(record);
			return;
		}
		temp = temp->next;
	}
	
	User *user = new User(usr, pwd);
	user->next = record;
	// �����ļ� 
	saveUser(user);
	releaseUser(user);
	cout << "�û������ɹ�" << endl;
}

void releaseUser(User* user) {
	while (user != NULL) {
		User *temp = user;
		user = user->next;
		free(temp);
	}
}

User* loadUser() {
	User *_u = NULL, *temp = new User();
	ifstream inFile(USER_FILE, ios::in | ios::binary); //�����ƶ���ʽ��
	while(inFile.read((char*)temp, sizeof(User))) { //һֱ�����ļ�����
		temp->next = _u;
		_u = temp;
		temp = new User();
    }
    inFile.close();
    return _u;
}

void saveUser(User *user) {
	ofstream outFile(USER_FILE, ios::out | ios::binary); //������д��ʽ��
	while (user != NULL) {
		outFile.write((char*) user, sizeof(User));
		user = user->next;
	}
	outFile.close();
}
