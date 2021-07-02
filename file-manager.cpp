using namespace std;

/** һ��λ��ʾ�Ŀռ��С */
#define BIT_BLKSIZE 32 
const char* FILE_TABLE = "filetable.dat";

class FileManager {
    public:
	    DirectoryTable *table; // �ļ���� 
	    BitMap *bitMap; // λʾͼ 
		DirectoryItem *cdi; // ��ǰ����Ŀ¼ 
		DirectoryItem *ofile; // ��ǰ�򿪵��ļ� 
	    FileManager() {
	    	table = new DirectoryTable();
	    	bitMap = new BitMap();
	    	cdi = NULL;
	    	ofile = NULL;
		}
		const char* getCurrentDirectoryName() {
			return this->cdi == NULL ? "/" : this->cdi->filename;
		}
		void displayChildDirectory(User *user);
		void createDirectory(User *user, string filename);
		void createFile(User *user, string filename);
		DirectoryItem* findByInum(short inum); 
		void cd(User *user, string filename);
		void rm(User *user, string target);
		void openFile(User *user, string filename);
	private:
		DirectoryItem* _userFileItem(User *user, string filename, int type);
		int getInum() {
	        if (this->cdi == NULL) return -1;
	        return this->cdi->inum;
        }
        void fileOperate();
		void read();
		void write(); 
		void releaseOpenFileBit(int size); 
		void mallocOpenFileBit(int size, bool retry);
		void saveFile(string content);
		void rm(User *user, DirectoryItem *item, bool isTop);
};

void saveFmData(FileManager *fm);
void saveFmData(FileManager *fm, ofstream *outFile);
void freeDirectoryArray(DirectoryArray *ary); 

/**
 * �г��������ļ��� 
 */
void FileManager::displayChildDirectory(User *user) {
	int inum = getInum();
	
	// ��ȡ�������ļ� 
	DirectoryArray *ary = table->findByParent(user->usr, inum);

	// �������ļ����� 
	int count = ary == NULL ? 0 : ary->count;
	// ���ļ����ļ������� �� �ļ���С 
	int dircount = 0, fileTotal = 0;
	// �����������ļ� 
	for (int i = 0; i < count; i++) {
		DirectoryItem *item = ary->items[i];
		cout << item->filename << "\t\t";
		// ��������� 
		if (item->type == DIRECTORY) {
			dircount++;
			cout << "<DIR>";
		} else {
			cout << "<FILE>\t\t";
			int ft = item->getUseBitCount() * BIT_BLKSIZE;
			cout << " total size " << ft << " bytes, actaul size " << item->length << " bytes ";
			fileTotal += ft;
		}
		cout << endl;
	}
	
	// ������� 
	cout << "\t\t" << (count - dircount) << " file(s) " << fileTotal << " bytes " << endl;
	cout << "\t\t" << dircount << " dir(s) " << (bitMap->getFreeCount() * BIT_BLKSIZE) << " bytes, free space " << endl;
    freeDirectoryArray(ary);
}

/**
 * ʹ���û��ļ������÷��� 
 */ 
DirectoryItem* FileManager::_userFileItem(User *user, string filename, int type) {
	int inum = getInum();
	
	// �ж��ļ��Ƿ���� 
	string tf = (type == DIRECTORY ? "�ļ���" : "�ļ�");
	if (table->fileExistBy(user->usr, inum, filename)) {
		cout << "����ʧ�ܣ�" << tf << "�Ѵ���" << endl;
		return NULL;
	}
	
	// ��ȡ���ÿ��� 
	DirectoryItem *empty = table->useEmptyItem();
	if (empty == NULL) {
		cout << tf << "����ʧ�ܣ�FAT����������" << endl;
		return NULL;
	}
	// �����ļ��� 
	empty->iparent = inum;
	strcpy(empty->filename, filename.c_str());
	empty->type = type;
	strcpy(empty->usr, user->usr);
	return empty;
}

/** 
 * �����ļ��� 
 */
