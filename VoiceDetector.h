#pragma once
#include <vector>
#include <Vfw.h>
// First, we'll define the WAV file format.
#include <pshpack1.h>
#include <poppack.h>
#include <atlstr.h>
#include <memory>
#define NO_DELETE

#define MinCDT  (1./(24.*60.))
 #define M_PI 3.1415927
//https://elemyo.com/podderjka/info_ispolzovanie/bandpass
#ifndef ELEMYO_h

#define ELEMYO_h

#define GAIN    B01000000     // write gain register

#define x1     B000           // x1  gain
#define x2     B001           // x2  gain
#define x4     B010           // x4  gain
#define x5     B011           // x5  gain
#define x8     B100           // x8  gain
#define x10    B101           // x10 gain
#define x16    B110           // x16 gain
#define x32    B111           // x32 gain

class BANDPASS {
public:
	float X[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };		// array for BandPass filter with x(n-1) ... x(n-8) values
	float Y[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// array for BandPass filter with y(n) ... y(n-8) values
	long T = 0;					// time in microseconds for BandPass filter
	float fl = 0;					// lower cutoff frequency in Hz
	float fh = 0;					// upper cutoff frequency in Hz

	void INITIAL();					// initilisation
};

class ELEMYO {
public:

	int LengthBP = 0;				// number of bandpass filters with different lower or upper cutoff frequencys
	BANDPASS* BP = (BANDPASS*)malloc(1);		// array of bandpass filters with different lower or upper cutoff frequencys

	int LengthMA = 10;                                  // length of moving average massive
	float MA[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };      // array for moving average method

	// 4th order bandpass filter, "sensorValue" - signal value, "fl" - lower cutoff frequency in Hz (≥30 Hz), "fh" - upper cutoff frequency in Hz (fh≥(fl+10)),
	// "type" - filter type: 0 - Butterworth, 1 - Chebyshev with 0.5 dB ripple, 2 - Chebyshev with 1 dB ripple
	int BandPass(int sensorValue, float fl, float fh, int type, float fs);

private:
	byte _cs;			// chip select pin
	int sensorInPin;		// chip select pin
};

#endif





struct BandPassParams {
	unsigned int nLowBorder = 210; //Low border of bAND
	unsigned int nHighBorder = 1010;//High border of the band
	unsigned int nOrder = 4; //Order of the filter, must be multiplied by 4 (4,8,12,16,20,24.....)
	unsigned int nDiscrFreq = 22050; //Frequency of our signal
	float epsilon = 1; //float value [0;1], if 1, then amplitude will be bigger, than way with epsilon =0
	BandPassParams() :nLowBorder(210), nHighBorder(1010), nOrder(4), nDiscrFreq(22050), epsilon(1.) {};
	bool SetParams(unsigned int low, unsigned int high, unsigned int order, float eps = 1.) {
		if ((order < 4) || (order > 32) || (order % 4))
			order = 4;
		if ((eps < 0.1) || (eps > 1.))
			eps = 1.;

		if ((low >= high) || (high * 2.1 > nDiscrFreq))
			return false;
		nLowBorder = low;
		nHighBorder = high;
		nOrder = order;
		epsilon = eps;
		return true;
	}
	
	BandPassParams(unsigned int low, unsigned int high, unsigned int order, float eps = 1.) {
		SetParams(low, high, order, eps);
	}

};

//https://github.com/adis300/filter-c/blob/master/filter.c
//https://exstrom.com/journal/sigproc/dsigproc.html
struct CHEBandPass { //
	CHEBandPass(BandPassParams& InitParams);
	~CHEBandPass() {
		this->free();
	}
	int m;
	float ep;
	float* A;
	float* d1;
	float* d2;
	float* d3;
	float* d4;
	float* w0;
	float* w1;
	float* w2;
	float* w3;
	float* w4;
	void free();
	float PassValue(float input);
};








struct WAVHEADER
{
	char chunkId[4]; //RIFF    
	unsigned long chunkSize;
	char format[4]; //WAVE      
	char subchunk1Id[4]; //fmt        
	unsigned long subchunk1Size; //18

