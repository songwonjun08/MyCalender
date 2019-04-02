// MyCalender.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//


#include "pch.h"
#include <iostream>
#include <vector>
#include <ctime> // 현재 날짜 시간 구하기
#include <cstring>
#include <string>
#include <conio.h>
#include <stdlib.h>
#include <fstream> // 파일 입출력
#include <windows.h> // for system("cls");
#include <algorithm>

#define leapyear(year) ((year)%4==0 && ( (year)%100!=0 || (year)%400==0 )) // 윤년판정매크로
#define LEFT   75
#define RIGHT  77
#define UP     72
#define DOWN   80
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
using namespace std;
int totalDay[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

class Date {
public:
	int year;
	int month;
	int day;

};

class Schedule {
public:
	int startTime;
	int finishTime;

	int startHour;
	int startMinute;
	int finishHour;
	int finishMinute;
	string data;

	Schedule(int _startTime, int _finishTime, string _data) {
		startTime = _startTime;
		finishTime = _finishTime;
		data = _data;
		startHour = startTime / 100;
		startMinute = startTime - (startHour * 100);
		finishHour = finishTime / 100;
		finishMinute = finishTime - (finishHour * 100);
	}

	// for compare (클래스의 어떤 변수로 정렬행할건지)
	static bool compareSpecific(const Schedule *x, const Schedule *y) {
		return (x->startTime < y->startTime);
	}
};

class ScheduledDay {
	Date day;
	vector<Schedule*> list;

public:
	void set_day(int _year, int _month, int _day) {
		day.year = _year;
		day.month = _month;
		day.day = _day;
	}

	int get_day() {
		return day.day;
	}

	void addSchedule(int startTime, int finishTime, string data) {
		Schedule *newData = new Schedule(startTime, finishTime, data);
		list.push_back(newData);
	}

	void delSchedule(int index) {
		if (list.empty() == false && index <= list.size()) {
			Schedule *temp = list[index - 1];
			list.erase(list.begin() + index - 1); // index번의 원소 삭제
			delete temp; // Memory
		}
	}

	bool islistEmpty() {
		if (list.empty()) {
			return TRUE;
		}
		return false;
	}

	void sortSchedule() {
		// sort(START, END, 해당 클래스에 선언되있는 비교 조건 함수)
		sort(list.begin(), list.end(), Schedule::compareSpecific);
	}


	void delListMemory() {
		// 동적 할당 한 벡터 메모리 해제
		for (int i = 0; i < list.size(); i++) {
			Schedule *temp = list[i];
			delete temp;
		}
		list.clear();
	}

	void print() {
		for (int i = 0; i < list.size(); i++) {
			int startHour = list[i]->startHour;
			int startMinute = list[i]->startMinute;
			int finishHour = list[i]->finishHour;
			int finishMinute = list[i]->finishMinute;

			if (startMinute < 10 && finishMinute < 10) {
				printf("%d) %d:0%d ~ %d:0%d %s\n", i + 1, startHour, startMinute, finishHour, finishMinute, list[i]->data.c_str());
			}
			else if (startMinute < 10) {
				printf("%d) %d:0%d ~ %d:%d %s\n", i + 1, startHour, startMinute, finishHour, finishMinute, list[i]->data.c_str());
			}
			else if (finishMinute < 10) {
				printf("%d) %d:%d ~ %d:0%d %s\n", i + 1, startHour, startMinute, finishHour, finishMinute, list[i]->data.c_str());
			}
			else {
				printf("%d) %d:%d ~ %d:%d %s\n", i + 1, startHour, startMinute, finishHour, finishMinute, list[i]->data.c_str());
			}

		}
	}

	string convertData() {
		if (list.size() != 0) {
			string dayInfo = to_string(day.year) + "_" + to_string(day.month) + "_" + to_string(day.day) + "*";
			string contentInfo;

			for (int i = 0; i < list.size(); i++) {
				contentInfo += to_string(list[i]->startTime) + "_" + to_string(list[i]->finishTime) + "_" + list[i]->data;
				if (i != list.size() - 1) {
					contentInfo += "/";
				}
			}

			return dayInfo + contentInfo;
		}
	}
};

class Calender {
	Date selected;
	vector<ScheduledDay*> scheduledDay_list;
	bool clear = false;

public:
	Calender() {
		selected.year = 1992;
		selected.month = 3;
		selected.day = 24;
	}

	int get_day() {
		return selected.day;
	}
	int get_month() {
		return selected.month;
	}

	void set_day(int _day) {
		selected.day = _day;
	}

	void nextMonth() {
		if (selected.month + 1 <= 12) {
			selected.month += 1;
		}
		else {
			selected.year += 1;
			selected.month = 1;
		}

		if (selected.day > totalDay[selected.month]) {
			selected.day = totalDay[selected.month];
		}

		loadScheduleData();
	}

	void prevMonth() {
		if (selected.month - 1 >= 1) {
			selected.month -= 1;
		}
		else {
			selected.year -= 1;
			selected.month = 12;
		}

		if (selected.day > totalDay[selected.month]) {
			selected.day = totalDay[selected.month];
		}

		loadScheduleData();
	}

	void setToday() {
		// 헤더 파일에 가보면 'typedef long time_t'로 되어 있다. 즉,long과 동일하다고 보면 된다.
		time_t curr_time;

		// 시간 표시를 위한 구조체를 선언합니다.
		struct tm *curr_tm;

		// time 함수는 1970년 1월 1일 이후 몇초가 지났는지를 계산합니다. NULL을 인자로 사용합니다.
		curr_time = time(NULL);

		// 지역 시간을 기준으로 변환 및 출력 편의를 위하여 tm 구조체에 저장합니다.(포맷팅)
		curr_tm = localtime(&curr_time);

		selected.year = curr_tm->tm_year + 1900;
		selected.month = curr_tm->tm_mon + 1;
		selected.day = curr_tm->tm_mday;
	}

	void delScheduledDayListMemory() {
		for (int i = 0; i < scheduledDay_list.size(); i++) {
			// scheduledDay 클래스의 vector 원소 삭제
			scheduledDay_list[i]->delListMemory();
			// delete scheduledDay list
			ScheduledDay *temp = scheduledDay_list[i];
			delete temp;
		}
		scheduledDay_list.clear();
	}

	string getFileName() {
		string fileName = to_string(selected.year);
		if (selected.month < 10) {
			fileName = fileName + "0";
		}
		fileName = fileName + to_string(selected.month) + ".txt"; // ex) 201903.txt
		return fileName;
	}

	void saveScheduleData() {

		if (scheduledDay_list.empty() == FALSE) {
			string fileName = getFileName();

			ofstream out(fileName);

			for (int i = 0; i < scheduledDay_list.size(); i++) {
				out << scheduledDay_list[i]->convertData() << "\n";
			}
			out.close();
		}


	}

	void loadScheduleData() {
		// Heap영역 메모리 해제
		delScheduledDayListMemory();

		// Set File name
		string fileName = getFileName();

		vector<string> strVector; // 자른 문자열을 보관할 벡터
		string in_line;
		ifstream in(fileName);

		while (getline(in, in_line)) { // 한 줄씩 읽어오기
			ScheduledDay *temp = new ScheduledDay; // Create new day

			cutString(in_line, "*", strVector);
			string schedule = strVector[1];
			cutString(strVector[0], "_", strVector);
			temp->set_day(stoi(strVector[0]), stoi(strVector[1]), stoi(strVector[2]));

			vector<string> tempVector; // 자른 문자열을 보관할 벡터
			cutString(schedule, "/", strVector);
			for (int i = 0; i < strVector.size(); i++) {
				cutString(strVector[i], "_", tempVector);
				temp->addSchedule(atoi(tempVector[0].c_str()), atoi(tempVector[1].c_str()), tempVector[2]);
			}
			scheduledDay_list.push_back(temp);
		}
		in.close();
	}

	int getStartDay() {
		// 해당년도가 윤년이면 윤달 처리
		if (leapyear(selected.year)) {
			totalDay[2] = 29;
		}
		else { totalDay[2] = 28; }

		int last_year = selected.year - 1;
		int start_day = (last_year + (last_year / 4) - (last_year / 100) + (last_year / 400) + 1) % 7;
		// start_day : 1년은 365일, 1년이 지날때마다 요일이 한번씩 늘어난다 (365 % 7 == 1)
		// (last_year/4) - (last_year/100) + (last_year/400) :
		// 윤년은 2월이 29일(+1)이 됨으로 다음 해의 요일이 두번 증가함으로 작년(lasteyear)까지 있던 모든 윤년을 더한다.
		// +1 : 1년 1월 1일은 일요일이 아닌 월요일이므로 1을 더해준다.
		// %7 : 요일은 7가지 이므로, 앞 계산값을 7로 나눈 나머지값이 해당 해의 첫 요일이 됨.

		for (int i = 1;i < selected.month;i++) {
			// 지정 달의 직전까지의 총 일수를 구하기 위해 start_day에 값을 더해준다
			start_day += totalDay[i];
		}

		return start_day %= 7; // 해당 해,월 1일의 시작 요일
	}

	void printCalender() {
		system("cls");
		
		printf("=================================\n");
		printf("←↑↓→ select day\n");
		printf("< > next(prev) month\n");
		printf("+ - add(del) schedule\n");
		printf("ESC : exit\n");
		printf("=================================\n");

		int startDay = getStartDay();

		printf("%d %d %d  \n", selected.year, selected.month, selected.day);
		printf("SUN  MON  TUE  WED  THU  FRI  SAT");
		for (int i = -startDay; i < totalDay[selected.month];i++) {
			bool starCheck = false;

			if ((i + startDay) % 7 == 0) {
				printf("\n");
			}
			if (i < 0) {
				printf("     ");
			}
			else {
				printf("%3d", i + 1);

				//printf(" ");
				for (int j = 0; j < scheduledDay_list.size(); j++) {
					int a = scheduledDay_list[j]->get_day();
					if ((i + 1) == a) {
						printf("*");
						starCheck = true;
						break;
					}
				}
				if (starCheck == false) {
					printf(" ");
				}

				if ((i + 1) == selected.day) {
					printf("<");
				}
				else {
					printf(" ");
				}
			}
		}
		printf("\n\n");

		// print schedule
		for (int j = 0; j < scheduledDay_list.size(); j++) {
			int a = scheduledDay_list[j]->get_day();
			if (selected.day == a) {
				scheduledDay_list[j]->print();
			}
		}
		printf("\n\n");
	}


	void addUserSchedule() {
		string user_input;
		vector<string> tempVector; // 자른 문자열을 보관할 벡터

		getline(cin, user_input); // 공백 포함해서 입력 받기
		cutString(user_input, " ", tempVector);

		// check the user input string

		if (tempVector.size() >= 3) {
			vector<string> startTimeV;
			vector<string> finishTimeV;
			string data;

			// input schedule data
			for (int i = 2; i < tempVector.size(); i++) {
				data += tempVector[i];
				data += " ";
			}
			cutString(tempVector[0], ":", startTimeV);
			cutString(tempVector[1], ":", finishTimeV);

			if (startTimeV.size() == 2 && finishTimeV.size() == 2) {
				int sTime = (atoi(startTimeV[0].c_str()) * 100) + atoi(startTimeV[1].c_str());
				int fTime = (atoi(finishTimeV[0].c_str()) * 100) + atoi(finishTimeV[1].c_str());

				if (sTime <= fTime) {
					bool already = false;

					// 현재 스케줄리스트에 해당 날짜가 있는 경우
					for (int i = 0; i < scheduledDay_list.size(); i++) {
						if (selected.day == scheduledDay_list[i]->get_day()) {
							scheduledDay_list[i]->addSchedule(sTime, fTime, data);
							scheduledDay_list[i]->sortSchedule();
							already = true;
						}
					}
					// 현재 스케줄리스트에 해당 날짜가 없는 경우
					if (already == false) {
						ScheduledDay *temp = new ScheduledDay; // Create new day
						temp->set_day(selected.year, selected.month, selected.day);
						temp->addSchedule(sTime, fTime, data);
						scheduledDay_list.push_back(temp);
					}

					printf("SUCCESS!");
				}

			}
		}
		else {
			printf("WRONG INPUT\n");
		}

	}

	void delUserSchedule() {
		for (int i = 0; i < scheduledDay_list.size(); i++) {
			if (selected.day == scheduledDay_list[i]->get_day()) {
				int user_input;
				scanf("%d", &user_input);
				if (user_input == 0) {
					return;
				}
				else {
					scheduledDay_list[i]->delSchedule(user_input);

					// 유저의 스케쥴 삭제로 인해 리스트가 빈 scheduledDay가 되었을때 삭제해줌
					if (scheduledDay_list[i]->islistEmpty()) {
						// delete scheduledDay list
						ScheduledDay *temp = scheduledDay_list[i];
						scheduledDay_list.erase(scheduledDay_list.begin() + i); // index번의 원소 삭제
						delete temp;
					}
				}
			}
		}
	}
};



int main()
{
	Calender calender;

	calender.setToday();
	calender.loadScheduleData();
	calender.printCalender();

	char key;
	do {
		key = getch();
		int tDay = calender.get_day();
		int lastDay = totalDay[calender.get_month()];

		switch (key) {
		case UP:
			if (tDay - 7 >= 1) {
				calender.set_day(tDay - 7);
			}
			else {
				calender.set_day(1);
			}
			break;
		case DOWN:
			if (tDay + 7 <= lastDay) {
				calender.set_day(tDay + 7);
			}
			else {
				calender.set_day(lastDay);
			}
			break;
		case LEFT:
			if (tDay - 1 >= 1) {
				calender.set_day(tDay - 1);
			}
			break;
		case RIGHT:
			if (tDay + 1 <= lastDay) {
				calender.set_day(tDay + 1);
			}
			break;
		case 60: // <
			system("cls");
			calender.saveScheduleData();
			calender.prevMonth();
			break;

		case 62: // >
			system("cls");
			calender.saveScheduleData();
			calender.nextMonth();
			break;

		case 43: // +
			printf("=================================\n");
			printf("START END CONTENT\n");
			printf("11:00 14:00 hang out with Jack (Example)\n");
			printf("=================================\n");
			calender.addUserSchedule();
			system("cls");
			break;

		case 45: // -
			printf("=================================\n");
			printf("Select the index number which you want to delete\n(0 = cancle)\n");
			printf("=================================\n");
			calender.delUserSchedule();
			system("cls");
			break;

		default:
			break;

		}
		// system("cls");와 같은의미 깜박임 덜함
		COORD pos = { 0,0 };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

		calender.printCalender();
		if (key == 27) {
			// save
			calender.saveScheduleData();
			break;
		}
	} while (true);

	
	// Heap영역 메모리 해제
	calender.delScheduledDayListMemory();

}
