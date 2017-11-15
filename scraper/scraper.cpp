#include "stdafx.h"

#include "Scraper.h"

#include <iostream>
#include <fstream>
#include <assert.h>

Scraper::Scraper()
{
	curl_global_init(CURL_GLOBAL_ALL);
}
Scraper::~Scraper()
{
	curl_global_cleanup();
}

void Scraper::Initialize(std::string anURL, std::string aDataPath, std::string aTopDataPathName)
{
	this->URL = anURL;
	this->dataPath = aDataPath;
	this->topDataPathName = aTopDataPathName;
}

void Scraper::Scrape()
{
	ScrapedData* baseURL = new ScrapedData();
	baseURL->siteRef = "/";
	DownloadURL(baseURL);

	while (toBeScrapedURLs.size() > 0)
	{
		DownloadURL(toBeScrapedURLs[0]);
		WriteURL(scrapedURLs[0]);
	}
	while (toBeScrapedImages.size() > 0)
	{
		DownloadImage(toBeScrapedImages[0]);
		WriteImage(scrapedImages[0]);
	}
	while (toBeScrapedFiles.size() > 0)
	{
		DownloadFile(toBeScrapedFiles[0]);
		WriteFile(scrapedFiles[0]);
	}
}

size_t IntermediaryCallbackBridge(char *ptr, size_t size, size_t nmemb, void *userData)
{
	std::string* scrapedData = (std::string*)userData;
	scrapedData->append(ptr, nmemb);

	return size * nmemb;
}

void Scraper::DownloadURL(ScrapedData* data)
{
	CURL* scraper = curl_easy_init();
	CURLcode status;

	scrapedURLs.push_back(data);
	for (int iScraped = 0; iScraped < toBeScrapedURLs.size(); ++iScraped)
	{
		if (toBeScrapedURLs[iScraped]->EqualURL(data))
		{
			toBeScrapedURLs.erase(toBeScrapedURLs.begin() + iScraped);
		}
	}

	if (scraper)
	{
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEFUNCTION, IntermediaryCallbackBridge);
		CheckError("Setup Write Function Callback: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEDATA, (void*)&data->data);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_FOLLOWLOCATION, 1L);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_URL, (URL + data->siteRef).c_str());
		CheckError("Setup URL Path: ", status);
		status = curl_easy_perform(scraper);
		CheckError("Perform: ", status);

		CleanData(data);
		ExtractContent(data->data);

		/*char* infoData;
		curl_easy_getinfo(scraper, CURLINFO::CURLINFO_EFFECTIVE_URL, &infoData);*/
	}

	curl_easy_cleanup(scraper);
}

void Scraper::DownloadImage(ScrapedData* data)
{
	CURL* scraper = curl_easy_init();
	CURLcode status;

	scrapedImages.push_back(data);
	for (int iScraped = 0; iScraped < toBeScrapedImages.size(); ++iScraped)
	{
		if (toBeScrapedImages[iScraped]->EqualURL(data))
		{
			toBeScrapedImages.erase(toBeScrapedImages.begin() + iScraped);
		}
	}

	if (scraper)
	{
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEFUNCTION, IntermediaryCallbackBridge);
		CheckError("Setup Write Function Callback: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEDATA, (void*)&data->data);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_FOLLOWLOCATION, 1L);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_URL, (URL + data->siteRef + data->ending).c_str());
		CheckError("Setup URL Path: ", status);
		status = curl_easy_perform(scraper);
		CheckError("Perform: ", status);
	}

	curl_easy_cleanup(scraper);
}

void Scraper::DownloadFile(ScrapedData* data)
{
	CURL* scraper = curl_easy_init();
	CURLcode status;

	scrapedFiles.push_back(data);
	for (int iScraped = 0; iScraped < toBeScrapedFiles.size(); ++iScraped)
	{
		if (toBeScrapedFiles[iScraped]->EqualURL(data))
		{
			toBeScrapedFiles.erase(toBeScrapedFiles.begin() + iScraped);
		}
	}

	if (scraper)
	{
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEFUNCTION, IntermediaryCallbackBridge);
		CheckError("Setup Write Function Callback: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_WRITEDATA, (void*)&data->data);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_FOLLOWLOCATION, 1L);
		CheckError("Setup Write Data end: ", status);
		status = curl_easy_setopt(scraper, CURLoption::CURLOPT_URL, (URL + data->siteRef + data->ending).c_str());
		CheckError("Setup URL Path: ", status);
		status = curl_easy_perform(scraper);
		CheckError("Perform: ", status);
	}

	curl_easy_cleanup(scraper);
}

void Scraper::WriteURL(ScrapedData* data)
{
	writtenURLs.push_back(data);
	for (int iScraped = 0; iScraped < scrapedURLs.size(); ++iScraped)
	{
		if (scrapedURLs[iScraped]->EqualURL(data))
		{
			scrapedURLs.erase(scrapedURLs.begin() + iScraped);
			break;
		}
	}

	WriteData(data, "w");
}

