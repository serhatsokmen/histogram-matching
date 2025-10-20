#include "histogram_tools.h"

void calculate_hist_and_cdf(
	const cv::Mat& channel,
	std::vector<int>& hist_data,
	std::vector<float>& cdf

)
{

	const int intensity_levels = 256; // parlaklik seviyesi (0'dan 255'e)

	// vektorlerin boyutlarini 256 olarak ayarlayip sifir ile baslatma islemi
	hist_data.assign(intensity_levels, 0); // piksel sayimi (nk)
	cdf.assign(intensity_levels, 0.0f); //	  kumulatif dagilim fonksiyonu (CDF)


	// 1. Histogram Hesaplama (piksel sayimi - nk)
	long long total_pixels = 0;

	// Goruntunun her pikselinde gezmek icin:
	for (int i = 0; i < channel.rows; ++i)
	{
		// Mat::ptr<uchar>(i) i. satirin baslangic adresini verir.
		// uchar (unsigned char) 0-255 araligindaki piksel degerlerini tutar
		const uchar* row_ptr = channel.ptr<uchar>(i);

		for (int j = 0; j < channel.cols; ++j)
		{
			uchar pixel_value = row_ptr[j]; // pikselin parlaklik degerini oku (0-255)

			// bu piksel degerine karsilik gelen historam bin' ini arttir.
			hist_data[pixel_value]++;
			total_pixels++;
		}
	}

	// eger goruntu bos ise cikis yap
	if (total_pixels == 0) return;



	// 2. Kumulatif Dagilim Fonksiyonu (CDF) Hesaplama

	// histogram verilerini olasilik dagilimi seklinde normalize etmek
	float cumulative_sum = 0.0f;

	for (int k = 0; k < intensity_levels; ++k)
	{
		// Olasilik: P(rk) = nk / N_toplam
		float pdf_k = (float)hist_data[k] / total_pixels;

		// kumulatif toplami guncelle (CDF)
		cumulative_sum += pdf_k;

		// CDF(k) = Toplam(P(rj)) j = 0' dan k ' ya.
		cdf[k] = cumulative_sum;
	}


}






/*
* @brief Input CDF ' yi Reference CDF ' ye esleyen mapping tablosunu olusturur.
* @param input_cdf: Input goruntusunun kanalina ait 256 boyutlu CDF vektoru.
* @param ref_cdf: Reference goruntusunun kanalina ait 256 boyutlu CDF vektoru.
* @param mapping_table: Input' tan gelen Reference' a donusum degerlerini tutan 256 boyutlu cikti vektoru.
*/
void create_mapping_table(
	const std::vector<float>& input_cdf,
	const std::vector<float>& ref_cdf,
	std::vector<int>& mapping_table
)
{
	const int intensity_levels = 256;
	mapping_table.assign(intensity_levels, 0); // 256 elemanli mapping tablosunu sifirla

	// Input CDF (CDF_r) uzerindeki her parlaklik seviyesi (r_k) icin dongu baslatilir (k=0' dan 255' e)
	for (int k = 0; k < intensity_levels; ++k)
	{
		float S_k_input = input_cdf[k];


		// En kucuk mutlak farki ve bu farka karsilik gelen referans parlaklik seviyesini (r_j) tutmak icin degiskenler
		float min_diff = 2.0f; // CDF 0-1 araliginda oldugu icin 2.0f baslangic degeri guvenlidir.
		int matched_intensity = 0; // Eslesen parlaklik seviyesi (j degeri)


		// Reference CDF uzerinde arama dongusu baslatilir
		for (int j = 0; j < intensity_levels; ++j)
		{
			float S_j_ref = ref_cdf[j]; // Reference CDF' in j. degeri

			//mutlak fark (CDF_input - CDF_reference)
			float current_diff = std::abs(S_k_input - S_j_ref);

			if (current_diff < min_diff)
			{
				min_diff = current_diff;
				matched_intensity = j;
			}

		}

		// Haritalama tablosunu doldurma:
		mapping_table[k] = matched_intensity;
	}
}


/*
* @brief Olusturulan haritalama tablosunu Input kanal goruntusune uygular.
* @param input_channel: Donusunumun uygulanacagi orijinal input kanali (cv::Mat).
* @param mapping_table: Kullanilacak 256 boyutlu haritalama tablosu.
* @return cv::Mat : Islemi yapilmis cikti kanali goruntusu.
*/
cv::Mat apply_mapping(
	const cv::Mat& input_channel,
	const std::vector<int>& mapping_table
)
{

	// yeni cikti kanalini tutacak cv::Mat nesnesi olusturma islemi.
	cv::Mat output_channel = input_channel.clone();

	// goruntunun her pikselinde gezmek icin:
	for (int i = 0; i < input_channel.rows; ++i)
	{
		const uchar* input_ptr = input_channel.ptr<uchar>(i); // i. satirin baslangic adresi (sadece okuma)

		uchar* output_ptr = output_channel.ptr<uchar>(i); // i. satirin baslangic adresi (yazma)

		for (int j = 0; j < input_channel.cols; ++j)
		{
			uchar old_value = input_ptr[j]; // 1. orijinalpiksel degerini al (r_k)

			uchar new_value = (uchar)mapping_table[old_value]; // 2. haritalama tablosundan yeni degeri bul (r_j)

			output_ptr[j] = new_value; // 3. yeni degeri cikti kanalina yaz.
		}
	}

	return output_channel;

}




/*
* @brief Histogram verilerini normalize eder ve cv::Mat uzerine cizer (gorsellestirme)
* @param hist_data: Cizilecek 256 boyutlu piksel sayimi (nk) verisi.
* @param title: Grafik penceresinin basligi.
* @param color: Grafik cizgisinin rengi (BGR formatinda cv::Scalar).
*/
void draw_histogram(
	const std::vector<int>& hist_data,
	const std::string& title,
	const cv::Scalar& color
)
{
	const int hist_w = 512; // pencere genisligi
	const int hist_h = 400; // pencere yuksekligi
	const int bin_w = cvRound((double)hist_w / 256); // her sutunun genisligi (256 seviye icin)

	int max_hist = *std::max_element(hist_data.begin(), hist_data.end()); // histogramin tepe noktasini bulup normalize etmek icin
	if (max_hist == 0) max_hist = 1; 

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255)); // 8 bit, 3 kanalli (renkli) ve beyaz arka planli bir mat olusturma

	std::vector<int> hist_normalized(256); // histogram verilerini goruntu yuksekligine gore normalize etme
	for (int k = 0; k < 256; ++k)
	{
		hist_normalized[k] = cvRound(((double)hist_data[k] / max_hist) * hist_h); // Degeri normalize etme (mevcut deger / max deger) * (goruntu yuksekligi)
	}

	// Histogrami Cizme
	for (int k = 1; k < 256; ++k)
	{
		// line fonksiyonu, iki nokta arasina cizgi ceker.
		cv::line(histImage,
			// Baslangic noktasi: (k-1) sutunu, yukseklikten (normalized) cikarilarak ters cevrilir.
			cv::Point(bin_w * (k - 1), hist_h - hist_normalized[k - 1]),
			// Bitis noktasi: k sutunu, yukseklikten (normalized) cikarilarak ters cevrilir.
			cv::Point(bin_w * (k), hist_h - hist_normalized[k]),
			color, // Cizgi rengi (Gelen parametre)
			2,     // Cizgi kalinligi
			8, 0);
	}

	// pencereyi gosterme
	cv::imshow(title, histImage);
}
