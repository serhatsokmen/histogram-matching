#pragma once
#include <vector>
#include <cmath>
#include <algorithm> 
#include <iostream>
#include <opencv2/core.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


/*
* @brief Tek bir kanal goruntusu icin histogrami (piksel sayimini) ve CDF' i hesaplar. 
*
* @param channel: Islemin yapilacagi tek kanalli cv::Mat (input veya reference)
* @param hist_data: 256 boyutlu, her parlaklik seviyesindeki piksel sayisini (nk) tutan cikti vektoru.
* @param cdf: 256 boyutlu, kumulatif dagilim fonksiyonu (CDF) degerlerini tutan cikti vektoru
*/
void calculate_hist_and_cdf(
	const cv::Mat& channel,
	std::vector<int>& hist_data,
	std::vector<float>& cdf

);


/*
* @brief Input CDF ' yi Refernce CDF ' ye esleyen haritalama (mapping) tablosunu olusturur.
* 
* @param input_cdf: Input goruntusunun kanalina ait 256 boyutlu CDF vektoru.
* @param ref_cdf: Reference goruntusunun kanalina ait 256 boyutlu CDF vektoru.
* @param mapping_table: Ýnput' tan Reference' a donusum degerlerini tutan 256 boyutlu cikti vektoru.
*/
void create_mapping_table(
	const std::vector<float>& input_cdf,
	const std::vector<float>& ref_cdf,
	std::vector<int>& mapping_table
);


/*
* @brief Olusturulan Haritalama tablosunu Input kanal goruntusune uygular.
* 
* @param input_channel: Donusumun uygulanacagi orijinal Input kanali.
* @param mapping_table: Kullanilacak 256 boyutlu Haritalama tablosu.
* @return cv::Mat : Islemi yapilmis cikti kanali goruntusu.
*/
cv::Mat apply_mapping(
	const cv::Mat& input_channel,
	const std::vector<int>& mapping_table
);


/*
* @brief Histogram verilerini normalize eder ve cv::Mat uzerinde cizer.
* 
* @param hist_data: Cizilecek 256 boyutlu piksel sayimi (nk) verisi.
* @param title: Grafik penceresinin basligi.
* @param color: Grafik cizgisinin rengi (BGR formatinda cv::Scalar).
*/
void draw_histogram(
	const std::vector<int>& hist_data,
	const std::string& title,
	const cv::Scalar& color
);