void FileManager::createDirectory(User *user, string filename) {
	DirectoryItem *empty = this->_userFileItem(user, filename, DIRECTORY);
	if (empty == NULL) return;
	
	cout << "�ļ��д������" << endl;
	// ʵʱд�뵽�ļ��� 
	saveFmData(this);
}

void FileManager::createFile(User *user, string filename) {
	DirectoryItem *empty = this->_userFileItem(user, filename, NORMAL_FILE);
	if (empty == NULL) return;
	
	empty->length = 0;
	cout << "�ļ��������" << endl;
	// ʵʱд�뵽�ļ��� 
	saveFmData(this);
}

DirectoryItem* FileManager::findByInum(short inum) {
	return table->find(inum);
}

void FileManager::cd(User *user, string filename) {
	// ����� cd .. �� cd ../ ����һ��Ŀ¼ 
	if (filename.compare("..") == 0 || filename.compare("../") == 0) {
		if (this->cdi == NULL) {
			cout << "�Ѿ��޸���Ŀ¼" << endl;
		    return;
		} else { // �õ�����Ŀ¼ 
			short pinum = this->cdi->iparent;
			// ����޸�����ֱ�ӵ���Ŀ¼ 
			if (pinum == -1) this->cdi = NULL;
			else {
				this->cdi = this->findByInum(pinum);
			}
		    return;
		}
	} else if (filename.compare("/") == 0) {
		this->cdi = NULL;
		return;
	}
	
	int inum = getInum();
	// ��ȡ�������ļ� 
	DirectoryArray *ary = table->findByParent(user->usr, inum);
	if (ary == NULL) {
		cout << "����ʧ�ܣ��ļ��в�����" << endl;
		return;
	}
	
	for (int i = 0; i < ary->count; i++) {
		// �ҵ�ͬ���ļ� 
		if (strcmp(filename.c_str(), ary->items[i]->filename) == 0) {
			DirectoryItem *temp = ary->items[i];
		    freeDirectoryArray(ary);
		    // �ж����� 
			if (temp->type != DIRECTORY) {
				cout << "����ʧ�ܣ�Ŀ����ļ���" << endl;
				return;
			}
			this->cdi = temp;
		    return;
		}
	}
	cout << "����ʧ�ܣ��ļ��в�����" << endl;
}

void FileManager::rm(User *user, string filename) {
	int inum = getInum();
	// ��ȡ�������ļ� 
	DirectoryArray *ary = table->findByParent(user->usr, inum);
	if (ary == NULL) {
		cout << "����ʧ�ܣ��ļ�������" << endl;
		return;
	}
	
	for (int i = 0; i < ary->count; i++) {
		// �ҵ�ͬ���ļ� 
		if (strcmp(filename.c_str(), ary->items[i]->filename) == 0) {
			DirectoryItem *temp = ary->items[i];
		    freeDirectoryArray(ary);
		    rm(user, temp, true);
			// �����ļ������������ 
			saveFmData(this);
		    return;
		}
	}
	
	cout << "����ʧ�ܣ��ļ�������" << endl;
}

/**
 * ɾ���ļ���������ļ�������ʾ��Ȼ��ݹ�ɾ����������ļ�ֱ��ɾ�����ͷ�bitmap 
 */
void FileManager::rm(User *user, DirectoryItem *item, bool isTop) {
	if (item->type == DIRECTORY) { // ������ļ�����������ļ���ɾ������ 
		if (isTop) { // ������ڶ����ļ����ļ��о���ʾ 
			cout << "�Ƿ�ɾ���ļ��� [" << item->filename << "] ������������ݣ�(Y/N)" << endl;
			cout << "> ";
			string temp;
			getline(cin, temp);;
			if (temp.compare("Y") != 0 && temp.compare("y") != 0) {
				return;
			}
		}
		// ��ȡ�������ļ� 
		DirectoryArray *ary = table->findByParent(user->usr, item->inum);
		if (ary != NULL) {
			for (int i = 0; i < ary->count; i++) {
				rm(user, ary->items[i], false);
			}
		}
		item->release();
	} else if (item->type == NORMAL_FILE) { // ������ļ�ֱ��ɾ�����ͷ�bitmap 
		// �ͷ�λʾͼ 
		for (int i = 0; i < BIT_RANGE_SIZE; i++) {
			if (item->address[i].min == -1) continue;
			this->bitMap->setRange(&item->address[i], 0);
			item->address[i].min = item->address[i].max = -1;
		}
		item->release();
	}
}

