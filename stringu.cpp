#include<stdio.h>
#include<string.h>

/**
 * �ж��ַ���str�Ƿ���prefix��ͷ 
 */
bool strStartWith(const char* str, const char* prefix) {
	if (str == prefix) return false;
	
	int i = 0;
	while (str[i] != '\0' && prefix[i] != '\0') {
		if (str[i] != prefix[i]) return false;
		i++;
	}
	// str�ַ����Ѿ����� 
	if (str[i] == '\0' && prefix[i] != '\0') return false;
	return true;
}