void Scraper::WriteImage(ScrapedData* data)
{
	writtenImages.push_back(data);
	for (int iScraped = 0; iScraped < scrapedImages.size(); ++iScraped)
	{
		if (scrapedImages[iScraped]->EqualURL(data))
		{
			scrapedImages.erase(scrapedImages.begin() + iScraped);
			break;
		}
	}

	WriteData(data, "wb"/*"ab"*/);
}

void Scraper::WriteFile(ScrapedData* data)
{
	writtenFiles.push_back(data);
	for (int iScraped = 0; iScraped < scrapedFiles.size(); ++iScraped)
	{
		if (scrapedFiles[iScraped]->EqualURL(data))
		{
			scrapedFiles.erase(scrapedFiles.begin() + iScraped);
			break;
		}
	}

	WriteData(data, "w");
}

void Scraper::WriteData(ScrapedData* data, std::string writeConfig)
{
	FILE* image;
	std::string combinedURL = dataPath + data->siteRef + data->ending;
	errno_t error = 0;
	error = fopen_s(&image, combinedURL.c_str(), writeConfig.c_str());
	if (error != 0)
	{
		std::string directoryChecker = combinedURL;
		std::vector<std::string> checkedDirectories;
		do
		{
			int lastSlash = directoryChecker.find_last_of('/');
			directoryChecker.erase(directoryChecker.begin() + lastSlash, directoryChecker.end());

			if (CreateDirectory(std::wstring(directoryChecker.begin(), directoryChecker.end()).c_str(), 0))
			{
				for (int iDirectory = checkedDirectories.size() - 1; iDirectory > -1; --iDirectory)
				{
					CreateDirectory(std::wstring(checkedDirectories[iDirectory].begin(), checkedDirectories[iDirectory].end()).c_str(), 0);
				}
			}
			else
				checkedDirectories.push_back(directoryChecker);
		} while (fopen_s(&image, combinedURL.c_str(), writeConfig.c_str()) != 0);
	}

	fwrite((void*)data->data.c_str(), sizeof(char), data->data.length(), image);

	fclose(image);
}

bool Scraper::FindContent(int& offset, const std::string& word, const std::string& scrapedData, std::string& foundContent)
{
	bool found = false;
	if ((offset = scrapedData.find(word, offset)) != std::string::npos)
	{
		offset += word.length() + 1;
		foundContent = scrapedData.substr(offset, scrapedData.find_first_of('\"', offset) - offset);
		offset += foundContent.length();
		found = true;
	}
	return found;
}

bool Scraper::ValidateSiteRef(const std::string& toValidate)
{
	bool valid = true;
	if (valid && toValidate == "")
		valid = false;
	if (valid && toValidate.find("/") == std::string::npos)
		valid = false;
	if (valid && toValidate.find("#") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("http://") != std::string::npos || toValidate.find("https://") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("javascript:") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".html") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".css") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".txt") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".jpg") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".png") != std::string::npos)
		valid = false;
	if (valid && toValidate.find(".svg") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("facebook") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("google") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("instagram") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("flickr") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("twitter") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("linkedin") != std::string::npos)
		valid = false;
	if (valid && toValidate.find("pinterest") != std::string::npos)
		valid = false;
	return valid;
}

bool Scraper::ValidatePicture(const std::string& toValidate)
{
	bool valid = false;
	if (toValidate != "")
	{
		if (toValidate.find(".jpg") != std::string::npos)
			valid = true;
		else if (toValidate.find(".png") != std::string::npos)
			valid = true;
		else if (toValidate.find(".svg") != std::string::npos)
			valid = true;
	}
	return valid;
}

bool Scraper::ValidateFile(const std::string& toValidate)
{
	bool valid = false;
	if (toValidate != "")
	{
		if (toValidate.find(".txt") != std::string::npos)
			valid = true;
		else if (toValidate.find(".css") != std::string::npos)
			valid = true;
	}
	return valid;
}

void Scraper::CleanData(ScrapedData* someData)
{
	int cleanIndex = 0;
	while ((cleanIndex = someData->data.find("<!--")) != std::string::npos)
	{
		int cleanEndIndex = someData->data.find("-->", cleanIndex) + 3;
		someData->data.erase(someData->data.begin() + cleanIndex, someData->data.begin() + cleanEndIndex);
	}
}