void FileManager::openFile(User *user, string filename) {
	int inum = getInum();
	// ��ȡ�������ļ� 
	DirectoryArray *ary = table->findByParent(user->usr, inum);
	if (ary == NULL) {
		cout << "����ʧ�ܣ��ļ��в�����" << endl;
		return;
	}
	
	for (int i = 0; i < ary->count; i++) {
		// �ҵ�ͬ���ļ� 
		if (strcmp(filename.c_str(), ary->items[i]->filename) == 0) {
			DirectoryItem *temp = ary->items[i];
		    freeDirectoryArray(ary);
		    // �ж����� 
			if (temp->type != NORMAL_FILE) {
				cout << "����ʧ�ܣ�Ŀ��ǿɴ��ļ�" << endl;
				return;
			}
			this->ofile = temp;
			// ������ļ��������
			this->fileOperate();
		    return;
		}
	}
    cout << "����ʧ�ܣ�ָ���ļ�������" << endl;
} 

void FileManager::fileOperate() {
	string cmd;
	while (true) {
		cout << "----------------------------" << endl;
		cout << "��������Ҫ���ļ� [" << this->ofile->filename << "] ִ�еĲ�����" << endl;
		cout << "    read    ��ȡ�ļ�����" << endl;
		cout << "    write   д���ļ�����" << endl;
		cout << "    close   �ر��ļ�" << endl;
		cout << "> ";
		cin >> cmd;
		if (strcmp(cmd.c_str(), "read") == 0) {
			read();
		} else if (strcmp(cmd.c_str(), "write") == 0) {
		    write(); 
		} else if (strcmp(cmd.c_str(), "close") == 0) {
			cout << "�ļ��رճɹ�" << endl;
		    cout << "----------------------------" << endl;
		    getchar();
			return;
		} else {
			cout << "��Ч������" << endl;
		}
	}
}

/**
 * �ͷŵ�ǰ���ļ�ռ�õ�λ��ͼռλ 
 */
void FileManager::releaseOpenFileBit(int size) {
	int bitc = 0, releaseMin, releaseMax;
	// �Ӻ���ǰ�ͷ� 
	for (int i = BIT_RANGE_SIZE - 1; i >= 0 && size > 0; i--) {
		BitEmptyRange *rg = &this->ofile->address[i];
		if (rg->min == -1) continue;
		// �õ���ǰ��Χ��bit�� 
		bitc = rg->max - rg->min + 1;
		
		if (bitc > size) { // �����ǰλ��λ�÷�Χ���е�λ��������Ҫ�ͷŵ�λ�� 
			releaseMax = rg->max;
			rg->max -= size; // �����ͷ�ָ��sizeλ���maxֵ 
			releaseMin = rg->max + 1;
		} else { // ����Ҫ�ͷŵ�λ��������һ��Χ��λ������ֱ���ͷ���һ��Χ��λ����ȥ��ǰ��С����ִ�� 
			releaseMin = rg->min;
			releaseMax = rg->max;
			size -= bitc;
			rg->min = rg->max = -1;
		}
		this->bitMap->setRange(releaseMin, releaseMax, 0);
	}
} 

/**
 * Ϊ��ǰ�ļ�����ָ����С��λʾͼ�ռ� 
 */
