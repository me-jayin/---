using namespace std;

/**
 * ���������� 
 */
class Command {
	private:
		string cmd; // �û���������� 
	public:
		string args[5]; // ���н�����Ĳ��� 
		int argsCount; // �������� 
		User *user; // ��ǰ�û� 
		FileManager *fm; // ��ǰ�ļ������� 
		Command(User *user, FileManager *fm) {
			// �����ָ���ļ���������ֱ���˳� 
			if (fm == NULL) {
				exit(1);
			}
			this->user = user;
			this->fm = fm;
		};
		int analyse(string cmd);
		void command();
};

class Handlers {
	public:
		static void exitc(Command* cmd) {
			cout << "ϵͳ�˳���...";
			exit(1);	
		};
		static void dir(Command* cmd) {
			cmd->fm->displayChildDirectory(cmd->user);
		};
		static void clear(Command* cmd) {
	        system("cls");
		}
		static void mkdir(Command* cmd) {
			if (cmd->argsCount != 1) {
				cout << "�������󣬲����б�[dir_name]" << endl;
				return;
			}
			// ���ô����ļ�
			cmd->fm->createDirectory(cmd->user, cmd->args[0]);
		}
		static void cd(Command* cmd) {
			if (cmd->argsCount != 1) {
				cout << "�������󣬲����б�[dir_name]" << endl;
				return;
			} 
			
			cmd->fm->cd(cmd->user, cmd->args[0]);
		}
		static void create(Command* cmd) {
			if (cmd->argsCount != 1) {
				cout << "�������󣬲����б�[file_name]" << endl;
				return;
			} 
			
			cmd->fm->createFile(cmd->user, cmd->args[0]);
		}
		static void open(Command* cmd) {
			if (cmd->argsCount != 1) {
				cout << "�������󣬲����б�[file_name]" << endl;
				return;
			}
			
			cmd->fm->openFile(cmd->user, cmd->args[0]);
		}
		static void rm(Command* cmd) {
			if (cmd->argsCount != 1) {
				cout << "�������󣬲����б�[file_name]" << endl;
				return;
			}
			
			cmd->fm->rm(cmd->user, cmd->args[0]);
		}
		static void prtbit(Command* cmd) {
			cmd->fm->bitMap->display(); 
		}
		static void rfm(Command* cmd) {
			FileManager *fm = initFileManager(true);
			if (fm == NULL) return;
			cmd->fm = fm;
		}
		static void logout(Command* cmd) {
			Handlers::clear(cmd);
			cmd->user = loginForm();
			cmd->fm->cd(cmd->user, "/");
			Handlers::clear(cmd);
			Handlers::help(cmd);
			getchar();
		}
		static void help(Command* cmd) {
			cout << 
	"               �η��ļ�ϵͳ\n\
	����     ��������            �﷨\n\
	exit     �˳�ϵͳ            exit\n\
	clear    �����Ļ            clear\n\
	help     ��ʾ�����˵�        help\n\
	cd       ǰ��ָ���ļ���      cd [dir_name]\n\
	dir      ��ʾ�ļ��б�        dir\n\
	mkdir    �����ļ���          mkdir [dir_name]\n\
	create   �����ļ�            create [file_name]\n\
	open     ���ļ�            open [file_name]\n\
	rm       ɾ���ļ����ļ���    rm [dir_name | file_name]\n\
	prtbit   ��ӡλʾͼ          prtbit\n\
	logout   ע���û�            logout\n\
	rfm      ��ʽ��ϵͳ          rfm\n";
		};
};

/** ϵͳ���� */
const char* syscmds[] = {
    "exit", "help", "dir", "clear", "mkdir", 
	"cd", "create", "open", "rm", "prtbit", "logout", "rfm",  
};
/** ��Ӧ����Ĵ����� */
void (*handlers[])(Command*) = {
    Handlers::exitc, Handlers::help, Handlers::dir, Handlers::clear, Handlers::mkdir,
    Handlers::cd, Handlers::create, Handlers::open, Handlers::rm, Handlers::prtbit, Handlers::logout, Handlers::rfm
	 
};
/** ������� */
int cmdsize = sizeof(syscmds) / sizeof(char*);

/**
 * �������� 
 */
int Command::analyse(string cmd) {
	int i = 0;
	for (; i < cmdsize; i++) {
		if (strStartWith(cmd.c_str(), syscmds[i])) {
			break;
		}
	}
	if (i == cmdsize) {
		return -1;
	}
	
	// ���������Ȳ��� 
	argsCount = 0;
	bool flag = false;
	int len = cmd.length(), cmdlen = strlen(syscmds[i]), start = 0, end = 0;
	const char *cmdcrs = cmd.c_str();
	for (int j = cmdlen; j < len; j++) {
		if (argsCount == 5) break;
		else if (cmdcrs[j] == ' ' && !flag) {
			start = end = j;
			flag = true;
		} else if (cmdcrs[j] == ' ' && start == end) {
			start++;
			end++;
		} else if (cmdcrs[j] == ' ') {
			// �ҵ�����һ���ո�
			args[argsCount++] = cmd.substr(start + 1, j - start - 1);
			cout << args[argsCount-1] << endl;
			start = end = j;
		} else {
			end++;
		}
	}
	if (argsCount != 5 && start != end) {
		args[argsCount++] = cmd.substr(start + 1, len);
	}
	return i;
}
/**
 * ��ʼִ������ 
 */
void Command::command() {
	Handlers::clear(NULL);
	Handlers::help(NULL);
	cin.get();
	while (true) {
		cout << "[" << user->usr << "@ " << this->fm->getCurrentDirectoryName() << "]# ";
		
		getline(cin, cmd);
		int idx = this->analyse(cmd);
		if (idx < 0) {
			if (!strStartWith(cmd.c_str(), "\n") && strlen(cmd.c_str()) != 0) cout << "��Ч������" << endl;
			continue;
		}
		handlers[idx](this);
	}
} 

