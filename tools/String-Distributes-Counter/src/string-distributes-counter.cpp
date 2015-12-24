// string-distributes-counter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

vector<wstring> txtFileList;
map<wchar_t, size_t> charFreq;
size_t totalCharNum;
size_t mbCharNum;

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
		mbCharNum++;
		charFreq[ch]++;
	}
	fclose(fp);
}

bool comp(pair<wchar_t, size_t>& a, pair<wchar_t, size_t>& b)
{
	return a.second > b.second;
}

void GenerateResult()
{
	vector<pair<wchar_t, size_t>> charFreqList;
	for each(pair<wchar_t, size_t> p in charFreq)
	{
		charFreqList.push_back(p);
	}
	sort(charFreqList.begin(), charFreqList.end(), comp);
	wcout << L"共统计字符数：" << totalCharNum << endl;
	wcout << L"共统计多字节字符数：" << mbCharNum << endl;
	for each(pair<wchar_t, size_t> p in charFreqList)
	{
		wcout << p.first << L" : " << p.second << endl;
	}
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

