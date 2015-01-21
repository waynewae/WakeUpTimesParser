/*
	ReadFile : PowerCalculator@version.log
	OutputFile : WakeUpTimes_date.log
*/
#include<fstream>
#include<iostream>
#include<string>
#include<stdlib.h>

using namespace std;

void OpenReadFile(string &ReadName, fstream &ReadFile) {
	ReadFile.open(ReadName.c_str(), ios::in);
	if(!ReadFile) cout << "Open " << ReadName << " failed.\n";
	else cout << "Open " << ReadName << " successful.\n";
}

void OpenWriteFile(string &WriteName, fstream &WriteFile) {
	WriteFile.open(WriteName.c_str(), ios::out | ios::trunc);
	if(!WriteFile) cout << "Open " << WriteName << " failed.\n";
	else cout << "Open " << WriteName << " successful.\n";
}

string parseDate(string &ReadName, fstream &ReadFile) {
	string line, date;
	OpenReadFile(ReadName, ReadFile);
	getline(ReadFile, line);
	getline(ReadFile, line);
	date.assign(line.substr(0,8));
	ReadFile.close();
	return date;
}

bool CommaExist(string &subline) {
	if(subline.find(',') != string::npos)
		return 1;
	else
		return 0;
}

string assignApkNane(string &subline) {
	string ApkName;
	if(CommaExist(subline))
		ApkName.assign(subline.substr(0, subline.find(',')));
	else
		ApkName.assign(subline.substr(0, subline.length()));
	return ApkName;
}

void calWakeTimes(string &subline, string ApkNames[100], fstream &WriteFile) {
	string ApkName;
	int ApkWakeTimes[100] = {0};
	int ApkIndex = 0;
	bool existFlag = 0;

	subline.erase(0, subline.find(',') + 1);        // Erase "total sleep time"

	while(!subline.empty()) {        // Parse APK
		ApkName.assign(assignApkNane(subline));
		existFlag = 0;
		for(int i = 0 ; i < 100 ; i++)        // Check if this APK already exist and find the index
			if(!ApkName.compare(ApkNames[i])) {
				existFlag = 1;
				ApkIndex = i;
				break;
			}
		if(existFlag) {        // Add wake-up time of the exist APK
			ApkWakeTimes[ApkIndex]++;
			cout << ApkNames[ApkIndex] << ',' << ApkWakeTimes[ApkIndex] << endl;
			if(CommaExist(subline)) subline.erase(0, subline.find(',') + 1);        // Erase this wake-up info of the recorded APK
			else subline.erase(subline.begin(), subline.end());
		}
		else {        // Add first wake-up time of the APK
			for(int i = 0 ; i < 100 ; i++) {
				if(ApkNames[i].empty()) {        // Add APK name to ApkNames & Add wake-up time to specific location of ApkWakeTimes
					ApkNames[i].assign(ApkName);
					ApkWakeTimes[i]++;
					cout << ApkNames[i] << ',' << ApkWakeTimes[i] << endl;
					if(CommaExist(subline))	subline.erase(0, subline.find(',') + 1);
					else subline.erase(subline.begin(), subline.end());
					break;
				}
			}
		}
	}
	for(int i = 0 ; ApkNames[i].empty() != 1 ; i++) WriteFile << ApkNames[i] << ',' << ApkWakeTimes[i] << '|';        // Write wake-up info
	WriteFile << endl;
}

int main(){
	string ReadName = "PowerCalculator@4316001.log";
	string WriteName;
	string WriteArg;
	string ArgName;
	fstream ReadFile;
	fstream WriteFile;
	fstream passArg;
	string LineCurrent;
	string LineLast;
	string subline;
	string ApkNames[100];
	string date;

	date.assign(parseDate(ReadName, ReadFile));
	WriteName = "WakeUpTimes_" + date +".log";
	ArgName = "ArgFile.txt";

	// Opren files
	OpenReadFile(ReadName, ReadFile);
	OpenWriteFile(WriteName, WriteFile);

	getline(ReadFile, LineCurrent);        // Ignore 1st line "NEW"
	getline(ReadFile, LineCurrent);        // Ignore 1st record
	while(getline(ReadFile, LineCurrent)) {        // Read line until EOF
		if(LineLast.length() >= 14) {
			subline.assign(LineLast.substr(LineLast.find('(') + 1, LineLast.find(')') - LineLast.find('(') - 1));        // Assign wake-up info to subline
			if(CommaExist(subline)) {        // Check if any wake-up APK exist
				WriteFile << LineLast.substr(0, LineLast.find(',')) << ';';        // Write time
				calWakeTimes(subline, ApkNames, WriteFile);        // Calculate the WakUpTimes of every APKs at this time
			}
		}
		LineLast.assign(LineCurrent);
	}
	ReadFile.close();
	WriteFile.close();

	WriteArg = "WakeUpTableConvertor.exe " + WriteName;
	system(WriteArg.c_str());

	return 0;
}
