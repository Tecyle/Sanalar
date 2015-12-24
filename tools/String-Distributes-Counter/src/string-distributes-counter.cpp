// string-distributes-counter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

vector<wstring> txtFileList;
map<wchar_t, size_t> charFreq;
size_t totalCharNum;
size_t mbCharNum;

const wchar_t* exceptChar = L"，。“”！？：、…‘’】【》《—　）（；┊「」 •①▲ǒ";

bool isInExceptChar(wchar_t ch)
{
	return (wcschr(exceptChar, ch) != NULL);
}

void findTxtFile(const wchar_t* lpPath)
{
	txtFileList.clear();

	wchar_t szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	wcscpy(szFind, lpPath);
	wcscat(szFind, L"*.txt");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;
	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else
		{
			txtFileList.push_back(FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	FindClose(hFind);
}

void CountChars(const wchar_t* file)
{
	FILE* fp;
	wchar_t ch;
	_wfopen_s(&fp, file, L"rb");
	if (!fp)
		return;
	while (!feof(fp))
	{
		fread_s(&ch, sizeof(wchar_t), 1, 2, fp);
		totalCharNum++;
		if (ch < L'\xff')
			continue;
		if (isInExceptChar(ch))
			continue;
		mbCharNum++;
		charFreq[ch]++;
	}
	fclose(fp);
}

bool comp(pair<wchar_t, size_t>& a, pair<wchar_t, size_t>& b)
{
	return a.second > b.second;
}

void WriteStringToFile(FILE* fp, const wchar_t* str)
{
	fwrite(str, sizeof(wchar_t)* wcslen(str), 1, fp);
}

void WriteCharToFile(FILE* fp, wchar_t ch)
{
	fwrite(&ch, sizeof(wchar_t), 1, fp);
}

void WriteNumberToFile(FILE* fp, size_t num)
{
	wchar_t n[MAX_PATH];
	_itow(num, n, 10);
	fwrite(n, sizeof(wchar_t)*wcslen(n), 1, fp);
}

void WriteNewlineToFile(FILE* fp)
{
	WriteStringToFile(fp, L"\r\n");
}
void GenerateResult()
{
	FILE* fp;
	fopen_s(&fp, "result.txt", "wb");
	char* bom = "\xff\xfe";
	fwrite(bom, 2, 1, fp);
	vector<pair<wchar_t, size_t>> charFreqList;
	for each(pair<wchar_t, size_t> p in charFreq)
	{
		charFreqList.push_back(p);
	}
	sort(charFreqList.begin(), charFreqList.end(), comp);
	WriteStringToFile(fp, L"共统计字符数：");
	WriteNumberToFile(fp, totalCharNum);
	WriteNewlineToFile(fp);
	WriteStringToFile(fp, L"共统计多字节字符数：");
	WriteNumberToFile(fp, mbCharNum);
	WriteNewlineToFile(fp);
	for each(pair<wchar_t, size_t> p in charFreqList)
	{
		WriteCharToFile(fp, p.first);
		WriteStringToFile(fp, L" : ");
		WriteNumberToFile(fp, p.second);
		WriteNewlineToFile(fp);
	}
	fclose(fp);
}

int _tmain(int argc, _TCHAR* argv[])
{
	totalCharNum = 0;
	mbCharNum = 0;
	findTxtFile(L"txt\\");
	for each(wstring str in txtFileList)
	{
		wstring fn = L"txt\\" + str;
		CountChars(fn.c_str());
	}
	GenerateResult();
	return 0;
}

