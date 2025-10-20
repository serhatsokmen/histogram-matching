#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "histogram_tools.h"

using namespace std;
using namespace cv;

int main() {

	try {
		// 1. Upload Images
		Mat inputImg = imread("input.jpg", IMREAD_COLOR);
		Mat refImg = imread("reference.jpg", IMREAD_COLOR);

		if (inputImg.empty() || refImg.empty()) {
			cerr << "ERROR: input.jpg or reference.jpg not found/read!" << endl;
			return -1;
		}

		cout << "Images loaded succesfully! Starting Histogram Equalization.." << endl;

		// 2. Separation Into RGB Channels
		vector<cv::Mat> input_bgr, ref_bgr;
		vector<cv::Mat> output_bgr(3); 

		// The split function, splits the Mat object into B, G, R channels.  
		cv::split(inputImg, input_bgr); // input_bgr[0] = Blue, [1] = Green, [2] = Red
		cv::split(refImg, ref_bgr); //	   ref_bgr[0] = Blue, [1] = Green, [2] = Red


		for (int i = 0; i < 3; ++i)
		{
			// veri yapilarini her kanal icin hazirlama:
			vector<int> input_hist, ref_hist;
			vector<float> input_cdf, ref_cdf;
			vector<int> mapping_table;

			string channel_name;
			if (i == 0) channel_name = "BLUE (B)";
			else if (i == 1) channel_name = "GREEN (G)";
			else channel_name = "RED (R)";

			cout << "\n----" << channel_name << "Kanali Isleniyor ---" << endl;



			// Input ve Reference kanallari icin Histogram ve CDF hesaplama
			calculate_hist_and_cdf(input_bgr[i], input_hist, input_cdf);
			calculate_hist_and_cdf(ref_bgr[i], ref_hist, ref_cdf);
			cout << channel_name << " Histogram ve CDF Hesaplamalari Tamamlandi." << endl;


			// Haritalama Tablosunu Olusturma
			create_mapping_table(input_cdf, ref_cdf, mapping_table);
			cout << channel_name << " Haritalama Tablosu Olusturuldu." << endl;


			// Haritalama Tablosunu Input Kanalina Uygulama
			output_bgr[i] = apply_mapping(input_bgr[i], mapping_table);
			cout << channel_name << " Kanali Donusturuldu." << endl;


			// Renkleri BGR sirasina gore ayarlamak
			cv::Scalar color;
			if (i == 0) color = cv::Scalar(255, 0, 0);      // Blue (B)
			else if (i == 1) color = cv::Scalar(0, 255, 0); // Green (G)
			else color = cv::Scalar(0, 0, 255);             // Red (R)

			
			// Input Goruntusunun orijinal Histogrami
			draw_histogram(input_hist, channel_name + " - Input Hist", color);

			// Reference Goruntusunun Histogrami
			draw_histogram(ref_hist, channel_name + " - Reference Hist", color);


			// Yeni Olusan Kanaldan Histogrami Hesapla ve Ciz
			vector<int> output_hist;
			vector<float> dummy_cdf; // sadece hesaplama icin bos degisken
			calculate_hist_and_cdf(output_bgr[i], output_hist, dummy_cdf);

			draw_histogram(output_hist, channel_name + "- Output Hist", color);

		}


		// Yeni Kararlari Birlestirerek Nihai Goruntuyu Olusturma
		cv::Mat outputImg;
		// merge fonksiyonu ile B, G, R kanallarini alip tek bir 3 kanalli Mat nesnesi olusturma
		cv::merge(output_bgr, outputImg);


		// Sonucu Gosterme
		cv::imshow("1- Input Original", inputImg);
		cv::imshow("2- Reference", refImg);
		cv::imshow("3- Output Matched", outputImg);


		// Sonuc Goruntusunu Dosyaya Kaydetme
		cv::imwrite("output_matched.jpg", outputImg);
		cout << "Sonuc Goruntusu 'output_matched.jpg' olarak kaydedildi!" << endl;

		cv::waitKey(0);

		cout << "\n-----------------" << endl;
		cout << "Goruntu Isleme Basariyla Tamamlandi!" << endl;
		cout << "--------------------" << endl;
		

	}
	catch (const Exception& ex) {
		cerr << "OpenCV Error!" << ex.what() << endl;
		return -1;
	}
	catch (const exception& ex) {
		cerr << "General Error!" << ex.what() << endl;
		return -1;
	}

	return 0;

}



