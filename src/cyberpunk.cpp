#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

class sequences {       
    public:             
        vector<string> sequence;        
        int reward;  
};

class code {
    public:
        string token;
        bool status;
};

class coordinat {
    public:
        int x;
        int y;
};

class listbuffer {
    public:
        vector<string> seqbuffer;
        int totalReward;
        vector<coordinat> seqcoordinat;
};

void readtxt(const string& namaFile, int &buffer_size, int &matrix_width, int &matrix_height, vector<vector<code>> &matrix, int &number_of_sequences) {
    string path = "../test/" + namaFile;

    ifstream file(path);

    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << path << endl;
        return;
    }

    string line;
    getline(file, line);
    buffer_size = stoi(line);

    string word;
    file >> word;
    matrix_width = stoi(word);
    file >> word;
    matrix_height = stoi(word);
    
    vector<code> baris;
    for(int i = 0; i < matrix_height; i++){
        for(int j = 0; j < matrix_width; j++){
            file >> word;
            code alfanum;
            alfanum.token = word;
            alfanum.status = false;
            baris.push_back(alfanum);
        }
        matrix.push_back(baris);
        baris.clear();
    }

    file >> word;
    number_of_sequences = stoi(word);

    file.close();
}

void readsequences(const string& namaFile, const int &number_of_sequences, vector<sequences> &objekSequences, int &skipline) {
    string path = "../test/" + namaFile;
    ifstream file(path);

    string line;
    for(int i = 0; i < skipline; i++){
        getline(file, line);
    }

    string word, token;
    for (int i = 0; i < number_of_sequences; i++) {
        while(file >> token){
            objekSequences[i].sequence.push_back(token);
            if(file.peek() == '\n'){
                break;
            }
        }
        int reward;
        file >> word;
        reward = stoi(word);
        objekSequences[i].reward = reward;
    }
}

void cetakMatriks(const vector<vector<code>>& matriks) {
    for (const auto& baris : matriks) {
        for (code angka : baris) {
            cout << angka.token << "\t";
        }
        cout << endl;
    }
}

void cetakSequences(const int &number_of_sequences, vector<sequences> objekSequences){
    for (int i = 0; i < number_of_sequences; ++i) {
        cout << "Sequence " << (i+1) << ":" << endl;
        for (const auto& seq : objekSequences[i].sequence) {
            cout << seq << " ";
        }
        cout << endl;
        cout << "Reward: " << objekSequences[i].reward << endl;
    }
}


void cekReward(const vector<string>& currentSequence, const vector<sequences>& objekSequences, vector<listbuffer> &list_of_buffer, vector<coordinat> &seqCoordinat) {
    int reward = 0;
    for (const sequences& objekSequence : objekSequences) {
        const vector<string>& sequence = objekSequence.sequence;
        bool isSubset = false;
        if(sequence.size() <= currentSequence.size()){
            int len1 = sequence.size();
            int len2 = currentSequence.size();
            auto subsetIter = sequence.begin();
            auto currseqIter = currentSequence.begin();
            int index1, index2 = 0;
            while(index2 <= len2){
                while(*subsetIter == *currseqIter){
                    index1++;
                    index2++;
                    if(index1 == len1){
                        isSubset = true;
                        index1 = 0;
                        break;
                    }
                    ++subsetIter;
                    ++currseqIter;
                    if(*subsetIter != *currseqIter){
                        subsetIter = sequence.begin();
                        index1 = 0;
                    }
                }
                if(index1 == len1){
                    isSubset = true;
                    index1 = 0;
                    break;
                }
                ++currseqIter;
                index2++;
            }
            if (isSubset) {
                reward += objekSequence.reward;
                isSubset = false;
            }
        }
    }
    listbuffer buffers;
    buffers.totalReward = reward;
    buffers.seqcoordinat = seqCoordinat;
    for(int i = 0; i < currentSequence.size(); i++){
        buffers.seqbuffer.push_back(currentSequence[i]);
    }
    list_of_buffer.push_back(buffers);
    
    reward = 0;
}

void moveVertical(vector<vector<code>> &matrix, vector<string> &currentSequence, int row, int col, vector<sequences> &objekSequences, int buffer_size, vector<listbuffer> &list_of_buffer, vector<coordinat> &seqCoordinat);
void moveHorizontal(vector<vector<code>> &matrix, vector<string> &currentSequence, int row, int col, vector<sequences> &objekSequences, int buffer_size,vector<listbuffer> &list_of_buffer, vector<coordinat> &seqCoordinat);

