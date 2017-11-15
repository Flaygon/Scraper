#pragma once

#include "curl/curl.h"

#include <string>
#include <vector>

class Scraper
{
public:
	Scraper();
	~Scraper();

	void Initialize(std::string anURL, std::string aDataPath, std::string aTopDataPathName);

	void Scrape();

private:
	std::string URL = "";
	std::string dataPath = "";
	std::string topDataPathName = "";

	struct ScrapedData
	{
		bool EqualURL(const ScrapedData& right)
		{
			return siteRef == right.siteRef;
		}
		bool EqualURL(const ScrapedData*const right)
		{
			return siteRef == right->siteRef;
		}
		std::string siteRef = "";
		std::string data = "";
		std::string ending = ".html";
	};

	std::vector<ScrapedData*> toBeScrapedURLs;
	std::vector<ScrapedData*> toBeScrapedImages;
	std::vector<ScrapedData*> toBeScrapedFiles;

	std::vector<ScrapedData*> scrapedURLs;
	std::vector<ScrapedData*> scrapedImages;
	std::vector<ScrapedData*> scrapedFiles;

	std::vector<ScrapedData*> writtenURLs;
	std::vector<ScrapedData*> writtenImages;
	std::vector<ScrapedData*> writtenFiles;

	void DownloadURL(ScrapedData* data);
	void DownloadImage(ScrapedData* data);
	void DownloadFile(ScrapedData* data);

	void WriteURL(ScrapedData* data);
	void WriteImage(ScrapedData* data);
	void WriteFile(ScrapedData* data);
	void WriteData(ScrapedData* data, std::string writeConfig);

	void CheckError(std::string prefix, CURLcode errorCode);

	bool FindContent(int& offset, const std::string& word, const std::string& scrapedData, std::string& foundContent);
	bool ValidateSiteRef(const std::string& toValidate);
	bool ValidatePicture(const std::string& toValidate);
	bool ValidateFile(const std::string& toValidate);

	void CleanData(ScrapedData* dataToClean);
	void ExtractContent(const std::string& scrapedData);

	bool IsDuplicate(ScrapedData* data, const std::vector<ScrapedData*>& toBeScraped, const std::vector<ScrapedData*>& scraped, const std::vector<ScrapedData*>& written);
};