	unsigned short audioFormat; //wFormatTag    
	unsigned short numChannels;
	unsigned long sampleRate; //Hz    
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	unsigned short cbSize;
};
const UINT WAVHEADER_SIZE = 36;
struct WAV_DATA_CHUNK
{
	char subchunk2Id[4]; //data    
	unsigned long subchunk2Size;
	// Далее следуют данные...
};

struct Channel
{

	double* Amplitudes;
	unsigned long size;
	unsigned long SamplesCount=0;
	double Max, Min;
	double  Medium;
	Channel() :size(0), Max(-256.), Min(MAXINT), Medium(0.), SamplesCount(0){
	};
	~Channel() {
	}
	
	void Reset() {

		if (size) {
			size = 0;
			Max = -256.;
			Min = MAXINT;
			Medium = 0;
			delete[] Amplitudes;
		}
	}
};

struct WavFile {
public:
	Channel m_Channel[2]; //Два основных канала c замплами

	bool InRange(unsigned long SampleNumber) {
		return ((SampleNumber < FinishPoint) && (SampleNumber >= StartPoint));
	}
	std::string FilePath; //Путь до нашего файла
	WAVEFORMATEX  wfx;// Набиваем из считанной структуры
	WAVHEADER m_wfh; //Наш Header 
	bool m_bExistVoice = false;
	unsigned long StartPoint = 0;
	unsigned long FinishPoint = 0;
public:
	bool Init(std::string &NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore, unsigned int mode, BandPassParams& BPparams); //Инициализируем класс
protected:
	bool GetAmplitudes(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Получаем положительные амлпитуды относительно среднего значения
	bool GetSamplesMed(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Получаем усреднённые значения замплов
	bool GetSamplesAmplit(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Получаем усреднённые значения замплов относительно амплитуды
	bool GetMedFreq(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Поулчаем Частоту относительно Среднего значения	
	bool GetSimpleFT(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Элементарное преобразование Фурье
	bool GetSimpleBandFilter(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore); //Пропуск через полосовой фильтр
	bool GetNextBandFilter(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore, BandPassParams &BandPassParams) ; //Пропуск через полосовой фильтр
	bool GetBandDispersion(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore, BandPassParams& BandPassParams);//Пропуск через полосовой фильтр с последующим определением дисперсии
public:
	bool TurnToSilence(); //Обращаем данный файлик в беззвучную запись, если нет голосовых меток
	void Reset() {
		for (int i = 0; i < 2; i++) {
			m_Channel[i].Reset();
		}
	}
	~WavFile() {
	}
};
class VoiceRecording {
public:
	struct FilterParams {
		BandPassParams BandPass;
		unsigned int nBorder_8bit=3; //Граница для 8битных файлов, относительно которой будем определять надо ли удалять
		unsigned int nBorder_16bit=5; //Граница для 16битных файлов, относительно которой будем определять надо ли удалять
		bool ReadFromReg(); //Читаем эти параметры из реестра
		bool WriteToReg(); //Пишем эти параметры в реестр
	};

	VoiceRecording() {
		m_FilterParams.ReadFromReg();
		Init(0, m_FilterParams.BandPass);
	}
public:
	std::vector<WavFile> m_wavFiles;  //Основной вектор, который содержит набор файлов, включая непосредственно замплы
public:
	WAVEFORMATEX  m_wfx;// информация о наших файлах
	FilterParams m_FilterParams; //Параметры фильтрации, берём из реестра
public:
	bool AddWav(std::string &PathToWav); //Добавляем замплы файла по указанному пути
	void reset(); //Очищаем буффер, сбрасываем значения
	void Analyze(); //Перестраиваем наши буферы с замплами 
	unsigned int GetAmplit(unsigned int Number, unsigned int NumbOfChannel); //Получаем амплитуду по текущему номеру
	void Init(int mode, BandPassParams &BPparams); //Предварительная инициализация
public:
	double m_MaxAmpl[2] = { 0,0 }; //Максимальная амплитуда
	double m_MinAmpl[2] = { 100,100 }; //Минимальная амплитуда (В процентах)
	double m_MedVal = 0.; //Среднее значение, относительно которого будем смотреть амплитуду
	bool m_bIsInitialized = false; //Была ли инициализация?
	unsigned long m_CurSampleCount = 0; //Текущее количество замплов
	int m_Freq_Div = 10; //Частота делений
	int m_msecDiv = 1000/ m_Freq_Div; //Миллисекунд между делениями


	unsigned int m_msecIgnore = 200; //Мы не будем пытаться найти максимум в первых  миллисекундах, т.к. запись только включается
	int m_ModeOfData = 0; //Режим
	~VoiceRecording() {
		reset();
		m_FilterParams.WriteToReg();
	}

};

class VoiceDetector
{
public:
	VoiceDetector() {
		Init("C:\\Backup");
	}
	~VoiceDetector() {
		clearAudio();
	}
public:
	VoiceRecording m_rec; //Класс, который будет работать с набором звуковых записей
protected:
	CString m_BackupPath = "C:\\Backup";
	COleDateTime m_cdtTheLastDir = 0.; //Дата последней обработанной директории Бэкапа (BackUp)
	std::string m_strCurDirectory[2]; //Текущая поддиректория Бэкапа (BackUp)
	COleDateTime m_cdtTheLastFile = 0.; //Дата последней доавбленной аудиозаписи
	std::string m_strCurWavFile[2]; //Последний добавленный файлик

protected:
	/*
	
ResFiles = Файлы, которые мы получили из попки
RequiredCount = Количество файлов, которое мы хотим получить

Возвращает количество файлов, которое получилось получить

*/
	int GetNextFiles(int RequiredCount, std::vector<std::string> &ResFiles, unsigned int NumberOfSoundCard); //Запрашиваем следующие RequiredCount файлов из Бэкапа
	/*
	* FolderName = Название директории, из которой будем получать файлы
	ResFiles = Файлы, которые мы получили из попки
	RequiredCount = Количество файлов, которое мы хотим получить
	
	Возвращает количество файлов, которое получилось получить

	*/
	int GetFilesFromDateDir(std::string &FolderName, int RequiredCount, std::vector<std::string>& ResFiles, unsigned int nSoundCard); //Запрашиваем RequiredCount файлов из конкретной папки с датой
	/*
	folder - Ссылка на название директории, из которой мы хотим получить папки

	Возвращает вектор std::string заполненный навзванием папок
	*/
	std::vector<std::string> get_directories(CString &folder);//Получаем все вложенные папки
	/*
	* Удаляем из вектора те папки, которые мы уже просмотрели, либо те, которые не удовлетворяют элементарным условием
	* 
	*/
	void Filter_Directories(std::vector<std::string> &Directories, unsigned int nSoundCard); //Удаляем из них те, которые уже проверяли  и те, которые не нужны
	/*
	* Проверка - существует ли каталог
	* dirName_in - Имя директории
	* Возвращает True, если существует, False, если не существует.
	* 
	*/
	bool IsDirExists(const std::string& dirName_in);
	/*
	Читаем реестр- получаем оттуда данные о последней прочитанной папке или последнем прочитанном файле
	Возвращает True, если данные получили, False, Если не получили
	*/
	bool GetPathesFromReg();
	/*
	* Переводим имена последних путей в дату и время
	* 	Возвращает True, Если получилось преобразовать, False, Если не получилось
	*/
	bool PathesToCDT(); //
	/*
	Записываем дату и время в реест

	*/
	bool DateTimeToReg();
	/*
	Передаём пути к файлам, (myfiles),
	наполняем каналы заплами, информацией, чтобы начать фильтровать
	
	*/
	bool FillFromFiles(std::vector<std::string>& myfiles);
	bool CorrectNextFile(int NumberOfSoundCard, unsigned int FilesPerCorrection);
	bool RememberPath(unsigned int NumbOfSoundCard, std::string &LasFile);
public:
	bool ReadNextFiles(unsigned int count);
	/*
* Функция, которая занимается удалением файлов, на основе анализа
*/
	bool DeleteNoise();
	bool Init(CString BackupPath);
	bool StartBackupCorrection();
	void clearAudio() {
		//for (int i = 0; i < 2; i++) {
		//	m_Channel[i].Reset();
		//}
	}
};

