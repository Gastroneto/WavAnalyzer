#include "pch.h"
#include "VoiceDetector.h"
#include <fstream>
#include <thread>






bool VoiceDetector::IsDirExists(const std::string& dirName_in)
{
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;  //something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;   // this is a directory!

    return false;    // this is not a directory!

}

bool VoiceDetector::GetPathesFromReg()
{
    DWORD DwLastDir = 1468; //Дата, время последней временной директории
    DWORD DWLastFile = 1468;  //Дата, время последнего файла
    //Читаем эти параметры из реестра



    char buff[64];//DateTimeTo
    DWORD sizeofbuff = 64;
    HKEY hKey = AfxGetApp()->GetAppRegistryKey();
    // CString KeyPath = "SOFTWARE\\NavDCo\\DkIceSynt";
    if (RegGetValue(hKey, NULL, _T("DT_LastFile"), RRF_RT_REG_SZ, NULL, (BYTE*)buff, &sizeofbuff) != ERROR_SUCCESS) {
        m_cdtTheLastDir = 1468.;
        m_cdtTheLastFile = 1468.;
    }
    else
        m_cdtTheLastFile.ParseDateTime(buff);

    if (RegGetValue(hKey, NULL, _T("DT_LastDir"), RRF_RT_REG_SZ, NULL, (BYTE*)buff, &sizeofbuff) != ERROR_SUCCESS) {
        m_cdtTheLastDir = 1468.;

    }
    else
        m_cdtTheLastDir.ParseDateTime(buff);
#ifdef NO_DELETE
    m_cdtTheLastDir = 1468.;
    m_cdtTheLastFile = 1468.;
#endif // NO_DELETE



    //m_cdtTheLastDir = double(DwLastDir)*double(MinCDT); //Дата последней обработанной директории Бэкапа (BackUp)
    //m_cdtTheLastFile = double(DWLastFile) * double(MinCDT);; //Дата последней добавленной  аудиозаписи
    //1-20220310-164202.001
    for (unsigned int DirCounter = 0; DirCounter < 2; DirCounter++) {
        m_strCurDirectory[DirCounter] = m_cdtTheLastDir.Format("1-%Y%m%d-%H%M%S.001"); //Текущая поддиректория Бэкапа (BackUp)
        //20220310.164203863.20220310.164213979.sbu
        m_strCurWavFile[DirCounter] = m_cdtTheLastFile.Format("%Y%m%d.%H%M%S.%Y%m%d.%H%M%S.sbu"); //Последний добавленный файлик
    }
    
     return true;
}