void moveHorizontal(vector<vector<code>> &matrix, vector<string> &currentSequence, int row, int col, vector<sequences> &objekSequences, int buffer_size, vector<listbuffer> &list_of_buffer, vector<coordinat> &seqCoordinat) {
    int matrix_height = matrix.size();
    int matrix_width = matrix[0].size();

    for (int j = 0; j < matrix_width; ++j) {
        // Check if the token has been taken
        if (!matrix[row][j].status) {
            matrix[row][j].status = true; // Mark the token as taken
            currentSequence.push_back(matrix[row][j].token);
            coordinat co;
            co.y = row + 1;
            co.x = j + 1;
            seqCoordinat.push_back(co);
            
            if (currentSequence.size() == buffer_size) {
                cekReward(currentSequence, objekSequences, list_of_buffer, seqCoordinat);
            } else {
                moveVertical(matrix, currentSequence, row, j, objekSequences, buffer_size, list_of_buffer, seqCoordinat);
            }
            
            matrix[row][j].status = false; // Reset the status of the token
            seqCoordinat.pop_back();
            currentSequence.pop_back();
        }
    }
    // cout << currentSequence[currentSequence.size()];
}

void moveVertical(vector<vector<code>> &matrix, vector<string> &currentSequence, int row, int col, vector<sequences> &objekSequences, int buffer_size, vector<listbuffer> &list_of_buffer, vector<coordinat> &seqCoordinat) {
    int matrix_height = matrix.size();
    int matrix_width = matrix[0].size();
    // cout << "check3" << endl;
    for (int i = 0; i < matrix_height; ++i) {
        // Check if the token has been taken
        if (!matrix[i][col].status) {
            matrix[i][col].status = true; // Mark the token as taken
            currentSequence.push_back(matrix[i][col].token);
            coordinat co;
            co.y = i + 1;
            co.x = col + 1;
            seqCoordinat.push_back(co);
            
            if (currentSequence.size() == buffer_size) {
                cekReward(currentSequence, objekSequences, list_of_buffer, seqCoordinat);
            } else {
                moveHorizontal(matrix, currentSequence, i, col, objekSequences, buffer_size, list_of_buffer, seqCoordinat);
            }
            
            matrix[i][col].status = false; // Reset the status of the token
            seqCoordinat.pop_back();
            currentSequence.pop_back();
        }
    }
    // cout << currentSequence[currentSequence.size()];
}

void generateSequences(vector<vector<code>> &matrix, int &col, int &buffer_size, int &matrix_height, int &matrix_width, vector<coordinat> &seqCoordinat, vector<string> &currentSequence, vector<sequences> &objekSequences, vector<listbuffer> &list_of_buffer){
    // cout << "check1" << endl;
    currentSequence.push_back(matrix[0][col].token);
    // cout << matrix[0][col].token << endl;
    matrix[0][col].status = true;
    coordinat first;
    first.y = 1;
    first.x = col + 1;
    seqCoordinat.push_back(first);
    moveVertical(matrix, currentSequence, 0, col, objekSequences, buffer_size, list_of_buffer, seqCoordinat);
    while(currentSequence.size() != 0){
        currentSequence.pop_back();
    }
    while(seqCoordinat.size() != 0){
        seqCoordinat.pop_back();
    }
}

void cetakListbuffer(const vector<listbuffer> &list_of_buffer) {
    cout << "List of Buffers:" << endl;
    for (const listbuffer& buffer : list_of_buffer) {
        if(buffer.seqbuffer[0] == "55"){
            cout << "Sequence Buffer:" << endl;
            for (const string& token : buffer.seqbuffer) {
                cout << token << " ";
            }
            cout << endl;
            cout << "Total Reward: " << buffer.totalReward << endl;
            cout << endl;
            cout << "Coordinat Buffer:" << endl;
            for (const coordinat& token : buffer.seqcoordinat) {
                cout << token.x << "," << token.y << endl;
            }
        }
    }
}

