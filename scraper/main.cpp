#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Scraper.h"

#define DATA_PATH "D:/Work/tretton37/scraper/Data"

#define BASE_FILE_TOP "tretton37"
#define BASE_URL_SECURE "https://www.tretton37.com"

int main()
{
	Scraper scraper;

	scraper.Initialize(BASE_URL_SECURE, DATA_PATH, BASE_FILE_TOP);

	scraper.Scrape();

	return 0;
}