bool VoiceDetector::PathesToCDT()
{
    bool IsCorrect = true;
    //std::string BuffStrDT; //Buffer for date time format
    char * ResStrDT = new char[30]; //string, which will be transformed to dt


    ////1-20220310-164202.001
    //BuffStrDT = m_strCurDirectory; //m_strCurDirectory; //Текущая поддиректория Бэкапа (BackUp)
    int CurPos = m_strCurDirectory[0].find('.')-2*3-1-2*2-4; //-2symb*(sec,min,hour) - 1symb('-')- 2symb*(month,day)-4symb(year)
    if (CurPos < 0)
        return false;
    int ResPos = 0;
    CurPos += 6;
    for(int i=0;i<2;i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Day
    CurPos -= 4;
    ResStrDT[ResPos++] = '/';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Month
    ResStrDT[ResPos++] = '/';
    CurPos -= 6;
    for (int i = 0; i < 4; i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Year
    ResStrDT[ResPos++] = ' ';

    CurPos += (4+1);

    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Hour
    ResStrDT[ResPos++] = ':';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Minute
    ResStrDT[ResPos++] = ':';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = m_strCurDirectory[0][CurPos++]; //Second
    ResStrDT[ResPos++] = 0; //End

    if (!m_cdtTheLastDir.ParseDateTime(ResStrDT)) ////Дата последней обработанной директории Бэкапа (BackUp)
        IsCorrect = false;

    ////20220310.164203863.20220310.164213979.sbu
    unsigned int NameFileStartPos = 0;// m_strCurWavFile[0].size()-41;
    std::string BuffPathWav;
    BuffPathWav = m_strCurWavFile[0].substr(NameFileStartPos);
    ResPos = 0;
    CurPos = 6;
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Day
    CurPos -= 4;
    ResStrDT[ResPos++] = '/';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Month
    ResStrDT[ResPos++] = '/';
    CurPos -= 6;
    for (int i = 0; i < 4; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Year
    ResStrDT[ResPos++] = ' ';

    CurPos += (4 + 1);

    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Hour
    ResStrDT[ResPos++] = ':';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Minute
    ResStrDT[ResPos++] = ':';
    for (int i = 0; i < 2; i++)
        ResStrDT[ResPos++] = BuffPathWav[CurPos++]; //Second
    ResStrDT[ResPos++] = 0; //End
    if (!m_cdtTheLastFile.ParseDateTime(ResStrDT)) ////Дата последней обработанной директории Бэкапа (BackUp)
        IsCorrect = false;
    if (IsCorrect)
        IsCorrect = DateTimeToReg();
    delete[]ResStrDT;
    return IsCorrect;
}

bool VoiceDetector::DateTimeToReg()
{
    unsigned long DTOfDir = double(m_cdtTheLastDir) / double(MinCDT);
    DWORD DwDTDir= (DWORD)DTOfDir; //Преобразуем папку в DWORD

    unsigned long DTOfFile = double(m_cdtTheLastFile) / double(MinCDT);
    DWORD DwDTFile = (DWORD)DTOfFile; //Преобразуем путь к файлу в DWORD


  
    // HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\NavDCo\dKart ODU 6.3\DkIciSynt
    //CString KeyPath = "SOFTWARE\\NavDCo\\DkIceSynt";
    std::string nstrDateTime = m_cdtTheLastFile.Format("%d/%m/%Y %H:%M:%S");
    HKEY hKey;
    hKey = AfxGetApp()->GetAppRegistryKey();

    //if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, KeyPath, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
    //	return 0;
    //}

    // Пишем тестовую строку в созданный ключ
    if (RegSetValueEx(hKey, _T("DT_LastFile"), 0, REG_SZ, (const BYTE*)nstrDateTime.c_str(), nstrDateTime.size() + 1) != ERROR_SUCCESS) {
        return 0;
    }
    nstrDateTime = m_cdtTheLastDir.Format("%d/%m/%Y %H:%M:%S");
    if (RegSetValueEx(hKey, _T("DT_LastDir"), 0, REG_SZ, (const BYTE*)nstrDateTime.c_str(), nstrDateTime.size() + 1) != ERROR_SUCCESS) {
        return 0;
    }

    // Закрываем описатель ключа
    if (RegCloseKey(hKey) != ERROR_SUCCESS) {
        return 0;
    };

    return false;
}

bool VoiceDetector::FillFromFiles(std::vector<std::string>& myfiles)
{
    for (auto it = myfiles.begin(); it != myfiles.end(); it++) {

            bool res = m_rec.AddWav(*it); //Добавляем его в нашу структуру по пути для дальнейшего анализа
            if (!res) {
                return false;
            }
    }
    //Добавить отметочки на каком месте какой зампл начинается
    return true;
}

bool VoiceDetector::CorrectNextFile(int NumberOfSoundCard, unsigned int FilesPerCorrection)
{
    std::vector<std::string> WavFiles;
    m_rec.reset(); //Удаляем все предыдущие файлы, с которыми работали
    int CountOfFiles = GetNextFiles(FilesPerCorrection, WavFiles, NumberOfSoundCard); //Получаем пути к очередным N файлам
    if (!CountOfFiles) //Если не получилось прочесть - возвращаем False
        return false;

    bool res = FillFromFiles(WavFiles); //Получаем все данные файлов, к которым есть пути
    if (!res)
        return false;
    res = DeleteNoise(); //удаляем файлы с шумами (Обращаем в ноль)
    if (!res)
        return false;
    res = RememberPath(NumberOfSoundCard, WavFiles.back()); //Запоминаем последний обработанный путь
    if (!res)
        return false;
    return true;
}

bool VoiceDetector::RememberPath(unsigned int NumbOfSoundCard, std::string& LastFile)
{
    unsigned int sizeOfPath = LastFile.size();
    unsigned int folderDelta = 72;
    unsigned int SizeOfFolder = 21;
    if (sizeOfPath < 75) {
        folderDelta = 55;
        SizeOfFolder = 4;
     //   return false;
    }
    m_strCurWavFile[NumbOfSoundCard] = LastFile.substr(sizeOfPath - 41);
    m_strCurDirectory[NumbOfSoundCard] = LastFile.substr(sizeOfPath - folderDelta, SizeOfFolder);

    return true;
}



bool VoiceDetector::ReadNextFiles(unsigned int Count)
{
    bool IsCurrentDir = false;
    std::vector<std::string> WavFiles;
    int CountOfFiles = GetNextFiles(Count, WavFiles,0); //Получаем пути к очередным N файлам
    if (!CountOfFiles) //Если не получилось прочесть - возвращаем False
        return false;
    bool res = FillFromFiles(WavFiles); //Получаем замплы всех файлов, к которым есть пути
    RememberPath(0, WavFiles.back());
    if(res)
        bool res = PathesToCDT();
    return res;
  
}

bool VoiceDetector::Init(CString BackupPath)
{
    m_BackupPath = BackupPath;
    GetPathesFromReg();
    return true;
}

bool VoiceDetector::StartBackupCorrection()
{

   
    unsigned int CountOfErrors=0; //Счётчик ошибок
    unsigned int FilesPerCorrection = 1;
    while (true) {
        bool Result = false;
        unsigned int nSoundCard = 0;
        while (CorrectNextFile(nSoundCard, FilesPerCorrection)) { //Читаем с каталога первой карточки
            Result = true;
        };
        nSoundCard = 1;
        while (CorrectNextFile(nSoundCard, FilesPerCorrection)) { //Читаем с каталога второй карточки
            Result = true;
        };
        if (Result) {
            PathesToCDT(); //Если была хоть одна успешная попытка - запоминаем данные в реестр
            CountOfErrors = 0;// Обнуляем счётчик ошибок после успешной попытки
        }
        else {
            if ((CountOfErrors++)==30) {
                //Ошибочка, добавить логирование
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); //Ждём одну секунду
        //И опять по новой пробуем найти некорректные логи

    }
    return false;
}

int VoiceDetector::GetNextFiles(int RequiredCount, std::vector<std::string> &ResFiles, unsigned int nSoundCard)
{
    int CountOfFiles = 0;
    auto Date_Directories = get_directories(m_BackupPath); //Получаем папки под бэкапом
    Filter_Directories(Date_Directories, nSoundCard);
    if (!Date_Directories.size())
        return 0;
    for (auto DirIt = Date_Directories.begin(); DirIt != Date_Directories.end(); DirIt++) {
        std::string FullPath = std::string(m_BackupPath) +"\\"+ (*DirIt);
        int nFilesFromDateDir = GetFilesFromDateDir(FullPath, (RequiredCount - CountOfFiles) , ResFiles, nSoundCard);//Прочитываем файлики из указанной библиотеки
        CountOfFiles += nFilesFromDateDir;
        //if (nFilesFromDateDir) { //Если добавили хоть один файл
        //    m_strCurDirectory[0] = *DirIt; //Текущая поддиректория Бэкапа (BackUp)
        //    m_strCurWavFile[0] = (ResFiles.back()); //Последний добавленный файлик
        //}
        if (CountOfFiles == RequiredCount) {
            //remember Последний прочитанную папку и файлик
            break;
        }
    }
    return CountOfFiles;
}

bool VoiceDetector::DeleteNoise()
{
    m_rec.Analyze(); //Перестраиваем
    return true;
}

int VoiceDetector::GetFilesFromDateDir(std::string &FolderName, int RequiredCount, std::vector<std::string>& ResFiles, unsigned int nSoundCard)
{
    {
        int NumberOfSoundCard = 0; //Номер звуковой карты
        std::string strSoundCard = NumberOfSoundCard? "02" : "01"; //номер звуковой карты в текстовом виде
        int AddedFiles = 0; //Количество добавленных файлов
        bool bStartPushBack = false; //Начинаем запоминать файлы, только когда прошли последний записанный
        std::string DirWithFiles = FolderName + "\\Sound\\" + strSoundCard;
        if (!IsDirExists(DirWithFiles)) //Если папки нет, то возвращаем false
            return false;
        std::string search_path = DirWithFiles + "\\*.sbu"; //Маска, по которой искать файлы
        WIN32_FIND_DATA fd;
        HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    std::string FullPath = DirWithFiles + "\\" + fd.cFileName;
                    if (!bStartPushBack) {
                        if (m_strCurWavFile[nSoundCard] < fd.cFileName) { //Как только дошли до файлика, который у нас уже есть- начинаем запись
                            bStartPushBack = true;  
                        }
                    }
                    if (bStartPushBack) { //Если пошли актуальные файлы
                        ResFiles.push_back(FullPath);
                        AddedFiles++;
                    }


                }

            } while (::FindNextFile(hFind, &fd)&&(AddedFiles< RequiredCount));
            ::FindClose(hFind);
        }
        return AddedFiles;
    }
}

std::vector<std::string> VoiceDetector::get_directories(CString &folder)
{

    std::vector<std::string> names;
    std::string search_path = folder + "\\*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if(fd.cFileName[0]!='.')
                    names.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}

void VoiceDetector::Filter_Directories(std::vector<std::string>& Directories, unsigned int nSoundCard)
{
    int SizeOfVec = Directories.size();
    if ((m_cdtTheLastDir < 1)||(!SizeOfVec)) { //Если не было прошлой запоминалки, то не фильтруем папки, не фильтруем, когда папок нет
        return;
    }
    int EndCurDir = m_strCurDirectory[nSoundCard].find('.');; //начальная и конечная точка из реестра в строке, относительно которой будем сравнивать 
    int BegCurDir = EndCurDir - (2*5+4+1); // -15
    //("1-20220303-143917.001")
    while (SizeOfVec > 0) {
        bool IsChanged = false;
        for (auto it = Directories.begin(); it != Directories.end(); it++) {
            int EndDir = (*it).find('.');
            int BegDir = EndDir - 15; ////начальная и конечная точка из в строке, относительно которой будем сравнивать 
            if (BegDir >= 0) {
                int ResultOfCompare = m_strCurDirectory[nSoundCard].compare(BegCurDir, (EndCurDir - BegCurDir), (*it), BegDir, (EndDir));
                if (ResultOfCompare > 0) { //Сравниваем части строки, смотрим с конца
                    SizeOfVec--;
                    Directories.erase(it);
                    IsChanged = true;
                    break;
                }
            }

        }
        if (!IsChanged) //Если вектор не изменили, то выходим из цикла
            break;
    }
    

}



void VoiceRecording::Init(int mode, BandPassParams &BPparams)
{
    m_ModeOfData = mode;
    m_FilterParams.BandPass = BPparams;

}

bool VoiceRecording::AddWav(std::string &mystring)
{
    WavFile NewValue;
    bool res = NewValue.Init(mystring, m_msecDiv, m_msecIgnore, m_ModeOfData, m_FilterParams.BandPass); //Считываем файл
    if (res) {
        auto size = NewValue.m_Channel[0].size; //Количество замплов в одном из каналов
        if (!size)
            return false;
        if (!m_bIsInitialized) {
            m_bIsInitialized = true;
            m_wfx = NewValue.wfx; //Запоминаем формат
            m_MedVal = NewValue.m_Channel[0].Medium; //Запоминаем среднее значение
            for (int ChanCounter = 0; ChanCounter < m_wfx.nChannels; ChanCounter++) {
                m_MaxAmpl[ChanCounter] = NewValue.m_Channel[ChanCounter].Max;
                m_MinAmpl[ChanCounter] = NewValue.m_Channel[ChanCounter].Min;
            }
        }
        for (int i = 0; i < m_wfx.nChannels; i++) {
            double Amplit = NewValue.m_Channel[i].Max;
            if(Amplit> m_MaxAmpl[i])
                m_MaxAmpl[i] = Amplit;
            Amplit = NewValue.m_Channel[i].Min;
            if (Amplit < m_MinAmpl[i])
                m_MinAmpl[i] = Amplit;
            
        }
        NewValue.StartPoint = m_CurSampleCount;
        m_CurSampleCount += size; //Текущее количество замплов
        NewValue.FinishPoint = m_CurSampleCount; //Устонавливаем меточки относительно общей записи

        m_wavFiles.push_back(NewValue); //Запоминаем непосредственно сам вектор
    }
    
    return res;
}

void VoiceRecording::reset()
{

    for (int i = 0; i < 2; i++) {
        m_MaxAmpl[i]=0; //Максимальная амплитуда
        m_MinAmpl[i] = 100; //Минимальная амплитуда (В процентах)
    }
    m_MedVal = 0.; //Среднее значение, относительно которого будем смотреть амплитуду
    m_bIsInitialized = false; //Была ли инициализация?
    m_CurSampleCount = 0; //Текущее количество замплов
    for (auto it = m_wavFiles.begin(); it != m_wavFiles.end(); it++) {
        (*it).Reset();
    }

    m_wavFiles.clear();
}

void VoiceRecording::Analyze()
{
    unsigned int AmplitudeBorder = 0; //Граничное знаечние амплитуды
    unsigned int MsecVoiceDiv = 1000; //в скольких миллисекундах будем искать голос
    unsigned int MsecInVoiceDiv = 100;  //Сколько миллисекунд должно быть, чтобы понять, что есть голос
    //switch (m_ModeOfData)
    //{
    //case 0:
    //{
    //    if ((m_wfx.wBitsPerSample == 8))
    //        AmplitudeBorder = 4;
    //    if ((m_wfx.wBitsPerSample == 16))
    //        AmplitudeBorder = 20;
    //    break;
    //}
    //case 7:
    //{
    //    if ((m_wfx.wBitsPerSample == 8))
    //        AmplitudeBorder = 4;
    //    if ((m_wfx.wBitsPerSample == 16))
    //        AmplitudeBorder = 35;
    //    break;
    //}
    //}
    switch (m_wfx.wBitsPerSample) {
    case 8: {
        AmplitudeBorder = m_FilterParams.nBorder_8bit;
        break;
    }
    case 16: {
        AmplitudeBorder = m_FilterParams.nBorder_16bit;
        break;
    }
    default:
        return;
    }
    unsigned int ReqValues = MsecInVoiceDiv / m_msecDiv; //Необходимо значений подряд
    unsigned int TotalValues = MsecVoiceDiv / m_msecDiv; //В скольких делениях должно быть Req Values

    unsigned int CurValuesInARow = 0; //Текущее количество подряд
    for (auto WavCounter = m_wavFiles.begin(); WavCounter != m_wavFiles.end(); WavCounter++) { //Бежим по всем файлам
        for (int ChannelCounter = 0; ChannelCounter < m_wfx.nChannels; ChannelCounter++) { //Далее по каждому каналу

            double DeltaVal = (m_MaxAmpl[ChannelCounter] - m_MinAmpl[ChannelCounter]) / 100.;
            for (unsigned long SampleCounter = 0; SampleCounter < (*WavCounter).m_Channel[ChannelCounter].size; SampleCounter++) {//Далее по каждому замплу
                double CurAmpl = (*WavCounter).m_Channel[ChannelCounter].Amplitudes[SampleCounter];

                if (CurAmpl > AmplitudeBorder) { //Если значение больше порогового
                    CurValuesInARow++; //Запоминаем это
                    if (CurValuesInARow == ReqValues) {
                        WavCounter->m_bExistVoice = true; //Как только необходимое количество повторени=> Есть голос
                        CurValuesInARow = 0;
                    }
                }
                else
                    CurValuesInARow = 0; //Если не было в ряд- обнуляем значение


                double NewAmplitude = (CurAmpl - m_MinAmpl[ChannelCounter]) / DeltaVal;
                if (NewAmplitude > 100)
                    NewAmplitude = 100; //нормализуем значеник
                ((*WavCounter).m_Channel[ChannelCounter].Amplitudes[SampleCounter]) = NewAmplitude;
            }
        }
        bool IsDeleted = (*WavCounter).TurnToSilence();

    }
    // m_MaxAmpl = 100;

   //  DefineMaxMin(); //Определяем значение, относительно которых будем анализировать график
   // m_MaxAmpl = 100;

  //  DefineMaxMin(); //Определяем значение, относительно которых будем анализировать график
}


unsigned int VoiceRecording::GetAmplit(unsigned int Number , unsigned int NumbOfChannel)
{
    if (Number > m_CurSampleCount) 
        return 0;
    int Limit = ((100));
    for (auto WavCounter = m_wavFiles.begin(); WavCounter != m_wavFiles.end(); WavCounter++) { //Бежим по всем файлам'
        if ((*WavCounter).InRange(Number)) { //Если зампл находится в этом файле
            unsigned int RealNumber = Number - (*WavCounter).StartPoint;
            unsigned int res = (*WavCounter).m_Channel[NumbOfChannel].Amplitudes[RealNumber];
            if (res > Limit)
                return Limit;
            return res;
        }
    }
    return 0;
}


bool WavFile::GetAmplitudes(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAVHEADER m_wfh;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ;
    }
    switch (BytesPerSample)
    {
    case 2:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                long long sum = 0;
                double ResultValue = 0;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    int amplitude = (buf[ByteCounter] << 8) | buf[ByteCounter + 1];


                    float  NormalizedAmpl= ((amplitude) * 100) / m_Channel[NumbOfChannel].Medium;
                    sum += (NormalizedAmpl * NormalizedAmpl);
                    ResultValue += (NormalizedAmpl * NormalizedAmpl);
                    //sum += abs(static_cast<int>(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium));

                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }

                ResultValue = sqrt(ResultValue / static_cast<double>(nSamplesPerDiv)); //Запоминаем это значение в массив
                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }
    case 1:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                unsigned long long sum = 0;
                double ResultValue = 0;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    unsigned int amplitude = (unsigned char)(buf[ByteCounter]);


                    float  NormalizedAmpl= ((amplitude- m_Channel[NumbOfChannel].Medium) * 100) / m_Channel[NumbOfChannel].Medium;
                    sum += (NormalizedAmpl * NormalizedAmpl);
                    ResultValue += (NormalizedAmpl * NormalizedAmpl);
                }

                ResultValue = sqrt(ResultValue / static_cast<double>(nSamplesPerDiv)); //Запоминаем это значение в массив
                if (BytesPerSample == 2) {
                    ResultValue = 100. - ResultValue;
                }
                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }
    default:
        break;
    }

    if (buf)delete[] buf;
    return true;
}

