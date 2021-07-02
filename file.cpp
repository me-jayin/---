
/** �ļ����ͣ�·�� */ 
#define DIRECTORY 1
/** �ļ����ͣ���ͨ�ļ� */
#define NORMAL_FILE 2

// ��128��table�� 
#define TABLE_ITEM_SIZE 128
// bit��Χ�����С 
#define BIT_RANGE_SIZE 5

/**
 * Ŀ¼�� 
 */
class DirectoryItem {
	public:
		short inum; // �ļ�i�ڵ�� 
		char filename[10]; // �ļ���
		char type; // �ļ�����
		char usr[10]; // �ļ�������
		short iparent; // ��Ŀ¼��i�ڵ��  
		short length; // �ļ�����
		BitEmptyRange address[BIT_RANGE_SIZE]; // ����ļ��ĵ�ַ    
		DirectoryItem() {
			this->release();
		}
		void release() {
			inum = -1;
			iparent = -1;
			length = 0;
			type = -1;
		}
		/**
		 * �õ���ǰ�ļ�ʹ�õ�λ�� 
		 */
		int getUseBitCount() {
			int count = 0, i = 0;
			for (; i < BIT_RANGE_SIZE; i++) {
				BitEmptyRange rg = address[i];
				if (rg.min == -1) continue;
				count += (rg.max - rg.min) + 1;
			}
			return count;
		}
		
		void adjustBitRange() {
			int offset = 1;
			for (int i = 0, last; i + offset < BIT_RANGE_SIZE;) {
				last = i + offset;
				
				if (address[i].min == -1) { // �����ǰ��ΧΪ�գ������һ���ǿշ�ΧŲ��ǰ�棬��ƫ�Ƶ���һ�� 
					address[i].min = address[last].min;
					address[i].max = address[last].max;
					address[last].min = address[last].max = -1;
					offset++; 
				} else if (address[last].min == -1) {
					offset++;
				} else if (address[i].max == address[last].min - 1) { // ����������С�͵�ǰ����������ڵ�����кϲ� 
					address[i].max = address[last].max;
					address[last].min = address[last].max = -1;
					offset++; // �Ա�λ��ƫ��һ�� 
				} else { // �����ǰ��ƫ�ƵĲ������ڣ���ô��ǰ����Ų��ƫ��λ���� 
					i++;
					if (offset > 1) offset--;
				}
			} 
		}
};

class DirectoryArray {
	public:
		int count; // ����Ҫ����ļ������ 
		DirectoryItem **items; // �ļ�� 
		DirectoryArray(int count, DirectoryItem **items) {
			this->count = count;
			this->items = items;
		}
};

/**
 * �ļ��� 
 */
class DirectoryTable {
	public:
		DirectoryItem items[TABLE_ITEM_SIZE];
		DirectoryTable() {}
		
		/**
		 * �ҵ�ָ���ڵ� 
		 */
		DirectoryItem* find(short inum) {
			for (int i = 0; i < TABLE_ITEM_SIZE; i++) {
				if (items[i].inum == inum) {
					return &items[i];
				}
			}
			return NULL;
		}
		
		bool fileExistBy(char *usr, short inum, string filename) {
			// �����õ��������ļ��������� 
			for (int i = 0; i < TABLE_ITEM_SIZE; i++) {
				// ��ǰ�û�������Ϊ��ǰĿ¼�µ�ͬ���ļ� 
 				if (items[i].inum > -1 && strcmp(usr, items[i].usr) == 0 
				 		&& items[i].iparent == inum && strcmp(items[i].filename, filename.c_str()) == 0) {
					return true;
				}
			}
			return false;
		}
		
		/**
		 * �ҵ�ָ���ڵ��ӽڵ� 
		 */
		DirectoryArray* findByParent(char *usr, short inum) {
			int idx[TABLE_ITEM_SIZE] = {-1}, count = 0;
			// �����õ��������ļ��������� 
			for (int i = 0; i < TABLE_ITEM_SIZE; i++) {
 				if (items[i].inum > -1 && strcmp(usr, items[i].usr) == 0 && items[i].iparent == inum) {
					idx[count++] = i;
				}
			}
			if (!count) return NULL;
			
//			DirectoryItem *_items[count];
			DirectoryItem **_items = (DirectoryItem**) malloc(sizeof(DirectoryItem *) * count);
			for (int i = 0; i < count; i++) {
				_items[i] = &items[idx[i]];
			}
			return new DirectoryArray(count, _items);
		}
		
		/**
		 * �ҵ��սڵ� 
		 */
		DirectoryItem* useEmptyItem() {
			for (int i = 0; i < TABLE_ITEM_SIZE; i++) {
				if (items[i].inum == -1) {
					items[i].inum = i;
					return &items[i];
				}
			}
			return NULL;
		}
		
		/** 
		 * �ͷŽڵ� 
		 */
		void freeItem(short inum) {
			DirectoryItem *item = find(inum);
			if (item != NULL) item->release();
		}
		
		
};