void cetakHasil(vector<listbuffer> &list_of_buffer, int &index){
    cout << "----------------------------" << endl;
    cout << "Hasil terbaik: " << endl;
    listbuffer& hasil = list_of_buffer[index];
    cout << hasil.totalReward << endl;
    for (const string& token : hasil.seqbuffer) {
        cout << token << " ";
    }
    cout << endl;
    for (const coordinat& token : hasil.seqcoordinat) {
        cout << token.x << ", " << token.y << endl;
    }
}

void cariTerbesar(vector<listbuffer> &list_of_buffer, int &index){
    int maxReward = list_of_buffer[0].totalReward;
    for (size_t i = 1; i < list_of_buffer.size(); ++i) {
        maxReward = max(maxReward, list_of_buffer[i].totalReward);
    }
    for (size_t i = 0; i < list_of_buffer.size(); ++i) {
        if (list_of_buffer[i].totalReward == maxReward) {
            index = i;
            break;
        }
    }
    // cout << maxReward << endl;
    // cout << index << endl;
    cetakHasil(list_of_buffer, index);
}

void prosestxt(){
    string namaFile;
    cout << "Masukkan nama file: ";
    cin >> namaFile;

    auto start = chrono::steady_clock::now();

    int buffer_size, matrix_width, matrix_height, number_of_sequences;
    vector<vector<code>> matrix;
    readtxt(namaFile, buffer_size, matrix_width, matrix_height, matrix, number_of_sequences);
    vector<sequences> objekSequences(number_of_sequences);
    int skipline = 3 + matrix_height;
    readsequences(namaFile, number_of_sequences, objekSequences, skipline);

    cout << "------------------------------------------" << endl;
    cout << "Buffer size: " << buffer_size << endl;
    cout << "Lebar dan panjang matriks: " << matrix_width << " " << matrix_height << endl;
    cout << "Matriks: " << number_of_sequences << endl;
    cetakMatriks(matrix);
    cout << "Jumlah sekuens: " << number_of_sequences << endl;
    cetakSequences(number_of_sequences, objekSequences);

    vector<string> currentSequence;
    vector<listbuffer> list_of_buffer;
    vector<coordinat> seqCoordinat;

    for(int i = 0; i < matrix_width; i++){
        int col = i;
        generateSequences(matrix, col, buffer_size, matrix_height, matrix_width, seqCoordinat, currentSequence, objekSequences, list_of_buffer);
    }
    // cout << list_of_buffer.size() << endl;
    // cetakListbuffer(list_of_buffer);
    int index;
    cariTerbesar(list_of_buffer, index);
    
    cout << endl;
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << endl;
    cout << duration.count() << " ms" << endl;

    cout << "Apakah Anda ingin menyimpan solusi? (y/n)" << endl;
    string ans;
    cin >> ans;
    if (ans == "y"){
        string folderPath = "../test/";
        string namaFileHasil;

        cout << "Tuliskan nama file: ";
        cin >> namaFileHasil;

        string fullPath = folderPath + namaFileHasil;
        ofstream file(fullPath);

        if (!file.is_open()) {
            cerr << "Gagal membuka file." << endl;
        }

        listbuffer& hasil = list_of_buffer[index];
        file << hasil.totalReward << endl;
        for (const string& token : hasil.seqbuffer) {
            file << token << " ";
        }
        file << endl;
        for (const coordinat& token : hasil.seqcoordinat) {
            file << token.x << ", " << token.y << endl;
        }
        file << endl;
        file << duration.count() << "ms" << endl;
        file.close();
    }
}

void generatematrix(vector<string> &token, int &matrix_width, int &matrix_height, vector<vector<code>> &matrix){
    vector<code> baris;
    for(int i = 0; i < matrix_height; i++){
        for(int j = 0; j < matrix_width; j++){
            random_device rd;
            mt19937 gen(rd()); 
            uniform_int_distribution<int> dis(0, token.size()-1); 
            int randomNumber = dis(gen);
            code alfanum;
            alfanum.token = token[randomNumber];
            alfanum.status = false;
            baris.push_back(alfanum);
        }
        matrix.push_back(baris);
        baris.clear();
    }
}

