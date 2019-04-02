#include "pch.h"
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;
#pragma warning(disable:4996)

void cutString(string str, string token, vector<string>& result) {
	result.clear(); // 결과값 반환할 문자열 벡터 초기화

	char *str_buff = new char[1000];
	strcpy(str_buff, str.c_str());

	char *token_buff = new char[5];
	strcpy(token_buff, token.c_str());

	char *tok = strtok(str_buff, token_buff);
	while (tok != nullptr) {
		result.push_back(string(tok));
		tok = strtok(nullptr, token_buff);
	}
	delete str_buff;
	delete token_buff;

}