bool WavFile::GetSamplesMed(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1))-0.5); //33150
    //127.5 ; 510 ; 
    }
    switch (BytesPerSample)
    {
    case 2:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                long long sum = 0;
                double ResultValue = 0;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    int amplitude = (buf[ByteCounter] << 8) | buf[ByteCounter + 1];
                    if (amplitude < -1)
                        amplitude++;
                    float NormalizedAmpl = ((amplitude) * 100) / m_Channel[0].Medium;
                    ResultValue +=(NormalizedAmpl);

                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }

                ResultValue = ResultValue / static_cast<double>(nSamplesPerDiv); //Запоминаем это значение в массив
                if (ResultValue < 0)
                    ResultValue++;
                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }
    case 1:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                long long sum = 0;
                double ResultValue = 0;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    unsigned int amplitude = (unsigned char)(buf[ByteCounter]);

                    float NormalizedAmpl = ((amplitude- m_Channel[0].Medium) * 100) / m_Channel[0].Medium;
                    ResultValue += (NormalizedAmpl);
                }

                ResultValue = ResultValue / static_cast<double>(nSamplesPerDiv); //Запоминаем это значение в массив
                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }
    default:
        break;
    }

    if (buf)delete[] buf;
    return true;
}

bool WavFile::TurnToSilence()
{
    if (m_bExistVoice)
        return false;
    std::string ResPath = FilePath;
#ifdef NO_DELETE
    ResPath[5] = 'd';
#endif // NO_DELETE

	BYTE zero = 0;

	//формирование заголовков WAV-файла

	WAV_DATA_CHUNK data = { {'d','a','t','a'},0 };
    unsigned int CountOfSamples = m_Channel[0].SamplesCount;
	data.subchunk2Size = CountOfSamples * wfx.nBlockAlign;
    unsigned int NumBytes = data.subchunk2Size;
	//запись заголовков
    FILE* fp;
    //std::fstream ResFile(ResPath);

    unsigned int res = fopen_s(&fp , ResPath.c_str(), "wb");
    if (!fp)
        return false;
	 res = fwrite(&m_wfh, WAVHEADER_SIZE, 1, fp);
	res = fwrite(&data, sizeof(data), 1, fp);
    unsigned int MedVal = m_Channel[0].Medium;
   // pData = new BYTE[NumBytes];
    for (int i = 0; i < NumBytes; i++) {
        /*unsigned int amplit = 100;
        unsigned int MaxValue = i % 10000+100;
        unsigned int bps = 4400 + i % MaxValue;
        unsigned int div = bps / (amplit*2);
        unsigned int ConstValue = 127 - amplit;*/
      //  pData[i] = MedVal; // (i % bps)l / div + ConstValue;
    }
    //fwrite(pData, 1, NumBytes, fp);


    fclose(fp);

    return true;
}