void generatesekuens(vector<string> &token, int &number_of_sequences, int &ukuran_maks_sekuens, vector<sequences> &objekSequences){
    random_device rd;
    mt19937 gen(rd()); 
    uniform_int_distribution<int> dis(0, token.size()-1); 
    for(int i = 0; i < number_of_sequences; i++){
        int randomNumberforSekuen = rand() % ukuran_maks_sekuens + 1;
        for(int j = 0; j < randomNumberforSekuen; j++){
            int randomNumberforToken = dis(gen);
            objekSequences[i].sequence.push_back(token[randomNumberforToken]);
        }
        int randomNumberforReward = (rand() % 20 + 1) * 5; //batasan reward 1-100 dengan kelipatan 5
        objekSequences[i].reward = randomNumberforReward;
    }
} 

void prosesotomatis() {
    int buffer_size, matrix_width, matrix_height, number_of_sequences;
    vector<vector<code>> matrix;
    int jumlah_token_unik;
    cout << "Masukkan jumlah token unik: " << endl;
    cin >> jumlah_token_unik;
    cout << "Masukkan token: " << endl;

    vector<string> token;
    for(int i = 0; i < jumlah_token_unik; i++){
        string alfanum;
        cin >> alfanum;
        token.push_back(alfanum);
    }

    cout << "Masukkan ukuran buffer: " << endl;
    cin >> buffer_size;
    cout << "Masukkan ukuran matriks: " << endl;
    cin >> matrix_width >> matrix_height;
    cout << "Jumlah sekuens: " << endl;
    cin >> number_of_sequences;
    cout << "Ukuran maksimal sekuens: " << endl;
    int ukuran_maks_sekuens;
    cin >> ukuran_maks_sekuens;

    vector<sequences> objekSequences(number_of_sequences);

    auto start = chrono::steady_clock::now();

    generatematrix(token, matrix_width, matrix_height, matrix);
    generatesekuens(token, number_of_sequences, ukuran_maks_sekuens, objekSequences);

    cout << "------------------------------------------" << endl;
    cout << "Buffer size: " << buffer_size << endl;
    cout << "Lebar dan panjang matriks: " << matrix_width << " " << matrix_height << endl;
    cout << "Matriks: " << number_of_sequences << endl;
    cetakMatriks(matrix);
    cout << "Jumlah sekuens: " << number_of_sequences << endl;
    cetakSequences(number_of_sequences, objekSequences);

    vector<string> currentSequence;
    vector<listbuffer> list_of_buffer;
    vector<coordinat> seqCoordinat;

    for(int i = 0; i < matrix_width; i++){
        // cout << "check" << endl;
        int col = i;
        generateSequences(matrix, col, buffer_size, matrix_height, matrix_width, seqCoordinat, currentSequence, objekSequences, list_of_buffer);
    }
    // cout << list_of_buffer.size() << endl;
    // cetakListbuffer(list_of_buffer);
    int index;
    cariTerbesar(list_of_buffer, index);

    cout << endl;
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << endl;
    cout << duration.count() << " ms" << endl;

    cout << "Apakah Anda ingin menyimpan solusi? (y/n)" << endl;
    string ans;
    cin >> ans;
    if (ans == "y"){
        string folderPath = "../test/";
        string namaFileHasil;

        cout << "Tuliskan nama file: ";
        cin >> namaFileHasil;

        string fullPath = folderPath + namaFileHasil;
        ofstream file(fullPath);

        if (!file.is_open()) {
            cerr << "Gagal membuka file." << endl;
        }

        listbuffer& hasil = list_of_buffer[index];
        file << hasil.totalReward << endl;
        for (const string& token : hasil.seqbuffer) {
            file << token << " ";
        }
        file << endl;
        for (const coordinat& token : hasil.seqcoordinat) {
            file << token.x << ", " << token.y << endl;
        }
        file << endl;
        file << duration.count() << "ms" << endl;
        file.close();
    }
}

int main() {
    cout << "------------------------------------------" << endl;
    cout << "Selamat datang di cyberpunk-hacker (lite version)" << endl;
    cout << "Ingin memasukkan data melalui: " << endl;
    cout << "1. file txt" << endl;
    cout << "2. masukan dengan generate otomatis" << endl;
    cout << "Ketik nomor sesuai yang Anda inginkan!" << endl;
    int tipe_proses;
    cin >> tipe_proses;
    cout << "------------------------------------------" << endl;

    if (tipe_proses == 1){
        prosestxt();
    } else if (tipe_proses == 2){
        prosesotomatis();
    } else {
        cout << "Masukan salah!" << endl;
    }

    return 0;
}