#ifndef ___FILE
#define ___FILE


#include <fstream>
#include <string.h>
#include <bitset>
#include "../global.h"
class File{
    //! goal: produce a compressed file 
    //! input: a vector<vector<long>> of data(last one is frequency), 
    //! an encoding scheme(vector<long>), 
    //! enoding method

    //following needs use of strcpy to rid of \0

    //signature
    char FILE_SIG[3] = "LT"; //2byte

    //name length
    char* FILE_NAME_LENGTH = nullptr; //1byte

    //transform history length
    char* TRANSFORM_LENGTH = nullptr; //1byte

    //name of file
    char FILE_NAME[11] = "sample.dat"; //10byte

    //transofrm history
    char *TRASNFORM_ARR = nullptr; // inited by ctor

    //how many chars we should MATCH with data
    unsigned int COMP_CHAR_COUNT = 0;        // inited by ctor 

    //actual data char count, compare after decompress
    unsigned int FILE_BYTE_COUNT; // inited by ctor

    //huff trio length
    unsigned char DATA_TRIO_COUNT = 0;  // inited by ctor

    //huff trio
    unsigned char *HUFFTRIOS = nullptr; // inited by ctor

    //data file
    char *dataPtr = nullptr; //inited by ctor


    void initTransformArr(vector<Transformation> &Tseq)
    {
        unsigned int count = Tseq.size();
        char *X_NULL_RESULT = new char[count]; //create a NOT null terminated char arr
        for (size_t i = 0; i < count;i++)
        {
            X_NULL_RESULT[i] = Tseq[i];
        }

        TRASNFORM_ARR = X_NULL_RESULT;
        cout << "complete Transform Init" << endl;
    }
    void initHuffTrio(vector<long> encodeMapArr){
        unsigned int count = encodeMapArr.size();
        cout << "size:  " << count << endl;
        unsigned char *X_NULL_RESULT = new unsigned char[count];
        for (size_t i = 0; i < count;i++){
            if (encodeMapArr[i] > 255){
                throw Error("weird bug at initHuffTrio");
            }
            X_NULL_RESULT[i] = static_cast<unsigned char>(encodeMapArr[i]);
        }
        HUFFTRIOS = X_NULL_RESULT;
        DATA_TRIO_COUNT = count;
        cout << "complete HUff Trio" << endl;
    }
    void writeToFile(){
        // ofstream outfile;
        // outfile.open("junk.dat", ios::binary | ios::out);
        // char c[12] = "lt";
        // outfile.write(c, 2*sizeof(char));
        // outfile.close();


        ofstream outfile;
        outfile.open("test.dat", ios::binary | ios::out);
        outfile.write(FILE_SIG, 2 * sizeof(char));
        outfile.write(FILE_NAME_LENGTH, sizeof(char));
        outfile.write(TRANSFORM_LENGTH, sizeof(char));
        outfile.write(FILE_NAME, sizeof(char) * FILE_NAME_LENGTH[0]);
        outfile.write(TRASNFORM_ARR, sizeof(char) * TRANSFORM_LENGTH[0]);
        //! convert size ints to char * lol!

        outfile.close();
        cout << "write complete" << endl;
    }
    void flatten(vector<long>& flatData,vector<unique_ptr<Block>>& data){
        for(auto && ptr: data){
            for(const auto& n: ptr->getData()){
                flatData.push_back(n);
            }
        }
        data.clear(); //free up data
    }
    void initData(vector<unique_ptr<Block>>& data,const vector<long>&encodingLength){
        bitset<8> bytemimick;
        vector<long> flatData{};
        flatten(flatData, data);
        
        vector<char> result{};
        int capacity = 8;
        size_t size = encodingLength.size();
        COMP_CHAR_COUNT = encodingLength.size();
        size_t counter = 0;
        while (counter < size)
        {
            if (capacity >= encodingLength[counter]){ //fit case
                bitset<8> tmp(flatData[counter]); //to be copied
                capacity -= encodingLength[counter]; //update capacity 
                int start = capacity;     //starting bit of the mimick
                
                for (int i = 0; i < encodingLength[counter]; i++)
                {
                    bytemimick[start] = tmp[i];
                    start++;
                }
                counter++; //check next data;
                if (capacity == 0){
                    capacity = 8;
                    result.push_back(static_cast<char>( bytemimick.to_ulong()));
                    bytemimick.reset();
                }
            }else{ //length > capacity, so we copy as many as possible. 
                bitset<8> tmp(flatData[counter]); //to be copied
                int tp = encodingLength[counter] - capacity; //make tp the begining par tof the copy //4
                for (int i = 0; i < capacity;i++){ //copy some over to mimick untill mimick full //3
                    bytemimick[i] = tmp[tp];
                    tp++;
                }
                result.push_back(static_cast<char>( bytemimick.to_ulong()));
                bytemimick.reset();
                capacity = 8 - (encodingLength[counter] - capacity); //4
                tp = capacity;
                for (int i = 0; tp < 8; i++)
                {
                    bytemimick[tp] = tmp[i];
                    tp++;
                }
                counter++;
            }
        }
        char *X_NULL_RESULT = new char[result.size()];
        strncpy(X_NULL_RESULT, result.data(), result.size());
        dataPtr = X_NULL_RESULT;
        cout << "complete data init" << endl;
    }

public:
    File(vector<unique_ptr<Block> > &data, vector<long> encodeMapArr, vector<Transformation> Tseq,unsigned int originalSize):FILE_BYTE_COUNT(originalSize)
    {
        const vector<long> encodingLengthArr = std::move(data.back()->getData());
        int encodeCounter = 0;
        data.pop_back(); //pop out the encodinglength arr

        FILE_NAME_LENGTH = new char[1];
        FILE_NAME_LENGTH[0] = 10;

        TRANSFORM_LENGTH = new char[1];
        TRANSFORM_LENGTH[0] = Tseq.size(); //! not including huffman

        initTransformArr(Tseq);
        initHuffTrio(encodeMapArr);
        initData(data,encodingLengthArr);
        writeToFile();
    }
};
#endif