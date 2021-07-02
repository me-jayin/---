#include<stdlib.h>

#include<iostream>
using namespace std;

#define UNIT_BIT 8
#define BIT_MAP_SIZE 512
#define BIT_TOTAL_BIT 4096

class BitEmptyRange {
	public:
		int min; // ռ��λ��ʼ�±� 
		int max; // ռ��λ�����±� 
		BitEmptyRange() {
			min = max = -1;
		}
		BitEmptyRange(int mi, int ma) {
			min = mi;
			max = ma;
		}
};

class BitMap {
	private:
		char map[BIT_MAP_SIZE];
		/** ʹ�õ�bit�� */
		int count;
	public:
		BitMap() {
			// ��ʼ���ֽ����� 
			for (int i = 0; i <= BIT_MAP_SIZE; i++) {
				map[i] = 0;
			}
			count = 0;
		};
		~BitMap() {
			free(map);
		} 
		
		int getMaxSize() {
			return BIT_MAP_SIZE;
		}
		
		/**
		 * �õ����еĿռ� 
		 */ 
		int getFreeCount() {
			return BIT_MAP_SIZE - count;
		}
		
		/**
		 * ����λͼλ�ã�����offset��0��ʼ�� statusΪ1��ʾ��ʹ�ã�Ϊ0��ʾδʹ�� 
		 */
		bool set(int offset, int status) {
			if (offset < 0 || offset >= BIT_TOTAL_BIT) return false;
			
			int idx = offset / UNIT_BIT, bitOffset = offset % UNIT_BIT;
			char target = map[idx];
			// �жϳ�ʼ״̬�ͽ�Ҫ���õ��Ƿ�һ�� 
			if (!((target >> bitOffset) ^ status)) return false; 
			map[idx] = target ^ (1 << bitOffset);
			
			if (status) count++;
			else count--;
			return true;
		}
		
		/**
		 * ��ȡĳһλ��״̬ 
		 */
		bool get(int offset) {
			if (offset < 0 || offset > BIT_TOTAL_BIT) return false;
			
			int idx = offset / UNIT_BIT, bitOffset = offset % UNIT_BIT;
			char target = map[idx];
			return (target >> bitOffset) & 1;
		}
		
		/**
		 * ��ָ����Χ����������״̬ 
		 */
		bool setRange(int start, int end, int status) {
			// ����У�����÷�Χ�ڵ�״̬�Ƿ��Ѿ���Ŀ��״̬һ�� 
			for (int i = start; i <= end; i++) {
				if (this->get(i) == status) return false;
			}
			 
			for (int i = start; i <= end; i++) {
				this->set(i, status);
			}
			return true; 
		}
		
		/**
		 * ����ָ����Χ��ֵ 
		 */
		bool setRange(BitEmptyRange *range, int status) {
			this->setRange(range->min, range->max, status);
		}
		
		/**
		 * ��ȡΪ�յ����ݿ鷶Χ 
		 */
		BitEmptyRange* getEmptyRange() {
			return this->getEmptyRange(BIT_MAP_SIZE);
		}
		
		/**
		 * ��ȡָ����С��ΧΪ�յ����ݿⷶΧ 
		 */
		BitEmptyRange* getEmptyRange(int size) {
			return getEmptyRange(0, size);
		}
		
		/**
		 * ��ȡָ��λ�ú�δʹ�õ�λ��Χ
		 */
		BitEmptyRange* getEmptyRange(int pos, int size) {
			int min = -1, max = -1;
			for (int i = pos; i < BIT_MAP_SIZE && size; i++) {
				if (!this->get(i)) {
					if (min == -1) min = i;
					max = i;
					size--;
				} else if (min != -1) {
					break;
				}
			}
			return new BitEmptyRange(min, max);
		}
		
		/**
		 * ��ȡ��һ��δ��ʹ�õ�bit 
		 */
		int getFristEmpty() {
			for (int i = 0; i < BIT_MAP_SIZE; i++) {
				if (!this->get(i)) {
					return i;
				}
			}
			return -1;
		}
        
        /**
         * ��ȡ��ָ��λ�ã�����ָ��λ�ã��ѱ�ʹ�õ�bit 
         */
        int getFirstUse(int pos) {
        	for (int i = pos; i < BIT_MAP_SIZE; i++) {
        		if (this->get(i)) {
					return i;
				}
			}
			return -1;
		}
		
		void display() {
			for (int i = 0; i < BIT_TOTAL_BIT; i++) {
				if (i % 32 == 0 && i != 0) cout << endl;
				cout << this->get(i) << " ";
			}
			cout << endl;
		}
				
		char* getMap() {
			return map;
		}
};