bool WavFile::GetMedFreq(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
        unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
        for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
            unsigned long long sum = 0;
            double ResultValue = 0;

            bool bIsigher = false;

            for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                unsigned int amplitude = 0;//Амплитуда зампла
                for (unsigned int NumbofByte = 0; NumbofByte < BytesPerSample; NumbofByte++) { //Пробегаем по каждому байту зампла
                    unsigned int nCurByte = ByteCounter + NumbOfChannel * BytesPerSample + NumbofByte; //Текущий байт который будем обрабатывать
                    unsigned int BufByte = (unsigned char)buf[nCurByte]; //Получили значение текущего байта
                    amplitude += (BufByte << (8 * (BytesPerSample - NumbofByte - 1))); //Добавили к амплитуде зампла
                }
                if ((amplitude < m_Channel[0].Medium) == (bIsigher)) {
                    sum++;
                    bIsigher = !bIsigher;
                }

                //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
            }

            ResultValue = sum/2.; // static_cast<double>(nSamplesPerDiv) - m_Channel[0].Medium); //Запоминаем это значение в массив

            if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Max = ResultValue;
            if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Min = ResultValue;
            m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

        }
    }
    if (buf)delete[] buf;
    return true;

}

bool WavFile::GetSamplesAmplit(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    switch (BytesPerSample)
    {
    case 2:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                long long sum = 0;
                double ResultValue = 0;
                unsigned int maxVal = 0;
                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    int amplitude = (buf[ByteCounter] << 8) | buf[ByteCounter+1];
                   
                   
                    sum += (amplitude);

                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }

                ResultValue = abs(sum / static_cast<double>(nSamplesPerDiv)); //Запоминаем это значение в массив

                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }
    case 1:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                unsigned long long sum = 0;
                double ResultValue = 0;
                unsigned int maxVal = 0;
                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    unsigned int amplitude = (unsigned char)(buf[ByteCounter]);


                    float  NormalizedAmpl = ((amplitude - m_Channel[NumbOfChannel].Medium) * 100) / m_Channel[NumbOfChannel].Medium;
                    ResultValue += (static_cast<double>(NormalizedAmpl));
                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }

                ResultValue = abs(ResultValue / static_cast<double>(nSamplesPerDiv)); //Запоминаем это значение в массив

                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

            }
        }
        break;
    }


    default:
        break;
    }
    if (buf)delete[] buf;
    return true;

}
#include <complex>
#include <valarray>
#include "../../../../../../../../Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.28.29910/include/xkeycheck.h"
typedef std::complex<double> Complex;
typedef std::valarray<Complex> ComArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void FirstFFT(ComArray& x)
{
    const double PI = 3.141592653589793238460;
    const unsigned int N = x.size();
    if (N <= 1) return;

    // divide
    ComArray even = x[std::slice(0, N / 2, 2)];
    ComArray  odd = x[std::slice(1, N / 2, 2)];

    // conquer
    FirstFFT(even);
    FirstFFT(odd);

    // combine
    for (unsigned int k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}
void SecondFFT(ComArray& x)
{
    // DFT
    unsigned int N = x.size(), k = N, n;
    double thetaT = 3.14159265358979323846264338328L / N;
    Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
    while (k > 1)
    {
        n = k;
        k >>= 1;
        phiT = phiT * phiT;
        T = 1.0L;
        for (unsigned int l = 0; l < k; l++)
        {
            for (unsigned int a = l; a < N; a += n)
            {
                unsigned int b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }
    // Decimate
    unsigned int m = (unsigned int)log2(N);
    for (unsigned int a = 0; a < N; a++)
    {
        unsigned int b = a;
        // Reverse bits
        b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
        b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
        b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
        b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if (b > a)
        {
            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }
    //// Normalize (This section make it not working correctly)
    //Complex f = 1.0 / sqrt(N);
    //for (unsigned int i = 0; i < N; i++)
    //	x[i] *= f;
}
bool WavFile::GetSimpleFT(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    Complex* AllSamples = new Complex[sampsperchannel];
    ComArray SampleData(AllSamples, sampsperchannel);
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
        unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
        for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
            unsigned long long sum = 0;
            double ResultValue = 0;

            for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                unsigned int amplitude = 0;//Амплитуда зампла
                for (unsigned int NumbofByte = 0; NumbofByte < BytesPerSample; NumbofByte++) { //Пробегаем по каждому байту зампла
                    unsigned int nCurByte = ByteCounter + NumbOfChannel * BytesPerSample + NumbofByte; //Текущий байт который будем обрабатывать
                    unsigned int BufByte = (unsigned char)buf[nCurByte]; //Получили значение текущего байта
                    amplitude += (BufByte << (8 * (BytesPerSample - NumbofByte - 1))); //Добавили к амплитуде зампла
                }
                ResultValue  = (static_cast<int>(static_cast<double>(amplitude)));
                SampleData[SampleCounter] = ResultValue;
            }
        }
    }
    FirstFFT(SampleData);
    for (int ndivcounter = 0; ndivcounter < (DivCount); ndivcounter++) { //пробегаем по всем делениям
        unsigned int firstdivbyte = ndivcounter * DSamplesPerDiv; //порядковый номер первого зампла в делении
        for (int numbofchannel = 0; numbofchannel < wfx.nChannels; numbofchannel++) { //выполняем для каждого канала
            unsigned long long sum = 0;
            double resultvalue = 0;
            double MaxValue=0;
            for (unsigned int samplecounter = firstdivbyte; samplecounter < (DSamplesPerDiv + firstdivbyte); samplecounter++) { //пробегаем по всем самплам в делении
                resultvalue += (SampleData[samplecounter]).real()/ DSamplesPerDiv;
                if ((SampleData[samplecounter]).real() > MaxValue) {
                    MaxValue = SampleData[samplecounter].real();
                }

            }
            resultvalue = resultvalue /2162;//abs(resultvalue);
            resultvalue = (resultvalue + 100)/2;
            if (resultvalue > 100) {
                resultvalue = 100;
            }
            if (resultvalue < 0) {
                resultvalue = 0;
            }
            m_Channel[numbofchannel].Amplitudes[ndivcounter] =resultvalue;
            if ((resultvalue > m_Channel[numbofchannel].Max))
                m_Channel[numbofchannel].Max = resultvalue;
            if ((resultvalue < m_Channel[numbofchannel].Min))
                m_Channel[numbofchannel].Min = resultvalue;

        }

    }
    if (buf)
        delete[] buf;
    return true;

}

bool WavFile::Init(std::string& NewFilePath, unsigned int MsecDiv, unsigned int MsecIgnore, unsigned int mode, BandPassParams& BPparams)
{
    bool res = false;
    switch (mode)
    {
    case 0: {
        res = GetAmplitudes(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 1: {
        res = GetSamplesMed(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 2: {
        res = GetSamplesAmplit(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 3: {
        res = GetMedFreq(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 4: {
        res = GetSimpleFT(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 5: {
        res = GetSimpleBandFilter(NewFilePath, MsecDiv, MsecIgnore);
        break;
    }
    case 6: {
        res = GetNextBandFilter(NewFilePath, MsecDiv, MsecIgnore, BPparams);
        break;
    }
    case 7: {
        res = GetBandDispersion(NewFilePath, MsecDiv, MsecIgnore, BPparams);
        break;
    }
    default:
    {
            break;
    }
    }
    return res;
}

bool WavFile::GetSimpleBandFilter(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    ELEMYO MyBP;
    for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
        unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
        for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
            unsigned long long sum = 0;
            double ResultValue = 0;

            bool bIsigher = false;

            for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                unsigned int amplitude = 0;//Амплитуда зампла
                for (unsigned int NumbofByte = 0; NumbofByte < BytesPerSample; NumbofByte++) { //Пробегаем по каждому байту зампла
                    unsigned int nCurByte = ByteCounter + NumbOfChannel * BytesPerSample + NumbofByte; //Текущий байт который будем обрабатывать
                    unsigned int BufByte = (unsigned char)buf[nCurByte]; //Получили значение текущего байта
                    amplitude += (BufByte << (8 * (BytesPerSample - NumbofByte - 1))); //Добавили к амплитуде зампла
                }
                int CurFreq = MyBP.BandPass(amplitude, 1000, 5480, 2, wfx.nSamplesPerSec);
                ResultValue += static_cast<double>((CurFreq)) / static_cast<double>(nSamplesPerDiv);

                //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
            }

            //ResultValue = sum / 2.; // static_cast<double>(nSamplesPerDiv) - m_Channel[0].Medium); //Запоминаем это значение в массив

            if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Max = ResultValue;
            if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Min = ResultValue;
            m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;

        }
    }
    if (buf)delete[] buf;
    return true;

}



void BANDPASS::INITIAL() {

    //---save these values: x(n-1) ... x(n-8), y(n-1) ... y(n-8)------------------------
    for (int i = 0; i < 8; i++)
    {
        Y[i] = 0;
        X[i] = 0;
    }

    Y[8] = 0;
    //----------------------------------------------------------------------------------

    T = 0;			// initialization of time
    fl = 0;			// initialization of lower cutoff friquency
    fh = 0;			// initialization of upper cutoff friquency
}

int ELEMYO::BandPass(int sensorValue, float fl, float fh, int type,float fs)
{

    //----- search for BandPass filter with "f" parameter, that has been created before---------------
    int i = 0;
    while ((abs(BP[i].fl - fl) + abs(BP[i].fh - fh)) > 0.001 && (i < LengthBP))
        i++;
    //------------------------------------------------------------------------------------------------

    //--if filter with notch frequency "f" have not call before, create BANPASS element for this "f"----
    if (i == LengthBP)
    {
        LengthBP += 1;
        BP = (BANDPASS*)realloc(BP, LengthBP * sizeof(BANDPASS));
        BP[i].INITIAL();
        BP[i].fl = fl;
        BP[i].fh = fh;
    }
    //--------------------------------------------------------------------------------------------------

    //----time start-----------------------------------------------------------------
    //-------------------------------------------------------------------------------

    float a[2], b[2], Q;

    //---Calculation of filter coefficients------------------------------------------
    float A[8], A1[4], A2[4], b0;
    float wah = 2.0 * fs * tan(3.1416 * fh / fs);
    float wal = 2.0 * fs * tan(3.1416 * fl / fs);
    float w = (wah - wal);
    float w0_2 = wah * wal;
    float w0_4 = w0_2 * w0_2 / fs / fs;
    float fsfs = 16.0 * fs * fs;
    float a0;

    switch (type) {
    case 0: //Butterworth 4th order
        Q = 1;
        a[0] = 0.7654;
        a[1] = 1.8478;
        b[0] = 1;
        b[1] = 1;
    case 1: // Chebishev, delta 0.5 db
        Q = 0.3579;
        a[0] = 0.3507;
        a[1] = 0.8467;
        b[0] = 1.0635;
        b[1] = 0.3564;
    case 2: //Chebishev, delta 1db
        Q = 0.2456;
        a[0] = 0.2791;
        a[1] = 0.6737;
        b[0] = 0.9865;
        b[1] = 0.2794;
    }


    a0 = fsfs + 8.0 * fs * a[0] * w + 4.0 * (2.0 * w0_2 + b[0] * w * w) + 2.0 * a[0] * w0_2 * w / fs + w0_4;
    A1[0] = (-4.0 * fsfs - 16.0 * fs * a[0] * w + 4.0 * a[0] * w0_2 * w / fs + 4.0 * w0_4) / a0;
    A1[1] = (6.0 * fsfs - 8.0 * (2.0 * w0_2 + b[0] * w * w) + 6.0 * w0_4) / a0;
    A1[2] = (-4.0 * fsfs + 16.0 * fs * a[0] * w - 4.0 * a[0] * w0_2 * w / fs + 4.0 * w0_4) / a0;
    A1[3] = (fsfs - 8.0 * fs * a[0] * w + 4.0 * (2.0 * w0_2 + b[0] * w * w) - 2.0 * a[0] * w0_2 * w / fs + w0_4) / a0;

    b0 = w * w * w;
    b0 = b0 / a0;

    a0 = fsfs + 8.0 * fs * a[1] * w + 4.0 * (2.0 * w0_2 + b[1] * w * w) + 2.0 * a[1] * w0_2 * w / fs + w0_4;
    A2[0] = (-4.0 * fsfs - 16.0 * fs * a[1] * w + 4.0 * a[1] * w0_2 * w / fs + 4.0 * w0_4) / a0;
    A2[1] = (6.0 * fsfs - 8.0 * (2.0 * w0_2 + b[1] * w * w) + 6.0 * w0_4) / a0;
    A2[2] = (-4.0 * fsfs + 16.0 * fs * a[1] * w - 4.0 * a[1] * w0_2 * w / fs + 4.0 * w0_4) / a0;
    A2[3] = (fsfs - 8.0 * fs * a[1] * w + 4.0 * (2.0 * w0_2 + b[1] * w * w) - 2.0 * a[1] * w0_2 * w / fs + w0_4) / a0;

    b0 = 16.0 * Q * w * b0 / a0;

    A[0] = A1[0] + A2[0];
    A[1] = A2[1] + A1[0] * A2[0] + A1[1];
    A[2] = A2[2] + A1[0] * A2[1] + A1[1] * A2[0] + A1[2];
    A[3] = A2[3] + A1[0] * A2[2] + A1[1] * A2[1] + A1[2] * A2[0] + A1[3];
    A[4] = A1[0] * A2[3] + A1[1] * A2[2] + A1[2] * A2[1] + A1[3] * A2[0];
    A[5] = A1[1] * A2[3] + A1[2] * A2[2] + A1[3] * A2[1];
    A[6] = A1[2] * A2[3] + A1[3] * A2[2];
    A[7] = A1[3] * A2[3];

    //----------------------------------------------------------------------------------

    //The new signal value after filtering
    BP[i].Y[8] = b0 * ((float)sensorValue) - 4.0 * b0 * BP[i].X[6] + 6.0 * b0 * BP[i].X[4] - 4.0 * b0 * BP[i].X[2] + b0 * BP[i].X[0] - A[0] * BP[i].Y[7] - A[1] * BP[i].Y[6] - A[2] * BP[i].Y[5] - A[3] * BP[i].Y[4] - A[4] * BP[i].Y[3] - A[5] * BP[i].Y[2] - A[6] * BP[i].Y[1] - A[7] * BP[i].Y[0];

    //---save these values: x(n-1) ... x(n-8), y(n-1) ... y(n-8)------------------------
    for (int j = 0; j < 8; j++)
        BP[i].Y[j] = BP[i].Y[j + 1];

    for (int j = 0; j < 7; j++)
        BP[i].X[j] = BP[i].X[j + 1];

    BP[i].X[7] = sensorValue;
    //-------------------------------------------------------------------------------------

    if (fs > 2.1 * fh)
        return (int)BP[i].Y[8];

    return 0;
}


#if DOUBLE_PRECISION
#define COS cos
#define SIN sin
#define TAN tan
#define COSH cosh
#define SINH sinh
#define SQRT sqrt
#define LOG log
#else
#define COS cosf
#define SIN sinf
#define TAN tanf
#define COSH coshf
#define SINH sinhf
#define SQRT sqrtf
#define LOG logf
#endif



bool WavFile::GetNextBandFilter(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore, BandPassParams &BPparams)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    BPparams.nDiscrFreq = wfx.nSamplesPerSec;
    CHEBandPass BPfilter[2] = { CHEBandPass(BPparams),CHEBandPass(BPparams) };
    for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
        unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
        for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
            unsigned long long sum = 0;
            double ResultValue = 0;

            bool bIsigher = false;

            for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                int amplitude = 0;//Амплитуда зампла
                for (unsigned int NumbofByte = 0; NumbofByte < BytesPerSample; NumbofByte++) { //Пробегаем по каждому байту зампла
                    unsigned int nCurByte = ByteCounter + NumbOfChannel * BytesPerSample + NumbofByte; //Текущий байт который будем обрабатывать
                    unsigned int BufByte = (unsigned char)buf[nCurByte]; //Получили значение текущего байта
                    //if (nCurByte > 0) {
                    //    BufByte = (unsigned char)buf[nCurByte-1]; //Получили значение текущего байта
                    //}
                   
                    amplitude += (BufByte << (8 * (BytesPerSample - NumbofByte - 1))); //Добавили к амплитуде зампла
                }
                amplitude = ((amplitude - m_Channel[NumbOfChannel].Medium) * 100) / m_Channel[NumbOfChannel].Medium;
                int CurFreq = BPfilter[NumbOfChannel].PassValue(amplitude);
              /*  if (CurFreq < 0)
                    CurFreq = abs(CurFreq);*/
                ResultValue += static_cast<double>((CurFreq)) / static_cast<double>(nSamplesPerDiv);

                //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
            }

            //ResultValue = sum / 2.; // static_cast<double>(nSamplesPerDiv) - m_Channel[0].Medium); //Запоминаем это значение в массив

            if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Max = ResultValue;
            if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                m_Channel[NumbOfChannel].Min = ResultValue;
            m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;
        }
    }
    if (buf)delete[] buf;
    return true;

}

CHEBandPass::CHEBandPass(BandPassParams& InitParams)
{
    unsigned int n = InitParams.nOrder;
    float epsilon = InitParams.epsilon;
    float s = InitParams.nDiscrFreq;
    float f_lower = InitParams.nLowBorder;
    float f_upper = InitParams.nHighBorder;

    this->m = n / 4;
    this->A = (float*)malloc(this->m * sizeof(float));
    this->d1 = (float*)malloc(this->m * sizeof(float));
    this->d2 = (float*)malloc(this->m * sizeof(float));
    this->d3 = (float*)malloc(this->m * sizeof(float));
    this->d4 = (float*)malloc(this->m * sizeof(float));
    this->w0 = (float*)calloc(this->m, sizeof(float));
    this->w1 = (float*)calloc(this->m, sizeof(float));
    this->w2 = (float*)calloc(this->m, sizeof(float));
    this->w3 = (float*)calloc(this->m, sizeof(float));
    this->w4 = (float*)calloc(this->m, sizeof(float));

    float a = COS(M_PI * (f_lower + f_upper) / s) / COS(M_PI * (f_upper - f_lower) / s);
    float a2 = a * a;
    float b = TAN(M_PI * (f_upper - f_lower) / s);
    float b2 = b * b;
    float u = LOG((1.0 + SQRT(1.0 + epsilon * epsilon)) / epsilon);
    float su = SINH(2.0 * u / (float)n);
    float cu = COSH(2.0 * u / (float)n);
    float r, c;

    int i;
    for (i = 0; i < this->m; ++i) {
        r = SIN(M_PI * (2.0 * i + 1.0) / n) * su;
        c = COS(M_PI * (2.0 * i + 1.0) / n) * cu;
        c = r * r + c * c;
        s = b2 * c + 2.0 * b * r + 1.0;
        this->A[i] = b2 / (4.0 * s); // 4.0
        this->d1[i] = 4.0 * a * (1.0 + b * r) / s;
        this->d2[i] = 2.0 * (b2 * c - 2.0 * a2 - 1.0) / s;
        this->d3[i] = 4.0 * a * (1.0 - b * r) / s;
        this->d4[i] = -(b2 * c - 2.0 * b * r + 1.0) / s;
    }
    this->ep = 2.0 / epsilon;  // used to normalize
}

void CHEBandPass::free()
{
    ::free(this->A);
    ::free(this->d1);
    ::free(this->d2);
    ::free(this->d3);
    ::free(this->d4);
    ::free(this->w0);
    ::free(this->w1);
    ::free(this->w2);
    ::free(this->w3);
    ::free(this->w4);
}

float CHEBandPass::PassValue(float x)
{
    int i;
    for (i = 0; i < this->m; ++i) {
        this->w0[i] = this->d1[i] * this->w1[i] + this->d2[i] * this->w2[i] + this->d3[i] * this->w3[i] + this->d4[i] * this->w4[i] + x;
        x = this->A[i] * (this->w0[i] - 2.0 * this->w2[i] + this->w4[i]);
        this->w4[i] = this->w3[i];
        this->w3[i] = this->w2[i];
        this->w2[i] = this->w1[i];
        this->w1[i] = this->w0[i];
    }
    return x *this->ep;
}

bool WavFile::GetBandDispersion(std::string& NewFilePath, unsigned int MsecBetweenDiv, unsigned int MsecIgnore, BandPassParams& BPparams)
{
    FilePath = NewFilePath;
    unsigned int divIgnore = MsecIgnore / MsecBetweenDiv;
    if (!divIgnore)
        divIgnore = 1;
    WAV_DATA_CHUNK data;
    //read wav file
    FILE* fr;
    fopen_s(&fr, FilePath.c_str(), "rb");
    if (!fr)//двоичный файл для чтения
    {
        return false;
    }

    {
        fread_s(&m_wfh, WAVHEADER_SIZE, WAVHEADER_SIZE, 1, fr);
        fseek(fr, WAVHEADER_SIZE, SEEK_SET);
        fread_s(&data, sizeof(WAV_DATA_CHUNK), sizeof(WAV_DATA_CHUNK), 1, fr);
        //set wave format info

        wfx.wFormatTag = m_wfh.audioFormat;
        wfx.cbSize = 0;
        wfx.nAvgBytesPerSec = m_wfh.byteRate;
        wfx.nBlockAlign = m_wfh.blockAlign;
        wfx.nChannels = m_wfh.numChannels;
        wfx.nSamplesPerSec = m_wfh.sampleRate;
        wfx.wBitsPerSample = m_wfh.bitsPerSample;
    }
    fseek(fr, (WAVHEADER_SIZE + sizeof(data)), SEEK_SET);//указатель положения в файле (ЧИТАТЬ данные)
    //write audio stream
    int numbytes = data.subchunk2Size;
    char* buf = new char[numbytes]; // Делаем массив на куче <===============

    unsigned long check(0);
    check = fread(buf, 1, numbytes, fr);// Гоним в массив весь звуковой файл <========     

    if (fr)fclose(fr);
    int BytesPerSample = wfx.wBitsPerSample / 8;

    unsigned long numsamps = numbytes / BytesPerSample;
    unsigned long sampsperchannel = numsamps / wfx.nChannels;
    //Получили байты и число самплов 
    //https://www.codeproject.com/Articles/29676/CWave-A-Simple-C-Class-to-Manipulate-WAV-Files

    double DSamplesPerDiv = double(wfx.nSamplesPerSec * MsecBetweenDiv) / 1000.; //То, сколько замплов будет занимать одно деление
    unsigned int nSamplesPerDiv = DSamplesPerDiv; //Тоже самое, но целочисленное
    unsigned int DivCount = sampsperchannel / DSamplesPerDiv;
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        // m_Channel[NumbOfChannel].samples = std::make_unique<unsigned int[]>(sampsperchannel); //Выделяем место под буфер со звуком
        m_Channel[NumbOfChannel].Amplitudes = new double[DivCount];
        m_Channel[NumbOfChannel].size = DivCount;
        m_Channel[NumbOfChannel].SamplesCount = sampsperchannel;
    }
    for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) {
        m_Channel[NumbOfChannel].Medium = ((1 << (BytesPerSample * 8 - 1)) - 0.5); //33150
    //127.5 ; 510 ; 
    }
    BPparams.nDiscrFreq = wfx.nSamplesPerSec;
    CHEBandPass BPfilter[2] = { CHEBandPass(BPparams),CHEBandPass(BPparams) };
    switch (BytesPerSample)
    {
    case 2:
    {
        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                unsigned long long sum = 0;
                double ResultValue = 0;

                bool bIsigher = false;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    int amplitude = (buf[ByteCounter] << 8) | buf[ByteCounter + 1];

                    float  NormalizedAmpl = ((amplitude) * 100) / m_Channel[NumbOfChannel].Medium;
                    sum += (NormalizedAmpl * NormalizedAmpl);
                    float CurFreq = BPfilter[NumbOfChannel].PassValue(NormalizedAmpl);
                    CurFreq = CurFreq * CurFreq;
                    /*  if (CurFreq < 0)
                          CurFreq = abs(CurFreq);*/
                    ResultValue += static_cast<double>((CurFreq)) / static_cast<double>(nSamplesPerDiv);

                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }
                ResultValue = sqrt(ResultValue);
                //ResultValue = sum / 2.; // static_cast<double>(nSamplesPerDiv) - m_Channel[0].Medium); //Запоминаем это значение в массив

                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;
            }
        }
        break;
    }
    case 1:
    {

        for (int nDivCounter = 0; nDivCounter < (DivCount); nDivCounter++) { //пробегаем по всем делениям
            unsigned int FirstDivByte = nDivCounter * DSamplesPerDiv; //Порядковый номер первого зампла в делении
            for (int NumbOfChannel = 0; NumbOfChannel < wfx.nChannels; NumbOfChannel++) { //Выполняем для каждого канала
                unsigned long long sum = 0;
                double ResultValue = 0;

                bool bIsigher = false;

                for (unsigned int SampleCounter = FirstDivByte; SampleCounter < (nSamplesPerDiv + FirstDivByte); SampleCounter++) { //Пробегаем по всем самплам в делении
                    unsigned int ByteCounter = SampleCounter * wfx.nChannels * BytesPerSample;//Порядковый номер первого байта в делении
                    //Амплитуда зампла
                    unsigned int amplitude = (unsigned char)(buf[ByteCounter]);

                    float  NormalizedAmpl = ((amplitude- m_Channel[NumbOfChannel].Medium) * 100) / m_Channel[NumbOfChannel].Medium;
                    sum += (NormalizedAmpl * NormalizedAmpl);
                    float CurFreq = BPfilter[NumbOfChannel].PassValue(NormalizedAmpl);
                    CurFreq = CurFreq * CurFreq;
                    /*  if (CurFreq < 0)
                          CurFreq = abs(CurFreq);*/
                    ResultValue += static_cast<double>((CurFreq)) / static_cast<double>(nSamplesPerDiv);

                    //  ResultValue += abs(static_cast<double>(amplitude) - m_Channel[NumbOfChannel].Medium)/ static_cast<double>(nSamplesPerDiv);  //Задаём среднее значение
                }
                ResultValue = sqrt(ResultValue);
                //ResultValue = sum / 2.; // static_cast<double>(nSamplesPerDiv) - m_Channel[0].Medium); //Запоминаем это значение в массив

                if ((ResultValue > m_Channel[NumbOfChannel].Max) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Max = ResultValue;
                if ((ResultValue < m_Channel[NumbOfChannel].Min) && (nDivCounter >= divIgnore))
                    m_Channel[NumbOfChannel].Min = ResultValue;
                m_Channel[NumbOfChannel].Amplitudes[nDivCounter] = ResultValue;
            }
        }
        break;
    }
    default:
        break;
    }
    if (buf)delete[] buf;
    return true;

}