void FileManager::mallocOpenFileBit(int size, bool retry) {
	BitEmptyRange *address = this->ofile->address;
	int rangeSize = 0;
	// ��ͳ�Ƶ�ǰ�Ѿ�ʹ�õķ�Χ��
	for (; rangeSize < BIT_MAP_SIZE; rangeSize++) {
		BitEmptyRange rg = address[rangeSize];
		if (rg.min < 0) break;
	}
	
	// ��ʼ���䷶Χ
	for (; rangeSize < BIT_RANGE_SIZE && size > 0; rangeSize++) {
		BitEmptyRange* range = this->bitMap->getEmptyRange(size);
		size -= (range->max - range->min + 1);
		// ����bitλ 
		this->bitMap->setRange(range->min, range->max, 1);
		// ��ӷ�Χ
		address[rangeSize].min = range->min;
		address[rangeSize].max = range->max;
		
		free(range);
	}
	// �жϷ���Ĵ�С�Ƿ��㹻 
	if (size > 0 && rangeSize == BIT_MAP_SIZE && !retry) { // �������size����0�����ҷ���λ�������� 
		this->ofile->adjustBitRange();
		// ������Ƭ����
		// ----------------- 
		mallocOpenFileBit(size, true);
		return; 
	}
}

void FileManager::read() {
	int length = this->ofile->length;
	if (!length) return;
	ifstream inFile(FILE_TABLE, ios::in | ios::binary);
	
	cout << "-------------- " << this->ofile->filename << " --------------" << endl;
	for (int i = 0; i < BIT_RANGE_SIZE; i++) {
		BitEmptyRange *range = &this->ofile->address[i];
		if (range->min == -1) continue;
		// ����Χ�ڵ�λ��Ӧ�Ŀ���������� 
		char temp[BIT_BLKSIZE];
		for (int i = range->min; i <= range->max; i++) {
			inFile.seekg(sizeof(char) * BIT_BLKSIZE * i, ios::beg);
			inFile.read((char*) temp, sizeof(temp));
			
			// �������������ݴ�ӡ 
			for (int j = 0; j < BIT_BLKSIZE && j < length; j++) {
				cout << temp[j];
			}
			
			// ����ʣ���ַ��� 
			length -= BIT_BLKSIZE; 
		}
	}
	cout << endl;
	cout << endl;
	inFile.close();
}

void FileManager::write() {
	// �õ��ļ���ǰռ�õ�λ�������Լ��ܹ����õ�λ�� 
	int ouBitc = this->ofile->getUseBitCount(), 
	        freeBitc = ouBitc + this->bitMap->getFreeCount();
	cout << "----------------------------" << endl;
	cout << "��ǰ���̿��ô�С��������ǰ�ļ�ռ�ã�   " << (freeBitc * BIT_BLKSIZE) << " bytes" << endl;
	cout << "        :q   �˳��༭" << endl;
	cout << "        :w   ����༭" << endl; 
	cout << "�������ļ����ݣ�" << endl;
	
	string input, temp = "";
	while (1) {
		cin >> input;
	    if (strcmp(input.c_str(), ":q") == 0) {
		    return;
	    } else if (strcmp(input.c_str(), ":w") == 0) {
	    	break;
		}
		if (temp.compare("") != 0) {
			temp.append("\n");
		}
		temp.append(input);
	}
	
    // ���㵱ǰ�༭������Ҫ��ռ�õ�λ��
    int strlen = temp.length(), needBitc = (strlen / BIT_BLKSIZE) + (strlen % BIT_BLKSIZE == 0 ? 0 : 1);
    BitEmptyRange *range = this->ofile->address;
    if (needBitc > freeBitc) { // ����ʣ���ڴ� 
    	cout << "�ļ��������̿��ô�С���༭ʧ��" << endl;
    	return;
	} else if (needBitc <= ouBitc) { // С��ԭ����ռ�ô�С����Ҫ�Ƿ񲿷�λ�����ڵĻ�������ֱ�Ӹ���ԭ��������   
		int releaseCount = ouBitc - needBitc; // �õ���Ҫ�ͷŵ�
	    this->releaseOpenFileBit(releaseCount);
	} else if (needBitc > ouBitc) { // ����ԭ�������ݵĻ�����Ҫ����ռ�õĲ���λ
	    int mallocCount = needBitc - ouBitc; // ��Ҫ��������� 
	    this->mallocOpenFileBit(mallocCount, false);
    }
//	int nbitc = 
	this->ofile->length = strlen; 
	saveFile(temp);
	cout << ">>> �ļ��༭�ɹ�" << endl;
}