void Scraper::ExtractContent(const std::string& scrapedData)
{
	std::string foundContent;
	int searchIndex = 0;
	std::string searchWord = "href=";
	while (FindContent(searchIndex, searchWord, scrapedData, foundContent))
	{
		bool siteRef = ValidateSiteRef(foundContent);
		bool picture = ValidatePicture(foundContent);
		bool file = ValidateFile(foundContent);
		if (siteRef || picture || file)
		{
			//int foundContentEntry = searchIndex - foundContent.length();

			int iBracket = foundContent.find_first_of('{');
			if (iBracket != std::string::npos)
				foundContent.erase(foundContent.begin() + iBracket, foundContent.begin() + (foundContent.find_first_of('}') + 1));

			if (foundContent[0] != '/')
				foundContent.insert(0, "/");

			if (foundContent[foundContent.length() - 1] == '/')
				foundContent.erase(foundContent.end() - 1, foundContent.end());

			ScrapedData* newData = new ScrapedData();
			newData->siteRef = foundContent;

			if (siteRef)
			{
				if (!IsDuplicate(newData, toBeScrapedURLs, scrapedURLs, writtenURLs))
					toBeScrapedURLs.push_back(newData);
			}
			else if (picture)
			{
				int ending = foundContent.find_last_of('.');
				newData->ending = foundContent.substr(ending);
				newData->siteRef.erase(newData->siteRef.begin() + ending, newData->siteRef.end());

				if (!IsDuplicate(newData, toBeScrapedImages, scrapedImages, writtenImages))
					toBeScrapedImages.push_back(newData);
			}
			else
			{
				bool assigned = false;
				int fileEnding = 0;
				if (!assigned && (fileEnding = newData->siteRef.find(".css")) != std::string::npos)
				{
					newData->siteRef.erase(newData->siteRef.begin() + fileEnding, newData->siteRef.end());
					newData->ending = ".css";

					assigned = true;
				}
				if (!assigned && (fileEnding = newData->siteRef.find(".txt")) != std::string::npos)
				{
					newData->siteRef.erase(newData->siteRef.begin() + fileEnding, newData->siteRef.end());
					newData->ending = ".txt";

					assigned = true;
				}

				if (!IsDuplicate(newData, toBeScrapedFiles, scrapedFiles, writtenFiles))
					toBeScrapedFiles.push_back(newData);
			}

			//scrapedData.insert(foundContentEntry, DATA_PATH);
		}
	}

	searchIndex = 0;
	searchWord = "content=";
	while (FindContent(searchIndex, searchWord, scrapedData, foundContent))
	{
		bool siteRef = ValidateSiteRef(foundContent);
		bool picture = ValidatePicture(foundContent);
		bool file = ValidateFile(foundContent);
		if (siteRef || picture || file)
		{
			//int foundContentEntry = searchIndex - foundContent.length();

			int index = foundContent.find(".com");
			if (index != std::string::npos)
				foundContent.erase(foundContent.begin(), foundContent.begin() + index + 4);

			ScrapedData* newData = new ScrapedData();
			newData->siteRef = foundContent;
			
			if (siteRef)
			{
				if (!IsDuplicate(newData, toBeScrapedURLs, scrapedURLs, writtenURLs))
					toBeScrapedURLs.push_back(newData);
			}
			else if(picture)
			{
				int ending = foundContent.find_last_of('.');
				newData->ending = foundContent.substr(ending);
				newData->siteRef.erase(newData->siteRef.begin() + ending, newData->siteRef.end());

				if (!IsDuplicate(newData, toBeScrapedImages, scrapedImages, writtenImages))
					toBeScrapedImages.push_back(newData);
			}
			else
			{
				bool assigned = false;
				int fileEnding = 0;
				if (!assigned && (fileEnding = newData->siteRef.find(".css")) != std::string::npos)
				{
					newData->siteRef.erase(newData->siteRef.begin() + fileEnding, newData->siteRef.end());
					newData->ending = ".css";

					assigned = true;
				}
				if (!assigned && (fileEnding = newData->siteRef.find(".txt")) != std::string::npos)
				{
					newData->siteRef.erase(newData->siteRef.begin() + fileEnding, newData->siteRef.end());
					newData->ending = ".txt";

					assigned = true;
				}

				if(!IsDuplicate(newData, toBeScrapedFiles, scrapedFiles, writtenFiles))
					toBeScrapedFiles.push_back(newData);
			}

			//scrapedData.insert(foundContentEntry, DATA_PATH);
		}
	}
}

void Scraper::CheckError(std::string prefix, CURLcode errorCode)
{
	if (errorCode != CURLcode::CURLE_OK)
	{
		std::cout << prefix << curl_easy_strerror(errorCode) << std::endl;
		assert(1);
	}
}

bool Scraper::IsDuplicate(ScrapedData* data, const std::vector<ScrapedData*>& toBeScraped, const std::vector<ScrapedData*>& scraped, const std::vector<ScrapedData*>& written)
{
	bool exist = false;

	for (int iScraped = 0; iScraped < toBeScraped.size() && !exist; ++iScraped)
	{
		if (toBeScraped[iScraped]->EqualURL(data))
		{
			exist = true;
		}
	}
	for (int iScraped = 0; iScraped < scraped.size() && !exist; ++iScraped)
	{
		if (scraped[iScraped]->EqualURL(data))
		{
			exist = true;
		}
	}
	for (int iWritten = 0; iWritten < written.size() && !exist; ++iWritten)
	{
		if (written[iWritten]->EqualURL(data))
		{
			exist = true;
		}
	}

	return exist;
}