bool VoiceRecording::FilterParams::ReadFromReg()
{
    char buff[6];
    DWORD sizeofbuff = 6;
    HKEY hKey = AfxGetApp()->GetAppRegistryKey();
    DWORD sizeofDWORD = sizeof(DWORD);
    //RRF_RT_ANY
    DWORD32 DWLowBandBorder;// = BandPass.nLowBorder;
    
    if (RegGetValue(hKey, NULL, _T("BandPass_nLowBorder"), RRF_RT_REG_DWORD, NULL, &DWLowBandBorder, &sizeofDWORD) != ERROR_SUCCESS) {
        return false;
    }

    DWORD32 DWHighBandBorder;// = BandPass.nHighBorder;
    if (RegGetValue(hKey, NULL, _T("BandPass_uHighBorder"), RRF_RT_REG_DWORD, NULL, &DWHighBandBorder, &sizeofDWORD) != ERROR_SUCCESS) {
        return false;
    }

    DWORD32 DWFilterOrder;// = BandPass.nOrder;
    if (RegGetValue(hKey, NULL, _T("BandPass_Order"), RRF_RT_REG_DWORD, NULL, &DWFilterOrder, &sizeofDWORD) != ERROR_SUCCESS) {
        return false;
    }
    BandPass.SetParams(DWLowBandBorder, DWHighBandBorder, DWFilterOrder);
   

    DWORD32 DWBorder16bit;// = this->nBorder_16bit;
    if (RegGetValue(hKey, NULL, _T("VoiceBorder_16bit"), RRF_RT_REG_DWORD, NULL, &DWBorder16bit, &sizeofDWORD) != ERROR_SUCCESS) {
        return false;
    }
    
    nBorder_16bit = DWBorder16bit;

    DWORD32 DWBorder8bit;// = this->nBorder_8bit;
    if (RegGetValue(hKey, NULL, _T("VoiceBorder_8bit"), RRF_RT_REG_DWORD, NULL, &DWBorder8bit, &sizeofDWORD) != ERROR_SUCCESS) {
        return false;
    }

    nBorder_8bit = DWBorder8bit;

    return false;
    
}