void FileManager::saveFile(string content) {
	ofstream outFile(FILE_TABLE, ios::out | ios::binary| ios::in); //������д��ʽ��
	
	int strOffset = 0;
	BitEmptyRange *address = this->ofile->address;
	// ��ʼ�Է��䷶Χ
	for (int i = 0; i < BIT_RANGE_SIZE; i++) {
		BitEmptyRange *range = &address[i];
		if (range->min == -1) continue;
		// ����Χ�ڵ�λ��Ӧ�Ŀ��������� dat 
		for (int i = range->min; i <= range->max; i++) {
			// �����и��ַ��� 
			const char *sub = content.substr(strOffset++ * BIT_BLKSIZE, BIT_BLKSIZE).c_str();
			// ���ַ�����λ��ָ��λ���Ӧ��λ�� 
			outFile.seekp(sizeof(char) * BIT_BLKSIZE * i, ios::beg);
			// ���ַ������� 
			outFile.write(sub, sizeof(char) * BIT_BLKSIZE);
		}
	}
	// �����ļ������������ 
	saveFmData(this, &outFile);
	outFile.close();
}

//---------------
// ȫ�ֺ��� 
//---------------
/**
 * �ͷ��ļ���������� 
 */
void freeDirectoryArray(DirectoryArray *ary) {
	if (ary == NULL) return;
	free(ary->items);
	free(ary);	
}
/**
 * �����ļ����������ݣ�ʹ��ָ�����ļ������ʽ���� 
 */ 
void saveFmData(FileManager *fm, ofstream *outFile) {
	outFile->seekp(sizeof(char) * BIT_BLKSIZE * BIT_MAP_SIZE - 1, ios::beg);
	// ���table��λͼ��Ϣ
	outFile->write((char*) fm->table, sizeof(DirectoryTable));
	outFile->write((char*) fm->bitMap, sizeof(BitMap));
}

/**
 * �����ļ����������ݣ�ʹ��Ĭ�϶�д��ʽ���� 
 */
void saveFmData(FileManager *fm) {
	ofstream outFile(FILE_TABLE, ios::out | ios::binary| ios::in); //������д��ʽ��
	saveFmData(fm, &outFile);
	outFile.close();
}

/**
 * ��ʼ���ļ��������� 
 */
void initFmDat(FileManager *fm) {
	ofstream outFile(FILE_TABLE, ios::trunc | ios::binary); //������д��ʽ��
	// �����ָ��Ӳ������ 
	cout << "����׼����..." << endl;
	char c[1];
	c[0] = '\0';
	for (int i = 0; i < BIT_BLKSIZE * BIT_TOTAL_BIT; i++) {
		outFile.write((char*) c, sizeof(c));
	}
	saveFmData(fm, &outFile);
	outFile.close();
} 

/**
 * �����ļ�ϵͳ��ʼ����initΪ�Ƿ��ʼ�� 
 */
FileManager* initFileManager(bool init) {
	FileManager *fm = new FileManager();
	
	ifstream fmd(FILE_TABLE, ios::in | ios::binary);
	if (!fmd.good() || init) {
		cout << "�Ƿ����ϵͳ�ļ���ʼ��(Y/N)��";
		string temp;
		cin >> temp;
	    getchar();
		if (temp.compare("Y") == 0 || temp.compare("y") == 0) {
			fmd.close();
			// ��ʼ���ļ� 
			initFmDat(fm);
			cout << "ϵͳ��ʼ�����" << endl;
			return fm;
		} else { // �˳� 
			return NULL;
		}
	}
	
	// ��ȡ����
	fmd.seekg(sizeof(char) * BIT_BLKSIZE * BIT_TOTAL_BIT - 1, ios::beg);
	// ���table��λͼ��Ϣ
	fmd.read((char*) fm->table, sizeof(DirectoryTable));
	fmd.read((char*) fm->bitMap, sizeof(BitMap));
	fmd.close();
	return fm;
}

