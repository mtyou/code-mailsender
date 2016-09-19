#pragma once
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <vector>
class CTString
{
public:
	CTString();
	virtual ~CTString();

	std::wstring utf82unicode(std::string const& str);
	std::string unicode2utf8(std::wstring const& str);
	std::wstring ansi2unicode(std::string const& str);
	std::string unicode2ansi(std::wstring const& str);
	std::string utf82ansi(std::string const& str);
	std::string ansi2utf8(std::string const& str);

	std::string replace(const std::string& str, const std::string& oldValue, const std::string& newValue);
	std::string replaceAll(const std::string& str, const std::string& oldValue, const std::string& newValue);
	std::string getFullFileName(const std::string& path);
	std::string getFileName(const std::string& path);
	std::string getFileNameWithExt(const std::string& path);
	std::string getDirectory(const std::string& path);
	std::string getFileExt(const std::string& path);
	bool compareIgnoreCase(const std::string& str1, const std::string& str2);
	std::string format(const char* format, ...);

	std::vector<std::string> compact(const std::vector<std::string> &tokens);
	std::vector<std::string> split(const std::string &str, const std::string &delim, const bool trim_empty = false);
	std::string join(const std::vector<std::string> &tokens, const std::string &delim, const bool trim_empty = false);
	std::string rrim(const std::string &str);
	std::string repeat(const std::string &str, unsigned int times);
	std::string replaceAll2(const std::string &source, const std::string &target, const std::string &replacement);
	std::string toUpper(const std::string &str);
	std::string toLower(const std::string &str);
	std::string readFile(const std::string &filepath);
	void writeFile(const std::string &filepath, const std::string &content);

	bool isInt(const std::string& str);
	int toInt(const std::string& str, int defValue = -1);
	size_t count(const std::string& str, const std::string& s);
	bool isDouble(const std::string& str);
	double toDouble(const std::string& str);
	bool isAlpha(const std::string& str);
	bool isNumeric(const std::string& str);
	bool isAlphaNumeric(const std::string& str);
	std::string itos(long int number);
	std::string ftos(float number);

	std::string getTimeSecond();
	std::string getTimeMillisecond();
	double getTimeMillisecondD();
	std::string getAppPath();
	std::string getTempFileName();
	std::string getMiddle(const std::string& str, const std::string& start, const std::string& end);
	std::string getImitateIMEI();
	std::string getImitateImsi();
	std::string getImitateMac();
	std::string getMacId();
	std::string getCpuId();
};