bool VoiceRecording::FilterParams::WriteToReg()
{
    HKEY hKey;
    hKey = AfxGetApp()->GetAppRegistryKey();

    //if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, KeyPath, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
    //	return 0;
    //}

    // Пишем тестовую строку в созданный ключ
    DWORD32 DWLowBandBorder = BandPass.nLowBorder;
    DWORD32 DWHighBandBorder = BandPass.nHighBorder;
    DWORD32 DWFilterOrder = BandPass.nOrder;
   
    DWORD32 DWBorder16bit = this->nBorder_16bit;
    DWORD32 DWBorder8bit = this->nBorder_8bit;
    
    if (RegSetValueEx(hKey, TEXT("BandPass_nLowBorder"), 0, REG_DWORD, (const BYTE*)&DWLowBandBorder, sizeof(DWLowBandBorder)) != ERROR_SUCCESS) {
        return false;
    }
    if (RegSetValueEx(hKey, TEXT("BandPass_uHighBorder"), 0, REG_DWORD, (const BYTE*)&DWHighBandBorder, sizeof(DWHighBandBorder)) != ERROR_SUCCESS) {
        return false;
    }
    if (RegSetValueEx(hKey, TEXT("BandPass_Order"), 0, REG_DWORD, (const BYTE*)&DWFilterOrder, sizeof(DWFilterOrder)) != ERROR_SUCCESS) {
        return false;
    }
    if (RegSetValueEx(hKey, TEXT("VoiceBorder_16bit"), 0, REG_DWORD, (const BYTE*)&DWBorder16bit, sizeof(DWBorder16bit)) != ERROR_SUCCESS) {
        return false;
    }
    if (RegSetValueEx(hKey, TEXT("VoiceBorder_8bit"), 0, REG_DWORD, (const BYTE*)&DWBorder8bit, sizeof(DWBorder8bit)) != ERROR_SUCCESS) {
        return false;
    }





    return true;
}
