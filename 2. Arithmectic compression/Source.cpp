#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector> 
#include <cmath> 

using namespace std; 
typedef long long ll; 

// ����������� ��������� ��������
long long leadingDigitThreshold = pow(10, 9); 
long long underflowThreshold = pow(10, 8); 

// ������� ��� �������������� ������ � long long
ll stringToLongLong(string inputString) {
    ll result = 0; 
    ll weight = 1; 

    // ���� �� �������� ������ � �������� �������
    for (int i = inputString.length() - 1; i >= 0; i--) {
        result += (inputString[i] - '0') * weight; 
        weight *= 10; 
    }

    return result; 
}

// ������� ��� �����������
void encode() {
    string inputMessage = ""; 
    vector<int> frequencyTable; 
    int totalCharacters = 256; 

    ll underflowFlag = -1; 
    ll underflowCount = 0; 

    ll low = 0; 
    ll high = 9999999999; 

    ifstream inputFile("input.txt"); 
    string temp; 

    // ������ ��������� ��������� �� �����
    while (getline(inputFile, temp)) {
        if (inputMessage == "") {
            inputMessage = temp;
        }
        else {
            inputMessage += '\n' + temp;
        }
    }

    inputMessage += "#"; 

    // ������������� ������� ������
    for (int i = 0; i <= 255; i++) {
        frequencyTable.push_back(1);
    }

    ofstream outputFile("output.bin", ios::binary); 
    string encodedMessage = ""; 

    // ��������� ������� ������� � �������� ���������
    for (int i = 0; i < inputMessage.length(); i++) {
        int characterPosition = int(inputMessage[i]); 

        ll intervalRange = (high - low) + 1; 
        ll cumulativeFrequency = 0; 

        // ���������� ����������� ������� ��� �������� �������
        for (int k = 0; k < characterPosition; k++) {
            cumulativeFrequency += frequencyTable[k];
        }

        // ���������� ������ ���������
        high = low + intervalRange * (cumulativeFrequency + frequencyTable[characterPosition]) / (totalCharacters + 1) - 1;
        low = low + intervalRange * cumulativeFrequency / (totalCharacters + 1);

        totalCharacters++; 
        frequencyTable[characterPosition]++; 

        // �����, ���� ������� ������� low � high ���������
        while ((low / leadingDigitThreshold) == (high / leadingDigitThreshold)) {
            ll digit = low / leadingDigitThreshold; 
            encodedMessage = encodedMessage + char(low / leadingDigitThreshold + '0');
            low = (low % leadingDigitThreshold) * 10; 
            high = (high % leadingDigitThreshold) * 10 + 9; 

            char underflowDigit = '0'; 

            // ��������� ����� ��� underflow, ���� ����������
            if (digit == underflowFlag) {
                underflowDigit = '9';
            }

            // ���������� underflow �����
            while (underflowCount) {
                underflowCount--;
                encodedMessage = encodedMessage + underflowDigit;
            }

            underflowFlag = -1; 
        }

        // ��������� underflow
        while (high - low < underflowThreshold) {
            if (underflowFlag == -1) {
                underflowFlag = low / leadingDigitThreshold; 
            }

            underflowCount++; 

            int temp = low / leadingDigitThreshold; 

            // ����� low � high
            low = temp * leadingDigitThreshold + (low % underflowThreshold) * 10;
            temp = high / leadingDigitThreshold;
            high = temp * leadingDigitThreshold + (high % underflowThreshold) * 10 + 9;
        }
    }

    // ���������� ���������� ����
    ll finalCode = (low + high) / 2;
    string finalCodeString = to_string(finalCode);

    // ���������� �����, ���� ����� ���������� ���� ������ 10
    while (finalCodeString.length() < 10) {
        finalCodeString = finalCodeString + "0";
    }

    encodedMessage = encodedMessage + finalCodeString;

    // ������ � �������� ����
    for (char c : encodedMessage) {
        outputFile.write(&c, sizeof(char)); 
    }

    cout << "Encoding of the file was successful!" << endl; 
}

// ������� ��� �������������
void decode() {
    string decodedMessage = ""; 
    string encodedMessage = ""; 

    vector<int> frequencyTable; 
    int totalCharacters = 256; 

    ll underflowFlag = -1; 
    ll underflowCount = 0; 

    ll low = 0; 
    ll high = 9999999999; 

    ifstream encodedFile("output.bin", ios::binary); 
    char c; 

    // ������ ��������������� ��������� �� ��������� �����
    while (encodedFile.get(c)) {
        encodedMessage += c;
    }

    // ������������� ������� ������
    for (int i = 0; i <= 255; i++) {
        frequencyTable.push_back(1);
    }

    ll currentCode = stringToLongLong(encodedMessage.substr(0, 10)); 
    int currentPosition = 10; 
    int currentCharacter = 257; 

    // ������������� ���������
    while (currentCharacter != int('#')) {
        ll temp = low; 
        ll intervalRange = (high - low) + 1; 
        ll cumulativeFrequency = 0; 

        // ����� ������� �� �������� ����
        for (int k = 0; k < frequencyTable.size(); k++) {
            ll topHalf = temp + intervalRange * (cumulativeFrequency + frequencyTable[k]) / (totalCharacters + 1) - 1; 
            ll bottomHalf = temp + intervalRange * (cumulativeFrequency) / (totalCharacters + 1); 

            cumulativeFrequency += frequencyTable[k]; 

            // ���� ������� ��� ��������� � ���������, ��������� ������ � �������
            if (currentCode >= bottomHalf && currentCode < topHalf) {
                currentCharacter = k;
                low = bottomHalf;
                high = topHalf;

                totalCharacters++; 
                frequencyTable[currentCharacter]++; 

                break;
            }
        }

        // ���������� ������� � ��������������� ���������, ���� ��� �� ������ ����� ���������
        if (currentCharacter != int('#')) {
            decodedMessage = decodedMessage + char(currentCharacter);
        }

        // �����, ���� ������� ������� low � high ���������
        while ((low / leadingDigitThreshold) == (high / leadingDigitThreshold)) {
            encodedMessage = encodedMessage + char(low / leadingDigitThreshold + '0');
            low = (low % leadingDigitThreshold) * 10; 
            high = (high % leadingDigitThreshold) * 10 + 9; 
            currentCode = (currentCode % leadingDigitThreshold) * 10 + encodedMessage[currentPosition] - '0'; 

            currentPosition++; 
        }

        // ��������� underflow
        while (high - low < underflowThreshold) {
            ll temp = currentCode / leadingDigitThreshold; 
            currentCode = temp * leadingDigitThreshold + (currentCode % underflowThreshold) * 10 + encodedMessage[currentPosition] - '0';

            currentPosition++; 

            temp = low / leadingDigitThreshold; 
            low = temp * leadingDigitThreshold + (low % underflowThreshold) * 10; 
            temp = high / leadingDigitThreshold; 
            high = temp * leadingDigitThreshold + (high % underflowThreshold) * 10 + 9; // ����� high
        }
    }

    ofstream decodedFile("decodedText.txt");
    decodedFile << decodedMessage;

    cout << "The decoding of the file was successful!" << endl; 
}

int main() {
    encode(); 
    decode(); 
    system("pause"); 

    return 0;
}