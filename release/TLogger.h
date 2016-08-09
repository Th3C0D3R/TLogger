#include <fstream>
#include <string>
#include <Windows.h>
#include <unordered_map>
#include <zip.h>
#include <time.h>
using namespace std;

class TLogger {
private:

#pragma region Variables
	unordered_map<int, string> errorLevel = {
		{	0	,"CriticalError" },
		{ 	1	,"WriteError" },
		{ 	2	,"ReadError" },
		{ 	3	,"ConvertError" },
		{ 	4	,"DebugLog" },
		{ 	5	,"StatusLog" },
		{ 	6	,"OffsetLog" },
		{	8	,"Success" },
		{ 	7	,"Other" }
	};

	enum errorName
	{
		CriticalError = 0,
		WriteError = 1,
		ReadError = 2,
		ConvertError = 3,
		DebugLog = 4,
		StatusLog = 5,
		OffsetLog = 6,
		Success = 7,
		Other = 8
	};


	HZIP zipper;
	fstream LogFile, fileWriter;
	time_t now;
	tm* localtm;
	const char* logFileName = "/Log/logFile.log";
	char szFileName[32];
	bool success = false;
#pragma endregion

#pragma region privFunctions

	long getFileSize() {
		streampos begin, end;
		ifstream file(logFileName, ios::binary);
		if (file.good()) {
			begin = file.tellg();
			file.seekg(0, ios::end);
			end = file.tellg();
		}
		file.close();
		return (end - begin);
	}

	const char * getZipNameWithNumber(int i) {
		sprintf(szFileName, "/log/logZip%04i.zip", i);
		return szFileName;
	}

	void Write(int level, string errormsg) {
		LogFile = fstream(logFileName, fstream::app);
		std::unordered_map<int,string>::const_iterator got = errorLevel.find(level);
		errormsg = getLocalTime() + got->second + errormsg;
		LogFile.write(stringToConstChar(errormsg),sizeof(errormsg));
		LogFile.close();
	}

	bool logFileExists() {
		ifstream infile(logFileName);
		return infile.good();
	}

	bool logZipExists(int i){
		sprintf(szFileName, "/log/logZip%04i.zip", i);
		ifstream infile(szFileName);
		return infile.good();
	}

	string getLocalTime() {
		now = time(0);
		localtm = localtime(&now);
		string timeNow = asctime(localtm);
		return timeNow;
	}

	const char * stringToConstChar(string input) {
		return input.c_str();
	}

	char * stringToCharArray(string input) {
		char * writable = new char[input.size() + 1];
		copy(input.begin(), input.end(), writable);
		writable[input.size()] = '\0';
		return writable;
	}

	string CharToString(char * input) {
		return string(input);
	}

	string createZipName(int i) {
		string sZipMessage;
		char* cZipMessage;
		sZipMessage = "Zip - File successful created at : /log/logZip%04i.zip";
		cZipMessage = stringToCharArray(sZipMessage);
		sprintf(cZipMessage, "Zip - File successful created at : /log/logZip%04i.zip", i);
		return CharToString(cZipMessage);
	}

#pragma endregion

public:

	TLogger() {
		if (logFileExists()) {
			if (getFileSize() <= (1024 * 25)) {
				LogFile = fstream(logFileName, fstream::app);
			}
			else {
				int i= 0;
				while(!logZipExists(i))
				{
					i++;
				}
				zipper = CreateZip(getZipNameWithNumber(i),"");
				ZipAdd(zipper,logFileName,"logFile.log");
				CloseZip(zipper);
				Write(Success, createZipName(i));
				if( remove(logFileName) != 0 )
					Write(CriticalError,"Error deleting file");
				  else
					Write(Success,"File successfully deleted");
				LogFile = fstream(logFileName, fstream::app);
			}
			executePostConstructor();
		}
	}

void executePostConstructor(){
	LogFile.close;
	Write(DebugLog, "TLogger successful initialiazed");
}

	~TLogger() {

	}
};
